#!/usr/bin/env bash
describe=$(git describe)
version=${describe#"v"$"-*"}
version=$(echo "$version" | sed -e 's/\([\-].*\)$//g')

PROJUCER_PATH="/Users/khrykin/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer"

$PROJUCER_PATH --set-version $version ./BlackFace.jucer

