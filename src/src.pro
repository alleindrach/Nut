QT       += sql gui script

TARGET = nut
TEMPLATE = lib
CONFIG += c++11
CONFIG += staticlib

DEFINES += QT_DEPRECATED_WARNINGS NUT_COMPILE_STATIC
CONFIG(debug, debug|release) {
DESTDIR +=$$PWD/../lib/debug
} else {
DESTDIR +=$$PWD/../lib/release
}

include($$PWD/src.pri)
include($$PWD/../ci-test-init.pri)
