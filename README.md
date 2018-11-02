# GBVS360, BMS360, ProSal: Extending existing saliency prediction models from 2D to omnidirectional images 

In this project, it is studied how existing visual saliency models designed for 2D images can be extended and applied to Omnidirectional images in the equirectangular format. Three different frameworks, BMS360, GBVS360 and Projected Saliency (ProSal) were designed to address this task. In the particular case of BMS360 and GBVS360, the 2D models Boolean Map Saliency (BMS) and Graph-Based Visual Saliency (GBVS) were rigorously extended to integrate the specific properties of equirectangular images to model visual attention in omnidirectional images watched using Head Mounted Displays (HMDs). With the proposed extensions, the saliency prediction performance in comparison to their original design could significantly be improved. 


# Context 

This project was intiated to address the ICME2017 Grand Challenge on Visual attention for 360 images. Two track of this challenged are considered: 

- The prediction of saliency maps obtained from Head motion data (on still images)

- The prediction of saliency maps obtained from Head and Eye motion data (on still images)



# Binaries

In the release section of GitHub, different set of binaries can be found. The version 1.0 correspond to the models submitted to the ICME2017 Grand Challenge. These models were ranked as 1st at addressing Head motion-based saliency maps out of 13 submissions. And was ranked and was ranked 6th (5.75) out of 16 submissions when Head/Eye motion-based saliency maps are addressed. 

The release with a version higher than 1.0 were the result of work performed after the end of the competition. Version 2.0 includes the BMS360 model.



# Build from source

To compile the project, only two dependencies are required: 

   - fftw (optional: it adds support of a CSF extention to GBVS) 

   - boost (>= 1.63)

   - OpenCV (>= 3.0)


## UNIX system

run  `make`.  

## Windows: 

A visual studio solution file is provided (tested using the 2015 Community edition). A complete set of all depending libraries can be found at the following URL: https://1drv.ms/u/s!AqL868MBKJ4Gh-guxlbtLEQ4h6VqXw 
Unzip the folder Library next to the clone of the repository. Then, open the visual studio solution file, and you can compile the project (The path to the external library folder is set relatively to the root folder). 



# Citation

The journal paper related to this work has been accepted for publication. In case you use this work, please cite:

```
@article{lebreton2018GBVS360,
  title={GBVS360, BMS360, ProSal: Extending existing saliency prediction models from 2D to omnidirectional images},
  author={Lebreton, Pierre and Raake, Alexander},
  journal={Signal Processing: Image Communication},
  volume={69},
  pages={69--78},
  year={2018},
  month={november},
  publisher={Elsevier}
}
```


For the evaluation scripts used in this study, please refer and cite:

```
@article{Gutierrez2018Toolbox, 
  title={Toolbox and Dataset for the Development of Saliency and Scanpath Models for Omnidirectional / 360{$^{\circ}$} Still Images}, 
  author={Guti{\'e}rrez, Jes{\'u}s and David, Erwan and Rai, Yashas and Le Callet, Patrick 
}, 
  journal={Signal Processing: Image Communication}, 
  volume={69}, 
  pages={35--42}, 
  month={november},
  year={2018}, 
  publisher={Elsevier} 
}
```





# License

The MIT License (MIT)

Copyright (c) 2017 Pierre Lebreton

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
