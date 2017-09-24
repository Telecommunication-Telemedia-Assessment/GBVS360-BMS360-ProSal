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

    /*! \file   inter-cubic.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Cubic C(1) class interpolation methods
     */

/* 
    Header - Include guard
 */

    # ifndef __LI_CUBIC__
    # define __LI_CUBIC__

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

    /* Define cubic element coefficient flag */
    # define LI_CUBIC_FLAG_LEA li_Enum_s( 0 )
    # define LI_CUBIC_FLAG_SET li_Enum_s( 1 )

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

    /*! \brief Elementary cubic interpolation
     *  
     *  The function returns the interpolated value by a cubic interpolant. The
     *  interpolant is computed considering the cubic function that intercept
     *  the points defined by (liX1,liY1) and (liX2,liY2). The two last degrees
     *  of liberty are filled considering the derivatives at the liX1 and liX2
     *  point given by liDX1 and liDX2, respectively.
     *  
     *  If the liFlag is set, the interpolant coefficients are computed befor
     *  interpolation. Otherwise, the previously computed coefficients are 
     *  considered in order to improve function performances.
     *  
     *  \param liFlag Cubic coefficient computation flag
     *  \param liX    Interpolated value absciss
     *  \param liX1   Cubic interpolant control point absciss
     *  \param liY1   Cubic interpolant control point ordinate
     *  \param liX2   Cubic interpolant control point absciss
     *  \param liY2   Cubic interpolant control point ordinate
     *  \param liDX1  Cubic interpolant derivative at liX1 absciss
     *  \param liDX2  Cubic interpolant derivative at liX2 absciss
     *  
     *  \return Returns interpolated value
     */

    li_Real_t li_cubic( 

        li_Enum_t const liFlag,
        li_Real_t       liX,
        li_Real_t const liX1,
        li_Real_t const liX2,
        li_Real_t const liY1,
        li_Real_t const liY2,
        li_Real_t const liDX1,
        li_Real_t const liDX2

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

