@echo OFF 
devenv "%~dp0..\Builds\VisualStudio2019\BlackFace.sln" /build "Release|x64"
devenv "%~dp0..\Builds\VisualStudio2019\BlackFace.sln" /build "Release|Win32"

iscc "%~dp0..\Releases\Windows\BlackFace-Win32.iss"
iscc "%~dp0..\Releases\Windows\BlackFace-x64.iss"