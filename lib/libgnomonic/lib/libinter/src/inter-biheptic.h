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

    /*! \file   inter-biheptic.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Biheptic C(0) class interpolation methods
     */

/* 
    Header - Include guard
 */

    # ifndef __LI_BIHEPTIC__
    # define __LI_BIHEPTIC__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include "inter.h"

/* 
    Header - Preprocessor definitions
 */

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

    /*! \brief Fast bipentic interpolation method
     *
     *  The functions performs an order eight fast biheptic interpolation of 
     *  bitmap pixels based on bitmap pointed by liBytes.
     *  
     *  \param  liBytes   Pointer to bitmap
     *  \param  liWidth   Bitmap width, in pixels
     *  \param  liHeight  Bitmap height, in pixels
     *  \param  liLayer   Bitmap number of chromatic layers
     *  \param  liChannel Bitmap interpolated layer
     *  \param  liX       Interpolated point position x
     *  \param  liY       Interpolated point position y
     *  
     *  \return Returns interpolated value
     */

    li_C8_t li_bihepticf(

        li_C8_t   const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    );



    float li_bihepticf_f(

        float   const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    );


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

