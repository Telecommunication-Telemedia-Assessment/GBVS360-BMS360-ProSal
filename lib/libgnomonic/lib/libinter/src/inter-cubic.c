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


/* 
    Source - Includes
 */

    # include "inter-cubic.h"

/*
    Source - Elementary cubic interpolation
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

    ) {

        /* Cubic coefficient variables */
        static li_Real_t liA = li_Real_s( 0.0 );
        static li_Real_t liB = li_Real_s( 0.0 );

        /* Verify cubic coefficient flag */
        if ( liFlag == LI_CUBIC_FLAG_SET ) {

            /* Compute cubic coefficients */
            liA = + ( liDX1 ) * ( liX2 - liX1 ) - ( liY2 - liY1 );
            liB = - ( liDX2 ) * ( liX2 - liX1 ) + ( liY2 - liY1 );

        }

        /* Compute normalized interpolation parameter */
        liX = ( liX - liX1 ) / ( liX2 - liX1 );

        /* Compute and return interpolated value */
        return( ( ( li_Real_s( 1.0 ) - liX ) * liY1 ) + ( liX * liY2 ) + ( liX * ( li_Real_s( 1.0 ) - liX ) ) * ( liA * ( li_Real_s( 1.0 ) - liX ) + liB * liX ) );

    }

