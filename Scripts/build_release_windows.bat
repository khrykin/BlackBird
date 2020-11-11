devenv "%~dp0..\Builds\VisualStudio2019\BlackBird.sln" /build "Release|x64"
devenv "%~dp0..\Builds\VisualStudio2019\BlackBird.sln" /build "Release|Win32"

iscc "%~dp0..\Releases\Windows\BlackBird-Win32.iss"
iscc "%~dp0..\Releases\Windows\BlackBird-x64.iss"