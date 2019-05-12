
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
# TO DO: If you want, change the name of the library
LIB1 := $(LIB_DIR)/lib$(MODULE_NAME).a

# TO DO: All the .cpp files, ... or choose the ones you need
L_CPPSRC1 := $(notdir $(wildcard $(SOURCE_DIR)/*.cpp))

# TO DO: All the .c files, ... or choose the ones you need
L_CSRC1 := $(notdir $(wildcard $(SOURCE_DIR)/*.c))

#-----------------------------------------------------------------
all: $(LIB1)
#-----------------------------------------------------------------
L_DEP1     := $(L_CPPSRC1:.cpp=.d) $(L_CSRC1:.c=.d)
L_OBJ1     := $(L_DEP1:.d=.o)

$(LIB1): $(L_DEP1) $(LIB1)($(L_OBJ1)) 
	@-test  -d $(@D) || (echo "Create lib dir: $(@D)"; mkdir -p $(@D) )
	ranlib $(LIB1)

include $(L_DEP1)
#-----------------------------------------------------------------
clean:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(L_DEP1) $(L_OBJ1)
#-----------------------------------------------------------------
clean_all:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(L_DEP1) $(L_OBJ1) $(LIB1)
endif

