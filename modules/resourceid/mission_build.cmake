###########################################################
#
# Resource ID mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

if (CFE_EDS_ENABLED_BUILD)
  set(RESOURCEID_HDR_FILE "cfe_resourceid_eds.h")
elseif (MISSION_RESOURCEID_MODE STREQUAL "STRICT")
  set(RESOURCEID_HDR_FILE "cfe_resourceid_strict.h")
else ()
  set(RESOURCEID_HDR_FILE "cfe_resourceid_simple.h")
endif ()

# Generate the header definition files, use local default for this module)
generate_config_includefile(
    FILE_NAME           "cfe_resourceid_typedef.h"
    FALLBACK_FILE       "${CMAKE_CURRENT_LIST_DIR}/option_inc/${RESOURCEID_HDR_FILE}"
)
