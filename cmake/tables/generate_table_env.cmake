##################################################################
#
# Sub-script to capture the table compile/generation environment
#
# This small script runs at build time (as opposed to prep time)
# which captures a set of environment metadata
#
# It must be done this way such that generator expressions will
# be evaluated now, during the arch build process, rather than
# deferring the evaluation to the parent build where they may
# have different values.
#
##################################################################

# In case the INCLUDE_DIRS is a CMake list, convert to a space-separated list
string(REPLACE ";" " " INCLUDE_DIRS "${INCLUDE_DIRS}")
string(REPLACE ";" " " COMPILE_DEFS "${COMPILE_DEFS}")

set(TABLE_BINARY "${TGT}/${INSTALL_SUBDIR}/${TABLE_NAME}.tbl")

if (TBL_SRC)
    set(TABLE_LEGACY_SO "legacydata_${TGT}_${TABLE_NAME}.so")
    string(CONCAT TBL_CSRC_RULE
        "${TABLE_LEGACY_SO}: C_INCLUDE_DIRS += ${INCLUDE_DIRS}\n"
        "${TABLE_LEGACY_SO}: C_COMPILE_DEFS += ${COMPILE_DEFS}\n"
        "${TABLE_LEGACY_SO}: ${TBL_SRC}\n"
        "${TABLE_BINARY}: CFE_TABLE_CMDLINE += \$(TABLE_BINARY_DIR)/${TABLE_LEGACY_SO}\n"
        "${TABLE_BINARY}: ${TABLE_LEGACY_SO}\n"
    )
endif()
if (TBL_LUA)
    string(CONCAT TBL_LUASRC_RULE
        "${TABLE_BINARY}: CFE_TABLE_CMDLINE += ${TBL_LUA}\n"
        "${TABLE_BINARY}: ${TBL_LUA}\n"
    )
endif()

configure_file(${TEMPLATE_FILE} ${OUTPUT_FILE})
