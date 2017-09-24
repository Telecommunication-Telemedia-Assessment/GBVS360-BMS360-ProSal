#!/bin/bash 

FOLDER=bms
NB_LINES=`wc -l $1 | awk '{ print int($1 / 4)+1 }'`

split -l $NB_LINES $1 batch_set_

for name in `ls batch_set_*` 
do
	echo $name
	sh run_batch.sh $name $ARGS $FOLDER &
done


