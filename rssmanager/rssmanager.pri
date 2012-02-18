INCLUDEPATH +=  $$PWD
DEPENDPATH += $$PWD
HEADERS += $$PWD/rssmanager.h \
           $$PWD/feedprofile.h
#           $$PWD/RSSModel.h
SOURCES += $$PWD/rssmanager.cpp \
           $$PWD/feedprofile.cpp
#           $$PWD/RSSModel.cpp
include(../rssparser/rssparser.pri)
QT += network xmlpatterns

contains(MEEGO_EDITION,harmattan) {
    message ('Buidling for Harmattan')
    DEFINES += DC_HARMATTAN
}
