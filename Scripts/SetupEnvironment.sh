#!/bin/bash
set -e
SCRIPTPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

# To override UITesterPublishDir, set environment variable "UITesterPublishDir" to the location of UITester's Published directory before building XCode project.
# You can also mount the Published directory from Windows build and point "UITesterPublishDir" to it.

if [ -z "${CONFIGURATION}" ]; then
    export CONFIGURATION=Debug
    echo "Setting CONFIGURATION=${CONFIGURATION}"
fi

if [ -z "${ENLISTMENTROOT}" ]; then
    export ENLISTMENTROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../" >/dev/null && pwd )"
    echo "Setting ENLISTMENTROOT=${ENLISTMENTROOT}"
fi

if [ -z "${NETCORETARGETFRAMEWORK}" ]; then
    export NETCORETARGETFRAMEWORK="net6.0"
    echo "Setting NETCORETARGETFRAMEWORK=${NETCORETARGETFRAMEWORK}"
fi

if [ -z "${UITesterOutputsDir}" ]; then
    export UITesterOutputsDir="${ENLISTMENTROOT}/bin/${CONFIGURATION}"
    echo "Setting UITesterOutputsDir=${UITesterOutputsDir}"
fi

if [ -z "${UITesterNativeBinDir}" ]; then
    export UITesterNativeBinDir="${UITesterOutputsDir}/Native"
    echo "Setting UITesterNativeBinDir=${UITesterNativeBinDir}"
fi

if [ -z "${UITesterPublishBaseDir}" ]; then
    export UITesterPublishBaseDir="${UITesterOutputsDir}/Publish/Product"
    echo "Setting UITesterPublishBaseDir=${UITesterPublishBaseDir}"
fi

if [ -z "${UITesterPublishDir}" ]; then
    export UITesterPublishDir="${UITesterPublishBaseDir}/UITester.Mac/${NETCORETARGETFRAMEWORK}${RuntimeIdentifierPostfix}"
    echo "Setting UITesterPublishDir=${UITesterPublishDir}"
fi

export VersionInfoFilePath="${UITesterPublishDir}/osx-x64/Version-Info.h"
