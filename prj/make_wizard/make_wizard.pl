#!/usr/bin/perl -w


#================================================================================
#
#  Copyright (C) Universitat Politècnica de Catalunya, Barcelona, Spain.
#
#  Permission to copy, use, modify, sell and distribute this software
#  is granted.
#  This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.
#
#  Antonio Bonafonte
#  Barcelona, 2004
#================================================================================




use Getopt::Std;
#use strict;
$opt_p = 0;
$opt_l = 0;
$opt_m = 0;
$opt_M = 0;
$opt_s = 0;
$opt_c = 0;
$opt_C = 0;
$opt_d = "prj";
$opt_f = "makefile";
$opt_a = 0;

sub syntax {
print<<EOF;

  Usage: $0 [options]

  Where options are:
      -p num      number of programs  to generate
      -l num      number of libraries to generate
      -m num      number of dynamic modules to generate
      -s num      number of scripts to link in the bin dir
      -M modules  generate a simple makefile just to make subdirectories
      -c          do not include lines for "c" sources
      -C          do not include lines for "c++" sources
      -d src_dir  (def. $opt_d) directory with the name of the root_src directory.
      -f filename name for the output file (def. $opt_f)
      -a          generate de default Make.aux file in current directory
                  the directory for this file is the root_src one

At least one of the -a, -p, -l, -m, -M  options must be given.

It you want to start your source tree  in "/home/your_name/your_soft/prj" just use
the word "prj" as "src_dir".

You will be able to move the whole structure 
(for instance to /home/your_friend/oldstuff/you_soft).

Just one warning:
     DON'T create other directory with the same src_dir name inside.
     Ex: /home/your_name/your_soft/prj/stuff/prj/... !!!

EOF
}

