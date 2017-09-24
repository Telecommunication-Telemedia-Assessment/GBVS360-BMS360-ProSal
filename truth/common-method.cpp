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


/* 
    Source - Includes
 */

    # include "common-method.h"

/*
    Source - Interpolation method by string
 */

    li_Method_t lc_method( char const * const nrTag ) {

        /* Interpolation method variables */
        li_Method_t nrMethod = li_bicubicf;

        /* Switch on string tag */
        if ( strcmp( nrTag, "bilinearf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bilinearf;

        } else
        if ( strcmp( nrTag, "bicubicf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bicubicf;

        } else
        if ( strcmp( nrTag, "bipenticf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bipenticf;

        } else
        if ( strcmp( nrTag, "bihepticf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bihepticf;

        }

        /* Return selected method */
        return( nrMethod );

    }

    li_Method_tf lc_method_f( char const * const nrTag ) {
        li_Method_tf nrMethod = li_bicubicf_f;

        /* Switch on string tag */
        if ( strcmp( nrTag, "bilinearf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bilinearf_f;

        } else
        if ( strcmp( nrTag, "bicubicf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bicubicf_f;

        } else
        if ( strcmp( nrTag, "bipenticf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bipenticf_f;

        } else
        if ( strcmp( nrTag, "bihepticf" ) == 0 ) {

            /* Assign interpolation method */
            nrMethod = li_bihepticf_f;

        }

        return( nrMethod );
    }

