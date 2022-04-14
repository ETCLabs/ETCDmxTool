TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../
HEADERS         = sipplugin.h \
    ../../packetbuffer.h \
    ../../util.h
SOURCES         = sipplugin.cpp \
    ../../packetbuffer.cpp \
    ../../util.cpp
TARGET          = $$qtLibraryTarget(dissectorpluginSIP)

CONFIG(debug, debug|release) {
    DESTDIR = ../../../debug
}
CONFIG(release, debug|release) {
    DESTDIR = ../../../release
}
