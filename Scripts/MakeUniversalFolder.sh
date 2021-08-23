#!/bin/bash -e
SCRIPTPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

OSX_PATH=$PWD
OSX_X64_PATH=$PWD/osx-x64
OSX_ARM64_PATH=$PWD/osx-arm64

if [ ! -d $OSX_X64_PATH ]
then
  exit 0
fi

pushd $OSX_X64_PATH

function cleanup {
    popd
}

trap cleanup EXIT

for FILE in $(find . -type f)
do 
    ARM64_FILE="${OSX_ARM64_PATH}/${FILE}"
    if [ ! -f "${ARM64_FILE}" ]
    then
        echo "${FILE} does not exist."
        continue
    fi

    if cmp -s "${FILE}" "${ARM64_FILE}"
    then
        echo "${OSX_X64_PATH}/${FILE} and ${ARM64_FILE} are identical. Merging them to ${OSX_PATH}"
        rsync -avR "${FILE}" "${OSX_PATH}" > /dev/null
        rm -rf "${FILE}"
        rm -f "${ARM64_FILE}"
    else
        if [ -f "${FILE}" ]
        then
            # Check if this is a dylib
            if [ ${FILE: -6} == ".dylib" ]
            then
                ARCH=$(lipo -archs ${FILE})
                if [ ! "$ARCH" = "x86_64" ]
                then
                    echo "${FILE} should be x64"
                    exit 1
                fi
                ARCH=$(lipo -archs ${ARM64_FILE})
                if [ ! "$ARCH" = "arm64" ]
                then
                    echo "${ARM64_FILE} should be arm64"
                    exit 1
                fi
                echo "${FILE} and ${ARM64_FILE} are different. Making them universal"
                lipo -create ${ARM64_FILE} ${FILE} -output "${OSX_PATH}/${FILE}"
                rm -rf "${FILE}"
                rm -f "${ARM64_FILE}"
            else
                echo "${FILE} and ${ARM64_FILE} are different. Leaving them where they are"
            fi
        fi

    fi
done

cd ${OSX_PATH}

find . -type d -empty -delete
