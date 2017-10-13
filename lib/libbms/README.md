# Saliency Detection: A Boolean Map Approach (BMS)

Implementation of the saliency detection method described in paper
"Saliency Detection: A Boolean Map Approach", Jianming Zhang, 
Stan Sclaroff, ICCV, 2013


# Notes

- The files `BMS.h` and `BMS.cpp` were written by Jianming Zhang (see Copyright information). These were originally downloaded from the website of the author: http://cs-people.bu.edu/jmzhang/BMS/BMS.html Compared to the original source code, two changes were made: In `BMS.cpp` lines 48, and 54 were changed from CV_RGB2* to CV_BGR2* as OpenCV's imread function load images in BRG and not RGB. A function `bmsNormalize` was also added to allow BMS360 to be a child class of BMS. 

-  The files `BMS360.h` and `BMS360.cpp` were written by Pierre Lebreton. These includes the extension of BMS for 360 degree images. 


# License of BMS

Copyright (C) 2013 Jianming Zhang

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
If you have problems about this software, please contact: jmzhang@bu.edu