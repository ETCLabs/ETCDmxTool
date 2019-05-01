TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../
HEADERS         = asciiplugin.h \
    ../../packetbuffer.h \
    ../../util.h
SOURCES         = asciiplugin.cpp \
    ../../packetbuffer.cpp \
    ../../util.cpp
TARGET          = $$qtLibraryTarget(dissectorpluginASCII)

CONFIG(debug, debug|release) {
    DESTDIR = ../../../debug
}
CONFIG(release, debug|release) {
    DESTDIR = ../../../release
}
