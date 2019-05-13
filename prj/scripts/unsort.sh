#!/bin/bash
(perl -pe 'BEGIN {srand(102001)} print rand, "\t"' $* | sort | cut -f 2) || exit 1
exit 0

