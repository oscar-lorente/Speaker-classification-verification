
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
SCRIPT1 := $(BIN_DIR)/run_spkid

# TO DO: write the name of the source script
SCRIPTSRC1 := run_spkid.sh

#-----------------------------------------------------------------
# TO DO: Complete the script name
SCRIPT2 := $(BIN_DIR)/unsort

# TO DO: write the name of the source script
SCRIPTSRC2 := unsort.sh

#-----------------------------------------------------------------
# TO DO: Complete the script name
SCRIPT3 := $(BIN_DIR)/wav2lpcc

# TO DO: write the name of the source script
SCRIPTSRC3 := wav2lpcc.sh

#-----------------------------------------------------------------
# TO DO: Complete the script name
SCRIPT4 := $(BIN_DIR)/wav2lp

# TO DO: write the name of the source script
SCRIPTSRC4 := wav2lp.sh

#-----------------------------------------------------------------
# TO DO: Complete the script name
SCRIPT5 := $(BIN_DIR)/spk_verif_score

# TO DO: write the name of the source script
SCRIPTSRC5 := spk_verif_score.pl

#-----------------------------------------------------------------
# TO DO: Complete the script name
#SCRIPT6 := $(BIN_DIR)/wav2mfcc

# TO DO: write the name of the source script
#SCRIPTSRC6 := wav2mfcc.sh


#-----------------------------------------------------------------
all: $(SCRIPT1) $(SCRIPT2) $(SCRIPT3) $(SCRIPT4) $(SCRIPT5) 
# add $(SCRIPT6) $(SCRIPT1) 
#-----------------------------------------------------------------

$(SCRIPT1): $(SCRIPTSRC1)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC1) $(@F) 

#-----------------------------------------------------------------

$(SCRIPT2): $(SCRIPTSRC2)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC2) $(@F) 

#-----------------------------------------------------------------
$(SCRIPT3): $(SCRIPTSRC3)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC3) $(@F) 

#-----------------------------------------------------------------
$(SCRIPT4): $(SCRIPTSRC4)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC4) $(@F) 
#-----------------------------------------------------------------
$(SCRIPT5): $(SCRIPTSRC5)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC5) $(@F) 
#-----------------------------------------------------------------
$(SCRIPT6): $(SCRIPTSRC6)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	cd $(@D); ln -fs $(SOURCE_DIR)/$(SCRIPTSRC6) $(@F) 
#-----------------------------------------------------------------
clean:
	-rm -f $(SOURCE_DIR)/*~
#-----------------------------------------------------------------
clean_all:
	-rm -f $(SOURCE_DIR)/*~
endif

