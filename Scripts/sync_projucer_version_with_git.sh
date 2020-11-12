#!/usr/bin/env bash
describe=$(git describe)
version=${describe#v}
version=${version%%-*}
version=$(echo "$version" | sed -e 's/\([\-].*\)$//g')

scripts_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
project_dir="$scripts_dir/../"

[[ -z "$PROJUCER_PATH" ]] && { echo "Error: PROJUCER_PATH not set"; exit 1; }

$PROJUCER_PATH --set-version $version "$project_dir./BlackBird.jucer"

sed -E -i "" "s/AppVersion=[^\n]+/AppVersion=$version/" \
  "$project_dir/Releases/Windows/BlackBird-Win32.iss"

sed -E -i "" "s/AppVersion=[^\n]+/AppVersion=$version/" \
  "$project_dir/Releases/Windows/BlackBird-x64.iss"
	
