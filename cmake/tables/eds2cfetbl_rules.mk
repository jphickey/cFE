# Rules for EDS-based CFE table generation

# the basic set of flags to pass to the table tool
TBLTOOL_FLAGS += -e MISSION_DEFS=\"$(MISSION_DEFS)\"
TBLTOOL_FLAGS += -e CPUNAME=\"$(CFE_TABLE_CPUNAME)\"
TBLTOOL_FLAGS += -e APPNAME=\"$(CFE_TABLE_APPNAME)\"
TBLTOOL_FLAGS += -e TABLENAME=\"$(CFE_TABLE_BASENAME)\"

LOCAL_CFLAGS += $(addprefix -D,$(C_COMPILE_DEFS))
LOCAL_CFLAGS += $(addprefix -I,$(C_INCLUDE_DIRS))
LOCAL_CFLAGS += -DCFE_TABLE_NAME="$(CFE_TABLE_BASENAME)"

eds/%.o:
	@mkdir -pv $(dir $(@))
	$(CC) $(CFLAGS) -fPIC $(LOCAL_CFLAGS) -MMD -c -o $(@) $(<)

%.so: %.o
	$(CC) $(CFLAGS) -shared -o $(@) $(<)
