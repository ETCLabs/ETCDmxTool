TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../
HEADERS         = rdmplugin.h \
    ../../packetbuffer.h \
    ../../util.h \
    rdmdissector.h \
    ../../rdm/rdmpidstrings.h \
    rdm_appendix_b.h \
    rdm_dimmermsg.h
SOURCES         = rdmplugin.cpp \
    ../../packetbuffer.cpp \
    ../../util.cpp \
    rdmdissector.cpp \
    ../../rdm/rdmpidstrings.cpp \
    rdm_appendix_b.cpp \
    rdm_dimmermsg.cpp
TARGET          = $$qtLibraryTarget(dissectorpluginRDM)

CONFIG(debug, debug|release) {
    DESTDIR = ../../../
}
CONFIG(release, debug|release) {
    DESTDIR = ../../../
}



CONFIG += install_ok
