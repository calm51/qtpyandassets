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


#include <Python.h>
#include <marshal.h>
#include <structmember.h>

#include <QByteArray>
#include <QChar>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QResource>

#include "pyqtdeploy_version.h"


#if PY_VERSION_HEX < 0x03070000
#error "Python v3.7 or later is required"
#endif


extern "C" {

// The module definition structure.
static struct PyModuleDef pdytoolsmodule = {
    PyModuleDef_HEAD_INIT,
    "pdytools",
    NULL,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};


#if defined(Q_OS_WIN)
static const char extension_module_extension[] = ".pyd";
#else
static const char extension_module_extension[] = ".so";
#endif


// C linkage forward declarations.
PyObject *PyInit_pdytools();

static int qrcimporter_init(PyObject *self, PyObject *args, PyObject *kwds);
static void qrcimporter_dealloc(PyObject *self);
static PyObject *qrcimporter_find_loader(PyObject *self, PyObject *args);
static PyObject *qrcimporter_find_module(PyObject *self, PyObject *args);
static PyObject *qrcimporter_get_code(PyObject *self, PyObject *args);
static PyObject *qrcimporter_get_data(PyObject *self, PyObject *args);
static PyObject *qrcimporter_get_resource_reader(PyObject *self,
                                                 PyObject *arg);
static PyObject *qrcimporter_get_source(PyObject *self, PyObject *args);
static PyObject *qrcimporter_is_package(PyObject *self, PyObject *args);
static PyObject *qrcimporter_load_module(PyObject *self, PyObject *args);


// The importer object structure.
typedef struct _qrcimporter
{
    PyObject_HEAD

    // The path that the importer handles.  It will be the name of a directory.
    QString *path;

    // The component parts of the path.
    QStringList *path_parts;

    bool *is_assets; bool *is_qrc;
    bool *is_zip;
    QString *invalid_part;

} QrcImporter;


// The importer method table.
static PyMethodDef qrcimporter_methods[] = {
    {"find_loader", qrcimporter_find_loader, METH_VARARGS, NULL},
    {"find_module", qrcimporter_find_module, METH_VARARGS, NULL},
    {"get_code", qrcimporter_get_code, METH_VARARGS, NULL},
    {"get_data", qrcimporter_get_data, METH_VARARGS, NULL},
    {"get_resource_reader", qrcimporter_get_resource_reader, METH_O, NULL},
    {"get_source", qrcimporter_get_source, METH_VARARGS, NULL},
    {"is_package", qrcimporter_is_package, METH_VARARGS, NULL},
    {"load_module", qrcimporter_load_module, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};


// The importer type structure.
static PyTypeObject QrcImporter_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pdytools.qrcimporter",
    sizeof (QrcImporter),
    0,                                          // tp_itemsize
    qrcimporter_dealloc,                        // tp_dealloc
    0,                                          // tp_print
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_reserved
    0,                                          // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_mapping
    0,                                          // tp_hash
    0,                                          // tp_call
    0,                                          // tp_str
    0,                                          // tp_getattro
    0,                                          // tp_setattro
    0,                                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                         // tp_flags
    0,                                          // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    qrcimporter_methods,                        // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    qrcimporter_init,                           // tp_init
    0,                                          // tp_alloc
    0,                                          // tp_new
    0,                                          // tp_free
    0,                                          // tp_is_gc
    0,                                          // tp_bases
    0,                                          // tp_mro
    0,                                          // tp_cache
    0,                                          // tp_subclasses
    0,                                          // tp_weaklist
    0,                                          // tp_del
    0,                                          // tp_version_tag
    0,                                          // tp_finalize
};


// The reader object structure.
typedef struct _qrcreader
{
    PyObject_HEAD

    // The pathname containing the resources handled by this reader.
    QString *pathname;
} QrcReader;


// The reader method declarations.
static int qrcreader_init(PyObject *self, PyObject *args, PyObject *kwds);
static void qrcreader_dealloc(PyObject *self);
static PyObject *qrcreader_contents(PyObject *self, PyObject *);
static PyObject *qrcreader_is_resource(PyObject *self, PyObject *args);
static PyObject *qrcreader_open_resource(PyObject *self, PyObject *arg);
static PyObject *qrcreader_resource_path(PyObject *self, PyObject *arg);


// The reader method table.
static PyMethodDef qrcreader_methods[] = {
    {"contents", qrcreader_contents, METH_NOARGS, NULL},
    {"is_resource", qrcreader_is_resource, METH_VARARGS, NULL},
    {"open_resource", qrcreader_open_resource, METH_O, NULL},
    {"resource_path", qrcreader_resource_path, METH_O, NULL},
    {NULL, NULL, 0, NULL}
};


// The reader type structure.
static PyTypeObject QrcReader_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pdytools.qrcreader",
    sizeof (QrcReader),
    0,                                          // tp_itemsize
    qrcreader_dealloc,                          // tp_dealloc
    0,                                          // tp_print
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_reserved
    0,                                          // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_mapping
    0,                                          // tp_hash
    0,                                          // tp_call
    0,                                          // tp_str
    0,                                          // tp_getattro
    0,                                          // tp_setattro
    0,                                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                         // tp_flags
    0,                                          // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    qrcreader_methods,                          // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    qrcreader_init,                             // tp_init
    0,                                          // tp_alloc
    0,                                          // tp_new
    0,                                          // tp_free
    0,                                          // tp_is_gc
    0,                                          // tp_bases
    0,                                          // tp_mro
    0,                                          // tp_cache
    0,                                          // tp_subclasses
    0,                                          // tp_weaklist
    0,                                          // tp_del
    0,                                          // tp_version_tag
    0,                                          // tp_finalize
};


