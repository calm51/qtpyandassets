// Copyright (c) 2022, Riverbank Computing Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#include <stdio.h>

#include <Python.h>

#if PY_VERSION_HEX >= 0x030b0000
#include <pycore_import.h>
#endif

#include <QByteArray>
#include <QDir>
#include <QString>
#include <QRegExp>
#include <QTextCodec>
#include <QDebug>

#include "frozen_bootstrap.h"
#include "frozen_bootstrap_external.h"
#include "frozen_zipimport.h"
#include "frozen_iimp.h"

#if defined(PYQTDEPLOY_FROZEN_MAIN)
#include "frozen_main.h"
#endif


extern "C" PyObject *PyInit_pdytools(void);

// The internal API.
void pdytools_init_executable_dir(const QString &argv0);
const QDir &pdytools_get_executable_dir();

// We use Qt as the source of the locale information, partly because it
// officially supports Android.
static QTextCodec *locale_codec;

// Foward declarations.
static int handle_exception();
static int append_path_dirs(PyObject *list, const char **path_dirs);


//const struct _frozen *PyImport_FrozenModules;
#if PY_VERSION_HEX >= 0x030b0000
static const struct _frozen frozen_sentinel[] = {{NULL, NULL, 0, false, NULL}};
const struct _frozen *_PyImport_FrozenBootstrap = frozen_sentinel;
const struct _frozen *_PyImport_FrozenStdlib = frozen_sentinel;
const struct _frozen *_PyImport_FrozenTest = frozen_sentinel;

static const struct _module_alias aliases[] = {
{"_frozen_importlib", "importlib._bootstrap"},
{"_frozen_importlib_external", "importlib._bootstrap_external"},
{"os.path", "posixpath"},
{NULL, NULL}
};
const struct _module_alias *_PyImport_FrozenAliases = aliases;
#endif


// The replacement table of frozen modules.
static struct _frozen modules[] = {
{
    "_frozen_importlib",
    frozen_pyqtdeploy_bootstrap,
    sizeof (frozen_pyqtdeploy_bootstrap),
#if PY_VERSION_HEX >= 0x030b0000
    false,
    NULL,
#endif
},
{
    "_frozen_importlib_external",
    frozen_pyqtdeploy_bootstrap_external,
    sizeof (frozen_pyqtdeploy_bootstrap_external),
#if PY_VERSION_HEX >= 0x030b0000
    false,
    NULL,
#endif
},
{
    "iimp",
    frozen_pyqtdeploy_iimp,
    sizeof (frozen_pyqtdeploy_iimp),
#if PY_VERSION_HEX >= 0x030b0000
    false,
    NULL,
#endif
},
{
    "zipimport",
    frozen_pyqtdeploy_zipimport,
    sizeof (frozen_pyqtdeploy_zipimport),
#if PY_VERSION_HEX >= 0x030b0000
    false,
    NULL,
#endif
},
#if defined(PYQTDEPLOY_FROZEN_MAIN)
{
    "__main__",
    frozen_pyqtdeploy_main,
    sizeof (frozen_pyqtdeploy_main),
#if PY_VERSION_HEX >= 0x030b0000
    false,
    NULL,
#endif
},
#endif
{NULL, NULL, 0}
};


#if defined(Q_OS_WIN)
int pyqtdeploy_start(int argc, wchar_t **w_argv,
                     struct _inittab *extension_modules, const char *main_module,
                     const char *entry_point, const char **path_dirs)
#else
int pyqtdeploy_start(int argc, char **argv,
                     struct _inittab *extension_modules, const char *main_module,
                     const char *entry_point, const char **path_dirs)
