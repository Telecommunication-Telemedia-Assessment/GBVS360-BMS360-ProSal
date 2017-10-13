# libhmd 

This project is a preliminary work performed in order to simulate artifacts induced by the usage of HMDs. This framework will generate color bleeding, screendoor artefacts, blur, ... 


# Related code

This code contains GLSL shaders which were converted to OpenCV code. 

author alteredq / http://alteredqualia.com/
Film grain & scanlines shader ported from HLSL to WebGL / GLSL
http://www.truevision3d.com/forums/showcase/staticnoise_colorblackwhite_scanline_shaders-t18698.0.html

Screen Space Static Postprocessor
Produces an analogue noise overlay similar to a film grain / TV static
Original implementation and noise algorithm Pat 'Hawthorne' Shearon
Optimized scanlines + noise version with intensity scaling Georg 'Leviathan' Steinrohder
This version is provided under a Creative Commons Attribution 3.0 License
http://creativecommons.org/licenses/by/3.0/

It was also based on the web-based simulator: https://github.com/mkeblx/oculus-sim

It should however be noted that this application was not meant to provide a realistic model of HMD, but was designed to provide an introduction to key problems and factors involved in HMD. The implementation of the artifacts is performed by the means of GLSL Shaders. The algorithm involved in the shaders is not scientifically motivated, but based on the appearance of the resulting processing.



# License

The MIT License (MIT)

Copyright (c) 2017 Pierre Lebreton

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.