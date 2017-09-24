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

    /*! \file   common-method.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Interpolation method interface
     */

/* 
    Header - Include guard
 */

    # ifndef __LC_METHOD__
    # define __LC_METHOD__


/* 
    Header - Includes
 */

    # include "common.h"

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

    /*! \brief Interpolation method by string
     *
     *  This function returns a pointer to an interpolation method based on the
     *  string passed as parameter. The list of implemented tags is given by :
     *
     *      bilinearf   Fast bilinear method
     *      bicubicf    Fast bicubic method
     *      bipenticf   Fast bipentic method
     *      bihepticf   Fast biheptic method
     *
     *  In case the provided string corresponds to an unknown tag, the fast
     *  bicubic method is returned as default.
     *
     *  \param  nrTag   String containing the method tag
     *
     *  \return Returns a pointer to the desired interpolation method
     */

    li_Method_t  lc_method  ( char const * const nrTag );
    li_Method_tf lc_method_f ( char const * const nrTag );

/* 
    Header - C/C++ compatibility
 */


/*
    Header - Include guard
 */

    # endif

