# GBVS360, BMS360, ProSal: Extending existing saliency prediction models from 2D to omnidirectional images 

In this project, it is studied how existing visual saliency models designed for 2D images can be extended and applied to Omnidirectional images in the equirectangular format. Three different frameworks, BMS360, GBVS360 and Projected Saliency (ProSal) were designed to address this task. In the particular case of BMS360 and GBVS360, the 2D models Boolean Map Saliency (BMS) and Graph-Based Visual Saliency (GBVS) were rigorously extended to integrate the specific properties of equirectangular images to model visual attention in omnidirectional images watched using Head Mounted Displays (HMDs). With the proposed extensions, the saliency prediction performance in comparison to their original design could significantly be improved. 


# Context 

This project was initially created to address the ICME2017 Grand Challenge on Visual attention for 360 images. 

The goals of this grand challenges were as follows: Understanding how users watch a 360° image and analyzing how they scan through the content with a combination of head and eye movement, is necessary to develop appropriate rendering devices and also create good VR/AR content for consumers. Good visual attention modeling is a key factor in that perspective that helps enhance the overall Quality of Experience (QoE). Although a huge number of algorithms have been developed in recent years to gauge visual attention in flat-2D images and videos and also a benchmarking platform (saliency.mit.edu) where users can submit and assess their results, attention studies in 360o scenarios are absent. The goal of this challenge is to therefore two-fold:

   - to produce a dataset to ensure easy and precise reproducibility of results for future saliency / scan-path computational models in line with the principles of Reproducible and Sustainable research from IEEE.

   - to set a first baseline for the taxonomy of several types of visual attention models (saliency models, importance models, saccadic models) and the correct methodology and ground-truth data to test each of them.


In the first stage, we present a dataset of sixty 360o images along with the associated head and eye-tracking data. An additional 20 images will be provided without any tracking data. As all images are covered under the Creative Commons copyright, you are free to reuse and redistribute the content for research purposes along with relevant citations and links to our hosting website and paper which provides appropriate credits to the photographers. We additionally provide 3 software: VR content playback module, a benchmarking tool and saliency/ scan-path generator for use by the participants.



# Binaries

In the release section of GitHub, different set of binaries can be found. The version 1.0 correspond to the models submitted to the ICME2017 Grand Challenge. These models were ranked as first at addressing Head motion-based saliency maps out of 13 submissions. And was ranked and was ranked 6th (5.75) out of 16 submissions when Head/Eye motion-based saliency maps are addressed. 

The release with a version higher than 1.0 were the result of work performed after the end of the competition. Version 2.0 includes the BMS360 model.



# Build from source

To compile the project, only two dependencies are required: 

   - boost (>= 1.63)

   - OpenCV (>= 3.0)


## UNIX system

run the script "build.sh".  

## Windows: 

A visual studio solution file is provided (tested using the 2015 Community edition). A complete set of all depending libraries can be found at the following URL: https://1drv.ms/u/s!AqL868MBKJ4Gh-guxlbtLEQ4h6VqXw 
Unzip the folder Library next to the clone of the repository. Then, open the visual studio solution file, and you can compile the project (The path to the external library folder is set relatively to the root folder). 



# Citation

The paper related to this work have been submitted to the journal: Journal Signal Processing: Image Communication, It is entitled "GBVS360, BMS360, ProSal: Extending existing saliency prediction models from 2D to omnidirectional images", and the authors are: Pierre Lebreton (Zhejiang University) and Alexander Raake (TU Ilmenau). More details on the citation will be added with the progress of the submissions of this work.  


# License

The MIT License (MIT)

Copyright (c) 2017 Pierre Lebreton

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.