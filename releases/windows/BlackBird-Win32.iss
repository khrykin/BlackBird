[Setup]
AppName=BlackBird
AppVersion=0.1.0
DefaultDirName={commoncf}
DefaultGroupName=BlackBird
OutputBaseFilename="BlackBird-{#SetupSetting("AppVersion")}-windows-x86"
 
[Files]
Source: "..\..\builds\VisualStudio2019\Win32\Release\VST3\BlackBird.vst3"; DestDir: "{app}\Steinberg\VST3"
Source: "..\..\builds\VisualStudio2019\Win32\Release\Standalone Plugin\BlackBird.exe"; DestDir: "{app}\BlackBird"
Source: "..\..\resources\Presets\*"; DestDir: "C:\ProgramData\BlackBird\Presets"
