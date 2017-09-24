/*
 * libinter - Interpolation methods library
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

    /*! \file   inter.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Library common header file
     */

    /*! \mainpage libinter
     *
     *  \section libinter
     *  \section _ Interpolation methods library
     *
     *  The libinter library is dedicated to interpolation methods. The provided 
     *  interpolation methods have different orders to provide schemes that can
     *  fit a large spectrum of situations and constraints.
     *
     *  \section Documentation
     *
     *  A detailed documentation can be generated through doxygen. A more general
     *  documentation can be consulted at https://github.com/FoxelSA/libinter/wiki.
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

    # ifndef __LI__
    # define __LI__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include <math.h>
    # include <stdint.h>

/* 
    Header - Preprocessor definitions
 */

/* 
    Header - Preprocessor macros
 */

    /* Define casting macro */
    # define li_C8_c( x )       ( ( li_C8_t ) x )
    # define li_Size_c( x )     ( ( li_Size_t ) x )
    # define li_Enum_c( x )     ( ( li_Enum_t ) x )
    # define li_Real_c( x )     ( ( li_Real_t ) x )

    /* Define litteral suffix */
    # define li_C8_s( x )       UINT8_C( x )
    # define li_Size_s( x )     INT64_C( x )
    # define li_Enum_s( x )     INT64_C( x )
    # define li_Real_s( x )     ( x )

    /* Define formated output specifiers */
    # define li_C8_p            PRIu8
    # define li_Size_p          PRId64
    # define li_Enum_p          PRId64
    # define li_Real_p          "lf"

    /* Define formated input specifiers */
    # define li_C8_i            SCNu8
    # define li_Size_i          SCNu64
    # define li_Enum_i          SCNu64
    # define li_Real_i          "lf"

    /* Define floating point operators */
    # define li_Floor( x )      floor( x )

/* 
    Header - Typedefs
 */

    /* Define pixel component type */
    typedef uint8_t li_C8_t;

    /* Define general index */
    typedef int64_t li_Size_t;

    /* Define general enumeration */
    typedef int64_t li_Enum_t;

    /* Define floating type */
    typedef double  li_Real_t;

    /* General interpolation method prototype */
    typedef li_C8_t ( * li_Method_t ) ( 

        li_C8_t const * const, 
        li_Size_t, 
        li_Size_t const, 
        li_Size_t const, 
        li_Size_t const, 
        li_Real_t const, 
        li_Real_t const

    );


    /* General interpolation method prototype */
    typedef float ( * li_Method_tf ) ( 

        float const * const, 
        li_Size_t, 
        li_Size_t const, 
        li_Size_t const, 
        li_Size_t const, 
        li_Real_t const, 
        li_Real_t const

    );

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    Header - Include guard
 */

    # endif