#endif
{
    // Get the codec for the locale.
    locale_codec = QTextCodec::codecForLocale();
    if (!locale_codec){
#if defined(Q_OS_WIN)
        fwprintf(stderr, L"%s: no locale codec found\n", w_argv[0]);
#else
        fprintf(stderr, "%s: no locale codec found\n", argv[0]);
#endif
        return 1;
    }
    // Initialise some Python globals.
    Py_FrozenFlag = 1;
    Py_NoSiteFlag = 1;
    Py_IgnoreEnvironmentFlag = 1;
#if defined(PYQTDEPLOY_OPTIMIZED)
    Py_OptimizeFlag = 1;
#endif
    //  qDebug()<<"Py_FileSystemDefaultEncoding: "<<(Py_FileSystemDefaultEncoding?"ok":"none")<<"name: "<<locale_codec->name();
    if (!Py_FileSystemDefaultEncoding){
        // Python doesn't have a platform default so get it from Qt.  However
        // if Qt isn't specific then let Python have a go later.
        static QByteArray locale_codec_name;
        locale_codec_name = locale_codec->name();
        if (locale_codec_name != "System"){
            Py_FileSystemDefaultEncoding = locale_codec_name.data();
            Py_HasFileSystemDefaultEncoding = 1;
        }
    }

    PyImport_FrozenModules = modules;

    // Add the importer to the table of builtins.
    if (PyImport_AppendInittab("pdytools", PyInit_pdytools) < 0){
#if defined(Q_OS_WIN)
        fwprintf(stderr, L"%s: PyImport_AppendInittab() failed\n", w_argv[0]);
#else
        fprintf(stderr, "%s: PyImport_AppendInittab() failed\n", argv[0]);
#endif
        return 1;
    }

    // Add any extension modules.
    if (extension_modules != NULL)
        if (PyImport_ExtendInittab(extension_modules) < 0){
#if defined(Q_OS_WIN)
            fwprintf(stderr, L"%s: PyImport_ExtendInittab() failed\n", w_argv[0]);
#else
            fprintf(stderr, "%s: PyImport_ExtendInittab() failed\n", argv[0]);
#endif
            return 1;
        }

#if !defined(Q_OS_WIN)
    // Convert the argument list to wide characters using the locale codec.
    wchar_t **w_argv = new wchar_t *[argc + 1];
    for (int i = 0; i < argc; i++){
        QString qs_arg = locale_codec->toUnicode(argv[i]);
        wchar_t *w_arg = new wchar_t[qs_arg.length() + 1];
        w_arg[qs_arg.toWCharArray(w_arg)] = 0;
        w_argv[i] = w_arg;
    }
    w_argv[argc] = NULL;
#endif
    // Initialise the Python v3 interpreter.
    Py_SetProgramName(w_argv[0]);

    // if (QString::fromWCharArray(Py_GetProgramName()).count()==0){ Py_SetProgramName(QString("python3").toStdWString().c_str()); }

    //    _PyCoreConfig config = _PyCoreConfig_INIT;
    //    _PyInitError status = _Py_InitializeFromConfig(&config);
    //    qDebug()<<status.user_err<<QString(QByteArray(status.msg))<<QString(QByteArray(status.prefix));
    //    //    if (status != PY_SUCCESS) {
    //    //        fprintf(stderr, "Failed to initialize Python interpreter\n");
    //    //        qDebug()<<22222222222222;
    //    //        return 1;
    //    //    }

    //    PyStatus status;
    //    PyConfig config;
    //    PyConfig_InitPythonConfig(&config);
    //    config.isolated = 1;
    //    status = PyConfig_SetBytesArgv(&config, argc, argv);
    //    if (PyStatus_Exception(status)) {qDebug()<<"PyStatus_Exception(status)";}
    //    status = Py_InitializeFromConfig(&config);
    //    if (PyStatus_Exception(status)) {qDebug()<<"PyStatus_Exception(status)";}
    //    PyObject *ptype, *pvalue, *ptraceback;
    //    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    //    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
    //    PyObject *pStrErrorMessage = PyObject_Str(pvalue);
    //    const char *errorMessage = PyUnicode_AsUTF8(pStrErrorMessage);
    //    qDebug()<<errorMessage;
    //    Py_DECREF(ptype);
    //    Py_DECREF(pvalue);
    //    Py_XDECREF(ptraceback);
    //    Py_DECREF(pStrErrorMessage);

    return 0;

    Py_Initialize();
    PySys_SetArgvEx(argc, w_argv, 0);

    // Set sys.frozen.
    if (PySys_SetObject("frozen", Py_True) < 0)
        return handle_exception();

    // Initialise the directory containing the executable.
    pdytools_init_executable_dir(QString::fromWCharArray(w_argv[0]));

    // Configure sys.path.
    if (path_dirs != NULL)
    {
        PyObject *py_path = PySys_GetObject("path");

        if (py_path)
        {
            if (append_path_dirs(py_path, path_dirs) < 0)
                return handle_exception();
        }
    }

#if defined(PYQTDEPLOY_FROZEN_MAIN)
    Q_UNUSED(entry_point)

    // Set the __file__ attribute of the main module.
    PyObject *mod, *mod_dict, *py_filename;

    if ((mod = PyImport_AddModule(main_module)) == NULL)
        return handle_exception();

    mod_dict = PyModule_GetDict(mod);

    py_filename = PyUnicode_FromString(":/__main__.pyo");
    if (py_filename == NULL)
        return handle_exception();

    if (PyDict_SetItemString(mod_dict, "__file__", py_filename) < 0)
        return handle_exception();

    Py_DECREF(py_filename);

    // Import the main module.
    if (PyImport_ImportFrozenModule(main_module) < 0)
        return handle_exception();
#else
    // Import the main module.
    PyObject *mod, *main_module_obj;

    main_module_obj = PyUnicode_FromString(main_module);
    if (main_module_obj == NULL)
        return handle_exception();

    mod = PyImport_Import(main_module_obj);
    if (!mod)
        return handle_exception();

    // Call the entry point.
    if (!PyObject_CallMethod(mod, entry_point, NULL))
        return handle_exception();
#endif

    // Tidy up.
    Py_Finalize();

    return 0;
}


