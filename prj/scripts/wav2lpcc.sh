#!/bin/bash
if [[ $# != 4 ]]; then
   echo "$0 lpc_order ncoef input.wav output.mcp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

lpc_order=$1
nceps=$2
inputfile=$3
outputfile=$4


UBUNTU_SPTK=1
if [[ $UBUNTU_SPTK == 1 ]]; then
   # In case you install using debian package 
   X2X="sptk x2x"
   FRAME="sptk frame"
   WINDOW="sptk window"
   LPC="sptk lpc"
   LPC2C="sptk lpc2c"
else
# or install using direct sptk package
   X2X="x2x"
   FRAME="frame"
   WINDOW="window"
   LPC="lpc"
   LPC2C="lpc2c"
fi



base=/tmp/wav2lpcc$$  # temporal file  
sox $inputfile $base.raw # $3 => 3rd argument, input.wav
$X2X +sf < $base.raw | $FRAME -l 400 -p 80 | $WINDOW -l 400 -L 512 |\
          $LPC -l 400 -m $lpc_order | $LPC2C -m $lpc_order -M $nceps > $base.cep

# Our array files need a header with number or cols and number of rows:
ncol=$((nceps+1))
nrow=`$X2X +fa < $base.cep | wc -l | perl -ne 'print $_/'$ncol', "\n";'`
echo $nrow $ncol | $X2X +aI > $outputfile
cat $base.cep >> $outputfile
\rm -f $base.raw $base.cep
exit
