#!/bin/bash

PathToBin=./bin
PathToImages=/Volumes/SSD/Salient360/FTP/Images/



# # -----------------------------------------------------------------------------------------------------------------------------------------------
# # GBVS360 + DIOB

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/GBVS_mx42_DIO
mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	./bin/gbvs -i $PathToImages/$fileName --salmapmaxsize 42 --disable-csf -o $PathToOutput/$fileName 

done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model

