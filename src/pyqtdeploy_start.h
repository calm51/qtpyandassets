#include "Python.h"

#include <QPair>
#include <QString>


//extern "C" PyObject *PyInit_Qsci(void);
//extern "C" PyObject *PyInit_QtAndroidExtras(void);
//extern "C" PyObject *PyInit_QtChart(void);
//extern "C" PyObject *PyInit_QtCore(void);
//extern "C" PyObject *PyInit_QtDataVisualization(void);
//extern "C" PyObject *PyInit_QtGui(void);
//extern "C" PyObject *PyInit_QtNetwork(void);
//extern "C" PyObject *PyInit_QtNetworkAuth(void);
//extern "C" PyObject *PyInit_QtPurchasing(void);
//extern "C" PyObject *PyInit_QtWidgets(void);
//extern "C" PyObject *PyInit_sip(void);
//extern "C" PyObject *PyInit__abc(void);
//extern "C" PyObject *PyInit__bisect(void);
//extern "C" PyObject *PyInit__blake2(void);
//extern "C" PyObject *PyInit__contextvars(void);
//extern "C" PyObject *PyInit__csv(void);
//extern "C" PyObject *PyInit__datetime(void);
//extern "C" PyObject *PyInit__elementtree(void);
//extern "C" PyObject *PyInit__hashlib(void);
//extern "C" PyObject *PyInit__heapq(void);
//extern "C" PyObject *PyInit__opcode(void);
//extern "C" PyObject *PyInit__pickle(void);
//extern "C" PyObject *PyInit__posixsubprocess(void);
//extern "C" PyObject *PyInit__queue(void);
//extern "C" PyObject *PyInit__random(void);
//extern "C" PyObject *PyInit__sha3(void);
//extern "C" PyObject *PyInit__sha512(void);
//extern "C" PyObject *PyInit__socket(void);
//extern "C" PyObject *PyInit__ssl(void);
//extern "C" PyObject *PyInit__struct(void);
//extern "C" PyObject *PyInit_array(void);
//extern "C" PyObject *PyInit_binascii(void);
//extern "C" PyObject *PyInit_math(void);
//extern "C" PyObject *PyInit_pyexpat(void);
//extern "C" PyObject *PyInit_select(void);
//extern "C" PyObject *PyInit_termios(void);
//extern "C" PyObject *PyInit_unicodedata(void);
//extern "C" PyObject *PyInit_zlib(void);

static struct _inittab extension_modules[] = {
    //    {"PyQt5.Qsci", PyInit_Qsci},
    //    {"PyQt5.QtAndroidExtras", PyInit_QtAndroidExtras},
    //    {"PyQt5.QtChart", PyInit_QtChart},
    //    {"PyQt5.QtCore", PyInit_QtCore},
    //    {"PyQt5.QtDataVisualization", PyInit_QtDataVisualization},
    //    {"PyQt5.QtGui", PyInit_QtGui},
    //    {"PyQt5.QtNetwork", PyInit_QtNetwork},
    //    {"PyQt5.QtNetworkAuth", PyInit_QtNetworkAuth},
    //    {"PyQt5.QtPurchasing", PyInit_QtPurchasing},
    //    {"PyQt5.QtWidgets", PyInit_QtWidgets},
    //    {"PyQt5.sip", PyInit_sip},
    //    {"_abc", PyInit__abc},
    //    {"_bisect", PyInit__bisect},
    //    {"_blake2", PyInit__blake2},
    //    {"_contextvars", PyInit__contextvars},
    //    {"_csv", PyInit__csv},
    //    {"_datetime", PyInit__datetime},
    //    {"_elementtree", PyInit__elementtree},
    //    {"_hashlib", PyInit__hashlib},
    //    {"_heapq", PyInit__heapq},
    //    {"_opcode", PyInit__opcode},
    //    {"_pickle", PyInit__pickle},
    //    {"_posixsubprocess", PyInit__posixsubprocess},
    //    {"_queue", PyInit__queue},
    //    {"_random", PyInit__random},
    //    {"_sha3", PyInit__sha3},
    //    {"_sha512", PyInit__sha512},
    //    {"_socket", PyInit__socket},
    //    {"_ssl", PyInit__ssl},
    //    {"_struct", PyInit__struct},
    //    {"array", PyInit_array},
    //    {"binascii", PyInit_binascii},
    //    {"math", PyInit_math},
    //    {"pyexpat", PyInit_pyexpat},
    //    {"select", PyInit_select},
    //    {"termios", PyInit_termios},
    //    {"unicodedata", PyInit_unicodedata},
    //    {"zlib", PyInit_zlib},
{NULL, NULL}
};

extern QString getPythonErrorString();

extern QPair<QString,QString> checkValidPath(QString fullPath);

extern int pyqtdeploy_start(int argc, char **argv,
                            struct _inittab *extension_modules, const char *main_module,
                            const char *entry_point, const char **path_dirs);
