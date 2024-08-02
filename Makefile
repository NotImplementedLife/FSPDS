#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

MAKEFILE_PATH:=$(realpath $(shell dirname $(firstword $(MAKEFILE_LIST))))
$(info MAKEFILE_PATH=$(MAKEFILE_PATH))

include $(DEVKITARM)/ds_rules

export GAME_TITLE	:=	FSPDS
export GAME_ICON	:=	$(CURDIR)/icon.bmp
export TARGET		:=	FSPDS
export NDS_FILE     := $(TARGET)

USE_NTFS := ${shell grep -E "^NTFS\s*\=\s*.*\s*$$" $(MAKEFILE_PATH)/config.ini | sed -E "s/^NTFS\s*\=\s*(.*)\s*$$/\1/" | grep . || echo "0"}

ifeq ($(USE_NTFS), 1)
$(info NTFS)
NITRO    := nitrofs
else
$(info NO_NTFS)
NITRO    := 
endif


ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

#NITROFS_INC := $(ROOT_DIR)/arm9/nitrofs

#include $(NITROFS_INC)

ifneq ($(strip $(NITRO)),)
NITRO_FILES := arm9/$(NITRO)
endif

.PHONY: arm7/$(TARGET).elf arm9/$(TARGET).elf

TARGET_BUILD_VERSION := ${shell python build_counter.py}

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------

ifneq ($(strip $(NITRO)),)
$(TARGET).nds	:	arm7/$(TARGET).elf arm9/$(TARGET).elf $(NITRO_FILES)
	$(info ndstool -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -b $(GAME_ICON) "$(GAME_TITLE)" -d $(NITRO_FILES) -c $(TARGET).nds)
	@ndstool -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -b $(GAME_ICON) "$(GAME_TITLE)" -d $(NITRO_FILES) -c $(TARGET).nds
	@echo built ... $(notdir $@)
	mv $(TARGET).nds $(TARGET)-V$(TARGET_BUILD_VERSION).nds
else
$(TARGET).nds	:	arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(info ndstool -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -b $(GAME_ICON) "$(GAME_TITLE)" -d $(NITRO_FILES) -c $(TARGET).nds)
	@ndstool -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -b $(GAME_ICON) "$(GAME_TITLE)" -c $(TARGET).nds
	@echo built ... $(notdir $@)
	mv $(TARGET).nds $(TARGET)-V$(TARGET_BUILD_VERSION).nds
endif

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).arm7 $(TARGET).arm9
