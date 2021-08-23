TpaListFileName="${TARGET_BUILD_DIR}/$1"
echo "Creating ${TpaListFileName}"
[ -f "${TpaListFileName}" ] && rm -f "${TpaListFileName}"

pushd "${TARGET_BUILD_DIR}/${SHARED_SUPPORT_FOLDER_PATH}"

DLL_EXT="dll"
DLL_NI_EXT="ni.dll"
DLL_EXT_LEN=${#DLL_EXT}
DLL_NI_EXT_LEN=${#DLL_NI_EXT}

addBinaries () {

    # Add all ni.dll assemblies
    echo "Adding *.ni.dll"

    FIND_COMMAND="find $1 $2 -type f -iname \"*.${DLL_NI_EXT}\""

    for FILE in $(eval $FIND_COMMAND)
    do
        echo "\"${FILE:2}\"," >> ${TpaListFileName}
        echo "Adding ${FILE:2}"
    done

    # Add all .dll assemblies
    echo "Adding *.dll"

    FIND_COMMAND="find $1 $2 -type f \( -iname \""*.${DLL_EXT}"\" -and -not -iname \""*.${DLL_NI_EXT}"\" \)"

    for FILE in $(eval $FIND_COMMAND)
    do
        # Check if this is a dylib
        if [ ${FILE:0:2} == "./" ]
        then
            # Remove "./" prefix
            FILE=${FILE:2}
        fi

        length=${#FILE}

        echo "Adding ${FILE}"

        # Verify corresponding ni.dll file doesn't exist already
        NI_FILE="${FILE::${length}-${DLL_EXT_LEN}}${DLL_NI_EXT}"
        [ -f "${NI_FILE}" ] || echo "\"${FILE}\"," >> ${TpaListFileName}
    done
}

addArchBinaries () {
    for DIR in $(find . -type d -name $1)
    do
        addBinaries "$DIR"
    done 
}

echo "Adding universal binaries"

addBinaries "." "-type d \( -name osx-arm64 -o -name osx-x64 \) -prune -false -o"

echo "Adding ARM64 binaries"

echo "#if TARGET_CPU_ARM64" >> ${TpaListFileName}

addArchBinaries "osx-arm64"

echo "Adding x64 binaries"

echo "#else // TARGET_CPU_ARM64" >> ${TpaListFileName}

addArchBinaries "osx-x64"

echo "#endif // TARGET_CPU_ARM64" >> ${TpaListFileName}

popd
