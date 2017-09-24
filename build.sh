#!/bin/bash

# lib required by gbvs360 -> handle projection from equirectangular to rectilinear and vice versa.
# It depends on libinter, but it will be compiled along libgnomonic

cd lib/libgnomonic/lib/libinter/
make $1 -j8

cd ../../
make $1 -j8

# BMS framework. Does not depend on other lib
cd ../libbms
make $1 -j8

# Line detector algorithm + clustering of vanishing point. Required by libgbvs
# Does not depend on other lib
cd ../liblinper
make $1 -j8

# Segmentation using the mean-shift algorithm. Required by libgbvs
# Does not depend on other lib
cd ../libmeanshift
make $1 -j8

# Model of head mounted display. Required by gbvs360
# Does not depend on other lib
cd ../libhmd
make $1 -j8

# GBVS visual attention model. Required by gbvs360
# It depends on liblinper and libmeanshift
cd ../libgbvs
make $1 -j8

# JSON spirit, a library to parse JSON files
# It depends on nothing
cd ../libjsonspirit
make $1 -j8


# compile libgbvs360 and make the final binary
# It depends on all other libraries
cd ../../
make $1 -j8

# compile the tool to generate ground truth saliency maps
# It depends on libjsonspirit and libgnomonic
cd ./truth
make $1 -j8

# compile the tool to perform gnomonic projections
# It depends on libjsonspirit and libgnomonic
cd ../gnomonic
make $1 -j8

cd ..

