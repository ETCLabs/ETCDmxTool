TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../
HEADERS         = utf8plugin.h \
    ../../packetbuffer.h \
    ../../util.h
SOURCES         = utf8plugin.cpp \
    ../../packetbuffer.cpp \
    ../../util.cpp
TARGET          = $$qtLibraryTarget(dissectorpluginUTF8)

CONFIG(debug, debug|release) {
    DESTDIR = ../../../debug
}
CONFIG(release, debug|release) {
    DESTDIR = ../../../release
}
