# Gadget II
INCLUDEPATH *= $$clean_path($$PWD/gadget/include)
HEADERS += $$PWD/gadget/include/gadget/GadgetDLL.h

win32 {
contains(QT_ARCH, i386) {
        GADGET_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/gadget/Win32/GadgetDll.dll))
        LIBS += -L$$PWD/gadget/Win32 -lGadgetDll
    } else {
        GADGET_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/gadget/x64/GadgetDll.dll))
        LIBS += -L$$PWD/gadget/x64 -lGadgetDll
    }
    GADGET_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/GadgetDll.dll))
} else {
    SOURCES += gadget/stub/GadgetDLL.cpp
}

win32 {
    # Copy the Gadget DLL to the debug directory for debugging
    QMAKE_POST_LINK += $$QMAKE_COPY $${GADGET_DLL_SRC} $$shell_quote($$system_path($${DESTDIR})) $$escape_expand(\\n\\t)
}
