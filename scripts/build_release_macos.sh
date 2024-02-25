#!/usr/bin/env bash

scripts_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
project_dir="$scripts_dir/../"

"$scripts_dir/sync_projucer_version_with_git.sh"

xcodebuild -project "$project_dir/build/MacOSX/BlackBird.xcodeproj" -scheme "BlackBird - All" -configuration Release build
packagesbuild "$project_dir/Releases/MacOS/BlackBird.pkgproj"