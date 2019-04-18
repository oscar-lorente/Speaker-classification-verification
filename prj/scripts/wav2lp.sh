#!/bin/bash
if [[ $# != 3 ]]; then
   echo "$0 lpc_order input.wav output.lp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

lpc_order=$1
inputfile=$2
outputfile=$3


UBUNTU_SPTK=1
if [[ $UBUNTU_SPTK == 1 ]]; then
   # In case you install using debian package 
   X2X="sptk x2x"
   FRAME="sptk frame"
   WINDOW="sptk window"
   LPC="sptk lpc"
else
# or install using direct sptk package
   X2X="x2x"
   FRAME="frame"
   WINDOW="window"
   LPC="lpc"
fi


base=/tmp/wav2lpcc$$  # temporal file  
sox $inputfile $base.raw 
$X2X +sf < $base.raw | $FRAME -l 400 -p 80 | $WINDOW -l 400 -L 400 |\
          $LPC -l 400 -m $lpc_order > $base.lp

# Our array files need a header with number or cols and number of rows:
ncol=$((lpc_order+1)) # lpc p =>  (gain a1 a2 ... ap) 

nrow=`$X2X +fa < $base.lp | wc -l | perl -ne 'print $_/'$ncol', "\n";'`

echo $nrow $ncol | $X2X +aI > $outputfile # $4 output file
cat $base.lp >> $outputfile
\rm -f $base.raw $base.lp
exit
