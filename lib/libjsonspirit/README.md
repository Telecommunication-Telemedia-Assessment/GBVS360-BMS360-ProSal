# JSON Spirit: A C++ JSON Parser/Generator Implemented with Boost Spirit


## Introduction

JSON is a text file format similar to XML, but less verbose. It has been called "XML lite". This article describes JSON Spirit, a C++ library that reads and writes JSON files or streams. It is written using the Boost Spirit parser generator. If you are already using Boost, you can use JSON Spirit without any additional dependencies.

## Key features:

supports ASCII or Unicode
std::vector or std::map implementations for JSON Objects
object library or header file only use
Using the Code
The JSON Spirit source code is available as a Microsoft Visual Studio 2005 C++ "solution". However, it should compile and work on any platform compatible with Boost. JSON Spirit has been built and tested with Visual C++ 2005, 2008, 2010, and G++ version 4.2.3 on Linux. It has been tested with Visual C++ using Boost versions 1.34.0, 1.37.0, 1.39.0, and 1.42.0. It has also been tested with STLPort.

Platform independent CMake files are included, kindly supplied by Uwe Arzt, with CPack declarations for packaging and a "make install" target supplied by Amzlkej.

The Visual C++ solution consists of five projects:

- The JSON Spirit library and header files

- An application running the library's unit tests

- Three small programs demonstrating how to use JSON Spirit



# License

The MIT License (MIT)

Copyright (c) 2017 John W. Wilkinson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.