
SHELL = /bin/sh

# If ROOT_DIR is not defined try to guess it here
ifndef ROOT_DIR
   export ROOT_DIR := $(shell pwd | sed 's/\/prj\/.*//')
endif

export ROOT_SOURCE := $(ROOT_DIR)/prj
include $(ROOT_SOURCE)/Make_aux

#------------------------------------------------------------------------------------
# This part of the makefile only is used when executed from the target directory
# Make_aux remakes it from there (and set/unset THIS_IS_A_SRC_DIR)
#------------------------------------------------------------------------------------

ifndef THIS_IS_A_SRC_DIR


#------------------------------------------------------------------------------------
# TO DO: Complete the script name
SCRIPT1 := $(BIN_DIR)/make_wizard

# TO DO: write the name of the source script
SCRIPTSRC1 := make_wizard.pl

#-----------------------------------------------------------------
all: $(SCRIPT1)
#-----------------------------------------------------------------

$(SCRIPT1): $(SCRIPTSRC1)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC1) $(@F) 

#-----------------------------------------------------------------
clean:
	-rm -f $(SOURCE_DIR)/*~
#-----------------------------------------------------------------
clean_all:
	-rm -f $(SOURCE_DIR)/*~
endif

