#!/bin/bash

PathToBin=../bin
PathToImages=/Volumes/SSD/Salient360/trainSet/Stimuli/
PathToOutput=/Volumes/SSD/Salient360/trainSet/$2


mkdir -p $PathToOutput/Action
mkdir -p $PathToOutput/Affective
mkdir -p $PathToOutput/Art
mkdir -p $PathToOutput/BlackWhite
mkdir -p $PathToOutput/Cartoon
mkdir -p $PathToOutput/Fractal
mkdir -p $PathToOutput/Indoor
mkdir -p $PathToOutput/Inverted
mkdir -p $PathToOutput/Jumbled
mkdir -p $PathToOutput/LineDrawing
mkdir -p $PathToOutput/LowResolution
mkdir -p $PathToOutput/Noisy
mkdir -p $PathToOutput/Object
mkdir -p $PathToOutput/OutdoorManMade
mkdir -p $PathToOutput/OutdoorNatural
mkdir -p $PathToOutput/Pattern
mkdir -p $PathToOutput/Random
mkdir -p $PathToOutput/Satelite
mkdir -p $PathToOutput/Sketch
mkdir -p $PathToOutput/Social


while read fileName
do
	echo $fileName
	$PathToBin/test-saliency -i $PathToImages/$fileName -o $PathToOutput/$fileName
done < $1

