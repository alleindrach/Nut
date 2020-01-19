win32 {
        CONFIG(debug,debug|release):   LIBDIR = $$PWD/../../lib/debug
        CONFIG(release,debug|release): LIBDIR = $$PWD/../../lib/release
} else {
        CONFIG(debug,debug|release):   LIBDIR = $$PWD/../../lib/debug
        CONFIG(release,debug|release): LIBDIR = $$PWD/../../lib/release
}

LIBS += -L$$LIBDIR -lnut
INCLUDEPATH += $$PWD/../../src $$PWD/../common
#include(../../src/src.pri)

DEFINES += NUT_SHARED_POINTER