// The resource object structure.
typedef struct _qrcresource
{
    PyObject_HEAD

    // The resource itself.
    QFile *resource;
} QrcResource;


// The resource method declarations.
static int qrcresource_init(PyObject *self, PyObject *args, PyObject *kwds);
static void qrcresource_dealloc(PyObject *self);
static PyObject *qrcresource_close(PyObject *self, PyObject *);
static PyObject *qrcresource_flush(PyObject *self, PyObject *);
static PyObject *qrcresource_read(PyObject *self, PyObject *args);
static PyObject *qrcresource_readable(PyObject *self, PyObject *);
static PyObject *qrcresource_seekable(PyObject *self, PyObject *);
static PyObject *qrcresource_writable(PyObject *self, PyObject *);


// The resource method table.
static PyMethodDef qrcresource_methods[] = {
    {"close", qrcresource_close, METH_NOARGS, NULL},
    {"flush", qrcresource_flush, METH_NOARGS, NULL},
    {"read", qrcresource_read, METH_VARARGS, NULL},
    {"readable", qrcresource_readable, METH_NOARGS, NULL},
    {"seekable", qrcresource_seekable, METH_NOARGS, NULL},
    {"writable", qrcresource_writable, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};


// The resource get/set declarations.
static PyObject *qrcresource_get_closed(PyObject *self, void *);


// The resource get/set table.
static PyGetSetDef qrcresource_getset[] = {
    {"closed", qrcresource_get_closed, NULL, NULL},
    {NULL, NULL, NULL, NULL}
};


// The resource type structure.
static PyTypeObject QrcResource_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pdytools.qrcresource",
    sizeof (QrcResource),
    0,                                          // tp_itemsize
    qrcresource_dealloc,                        // tp_dealloc
    0,                                          // tp_print
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_reserved
    0,                                          // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_mapping
    0,                                          // tp_hash
    0,                                          // tp_call
    0,                                          // tp_str
    0,                                          // tp_getattro
    0,                                          // tp_setattro
    0,                                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                         // tp_flags
    0,                                          // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    qrcresource_methods,                        // tp_methods
    0,                                          // tp_members
    qrcresource_getset,                         // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    qrcresource_init,                           // tp_init
    0,                                          // tp_alloc
    0,                                          // tp_new
    0,                                          // tp_free
    0,                                          // tp_is_gc
    0,                                          // tp_bases
    0,                                          // tp_mro
    0,                                          // tp_cache
    0,                                          // tp_subclasses
    0,                                          // tp_weaklist
    0,                                          // tp_del
    0,                                          // tp_version_tag
    0,                                          // tp_finalize
};

}


// The different results that can be returned by find_module().
enum ModuleType {
    ModuleNotFound,
    ModuleIsModule,
    ModuleIsPackage,
    ModuleIsNamespace,
    ModuleIsZip,
    ModuleIsAdjacentExtensionModule
};


// The internal API.
void pdytools_init_executable_dir(const QString &argv0);
const QDir &pdytools_get_executable_dir();


// Other forward declarations.
static ModuleType find_module(QrcImporter *self, const QString &fqmn,
                              QString &pathname, QString &filename);
static bool read_data(const QString &filename, QByteArray &data);
static PyObject *get_code_object(const QString &filename);
static void raise_import_error(const QString &fqmn);
static QString str_to_qstring(PyObject *str);
static PyObject *qstring_to_str(const QString &qstring);
static bool parse_qstring(PyObject *args, const char *fmt, QString &qstring,
                          PyObject **str_obj = 0);
static QString get_resource_path(QrcReader *reader, const QString &resource);


// The directory containing the application executable.
static QDir *executable_dir = 0;