sub create_makeaux {
    open(MAKEAUX, ">Make_aux")
	or die "Error opening output file Make.aux\n";

    print MAKEAUX <<'EOF';
#================================================================================
#
#  Copyright (C) Universitat Politècnica de Catalunya, Barcelona, Spain.
# 
#  Permission to copy, use, modify, sell and distribute this software
#  is granted.
#  This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.
# 
#  Antonio Bonafonte
#  Barcelona, 2004
#================================================================================

CXX	  := g++
CC	  := gcc

#------------------------------------------------------------------------------------
SHELL = /bin/sh
THIS_DIR    := $(shell pwd)

ROOT_OBJ    := $(ROOT_DIR)/intermediate
ROOT_LIB    := $(ROOT_DIR)/lib
ROOT_BIN    := $(ROOT_DIR)/bin
ROOT_MOD    := $(ROOT_DIR)/modules
#------------------------------------------------------------------------------------
ARCH        := $(shell uname --all | tr ' ' '\n' | grep ^i*.86$ | head -1)
#------------------------------------------------------------------------------------

ifneq (,$(findstring $(ROOT_SOURCE),$(THIS_DIR)))
#------------------------------------------------------------------------------------
# We are in the source directory; change to the build one and make from there
# Redo with the same makefile from the target directory $(BUILD_DIR)
#------------------------------------------------------------------------------------

  THIS_IS_A_SRC_DIR   := "yep!"

  export SOURCE_DIR   := $(THIS_DIR)
  MAKEFILE := $(THIS_DIR)/makefile


.PHONY: help all debug release clean clean_debug clean_release \
        clean_all clean_all_debug clean_all_release

#------------------------------------------------------------------------------------
# Choose your favorite default rule, the one made by make without arguments
default_rule: help

help:
	@echo " "
	@echo "------------------------------------------------------------------------"
	@echo "Use:"
	@echo '  make release       : create "bin-&-lib"  release version'
	@echo '  make debug         : create "bin-&-lib"  debug version '
	@echo "  make all           : make debug and release  "
	@echo " "
	@echo '  make clean_release : remove the "release" intermediate files'
	@echo '  make clean_debug   : remove the "debug"   intermediate files'
	@echo '  make clean         : make clean_debug and clean_release'
	@echo " "
	@echo '  make clean_all     : remove also the "bin-&-lib" for both '
	@echo '                       debug and release'
	@echo " "
	@echo '  make help          : show this menu'
	@echo "------------------------------------------------------------------------"
	@echo " "


all: debug release
clean: clean_release clean_debug
clean_all: clean_all_release clean_all_debug

release clean_release clean_all_release: BUILD_DIR := \
       $(patsubst $(ROOT_SOURCE)/%,$(ROOT_OBJ)/%/release,$(THIS_DIR))

debug clean_debug clean_all_debug:   BUILD_DIR := \
       $(patsubst $(ROOT_SOURCE)/%,$(ROOT_OBJ)/%/debug,$(THIS_DIR))


release:
	@-test  -d $(BUILD_DIR) || \
	(echo "Create $(BUILD_DIR)"; mkdir -p $(BUILD_DIR) )
	@-test  -d $(ROOT_BIN)/release || \
	(echo "Create $(ROOT_BIN)/release)"; mkdir -p $(ROOT_BIN)/release )
	@-test  -d $(ROOT_LIB)/release || \
	(echo "Create $(ROOT_LIB)/release)"; mkdir -p $(ROOT_LIB)/release )
	@echo " "
	@echo " "
	@echo "=============================================================="
	@echo "Make release from $(BUILD_DIR)"
	@echo "--------------------------------------------------------------"
	$(MAKE)   -C $(BUILD_DIR) -f $(MAKEFILE)
	@echo " "

debug:
	@-test  -d $(BUILD_DIR) || \
	(echo "Create $(BUILD_DIR)"; mkdir -p $(BUILD_DIR) )
	@-test  -d $(ROOT_BIN)/debug || \
	(echo "Create lib/debug)"; mkdir -p $(ROOT_BIN)/debug )
	@-test  -d $(ROOT_LIB)/debug || \
	(echo "Create lib/debug)"; mkdir -p $(ROOT_LIB)/debug )
	@echo " "
	@echo " "
	@echo "=============================================================="
	@echo "Make debug from $(BUILD_DIR)"
	@echo "--------------------------------------------------------------"
	@$(MAKE)   -C $(BUILD_DIR) -f $(MAKEFILE)
	@echo "--------------------------------------------------------------"
	@echo " "


clean_debug clean_release:
	@-test  -d $(BUILD_DIR) || \
	(echo "Create $(BUILD_DIR)"; mkdir -p $(BUILD_DIR) )
	@echo " "
	@echo " "
	@echo "=============================================================="
	@echo "Make clean in $(BUILD_DIR)"
	@echo "--------------------------------------------------------------"
	@$(MAKE)   -C $(BUILD_DIR) -f $(MAKEFILE) clean
	@echo "--------------------------------------------------------------"
	@echo " "

clean_all_debug clean_all_release:
	@-test  -d $(BUILD_DIR) || \
	(echo "Create $(BUILD_DIR)"; mkdir -p $(BUILD_DIR) )
	@echo " "
	@echo " "
	@echo "=============================================================="
	@echo "Make clean_all from $(BUILD_DIR)"
	@echo "--------------------------------------------------------------"
	@$(MAKE)   -C $(BUILD_DIR) -f $(MAKEFILE) clean_all
	@echo "--------------------------------------------------------------"
	@echo " "


else
#------------------------------------------------------------------------------------
# We are in the target directory; 
# Define compile-related variables
#------------------------------------------------------------------------------------


#------------------------------------------------------------------------------------
# Rules to generate dependencies files
#------------------------------------------------------------------------------------
%.d: %.cpp
	@echo  Generating dependency file: $@
	@set -e; $(CXX) $(CXXFLAGS) -MM $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@          

%.d: %.c
	@echo  Generating dependency file: $@
	@set -e; $(CC) $(CFLAGS) -MM $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@          

#------------------------------------------------------------------------------------



#------------------------------------------------------------------------------------
# C and C++ compiler and flags and BIN, LIB and OBJ dir, depending of the keyword
# '/debug/' in the present path.
#------------------------------------------------------------------------------------



# Preprocessor flag: where to find includes, define macros, etc.
CPPFLAGS  = -Wall -I$(SOURCE_DIR) -I$(SOURCE_DIR)/../include 


# Generic libraries (to avoid include a very generic one in all the projects)
LDLIBS    := -lpav -lm #-lpthread  -ldl

ifneq (,$(findstring /debug,$(THIS_DIR)))
  BIN_DIR  := $(ROOT_BIN)/debug
  LIB_DIR  := $(ROOT_LIB)/debug
  MOD_DIR  := $(ROOT_MOD)/debug
  LDDFLAGS := -rdynamic -Wl,--rpath,$(LIB_DIR),--rpath,$(MOD_DIR) -L$(LIB_DIR) -L$(MOD_DIR)
  CFLAGS   += -g -O0 -fPIC -fno-inline
  CXXFLAGS += -g -O0 -fno-inline
  CPPFLAGS += -Wall -D_DEBUG -DSTDC_HEADERS -pthread -fPIC -DMOD_DIR=\"$(MOD_DIR)\"
  VPATH        := $(SOURCE_DIR):$(LIB_DIR)
else
  BIN_DIR  := $(ROOT_BIN)/release
  LIB_DIR  := $(ROOT_LIB)/release
  MOD_DIR  := $(ROOT_MOD)/release
  LDDFLAGS := -rdynamic -Wl,--rpath,$(LIB_DIR),--rpath,$(MOD_DIR) -L$(LIB_DIR) -L$(MOD_DIR)
  CFLAGS   += -O3 -fPIC -finline-functions -Wno-inline
  CXXFLAGS += -O3 -finline-functions -Wno-inline
  CPPFLAGS += -Wall -DNDEBUG -DSTDC_HEADERS -pthread -fPIC -DMOD_DIR=\"$(MOD_DIR)\"
  VPATH        := $(SOURCE_DIR):$(LIB_DIR)
endif

MODULE_NAME  := $(notdir $(SOURCE_DIR))

endif


EOF
    close(MAKEAUX);
}

