export TARGET = native
export ARCHS = $(shell uname -p)
include $(THEOS)/makefiles/common.mk

TOOL_NAME = fscod fsdec
fscod_FILES = src/maincod.c src/fscodec.c src/fscod.c src/fscodec2.c
fsdec_FILES = src/maindec.c src/fscodec.c src/fsdec.c src/fs_file.c src/rd_file.c
# For desktop
fsdec_FILES += src/fscodec2.c src/fsdec2.c

OUTPUT_FILE = output.fs

include $(THEOS_MAKE_PATH)/tool.mk

all::
	@rm -r obj

test: all
	@$(or $(THEOS_OBJ_DIR),./)/$(firstword $(TOOL_NAME)) test/ $(OUTPUT_FILE)
	@echo ---; cat $(OUTPUT_FILE); echo ...
	@$(or $(THEOS_OBJ_DIR),./)/$(lastword $(TOOL_NAME)) $(OUTPUT_FILE)
