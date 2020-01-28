#!/usr/bin/env bash

Scripts/release.sh "$(Scripts/repo_name.sh)" \
"$(git describe)" -- \
 "./Releases/MacOS/build/BlackFace.pkg" \
 "./Releases/Windows/Output/BlackFace-windows-x64.exe" \
 "./Releases/Windows/Output/BlackFace-windows-x86.exe"