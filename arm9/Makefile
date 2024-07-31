#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules


MAKEFILE_PATH:=$(realpath $(shell dirname $(firstword $(MAKEFILE_LIST))))

$(info MAKEFILE_PATH=$(MAKEFILE_PATH))


# Read value of NTFS attribute in ../config.ini (default 0)
USE_NTFS := ${shell grep -E "^NTFS\s*\=\s*.*\s*$$" $(MAKEFILE_PATH)/../config.ini | sed -E "s/^NTFS\s*\=\s*(.*)\s*$$/\1/" | grep . || echo "0"}

$(info Build Configuration)
$(info NTFS Enabled = $(USE_NTFS))

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	FSPDS
BUILD		:=	build
FONTS       :=  fonts
SOURCES		:=	gfx data source ${shell find ${source} -regex ".*source/.*" -type d -print}
INCLUDES	:=	include build
GRAPHICS    :=  assets
DATA        :=  data


# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
ifeq ($(USE_NTFS), 1)
$(info NTFS)
NITRO    := nitrofs
else
$(info NO_NTFS)
NITRO    := 
endif



# $(error x)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH := -marm -mthumb-interwork -march=armv5te -mtune=arm946e-s

CFLAGS   := -g -Wall -O3\
            $(ARCH) $(INCLUDE) -DARM9 -DUSE_NTFS=$(USE_NTFS)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS   = -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS := -lfat -lnds9

# automatigically add libraries for NitroFS
ifneq ($(strip $(NITRO)),)
LIBS := -lfilesystem -lfat $(LIBS)
endif
# automagically add maxmod library
ifneq ($(strip $(AUDIO)),)
LIBS := -lmm9 $(LIBS)
endif

LIBS := $(LIBS)

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS := $(LIBDSC) $(LIBNDS) $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export ARM9ELF	:=	$(CURDIR)/$(TARGET).elf
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export VPATH := $(CURDIR)/$(subst /,,$(dir $(ICON)))\
                $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))\
                $(foreach dir,$(DATA),$(CURDIR)/$(dir))\
                $(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))\
				$(foreach dir,$(FONTS),$(CURDIR)/$(dir))

export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
FONTFILES:= $(foreach dir,$(FONTS),$(notdir $(wildcard $(dir)/*.astralfont)))
PNGFILES := $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

# prepare NitroFS directory
ifneq ($(strip $(NITRO)),)
  export NITRO_FILES := $(CURDIR)/$(NITRO)
endif

# get audio list for maxmod
ifneq ($(strip $(AUDIO)),)
  export MODFILES	:=	$(foreach dir,$(notdir $(wildcard $(AUDIO)/*.*)),$(CURDIR)/$(AUDIO)/$(dir))

  # place the soundbank file in NitroFS if using it
  ifneq ($(strip $(NITRO)),)
    export SOUNDBANK := $(NITRO_FILES)/soundbank.bin

  # otherwise, needs to be loaded from memory
  else
    export SOUNDBANK := soundbank.bin
    BINFILES += $(SOUNDBANK)
  endif
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
  export LD := $(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
  export LD := $(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_FONTS := $(addsuffix .o,$(FONTFILES)) 

export OFILES_BIN   :=	$(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES := $(PNGFILES:.png=.o) $(OFILES_FONTS) $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(PNGFILES:.png=.h) $(addsuffix .h,$(subst .,_,$(BINFILES))) $(addsuffix .h,$(FONTFILES))

export INCLUDE  := $(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir))\
                   $(foreach dir,$(LIBDIRS),-I$(dir)/include)\
                   -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(ICON)),)
  icons := $(wildcard *.bmp)

  ifneq (,$(findstring $(TARGET).bmp,$(icons)))
    export GAME_ICON := $(CURDIR)/$(TARGET).bmp
  else
    ifneq (,$(findstring icon.bmp,$(icons)))
      export GAME_ICON := $(CURDIR)/icon.bmp
    endif
  endif
else
  ifeq ($(suffix $(ICON)), .grf)
    export GAME_ICON := $(CURDIR)/$(ICON)
  else
    export GAME_ICON := $(CURDIR)/$(BUILD)/$(notdir $(basename $(ICON))).grf
  endif
endif

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(SOUNDBANK)

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(ARM9ELF)	:	$(OFILES) 		
	echo $(NITROFS_INC)
	@echo linking $(notdir $@)	
	echo $(NITRO_FILES)
	@$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

# source files depend on generated headers
$(OFILES_SOURCES) : $(HFILES)

# need to build soundbank first
$(OFILES): $(SOUNDBANK)

#---------------------------------------------------------------------------------
# rule to build solution from music files
#---------------------------------------------------------------------------------
$(SOUNDBANK) : $(MODFILES)
#---------------------------------------------------------------------------------
	mmutil $^ -d -o$@ -hsoundbank.h

#---------------------------------------------------------------------------------
%.bin.o %_bin.h : %.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#---------------------------------------------------------------------------------
%.s %.h: %.png
#---------------------------------------------------------------------------------	
	echo $*
	../tools/DSC.Toolchain.AssetBuild.exe -i$< -h$*.h -s$*.s
	# replace #include<> with #include"" because DSC is not an external library in this case
	sed -i 's/[\<\>]/\"/g' $*.h
	
%.astralfont.s %.astralfont.h : %.astralfont
	@echo $(notdir $<)		
	python ../tools/font_header.py $(CURDIR) $(basename $(basename $@))	
	sed -i 's/Astralbrew/DSC/g' $*.astralfont.h
	sed -i 's/_ZN10Astralbrew/_ZN3DSC/g' $*.astralfont.s


-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------