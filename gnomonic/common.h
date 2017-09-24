/*
 * norama-suite libcommon - norama-suite common library
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
 * Please read <http://foxel.ch/license> for more information.
 *
 *
 * Author(s):
 *
 *      Nils Hamel <n.hamel@foxel.ch>
 *
 *
 * This file is part of the FOXEL project <http://foxel.ch>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Additional Terms:
 *
 *      You are required to preserve legal notices and author attributions in
 *      that material or in the Appropriate Legal Notices displayed by works
 *      containing it.
 *
 *      You are required to attribute the work as explained in the "Usage and
 *      Attribution" section of <http://foxel.ch/license>.
 */

    /*! \file   common.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Library common header file
     */

    /*! \mainpage norama-suite common library
     *
     *  \section norama-suite
     *  \section _ Panorama tools suite
     *
     *  This software suite offers tools for numerical panoramas manipulation.
     *  Mostly designed for equirectangular mappings, its role is to provide an
     *  interface to libgnomonic and its algorithms. It then offers an interface
     *  to projection and transformation features implemented in the gnomonic
     *  library. The norama-suite is also interfaced with libcsps in order to 
     *  take advantage of camera motion tracking to operate and align panoramas
     *  on earth. It also comes with a panorama viewer to complete the suite. 
     *
     *  \section Documentation
     *
     *  A detailed documentation can be generated through doxygen. A more general
     *  documentation can be consulted at https://github.com/FoxelSA/norama-suite/wiki
     *
     *  \section Copyright
     * 
     *  Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch \n
     *  This program is part of the FOXEL project <http://foxel.ch>.
     *  
     *  Please read the COPYRIGHT.md file for more information.
     *
     *  \section License
     *
     *  This program is licensed under the terms of the GNU Affero General Public
     *  License v3 (GNU AGPL), with two additional terms. The content is licensed
     *  under the terms of the Creative Commons Attribution-ShareAlike 4.0
     *  International (CC BY-SA) license.
     *
     *  You must read <http://foxel.ch/license> for more information about our
     *  Licensing terms and our Usage and Attribution guidelines.
     */

/* 
    Header - Include guard
 */

    # ifndef __LC__
    # define __LC__



/* 
    Header - Includes
 */

    # include <stdio.h>
    # include <stdlib.h>
    # include <string.h>

extern "C" {
    # include <inter-all.h>
}

/* 
    Header - Preprocessor definitions
 */

    /* Define standard output */
    # define LC_OUT     stdout
    # define LC_ERR     stderr

    /* Define boolean constants */
    # define LC_FALSE   0
    # define LC_TRUE    1

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */


/*
    Header - Include guard
 */

    # endif


