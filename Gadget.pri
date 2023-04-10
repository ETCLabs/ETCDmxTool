# Gadget II
win32 {
contains(QT_ARCH, i386) {
        GADGET_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/gadget/libGadget/Win32/bin/GadgetDLL.dll))
        LIBS += -L$$PWD/gadget/libGadget/Win32/lib -lGadgetDll
        INCLUDEPATH += $$PWD/gadget/libGadget/Win32/include
        HEADERS += $$PWD/gadget/libGadget/Win32/include/GadgetDLL.h
    } else {
        GADGET_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/gadget/libGadget/x64/bin/GadgetDLL.dll))
        LIBS += -L$$PWD/gadget/libGadget/x64/lib -lGadgetDll
        INCLUDEPATH += $$PWD/gadget/libGadget/x64/include
        HEADERS += $$PWD/gadget/libGadget/x64/include/GadgetDLL.h
    }
    GADGET_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/GadgetDll.dll))
} else {
    SOURCES += gadget/stub/GadgetDLL.cpp
}

win32 {
    # Copy the Gadget DLL to the debug directory for debugging
    QMAKE_POST_LINK += $$QMAKE_COPY $${GADGET_DLL_SRC} $$shell_quote($$system_path($${DESTDIR})) $$escape_expand(\\n\\t)
}
