TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../
HEADERS         = dmxplugin.h \
    ../../packetbuffer.h \
    ../../util.h
SOURCES         = dmxplugin.cpp \
    ../../packetbuffer.cpp \
    ../../util.cpp
TARGET          = $$qtLibraryTarget(dissectorpluginDMX)

CONFIG(debug, debug|release) {
    DESTDIR = ../../../
}
CONFIG(release, debug|release) {
    DESTDIR = ../../../
}
