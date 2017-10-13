# GBVS360, ProSal

This library contains the model GBVS360 which provides extensions to GBVS to support 360 degree images. The main entry class is the files `GBVS360.h` and `GBVS360.cpp`. 

Simple example of usage of these class can be found in the files `Saliency360.h` and `Saliency360.cpp`. The class `Saliency360` could be considered to be used as an interface to all the different models. To use it in this manner, it is only necessary to link the libgbvs360 to your project, and import the `Saliency360.h` file. 

An example of such usage, can be found in the folder `../Saliency360/main.cpp`




# License

The MIT License (MIT)

Copyright (c) 2017 Pierre Lebreton

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.