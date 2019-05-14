Wav2mfcc.sh
 #!/bin/bash
if [[ $# != 3 ]]; then  
 echo "$0 mfcc_order input.wav output.mcp" 
  exit 1
fi 
# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal
mfcc_order=$1
inputfile=$2
outputfile=$3
base=/tmp/wav2mfcc$$  # temporal file
 sox $inputfile $base.raw # $2 => 2rd argument, input.wav
sf=$(sox --i $inputfile | grep "Sample Rate" | grep -Po "\d+")
sf=$((sf/1000))
x2x +sf < $base.raw | frame -l 400 -p 80 |\ mfcc -l 400 -m $mfcc_order -s $sf > $base.cep
#mfcc -l 400 -m $mfcc_order -s $sf -  E > $base.cep
# Our array files need a header with number or cols and number of rows:
ncol=$((mfcc_order)) 
#ncol=$((mfcc_order+1))
nrow=`x2x +fa < $base.cep | wc -l | perl -ne 'print $_/'$ncol', "\n";'`
echo $nrow $ncol | x2x +aI > $outputfile
cat $base.cep >> $outputfile
\rm -f $base.raw $base.cep
exit
