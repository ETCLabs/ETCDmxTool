# Gadget II
INCLUDEPATH += gadget
HEADERS += gadget/GadgetDLL.h
win32 {
    GADGET_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/gadget/GadgetDll.dll))
    GADGET_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/GadgetDll.dll))
    LIBS += -L$$PWD/gadget -lGadgetDll
}
unix {
    SOURCES += gadget/GadgetDLL.cpp
}

win32 {
    # Copy the Gadget DLL to the debug directory for debugging
    QMAKE_POST_LINK += $$QMAKE_COPY $${GADGET_DLL_SRC} $$shell_quote($$system_path($${DESTDIR})) $$escape_expand(\\n\\t)
}
