
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
# TO DO: Change the name of the program if you want
PROG1 := $(BIN_DIR)/$(MODULE_NAME)

# TO DO: All the .cpp files, ... or choose the ones you need
CPPSRC1  := $(notdir $(wildcard $(SOURCE_DIR)/*.cpp))

# TO DO: All the .c files, ... or choose the ones you need
CSRC1 := $(notdir $(wildcard $(SOURCE_DIR)/*.c))

# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS1 :=

#-----------------------------------------------------------------
all: $(PROG1)
#-----------------------------------------------------------------
DEP1     := $(CPPSRC1:.cpp=.d) $(CSRC1:.c=.d)
OBJ1     := $(DEP1:.d=.o)

$(PROG1): $(DEP1) $(OBJ1) $(LDLIBS1) $(LDLIBS)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	$(CXX) $(LDDFLAGS) $(OBJ1) $(LDLIBS1) $(LDLIBS) -o $@

include $(DEP1)
#-----------------------------------------------------------------
clean:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(DEP1) $(OBJ1)
#-----------------------------------------------------------------
clean_all:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(DEP1) $(OBJ1) $(PROG1)
endif

