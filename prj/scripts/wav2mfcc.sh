#!/bin/bash
if [[ $# != 4 ]]; then
   echo "$0 lpc_order ncoef input.wav output.mcp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

mfcc_order=$1
mfcc_numfilters=$2
inputfile=$3
outputfile=$4


UBUNTU_SPTK=1
if [[ $UBUNTU_SPTK == 1 ]]; then
   # In case you install using debian package
   X2X="sptk x2x"
   FRAME="sptk frame"
   WINDOW="sptk window"
   MFCC="sptk mfcc"
else
# or install using direct sptk package
   X2X="x2x"
   FRAME="frame"
   WINDOW="window"
   MFCC="mfcc"
fi


base=/tmp/wav2mfcc$$  # temporal file
sox $inputfile $base.raw # $3 => 3rd argument, input.wav
$X2X +sf < $base.raw | $FRAME -l 400 -p 80 |\
          $MFCC -l 400 -m $mfcc_order -n $mfcc_numfilters > $base.cep

# Our array files need a header with number or cols and number of rows:
ncol=$((mfcc_order)) #+1?
nrow=`$X2X +fa < $base.cep | wc -l | perl -ne 'print $_/'$ncol', "\n";'`
echo $nrow $ncol | $X2X +aI > $outputfile
cat $base.cep >> $outputfile
\rm -f $base.raw $base.cep
exit