QPair<QString,QString> checkValidPath(QString fullPath) {
    QPair<QString,QString> result;
    QStringList invalidPart;
    QString _p;
    while (!QFileInfo::exists(fullPath)) {
        invalidPart.append(QFileInfo(fullPath).fileName());
        fullPath = QFileInfo(fullPath).absolutePath();
#ifdef QTPYANDASSETS_DEBUG
        //        qDebug()<<fullPath<<invalidPart;
#endif
        if (fullPath == _p) {
            break;
        }
        _p=fullPath;
    }
    result.first = fullPath;
    if (invalidPart.count()>0){
        std::reverse(invalidPart.begin(), invalidPart.end());
        result.second = "/"+invalidPart.join("/");
    }
    return result;
}
QString getPythonErrorString() {
    // Fetch the current exception
    PyObject *exc_type, *exc_value, *traceback;
    PyErr_Fetch(&exc_type, &exc_value, &traceback);

    // Normalize the exception
    PyErr_NormalizeException(&exc_type, &exc_value, &traceback);

    // Convert the exception to a string
    PyObject *exc_string = PyObject_Str(exc_value);
    if (!exc_string) {
        PyErr_Clear();
        return QString("Failed to convert exception to string");
    }

    // Convert the Python string to a QString
    const char *cstr_exc_string = PyUnicode_AsUTF8(exc_string);
    QString errorString = QString::fromUtf8(cstr_exc_string);

    // Clean up
    Py_XDECREF(exc_type);
    Py_XDECREF(exc_value);
    Py_XDECREF(traceback);
    Py_DECREF(exc_string);

    return errorString;
}
// The importer initialisation function.
static int qrcimporter_init(PyObject *self, PyObject *args, PyObject *kwds)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_init";
#endif
    // It's not clear if this is part of the public API.
    if (!_PyArg_NoKeywords("qrcimporter()", kwds))
        return -1;

    PyObject *path;

    if (!PyArg_ParseTuple(args, "O&:qrcimporter", PyUnicode_FSDecoder, &path))
        return -1;

    QString *q_path = new QString(str_to_qstring(path));

    ((QrcImporter *)self)->is_assets = new bool(q_path->startsWith(QString("assets:/")));
    ((QrcImporter *)self)->is_qrc = new bool(q_path->startsWith(QString(":/")));
    //#ifdef QTPYANDASSETS_DEBUG
    //    qDebug()<<"q_path"<<*q_path;
    //#endif
    //    if (*((QrcImporter *)self)->is_qrc || *((QrcImporter *)self)->is_assets){
    //        *q_path = q_path->mid(1);
    //    }

    //    if (!*((QrcImporter *)self)->is_qrc && !*((QrcImporter *)self)->is_assets){
    //        delete q_path;
    //        PyErr_SetString(PyExc_ImportError, "qrcimporter: not a qrc file");
    //        return -1;
    //    }

    ((QrcImporter *)self)->invalid_part = new QString();
    if (!QFileInfo(*q_path).exists()){
        const auto &_ = checkValidPath(*q_path);
#ifdef QTPYANDASSETS_DEBUG
        qDebug()<<"checkValidPath"<<_;
#endif
        *q_path = _.first;
        *((QrcImporter *)self)->invalid_part = _.second;
        if (_.second.startsWith("/assets:/") || _.second.startsWith("/:/")){
            *((QrcImporter *)self)->invalid_part = "";
        }
    }
    QFileInfo _fl(*q_path);
    ((QrcImporter *)self)->is_zip = new bool(_fl.isFile()&&q_path->endsWith(QString(".zip")));
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"q_path"<<*q_path<<"is_zip:"<< *((QrcImporter *)self)->is_zip<<"is_dir:"<<_fl.isDir();
#endif

    if (!_fl.isDir() && !*((QrcImporter *)self)->is_zip){
        delete q_path;

        PyErr_SetString(PyExc_ImportError, "qrcimporter: not a qrc file");
        return -1;
    }

    if (!q_path->endsWith(QChar('/')) &&!*((QrcImporter *)self)->is_zip)
        q_path->append(QChar('/'));

    ((QrcImporter *)self)->path = q_path;

    int _start_index = 0;
    if (*((QrcImporter *)self)->is_assets){
        _start_index = 8;
    }else if (*((QrcImporter *)self)->is_qrc){
        _start_index = 2;
    }
    if (*((QrcImporter *)self)->is_zip){
        ((QrcImporter *)self)->path_parts = new QStringList(
                    q_path->mid(_start_index, q_path->length() -
                                (*((QrcImporter *)self)->is_assets?2:2)).split(QChar('/'),
                                                                               QString::SkipEmptyParts));
    }else{
        ((QrcImporter *)self)->path_parts = new QStringList(
                    q_path->mid(_start_index, q_path->length() -
                                (*((QrcImporter *)self)->is_assets?3:3)).split(QChar('/'),
                                                                               QString::SkipEmptyParts));
    }

#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"self.path_parts"<<*(((QrcImporter *)self)->path_parts);
#endif
    return 0;
}


// The importer deallocation function.
static void qrcimporter_dealloc(PyObject *self)
{
    if (((QrcImporter *)self)->path)
    {
        delete ((QrcImporter *)self)->path;
        ((QrcImporter *)self)->path = 0;
    }

    if (((QrcImporter *)self)->path_parts)
    {
        delete ((QrcImporter *)self)->path_parts;
        ((QrcImporter *)self)->path_parts = 0;
    }

    Py_TYPE(self)->tp_free(self);
}