sub hline {
print MAKE "#-----------------------------------------------------------------\n"
}

sub print_prog_vars {
  my $i = shift;
  if ($wildcards) {

    print MAKE<<EOF;
# TO DO: Change the name of the program if you want
PROG$i := \$(BIN_DIR)/\$(MODULE_NAME)

EOF

  } else {

    print MAKE<<EOF;
# TO DO: Complete the program name,
PROG$i := \$(BIN_DIR)/

EOF
  }

  if (!$opt_C) {
    if ($wildcards) {
      print MAKE<<EOF;
# TO DO: All the .cpp files, ... or choose the ones you need
CPPSRC$i  := \$(notdir \$(wildcard \$(SOURCE_DIR)/\*.cpp))

EOF
    } else {
      print MAKE<<EOF;
# TO DO: Add the c++ sources (if any). Do not include the path
CPPSRC$i  :=

EOF

    }
  }

  if (!$opt_c) {
    if ($wildcards) {
      print MAKE<<EOF;
# TO DO: All the .c files, ... or choose the ones you need
CSRC$i := \$(notdir \$(wildcard \$(SOURCE_DIR)/\*.c))

EOF
    } else {
      print MAKE<<EOF;
# TO DO: Add the c sources (if any). Do not include the path
CSRC$i :=

EOF
    }
  }
  print MAKE<<EOF;
# TO DO: Add the requiered libraries using the -l form (-lmy -> libmy.a)
LDLIBS$i :=

EOF
  hline;

}

