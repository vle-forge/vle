; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "VLE"
#define MyABI GetEnv('VLE_ABI')
#define MyAppVersion GetEnv('VLE_VERSION')
#define MyAppPublisher "VLE Development Team"
#define MyAppURL "https://www.vle-project.org"
#define MyAppExeName "gvle.exe"

#define SourcePath GetEnv('SourcePath')
#define QtDir GetEnv('QtDir')
#define QtDirTools GetEnv('QtDirTools')
#define InstallPath GetEnv('InstallPath')
#define CMakePath GetEnv('CMakePath')
#define VleDeps GetEnv('VleDeps')
#define MingwPath GetEnv('MingwPath')
#define MyAppVersionString StringChange(MyAppVersion, ".", "_")

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{FC8138BE-AE90-4A8E-92DB-B8B8E96A1BDB}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}_{#MyAppVersionString}
DefaultGroupName={#MyAppName}
LicenseFile={#InstallPath}\share\vle-{#MyABI}\CopyRight.txt
InfoBeforeFile={#InstallPath}\share\vle-{#MyABI}\Readme.txt
OutputBaseFilename=Setup-VLE_{#MyAppVersionString}
SetupIconFile={#InstallPath}\share\vle-{#MyABI}\pixmaps\vle.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Registry]
Root: HKCU; Subkey: "Software\{#MyAppPublisher}"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName} {#MyABI}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppPublisher}"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\{#MyAppPublisher}\{#MyAppName} {#MyABI}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppPublisher}\{#MyAppName} {#MyABI}"; ValueType: string; ValueName: "Path"; ValueData: "{app}"
Root: HKLM; Subkey: "Software\{#MyAppPublisher}"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\{#MyAppPublisher}\{#MyAppName} {#MyABI}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppPublisher}\{#MyAppName} {#MyABI}"; ValueType: string; ValueName: "Path"; ValueData: "{app}"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#InstallPath}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#InstallPath}\share\vle-{#MyABI}\qt.conf"; DestDir: "{app}\bin"; Flags: ignoreversion

Source: "{#QtDirTools}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#MingwPath}\bin\curl.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\include\iconv.h"; DestDir: "{app}\include"; Flags: ignoreversion
Source: "{#MingwPath}\include\libintl.h"; DestDir: "{app}\include"; Flags: ignoreversion
Source: "{#MingwPath}\include\lzma.h"; DestDir: "{app}\include"; Flags: ignoreversion
Source: "{#MingwPath}\include\zlib.h"; DestDir: "{app}\include"; Flags: ignoreversion
Source: "{#MingwPath}\include\lzma\*"; DestDir: "{app}\include\lzma"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MingwPath}\include\libxml2\*"; DestDir: "{app}\include\libxml2"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#MingwPath}\bin\libbrotlicommon.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libbrotlidec.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libcrypto-1_1.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libpsl-5.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libssl-1_1.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

Source: "{#MingwPath}\bin\libcurl-4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libexpat-1.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libiconv-2.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libidn2-4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libintl-8.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\liblzma-5.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libmetalink-3.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libnghttp2-14.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libssh2-1.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libunistring-2.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\libxml2-2.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\zlib1.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\bin\curl.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#MingwPath}\lib\liblzma.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\liblzma.dll.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libxml2.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libxml2.dll.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libz.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libz.dll.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libiconv.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libiconv.dll.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libintl.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\lib\libintl.dll.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#MingwPath}\ssl\*"; DestDir: "{app}\ssl"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MingwPath}\ssl\certs\*"; DestDir: "{app}\ssl\certs"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#CMakePath}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#QtDir}\bin\qmake.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\moc.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\uic.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\rcc.exe"; DestDir: "{app}\bin"; Flags: ignoreversion

Source: "{#QtDir}\mkspecs\*"; DestDir: "{app}\mkspecs"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#QtDir}\bin\Qt5Concurrent.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Core.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Gui.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Help.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5OpenGL.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5PrintSupport.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Network.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Sql.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Svg.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Xml.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5XmlPatterns.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Widgets.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Concurrentd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Cored.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Guid.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Helpd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5OpenGLd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5PrintSupportd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Networkd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Sqld.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Svgd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Xmld.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5XmlPatternsd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt5Widgetsd.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

Source: "{#QtDir}\lib\libQt5Concurrent.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Core.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Gui.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Help.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5OpenGL.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5PrintSupport.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Network.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Sql.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Svg.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Xml.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5XmlPatterns.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Widgets.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Concurrentd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Cored.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Guid.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Helpd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5OpenGLd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5PrintSupportd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Networkd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Sqld.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Svgd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Xmld.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5XmlPatternsd.a"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "{#QtDir}\lib\libQt5Widgetsd.a"; DestDir: "{app}\lib"; Flags: ignoreversion

Source: "{#QtDir}\bin\libGLESV2.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\libEGL.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\D3Dcompiler_47.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtDir}\bin\opengl32sw.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

Source: "{#QtDir}\include\QtANGLE\*"; DestDir: "{app}\include\QtANGLE"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtConcurrent\*"; DestDir: "{app}\include\QtConcurrent"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtCore\*"; DestDir: "{app}\include\QtCore"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtGui\*"; DestDir: "{app}\include\QtGui"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtHelp\*"; DestDir: "{app}\include\QtHelp"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtOpenGL\*"; DestDir: "{app}\include\QtOpenGL"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtPrintSupport\*"; DestDir: "{app}\include\QtPrintSupport"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtNetwork\*"; DestDir: "{app}\include\QtNetwork"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtSvg\*"; DestDir: "{app}\include\QtSvg"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtXml\*"; DestDir: "{app}\include\QtXml"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtXmlPatterns\*"; DestDir: "{app}\include\QtXmlPatterns"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\include\QtWidgets\*"; DestDir: "{app}\include\QtWidgets"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#QtDir}\lib\cmake\Qt5\*"; DestDir: "{app}\lib\cmake\Qt5"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Concurrent\*"; DestDir: "{app}\lib\cmake\Qt5Concurrent"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Core\*"; DestDir: "{app}\lib\cmake\Qt5Core"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Gui\*"; DestDir: "{app}\lib\cmake\Qt5Gui"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Help\*"; DestDir: "{app}\lib\cmake\Qt5Help"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5OpenGL\*"; DestDir: "{app}\lib\cmake\Qt5OpenGL"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5PrintSupport\*"; DestDir: "{app}\lib\cmake\Qt5PrintSupport"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Network\*"; DestDir: "{app}\lib\cmake\Qt5Network"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Svg\*"; DestDir: "{app}\lib\cmake\Qt5Svg"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Xml\*"; DestDir: "{app}\lib\cmake\Qt5Xml"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5XmlPatterns\*"; DestDir: "{app}\lib\cmake\Qt5XmlPatterns"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtDir}\lib\cmake\Qt5Widgets\*"; DestDir: "{app}\lib\cmake\Qt5Widgets"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#QtDir}\lib\pkgconfig\Qt5Concurrent.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Core.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Gui.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Help.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5OpenGL.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5PrintSupport.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Network.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Svg.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Xml.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5XmlPatterns.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion
Source: "{#QtDir}\lib\pkgconfig\Qt5Widgets.pc"; DestDir: "{app}\lib\pkgconfig"; Flags: ignoreversion

Source: "{#QtDir}\plugins\bearer\*"; DestDir: "{app}\bin\bearer"; Flags: ignoreversion
Source: "{#QtDir}\plugins\iconengines\*"; DestDir: "{app}\bin\iconengines"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\*"; DestDir: "{app}\bin\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\generic\*"; DestDir: "{app}\bin\generic"; Flags: ignoreversion
Source: "{#QtDir}\plugins\platforms\*"; DestDir: "{app}\bin\platforms"; Flags: ignoreversion
Source: "{#QtDir}\plugins\platforminputcontexts\*"; DestDir: "{app}\bin\platforminputcontexts"; Flags: ignoreversion
Source: "{#QtDir}\plugins\bearer\*"; DestDir: "{app}\plugins\bearer"; Flags: ignoreversion
Source: "{#QtDir}\plugins\iconengines\*"; DestDir: "{app}\plugins\iconengines"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\*"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\generic\*"; DestDir: "{app}\plugins\generic"; Flags: ignoreversion
Source: "{#QtDir}\plugins\platforms\*"; DestDir: "{app}\plugins\platforms"; Flags: ignoreversion
Source: "{#QtDir}\plugins\platforminputcontexts\*"; DestDir: "{app}\plugins\platforminputcontexts"; Flags: ignoreversion
Source: "{#QtDir}\plugins\printsupport\*"; DestDir: "{app}\plugins\printsupport"; Flags: ignoreversion


Source: "{#QtDir}\translations\qt_bg.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_bg.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_ca.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_ca.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_cs.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_cs.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_da.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_da.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_de.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_de.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_en.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_en.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_es.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_es.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_fi.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_fi.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_fr.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_fr.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_he.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_he.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_hu.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_hu.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_it.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_it.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_ja.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_ja.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_ko.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_ko.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_pl.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_pl.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_ru.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_ru.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_sk.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_sk.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qt_uk.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion
Source: "{#QtDir}\translations\qtbase_uk.qm"; DestDir: "{app}\bin\translations"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
