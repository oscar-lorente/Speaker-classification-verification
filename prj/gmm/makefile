
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
PROG1 := $(BIN_DIR)/gmm_train

# TO DO: Add the c++ sources (if any). Do not include the path
CPPSRC1  := gmm_train.cpp

# TO DO: Add the c sources (if any). Do not include the path
CSRC1 :=

# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS1 :=

#-----------------------------------------------------------------
# TO DO: Complete the program name,
PROG2 := $(BIN_DIR)/gmm_classify

# TO DO: Add the c++ sources (if any). Do not include the path
CPPSRC2  := gmm_classify.cpp 


# TO DO: Add the c sources (if any). Do not include the path
CSRC2 :=

# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS2 :=

#-----------------------------------------------------------------
# TO DO: Complete the program name,
PROG3 := $(BIN_DIR)/gmm_show

# TO DO: Add the c++ sources (if any). Do not include the path
CPPSRC3  := gmm_show.cpp 


# TO DO: Add the c sources (if any). Do not include the path
CSRC3 :=

# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS3 :=
#-----------------------------------------------------------------
PROG4 := $(BIN_DIR)/gmm_verify
CPPSRC4  := gmm_verify.cpp 
LDLIBS4 :=

#-----------------------------------------------------------------
all: $(PROG1) $(PROG2) $(PROG3) # $(PROG4) 
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
DEP3     := $(CPPSRC3:.cpp=.d) $(CSRC3:.c=.d)
OBJ3     := $(DEP3:.d=.o)

$(PROG3): $(DEP3) $(OBJ3) $(LDLIBS3) $(LDLIBS)
	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
	$(CXX) $(LDDFLAGS) $(OBJ3) $(LDLIBS3) $(LDLIBS) -o $@

include $(DEP3)
#-----------------------------------------------------------------
DEP4     := $(CPPSRC4:.cpp=.d) $(CSRC4:.c=.d)
OBJ4     := $(DEP4:.d=.o)

#-----------------------------------------------------------------
#$(PROG4): $(DEP4) $(OBJ4) $(LDLIBS4) $(LDLIBS)
#	@-test  -d $(@D) || (echo "Create bin dir: $(@D)"; mkdir -p $(@D) )
#	$(CXX) $(LDDFLAGS) $(OBJ4) $(LDLIBS4) $(LDLIBS) -o $@

# include $(DEP4)
#-----------------------------------------------------------------
clean:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(DEP1) $(OBJ1)
	-rm -f $(DEP2) $(OBJ2)
	-rm -f $(DEP3) $(OBJ3)
#	-rm -f $(DEP4) $(OBJ4)
#-----------------------------------------------------------------
clean_all:
	-rm -f $(SOURCE_DIR)/*~
	-rm -f $(DEP1) $(OBJ1) $(PROG1)
	-rm -f $(DEP2) $(OBJ2) $(PROG2)
	-rm -f $(DEP3) $(OBJ3) $(PROG3)
#	-rm -f $(DEP4) $(OBJ4) $(PROG4)
endif