// Implement the standard find_loader() method for the importer.
static PyObject *qrcimporter_find_loader(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_find_loader";
#endif
    QString fqmn;

    if (!parse_qstring(args, "U:qrcimporter.find_loader", fqmn))
        return NULL;

    QString pathname, filename;
    PyObject *result;

    switch (find_module((QrcImporter *)self, fqmn, pathname, filename))
    {
    case ModuleIsModule:
    case ModuleIsPackage:
    case ModuleIsAdjacentExtensionModule:
        result = Py_BuildValue("O[]", self);
        break;

    case ModuleIsNamespace:
    {
        PyObject *py_pathname = qstring_to_str(pathname);
        if (!py_pathname)
            return NULL;

        result = Py_BuildValue("O[N]", Py_None, py_pathname);
        break;
    }
    case ModuleIsZip:
    {
        //        QResource resource(*((QrcImporter *)self)->path);
        //        if (!resource.isValid()) {
        //            PyErr_SetString(PyExc_IOError, "Failed to load zip file into memory");
        //            return NULL;
        //        }
        //        QByteArray zipData(reinterpret_cast<const char*>(resource.data()), resource.size());
        QFile zipFile(*((QrcImporter *)self)->path);
        if (!zipFile.open(QIODevice::ReadOnly)) {
            PyErr_SetString(PyExc_IOError, "Failed to open ZIP file");
            return NULL;
        }
        QByteArray zipData = zipFile.readAll();
        zipFile.close();
        PyObject* py_zip_data = PyBytes_FromStringAndSize(zipData.constData(), zipData.size());
        if (!py_zip_data)
            return NULL;
        //        qDebug()<<111<<PyBytes_Size(py_zip_data);

        PyObject *zipimport_module = PyImport_ImportModule("zipimport");
        if (!zipimport_module) {
            Py_DECREF(py_zip_data);
            return NULL;
        }

        PyObject *zipimporter_class = PyObject_GetAttrString(zipimport_module, "zipimporter");
        Py_DECREF(zipimport_module);
        if (!zipimporter_class) {
            Py_DECREF(py_zip_data);
            return NULL;
        }
#ifdef QTPYANDASSETS_DEBUG
        qDebug()<<"path,invalid_part"<<*((QrcImporter *)self)->path<<*((QrcImporter *)self)->invalid_part;
#endif
        PyObject *str_path = qstring_to_str(*((QrcImporter *)self)->path);
        if (!str_path) {
            qDebug()<<"error: "<<getPythonErrorString();
            PyErr_Print();
            Py_DECREF(zipimporter_class);
            return NULL;
        }
        PyObject *str_path__invalid_part = qstring_to_str(*((QrcImporter *)self)->invalid_part);
        if (!str_path__invalid_part) {
            qDebug()<<"error: "<<getPythonErrorString();
            PyErr_Print();
            Py_DECREF(zipimporter_class);
            return NULL;
        }

        PyObject *args_tuple = PyTuple_New(3);
        if (!args_tuple) {
            qDebug()<<"error: "<<getPythonErrorString();
            Py_DECREF(zipimporter_class);
            return NULL;
        }
        PyTuple_SetItem(args_tuple, 0, str_path);
        PyTuple_SetItem(args_tuple, 1, py_zip_data);
        PyTuple_SetItem(args_tuple, 2, str_path__invalid_part);


        PyObject *zipimporter_obj = PyObject_CallObject(zipimporter_class, args_tuple);
        if (!zipimporter_obj) {
            qDebug()<<"error: "<<getPythonErrorString();
            PyErr_Print();
            Py_DECREF(zipimporter_class);
            return NULL;
        }

        Py_DECREF(args_tuple);
        PyObject *args_tuple2 = PyTuple_New(2);
        if (!args_tuple2) {
            Py_DECREF(zipimporter_class);
            return NULL;
        }
        PyObject *str_path2 = qstring_to_str(fqmn);
        if (!str_path2) {
            qDebug()<<"error: "<<getPythonErrorString();
            PyErr_Print();
            Py_DECREF(zipimporter_class);
            return NULL;
        }
        PyTuple_SetItem(args_tuple2, 0, str_path2);
        PyTuple_SetItem(args_tuple2, 1, str_path2);

        PyObject *loader_method = PyObject_GetAttrString(zipimporter_obj, "find_loader");
        if (!loader_method || !PyCallable_Check(loader_method)) {
            return NULL;
        }
        PyObject *loader = PyObject_CallObject(loader_method, args_tuple2);
        if (!loader) {
            return NULL;
        }
        return loader;
        break;
    }
    case ModuleNotFound:
    {
        static bool recursing = false;

        // If we have failed to find a sub-package then it may be because
        // it is a builtin so start a high-level search for it while
        // watching for recursing back here.
        if (fqmn.contains(QChar('.')) && !recursing)
        {
            static PyObject *find_loader = 0;

            if (!find_loader)
            {
                PyObject *importlib = PyImport_ImportModule("importlib");

                if (!importlib)
                    return NULL;

                find_loader = PyObject_GetAttrString(importlib,
                                                     "find_loader");

                Py_DECREF(importlib);

                if (!find_loader)
                    return NULL;
            }

            recursing = true;
            PyObject *loader = PyObject_CallObject(find_loader, args);
            recursing = false;

            if (!loader)
                return NULL;

            result = Py_BuildValue("N[]", loader);
        }
        else
        {
            result = Py_BuildValue("O[]", Py_None);
        }
    }

        break;
    }

    return result;
}


