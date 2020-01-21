QT       += testlib sql

TARGET = tst_basic
TEMPLATE = app

CONFIG   += warn_on c++11

include(../common/nut-lib.pri)


#DEFINES += Q_OS_OSX

SOURCES += \
    ../common/comment.cpp \
    ../common/post.cpp \
    ../common/user.cpp \
    ../common/weblogdatabase.cpp \
    ../common/score.cpp \
    tst_basic.cpp

HEADERS += \
    ../common/consts.h \
    ../common/comment.h \
    ../common/post.h \
    ../common/user.h \
    ../common/weblogdatabase.h \
    ../common/score.h \
    tst_basic.h

include($$PWD/../../ci-test-init.pri)

