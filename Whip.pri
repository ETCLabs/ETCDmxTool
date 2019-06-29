# Whip
HEADERS +=  src/whip/ftdcomm.h
SOURCES +=  src/whip/ftdcomm.cpp
contains(QT_ARCH, i386) {
    win32 {
        INCLUDEPATH += whip/windows
        FTD2xx_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/whip/windows/i386/ftd2xx.dll))
        FTD2xx_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/ftd2xx.dll))
        CONFIG(release, debug|release): LIBS += -L$$PWD/whip/windows/i386 -lftd2xx
        else:CONFIG(debug, debug|release): LIBS += -L$$PWD/whip/windows/i386 -lftd2xxd
    }
    unix {
        INCLUDEPATH += whip/linux/libftd2xx-i386-1.4.8/release
        LIBS += -L$$PWD/whip/linux/libftd2xx-i386-1.4.8/release/build -lftd2xx
    }
} else {
    win32 {
        INCLUDEPATH += whip/windows
        FTD2xx_DLL_SRC = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/whip/windows/amd64/ftd2xx.dll))
        FTD2xx_DLL_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/ftd2xx.dll))
        CONFIG(release, debug|release): LIBS += -L$$PWD/whip/windows/amd64 -lftd2xx
        else:CONFIG(debug, debug|release): LIBS += -L$$PWD/whip/windows/amd64 -lftd2xxd
    }
    unix {
        INCLUDEPATH += whip/linux/libftd2xx-x86_64-1.4.8/release
        LIBS += -L$$PWD/whip/linux/libftd2xx-x86_64-1.4.8/release/build -lftd2xx
    }
}

win32 {
    # Copy the Whip DLL to the debug directory for debugging
    QMAKE_POST_LINK += $$QMAKE_COPY $${FTD2xx_DLL_SRC} $$shell_quote($$system_path($${DESTDIR})) $$escape_expand(\\n\\t)
}
