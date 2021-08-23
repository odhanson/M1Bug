source "${ENLISTMENTROOT}/Scripts/SetupEnvironment.sh"

INCLUDE_FILES_WITH_EXTENSIONS=('*.dll' '*.dylib' '*.dwarf')

if [ "${CONFIGURATION}" = "Debug" ]; then
	INCLUDE_FILES_WITH_EXTENSIONS+=('*.pdb' '*.dSYM')
fi

for FILE_EXTENSION in "${INCLUDE_FILES_WITH_EXTENSIONS[@]}"; do
	INCLUDE_FILES_OPTION+=\ --include=$FILE_EXTENSION
done

rsync -av $INCLUDE_FILES_OPTION --exclude=*.* "${UITesterPublishDir}/" "${TARGET_BUILD_DIR}/${SHARED_SUPPORT_FOLDER_PATH}"

pushd ${UITesterPublishDir}
for FILE in $(eval "find . -type f ${INCLUDE_FILES_WITH_EXTENSIONS}")
do 
    if [ ! -f "${UITesterPublishDir}/${FILE}" ]
    then
        echo "Copying ${FILE} into ${SHARED_SUPPORT_FOLDER_PATH}."
        rsync -avR ${FILE} "${TARGET_BUILD_DIR}/${SHARED_SUPPORT_FOLDER_PATH}"
    fi
done

popd