// Handle an exception and return the error code to immediately pass back to
// the operating system.
static int handle_exception()
{
    int exit_code;

    if (PyErr_ExceptionMatches(PyExc_SystemExit))
    {
        PyObject *exc, *value, *tb;

        PyErr_Fetch(&exc, &value, &tb);
        PyErr_NormalizeException(&exc, &value, &tb);

        if (!value || value == Py_None)
        {
            exit_code = 0;
        }
        else
        {
            PyObject *code = PyObject_GetAttrString(value, "code");

            if (code)
            {
                if (code == Py_None)
                {
                    exit_code = 0;
                    Py_DECREF(code);
                }
                else if (PyLong_Check(code))
                {
                    exit_code = (int)PyLong_AsLong(code);
                    Py_DECREF(code);
                }
                else
                {
                    exit_code = 1;
                }
            }
            else
            {
                exit_code = 1;
            }
        }

        PyErr_Restore(exc, value, tb);
        PyErr_Clear();
    }
    else
    {
        PyErr_Print();
        exit_code = 1;
    }

    Py_Finalize();

    return exit_code;
}


// Extend a list with an array of UTF-8 encoded path directory names.  Return
// -1 if there was an error.
static int append_path_dirs(PyObject *list, const char **path_dirs)
{
    const char *path_dir_utf8;

    while ((path_dir_utf8 = *path_dirs++) != NULL)
    {
        // Convert to a QString.
        QString path_dir(QString::fromUtf8(path_dir_utf8));

        // Expand any (locale encoded) environment variables.
        QRegExp env_var_name_re("\\$([A-Za-z0-9_]+)");

        int i;

        while ((i = env_var_name_re.indexIn(path_dir)) != -1)
        {
            QByteArray name(locale_codec->fromUnicode(env_var_name_re.cap(1)));
            QByteArray value(qgetenv(name.data()));

            path_dir.replace(i, env_var_name_re.matchedLength(),
                             locale_codec->toUnicode(value));
        }

        // Make sure the path is absolute.
        if (QDir::isRelativePath(path_dir))
        {
            const QDir &exec_dir = pdytools_get_executable_dir();
            path_dir = exec_dir.filePath(path_dir);
        }

        // Convert to the native format unless it is a resource path.  (Note
        // that we don't resolve symbolic links.)
        path_dir = QDir::cleanPath(path_dir);
        if (!path_dir.startsWith(QChar(':')))
            path_dir = QDir::toNativeSeparators(path_dir);

        // Convert to a Python string.
        PyObject *py_path_dir;

        QByteArray utf8(path_dir.toUtf8());
        py_path_dir = PyUnicode_FromStringAndSize(utf8.data(), utf8.length());

        if (!py_path_dir)
            return -1;

        // Append to the list.
        int rc = PyList_Append(list, py_path_dir);
        Py_DECREF(py_path_dir);

        if (rc < 0)
            return -1;
    }

    return 0;
}


// Note that we don't support deepfrozen modules (ie. static declarations of
// the corresponding Python objects).
#if PY_VERSION_HEX >= 0x030b0000
extern "C" int _Py_Deepfreeze_Init(void)
{
    return 0;
}

extern "C" void _Py_Deepfreeze_Fini(void)
{
}
#endif
