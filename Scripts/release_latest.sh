#!/usr/bin/env bash

Scripts/release.sh "$(Scripts/repo_name.sh)" "$(git describe)" -- "./Releases/MacOS/build/BlackFace.pkg"