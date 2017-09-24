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

    # include "inter-bicubic.h"

/*
    Source - Fast bicubic image pixel interpolation method
 */

    li_C8_t li_bicubicf(

        li_C8_t   const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    ) {

        /* Interpolation vectors variables */
        li_Real_t liVS[16] = { li_Real_s( 0.0 ) };
        li_Real_t liVC[16] = { li_Real_s( 0.0 ) };

        /* Optimization variables */
        li_Real_t liTX1 = li_Real_s( 0.0 );
        li_Real_t liTY1 = li_Real_s( 0.0 );
        li_Real_t liTX2 = li_Real_s( 0.0 );
        li_Real_t liTY2 = li_Real_s( 0.0 );
        li_Real_t liTX3 = li_Real_s( 0.0 );
        li_Real_t liTY3 = li_Real_s( 0.0 );

        /* Interpolation reference variables */
        li_Size_t liPXrf = li_Size_s( 0 );
        li_Size_t liPYrf = li_Size_s( 0 );
        li_Size_t liPXmm = li_Size_s( 0 );
        li_Size_t liPYmm = li_Size_s( 0 );

        /* Interpolation sampling variables */
        li_Size_t liPXm1 = li_Size_s( 0 );
        li_Size_t liPXp1 = li_Size_s( 0 );
        li_Size_t liPXp2 = li_Size_s( 0 );
        li_Size_t liPYm1 = li_Size_s( 0 );
        li_Size_t liPYp1 = li_Size_s( 0 );
        li_Size_t liPYp2 = li_Size_s( 0 );

        /* Interpolated variables */
        li_Real_t liIV = li_Real_s( 0.0 );

        /* Compute relative grid parameters */
        liPXrf = li_Floor( liX ); 
        liPYrf = li_Floor( liY );

        /* Memorize reference point */
        liPXmm = liPXrf;
        liPYmm = liPYrf;

        /* Compute sampling nodes */
        liPXm1 = liPXrf - li_Size_s( 1 );
        liPYm1 = liPYrf - li_Size_s( 1 );
        liPXp1 = liPXrf + li_Size_s( 1 );
        liPXp2 = liPXrf + li_Size_s( 2 );
        liPYp1 = liPYrf + li_Size_s( 1 );
        liPYp2 = liPYrf + li_Size_s( 2 );

        /* Boundaries analysis */
        if ( liPXm1 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPXm1 = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPXrf < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPXrf = li_Size_s( 0 );

                /* Boundaries analysis */
                if ( liPXp1 < li_Size_s( 0 ) ) {

                    /* Boundary condition correction */
                    liPXp1 = li_Size_s( 0 );

                    /* Boundaries analysis */
                    if ( liPXp2 < li_Size_s( 0 ) ) {

                        /* Boundary condition correction */
                        liPXp2 = li_Size_s( 0 );

                    }

                }

            }

        } else
        if ( liPXp2 >= liWidth ) {

            /* Boundary condition correction */
            liPXp2 = liWidth - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPXp1 >= liWidth ) {

                /* Boundary condition correction */
                liPXp1 = liPXp2;

                /* Boundaries analysis */
                if ( liPXrf >= liWidth ) {

                    /* Boundary condition correction */
                    liPXrf = liPXp2;

                    /* Boundaries analysis */
                    if ( liPXm1 >= liWidth ) {

                        /* Boundary condition correction */
                        liPXm1 = liPXp2;

                    }

                }

            }

        }

        /* Boundaries analysis */
        if ( liPYm1 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPYm1 = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPYrf < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPYrf = li_Size_s( 0 );

                /* Boundaries analysis */
                if ( liPYp1 < li_Size_s( 0 ) ) {

                    /* Boundary condition correction */
                    liPYp1 = li_Size_s( 0 );

                    /* Boundaries analysis */
                    if ( liPYp2 < li_Size_s( 0 ) ) {

                        /* Boundary condition correction */
                        liPYp2 = li_Size_s( 0 );

                    }

                }

            }

        } else
        if ( liPYp2 >= liHeight ) {

            /* Boundary condition correction */
            liPYp2 = liHeight - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPYp1 >= liHeight ) {

                /* Boundary condition correction */
                liPYp1 = liPYp2;

                /* Boundaries analysis */
                if ( liPYrf >= liHeight ) {

                    /* Boundary condition correction */
                    liPYrf = liPYp2;

                    /* Boundaries analysis */
                    if ( liPYm1 >= liHeight ) {

                        /* Boundary condition correction */
                        liPYm1 = liPYp2;

                    }

                }

            }

        }

        /* Compute memory width */
        liWidth *= liLayer; if ( liWidth % li_Size_s( 4 ) ) liWidth += li_Size_s( 4 ) - liWidth % li_Size_s( 4 );

        /* Compute interpolation vector */
        liVS[ 0] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXm1 + liChannel );
        liVS[ 1] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXrf + liChannel );
        liVS[ 2] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp1 + liChannel );
        liVS[ 3] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp2 + liChannel );
        liVS[ 4] = * ( liBytes + liWidth * liPYrf + liLayer * liPXm1 + liChannel );
        liVS[ 5] = * ( liBytes + liWidth * liPYrf + liLayer * liPXrf + liChannel );
        liVS[ 6] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp1 + liChannel );
        liVS[ 7] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp2 + liChannel );
        liVS[ 8] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXm1 + liChannel );
        liVS[ 9] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXrf + liChannel );
        liVS[10] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp1 + liChannel );
        liVS[11] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp2 + liChannel );
        liVS[12] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXm1 + liChannel );
        liVS[13] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXrf + liChannel );
        liVS[14] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp1 + liChannel );
        liVS[15] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp2 + liChannel );

        /* Compute interpolation matrix product */
        liVC[ 0] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0];
        liVC[ 1] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[12];
        liVC[ 2] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[12];
        liVC[ 3] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[12];
        liVC[ 4] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3];
        liVC[ 5] = ( li_Real_s(  121.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s( -198.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   99.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -22.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s( -198.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  324.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s( -162.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   99.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s( -162.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   81.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(  -22.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    4.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[ 6] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  165.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s( -270.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  135.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -30.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s( -132.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  216.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s( -108.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   24.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[ 7] = ( li_Real_s(   11.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -2.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -27.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(  -11.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    2.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[ 8] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3];
        liVC[ 9] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  165.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s( -132.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s( -270.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  216.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  135.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s( -108.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(  -30.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   24.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[10] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  225.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s( -180.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s( -180.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  144.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[11] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   15.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(  -15.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[12] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3];
        liVC[13] = ( li_Real_s(   11.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -11.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -27.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   -2.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    2.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[14] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   15.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -15.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[15] = ( li_Real_s(    1.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -1.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   -1.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    1.0 ) / li_Real_s(   36.0 ) ) * liVS[15];

        /* Prepare interpolated value computation */
        liTX1 = ( liX + li_Real_s( 1.0 ) ) - liPXmm; 
        liTX2 = liTX1 * liTX1; 
        liTX3 = liTX1 * liTX2;
        liTY1 = ( liY + li_Real_s( 1.0 ) ) - liPYmm;
        liTY2 = liTY1 * liTY1;
        liTY3 = liTY1 * liTY2;

        /* Compute interpolated value */
        liIV = liVC[ 0]                 + 
               liVC[ 1] * liTY1         + 
               liVC[ 2] * liTY2         + 
               liVC[ 3] * liTY3         +
               liVC[ 4] * liTX1         + 
               liVC[ 5] * liTY1 * liTX1 + 
               liVC[ 6] * liTY2 * liTX1 + 
               liVC[ 7] * liTY3 * liTX1 +
               liVC[ 8] * liTX2         + 
               liVC[ 9] * liTY1 * liTX2 + 
               liVC[10] * liTY2 * liTX2 + 
               liVC[11] * liTY3 * liTX2 +
               liVC[12] * liTX3         + 
               liVC[13] * liTY1 * liTX3 + 
               liVC[14] * liTY2 * liTX3 + 
               liVC[15] * liTY3 * liTX3;

        /* Verify interpolated value */
        if ( liIV < li_Real_s( 0.0 ) ) {

            /* Clamp interpolated value */
            liIV = li_Real_s( 0.0 );

        } else if ( liIV > li_Real_s( 255.0 ) ) {

            /* Clamp interpolated value */
            liIV = li_Real_s( 255.0 );

        }

        /* Return interpolated value */
        return( li_C8_c( liIV ) );

    }









    float li_bicubicf_f(

        float     const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    ) {

        /* Interpolation vectors variables */
        li_Real_t liVS[16] = { li_Real_s( 0.0 ) };
        li_Real_t liVC[16] = { li_Real_s( 0.0 ) };

        /* Optimization variables */
        li_Real_t liTX1 = li_Real_s( 0.0 );
        li_Real_t liTY1 = li_Real_s( 0.0 );
        li_Real_t liTX2 = li_Real_s( 0.0 );
        li_Real_t liTY2 = li_Real_s( 0.0 );
        li_Real_t liTX3 = li_Real_s( 0.0 );
        li_Real_t liTY3 = li_Real_s( 0.0 );

        /* Interpolation reference variables */
        li_Size_t liPXrf = li_Size_s( 0 );
        li_Size_t liPYrf = li_Size_s( 0 );
        li_Size_t liPXmm = li_Size_s( 0 );
        li_Size_t liPYmm = li_Size_s( 0 );

        /* Interpolation sampling variables */
        li_Size_t liPXm1 = li_Size_s( 0 );
        li_Size_t liPXp1 = li_Size_s( 0 );
        li_Size_t liPXp2 = li_Size_s( 0 );
        li_Size_t liPYm1 = li_Size_s( 0 );
        li_Size_t liPYp1 = li_Size_s( 0 );
        li_Size_t liPYp2 = li_Size_s( 0 );

        /* Interpolated variables */
        li_Real_t liIV = li_Real_s( 0.0 );

        /* Compute relative grid parameters */
        liPXrf = li_Floor( liX ); 
        liPYrf = li_Floor( liY );

        /* Memorize reference point */
        liPXmm = liPXrf;
        liPYmm = liPYrf;

        /* Compute sampling nodes */
        liPXm1 = liPXrf - li_Size_s( 1 );
        liPYm1 = liPYrf - li_Size_s( 1 );
        liPXp1 = liPXrf + li_Size_s( 1 );
        liPXp2 = liPXrf + li_Size_s( 2 );
        liPYp1 = liPYrf + li_Size_s( 1 );
        liPYp2 = liPYrf + li_Size_s( 2 );

        /* Boundaries analysis */
        if ( liPXm1 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPXm1 = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPXrf < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPXrf = li_Size_s( 0 );

                /* Boundaries analysis */
                if ( liPXp1 < li_Size_s( 0 ) ) {

                    /* Boundary condition correction */
                    liPXp1 = li_Size_s( 0 );

                    /* Boundaries analysis */
                    if ( liPXp2 < li_Size_s( 0 ) ) {

                        /* Boundary condition correction */
                        liPXp2 = li_Size_s( 0 );

                    }

                }

            }

        } else
        if ( liPXp2 >= liWidth ) {

            /* Boundary condition correction */
            liPXp2 = liWidth - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPXp1 >= liWidth ) {

                /* Boundary condition correction */
                liPXp1 = liPXp2;

                /* Boundaries analysis */
                if ( liPXrf >= liWidth ) {

                    /* Boundary condition correction */
                    liPXrf = liPXp2;

                    /* Boundaries analysis */
                    if ( liPXm1 >= liWidth ) {

                        /* Boundary condition correction */
                        liPXm1 = liPXp2;

                    }

                }

            }

        }

        /* Boundaries analysis */
        if ( liPYm1 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPYm1 = li_Size_s( 0 );

            /* Boundaries analysis */
            if ( liPYrf < li_Size_s( 0 ) ) {

                /* Boundary condition correction */
                liPYrf = li_Size_s( 0 );

                /* Boundaries analysis */
                if ( liPYp1 < li_Size_s( 0 ) ) {

                    /* Boundary condition correction */
                    liPYp1 = li_Size_s( 0 );

                    /* Boundaries analysis */
                    if ( liPYp2 < li_Size_s( 0 ) ) {

                        /* Boundary condition correction */
                        liPYp2 = li_Size_s( 0 );

                    }

                }

            }

        } else
        if ( liPYp2 >= liHeight ) {

            /* Boundary condition correction */
            liPYp2 = liHeight - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPYp1 >= liHeight ) {

                /* Boundary condition correction */
                liPYp1 = liPYp2;

                /* Boundaries analysis */
                if ( liPYrf >= liHeight ) {

                    /* Boundary condition correction */
                    liPYrf = liPYp2;

                    /* Boundaries analysis */
                    if ( liPYm1 >= liHeight ) {

                        /* Boundary condition correction */
                        liPYm1 = liPYp2;

                    }

                }

            }

        }

        /* Compute memory width */
        liWidth *= liLayer; if ( liWidth % li_Size_s( 4 ) ) liWidth += li_Size_s( 4 ) - liWidth % li_Size_s( 4 );

        /* Compute interpolation vector */
        liVS[ 0] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXm1 + liChannel );
        liVS[ 1] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXrf + liChannel );
        liVS[ 2] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp1 + liChannel );
        liVS[ 3] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp2 + liChannel );
        liVS[ 4] = * ( liBytes + liWidth * liPYrf + liLayer * liPXm1 + liChannel );
        liVS[ 5] = * ( liBytes + liWidth * liPYrf + liLayer * liPXrf + liChannel );
        liVS[ 6] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp1 + liChannel );
        liVS[ 7] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp2 + liChannel );
        liVS[ 8] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXm1 + liChannel );
        liVS[ 9] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXrf + liChannel );
        liVS[10] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp1 + liChannel );
        liVS[11] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp2 + liChannel );
        liVS[12] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXm1 + liChannel );
        liVS[13] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXrf + liChannel );
        liVS[14] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp1 + liChannel );
        liVS[15] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp2 + liChannel );

        /* Compute interpolation matrix product */
        liVC[ 0] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0];
        liVC[ 1] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[12];
        liVC[ 2] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[12];
        liVC[ 3] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[12];
        liVC[ 4] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3];
        liVC[ 5] = ( li_Real_s(  121.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s( -198.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   99.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -22.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s( -198.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  324.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s( -162.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   99.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s( -162.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   81.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(  -22.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    4.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[ 6] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  165.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s( -270.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  135.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -30.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s( -132.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  216.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s( -108.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   24.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[ 7] = ( li_Real_s(   11.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -2.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -27.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(  -11.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    2.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[ 8] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3];
        liVC[ 9] = ( li_Real_s(  -66.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  165.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s( -132.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  108.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s( -270.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  216.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  135.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s( -108.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(  -30.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   24.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[10] = ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -90.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  225.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s( -180.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   72.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s( -180.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  144.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[11] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   15.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(  -15.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[12] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3];
        liVC[13] = ( li_Real_s(   11.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   33.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -11.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -54.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -27.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   27.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   -2.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    2.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[14] = ( li_Real_s(   -6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -18.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    6.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   15.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(  -45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   45.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -15.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -12.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   36.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -36.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   12.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[15];
        liVC[15] = ( li_Real_s(    1.0 ) / li_Real_s(   36.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -1.0 ) / li_Real_s(   36.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   -9.0 ) / li_Real_s(   36.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    9.0 ) / li_Real_s(   36.0 ) ) * liVS[10] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[11] +
                   ( li_Real_s(   -1.0 ) / li_Real_s(   36.0 ) ) * liVS[12] +
                   ( li_Real_s(    3.0 ) / li_Real_s(   36.0 ) ) * liVS[13] +
                   ( li_Real_s(   -3.0 ) / li_Real_s(   36.0 ) ) * liVS[14] +
                   ( li_Real_s(    1.0 ) / li_Real_s(   36.0 ) ) * liVS[15];

        /* Prepare interpolated value computation */
        liTX1 = ( liX + li_Real_s( 1.0 ) ) - liPXmm; 
        liTX2 = liTX1 * liTX1; 
        liTX3 = liTX1 * liTX2;
        liTY1 = ( liY + li_Real_s( 1.0 ) ) - liPYmm;
        liTY2 = liTY1 * liTY1;
        liTY3 = liTY1 * liTY2;

        /* Compute interpolated value */
        liIV = liVC[ 0]                 + 
               liVC[ 1] * liTY1         + 
               liVC[ 2] * liTY2         + 
               liVC[ 3] * liTY3         +
               liVC[ 4] * liTX1         + 
               liVC[ 5] * liTY1 * liTX1 + 
               liVC[ 6] * liTY2 * liTX1 + 
               liVC[ 7] * liTY3 * liTX1 +
               liVC[ 8] * liTX2         + 
               liVC[ 9] * liTY1 * liTX2 + 
               liVC[10] * liTY2 * liTX2 + 
               liVC[11] * liTY3 * liTX2 +
               liVC[12] * liTX3         + 
               liVC[13] * liTY1 * liTX3 + 
               liVC[14] * liTY2 * liTX3 + 
               liVC[15] * liTY3 * liTX3;


        /* Return interpolated value */
        return( ( liIV ) );

    }



