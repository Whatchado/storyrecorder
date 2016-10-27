;NSIS Modern User Interface
;Welcome/Finish Page Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General
!define VERSION "1.76"

!define APPNAME "whatchado StoryRecorder"
!define COMPANYNAME "whatchado"
!define DESCRIPTION "whatchado StoryRecorder"
# These three must be integers
!define VERSIONMAJOR 1
!define VERSIONMINOR 57
!define VERSIONBUILD 0

  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY "Software\whatchado StoryRecorder"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"


# These will be displayed by the "Click here for support information" link in "Add/Remove Programs"
# It is possible to use "mailto:" links in here to open the email client
!define HELPURL "http://..." # "Support Information" link
!define UPDATEURL "http://..." # "Product Updates" link
!define ABOUTURL "http://..." # "Publisher" link
# This is the size (in kB) of all the files copied into "Program Files"
!define INSTALLSIZE 7233

  ;Name and file
  Name "whatchado StoryRecorder"
  OutFile "Z:\whatchado_StoryRecorder_${VERSION}_installer.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\whatchado StoryRecorder"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\whatchado StoryRecorder" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE_3LINES
;!define MUI_WELCOMEFINISHPAGE_BITMAP "d:\whatchado_diy\WinInstall-01_Welcome-x2-72dpi.bmp"
;!define MUI_WELCOMEFINISHPAGE_BITMAP "C:\Program Files (x86)\NSIS\Contrib\Graphics\Wizard\arrow.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "d:\whatchado_diy\install_side.bmp"
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "d:\whatchado_diy\License.txt"
 ; !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !define MUI_WELCOMEPAGE_TITLE_3LINES
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "German"


Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR"

  ;ADD YOUR OWN FILES HERE...

  File "D:\build-whatchado_diy-Desktop_Qt_5_4_0_MSVC2010_OpenGL_32bit-Debug\debug\whatchado_StoryRecorder.exe"
  ;File "D:\whatchado_diy\resources\whatchado_diy.ico"
  File /r "Z:\whatchado\whatchado_diy_win_1.45\*"
  File D:\whatchado_diy\languages\whatchado_diy.de.qm
  File D:\whatchado_diy\languages\whatchado_diy.en.qm

  ;Store installation folder
  WriteRegStr HKCU "Software\whatchado StoryRecorder" "" $INSTDIR

  CreateShortCut "$DESKTOP\whatchado StoryRecorder.lnk" "$INSTDIR\whatchado_StoryRecorder.exe" ""

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${APPNAME}"
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$\"$INSTDIR\whatchado_diy.ico$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "$\"${COMPANYNAME}$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
        WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayVersion" "$\"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}$\""
        WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
        WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "VersionMinor" ${VERSIONMINOR}
        # There is no option for modifying or repairing the install
        WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoModify" 1
        WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoRepair" 1
        # Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
        WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "EstimatedSize" ${INSTALLSIZE}
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$DESKTOP\whatchado StoryRecorder.lnk"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\whatchado StoryRecorder"
  # Remove uninstaller information from the registry
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
SectionEnd
