#!/bin/bash

source "${ENLISTMENTROOT}/Scripts/SetupEnvironment.sh"

# TODO (VSO5290452): Remove once M1 is officially released
if [ "$NETCORETARGETFRAMEWORK" == "net5.0" ]
then
    if ! [[ -z "${EXPANDED_CODE_SIGN_IDENTITY}" ]]
    then
        # Convert to Universal and Codesign all Mach-O executables in SharedSupport folder
        echo "Convert to Universal and Codesign all Mach-O executables in ${SHARED_SUPPORT_FOLDER_PATH} folder"
        for FILE in ${TARGET_BUILD_DIR}/${SHARED_SUPPORT_FOLDER_PATH}/*.dylib
        do
            if [ -f "${FILE}" ]
            then
                # Create universal binaries only if apple silicon stub exists
                if [ -f "${UITesterNativeBinDir}/libAppleSiliconStubLib.dylib" ]
                then
                    ARCH=$(lipo -archs ${FILE})
                    if [ "$ARCH" = "x86_64" ]
                    then
                        echo "${FILE} is x64. Converting into universal"
                        lipo -create ${UITesterNativeBinDir}/libAppleSiliconStubLib.dylib ${FILE} -output ${FILE}
                    else
                        echo "${FILE} is already universal"
                    fi
                fi

                echo "Signing ${FILE}"
                /usr/bin/codesign --force --sign $EXPANDED_CODE_SIGN_IDENTITY --preserve-metadata=identifier,entitlements,flags --timestamp=none $FILE
            fi
        done

        # Create universal binaries only if apple silicon stub exists
        if [ -f "${UITesterNativeBinDir}/AppleSiliconStubApp" ]
        then
            # Convert to Universal and Codesign all Mach-O executables in Executable folder
            echo "Convert to Universal and Codesign all Mach-O executables in ${EXECUTABLE_FOLDER_PATH} folder"
            for FILE in ${TARGET_BUILD_DIR}/${EXECUTABLE_FOLDER_PATH}/*
            do
                if [ -f "${FILE}" ]
                then
                    ARCH=$(lipo -archs ${FILE})
                    if [ "$ARCH" = "x86_64" ]
                    then
                        echo "${FILE} is x64. Converting into universal"
                        lipo -create ${UITesterNativeBinDir}/AppleSiliconStubApp ${FILE} -output ${FILE}
                    else
                        echo "${FILE} is already universal"
                    fi

                    echo "Signing ${FILE}"
                    /usr/bin/codesign --force --sign $EXPANDED_CODE_SIGN_IDENTITY --preserve-metadata=identifier,entitlements,flags --timestamp=none $FILE
                fi
            done
        fi
    fi
else
    # Codesign all Mach-O executables in SharedSupport folder
    if ! [[ -z "${EXPANDED_CODE_SIGN_IDENTITY}" ]]
    then
        for FILE in ${TARGET_BUILD_DIR}/${SHARED_SUPPORT_FOLDER_PATH}/*.dylib
        do
            if [ -f "${FILE}" ]
            then
                echo "Signing ${FILE}"
                /usr/bin/codesign --force --sign $EXPANDED_CODE_SIGN_IDENTITY --preserve-metadata=identifier,entitlements,flags --timestamp=none $FILE
            fi
        done
    fi
fi