sub print_lib_vars {
  my $i = shift;
  if ($wildcards) {
    print MAKE<<EOF;
# TO DO: If you want, change the name of the library
LIB$i := \$(LIB_DIR)/lib\$(MODULE_NAME).a

EOF
  } else {
    print MAKE<<EOF;
# TO DO: Complete the library name,
LIB$i := \$(LIB_DIR)/

EOF
  }

  if (!$opt_C) {
    if ($wildcards) {
       print MAKE<<EOF;
# TO DO: All the .cpp files, ... or choose the ones you need
L_CPPSRC$i := \$(notdir \$(wildcard \$(SOURCE_DIR)/\*.cpp))

EOF
    } else {
      print MAKE<<EOF;
# TO DO: Add the c++ sources (if any). Do not include the path
L_CPPSRC$i  :=

EOF
    }
  }

  if (!$opt_c) {
    if ($wildcards) {
       print MAKE<<EOF;
# TO DO: All the .c files, ... or choose the ones you need
L_CSRC$i := \$(notdir \$(wildcard \$(SOURCE_DIR)/\*.c))

EOF
    } else {
       print MAKE<<EOF;
# TO DO: Add the c sources (if any). Do not include the path
L_CSRC$i :=

EOF
    }
  }
  hline;

}


sub print_script_vars {
  my $i = shift;
  if ($wildcards) {
    print MAKE<<EOF;
# TO DO: If you want, change the name of the script
SCRIPT$i := \$(BIN_DIR)/\$(MODULE_NAME)

EOF
  } else {
    print MAKE<<EOF;
# TO DO: Complete the script name
SCRIPT$i := \$(BIN_DIR)/

EOF
  }

  print MAKE<<EOF;
# TO DO: write the name of the source script
SCRIPTSRC$i := 

EOF

  hline;

}

sub print_mod_vars {
  my $i = shift;
  if ($wildcards) {
    print MAKE<<EOF;
# TO DO: If you want, change the name of the module
MOD$i := \$(MOD_DIR)/lib\$(MODULE_NAME).so

EOF
  } else {
    print MAKE<<EOF;
# TO DO: Complete the module name,
MOD$i := \$(MOD_DIR)/

EOF
  }

  if (!$opt_C) {
    if ($wildcards) {
       print MAKE<<EOF;
# TO DO: All the .cpp files, ... or choose the ones you need
M_CPPSRC$i := \$(notdir \$(wildcard \$(SOURCE_DIR)/\*.cpp))

EOF
    } else {
      print MAKE<<EOF;
# TO DO: Add the c++ sources (if any). Do not include the path
M_CPPSRC$i  :=

EOF
    }
  }

  if (!$opt_c) {
    if ($wildcards) {
       print MAKE<<EOF;
# TO DO: All the .c files, ... or choose the ones you need
M_CSRC$i := \$(notdir \$(wildcard \$(SOURCE_DIR)/\*.c))

EOF
    } else {
       print MAKE<<EOF;
# TO DO: Add the c sources (if any). Do not include the path
M_CSRC$i :=

EOF
    }
  }
  hline;

}


sub print_prog_rule {
  my $i = shift;

  print MAKE<<EOF;
DEP$i     := \$(CPPSRC$i:.cpp=.d) \$(CSRC$i:.c=.d)
OBJ$i     := \$(DEP$i:.d=.o)

\$(PROG$i): \$(DEP$i) \$(OBJ$i) \$(LDLIBS$i) \$(LDLIBS)
	\@-test  -d \$(\@D) || (echo "Create bin dir: \$(\@D)"; mkdir -p \$(\@D) )
	\$(CXX) \$(LDDFLAGS) \$(OBJ$i) \$(LDLIBS$i) \$(LDLIBS) -o \$@

include \$(DEP$i)
#-----------------------------------------------------------------
EOF
}

sub print_lib_rule {
  my $i = shift;

  print MAKE<<EOF;
L_DEP$i     := \$(L_CPPSRC$i:.cpp=.d) \$(L_CSRC$i:.c=.d)
L_OBJ$i     := \$(L_DEP$i:.d=.o)

\$(LIB$i): \$(L_DEP$i) \$(LIB$i)(\$(L_OBJ$i)) 
	\@-test  -d \$(\@D) || (echo "Create lib dir: \$(\@D)"; mkdir -p \$(\@D) )
	ranlib \$(LIB$i)

include \$(L_DEP$i)
#-----------------------------------------------------------------
EOF
}

