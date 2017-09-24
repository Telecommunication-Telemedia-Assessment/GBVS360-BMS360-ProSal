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

    # include "inter-bilinear.h"

/*
    Source - Fast bilinear image pixel interpolation method
 */

    li_C8_t li_bilinearf(

        li_C8_t   const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    ) {

        /* Interpolation vectors variables */
        li_Real_t liVS[4] = { li_Real_s( 0.0 ) };
        li_Real_t liVC[4] = { li_Real_s( 0.0 ) };

        /* Optimization variables */
        li_Real_t liTX = li_Real_s( 0.0 );
        li_Real_t liTY = li_Real_s( 0.0 );

        /* Interpolation reference variables */
        li_Size_t liPXrf = li_Size_s( 0 );
        li_Size_t liPYrf = li_Size_s( 0 );
        li_Size_t liPXmm = li_Size_s( 0 );
        li_Size_t liPYmm = li_Size_s( 0 );

        /* Interpolation sampling variables */
        li_Size_t liPXp1 = li_Size_s( 0 );
        li_Size_t liPYp1 = li_Size_s( 0 );

        /* Interpolated variables */
        li_Real_t liIV = li_Real_s( 0.0 );

        /* Compute relatlive grid parameters */
        liPXrf = li_Floor( liX ); 
        liPYrf = li_Floor( liY );

        /* Memorize reference point */
        liPXmm = liPXrf;
        liPYmm = liPYrf;

        /* Compute sampling nodes */
        liPXp1 = liPXrf + li_Size_s( 1 );
        liPYp1 = liPYrf + li_Size_s( 1 );

        /* Boundaries analysis */
        if ( liPXrf < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPXrf = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPXp1 < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPXp1 = li_Size_s( 0 );

            }

        } else
        if ( liPXp1 >= liWidth ) {

            /* Boundary condition correction */
            liPXp1 = liWidth - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPXrf >= liWidth ) {

                /* Boundary condition correction */
                liPXrf = liPXp1;

            }

        }

        /* Boundaries analysis */
        if ( liPYrf < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPYrf = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPYp1 < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPYp1 = li_Size_s( 0 );

            }

        } else
        if ( liPYp1 >= liHeight ) {

            /* Boundary condition correction */
            liPYp1 = liHeight - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPYrf >= liHeight ) {

                /* Boundary condition correction */
                liPYrf = liPYp1;

            }

        }

        /* Compute memory width */
        liWidth *= liLayer; if ( liWidth % li_Size_s( 4 ) ) liWidth += li_Size_s( 4 ) - liWidth % li_Size_s( 4 );

        /* Compute interpolation vector */
        liVS[0] = * ( liBytes + liWidth * liPYrf + liLayer * liPXrf + liChannel );
        liVS[1] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp1 + liChannel );
        liVS[2] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXrf + liChannel );
        liVS[3] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp1 + liChannel );

        /* Compute interpolation matrix product */
        liVC[0] = + liVS[0];
        liVC[1] = - liVS[0] + liVS[2];
        liVC[2] = - liVS[0] + liVS[1];
        liVC[3] = + liVS[0] - liVS[1] - liVS[2] + liVS[3];

        /* Prepare interpolated value computation */
        liTX = liX - liPXmm;
        liTY = liY - liPYmm;

        /* Compute interpolated value */
        liIV = liVC[0]               + 
               liVC[1] * liTY        + 
               liVC[2] * liTX        + 
               liVC[3] * liTX * liTY;

        /* Verify interpolated value */
        if ( liIV < li_Real_s( 0.0 ) ) {

            /* Clamp interpolated value */
            liIV = li_Real_s( 0.0 );

        } else if ( liIV > li_Real_s( 255.0 ) ) {

            /* Clamp interpolated value */
            liIV = li_Real_s( 255.0 );

        }

        /* Return interpolated value */
        return( liIV );

    }




    float li_bilinearf_f(

        float     const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    ) {

        /* Interpolation vectors variables */
        li_Real_t liVS[4] = { li_Real_s( 0.0 ) };
        li_Real_t liVC[4] = { li_Real_s( 0.0 ) };

        /* Optimization variables */
        li_Real_t liTX = li_Real_s( 0.0 );
        li_Real_t liTY = li_Real_s( 0.0 );

        /* Interpolation reference variables */
        li_Size_t liPXrf = li_Size_s( 0 );
        li_Size_t liPYrf = li_Size_s( 0 );
        li_Size_t liPXmm = li_Size_s( 0 );
        li_Size_t liPYmm = li_Size_s( 0 );

        /* Interpolation sampling variables */
        li_Size_t liPXp1 = li_Size_s( 0 );
        li_Size_t liPYp1 = li_Size_s( 0 );

        /* Interpolated variables */
        li_Real_t liIV = li_Real_s( 0.0 );

        /* Compute relatlive grid parameters */
        liPXrf = li_Floor( liX ); 
        liPYrf = li_Floor( liY );

        /* Memorize reference point */
        liPXmm = liPXrf;
        liPYmm = liPYrf;

        /* Compute sampling nodes */
        liPXp1 = liPXrf + li_Size_s( 1 );
        liPYp1 = liPYrf + li_Size_s( 1 );

        /* Boundaries analysis */
        if ( liPXrf < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPXrf = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPXp1 < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPXp1 = li_Size_s( 0 );

            }

        } else
        if ( liPXp1 >= liWidth ) {

            /* Boundary condition correction */
            liPXp1 = liWidth - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPXrf >= liWidth ) {

                /* Boundary condition correction */
                liPXrf = liPXp1;

            }

        }

        /* Boundaries analysis */
        if ( liPYrf < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPYrf = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPYp1 < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPYp1 = li_Size_s( 0 );

            }

        } else
        if ( liPYp1 >= liHeight ) {

            /* Boundary condition correction */
            liPYp1 = liHeight - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPYrf >= liHeight ) {

                /* Boundary condition correction */
                liPYrf = liPYp1;

            }

        }

        /* Compute memory width */
        liWidth *= liLayer; if ( liWidth % li_Size_s( 4 ) ) liWidth += li_Size_s( 4 ) - liWidth % li_Size_s( 4 );

        /* Compute interpolation vector */
        liVS[0] = * ( liBytes + liWidth * liPYrf + liLayer * liPXrf + liChannel );
        liVS[1] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp1 + liChannel );
        liVS[2] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXrf + liChannel );
        liVS[3] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp1 + liChannel );

        /* Compute interpolation matrix product */
        liVC[0] = + liVS[0];
        liVC[1] = - liVS[0] + liVS[2];
        liVC[2] = - liVS[0] + liVS[1];
        liVC[3] = + liVS[0] - liVS[1] - liVS[2] + liVS[3];

        /* Prepare interpolated value computation */
        liTX = liX - liPXmm;
        liTY = liY - liPYmm;

        /* Compute interpolated value */
        liIV = liVC[0]               + 
               liVC[1] * liTY        + 
               liVC[2] * liTX        + 
               liVC[3] * liTX * liTY;


        /* Return interpolated value */
        return( liIV );

    }

