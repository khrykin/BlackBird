devenv "%~dp0..\builds\VisualStudio2019\BlackBird.sln" /build "Release|x64"
devenv "%~dp0..\builds\VisualStudio2019\BlackBird.sln" /build "Release|Win32"

iscc "%~dp0..\releases\Windows\BlackBird-Win32.iss"
iscc "%~dp0..\releases\Windows\BlackBird-x64.iss"