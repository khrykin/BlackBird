[Setup]
AppName=BlackBird
AppVersion=0.1.0
DefaultDirName={cf}
DefaultGroupName=BlackBird
OutputBaseFilename=BlackBird-windows-x86
 
[Files]
Source: "..\..\Builds\VisualStudio2019\Win32\Release\VST3\BlackBird.vst3"; DestDir: "{app}\Steinberg\VST3"
Source: "..\..\Builds\VisualStudio2019\Win32\Release\Standalone Plugin\BlackBird.exe"; DestDir: "{app}\BlackBird"
Source: "..\..\Presets\*"; DestDir: "C:\ProgramData\BlackBird\Presets"
