#!/bin/bash

if [ x"$1" = x"" ] ; then
   echo "usage: $0 <file>.mdve"
   exit 1;
fi

if test ! -f $1 ; then
   echo "$1 must be an existing regular file"
   exit 1;
fi

gen_file=${1%.*}_gen.dve
m4 $1 > $gen_file
its-reach -t DVE -i $gen_file

