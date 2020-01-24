#!/usr/bin/env bash

./Scripts/sync_projucer_version_with_git.sh

xcodebuild -project ./Builds/MacOSX/BlackFace.xcodeproj -scheme "BlackFace - All" build
packagesbuild ./Releases/MacOS/BlackFace.pkgproj 