[Setup]
AppName=BlackFace
AppVersion=0.0.1
DefaultDirName={cf}
DefaultGroupName=BlackFace
OutputBaseFilename=BlackFace-windows-x64
 
[Files]
Source: "..\..\Builds\VisualStudio2019\x64\Release\VST3\BlackFace.vst3"; DestDir: "{app}\Steinberg\VST3"
Source: "..\..\Builds\VisualStudio2019\x64\Release\Standalone Plugin\BlackFace.exe"; DestDir: "{app}\BlackFace"
Source: "..\..\Presets\*"; DestDir: "C:\ProgramData\BlackFace\Presets"
