TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets

INCLUDEPATH    *= $$clean_path($$PWD/../../)

HEADERS += \
    $$PWD/rdmplugin.h \
    $$PWD/../../packetbuffer.h \
    $$PWD/../../util.h \
    $$PWD/rdmdissector.h \
    $$PWD/../../rdm/rdmpidstrings.h \
    $$PWD/rdm_appendix_b.h \
    $$PWD/rdm_dimmermsg.h

SOURCES += \
    $$PWD/rdmplugin.cpp \
    $$PWD/../../packetbuffer.cpp \
    $$PWD/../../util.cpp \
    $$PWD/rdmdissector.cpp \
    $$PWD/../../rdm/rdmpidstrings.cpp \
    $$PWD/rdm_appendix_b.cpp \
    $$PWD/rdm_dimmermsg.cpp

TARGET          = $$qtLibraryTarget(dissectorpluginRDM)

CONFIG(debug, debug|release) {
    DESTDIR = ../../../debug
}
CONFIG(release, debug|release) {
    DESTDIR = ../../../release
}



CONFIG += install_ok
