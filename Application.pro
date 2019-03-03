# Copyright (c) 2018 Electronic Theatre Controls, Inc., http://www.etcconnect.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# PRO file for the core application

TEMPLATE = app
TARGET = EtcDmxTool
QT += core gui

#   v2.0.0.3    Build with plugin-ized dissectors from Marcus

PRODUCT_VERSION=2.0.0.3

DEFINES += VERSION=\\\"$$PRODUCT_VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DLL

win32:LIBS += -l$$PWD/whip/ftd2xx -l$$PWD/gadget/GadgetDll

INCLUDEPATH *=  \
    src/ \
    RDM/ \
    whip/ \
    gadget

HEADERS += src/e110_startcodes.h \
    src/rdm/estardm.h \
    src/rdm/rdmcontroller.h \
    src/rdm/rdmEtcConsts.h \
    src/rdm/rdmpidstrings.h \
    src/dissectors/dissectorplugin.h \
    src/whip/ftdcomm.h \
    src/mainwindow.h \
    src/packettable.h \
    src/packetbuffer.h \
    src/capturedevice.h \
    src/fancysliderstyle.h \
    src/pcap/pcapng.h \
    src/pcap/pcapngLinkTypes.h \
    src/util.h \
    src/fileopen.h \
    src/hexlineedit.h \
    src/selectdevicedialog.h \
    src/dissectors.h \
    src/customdataroles.h \
    src/levelindicator.h

SOURCES += src/main.cpp \
    src/rdm/rdmcontroller.cpp \
    src/rdm/rdmpidstrings.cpp \
    src/whip/FTDComm.cpp \
    src/mainwindow.cpp \
    src/packetbuffer.cpp \
    src/packettable.cpp \
    src/capturedevice.cpp \
    src/fancysliderstyle.cpp \
    src/pcap/pcapng.cpp \
    src/util.cpp \
    src/selectdevicedialog.cpp \
    src/fileopen.cpp \
    src/hexlineedit.cpp \
    src/dissectors.cpp \
    src/levelindicator.cpp

FORMS += ui/mainwindow.ui \
    ui/selectdevicedialog.ui

RESOURCES += res/resources.qrc

win32:RC_ICONS += res/icon.ico

CONFIG(debug, debug|release) {
    DESTDIR = $${OUT_PWD}/debug
}
CONFIG(release, debug|release) {
    DESTDIR = $${OUT_PWD}/release
}

TARGET_CUSTOM_EXT = .exe
DEPLOY_DIR = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy))
DEPLOY_TARGET = $$shell_quote($$system_path($${DESTDIR}/$${TARGET}$${TARGET_CUSTOM_EXT}))

FTD2xx_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/whip/ftd2xx.dll))
FTD2xx_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/ftd2xx.dll))

GADGET_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/gadget/GadgetDll.dll))
GADGET_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/GadgetDll.dll))

DISSECTOR_DLL_SRC = $$shell_quote($$system_path($${OUT_PWD}/dissectorplugin*.dll))
DISSECTOR_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/dissectorplugin*.dll))

PRE_DEPLOY_COMMAND =  $$sprintf($${QMAKE_MKDIR_CMD}, $$shell_path($${DEPLOY_DIR})) $$escape_expand(\\n\\t)
PRE_DEPLOY_COMMAND += $$QMAKE_DEL_FILE $${DEPLOY_DIR}\*.* /S /Q $$escape_expand(\\n\\t)
PRE_DEPLOY_COMMAND += $$QMAKE_COPY $${FTD2xx_DLL_SRC} $${FTD2xx_DLL_DST} $$escape_expand(\\n\\t)
PRE_DEPLOY_COMMAND += $$QMAKE_COPY $${GADGET_DLL_SRC} $${GADGET_DLL_DST} $$escape_expand(\\n\\t)
PRE_DEPLOY_COMMAND += $$QMAKE_COPY $${DISSECTOR_DLL_SRC} $${DISSECTOR_DLL_DST} $$escape_expand(\\n\\t)
PRE_DEPLOY_COMMAND += $$QMAKE_COPY $${DEPLOY_TARGET} $${DEPLOY_DIR} $$escape_expand(\\n\\t)
DEPLOY_COMMAND = windeployqt
DEPLOY_OPT = --dir $${DEPLOY_DIR}
DEPLOY_INSTALLER = makensis /DPRODUCT_VERSION="$${PRODUCT_VERSION}" $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/install.nsi))

win32 {
    QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /INCREMENTAL:NO /Debug
}

# Copy the Gadget and Whip DLL to the debug directory for debugging
QMAKE_POST_LINK += $$QMAKE_COPY $${GADGET_DLL_SRC} $$shell_quote($$system_path($${DESTDIR})) $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $$QMAKE_COPY $${FTD2xx_DLL_SRC} $$shell_quote($$system_path($${DESTDIR})) $$escape_expand(\\n\\t)

CONFIG(release, debug|release) {
    QMAKE_POST_LINK += $${PRE_DEPLOY_COMMAND} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_COMMAND} $${DEPLOY_TARGET} $${DEPLOY_OPT} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_CLEANUP} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_INSTALLER} $$escape_expand(\\n\\t)
}
