# Generated for android-32 and Python v3.8.10.

TEMPLATE = app

QT += purchasing charts androidextras networkauth datavisualization network widgets
CONFIG += warn_off
CONFIG += qscintilla2

QMAKE_CFLAGS += -std=c99

RESOURCES = \
    resources/pyqtdeploy0.qrc \
    resources/pyqtdeploy1.qrc \
    resources/pyqtdeploy2.qrc

DEFINES += PYQTDEPLOY_FROZEN_MAIN PYQTDEPLOY_OPTIMIZED
DEFINES += HAVE_EXPAT_CONFIG_H
DEFINES += Py_BUILD_CORE_MODULE
DEFINES += USE_PYEXPAT_CAPI
DEFINES += XML_DEV_URANDOM
DEFINES += XML_POOR_ENTROPY
DEFINES += XML_STATIC

INCLUDEPATH += /home/m/client_android/build/sysroot-android-32/include
INCLUDEPATH += /home/m/client_android/build/sysroot-android-32/include/python3.8
INCLUDEPATH += /home/m/client_android/build/sysroot-android-32/include/python3.8/internal
INCLUDEPATH += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_blake2
INCLUDEPATH += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_sha3
INCLUDEPATH += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/expat

SOURCES = pyqtdeploy_main.cpp pyqtdeploy_start.cpp pdytools_module.cpp
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_abc.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_bisectmodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_blake2/blake2b_impl.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_blake2/blake2module.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_blake2/blake2s_impl.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_contextvarsmodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_csv.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_datetimemodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_elementtree.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_hashopenssl.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_heapqmodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_math.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_opcode.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_pickle.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_posixsubprocess.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_queuemodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_randommodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_sha3/sha3module.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_ssl.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/_struct.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/arraymodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/binascii.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/expat/xmlparse.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/expat/xmlrole.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/expat/xmltok.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/mathmodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/pyexpat.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/selectmodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/sha512module.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/socketmodule.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/termios.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/unicodedata.c
SOURCES += /home/m/client_android/build/sysroot-android-32/src/Python-3.8.10/Modules/zlibmodule.c

HEADERS = pyqtdeploy_version.h frozen_bootstrap.h frozen_bootstrap_external.h frozen_main.h

LIBS += -L/home/m/client_android/build/sysroot-android-32/lib
LIBS += -L/home/m/client_android/build/sysroot-android-32/lib/python3.8/site-packages/PyQt5
LIBS += -lQsci
LIBS += -lQtAndroidExtras
LIBS += -lQtChart
LIBS += -lQtCore
LIBS += -lQtDataVisualization
LIBS += -lQtGui
LIBS += -lQtNetwork
LIBS += -lQtNetworkAuth
LIBS += -lQtPurchasing
LIBS += -lQtWidgets
LIBS += -lcrypto
LIBS += -lpython3.8
LIBS += -lsip
LIBS += -lssl
LIBS += -lz

ANDROID_ABIS = armeabi-v7a
ANDROID_EXTRA_LIBS += /home/m/client_android/build/sysroot-android-32/lib/libssl_1_1.so /home/m/client_android/build/sysroot-android-32/lib/libcrypto_1_1.so

cython.name = Cython compiler
cython.input = CYTHONSOURCES
cython.output = ${QMAKE_FILE_BASE}.c
cython.variable_out = GENERATED_SOURCES
cython.commands = cython ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}

QMAKE_EXTRA_COMPILERS += cython

linux-* {
    LIBS += -lutil -ldl
}

win32 {
    masm.input = MASMSOURCES
    masm.output = ${QMAKE_FILE_BASE}.obj

    contains(QMAKE_TARGET.arch, x86_64) {
        masm.name = MASM64 compiler
        masm.commands = ml64 /Fo ${QMAKE_FILE_OUT} /c ${QMAKE_FILE_IN}
    } else {
        masm.name = MASM compiler
        masm.commands = ml /Fo ${QMAKE_FILE_OUT} /c ${QMAKE_FILE_IN}
    }

    QMAKE_EXTRA_COMPILERS += masm

    LIBS += -lpathcch -lshlwapi -ladvapi32 -lshell32 -luser32 -lws2_32 -lole32 -loleaut32 -lversion
    DEFINES += MS_WINDOWS _WIN32_WINNT=Py_WINVER NTDDI_VERSION=Py_NTDDI WINVER=Py_WINVER

    # This is added from the qmake spec files but clashes with _pickle.c.
    DEFINES -= UNICODE
}

macx {
    LIBS += -framework SystemConfiguration -framework CoreFoundation
}

DISTFILES += \
    ../android_res/AndroidManifest.xml \
    ../android_res/res/drawable-hdpi/icon.png \
    ../android_res/res/drawable-ldpi/icon.png \
    ../android_res/res/drawable-mdpi/icon.png \
    ../android_res/res/drawable-xhdpi/icon.png \
    ../android_res/res/drawable-xxhdpi/icon.png \
    ../android_res/res/drawable-xxxhdpi/icon.png \
    ../android_res/build.gradle \
    ../android_res/gradle.properties \
    ../android_res/gradle/wrapper/gradle-wrapper.jar \
    ../android_res/gradle/wrapper/gradle-wrapper.properties \
    ../android_res/gradlew \
    ../android_res/gradlew.bat \
    ../android_res/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = ../android_res

CONFIG += resources_big

