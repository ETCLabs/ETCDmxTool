; Installation Script for ETCDMXTool
SetCompressor /SOLID lzma

!define PRODUCT_NAME "ETCDmxTool"
!define PRODUCT_PUBLISHER "ETC Labs"
!define PRODUCT_WEB_SITE "https://github.com/ETCLabs"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

;..................................................................................................
;Following two definitions required. Uninstall log will use these definitions.
;You may use these definitions also, when you want to set up the InstallDirRagKey,
;store the language selection, store Start Menu folder etc.
;Enter the windows uninstall reg sub key to add uninstall information to Add/Remove Programs also.

!define INSTDIR_REG_ROOT "HKLM"
!define INSTDIR_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
;..................................................................................................

!include MUI.nsh
!include AdvUninstLog.nsh
!include WinVer.nsh

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\res\icon.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE

; MSVC RunTime
!define MSVC_EXE "vcredist_x86.exe"
!define MSVC_OPT "/install /passive /norestart" 

Name "${PRODUCT_NAME}"
OutFile "${PRODUCT_NAME}_${PRODUCT_VERSION}.exe"
ShowInstDetails show
ShowUninstDetails show
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir"

!insertmacro INTERACTIVE_UNINSTALL

!insertmacro MUI_PAGE_WELCOME

; Check for Admin rights
Section CheckAdmin
	DetailPrint "Checking Admin Rights"
	System::Call "kernel32::GetModuleHandle(t 'shell32.dll') i .s"
	System::Call "kernel32::GetProcAddress(i s, i 680) i .r0"
	System::Call "::$0() i .r0"

	IntCmp $0 0 isNotAdmin isNotAdmin isAdmin
isNotAdmin:
	DetailPrint "Missing Administrator Rights !!!"
	messageBox MB_OK "You do not have Administrator rights on this computer.$\r$\r\
Please log in as an administrator to install sACNView."
	quit
isAdmin:
	DetailPrint "Administrator Rights granted"
SectionEnd

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN $INSTDIR\${PRODUCT_NAME}.exe
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"


Section "Main Application" sec01

	SetOutPath '$INSTDIR'

;After set the output path open the uninstall log macros block and add files/dirs with File /r
;This should be repeated every time the parent output path is changed either within the same
;section, or if there are more sections including optional components.
	!insertmacro UNINSTALL.LOG_OPEN_INSTALL

	File /r ".\deploy\"

;Once required files/dirs added and before change the parent output directory we need to
;close the opened previously uninstall log macros block.
	!insertmacro UNINSTALL.LOG_CLOSE_INSTALL

	; Drivers for the Gadget2
    CreateDirectory "$INSTDIR\Gadget_Drivers"
    SetOutPath "$INSTDIR\Gadget_Drivers"
	File ".\gadget_drv\ETC_WinUSB.exe"
	
	; Drivers for the USB FTDI Whip
    CreateDirectory "$INSTDIR\Whip_Drivers"
    SetOutPath "$INSTDIR\Whip_Drivers"
    File ".\whip_drv\ftdibus.cat"
    File ".\whip_drv\etc_whip.inf"
    CreateDirectory "$INSTDIR\Whip_Drivers\i386"
    SetOutPath "$INSTDIR\Whip_Drivers\i386"
	File ".\whip_drv\i386\ftbusui.dll"
	File ".\whip_drv\i386\ftcserco.dll"
	File ".\whip_drv\i386\ftd2xx.dll"
	File ".\whip_drv\i386\ftd2xx.lib"
	File ".\whip_drv\i386\ftdibus.sys"
	File ".\whip_drv\i386\ftlang.dll"
	File ".\whip_drv\i386\ftser2k.sys"
	File ".\whip_drv\i386\ftserui2.dll"
    CreateDirectory "$INSTDIR\Whip_Drivers\amd64"
    SetOutPath "$INSTDIR\Whip_Drivers\amd64"
	File ".\whip_drv\amd64\ftbusui.dll"
	File ".\whip_drv\amd64\ftcserco.dll"
	File ".\whip_drv\amd64\ftd2xx64.dll"
	File ".\whip_drv\amd64\ftd2xx.lib"
	File ".\whip_drv\amd64\ftdibus.sys"
	File ".\whip_drv\amd64\ftlang.dll"
	File ".\whip_drv\amd64\ftser2k.sys"
	File ".\whip_drv\amd64\ftserui2.dll"
	

	;Visual Studio runtime requirements
	DetailPrint "Installing MSVC Redistributables"
	ExecWait '"$INSTDIR\${MSVC_EXE}" ${MSVC_OPT}'

	;Gadget2 Drivers
	DetailPrint "Installing Gadget2 Driver"
	ExecWait '"$INSTDIR\Gadget_Drivers\ETC_WinUSB"'

	;Whip Drivers
	DetailPrint "Checking Windows Version"
	${If} ${AtLeastWinVista}
		DetailPrint "Windows Vista or above detected"
		DetailPrint '$SYSDIR\pnputil /a "$INSTDIR\Whip_Drivers\etc_whip.inf"'
		nsExec::ExecToLog '$SYSDIR\pnputil /a "$INSTDIR\Whip_Drivers\etc_whip.inf"'
	${EndIf}
	
	;Shortcuts
	CreateDirectory '$SMPROGRAMS\${PRODUCT_NAME}'
	SetOutPath '$INSTDIR' ;Used by CreateShortcut as the "Start In" path
	CreateShortcut '$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk' '$INSTDIR\${PRODUCT_NAME}.exe'
	;create shortcut for uninstaller always use ${UNINST_EXE} instead of uninstall.exe
	CreateShortcut '$SMPROGRAMS\${PRODUCT_NAME}\uninstall.lnk' '${UNINST_EXE}'

	WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir" "$INSTDIR"
	WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
	;Same as create shortcut you need to use ${UNINST_EXE} instead of anything else.
	WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "UninstallString" "${UNINST_EXE}"