sub print_mod_rule {
  my $i = shift;

  print MAKE<<EOF;
M_DEP$i     := \$(M_CPPSRC$i:.cpp=.d) \$(M_CSRC$i:.c=.d)
M_OBJ$i     := \$(M_DEP$i:.d=.o)

\$(MOD$i): \$(M_DEP$i) \$(M_OBJ$i)
	\@-test  -d \$(\@D) || (echo "Create mod dir: \$(\@D)"; mkdir -p \$(\@D) )
	\$(CXX) -shared -Wl,-soname,lib\$(MODULE_NAME).so -o \$(MOD$i) \$(M_OBJ$i)

include \$(M_DEP$i)
#-----------------------------------------------------------------
EOF
}

sub print_script_rule {
  my $i = shift;

  print MAKE<<EOF;

\$(SCRIPT$i): \$(SCRIPTSRC$i)
	\@-test  -d \$(\@D) || (echo "Create bin dir: \$(\@D)"; mkdir -p \$(\@D) )
	cd \$(\@D); ln -fs \$(SOURCE_DIR)/\$(SCRIPTSRC$i) \$(\@F) 

#-----------------------------------------------------------------
EOF
}


sub modules_makefile {
  $modules = shift;

  print MAKE<<'EOF';


SHELL   = /bin/sh

#------------------------------------------------------------------------------------
# TO DO: Define the directories (and order) where something must be done.
EOF

  print MAKE<<EOF;
MODULES := $modules
EOF

  print MAKE<<'EOF';
#------------------------------------------------------------------------------------

.PHONY: help all debug release clean clean_debug clean_release clean_all $(MODULES)


help:
	@echo " "
	@echo "------------------------------------------------------------------------"
	@echo "Use:"
	@echo '  make release       : create "bin-&-lib"  release version'
	@echo '  make debug         : create "bin-&-lib"  debug version '
	@echo "  make all           : make debug and release  "
	@echo " "
	@echo '  make clean_release : remove the "release" intermediate files'
	@echo '  make clean_debug   : remove the "debug"   intermediate files'
	@echo '  make clean         : make clean_debug and clean_release'
	@echo " "
	@echo '  make clean_all     : remove also the "bin-&-lib" for both '
	@echo '                       debug and release'
	@echo "------------------------------------------------------------------------"
	@echo " "



all debug release clean clean_debug clean_release clean_all : $(MODULES)

$(MODULES):
	$(MAKE) -C $@ $(MAKECMDGOALS)


# If the work must continue not in the subdirectory but in some inner places,
# you can use as in the following example:

# $(MODULES):
#	$(MAKE) -C $@/src $(MAKECMDGOALS)
EOF

}




#================================================================================
# MAIN
#================================================================================

if (!getopts('ap:l:m:s:cCd:f:M:') || (!$opt_p && !$opt_l && !$opt_m && !$opt_M && !$opt_a && !$opt_s)) {
  syntax $0;
  exit;
}

create_makeaux if $opt_a;
exit if (!$opt_p && !$opt_l && !$opt_m && !$opt_M && !$opt_s);

$wildcards = 0;
$wildcards = 1 if ($opt_p + $opt_l + $opt_m == 1);

# Save, just in case.
rename $opt_f, "$opt_f.bak" if -f $opt_f;


open (MAKE, ">$opt_f")
  or die "Error opening output file: $opt_f\n";

if ($opt_M) {
  die "This wizard does not support both options -M and -l or -p"
    if  ($opt_M && ($opt_p || $opt_l));

  modules_makefile($opt_M);
  exit;
}

my $j;
#================================================================================
# Print header
#================================================================================
print MAKE <<"EOF";

SHELL = /bin/sh

# If ROOT_DIR is not defined try to guess it here
ifndef ROOT_DIR
   export ROOT_DIR := \$(shell pwd | sed 's/\\/$opt_d\\/.*//')
endif

export ROOT_SOURCE := \$(ROOT_DIR)/$opt_d
include \$(ROOT_SOURCE)/Make_aux

#------------------------------------------------------------------------------------
# This part of the makefile only is used when executed from the target directory
# Make_aux remakes it from there (and set/unset THIS_IS_A_SRC_DIR)
#------------------------------------------------------------------------------------

