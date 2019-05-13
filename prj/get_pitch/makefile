
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
# TO DO: Complete the program name,
PROG1 := $(BIN_DIR)/get_pitch

# TO DO: Add the c++ sources (if any). Do not include the path
CPPSRC1  := get_pitch.cpp  pitch_analyzer.cpp

# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS1 :=

#-----------------------------------------------------------------
# TO DO: Complete the program name,
PROG2 := $(BIN_DIR)/pitch_evaluate

# TO DO: Add the c++ sources (if any). Do not include the path
CPPSRC2  := pitch_evaluate.cpp

# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS2 :=

#-----------------------------------------------------------------
all: $(PROG1) $(PROG2)
#-----------------------------------------------------------------
DEP1     := $(CPPSRC1:.cpp=.d) $(CSRC1:.c=.d)
OBJ1     := $(DEP1:.d=.o)

$(PROG1): $(DEP1) $(OBJ1) $(LDLIBS1) $(LDLIBS)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	$(CXX) $(LDDFLAGS) $(OBJ1) $(LDLIBS1) $(LDLIBS) -o $@

include $(DEP1)
#-----------------------------------------------------------------
DEP2     := $(CPPSRC2:.cpp=.d) $(CSRC2:.c=.d)
OBJ2     := $(DEP2:.d=.o)

$(PROG2): $(DEP2) $(OBJ2) $(LDLIBS2) $(LDLIBS)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	$(CXX) $(LDDFLAGS) $(OBJ2) $(LDLIBS2) $(LDLIBS) -o $@

include $(DEP2)
#-----------------------------------------------------------------
clean:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(DEP1) $(OBJ1)
	-rm -f $(DEP2) $(OBJ2)
#-----------------------------------------------------------------
clean_all:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(DEP1) $(OBJ1) $(PROG1)
	-rm -f $(DEP2) $(OBJ2) $(PROG2)
endif