SectionEnd


Function .onInit
        ;prepare log always within .onInit function
        !insertmacro UNINSTALL.LOG_PREPARE_INSTALL
FunctionEnd


Function .onInstSuccess

         ;create/update log always within .onInstSuccess function
         !insertmacro UNINSTALL.LOG_UPDATE_INSTALL

FunctionEnd

#######################################################################################

Section UnInstall

         ;begin uninstall, especially for MUI could be added in UN.onInit function instead
         ;!insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL

         ;uninstall from path, must be repeated for every install logged path individual
         !insertmacro UNINSTALL.LOG_UNINSTALL "$INSTDIR"

         ;end uninstall, after uninstall from all logged paths has been performed
         !insertmacro UNINSTALL.LOG_END_UNINSTALL

		 
		Delete "$INSTDIR\Gadget_Drivers\ETC_WinUSB.exe"
		RmDir "$INSTDIR\Gadget_Drivers"

		Delete "$INSTDIR\Whip_Drivers\ftdibus.cat"
		Delete "$INSTDIR\Whip_Drivers\etc_whip.inf"
		Delete "$INSTDIR\Whip_Drivers\i386\ftbusui.dll"
		Delete "$INSTDIR\Whip_Drivers\i386\ftcserco.dll"
		Delete "$INSTDIR\Whip_Drivers\i386\ftd2xx.dll"
		Delete "$INSTDIR\Whip_Drivers\i386\ftd2xx.lib"
		Delete "$INSTDIR\Whip_Drivers\i386\ftdibus.sys"
		Delete "$INSTDIR\Whip_Drivers\i386\ftlang.dll"
		Delete "$INSTDIR\Whip_Drivers\i386\ftser2k.sys"
		Delete "$INSTDIR\Whip_Drivers\i386\ftserui2.dll"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftbusui.dll"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftcserco.dll"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftd2xx64.dll"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftd2xx.lib"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftdibus.sys"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftlang.dll"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftser2k.sys"
		Delete "$INSTDIR\Whip_Drivers\amd64\ftserui2.dll"
		RmDir "$INSTDIR\Whip_Drivers\i386"
		RmDir "$INSTDIR\Whip_Drivers\amd64"
		RmDir "$INSTDIR\Whip_Drivers"

        Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
        Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
        RmDir "$SMPROGRAMS\${PRODUCT_NAME}"

        DeleteRegKey /ifempty ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}"

SectionEnd


Function UN.onInit
         ;begin uninstall, could be added on top of uninstall section instead
         !insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL
FunctionEnd