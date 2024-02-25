#!/usr/bin/env bash

Scripts/release.sh "$(Scripts/repo_name.sh)" \
"$(git describe)" -- \
 "./releases/MacOS/build/BlackBird.pkg" \
 "./releases/Windows/Output/BlackBird-windows-x64.exe" \
 "./releases/Windows/Output/BlackBird-windows-x86.exe"