#!/usr/bin/env bash
describe=$(git describe)
version=${describe#"v"$"-*"}
version=$(echo "$version" | sed -e 's/\([\-].*\)$//g')

[[ -z "$PROJUCER_PATH" ]] && { echo "Error: PROJUCER_PATH not set"; exit 1; }

$PROJUCER_PATH --set-version $version ./BlackFace.jucer

