#!/bin/bash
set -e
SCRIPTPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

# Overide CONFIGURATION with script argument
[ -z "$1" ] || export CONFIGURATION="$1"

. "${SCRIPTPATH}/Scripts/SetupEnvironment.sh"

# Clean first
/usr/bin/xcodebuild -sdk macosx11.3 -configuration ${CONFIGURATION} -arch x86_64 -arch arm64 ONLY_ACTIVE_ARCH=NO -workspace ${ENLISTMENTROOT}/MacNative.xcodeproj/project.xcworkspace -scheme BuildAll clean

dotnet publish --interactive --self-contained -r osx-arm64 -o ${UITesterPublishDir} ${ENLISTMENTROOT}/src/Product/UITester.Mac/UITester.Mac.csproj

/usr/bin/xcodebuild -sdk macosx11.3 -configuration ${CONFIGURATION} -arch x86_64 -arch arm64 ONLY_ACTIVE_ARCH=NO -workspace ${ENLISTMENTROOT}/MacNative.xcodeproj/project.xcworkspace -scheme BuildAll build CODE_SIGN_STYLE=Auto 
