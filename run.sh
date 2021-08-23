#!/bin/bash
set -e
SCRIPTPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

# Overide CONFIGURATION with script argument
[ -z "$1" ] || export CONFIGURATION="$1"

. "${SCRIPTPATH}/Scripts/SetupEnvironment.sh"

${UITesterNativeBinDir}/UITester.Mac.app/Contents/MacOS/UITester.Mac