// Implement the standard find_module() method for the importer.  Note that we
// implement this for Python v3 to support things (like pkg_resources) that are
// still using the old API.
static PyObject *qrcimporter_find_module(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_find_module";
#endif
    PyObject *py_fqmn, *path;

    if (!PyArg_ParseTuple(args, "U|O:qrcimporter.find_module", &py_fqmn, &path))
        return NULL;

    QString fqmn = str_to_qstring(py_fqmn);
    QString pathname, filename;
    PyObject *result;

    if (find_module((QrcImporter *)self, fqmn, pathname, filename) == ModuleNotFound)
    {
        result = Py_None;

        // If we have failed to find a sub-package then it may be because it is
        // a builtin.
        if (fqmn.contains(QChar('.')))
            for (struct _inittab *p = PyImport_Inittab; p->name; ++p)
                if (fqmn == p->name)
                {
                    result = self;
                    break;
                }
    }
    else
    {
        result = self;
    }

    Py_INCREF(result);
    return result;
}

#include <QCoreApplication>

// Implement the standard load_module() method for the importer.
static PyObject *qrcimporter_load_module(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_load_module";
#endif
    QString fqmn;
    PyObject *py_fqmn;

    if (!parse_qstring(args, "U:qrcimporter.load_module", fqmn, &py_fqmn))
        return NULL;

    QString pathname, filename;
    PyObject *code, *py_filename, *mod_dict;

    ModuleType mt = find_module((QrcImporter *)self, fqmn, pathname, filename);

    if (mt == ModuleIsAdjacentExtensionModule)
    {
        // We use the imp module to load sub-packages that are dynamically
        // linked extension modules installed in the same directory as the
        // executable.
        static PyObject *load_module = NULL;
        static PyObject *open_file = NULL;

        if (!load_module)
        {
            PyObject *imp_module = PyImport_ImportModule("iimp");
            if (!imp_module)
                return NULL;

            load_module = PyObject_GetAttrString(imp_module, "load_module");
            Py_DECREF(imp_module);

            if (!load_module)
                return NULL;
        }

        if (!open_file)
        {
            PyObject *builtins = PyEval_GetBuiltins();
            if (!builtins)
                return NULL;

            open_file = PyDict_GetItemString(builtins, "open");
            if (!open_file)
                return NULL;
        }

        py_filename = qstring_to_str(filename);
        if (!py_filename)
            return NULL;

        //        PyObject *module_file = PyLong_FromLong(88);
        PyObject *module_file = Py_None;
        //        PyObject *module_file = PyObject_CallFunction(open_file, "Os",
        //                                                      py_filename, "rb");
        //        if (!module_file)
        //        {
        //            Py_DECREF(py_filename);
        //            return NULL;
        //        }


        //        QFile soFile1(filename);
        //        QFile soFile2(QDir::current().absoluteFilePath(QFileInfo(filename).fileName()));
        //        qDebug()<<"ModuleIsAdjacentExtensionModule"<<soFile1.fileName()<<soFile2.fileName();
        //        if (!soFile2.exists()){
        //            if (!soFile1.open(QIODevice::ReadOnly)) {
        //                PyErr_SetString(PyExc_IOError, "Failed to open .so file1");
        //                return NULL;
        //            }
        //            if (!soFile2.open(QIODevice::WriteOnly)) {
        //                PyErr_SetString(PyExc_IOError, "Failed to open .so file2");
        //                soFile1.close();
        //                return NULL;
        //            }
        //            soFile2.write(soFile1.readAll());
        //            soFile1.close();soFile2.close();
        //        }
        //        py_filename = qstring_to_str(soFile2.fileName());
        //        if (!py_filename)
        //            return NULL;
        //        QString linkName = QDir(QCoreApplication::libraryPaths().at(0)).absoluteFilePath("libpython3.8.so");
        //        QString fileName = QDir::current().absoluteFilePath("libpython3.8.so.1.0");
        //        QFile::remove(fileName);
        //        if (QFile::copy(linkName, fileName)) { // link copy
        //            qDebug() << "Soft link created successfully!";
        //        } else {
        //            qDebug() << "Failed to create soft link!";
        //        }


        PyObject *module = PyObject_CallFunction(load_module, "OOO(ssi)",
                                                 py_fqmn, module_file, py_filename, extension_module_extension,
                                                 "rb", 3);
#ifdef QTPYANDASSETS_DEBUG
        qDebug()<<"ModuleIsAdjacentExtensionModule ok";
#endif
        Py_DECREF(module_file);
        Py_DECREF(py_filename);

        return module;
    }

    if (mt != ModuleIsModule && mt != ModuleIsPackage)
    {
        raise_import_error(fqmn);
        return NULL;
    }

    // Read in the code object from the file.
    code = get_code_object(filename);
    if (!code)
        return NULL;

    // Get the module object and its dict.
    PyObject *mod = PyImport_AddModuleObject(py_fqmn);
    if (!mod)
        goto error;

    mod_dict = PyModule_GetDict(mod);

    // Set the loader object.
    if (PyDict_SetItemString(mod_dict, "__loader__", self) != 0)
        goto error;

    if (mt == ModuleIsPackage)
    {
        // Add __path__ to the module before the code gets executed.

        PyObject *py_pathname = qstring_to_str(pathname);
        if (!py_pathname)
            goto error;

        PyObject *path_list = Py_BuildValue("[N]", py_pathname);
        if (!path_list)
            goto error;

        int rc = PyDict_SetItemString(mod_dict, "__path__", path_list);
        Py_DECREF(path_list);

        if (rc != 0)
            goto error;
    }

    py_filename = qstring_to_str(filename);
    if (!py_filename)
        goto error;

    mod = PyImport_ExecCodeModuleObject(py_fqmn, code, py_filename, NULL);

    Py_DECREF(py_filename);
    Py_DECREF(code);

    return mod;

error:
    Py_DECREF(code);
    return NULL;
}


