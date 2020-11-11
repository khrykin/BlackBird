#!/usr/bin/env bash

Scripts/release.sh "$(Scripts/repo_name.sh)" \
"$(git describe)" -- \
 "./Releases/MacOS/build/BlackBird.pkg" \
 "./Releases/Windows/Output/BlackBird-windows-x64.exe" \
 "./Releases/Windows/Output/BlackBird-windows-x86.exe"