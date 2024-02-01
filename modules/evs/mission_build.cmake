###########################################################
#
# EVS Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the EVS configuration
set(EVS_MISSION_CONFIG_FILE_LIST
  cfe_evs_mission_cfg.h
  cfe_evs_interface_cfg.h
  cfe_evs_extern_typedefs.h
  cfe_evs_fcncodes.h
  cfe_evs_msgdefs.h
  cfe_evs_msg.h
  cfe_evs_msgstruct.h
  cfe_evs_topicids.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(EVS_CFGFILE_SRC_cfe_evs_fcncodes        "cfe_evs_eds_cc.h")
  set(EVS_CFGFILE_SRC_cfe_evs_msgstruct       "cfe_evs_eds_typedefs.h")
  set(EVS_CFGFILE_SRC_cfe_evs_msgdefs         "cfe_evs_eds_typedefs.h")
  set(EVS_CFGFILE_SRC_cfe_evs_interface_cfg   "cfe_evs_eds_designparameters.h")
  set(EVS_CFGFILE_SRC_cfe_evs_topicids        "cfe_mission_eds_designparameters.h")

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(EVS_CFGFILE ${EVS_MISSION_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${EVS_CFGFILE}" NAME_WE)
  if (DEFINED EVS_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${EVS_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${EVS_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${EVS_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