// Implement the optional get_code() method for the importer.
static PyObject *qrcimporter_get_code(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_get_code";
#endif
    QString fqmn;

    if (!parse_qstring(args, "U:qrcimporter.get_code", fqmn))
        return NULL;

    QString pathname, filename;
    PyObject *result;

    switch (find_module((QrcImporter *)self, fqmn, pathname, filename))
    {
    case ModuleNotFound:
        raise_import_error(fqmn);
        return NULL;

    case ModuleIsModule:
    case ModuleIsPackage:
        result = get_code_object(filename);
        break;

    default:
        result = Py_None;
        Py_INCREF(result);
    }

    return result;
}


// Implement the optional get_resource_reader() method for the importer.
static PyObject *qrcimporter_get_resource_reader(PyObject *self, PyObject *arg)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_get_resource_reader";
#endif
    return PyObject_CallFunctionObjArgs((PyObject *)&QrcReader_Type, self,
                                        arg, NULL);
}


// The reader initialisation function.
static int qrcreader_init(PyObject *self, PyObject *args, PyObject *kwds)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcreader_init";
#endif
    // It's not clear if this is part of the public API.
    if (!_PyArg_NoKeywords("qrcreader()", kwds))
        return -1;

    QrcImporter *importer;
    PyObject *py_package;

    if (!PyArg_ParseTuple(args, "OU:qrcreader", &importer, &py_package))
        return -1;

    QString package = str_to_qstring(py_package);
    QString pathname, filename;

    if (find_module(importer, package, pathname, filename) != ModuleIsPackage)
    {
        PyErr_Format(PyExc_ImportError, "qrcreader: %s is not a package",
                     package.toLatin1().constData());

        return -1;
    }

    ((QrcReader *)self)->pathname = new QString(pathname);

    return 0;
}


// The reader deallocation function.
static void qrcreader_dealloc(PyObject *self)
{
    if (((QrcReader *)self)->pathname)
    {
        delete ((QrcReader *)self)->pathname;
        ((QrcReader *)self)->pathname = 0;
    }

    Py_TYPE(self)->tp_free(self);
}


// Implement the contents() method for the reader.
static PyObject *qrcreader_contents(PyObject *self, PyObject *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcreader_contents";
#endif
    QStringList contents(QDir(*((QrcReader *)self)->pathname).entryList());

    PyObject *py_contents = PyTuple_New(contents.size());
    if (!py_contents)
        return NULL;

    for (int i = 0; i < contents.size(); ++i)
    {
        PyObject *py_name = qstring_to_str(contents.at(i));
        if (!py_name)
        {
            Py_DECREF(py_contents);
            return NULL;
        }

        PyTuple_SET_ITEM(py_contents, i, py_name);
    }

    return py_contents;
}


// Implement the is_resource() method for the reader.
static PyObject *qrcreader_is_resource(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcreader_is_resource";
#endif
    QString resource;
    PyObject *py_resource;

    if (!parse_qstring(args, "U:qrcreader.is_resource", resource, &py_resource))
        return NULL;

    QFileInfo resource_info(get_resource_path((QrcReader *)self, resource));

    if (!resource_info.exists())
    {
        PyErr_Format(PyExc_FileNotFoundError, "%R does not exist",
                     py_resource);

        return NULL;
    }

    if (resource_info.isFile())
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}


// Implement the open_resource() method for the reader.
static PyObject *qrcreader_open_resource(PyObject *self, PyObject *arg)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcreader_open_resource";
#endif
    // Note that a resource should be able to be specified as a path-like
    // object but (at the moment) we only support strings.
    return PyObject_CallFunctionObjArgs((PyObject *)&QrcResource_Type, self,
                                        arg, NULL);
}


// Implement the resource_path() method for the reader.
static PyObject *qrcreader_resource_path(PyObject *self, PyObject *arg)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcreader_resource_path";
#endif
    PyErr_Format(PyExc_FileNotFoundError, "%R is not on the file system", arg);

    return NULL;
}


// The resource initialisation function.
static int qrcresource_init(PyObject *self, PyObject *args, PyObject *kwds)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_init";
#endif
    // It's not clear if this is part of the public API.
    if (!_PyArg_NoKeywords("qrcresource()", kwds))
        return -1;

    QrcReader *reader;
    PyObject *py_resource;

    if (!PyArg_ParseTuple(args, "OU:qrcresource", &reader, &py_resource))
        return -1;

    QFile *resource = new QFile(
                get_resource_path(reader, str_to_qstring(py_resource)));

    if (!resource->open(QIODevice::ReadOnly))
    {
        delete resource;
        return -1;
    }

    ((QrcResource *)self)->resource = resource;

    return 0;
}


// The resource deallocation function.
static void qrcresource_dealloc(PyObject *self)
{
    QFile *resource = ((QrcResource *)self)->resource;

    if (resource)
    {
        if (resource->isOpen())
            resource->close();

        delete resource;
        ((QrcResource *)self)->resource = 0;
    }

    Py_TYPE(self)->tp_free(self);
}


