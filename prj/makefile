

SHELL   = /bin/sh

#------------------------------------------------------------------------------------
# TO DO: Add the directories to "make" (directories where where something must be done.)
MODULES := pav make_wizard get_pitch vad test_fft fmatrix gmm gmm_verify pearson scripts
#------------------------------------------------------------------------------------

.PHONY: help all debug release clean clean_debug clean_release clean_all $(MODULES)


help:
	@echo " "
	@echo "------------------------------------------------------------------------"
	@echo "Use:"
	@echo '  make doc           : create documentation using doxygen'
	@echo " "
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


doc:
	doxygen 