ifndef THIS_IS_A_SRC_DIR


#------------------------------------------------------------------------------------
EOF

#================================================================================
# Print variable definition (to be finished by the user)
#================================================================================
if ($opt_l) {
  for ($j = 1; $j <= $opt_l; ++$j) {
    print_lib_vars($j);
  }
}

if ($opt_m) {
  for ($j = 1; $j <= $opt_m; ++$j) {
    print_mod_vars($j);
  }
}

if ($opt_p) {
  for ($j = 1; $j <= $opt_p; ++$j) {
    print_prog_vars($j);
  }
}

if ($opt_s) {
  for ($j = 1; $j <= $opt_s; ++$j) {
    print_script_vars($j);
  }
}

#================================================================================
# Print the 'all' rule
#================================================================================
print MAKE "all:";
my $n = 0;
if ($opt_l) {
  for ($j = 1; $j <= $opt_l; ++$j) {
    print MAKE " \$(LIB$j)";
    print MAKE " \\\n    " unless ++$n % 5;
  }
}
if ($opt_m) {
  for ($j = 1; $j <= $opt_m; ++$j) {
    print MAKE " \$(MOD$j)";
    print MAKE " \\\n    " unless ++$n % 5;
  }
}
if ($opt_p) {
  for ($j = 1; $j <= $opt_p; ++$j) {
    print MAKE " \$(PROG$j)";
    print MAKE " \\\n    " unless ++$n % 5;
  }
}

if ($opt_s) {
  for ($j = 1; $j <= $opt_s; ++$j) {
    print MAKE " \$(SCRIPT$j)";
    print MAKE " \\\n    " unless ++$n % 5;
  }
}
print MAKE "\n";

#================================================================================
# Print the prog & lib rules
#================================================================================

hline;
if ($opt_l) {
  for ($j=1; $j <= $opt_l; ++$j) {
    print_lib_rule($j);
  }
}

if ($opt_m) {
  for ($j=1; $j <= $opt_m; ++$j) {
    print_mod_rule($j);
  }
}

if ($opt_s) {
  for ($j=1; $j <= $opt_s; ++$j) {
    print_script_rule($j);
  }
}

if ($opt_p) {
  for ($j=1; $j <= $opt_p; ++$j) {
    print_prog_rule($j);
  }
}

#================================================================================
# Print the 'clean' rule
#================================================================================
print MAKE<<EOF;
clean:
	-rm -f \$(SOURCE_DIR)/\*~
EOF
if ($opt_l) {
  for ($j = 1; $j <= $opt_l; ++$j) {
    print MAKE "\t-rm -f \$(L_DEP$j) \$(L_OBJ$j)\n";
  }
}
if ($opt_m) {
  for ($j = 1; $j <= $opt_m; ++$j) {
    print MAKE "\t-rm -f \$(M_DEP$j) \$(M_OBJ$j)\n";
  }
}
if ($opt_p) {
  for ($j = 1; $j <= $opt_p; ++$j) {
    print MAKE "\t-rm -f \$(DEP$j) \$(OBJ$j)\n";
  }
}

#================================================================================
# Print the 'clean_all' rule
#================================================================================
hline;
print MAKE<<EOF;
clean_all:
	-rm -f \$(SOURCE_DIR)/\*~
EOF

if ($opt_l) {
  for ($j = 1; $j <= $opt_l; ++$j) {
    print MAKE "\t-rm -f \$(L_DEP$j) \$(L_OBJ$j) \$(LIB$j)\n";
  }
}
if ($opt_m) {
  for ($j = 1; $j <= $opt_m; ++$j) {
    print MAKE "\t-rm -f \$(M_DEP$j) \$(M_OBJ$j) \$(MOD$j)\n";
  }
}
if ($opt_p) {
  for ($j = 1; $j <= $opt_p; ++$j) {
    print MAKE "\t-rm -f \$(DEP$j) \$(OBJ$j) \$(PROG$j)\n";
  }
}


#================================================================================
# Print tail
#================================================================================
print MAKE<<EOF;
endif

EOF

