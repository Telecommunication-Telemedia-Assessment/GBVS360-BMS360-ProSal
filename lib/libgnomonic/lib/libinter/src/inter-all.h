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

    /*! \file   inter-all.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Library general includer
     */

/* 
    Header - Include guard
 */

    # ifndef __LI_ALL__
    # define __LI_ALL__

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
    # include "inter-bicubic.h"
    # include "inter-biheptic.h"
    # include "inter-bilinear.h"
    # include "inter-bipentic.h"
    # include "inter-cubic.h"

/* 
    Header - Preprocessor definitions
 */

/* 
    Header - Preprocessor macros
 */

    /* Ascending compatibility - Functions */
    # define inter_bicubicf     li_bicubicf     /* Defined under beta 0.2 */
    # define inter_bilinearf    li_bilinearf    /* Defined under beta 0.2 */
    # define inter_bipenticf    li_bipenticf    /* Defined under beta 0.2 */

    /* Ascending compatibility - Types */
    # define inter_C8_t         li_C8_t         /* Defined under beta 0.2 */
    # define inter_Index_t      li_Size_t       /* Defined under beta 0.2 */
    # define inter_Real_t       li_Real_t       /* Defined under beta 0.2 */
    # define inter_Method_t     li_Method_t     /* Defined under beta 0.2 */
    # define li_Method_tf       li_Method_tf
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
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    Header - Include guard
 */

    # endif

