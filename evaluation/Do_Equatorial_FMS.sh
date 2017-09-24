#!/bin/bash

PathToBin=./bin
PathToImages=/Volumes/SSD/Salient360/FTP/Images/



# -----------------------------------------------------------------------------------------------------------------------------------------------
# BMS + FMS + EQ

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/BMS_FMS4_Eq
mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	# $PathToBin/salient -i $PathToImages/$fileName --rect-width 960 --rect-height 960 --aperture 70 --threads 12 -o $PathToOutput/$fileName --gbvs360-salmapmaxsize 42 --gbvs360-channels DIO
	# ./lib/libgbvs/bin/gbvs -i $PathToImages/$fileName  --salmapmaxsize 42 --blurfrac 0.02 -o $PathToOutput/$fileName --equatorial-prior --apply-fms 4
	./bin/bms -i $PathToImages/$fileName  --max-dim 2000 -o $PathToOutput/$fileName --apply-fms 4  --equatorial-prior
done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model



# -----------------------------------------------------------------------------------------------------------------------------------------------
# BMS + FMS 

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/BMS_FMS4
mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	# $PathToBin/salient -i $PathToImages/$fileName --rect-width 960 --rect-height 960 --aperture 70 --threads 12 -o $PathToOutput/$fileName --gbvs360-salmapmaxsize 42 --gbvs360-channels DIO
	# ./lib/libgbvs/bin/gbvs -i $PathToImages/$fileName  --salmapmaxsize 42 --blurfrac 0.02 -o $PathToOutput/$fileName --equatorial-prior --apply-fms 4
	./bin/bms -i $PathToImages/$fileName  --max-dim 2000 -o $PathToOutput/$fileName --apply-fms 4 
done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model



# -----------------------------------------------------------------------------------------------------------------------------------------------
# GBVS + FMS + EQ

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/GBVS_mx42_FMS4_Eq
mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	# $PathToBin/salient -i $PathToImages/$fileName --rect-width 960 --rect-height 960 --aperture 70 --threads 12 -o $PathToOutput/$fileName --gbvs360-salmapmaxsize 42 --gbvs360-channels DIO
	./lib/libgbvs/bin/gbvs -i $PathToImages/$fileName  --salmapmaxsize 42 --blurfrac 0.02 -o $PathToOutput/$fileName --equatorial-prior --apply-fms 4
	# ./bin/bms -i $PathToImages/$fileName  --max-dim 2000 --dilatation-width-1 $level  --dilatation-width-2 $2 -o $PathToOutputF/$fileName 
done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model


# -----------------------------------------------------------------------------------------------------------------------------------------------
# GBVS + FMS

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/GBVS_mx42_FMS4

mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	# $PathToBin/salient -i $PathToImages/$fileName --rect-width 960 --rect-height 960 --aperture 70 --threads 12 -o $PathToOutput/$fileName --gbvs360-salmapmaxsize 42 --gbvs360-channels DIO
	./lib/libgbvs/bin/gbvs -i $PathToImages/$fileName  --salmapmaxsize 42 --blurfrac 0.02 -o $PathToOutput/$fileName --apply-fms 4
	# ./bin/bms -i $PathToImages/$fileName  --max-dim 2000 --dilatation-width-1 $level  --dilatation-width-2 $2 -o $PathToOutputF/$fileName 
done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model



-----------------------------------------------------------------------------------------------------------------------------------------------
GBVS360 + FMS + EQ

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/GBVS360_DIO_mx42_FMS4_Eq
mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	$PathToBin/salient -i $PathToImages/$fileName --rect-width 960 --rect-height 960 --aperture 70 --threads 12 -o $PathToOutput/$fileName --gbvs360-salmapmaxsize 42 --gbvs360-channels DIO --gbvs360-apply-fms 4 --gbvs360-equatorial-prior
	# ./lib/libgbvs/bin/gbvs -i $PathToImages/$fileName  --salmapmaxsize 42 --blurfrac 0.02 -o $PathToOutput/$fileName --equatorial-prior --apply-fms 4
	# ./bin/bms -i $PathToImages/$fileName  --max-dim 2000 -o $PathToOutputF/$fileName --apply-fms 4  --equatorial-prior
done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model



# # -----------------------------------------------------------------------------------------------------------------------------------------------
# # GBVS360 + FMS 

PathToOutput=/Volumes/SSD/Salient360/FTP/Model/GBVS360_DIO_mx42_FMS4
mkdir -p $PathToOutput

while read fileName
do
	echo $fileName
	$PathToBin/salient -i $PathToImages/$fileName --rect-width 960 --rect-height 960 --aperture 70 --threads 12 -o $PathToOutput/$fileName --gbvs360-salmapmaxsize 42 --gbvs360-channels DIO --gbvs360-apply-fms 4
	# ./lib/libgbvs/bin/gbvs -i $PathToImages/$fileName  --salmapmaxsize 42 --blurfrac 0.02 -o $PathToOutput/$fileName --equatorial-prior --apply-fms 4
	# ./bin/bms -i $PathToImages/$fileName  --max-dim 2000 -o $PathToOutputF/$fileName --apply-fms 4  --equatorial-prior
done < $1

scp -r $PathToOutput sp:/home/nesc/benchmark/equirectangular/FTP/Model




