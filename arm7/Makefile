# This is based off of the default arm7 code.

export ARM7_MAJOR	:= 0
export ARM7_MINOR	:= 5
export ARM7_PATCH	:= 24

VERSTRING	:=	$(ARM7_MAJOR).$(ARM7_MINOR).$(ARM7_PATCH)
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

TARGET		:=	FSPDS
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include ../include

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv4t -mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -Wno-psabi -O3\
 		-mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer\
		-ffast-math \
		$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM7

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=ds_arm7.specs -g $(ARCH) -Wl,-Map,$(notdir $*).map


LIBS	:= -lfat -lnds7

LIBDIRS	:=	$(LIBNDS)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export OFILES	:=  $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export DEPSDIR	:=	$(CURDIR)/build

export OUTPUT	:=	$(CURDIR)/$(TARGET)


export LD	:=	$(CC)

.PHONY: all $(BUILD) clean

all : $(BUILD)

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile


#---------------------------------------------------------------------------------
dist: all
#---------------------------------------------------------------------------------
	@tar --exclude=*CVS* --exclude=.svn -cvjf default_arm7-src-$(VERSTRING).tar.bz2 source Makefile
	@tar -cvjf default_arm7-$(VERSTRING).tar.bz2 default.elf

#---------------------------------------------------------------------------------
install: all
#---------------------------------------------------------------------------------
	cp $(TARGET).elf $(LIBNDS)

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf  $(TARGET).arm7

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).elf	:	$(OFILES) $(LIBNDS)/lib/libnds7.a

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