// Implement the close() method for the resource.
static PyObject *qrcresource_close(PyObject *self, PyObject *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_close";
#endif
    QFile *resource = ((QrcResource *)self)->resource;

    if (resource->isOpen())
        resource->close();

    Py_RETURN_NONE;
}


// Implement the flush() method for the resource.
static PyObject *qrcresource_flush(PyObject *self, PyObject *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_flush";
#endif
    Py_RETURN_NONE;
}


// Implement the read() method for the resource.
static PyObject *qrcresource_read(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_read";
#endif
    Py_ssize_t size = -1;

    if (!PyArg_ParseTuple(args, "|n:qrcresource.read", &size))
        return NULL;

    QFile *resource = ((QrcResource *)self)->resource;

    QByteArray data = (size < 0 ? resource->readAll() : resource->read(size));

    return PyBytes_FromStringAndSize(data.constData(), data.size());
}


// Implement the readable() method for the resource.
static PyObject *qrcresource_readable(PyObject *self, PyObject *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_readable";
#endif
    Py_RETURN_TRUE;
}


// Implement the seekable() method for the resource.
static PyObject *qrcresource_seekable(PyObject *self, PyObject *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_seekable";
#endif
    Py_RETURN_FALSE;
}


// Implement the writable() method for the resource.
static PyObject *qrcresource_writable(PyObject *self, PyObject *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_writable";
#endif
    Py_RETURN_FALSE;
}


// Implement the closed getter for the resource.
static PyObject *qrcresource_get_closed(PyObject *self, void *)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcresource_get_closed";
#endif
    QFile *resource = ((QrcResource *)self)->resource;

    if (resource->isOpen())
        Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}


// Return the full pathname of a resource.
static QString get_resource_path(QrcReader *reader, const QString &resource)
{
    return QString("%1/%2").arg(*reader->pathname).arg(resource);
}


// Implement the optional get_source() method for the importer.
static PyObject *qrcimporter_get_source(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_get_source";
#endif
    Py_RETURN_NONE;
}


// Implement the optional is_package() method for the importer.
static PyObject *qrcimporter_is_package(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_is_package";
#endif
    QString fqmn;

    if (!parse_qstring(args, "U:qrcimporter.is_package", fqmn))
        return NULL;

    QString pathname, filename;
    PyObject *result;

    switch (find_module((QrcImporter *)self, fqmn, pathname, filename))
    {
    case ModuleNotFound:
        raise_import_error(fqmn);
        return NULL;

    case ModuleIsPackage:
        result = Py_True;
        break;

    default:
        result = Py_False;
    }

    Py_INCREF(result);
    return result;
}


// Implement the optional get_data() method for the importer.
static PyObject *qrcimporter_get_data(PyObject *self, PyObject *args)
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"qrcimporter_get_data";
#endif
    QString filename;

    if (!parse_qstring(args, "U:qrcimporter.get_data", filename))
        return NULL;

    QByteArray data;

    if (!read_data(filename, data))
        return NULL;

    return PyBytes_FromStringAndSize(data.constData(), data.size());
}


// Find a fully qualified module name handled by an importer and return its
// type, path name and file name.
static ModuleType find_module(QrcImporter *self, const QString &fqmn,
                              QString &pathname, QString &filename)
{
    QStringList fqmn_parts = fqmn.split(QChar('.'));
    QString fqmn_last = fqmn_parts.takeLast();
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"find_module"<<"fqmn"<<fqmn<<"self.path_parts"<<*self->path_parts<<"self.path"<<*self->path<< "self.invalid_part"<<*self->invalid_part;
#endif
    //    // Reject it if the path is clearly wrong.
    //    if (*self->path_parts != fqmn_parts)
    //        return ModuleNotFound;


    if (*(self->is_zip)){ // (*self->path_parts).last().endsWith(".zip")
        return ModuleIsZip;
    }

    QDir _lib_dir(QCoreApplication::libraryPaths().at(0));
    pathname = _lib_dir.absoluteFilePath(fqmn_last);
    filename = _lib_dir.absoluteFilePath("lib"+fqmn_last+".cpython-38.so");
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<pathname; // filename
#endif
    if (QFileInfo(filename).isFile())
        return ModuleIsAdjacentExtensionModule;
    pathname = *self->path + fqmn_last;
    filename = pathname + ".cpython-38.so";
    if (QFileInfo(filename).isFile())
        return ModuleIsAdjacentExtensionModule;

    // See if it is an ordinary module.
    filename = pathname + ".pyo";
    if (QFileInfo(filename).isFile())
        return ModuleIsModule;
    filename = pathname + ".pyc";
    if (QFileInfo(filename).isFile())
        return ModuleIsModule;
    filename = pathname + ".py";
    if (QFileInfo(filename).isFile())
        return ModuleIsModule;

    // See if it is a package.
    filename = pathname + "/__init__.pyo";
    if (QFileInfo(filename).isFile())
        return ModuleIsPackage;
    filename = pathname + "/__init__.pyc";
    if (QFileInfo(filename).isFile())
        return ModuleIsPackage;
    filename = pathname + "/__init__.py";
    if (QFileInfo(filename).isFile())
        return ModuleIsPackage;




    // See if it is an adjacent extension module.  Allow for the fact that we
    // can be called before we have set the executable directory.
    if (executable_dir)
    {
        const QDir &exec_dir = pdytools_get_executable_dir();

        QString em_name(fqmn);
        em_name.append(extension_module_extension);

#if defined(Q_OS_DARWIN)
        // The PlugIns directory is the prefered location for dynamic modules.
        filename = exec_dir.filePath(QString("../PlugIns/%1").arg(em_name));

        if (QFileInfo(filename).isFile())
            return ModuleIsAdjacentExtensionModule;

        filename = exec_dir.filePath(QString("../Frameworks/%1").arg(em_name));

        if (QFileInfo(filename).isFile())
            return ModuleIsAdjacentExtensionModule;
#endif

        filename = exec_dir.filePath(em_name);

        if (QFileInfo(filename).isFile())
            return ModuleIsAdjacentExtensionModule;
    }

    // See if it is a namespace.
    filename = pathname;

    if (QFileInfo(filename).isDir())
        return ModuleIsNamespace;

    // Nothing was found.
    return ModuleNotFound;
}


