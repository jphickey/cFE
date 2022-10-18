# Makefile for EDS-based CFE table generation

TABLE_BINARY_DIR := $(CURDIR)

# the basic set of flags to pass to the table tool
TBLTOOL_FLAGS += -e MISSION_DEFS=\"$(MISSION_DEFS)\"
TBLTOOL_FLAGS += -e CPUNAME=\"$(CFE_TABLE_CPUNAME)\"
TBLTOOL_FLAGS += -e APPNAME=\"$(CFE_TABLE_APPNAME)\"
TBLTOOL_FLAGS += -e TABLENAME=\"$(notdir $(basename $(@)))\"

SO_CFLAGS = $(addprefix -D,$(C_COMPILE_DEFS)) $(addprefix -I,$(C_INCLUDE_DIRS)) -MMD -fPIC -shared

.PHONY: cfetables

cfetables:
	@echo "Table build completed"

%.so: $(CFE_MAKEFILE_SOURCE)
	$(CC) $(CFLAGS) $(SO_CFLAGS) -o $(@) $(filter %.c,$(^))

%.tbl: $(CFE_MAKEFILE_SOURCE) $(TBLTOOL)
	cd $(dir $(@)) && $(TBLTOOL) $(TBLTOOL_FLAGS) $(CFE_TABLE_CMDLINE)
