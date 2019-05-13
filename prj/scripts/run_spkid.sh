#!/bin/bash

# Scripting is very useful to repeat tasks, as testing different configuration, multiple files, etc.
# This bash script is provided as one example
# Please, adapt at your convinience, add cmds, etc.
# Antonio Bonafonte, Nov. 2015


# TODO
# Set the proper value to variables: w, db
# w:  a working directory for temporary files
# db: directory of the speecon database 
w=
db=


# ------------------------
# Usage
# ------------------------

if [[ $# < 1 ]]; then
   echo "$0 cmd1 [...]"
   echo "Where commands can be:"
   echo  "    lists: create, for each spk, training and devel. list of files"
   echo  "      mcp: feature extraction (mel cepstrum parameters)"
   echo  " trainmcp: train gmm for the mcp features"
   echo  "  testmcp: test GMM using only mcp features"
   echo  " classerr: count errors of testmcp"
   echo  "finaltest: reserved for final test"
   echo  "listverif: create, list for verification -- files, candidates, ...."
   echo "trainworld: estimate world model"
   echo "    verify: test gmm in verification task"
   echo " verifyerr: count errors of verify"
   echo "       roc: print error as a function of thr, so that roc can be plot"
   exit 1
fi


# ------------------------
# Check directories
# ------------------------

if [[ -z "$w" ]]; then echo "Edit this script and set variable 'w'"; exit 1; fi
mkdir -p $w  #Create directory if it does not exists
if [[ $? -ne 0 ]]; then echo "Error creating directory $w"; exit 1; fi

if [[ ! -d "$db" ]]; then
   echo "Edit this script and set variable 'db' to speecon db"
   exit 1
fi
	

# ------------------------
# Check if gmm_train is in path
# ------------------------
which gmm_train > /dev/null
if [[ $? != 0 ]] ; then
   echo "Set PATH to include PAV executable programs ... "
   echo "Maybe ... source pavrc ? or modify bashrc ..."
   exit 1
fi 
# Now, we assume that all the path for programs are already in the path 

CMDS="lists mcp trainmcp testmcp classerr finaltest listverif trainworld verify verifyerr roc"


# ------------------------
# Auxiliar functions
# ------------------------

# cmd lists: foreach spk, create train and test list.
create_lists() {
    \rm -fR $w/lists
    mkdir -p $w/lists
    for dir in $db/BLOCK*/SES* ; do
	name=${dir/*\/}
	echo Create list for speaker $dir $name ----
	(find -L $db/BLOCK*/$name -name "*.wav" | perl -pe 's/^.*BLOCK/BLOCK/; s/\.wav$//' | sort | unsort.sh > $name.list) || exit 1
	# split in test list (5 files) and train list (other files)
	(head -5 $name.list | sort > $w/lists/$name.test) || exit 1
	(tail -n +6 $name.list | sort > $w/lists/$name.train) || exit 1
	\rm -f $name.list
    done
    cat $w/lists/*.train | sort > $w/lists/all.train
    cat $w/lists/*.test | sort > $w/lists/all.test
}


# command mcp: create feature from wave files
# TODO: select (or change) different features, options. 
# Make you best choice or try several options

compute_mcp() {
    for line in $(cat $w/lists/all.train) $(cat $w/lists/all.test); do
        mkdir -p `dirname $w/mcp/$line.mcp`
        echo "wav2mfcc 24 16 $db/$line.wav" "$w/mcp/$line.mcp"
        wav2lpcc.sh 4 8 "$db/$line.wav" "$w/mcp/$line.mcp" || exit 1
    done
}


# command listverify
# Create list for users, candidates, impostors, etc.

create_lists_verif() {
    dirlv=$w/lists_verif
    mkdir -p $dirlv 

    # find all the speakers names
    find  -L $db -type d -name 'SES*' -printf '%P\n'|\
           perl -pe 's|BLOCK../||' | sort > $dirlv/all.txt

    unsort.sh $dirlv/all.txt > all_spk 

    # split speakers into: users (50), impostors (50) and the others
    (head -50 all_spk | sort > $dirlv/users.txt) || exit 1
    (tail -n +51 all_spk  | head -50 | sort > $dirlv/impostors.txt) || exit 1
    (tail -n +101 all_spk | sort > $dirlv/others.txt) || exit 1
    \rm -f all_spk

    # create trainning/test data from legitime users
    \rm -f $dirlv/users.train; touch $dirlv/users.train
    \rm -f $dirlv/users.test; touch $dirlv/users.test
    for spk in `cat $dirlv/users.txt`; do
	cat $w/lists/$spk.train >> $dirlv/users.train
	cat $w/lists/$spk.test >> $dirlv/users.test
    done

    # create trainning data from 'other' speakers
    \rm -f $dirlv/others.train; touch $dirlv/others.train
    for spk in `cat $dirlv/others.txt`; do
	cat $w/lists/$spk.train >> $dirlv/others.train
    done

    # Join users and others training, just in case you want to use it both
    cat $dirlv/users.train $dirlv/others.train > $dirlv/users_and_others.train

    # create test data from 'impostors' speakers
    \rm -f $dirlv/impostors.test; touch $dirlv/impostors.test
    for spk in `cat $dirlv/impostors.txt`; do
	cat $w/lists/$spk.test >> $dirlv/impostors.test
    done

    # Create 4 claims for each file:
    cat $dirlv/impostors.test $dirlv/impostors.test \
        $dirlv/impostors.test $dirlv/impostors.test |\
        sort > $dirlv/impostors4.test

    # Create candidate claims:

    # From legitime users:
    cat $dirlv/users.test | perl -ne 'chomp; print "$1\n" if m|(SES\d+)|' > $dirlv/users.test.candidates

    # From impostors: (claim a random legitime user)
    perl -ne '{
       BEGIN {
          open("USERS", "$ARGV[0]") or die "Error opening user file: $ARGV[0]\n"; 
          @users = <USERS>;
          $nusers = int(@users);
          die "Error: nUsers == 0 in file $ARGV[0]\n" if $nusers == 0;
          shift;
          srand(102001);
       }
       $v = int(rand($nusers));
       print $users[$v];
    }' $dirlv/users.txt < $dirlv/impostors4.test > $dirlv/impostors4.test.candidates


    # Join all the test
    cat $dirlv/users.test $dirlv/impostors4.test  > $dirlv/all.test 
    cat $dirlv/users.test.candidates $dirlv/impostors4.test.candidates  > $dirlv/all.test.candidates 

     echo "Train lists:"
     wc -l $dirlv/*.train | grep -v total; echo

     echo "Test lists"
     wc -l $dirlv/*.test | grep -v total; echo
     wc -l $dirlv/*.test.candidates | grep -v total

}

# ---------------------------------
# Main program: 
# For each cmd in command line ...
# ---------------------------------


for cmd in $*; do
   echo `date`: $cmd '---';

   if [[ $cmd == lists ]]; then
      create_lists
   elif [[ $cmd == mcp ]]; then
       compute_mcp       
   elif [[ $cmd == trainmcp ]]; then
       # TODO: select (or change) good parameters of gmm_train
       for dir in $db/BLOCK*/SES* ; do
	   name=${dir/*\/}
	   echo $name ----
	   gmm_train  -v 1 -T 0.01 -N5 -m 1 -d $w/mcp -e mcp -g $w/gmm/mcp/$name.gmm $w/lists/$name.train || exit 1
           echo
       done
   elif [[ $cmd == testmcp ]]; then
       find $w/gmm/mcp -name '*.gmm' -printf '%P\n' | perl -pe 's/.gmm$//' | sort  > $w/lists/gmm.list
       (gmm_classify -d $w/mcp -e mcp -D $w/gmm/mcp -E gmm $w/lists/gmm.list  $w/lists/all.test | tee $w/spk_classification.log) || exit 1

   elif [[ $cmd == classerr ]]; then
       if [[ ! -s $w/spk_classification.log ]] ; then
          echo "ERROR: $w/spk_classification.log not created"
          exit 1
       fi
       # Count errors
       perl -ne 'BEGIN {$ok=0; $err=0}
                 next unless /^.*SA(...).*SES(...).*$/; 
                 if ($1 == $2) {$ok++}
                 else {$err++}
                 END {printf "nerr=%d\tntot=%d\terror_rate=%.2f%%\n", ($err, $ok+$err, 100*$err/($ok+$err))}' $w/spk_classification.log
   elif [[ $cmd == finaltest ]]; then
       echo "To be implemented ..."
   elif [[ $cmd == listverif ]]; then
      create_lists_verif
   elif [[ $cmd == trainworld ]]; then
       # TODO
       echo "Implement the trainworld option ..."
   elif [[ $cmd == verify ]]; then
       # TODO gmm_verify --> put std output in $w/spk_verify.log, ej gmm_verify .... > $w/spk_verify.log   or gmm_verify ... | tee $w/spk_verify.log
       echo "Implement the verify option ..."
   elif [[ $cmd == verif_err ]]; then
       if [[ ! -s $w/spk_verify.log ]] ; then
          echo "ERROR: $w/spk_verify.log not created"
          exit 1
       fi
       # You can pass the threshold to spk_verif_score.pl or it computes the
       # best one for these particular results.
       spk_verif_score.pl $w/spk_verify.log | tee $w/spk_verify.res
   elif [[ $cmd == roc ]]; then
       # Change threshold and compute table prob false alarm vs. prob. detection 
       spk_verif_score.pl $w/spk_verify.log | tee $w/spk_verify.res
       perl -ne '
         next if ! /^THR\t/; 
         chomp;
         @F=split/\t/;
         ($prob_miss = $F[3]) =~ s/\(.*//;  
         $prob_detect = 1 - $prob_miss;
         ($prob_fa = $F[4]) =~ s/\(.*//; 
         print "$prob_fa\t$prob_detect\n"'  $w/spk_verify.res > $w/spk_verify.roc

   else
       echo "undefined command $cmd" && exit 1
   fi
done

exit 0