// Get the data from a file.
static bool read_data(const QString &filename, QByteArray &data)
{
    QFile mfile(filename);
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"read_data"<<filename;
#endif
    if (!mfile.open(QIODevice::ReadOnly))
    {
        PyErr_Format(PyExc_ImportError, "qrcimporter: error opening file %s",
                     filename.toLatin1().constData());
        return false;
    }

    data = mfile.readAll();

    mfile.close();

    return true;
}


// Get the code object from a file.
static PyObject *get_code_object(const QString &filename)
{
    QByteArray data;

    if (!read_data(filename, data))
        return NULL;

    return PyMarshal_ReadObjectFromString(data.data(), data.size());
}


// Parse an argument tuple for a single QString.
static bool parse_qstring(PyObject *args, const char *fmt, QString &qstring,
                          PyObject **str_obj)
{
    PyObject *py_string;

    if (!PyArg_ParseTuple(args, fmt, &py_string))
        return false;

    if (str_obj)
        *str_obj = py_string;

    qstring = str_to_qstring(py_string);

    return true;
}


// Convert a Python str object to a QString.
static QString str_to_qstring(PyObject *str)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(str);

    switch (PyUnicode_KIND(str))
    {
    case PyUnicode_1BYTE_KIND:
        return QString::fromLatin1((char *)PyUnicode_1BYTE_DATA(str), len);

    case PyUnicode_2BYTE_KIND:
        // The (QChar *) cast should be safe.
        return QString((QChar *)PyUnicode_2BYTE_DATA(str), len);

    case PyUnicode_4BYTE_KIND:
        return QString::fromUcs4(PyUnicode_4BYTE_DATA(str), len);
    }

    return QString();
}


// Convert a QString to a Python str object.
static PyObject *qstring_to_str(const QString &qstring)
{
    QVector<uint> ucs4 = qstring.toUcs4();

    return PyUnicode_FromKindAndData(PyUnicode_4BYTE_KIND, ucs4.data(),
                                     ucs4.size());
}


// Raise an ImportError when a module could not be found.
static void raise_import_error(const QString &fqmn)
{
    PyErr_Format(PyExc_ImportError, "qrcimporter: can't find module %s",
                 fqmn.toLatin1().constData());
}


// Initialise the directory containing the executable.
void pdytools_init_executable_dir(const QString &argv0)
{
    QString name;
    PyObject *executable = PySys_GetObject("executable");

    if (executable && executable != Py_None)
        name = str_to_qstring(executable);
    else
        name = argv0;

    executable_dir = new QDir(name);
    executable_dir->makeAbsolute();
    executable_dir->cdUp();
}


// Return the directory containing the executable.
const QDir &pdytools_get_executable_dir()
{
    return *executable_dir;
}


// The module initialisation function.
PyObject *PyInit_pdytools()
{
#ifdef QTPYANDASSETS_DEBUG
    qDebug()<<"PyInit_pdytools";
#endif
    PyObject *mod;

    // Just in case we are linking against Python as a Windows DLL.
    QrcImporter_Type.tp_new = PyType_GenericNew;

    if (PyType_Ready(&QrcImporter_Type) < 0)
        return NULL;

    QrcReader_Type.tp_new = PyType_GenericNew;

    if (PyType_Ready(&QrcReader_Type) < 0)
        return NULL;

    QrcResource_Type.tp_new = PyType_GenericNew;

    if (PyType_Ready(&QrcResource_Type) < 0)
        return NULL;

    mod = PyModule_Create(&pdytoolsmodule);
    if (mod == NULL)
        return NULL;

    if (PyModule_AddIntConstant(mod, "hexversion", PYQTDEPLOY_HEXVERSION) < 0)
    {
        Py_DECREF(mod);
        return NULL;
    }

    Py_INCREF(&QrcImporter_Type);
    if (PyModule_AddObject(mod, "qrcimporter", (PyObject *)&QrcImporter_Type) < 0)
    {
        Py_DECREF(mod);
        return NULL;
    }

    return mod;
}
