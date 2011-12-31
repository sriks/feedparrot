# -------------------------------------------------
# Project created by QtCreator 2010-08-25T11:13:26
# -------------------------------------------------
QT += testlib
TARGET = unittest_rssparser
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
HEADERS += test_rssparser.h
SOURCES += main.cpp \
    test_rssparser.cpp


RESOURCES += \
    resource.qrc

# rsssuite dependency start
# Any client willing to use rsssuite can copy the following lines in .pro
#####################################################
# For unix systems, dlls and headers are copied to standard /usr/lib and /usr/include respectively
# hence they need not be mentioned in unix targets.

QT += xmlpatterns
win32: {
    INCLUDEPATH += c:/decii/include
    LIBS += -Lc:/decii/lib
        }
    LIBS += -lrssparser
#####################################################


