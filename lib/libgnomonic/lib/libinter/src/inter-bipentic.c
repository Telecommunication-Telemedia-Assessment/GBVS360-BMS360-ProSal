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

    # include "inter-bipentic.h"

/*
    Source - Fast bipentic image pixel interpolation method
 */

    li_C8_t li_bipenticf(

        li_C8_t   const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    ) {

        /* Interpolation vectors variables */
        li_Real_t liVS[36] = { li_Real_s( 0.0 ) };
        li_Real_t liVC[36] = { li_Real_s( 0.0 ) };

        /* Optimization variables */
        li_Real_t liTX1 = li_Real_s( 0.0 );
        li_Real_t liTY1 = li_Real_s( 0.0 );
        li_Real_t liTX2 = li_Real_s( 0.0 );
        li_Real_t liTY2 = li_Real_s( 0.0 );
        li_Real_t liTX3 = li_Real_s( 0.0 );
        li_Real_t liTY3 = li_Real_s( 0.0 );
        li_Real_t liTX4 = li_Real_s( 0.0 );
        li_Real_t liTY4 = li_Real_s( 0.0 );
        li_Real_t liTX5 = li_Real_s( 0.0 );
        li_Real_t liTY5 = li_Real_s( 0.0 );

        /* Interpolation reference variables */
        li_Size_t liPXrf = li_Size_s( 0 );
        li_Size_t liPYrf = li_Size_s( 0 );
        li_Size_t liPXmm = li_Size_s( 0 );
        li_Size_t liPYmm = li_Size_s( 0 );

        /* Interpolation sampling variables */
        li_Size_t liPXm2 = li_Size_s( 0 );
        li_Size_t liPXm1 = li_Size_s( 0 );
        li_Size_t liPXp1 = li_Size_s( 0 );
        li_Size_t liPXp2 = li_Size_s( 0 );
        li_Size_t liPXp3 = li_Size_s( 0 );
        li_Size_t liPYm2 = li_Size_s( 0 );
        li_Size_t liPYm1 = li_Size_s( 0 );
        li_Size_t liPYp1 = li_Size_s( 0 );
        li_Size_t liPYp2 = li_Size_s( 0 );
        li_Size_t liPYp3 = li_Size_s( 0 );

        /* Interpolated variables */
        li_Real_t liIV = li_Real_s( 0.0 );

        /* Compute relatlive grid parameters */
        liPXrf = li_Floor( liX );
        liPYrf = li_Floor( liY );

        /* Memorize reference point */
        liPXmm = liPXrf;
        liPYmm = liPYrf;

        /* Compute sampling nodes */
        liPXm2 = liPXrf - li_Size_s( 2 );
        liPXm1 = liPXrf - li_Size_s( 1 );
        liPYm2 = liPYrf - li_Size_s( 2 );
        liPYm1 = liPYrf - li_Size_s( 1 );
        liPXp1 = liPXrf + li_Size_s( 1 );
        liPXp2 = liPXrf + li_Size_s( 2 );
        liPXp3 = liPXrf + li_Size_s( 3 );
        liPYp1 = liPYrf + li_Size_s( 1 );
        liPYp2 = liPYrf + li_Size_s( 2 );
        liPYp3 = liPYrf + li_Size_s( 3 );

        /* Boundaries analysis */
        if ( liPXm2 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPXm2 = li_Size_s( 0 );

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

                            /* Boundaries analysis */
                            if ( liPXp3 < li_Size_s( 0 ) ) {

                                /* Boundary condition correction */
                                liPXp3 = li_Size_s( 0 );

                            }

                        }

                    }

                }

            }

        } else
        if ( liPXp3 >= liWidth ) {

            /* Boundary condition correction */
            liPXp3 = liWidth - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPXp2 >= liWidth ) {

                /* Boundary condition correction */
                liPXp2 = liPXp3;

                /* Boundaries analysis */
                if ( liPXp1 >= liWidth ) {

                    /* Boundary condition correction */
                    liPXp1 = liPXp3;

                    /* Boundaries analysis */
                    if ( liPXrf >= liWidth ) {

                        /* Boundary condition correction */
                        liPXrf = liPXp3;

                        /* Boundaries analysis */
                        if ( liPXm1 >= liWidth ) {

                            /* Boundary condition correction */
                            liPXm1 = liPXp3;

                            /* Boundaries analysis */
                            if ( liPXm2 >= liWidth ) {

                                /* Boundary condition correction */
                                liPXm2 = liPXp3;

                            }

                        }

                    }

                }

            }

        }

        /* Boundaries analysis */
        if ( liPYm2 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPYm2 = li_Size_s( 0 );

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

                            /* Boundaries analysis */
                            if ( liPYp3 < li_Size_s( 0 ) ) {

                                /* Boundary condition correction */
                                liPYp3 = li_Size_s( 0 );

                            }

                        }

                    }

                }

            }

        } else
        if ( liPYp3 >= liHeight ) {

            /* Boundary condition correction */
            liPYp3 = liHeight - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPYp2 >= liHeight ) {

                /* Boundary condition correction */
                liPYp2 = liPYp3;

                /* Boundaries analysis */
                if ( liPYp1 >= liHeight ) {

                    /* Boundary condition correction */
                    liPYp1 = liPYp3;

                    /* Boundaries analysis */
                    if ( liPYrf >= liHeight ) {

                        /* Boundary condition correction */
                        liPYrf = liPYp3;

                        /* Boundaries analysis */
                        if ( liPYm1 >= liHeight ) {

                            /* Boundary condition correction */
                            liPYm1 = liPYp3;

                            /* Boundaries analysis */
                            if ( liPYm2 >= liHeight ) {

                                /* Boundary condition correction */
                                liPYm2 = liPYp3;

                            }

                        }

                    }

                }

            }

        }

        /* Compute memory width */
        liWidth *= liLayer; if ( liWidth % li_Size_s( 4 ) ) liWidth += li_Size_s( 4 ) - liWidth % li_Size_s( 4 );

        /* Compute interpolation vector */
        liVS[ 0] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXm2 + liChannel );
        liVS[ 1] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXm1 + liChannel );
        liVS[ 2] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXrf + liChannel );
        liVS[ 3] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXp1 + liChannel );
        liVS[ 4] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXp2 + liChannel );
        liVS[ 5] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXp3 + liChannel );
        liVS[ 6] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXm2 + liChannel );
        liVS[ 7] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXm1 + liChannel );
        liVS[ 8] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXrf + liChannel );
        liVS[ 9] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp1 + liChannel );
        liVS[10] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp2 + liChannel );
        liVS[11] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp3 + liChannel );
        liVS[12] = * ( liBytes + liWidth * liPYrf + liLayer * liPXm2 + liChannel );
        liVS[13] = * ( liBytes + liWidth * liPYrf + liLayer * liPXm1 + liChannel );
        liVS[14] = * ( liBytes + liWidth * liPYrf + liLayer * liPXrf + liChannel );
        liVS[15] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp1 + liChannel );
        liVS[16] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp2 + liChannel );
        liVS[17] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp3 + liChannel );
        liVS[18] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXm2 + liChannel );
        liVS[19] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXm1 + liChannel );
        liVS[20] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXrf + liChannel );
        liVS[21] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp1 + liChannel );
        liVS[22] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp2 + liChannel );
        liVS[23] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp3 + liChannel );
        liVS[24] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXm2 + liChannel );
        liVS[25] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXm1 + liChannel );
        liVS[26] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXrf + liChannel );
        liVS[27] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp1 + liChannel );
        liVS[28] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp2 + liChannel );
        liVS[29] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp3 + liChannel );
        liVS[30] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXm2 + liChannel );
        liVS[31] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXm1 + liChannel );
        liVS[32] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXrf + liChannel );
        liVS[33] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXp1 + liChannel );
        liVS[34] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXp2 + liChannel );
        liVS[35] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXp3 + liChannel );

        /* Compute interpolation matrix product */
        liVC[ 0] = ( li_Real_s(   -300189270608550490089696788480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0];
        liVC[ 1] = ( li_Real_s(    685432167889520651822761508864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1500946353042737954487183343616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   1500946353042724162213324521472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -1000630902028473439222155444224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    375236588260674003678913363968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -60037854121707283268172251136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 2] = ( li_Real_s(   -562854882391026838485810020352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   1926214486404840394091185307648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -2676687662926191779148732563456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   1951230258955527449718477029376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -762981062796707438507656740864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    125078862753557930857560276992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 3] = ( li_Real_s(    212634066681053577154930933760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -888059925550281202332656992256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   1475930580492012055313105551360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -1225772854984886482711288479744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    512823337289593124904908095488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -87555203927491107513371197440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 4] = ( li_Real_s(    -37523658826068142758142410752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    175110407854984748301532790784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   -325205043159256897121637367808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    300189270608544262455837065216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -137586749028915901855948603392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     25015772550711917784218992640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 5] = ( li_Real_s(      2501577255071203323293990912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -12507886275356033658900185088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(     25015772550712067317800370176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    -25015772550712062919753859072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     12507886275356033658900185088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     -2501577255071209920363757568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 6] = ( li_Real_s(    685432167888330153408763789312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1500946353037374167330985082880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   1500946353033083925735961264128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1000630902019816535575930863616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    375236588256777967788772818944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -60037854121001256065651703808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(               5307535142640928768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(             -23890720206887014400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(              42905267744887128064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(             -38515568462088527872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(              17335721846887892992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(              -3142236065440415744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(              -9424565637665363968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(              42357221425809219584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(             -75981003189760147456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(              68164328424607531008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(             -30676299989447696384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(               5560318966456455168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(               8369701100107887616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(             -37547909685822480384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(              67265149446485196800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(             -60300096274281152512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(              27130102676398624768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(              -4916947262888051712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(              -3729428933099930624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(              16703063155371606016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(             -29888172368504315904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(              26776243676818083840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(             -12044190816380643328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(               2182485285795192064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(                665298017487659264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(              -2975846611402234880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(               5320440590865428480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(              -4764568063767093248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(               2142852871757383168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(               -388176804941194304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[ 7] = ( li_Real_s(  -1565070116678345199619059744768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   3427160839432998789209686802432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -3427160839418016439149332004864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   2284773892936142594302573281280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -856790209848085072943446491136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    137086433575298326509532479488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   3427160839435403148460749225984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -7504731765148982897414787563520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   7504731765083469033635428958208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -5003154510015967620856624971776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   1876182941243353642385344036864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -300189270597273476622761066496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -3427160839425289189597581934592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   7504731765096440526462162829312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7504731764979626159327426904064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   5003154509916600198678322348032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -1876182941196742512141966245888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    300189270588617311848350351360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   2284773892944357722972850487296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -5003154510034762268001548894208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   5003154509929143849540456022016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -3335436339890377766072045535232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   1250788627439434071193195380736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -200126180387799163254488432640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -856790209852219095926395895808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   1876182941253537407042735570944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -1876182941205289218334808604672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   1250788627441973397695590563840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -469045735282007069469351870464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     75047317644004332701625614336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    137086433576091804468879818752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -300189270599268184229593219072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    300189270590389302379860197376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -200126180388437724423528644608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     75047317644079249025895759872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -12007570822852549464444370944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[ 8] = ( li_Real_s(   1285185314790297311743051825152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -2814274411939765869806880817152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   2814274411921745841797864620032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1876182941269262851041606500352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    703568602971947175151872770048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   -112570976474956857954069905408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -4398189743945710398587627634688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   9631072431953382303365572067328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -9631072431872888341325672349696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   6420714954530050848575914508288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -2407768107931550399465646981120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    385242897266714157850111770624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   6111770163658232833246526177280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s( -13383438314497336531849626779648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13383438314350321025613744308224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -8922292209474251021906708791296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   3345859578522616394728570093568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -535337532559583262782458429440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -4455309091259073575104062423040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   9756151294650164549989523521536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -9756151294514073650549735030784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   6504100862925294599157281980416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -2439037823569940655234179137536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    390246051767625424410194739200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   1742140093374712769686320185344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -3814905313922524535848993554432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   3814905313859026032902883246080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -2543270209200736450096878583808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    953726328438001326526954471424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -152596212548478861695309053952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(   -285596736618458272481138442240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    625394313756124205353004957696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -625394313744218728631818518528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    416929542488993539929661243392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -156348578431118596228868210688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     25015772548686929627157889024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[ 9] = ( li_Real_s(   -485514452253329456108121620480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   1063170333395101185537726218240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -1063170333383394219043864969216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    708780222247850189232646651904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -265792583340294701626744111104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     42526813334078058697286746112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   2027736829998292303446848569344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -4440299627704281893274851999744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   4440299627651611732682800627712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -2960199751733704825638045614080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   1110074906888704810897861771264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -177611985100623852148845707264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -3370041492107582291240548302848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   7379652902370807094725369135104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7379652902274011227934295064576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   4919768601454778045955676569600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -1844913225525286827909279907840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    295186116081295487878054281216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   2798848018866682101301629681664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -6128864274839106933541304270848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   6128864274749070969790913314816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -4085909516443454758704604250112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   1532216068648038856418540912640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -245154570981234562942942314496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(  -1170946620137031105368599560192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   2564116686407111079767360667648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -2564116686364974275753775464448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   1709411124217530542694363824128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -641029171573246481140029587456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    102564667450610415722540564480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    199917715632976012608790331392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -437776019629651784575041404928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    437776019621741634042030194688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -291850679743042259211473911808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    109444004902108497431089905664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -17511040784134446653208788992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[10] = ( li_Real_s(     85679020985803774664495857664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -187618294128044704813497188352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    187618294125354754014189715456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -125078862748434294633054339072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     46904573530039109739981307904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -7504731764718752221813014528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -399835431266988556819317653504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    875552039263950439540599029760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -875552039251586932663560175616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    583701359493006691787704631296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -218888009807146903079133118464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     35022081568765010545056153600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    742551515209864126286643855360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -1626025215775022639923649839104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1626025215751911011060914585600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1084016810486586033226731487232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    406506303927573733938561024000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -65041008627740435630249738240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -685432167885757753596605104128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   1500946353022080787421365010432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1500946353000321082871771037696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1000630901986658783319415586816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -375236588240542913344567246848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     60037854117882398974488346624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    314156410280842438214397460480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -687933745134598177907164577792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    687933745124331659606620110848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -458622496743260635802248413184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    171983436276678288912970416128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -27517349803993731354249396224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -57119347323765559269800280064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    125078862751641842730160619520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -125078862749705769879414505472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     83385908498631479852028395520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -31269715686609465747997458432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      5003154509807409642860445696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[11] = ( li_Real_s(     -5711934732374463116547719168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     12507886275136626113577484288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -12507886274887296458816487424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(      8338590849759449745052401664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     -3126971568602621661090414592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(       500315450968386810404667392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     28559673661877378833784504320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -62539431375735766388532314112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(     62539431374589776205226967040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    -41692954248986106340006952960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     15634857843118647128067407872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     -2501577254863921741958217728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -57119347323760545496777621504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    125078862751549624490915790848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -125078862749412543322426179584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(     83385908498258437546956554240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -31269715686396336414069358592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(      5003154509761354399307792384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     57119347323766438879102500864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -125078862751621259872488652800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    125078862749615557149378740224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    -83385908498500171775392874496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     31269715686528018324658323456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     -5003154509788607994025476096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    -28559673661887463554434465792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     62539431375848259622193332224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -62539431374905054567421968384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     41692954249360547223869325312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -15634857843322527370182656000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      2501577254906231499150852096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      5711934732378712728989073408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    -12507886275178060109758595072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     12507886275000814437314592768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     -8338590849893520894408523776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      3126971568675517632744521728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -500315450983480081397055488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[12] = ( li_Real_s(   -562854882389117875193758351360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   1926214486396194608706540797952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -2676687662910577236290685632512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   1951230258941434842059504615424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -762981062790331326597718605824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    125078862752395966969326600192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(              -8573955679144479744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(              38768670772946149376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(             -69943073048435990528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(              63085125859108888576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(             -28536562952776269824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(               5199795048301700096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(              15314536944104347648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(             -69147645983277924352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(             124612877735164428288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(            -112329848735047516160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(              50808924106665828352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(              -9258844067609143296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(             -13672179910135246848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(              61637896234678550528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(            -110965868017657757696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(              99984319192067850240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(             -45227058907053998080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(               8242891408100613120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(               6118991471652910080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(             -27550013234299973632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(              49559984445629808640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(             -44645731375749234688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(              20199274587471200256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(              -3682505894704696320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(              -1095144521073004416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(               4927045924926621696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(              -8860595961360388096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(               7982935564398760960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(              -3613245252400302592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(                659004245508286464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[13] = ( li_Real_s(   1285185314788526834139541798912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -4398189743933749963877238439936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   6111770163631862005628458237952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -4455309091232612112543493652480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   1742140093362120142178238857216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   -285596736616133324357880512512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -2814274411934856946213046976512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   9631072431913670687751325876224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s( -13383438314404960948092816850944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   9756151294555811885996297945088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -3814905313877473339926546219008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    625394313747809645647204712448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   2814274411916758668160505217024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -9631072431818301336142221410304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13383438314214787446627842916352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -9756151294372715791345736744960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   3814905313791195630065195941888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -625394313731722717409493123072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -1876182941266943215758533984256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   6420714954489849466502191775744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -8922292209367950308102069288960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   6504100862812294780906928865280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -2543270209146244583455555846144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    416929542478975775139784818688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    703568602971406461721611599872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -2407768107915314078384046211072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   3345859578477750409340798369792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -2439037823521634201206073065472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    953726328414616948411786592256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -156348578426824378799798353920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(   -112570976474894986235751694336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    385242897263890400883750469632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -535337532551577762601086484480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    390246051758939176997648072704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -152596212544259833269393031168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     25015772547910524884366655488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[14] = ( li_Real_s(  -1055352904477904008330638000128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   3611652161977500931564733726720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -5018789367918194629908302397440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   3658556735471668914253174145024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -1430589492709358657627171586048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    234522867656281609442437365760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   3611652161982889488518882525184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s( -12359876287612582424675096723456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  17175412503454441995963897741312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s( -12520394161307281356990577639424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   4895795152787704450146994487296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -802589369305171590014146969600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -5018789367934603495150626799616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  17175412503483314573174969991168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s( -23867131660481467858027485855744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  17398469808413820830567651868672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -6803247809644775377742321418240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   1115286526163712593815207411712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   3658556735490259773514959552512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s( -12520394161348583869173192458240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  17398469808440187154586092437504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s( -12682996682640873759861635022848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   4959376907905642987815446773760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -813012607846619198295254237184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(  -1430589492718665909207086137344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   4895795152809639232132102488064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -6803247809661878923227167719424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   4959376907910115062245425676288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(  -1939243534480490084796945399808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    317908776141281748753577934848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    234522867658024221023253037056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -802589369309327568045279805440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   1115286526166991777293886554112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -813012607847523577395425574912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    317908776141311655469853442048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -52116192809489306661634965504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[15] = ( li_Real_s(    398688875023999241916826779648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1364401927851941855456193937408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   1895987094533059798347200593920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1382121433386000690847730892800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    540444919460752161887599198208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -88597527779845838782402134016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -1665112360391432311766815604736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   5698384522191612958391612211200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -7918534335966341812509143465984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   5772389515884426650362864205824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -2257152310679475611151091367936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    370024968961211745153690107904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   2767369838391382428904247525376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -9470554557981480058585996066816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13160381009020789085836595429376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -9593548772833204176812511330304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   3751323558573862085670723387392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -614971075171351264969151741952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -2298324103065461392787103547392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   7865375819314846386815198298112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s( -10929807956596616726586077478912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   7967523557055211131459633938432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -3115506006253506631890488000512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    510738689545525684876464881664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    961543757402565253552400236544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -3290616414191763218777077972992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   4572674757330438495053975388160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -3333351692217970195939438100480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   1303425982199275415378186993664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -213676390522545643714930278400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(   -164166007361039286808208211968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    561812558518750346304175472640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -780700568321370498439103643648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    569108825497575492005270650880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -222536143300929550864974086144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     36481334967012763329072463872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[16] = ( li_Real_s(    -70356860298080223718170165248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    240776810795884256505123307520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   -334585957855544186577507844096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    243903782358941105342944116736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    -95372632844383288991007375360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     15634857843185020246989733888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    328332014723998232957583097856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -1123625117046054013463146004480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   1561401136657458760460454068224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -1138217651007549393784692277248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    445072286607041689369888948224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    -72962669934894712590134411264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   -609759455915272955710574428160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   2086732360225677270669965918208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -2899744968074375583880693415936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   2113832780440205205513676259328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   -826562817983832243976611561472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    135502101307598518490469761024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    562854882382679205470246273024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -1926214486359637764631267639296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   2676687662834110618217561980928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -1951230258865154278945986707456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    762981062753344388758031564800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -125078862745342045723283161088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -257975154425105778009742245888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    882848306246992221130308190208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -1226815178797099451825101209600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    894313868645062512975751938048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -349699653761373660624243916800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     57327812091524050799910977536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     46904573531783753218285109248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -160517873862868655241680650240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    223057305235463776616633597952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -162602521571518591531831263232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     63581755229210961578918150144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -10423238562073443663580495872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[17] = ( li_Real_s(      4690457353185547106323005440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -16051787386281341577750118400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(     22305730523469724988630630400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    -16260252157026564085737586688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      6358175522842243142189580288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -1042323856189667435454922752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    -23452286765917354492581380096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(     80258936931421766800004612096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -111528652617493408634298695680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     81301260785364403965776625664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    -31790877614355808087070212096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      5211619280980381756471377920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(     46904573531824707827396509696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -160517873862904402563722903552.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(    223057305235282119703538958336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   -162602521571147114931317374976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     63581755228959340541924868096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -10423238562014635184657268736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    -46904573531826273531954462720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    160517873863002531777478656000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   -223057305235607364039128121344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    162602521571557153603640623104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -63581755229188698667478941696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     10423238562062712430093402112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     23452286765919703049418309632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -80258936931568362486312730624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    111528652617984177848379768832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -81301260785986921061144330240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(     31790877614705334037447180288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -5211619281053926989741686784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     -4690457353186280480578732032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     16051787386330529329930305536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -22305730523636564883029360640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     16260252157240240976456318976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -6358175522963022295477518336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      1042323856215170332783280128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[18] = ( li_Real_s(    212634066680037311751517110272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -888059925545665434927067299840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   1475930580483649715230008672256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1225772854977316213212655386624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    512823337286157652445410361344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -87555203926863066471585546240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(               4601084258508541440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(             -20883048753083760640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(              37806271910239207424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(             -34211324189537599488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(              15525243547238862848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(              -2838226773365261824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(              -8279392544356912128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(              37536554196357595136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(             -67899735469091979264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(              61423700204320923648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(             -27878577178398392320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(               5097450791168773120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(               7435966285061312512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(             -33668374540470714368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(              60853386151950745600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(             -55037433948436054016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(              24986602077657018368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(              -4570146025762332160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(              -3341809584472182784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(              15112904015607369728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(             -27298780047586545664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(              24688736490416644096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(             -11212813928172681216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(               2051763054207395072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(                599402188778634752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(              -2708845722351147520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(               4892367772573941760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(              -4426342271269474816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(               2011504533192645632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(               -368086500924583872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[19] = ( li_Real_s(   -485514452252734347638611116032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   2027736829993189725069037797376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -3370041492095445653194738237440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   2798848018854095103293082566656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -1170946620130942801622348070912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    199917715631840225893390745600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   1063170333393976411530790436864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -4440299627688566582375142653952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   7379652902329850233814155001856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -6128864274795444535153947312128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   2564116686385883925873703256064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -437776019625699383320814551040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -1063170333383408574267677212672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   4440299627632166315391721668608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7379652902216475490894823292928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   6128864274685620880540797239296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -2564116686333883112776269824000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    437776019615979419056298000384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    708780222249049694845899112448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -2960199751721057029034528997376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   4919768601411533919383711645696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -4085909516394160046033267064832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   1709411124193171979684800364544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -291850679738537955896661639168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -265792583341143102390922117120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   1110074906884136612763336179712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -1844913225507647354068776517632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   1532216068627441080572808527872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -641029171562999806825343221760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    109444004900212816239501770752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     42526813334264694199031955456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -177611985099889659856468049920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    295186116078225686597675778048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -245154570977593578566073712640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    102564667448788604120152932352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -17511040783796469972969979904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[20] = ( li_Real_s(    398688875024012049028267114496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1665112360388626850673940496384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   2767369838382639816127614550016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -2298324103055539399858052923392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    961543757397614530924524863488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   -164166007360107182422830874624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -1364401927854865817514264231936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   5698384522191378771210988945408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -9470554557965403333816190238720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   7865375819292384683673687949312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -3290616414180066244644889952256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    561812558516570744822016507904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   1895987094541790026224858300416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -7918534335980668888823715856384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13160381009018541789622537551872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s( -10929807956581083811471276638208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   4572674757321096340576948715520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -780700568319674048754468519936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -1382121433395835708008977924096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   5772389515905102676252122152960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -9593548772844501456477770219520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   7967523557053284716719026208768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -3333351692215150942572704169984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    569108825497101417775745728512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    540444919465683603479569891328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -2257152310690474245866060251136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   3751323558581523834536787443712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -3115506006254830690180934926336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   1303425982198999006951057129472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -222536143300901262629814665216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -88597527780784346723499638784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    370024968963300008005906464768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -614971075172823519834836828160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    510738689545805119159594385408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -213676390522508348280516116480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     36481334967010361995677401088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[21] = ( li_Real_s(   -150615797230611547284182138880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    629042447253745081105644519424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -1045450827824230489742016249856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    868255772255856259123734642688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -363249863900976288120386355200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     62018269446214478030694252544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    629042447255282427059693944832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -2627177279697389153357003751424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   4366294633836961597042341183488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -3626244695876855222015356829696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   1517102372756187288977767661568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -259017478274184967382605234176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -1045450827828937032827595128832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   4366294633845102416318766776320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7256658687188462468854517334016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   6026716536779112716143129788416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -2521381408229079314917039800320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    430479752622263895243488231424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    868255772261194432057052233728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -3626244695888378807561891086336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   6026716536786165353159591985152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -5005239157635374578774522724352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   2094028627160330407538369495040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -357517082683938636005948522496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -363249863903656352111136866304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   1517102372762242660151744004096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -2521381408233505227450838155264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   2094028627161365109552757866496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -876073201153172999463237582848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    149573473366726703767594467328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     62018269446725856490726359040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -259017478275326524335027388416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    430479752623084535538088148992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -357517082684119202203508408320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    149573473366720581686851010560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -25536934477084275208850767872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[22] = ( li_Real_s(     26579258334653044548423385088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -111007490690910745421598949376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    184491322555231827729768775680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -153221606866648690231401775104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     64102917157986851421868261376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -10944400490312085736920186880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -124036538894746427505577558016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    518034956556446225903225143296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -860959505256186765497308545024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    715034165376741200503492313088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -299146946737027611973832933376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     51073868954773519307489935360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    230353572232583810641502928896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -962064919317117183679277301760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1598924795472870981879958863872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1327920592840525081942922625024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    555558615368114337551268970496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -94851470915927269070809858048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -212634066675786195178256072704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    888059925521732602819778707456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1475930580433804718979202154496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1225772854927706530042378977280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -512823337262133903920644227072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     87555203922285386148782014464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     97457280559543354464382484480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -407027465863339588635190099968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    676468182697560068073432547328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -561812558507523787226815135744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    235044029578075855535707521024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -40129468464315919803703361536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -17719505556247305495498457088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     74004993793190421843387875328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -122994215035675580146928058368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    102147737910253312108105039872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -42735278105017956336041721856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      7296266993497449975091560448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[23] = ( li_Real_s(     -1771950555631955935236194304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(      7400499379328277202402803712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -12299421503542624609412055040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     10214773790967935384426643456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     -4273527810462150432890290176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(       729626699340505883764326400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(      8859752778146798567025541120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -37002496896622465615923249152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(     61497107517752256864916078592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    -51073868954937821529051758592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     21367639052379855920501358592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     -3648133496718609913816809472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -17719505556280015966424793088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(     74004993793251325991473643520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -122994215035632338553630883840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    102147737910086115971938910208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -42735278104891433334010281984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(      7296266993466356885769682944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     17719505556277133046936764416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    -74004993793295684688584638464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    122994215035805480848680026112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   -102147737910315465301399961600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     42735278105022697430180691968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     -7296266993494209714324504576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     -8859752778141716624281960448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     37002496896685612767729680384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -61497107518008205579676286976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     51073868955281405718592225280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -21367639052577842780291727360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      3648133496760740450614116352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      1771950555629789622451568640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     -7400499379347275663819145216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     12299421503625901620099809280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    -10214773791082612248180424704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      4273527810529130482231148544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -729626699354867979524243456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[24] = ( li_Real_s(    -37523658825855980994446753792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    175110407854018303968996753408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   -325205043157500493266962874368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    300189270606949143762817777664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -137586749028189450125430489088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     25015772550578591004234874880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(               -968050007265307392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(               4409326831579022336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(              -8008926852715527168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(               7271542144071504896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(              -3311759442364600832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(                607867326694908032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(               1754326641737957632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(              -7983092636107497472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(              14490707949792546816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(             -13154444766452748288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(               5992881577648316416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(              -1100378766618573568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(              -1584633452842397696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(               7202205707463848960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(             -13064478481852508160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(              11859093192914380800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(              -5405074073276363776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(                992887107593034496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(                715062841324362624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(              -3246218352718725120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(               5885405904002114560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(              -5342911527469947904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(               2436498330867487232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(               -447837196005290880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(               -128515668333749424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(                583177236700044672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(              -1057243644627052928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(                960151402452189568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(               -438225980568408000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(                 80656654376973968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[25] = ( li_Real_s(     85679020985694456820415856640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -399835431265980876402693505024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    742551515207424301188515889152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -685432167883213782757094195200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    314156410279612111091195183104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -57119347323535866892711362560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -187618294127878599392865812480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    875552039260916983716588290048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -1626025215766894487021176225792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   1500946353013369136892170207232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -687933745130372675556784209920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    125078862750859518216765440000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    187618294125474204957431300096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -875552039247939861390320205824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1626025215740619360895189909504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1500946352987786720683868815360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    687933745118213800987814002688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -125078862748580397738153213952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -125078862748787334622593679360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    583701359490712248515047718912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1084016810478155435461782274048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1000630901976968444296197832704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -458622496738494068177885986816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     83385908497756954691574431744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     46904573530264272129163788288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -218888009806344523473647304704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    406506303924143890978597502976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -375236588236496217973142323200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    171983436274674433369395167232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -31269715686241793457715675136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     -7504731764767278067993280512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     35022081568639697005815267328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -65041008627145283180274122752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     60037854117168314550759456768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -27517349803638061332896415744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      5003154509741712723589201920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[26] = ( li_Real_s(    -70356860298217803409130520576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    328332014723943837918333763584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   -609759455914370546935240065024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    562854882381426501086395498496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -257975154424441497064705097728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     46904573531658971842672066560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    240776810797098293264048455680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -1123625117048561111080707817472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   2086732360226789378302949720064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -1926214486358950684213116928000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    882848306246374524293916655616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -160517873862750365382717997056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   -334585957858513395738084376576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   1561401136665286016612823990272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -2899744968082351458820766564352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   2676687662838444769908952662016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -1226815178798610972450037432320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    223057305235745181224600076288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    243903782362081838317453705216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -1138217651016394885665312997376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   2113832780450426406342970310656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -1951230258871789488571987001344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    894313868647684030171346632704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -162602521572006774694563807232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    -95372632845924364488498216960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    445072286611449165292712755200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   -826562817989072885830498910208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    762981062756865359241705160704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -349699653762791098238214602752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     63581755229473718469677547520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     15634857843477237452303761408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    -72962669935724755908175069184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    135502101308582555009050214400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -125078862746000345325065207808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     57327812091787546562484240384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -10423238562121870553714262016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[27] = ( li_Real_s(     26579258334718100452415635456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -124036538894697714742420570112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    230353572232076839024074948608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -212634066675096053719733633024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     97457280559176610161914544128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -17719505556176384796483649536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -111007490691540967894454108160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    518034956557768313868835094528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -962064919317752331964224897024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    888059925521439446631534559232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -407027465863046221340713418752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     74004993793132094950557614080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    184491322556809706080464535552.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -860959505260414660384990953472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1598924795477328982561102233600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1475930580436375992891453997056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    676468182698501601870529691648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -122994215035849566866907332608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -153221606868333880916968472576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    715034165381535282306828206080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1327920592846178788325132861440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1225772854931490961104253747200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -561812558509051281556679688192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    102147737910537003700257292288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     64102917158817642407815806976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -299146946739414132748246319104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    555558615370988548907205656576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -512823337264106973138641747968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    235044029578883160953285771264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -42735278105168123236116856832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -10944400490470327450389708800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     51073868955223545018692141056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -94851470916464710354466766848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     87555203922650934182598934528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -40129468464464626552336809984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      7296266993524971850646421504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[28] = ( li_Real_s(     -4690457353147286850455470080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     21888800981195543686330449920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -40650630393418417376661078016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     37523658824526847358326013952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    -17198343627846289743992061952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      3126971568689414909963796480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     21888800981270530379344773120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   -102147737911924833665115226112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    189702941835492778137465389056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   -175110407847488014140565356544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     80258936929857012220097986560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    -14592533987207382771274088448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -40650630393650643028540391424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    189702941835891311520115589120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -352305463407966660555880529920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    325205043144760021027364274176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   -149052311440963801240192417792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     27100420261929750286900920320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     37523658824792594920712962048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -175110407848052371468870221824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    325205043145098283580626305024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   -300189270594618287983077359616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    137586749022222321356652937216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    -25015772549442465639253934080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    -17198343627980834782859755520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     80258936930154795153271816192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   -149052311441175452830492786688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    137586749022270260063623970816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -63060593301745900702509563904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     11465562418477082521431441408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      3126971568715377128274657280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    -14592533987264528788616118272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     27100420261970075975361232896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    -25015772549451481634601697280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     11465562418477141895059341312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     -2084647712446590073035554816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[29] = ( li_Real_s(       312697156873724022979821568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     -1459253398730618983394836480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(      2710042026194295542155051008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     -2501577254934140402798690304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      1146556241839388467558088704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      -208464771242660191371526144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     -1563485784364566834002788352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(      7296266993644278757864767488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    -13550210130973565683356401664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     12507886274688005578235576320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     -5732781209210817075021348864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      1042323856216662782379032576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(      3126971568724696588831686656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    -14592533987285085258009018368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(     27100420261969411870338056192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    -25015772549419455059907837952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     11465562418450201661155573760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     -2084647712439777773867761664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     -3126971568723280417855111168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     14592533987292856606194139136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    -27100420262006293888380174336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(     25015772549470674709576155136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -11465562418480121571570614272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(      2084647712446194248849555456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      1563485784362210855462371328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     -7296266993654739511491428352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     13550210131027301015629070336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -12507886274764142360412553216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(      5732781209255683746504376320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -1042323856226317456543580160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      -312697156872765867315691520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      1459253398733330653946839040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     -2710042026211210429036756992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(      2501577254959113060644552704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -1146556241854383882015604736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(       208464771245913646278115328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[30] = ( li_Real_s(      2501577255055671622040027136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -12507886275285053586257477632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(     25015772550582782342559956992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    -25015772550594388787302760448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     12507886275302311520767049728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -2501577255061315415225401344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(                 71445203916191176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(               -326279456486275136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(                594052128603605120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(               -540597577289850560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(                246808620534074848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(                -45428919277745568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(               -130253677999630000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(                594361027046366208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(              -1081543243933652352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(                984163704413224960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(               -449488275077819456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(                 82760465551510496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(                118194480376958784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(               -538716413554154496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(                979692546157993088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(               -891508675740680192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(                407388595952675264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(                -75050533192791424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(                -53494150615275520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(                243545807492899392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(               -442702353350726912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(                402929081652386624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(               -184241395454544064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(                 33963010275260468.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(                  9631034454616364.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(                -43819982660120688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(                 79650250088057712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(                -72532578882590032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(                 33195550442918112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(                 -6124273442881405.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[31] = ( li_Real_s(     -5711934732375862794849878016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     28559673661841838219928272896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -57119347323645387046930874368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     57119347323634435911118225408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    -28559673661821119022814461952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      5711934732366098032083599360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     12507886275168791226736443392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -62539431375694380770862825472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    125078862751257488649462218752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   -125078862751244417655231217664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     62539431375653909946867646464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    -12507886275141362809669943296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -12507886274979439931270627328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(     62539431374662625447636893696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -125078862749155890920224194560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    125078862749189791062731784192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -62539431374674359435728519168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     12507886274957260582715129856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(      8338590849863597685458599936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    -41692954249135991765105377280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(     83385908498173590433664335872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    -83385908498249623861748301824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     41692954249214074682863517696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     -8338590849865629582946729984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     -3126971568655695637118976000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     15634857843208206748196274176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -31269715686393169820581363712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     31269715686445163526435635200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -15634857843269814583723819008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      3126971568665315264350388224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(       500315450978685042468847616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     -2501577254882618937188548608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(      5003154509764041605726076928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     -5003154509776015287352557568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      2501577254897574494349557760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -500315450981726222911799296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[32] = ( li_Real_s(      4690457353207276754622742528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -23452286765963635136017727488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(     46904573531845932799859097600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    -46904573531812525238560751616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     23452286765906456133326864384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -4690457353183497616648830976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    -16051787386428236331220992000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(     80258936931851491128511561728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -160517873863417285155661807616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    160517873863339879537066377216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    -80258936931700075183227273216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     16051787386354298572299567104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(     22305730523799600467195985920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -111528652618540794614825091072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(    223057305236695651852207783936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   -223057305236669826523094581248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    111528652618434836878279573504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -22305730523719459263670648832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    -16260252157364327460720607232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     81301260786462314704622583808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   -162602521572679148045181386752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    162602521572744766899127058432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -81301260786498325909455503360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     16260252157334550486817177600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      6358175523006282580472365056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -31790877614890702901797191680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     63581755229710245411044720640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -63581755229774201803409195008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(     31790877614957931440765927424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -6358175523009556926099881984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     -1042323856220662118486114304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      5211619281081356506319814656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -10423238562155531002686996480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     10423238562172127031196647424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -5211619281100977291317477376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      1042323856223778271878184960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[33] = ( li_Real_s(     -1771950555642354291577978880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(      8859752778169012000441499648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -17719505556289207883633000448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     17719505556268726181030789120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     -8859752778134143188189839360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      1771950555628040024573870080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(      7400499379403814750742642688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -37002496896848745108919549952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(     74004993793530338062138081280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    -74004993793486392781399130112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     37002496896761905680557801472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     -7400499379360921702631473152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -12299421503716052777484419072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(     61497107518313878608291037184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -122994215036409702070561538048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    122994215036404864219399323648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -61497107518266423686436225024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     12299421503673444502884843520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     10214773791147151381707620352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    -51073868955529569891027779584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    102147737910927831705420038144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   -102147737910980995291646263296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     51073868955571711972697178112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    -10214773791136202444918226944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     -4273527810549710591124045824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     21367639052668416150141403136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -42735278105302624294519439360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     42735278105348645453211631616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -21367639052720080002507341824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      4273527810555351085774536704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(       729626699357153039564668928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     -3648133496772975816008007680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(      7296266993543343590434930688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     -7296266993554878566921928704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      3648133496787081450680745984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -729626699359719299703898112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[34] = ( li_Real_s(       312697156874973824103219200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     -1563485784364555838886510592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(      3126971568717537668623237120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     -3126971568712778982298222592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      1563485784356426324788641792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      -312697156871603546086178816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     -1459253398742451377777147904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(      7296266993672112894721916928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    -14592533987305850634611195904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     14592533987297065536705265664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     -7296266993653873096328740864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      1459253398732992554121297920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(      2710042026223837220570136576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    -13550210131057330877206888448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(     27100420262066947347814809600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    -27100420262070377824093470720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     13550210131052693137160929280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     -2710042026215772852536213504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     -2501577254965779949399572480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     12507886274781721352317435904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    -25015772549537107202126381056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(     25015772549554967669007974400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -12507886274799337727617662976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(      2501577254965546303178670080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      1146556241855102550303309824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     -5732781209257492443132067840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     11465562418509447745706655744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -11465562418523125670356189184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(      5732781209273759717665013760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -1146556241857689563724513280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      -208464771245676323565207552.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      1042323856225541476212277248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     -2084647712450987294912937984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(      2084647712454279507604406272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -1042323856229693919313526784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(       208464771246528273278042112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[35] = ( li_Real_s(       -20846477124776349728243712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(       104232385623014444420300800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(      -208464771245073172717895680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(       208464771244689133922156544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      -104232385622355166940364800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(        20846477124499573445754880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(       104232385623480242213486592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(      -521161928114097779921387520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(      1042323856225112529238491136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     -1042323856224506698331586560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(       521161928112762010732593152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      -104232385622750166492643328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(      -208464771246525421419757568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(      1042323856227607321121914880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(     -2084647712451554642912870400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(      2084647712452168720156983296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     -1042323856227701054488182784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(       208464771246005043182174208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(       208464771246367366623264768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     -1042323856228067604177092608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(      2084647712454300673203240960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(     -2084647712456153075418136576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(      1042323856230068852778598400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(      -208464771246516831485165568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      -104232385623216359422820352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(       521161928114667807980912640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     -1042323856229062524761276416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(      1042323856230390459929722880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(      -521161928116298452444381184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(       104232385623519171797057536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(        20846477124667510961995776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      -104232385623124704820723712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(       208464771246279749290426368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(      -208464771246592079312191488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(       104232385623526078104469504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(       -20846477124757082504953856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];

        /* Prepare interpolated value computation */
        liTX1 = ( liX + li_Real_s( 2.0 ) ) - liPXmm;
        liTX2 = liTX1 * liTX1; 
        liTX3 = liTX1 * liTX2; 
        liTX4 = liTX1 * liTX3; 
        liTX5 = liTX1 * liTX4;
        liTY1 = ( liY + li_Real_s( 2.0 ) ) - liPYmm; 
        liTY2 = liTY1 * liTY1; 
        liTY3 = liTY1 * liTY2; 
        liTY4 = liTY1 * liTY3; 
        liTY5 = liTY1 * liTY4;

        /* Compute interpolated value */
        liIV = liVC[ 0]                 + 
               liVC[ 1] * liTY1         + 
               liVC[ 2] * liTY2         + 
               liVC[ 3] * liTY3         + 
               liVC[ 4] * liTY4         + 
               liVC[ 5] * liTY5         +
               liVC[ 6] * liTX1         + 
               liVC[ 7] * liTY1 * liTX1 + 
               liVC[ 8] * liTY2 * liTX1 + 
               liVC[ 9] * liTY3 * liTX1 + 
               liVC[10] * liTY4 * liTX1 + 
               liVC[11] * liTY5 * liTX1 +
               liVC[12] * liTX2         + 
               liVC[13] * liTY1 * liTX2 + 
               liVC[14] * liTY2 * liTX2 + 
               liVC[15] * liTY3 * liTX2 + 
               liVC[16] * liTY4 * liTX2 + 
               liVC[17] * liTY5 * liTX2 +
               liVC[18] * liTX3         + 
               liVC[19] * liTY1 * liTX3 + 
               liVC[20] * liTY2 * liTX3 + 
               liVC[21] * liTY3 * liTX3 + 
               liVC[22] * liTY4 * liTX3 + 
               liVC[23] * liTY5 * liTX3 +
               liVC[24] * liTX4         + 
               liVC[25] * liTY1 * liTX4 + 
               liVC[26] * liTY2 * liTX4 + 
               liVC[27] * liTY3 * liTX4 + 
               liVC[28] * liTY4 * liTX4 + 
               liVC[29] * liTY5 * liTX4 +
               liVC[30] * liTX5         + 
               liVC[31] * liTY1 * liTX5 + 
               liVC[32] * liTY2 * liTX5 + 
               liVC[33] * liTY3 * liTX5 + 
               liVC[34] * liTY4 * liTX5 + 
               liVC[35] * liTY5 * liTX5;

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




    float li_bipenticf_f(

        float     const * const liBytes, 
        li_Size_t               liWidth,
        li_Size_t const         liHeight,
        li_Size_t const         liLayer, 
        li_Size_t const         liChannel,
        li_Real_t const         liX,
        li_Real_t const         liY

    ) {

        /* Interpolation vectors variables */
        li_Real_t liVS[36] = { li_Real_s( 0.0 ) };
        li_Real_t liVC[36] = { li_Real_s( 0.0 ) };

        /* Optimization variables */
        li_Real_t liTX1 = li_Real_s( 0.0 );
        li_Real_t liTY1 = li_Real_s( 0.0 );
        li_Real_t liTX2 = li_Real_s( 0.0 );
        li_Real_t liTY2 = li_Real_s( 0.0 );
        li_Real_t liTX3 = li_Real_s( 0.0 );
        li_Real_t liTY3 = li_Real_s( 0.0 );
        li_Real_t liTX4 = li_Real_s( 0.0 );
        li_Real_t liTY4 = li_Real_s( 0.0 );
        li_Real_t liTX5 = li_Real_s( 0.0 );
        li_Real_t liTY5 = li_Real_s( 0.0 );

        /* Interpolation reference variables */
        li_Size_t liPXrf = li_Size_s( 0 );
        li_Size_t liPYrf = li_Size_s( 0 );
        li_Size_t liPXmm = li_Size_s( 0 );
        li_Size_t liPYmm = li_Size_s( 0 );

        /* Interpolation sampling variables */
        li_Size_t liPXm2 = li_Size_s( 0 );
        li_Size_t liPXm1 = li_Size_s( 0 );
        li_Size_t liPXp1 = li_Size_s( 0 );
        li_Size_t liPXp2 = li_Size_s( 0 );
        li_Size_t liPXp3 = li_Size_s( 0 );
        li_Size_t liPYm2 = li_Size_s( 0 );
        li_Size_t liPYm1 = li_Size_s( 0 );
        li_Size_t liPYp1 = li_Size_s( 0 );
        li_Size_t liPYp2 = li_Size_s( 0 );
        li_Size_t liPYp3 = li_Size_s( 0 );

        /* Interpolated variables */
        li_Real_t liIV = li_Real_s( 0.0 );

        /* Compute relatlive grid parameters */
        liPXrf = li_Floor( liX );
        liPYrf = li_Floor( liY );

        /* Memorize reference point */
        liPXmm = liPXrf;
        liPYmm = liPYrf;

        /* Compute sampling nodes */
        liPXm2 = liPXrf - li_Size_s( 2 );
        liPXm1 = liPXrf - li_Size_s( 1 );
        liPYm2 = liPYrf - li_Size_s( 2 );
        liPYm1 = liPYrf - li_Size_s( 1 );
        liPXp1 = liPXrf + li_Size_s( 1 );
        liPXp2 = liPXrf + li_Size_s( 2 );
        liPXp3 = liPXrf + li_Size_s( 3 );
        liPYp1 = liPYrf + li_Size_s( 1 );
        liPYp2 = liPYrf + li_Size_s( 2 );
        liPYp3 = liPYrf + li_Size_s( 3 );

        /* Boundaries analysis */
        if ( liPXm2 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPXm2 = li_Size_s( 0 );

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

                            /* Boundaries analysis */
                            if ( liPXp3 < li_Size_s( 0 ) ) {

                                /* Boundary condition correction */
                                liPXp3 = li_Size_s( 0 );

                            }

                        }

                    }

                }

            }

        } else
        if ( liPXp3 >= liWidth ) {

            /* Boundary condition correction */
            liPXp3 = liWidth - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPXp2 >= liWidth ) {

                /* Boundary condition correction */
                liPXp2 = liPXp3;

                /* Boundaries analysis */
                if ( liPXp1 >= liWidth ) {

                    /* Boundary condition correction */
                    liPXp1 = liPXp3;

                    /* Boundaries analysis */
                    if ( liPXrf >= liWidth ) {

                        /* Boundary condition correction */
                        liPXrf = liPXp3;

                        /* Boundaries analysis */
                        if ( liPXm1 >= liWidth ) {

                            /* Boundary condition correction */
                            liPXm1 = liPXp3;

                            /* Boundaries analysis */
                            if ( liPXm2 >= liWidth ) {

                                /* Boundary condition correction */
                                liPXm2 = liPXp3;

                            }

                        }

                    }

                }

            }

        }

        /* Boundaries analysis */
        if ( liPYm2 < li_Size_s( 0 ) ) {

            /* Boundary condition correction */
            liPYm2 = li_Size_s( 0 );

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

                            /* Boundaries analysis */
                            if ( liPYp3 < li_Size_s( 0 ) ) {

                                /* Boundary condition correction */
                                liPYp3 = li_Size_s( 0 );

                            }

                        }

                    }

                }

            }

        } else
        if ( liPYp3 >= liHeight ) {

            /* Boundary condition correction */
            liPYp3 = liHeight - li_Size_s( 1 );

            /* Boundaries analysis */
            if ( liPYp2 >= liHeight ) {

                /* Boundary condition correction */
                liPYp2 = liPYp3;

                /* Boundaries analysis */
                if ( liPYp1 >= liHeight ) {

                    /* Boundary condition correction */
                    liPYp1 = liPYp3;

                    /* Boundaries analysis */
                    if ( liPYrf >= liHeight ) {

                        /* Boundary condition correction */
                        liPYrf = liPYp3;

                        /* Boundaries analysis */
                        if ( liPYm1 >= liHeight ) {

                            /* Boundary condition correction */
                            liPYm1 = liPYp3;

                            /* Boundaries analysis */
                            if ( liPYm2 >= liHeight ) {

                                /* Boundary condition correction */
                                liPYm2 = liPYp3;

                            }

                        }

                    }

                }

            }

        }

        /* Compute memory width */
        liWidth *= liLayer; if ( liWidth % li_Size_s( 4 ) ) liWidth += li_Size_s( 4 ) - liWidth % li_Size_s( 4 );

        /* Compute interpolation vector */
        liVS[ 0] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXm2 + liChannel );
        liVS[ 1] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXm1 + liChannel );
        liVS[ 2] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXrf + liChannel );
        liVS[ 3] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXp1 + liChannel );
        liVS[ 4] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXp2 + liChannel );
        liVS[ 5] = * ( liBytes + liWidth * liPYm2 + liLayer * liPXp3 + liChannel );
        liVS[ 6] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXm2 + liChannel );
        liVS[ 7] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXm1 + liChannel );
        liVS[ 8] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXrf + liChannel );
        liVS[ 9] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp1 + liChannel );
        liVS[10] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp2 + liChannel );
        liVS[11] = * ( liBytes + liWidth * liPYm1 + liLayer * liPXp3 + liChannel );
        liVS[12] = * ( liBytes + liWidth * liPYrf + liLayer * liPXm2 + liChannel );
        liVS[13] = * ( liBytes + liWidth * liPYrf + liLayer * liPXm1 + liChannel );
        liVS[14] = * ( liBytes + liWidth * liPYrf + liLayer * liPXrf + liChannel );
        liVS[15] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp1 + liChannel );
        liVS[16] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp2 + liChannel );
        liVS[17] = * ( liBytes + liWidth * liPYrf + liLayer * liPXp3 + liChannel );
        liVS[18] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXm2 + liChannel );
        liVS[19] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXm1 + liChannel );
        liVS[20] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXrf + liChannel );
        liVS[21] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp1 + liChannel );
        liVS[22] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp2 + liChannel );
        liVS[23] = * ( liBytes + liWidth * liPYp1 + liLayer * liPXp3 + liChannel );
        liVS[24] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXm2 + liChannel );
        liVS[25] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXm1 + liChannel );
        liVS[26] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXrf + liChannel );
        liVS[27] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp1 + liChannel );
        liVS[28] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp2 + liChannel );
        liVS[29] = * ( liBytes + liWidth * liPYp2 + liLayer * liPXp3 + liChannel );
        liVS[30] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXm2 + liChannel );
        liVS[31] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXm1 + liChannel );
        liVS[32] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXrf + liChannel );
        liVS[33] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXp1 + liChannel );
        liVS[34] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXp2 + liChannel );
        liVS[35] = * ( liBytes + liWidth * liPYp3 + liLayer * liPXp3 + liChannel );

        /* Compute interpolation matrix product */
        liVC[ 0] = ( li_Real_s(   -300189270608550490089696788480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0];
        liVC[ 1] = ( li_Real_s(    685432167889520651822761508864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1500946353042737954487183343616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   1500946353042724162213324521472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -1000630902028473439222155444224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    375236588260674003678913363968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -60037854121707283268172251136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 2] = ( li_Real_s(   -562854882391026838485810020352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   1926214486404840394091185307648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -2676687662926191779148732563456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   1951230258955527449718477029376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -762981062796707438507656740864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    125078862753557930857560276992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 3] = ( li_Real_s(    212634066681053577154930933760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -888059925550281202332656992256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   1475930580492012055313105551360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -1225772854984886482711288479744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    512823337289593124904908095488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -87555203927491107513371197440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 4] = ( li_Real_s(    -37523658826068142758142410752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    175110407854984748301532790784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   -325205043159256897121637367808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    300189270608544262455837065216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -137586749028915901855948603392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     25015772550711917784218992640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 5] = ( li_Real_s(      2501577255071203323293990912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -12507886275356033658900185088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(     25015772550712067317800370176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    -25015772550712062919753859072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     12507886275356033658900185088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     -2501577255071209920363757568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30];
        liVC[ 6] = ( li_Real_s(    685432167888330153408763789312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1500946353037374167330985082880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   1500946353033083925735961264128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1000630902019816535575930863616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    375236588256777967788772818944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -60037854121001256065651703808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(               5307535142640928768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(             -23890720206887014400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(              42905267744887128064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(             -38515568462088527872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(              17335721846887892992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(              -3142236065440415744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(              -9424565637665363968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(              42357221425809219584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(             -75981003189760147456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(              68164328424607531008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(             -30676299989447696384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(               5560318966456455168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(               8369701100107887616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(             -37547909685822480384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(              67265149446485196800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(             -60300096274281152512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(              27130102676398624768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(              -4916947262888051712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(              -3729428933099930624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(              16703063155371606016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(             -29888172368504315904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(              26776243676818083840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(             -12044190816380643328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(               2182485285795192064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(                665298017487659264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(              -2975846611402234880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(               5320440590865428480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(              -4764568063767093248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(               2142852871757383168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(               -388176804941194304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[ 7] = ( li_Real_s(  -1565070116678345199619059744768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   3427160839432998789209686802432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -3427160839418016439149332004864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   2284773892936142594302573281280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -856790209848085072943446491136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    137086433575298326509532479488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   3427160839435403148460749225984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -7504731765148982897414787563520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   7504731765083469033635428958208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -5003154510015967620856624971776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   1876182941243353642385344036864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -300189270597273476622761066496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -3427160839425289189597581934592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   7504731765096440526462162829312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7504731764979626159327426904064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   5003154509916600198678322348032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -1876182941196742512141966245888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    300189270588617311848350351360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   2284773892944357722972850487296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -5003154510034762268001548894208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   5003154509929143849540456022016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -3335436339890377766072045535232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   1250788627439434071193195380736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -200126180387799163254488432640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -856790209852219095926395895808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   1876182941253537407042735570944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -1876182941205289218334808604672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   1250788627441973397695590563840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -469045735282007069469351870464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     75047317644004332701625614336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    137086433576091804468879818752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -300189270599268184229593219072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    300189270590389302379860197376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -200126180388437724423528644608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     75047317644079249025895759872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -12007570822852549464444370944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[ 8] = ( li_Real_s(   1285185314790297311743051825152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -2814274411939765869806880817152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   2814274411921745841797864620032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1876182941269262851041606500352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    703568602971947175151872770048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   -112570976474956857954069905408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -4398189743945710398587627634688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   9631072431953382303365572067328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -9631072431872888341325672349696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   6420714954530050848575914508288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -2407768107931550399465646981120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    385242897266714157850111770624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   6111770163658232833246526177280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s( -13383438314497336531849626779648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13383438314350321025613744308224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -8922292209474251021906708791296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   3345859578522616394728570093568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -535337532559583262782458429440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -4455309091259073575104062423040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   9756151294650164549989523521536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -9756151294514073650549735030784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   6504100862925294599157281980416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -2439037823569940655234179137536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    390246051767625424410194739200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   1742140093374712769686320185344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -3814905313922524535848993554432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   3814905313859026032902883246080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -2543270209200736450096878583808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    953726328438001326526954471424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -152596212548478861695309053952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(   -285596736618458272481138442240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    625394313756124205353004957696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -625394313744218728631818518528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    416929542488993539929661243392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -156348578431118596228868210688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     25015772548686929627157889024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[ 9] = ( li_Real_s(   -485514452253329456108121620480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   1063170333395101185537726218240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -1063170333383394219043864969216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    708780222247850189232646651904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -265792583340294701626744111104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     42526813334078058697286746112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   2027736829998292303446848569344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -4440299627704281893274851999744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   4440299627651611732682800627712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -2960199751733704825638045614080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   1110074906888704810897861771264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -177611985100623852148845707264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -3370041492107582291240548302848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   7379652902370807094725369135104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7379652902274011227934295064576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   4919768601454778045955676569600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -1844913225525286827909279907840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    295186116081295487878054281216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   2798848018866682101301629681664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -6128864274839106933541304270848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   6128864274749070969790913314816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -4085909516443454758704604250112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   1532216068648038856418540912640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -245154570981234562942942314496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(  -1170946620137031105368599560192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   2564116686407111079767360667648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -2564116686364974275753775464448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   1709411124217530542694363824128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -641029171573246481140029587456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    102564667450610415722540564480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    199917715632976012608790331392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -437776019629651784575041404928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    437776019621741634042030194688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -291850679743042259211473911808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    109444004902108497431089905664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -17511040784134446653208788992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[10] = ( li_Real_s(     85679020985803774664495857664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -187618294128044704813497188352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    187618294125354754014189715456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -125078862748434294633054339072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     46904573530039109739981307904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -7504731764718752221813014528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -399835431266988556819317653504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    875552039263950439540599029760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -875552039251586932663560175616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    583701359493006691787704631296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -218888009807146903079133118464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     35022081568765010545056153600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    742551515209864126286643855360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -1626025215775022639923649839104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1626025215751911011060914585600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1084016810486586033226731487232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    406506303927573733938561024000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -65041008627740435630249738240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -685432167885757753596605104128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   1500946353022080787421365010432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1500946353000321082871771037696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1000630901986658783319415586816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -375236588240542913344567246848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     60037854117882398974488346624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    314156410280842438214397460480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -687933745134598177907164577792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    687933745124331659606620110848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -458622496743260635802248413184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    171983436276678288912970416128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -27517349803993731354249396224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -57119347323765559269800280064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    125078862751641842730160619520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -125078862749705769879414505472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     83385908498631479852028395520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -31269715686609465747997458432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      5003154509807409642860445696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[11] = ( li_Real_s(     -5711934732374463116547719168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     12507886275136626113577484288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -12507886274887296458816487424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(      8338590849759449745052401664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     -3126971568602621661090414592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(       500315450968386810404667392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     28559673661877378833784504320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -62539431375735766388532314112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(     62539431374589776205226967040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    -41692954248986106340006952960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     15634857843118647128067407872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     -2501577254863921741958217728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -57119347323760545496777621504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    125078862751549624490915790848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -125078862749412543322426179584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(     83385908498258437546956554240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -31269715686396336414069358592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(      5003154509761354399307792384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     57119347323766438879102500864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -125078862751621259872488652800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    125078862749615557149378740224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    -83385908498500171775392874496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     31269715686528018324658323456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     -5003154509788607994025476096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    -28559673661887463554434465792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     62539431375848259622193332224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -62539431374905054567421968384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     41692954249360547223869325312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -15634857843322527370182656000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      2501577254906231499150852096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      5711934732378712728989073408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    -12507886275178060109758595072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     12507886275000814437314592768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     -8338590849893520894408523776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      3126971568675517632744521728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -500315450983480081397055488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[12] = ( li_Real_s(   -562854882389117875193758351360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   1926214486396194608706540797952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -2676687662910577236290685632512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   1951230258941434842059504615424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -762981062790331326597718605824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    125078862752395966969326600192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(              -8573955679144479744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(              38768670772946149376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(             -69943073048435990528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(              63085125859108888576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(             -28536562952776269824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(               5199795048301700096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(              15314536944104347648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(             -69147645983277924352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(             124612877735164428288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(            -112329848735047516160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(              50808924106665828352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(              -9258844067609143296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(             -13672179910135246848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(              61637896234678550528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(            -110965868017657757696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(              99984319192067850240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(             -45227058907053998080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(               8242891408100613120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(               6118991471652910080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(             -27550013234299973632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(              49559984445629808640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(             -44645731375749234688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(              20199274587471200256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(              -3682505894704696320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(              -1095144521073004416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(               4927045924926621696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(              -8860595961360388096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(               7982935564398760960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(              -3613245252400302592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(                659004245508286464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[13] = ( li_Real_s(   1285185314788526834139541798912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -4398189743933749963877238439936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   6111770163631862005628458237952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -4455309091232612112543493652480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   1742140093362120142178238857216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   -285596736616133324357880512512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -2814274411934856946213046976512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   9631072431913670687751325876224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s( -13383438314404960948092816850944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   9756151294555811885996297945088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -3814905313877473339926546219008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    625394313747809645647204712448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   2814274411916758668160505217024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -9631072431818301336142221410304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13383438314214787446627842916352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -9756151294372715791345736744960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   3814905313791195630065195941888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -625394313731722717409493123072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -1876182941266943215758533984256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   6420714954489849466502191775744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -8922292209367950308102069288960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   6504100862812294780906928865280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -2543270209146244583455555846144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    416929542478975775139784818688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    703568602971406461721611599872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -2407768107915314078384046211072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   3345859578477750409340798369792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -2439037823521634201206073065472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    953726328414616948411786592256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -156348578426824378799798353920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(   -112570976474894986235751694336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    385242897263890400883750469632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -535337532551577762601086484480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    390246051758939176997648072704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -152596212544259833269393031168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     25015772547910524884366655488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[14] = ( li_Real_s(  -1055352904477904008330638000128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   3611652161977500931564733726720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -5018789367918194629908302397440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   3658556735471668914253174145024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -1430589492709358657627171586048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    234522867656281609442437365760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   3611652161982889488518882525184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s( -12359876287612582424675096723456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  17175412503454441995963897741312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s( -12520394161307281356990577639424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   4895795152787704450146994487296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -802589369305171590014146969600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -5018789367934603495150626799616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  17175412503483314573174969991168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s( -23867131660481467858027485855744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  17398469808413820830567651868672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -6803247809644775377742321418240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   1115286526163712593815207411712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   3658556735490259773514959552512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s( -12520394161348583869173192458240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  17398469808440187154586092437504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s( -12682996682640873759861635022848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   4959376907905642987815446773760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -813012607846619198295254237184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(  -1430589492718665909207086137344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   4895795152809639232132102488064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -6803247809661878923227167719424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   4959376907910115062245425676288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(  -1939243534480490084796945399808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    317908776141281748753577934848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    234522867658024221023253037056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -802589369309327568045279805440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   1115286526166991777293886554112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -813012607847523577395425574912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    317908776141311655469853442048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -52116192809489306661634965504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[15] = ( li_Real_s(    398688875023999241916826779648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1364401927851941855456193937408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   1895987094533059798347200593920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1382121433386000690847730892800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    540444919460752161887599198208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -88597527779845838782402134016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -1665112360391432311766815604736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   5698384522191612958391612211200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -7918534335966341812509143465984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   5772389515884426650362864205824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -2257152310679475611151091367936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    370024968961211745153690107904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   2767369838391382428904247525376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -9470554557981480058585996066816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13160381009020789085836595429376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -9593548772833204176812511330304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   3751323558573862085670723387392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -614971075171351264969151741952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -2298324103065461392787103547392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   7865375819314846386815198298112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s( -10929807956596616726586077478912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   7967523557055211131459633938432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -3115506006253506631890488000512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    510738689545525684876464881664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    961543757402565253552400236544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -3290616414191763218777077972992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   4572674757330438495053975388160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -3333351692217970195939438100480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   1303425982199275415378186993664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -213676390522545643714930278400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(   -164166007361039286808208211968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    561812558518750346304175472640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -780700568321370498439103643648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    569108825497575492005270650880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -222536143300929550864974086144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     36481334967012763329072463872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[16] = ( li_Real_s(    -70356860298080223718170165248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    240776810795884256505123307520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   -334585957855544186577507844096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    243903782358941105342944116736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    -95372632844383288991007375360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     15634857843185020246989733888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    328332014723998232957583097856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -1123625117046054013463146004480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   1561401136657458760460454068224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -1138217651007549393784692277248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    445072286607041689369888948224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    -72962669934894712590134411264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   -609759455915272955710574428160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   2086732360225677270669965918208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -2899744968074375583880693415936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   2113832780440205205513676259328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   -826562817983832243976611561472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    135502101307598518490469761024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    562854882382679205470246273024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -1926214486359637764631267639296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   2676687662834110618217561980928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -1951230258865154278945986707456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    762981062753344388758031564800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -125078862745342045723283161088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -257975154425105778009742245888.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    882848306246992221130308190208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -1226815178797099451825101209600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    894313868645062512975751938048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -349699653761373660624243916800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     57327812091524050799910977536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     46904573531783753218285109248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -160517873862868655241680650240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    223057305235463776616633597952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -162602521571518591531831263232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     63581755229210961578918150144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -10423238562073443663580495872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[17] = ( li_Real_s(      4690457353185547106323005440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -16051787386281341577750118400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(     22305730523469724988630630400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    -16260252157026564085737586688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      6358175522842243142189580288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -1042323856189667435454922752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    -23452286765917354492581380096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(     80258936931421766800004612096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -111528652617493408634298695680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     81301260785364403965776625664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    -31790877614355808087070212096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      5211619280980381756471377920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(     46904573531824707827396509696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -160517873862904402563722903552.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(    223057305235282119703538958336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   -162602521571147114931317374976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     63581755228959340541924868096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -10423238562014635184657268736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    -46904573531826273531954462720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    160517873863002531777478656000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   -223057305235607364039128121344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    162602521571557153603640623104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -63581755229188698667478941696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     10423238562062712430093402112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     23452286765919703049418309632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -80258936931568362486312730624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    111528652617984177848379768832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -81301260785986921061144330240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(     31790877614705334037447180288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -5211619281053926989741686784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     -4690457353186280480578732032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     16051787386330529329930305536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -22305730523636564883029360640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     16260252157240240976456318976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -6358175522963022295477518336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      1042323856215170332783280128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[18] = ( li_Real_s(    212634066680037311751517110272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -888059925545665434927067299840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   1475930580483649715230008672256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -1225772854977316213212655386624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    512823337286157652445410361344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -87555203926863066471585546240.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(               4601084258508541440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(             -20883048753083760640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(              37806271910239207424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(             -34211324189537599488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(              15525243547238862848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(              -2838226773365261824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(              -8279392544356912128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(              37536554196357595136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(             -67899735469091979264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(              61423700204320923648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(             -27878577178398392320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(               5097450791168773120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(               7435966285061312512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(             -33668374540470714368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(              60853386151950745600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(             -55037433948436054016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(              24986602077657018368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(              -4570146025762332160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(              -3341809584472182784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(              15112904015607369728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(             -27298780047586545664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(              24688736490416644096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(             -11212813928172681216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(               2051763054207395072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(                599402188778634752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(              -2708845722351147520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(               4892367772573941760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(              -4426342271269474816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(               2011504533192645632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(               -368086500924583872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[19] = ( li_Real_s(   -485514452252734347638611116032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   2027736829993189725069037797376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -3370041492095445653194738237440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   2798848018854095103293082566656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(  -1170946620130942801622348070912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    199917715631840225893390745600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   1063170333393976411530790436864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -4440299627688566582375142653952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   7379652902329850233814155001856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -6128864274795444535153947312128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   2564116686385883925873703256064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -437776019625699383320814551040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -1063170333383408574267677212672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   4440299627632166315391721668608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7379652902216475490894823292928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   6128864274685620880540797239296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -2564116686333883112776269824000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    437776019615979419056298000384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    708780222249049694845899112448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -2960199751721057029034528997376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   4919768601411533919383711645696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -4085909516394160046033267064832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   1709411124193171979684800364544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -291850679738537955896661639168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -265792583341143102390922117120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   1110074906884136612763336179712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -1844913225507647354068776517632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   1532216068627441080572808527872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -641029171562999806825343221760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    109444004900212816239501770752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     42526813334264694199031955456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -177611985099889659856468049920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    295186116078225686597675778048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -245154570977593578566073712640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    102564667448788604120152932352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -17511040783796469972969979904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[20] = ( li_Real_s(    398688875024012049028267114496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(  -1665112360388626850673940496384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   2767369838382639816127614550016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(  -2298324103055539399858052923392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    961543757397614530924524863488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(   -164166007360107182422830874624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(  -1364401927854865817514264231936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   5698384522191378771210988945408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -9470554557965403333816190238720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   7865375819292384683673687949312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(  -3290616414180066244644889952256.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    561812558516570744822016507904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   1895987094541790026224858300416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(  -7918534335980668888823715856384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  13160381009018541789622537551872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s( -10929807956581083811471276638208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   4572674757321096340576948715520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -780700568319674048754468519936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(  -1382121433395835708008977924096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   5772389515905102676252122152960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -9593548772844501456477770219520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   7967523557053284716719026208768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(  -3333351692215150942572704169984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    569108825497101417775745728512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    540444919465683603479569891328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(  -2257152310690474245866060251136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   3751323558581523834536787443712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(  -3115506006254830690180934926336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   1303425982198999006951057129472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(   -222536143300901262629814665216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -88597527780784346723499638784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    370024968963300008005906464768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -614971075172823519834836828160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    510738689545805119159594385408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(   -213676390522508348280516116480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     36481334967010361995677401088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[21] = ( li_Real_s(   -150615797230611547284182138880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    629042447253745081105644519424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(  -1045450827824230489742016249856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    868255772255856259123734642688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -363249863900976288120386355200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     62018269446214478030694252544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    629042447255282427059693944832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -2627177279697389153357003751424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   4366294633836961597042341183488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -3626244695876855222015356829696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   1517102372756187288977767661568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -259017478274184967382605234176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(  -1045450827828937032827595128832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   4366294633845102416318766776320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -7256658687188462468854517334016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   6026716536779112716143129788416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -2521381408229079314917039800320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    430479752622263895243488231424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    868255772261194432057052233728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -3626244695888378807561891086336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   6026716536786165353159591985152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -5005239157635374578774522724352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   2094028627160330407538369495040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -357517082683938636005948522496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(   -363249863903656352111136866304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   1517102372762242660151744004096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(  -2521381408233505227450838155264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   2094028627161365109552757866496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -876073201153172999463237582848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    149573473366726703767594467328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     62018269446725856490726359040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(   -259017478275326524335027388416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    430479752623084535538088148992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -357517082684119202203508408320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    149573473366720581686851010560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -25536934477084275208850767872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[22] = ( li_Real_s(     26579258334653044548423385088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -111007490690910745421598949376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    184491322555231827729768775680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -153221606866648690231401775104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     64102917157986851421868261376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -10944400490312085736920186880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -124036538894746427505577558016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    518034956556446225903225143296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -860959505256186765497308545024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    715034165376741200503492313088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -299146946737027611973832933376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     51073868954773519307489935360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    230353572232583810641502928896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -962064919317117183679277301760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1598924795472870981879958863872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1327920592840525081942922625024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    555558615368114337551268970496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -94851470915927269070809858048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -212634066675786195178256072704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    888059925521732602819778707456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1475930580433804718979202154496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1225772854927706530042378977280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -512823337262133903920644227072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     87555203922285386148782014464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     97457280559543354464382484480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -407027465863339588635190099968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    676468182697560068073432547328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -561812558507523787226815135744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    235044029578075855535707521024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -40129468464315919803703361536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -17719505556247305495498457088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     74004993793190421843387875328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(   -122994215035675580146928058368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    102147737910253312108105039872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -42735278105017956336041721856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      7296266993497449975091560448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[23] = ( li_Real_s(     -1771950555631955935236194304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(      7400499379328277202402803712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -12299421503542624609412055040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     10214773790967935384426643456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     -4273527810462150432890290176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(       729626699340505883764326400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(      8859752778146798567025541120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -37002496896622465615923249152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(     61497107517752256864916078592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    -51073868954937821529051758592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     21367639052379855920501358592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     -3648133496718609913816809472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -17719505556280015966424793088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(     74004993793251325991473643520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -122994215035632338553630883840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    102147737910086115971938910208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -42735278104891433334010281984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(      7296266993466356885769682944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     17719505556277133046936764416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    -74004993793295684688584638464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    122994215035805480848680026112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   -102147737910315465301399961600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     42735278105022697430180691968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     -7296266993494209714324504576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     -8859752778141716624281960448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     37002496896685612767729680384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -61497107518008205579676286976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     51073868955281405718592225280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -21367639052577842780291727360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      3648133496760740450614116352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      1771950555629789622451568640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     -7400499379347275663819145216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     12299421503625901620099809280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    -10214773791082612248180424704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      4273527810529130482231148544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -729626699354867979524243456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[24] = ( li_Real_s(    -37523658825855980994446753792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    175110407854018303968996753408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   -325205043157500493266962874368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    300189270606949143762817777664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -137586749028189450125430489088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     25015772550578591004234874880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(               -968050007265307392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(               4409326831579022336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(              -8008926852715527168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(               7271542144071504896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(              -3311759442364600832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(                607867326694908032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(               1754326641737957632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(              -7983092636107497472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(              14490707949792546816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(             -13154444766452748288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(               5992881577648316416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(              -1100378766618573568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(              -1584633452842397696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(               7202205707463848960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(             -13064478481852508160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(              11859093192914380800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(              -5405074073276363776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(                992887107593034496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(                715062841324362624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(              -3246218352718725120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(               5885405904002114560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(              -5342911527469947904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(               2436498330867487232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(               -447837196005290880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(               -128515668333749424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(                583177236700044672.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(              -1057243644627052928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(                960151402452189568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(               -438225980568408000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(                 80656654376973968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[25] = ( li_Real_s(     85679020985694456820415856640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -399835431265980876402693505024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    742551515207424301188515889152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -685432167883213782757094195200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    314156410279612111091195183104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -57119347323535866892711362560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -187618294127878599392865812480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    875552039260916983716588290048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(  -1626025215766894487021176225792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   1500946353013369136892170207232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -687933745130372675556784209920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    125078862750859518216765440000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    187618294125474204957431300096.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -875552039247939861390320205824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1626025215740619360895189909504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1500946352987786720683868815360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    687933745118213800987814002688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -125078862748580397738153213952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -125078862748787334622593679360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    583701359490712248515047718912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1084016810478155435461782274048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1000630901976968444296197832704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -458622496738494068177885986816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     83385908497756954691574431744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     46904573530264272129163788288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -218888009806344523473647304704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    406506303924143890978597502976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -375236588236496217973142323200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    171983436274674433369395167232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -31269715686241793457715675136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     -7504731764767278067993280512.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     35022081568639697005815267328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -65041008627145283180274122752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     60037854117168314550759456768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -27517349803638061332896415744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      5003154509741712723589201920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[26] = ( li_Real_s(    -70356860298217803409130520576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    328332014723943837918333763584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(   -609759455914370546935240065024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    562854882381426501086395498496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(   -257975154424441497064705097728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     46904573531658971842672066560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    240776810797098293264048455680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(  -1123625117048561111080707817472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   2086732360226789378302949720064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(  -1926214486358950684213116928000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    882848306246374524293916655616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(   -160517873862750365382717997056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(   -334585957858513395738084376576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   1561401136665286016612823990272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(  -2899744968082351458820766564352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   2676687662838444769908952662016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(  -1226815178798610972450037432320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    223057305235745181224600076288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    243903782362081838317453705216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(  -1138217651016394885665312997376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   2113832780450426406342970310656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(  -1951230258871789488571987001344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    894313868647684030171346632704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(   -162602521572006774694563807232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    -95372632845924364488498216960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    445072286611449165292712755200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   -826562817989072885830498910208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    762981062756865359241705160704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(   -349699653762791098238214602752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     63581755229473718469677547520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     15634857843477237452303761408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    -72962669935724755908175069184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    135502101308582555009050214400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(   -125078862746000345325065207808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     57327812091787546562484240384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(    -10423238562121870553714262016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[27] = ( li_Real_s(     26579258334718100452415635456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(   -124036538894697714742420570112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    230353572232076839024074948608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(   -212634066675096053719733633024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     97457280559176610161914544128.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(    -17719505556176384796483649536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(   -111007490691540967894454108160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    518034956557768313868835094528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -962064919317752331964224897024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    888059925521439446631534559232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(   -407027465863046221340713418752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     74004993793132094950557614080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    184491322556809706080464535552.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -860959505260414660384990953472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   1598924795477328982561102233600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(  -1475930580436375992891453997056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    676468182698501601870529691648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(   -122994215035849566866907332608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(   -153221606868333880916968472576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    715034165381535282306828206080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(  -1327920592846178788325132861440.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   1225772854931490961104253747200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(   -561812558509051281556679688192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    102147737910537003700257292288.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     64102917158817642407815806976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(   -299146946739414132748246319104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    555558615370988548907205656576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(   -512823337264106973138641747968.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    235044029578883160953285771264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(    -42735278105168123236116856832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(    -10944400490470327450389708800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     51073868955223545018692141056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -94851470916464710354466766848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     87555203922650934182598934528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(    -40129468464464626552336809984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      7296266993524971850646421504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[28] = ( li_Real_s(     -4690457353147286850455470080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     21888800981195543686330449920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -40650630393418417376661078016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     37523658824526847358326013952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    -17198343627846289743992061952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      3126971568689414909963796480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     21888800981270530379344773120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(   -102147737911924833665115226112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    189702941835492778137465389056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   -175110407847488014140565356544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     80258936929857012220097986560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    -14592533987207382771274088448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -40650630393650643028540391424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    189702941835891311520115589120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -352305463407966660555880529920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    325205043144760021027364274176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(   -149052311440963801240192417792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     27100420261929750286900920320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     37523658824792594920712962048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(   -175110407848052371468870221824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    325205043145098283580626305024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   -300189270594618287983077359616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    137586749022222321356652937216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    -25015772549442465639253934080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(    -17198343627980834782859755520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     80258936930154795153271816192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(   -149052311441175452830492786688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    137586749022270260063623970816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -63060593301745900702509563904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     11465562418477082521431441408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      3126971568715377128274657280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(    -14592533987264528788616118272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     27100420261970075975361232896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(    -25015772549451481634601697280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     11465562418477141895059341312.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(     -2084647712446590073035554816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[29] = ( li_Real_s(       312697156873724022979821568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     -1459253398730618983394836480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(      2710042026194295542155051008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     -2501577254934140402798690304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      1146556241839388467558088704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      -208464771242660191371526144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     -1563485784364566834002788352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(      7296266993644278757864767488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    -13550210130973565683356401664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     12507886274688005578235576320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     -5732781209210817075021348864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      1042323856216662782379032576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(      3126971568724696588831686656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    -14592533987285085258009018368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(     27100420261969411870338056192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    -25015772549419455059907837952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     11465562418450201661155573760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     -2084647712439777773867761664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     -3126971568723280417855111168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     14592533987292856606194139136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    -27100420262006293888380174336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(     25015772549470674709576155136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -11465562418480121571570614272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(      2084647712446194248849555456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      1563485784362210855462371328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     -7296266993654739511491428352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     13550210131027301015629070336.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -12507886274764142360412553216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(      5732781209255683746504376320.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -1042323856226317456543580160.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      -312697156872765867315691520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      1459253398733330653946839040.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     -2710042026211210429036756992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(      2501577254959113060644552704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -1146556241854383882015604736.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(       208464771245913646278115328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[30] = ( li_Real_s(      2501577255055671622040027136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -12507886275285053586257477632.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(     25015772550582782342559956992.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    -25015772550594388787302760448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     12507886275302311520767049728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -2501577255061315415225401344.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(                 71445203916191176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(               -326279456486275136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(                594052128603605120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(               -540597577289850560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(                246808620534074848.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(                -45428919277745568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(               -130253677999630000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(                594361027046366208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(              -1081543243933652352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(                984163704413224960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(               -449488275077819456.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(                 82760465551510496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(                118194480376958784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(               -538716413554154496.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(                979692546157993088.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(               -891508675740680192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(                407388595952675264.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(                -75050533192791424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(                -53494150615275520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(                243545807492899392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(               -442702353350726912.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(                402929081652386624.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(               -184241395454544064.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(                 33963010275260468.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(                  9631034454616364.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(                -43819982660120688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(                 79650250088057712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(                -72532578882590032.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(                 33195550442918112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(                 -6124273442881405.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[31] = ( li_Real_s(     -5711934732375862794849878016.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     28559673661841838219928272896.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -57119347323645387046930874368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     57119347323634435911118225408.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(    -28559673661821119022814461952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      5711934732366098032083599360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     12507886275168791226736443392.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -62539431375694380770862825472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    125078862751257488649462218752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(   -125078862751244417655231217664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     62539431375653909946867646464.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(    -12507886275141362809669943296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -12507886274979439931270627328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(     62539431374662625447636893696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -125078862749155890920224194560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    125078862749189791062731784192.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -62539431374674359435728519168.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     12507886274957260582715129856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(      8338590849863597685458599936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    -41692954249135991765105377280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(     83385908498173590433664335872.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    -83385908498249623861748301824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     41692954249214074682863517696.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     -8338590849865629582946729984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     -3126971568655695637118976000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     15634857843208206748196274176.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -31269715686393169820581363712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     31269715686445163526435635200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -15634857843269814583723819008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      3126971568665315264350388224.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(       500315450978685042468847616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     -2501577254882618937188548608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(      5003154509764041605726076928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     -5003154509776015287352557568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      2501577254897574494349557760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -500315450981726222911799296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[32] = ( li_Real_s(      4690457353207276754622742528.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(    -23452286765963635136017727488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(     46904573531845932799859097600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(    -46904573531812525238560751616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     23452286765906456133326864384.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(     -4690457353183497616648830976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(    -16051787386428236331220992000.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(     80258936931851491128511561728.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(   -160517873863417285155661807616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    160517873863339879537066377216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(    -80258936931700075183227273216.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     16051787386354298572299567104.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(     22305730523799600467195985920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(   -111528652618540794614825091072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(    223057305236695651852207783936.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(   -223057305236669826523094581248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    111528652618434836878279573504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(    -22305730523719459263670648832.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(    -16260252157364327460720607232.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     81301260786462314704622583808.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(   -162602521572679148045181386752.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(    162602521572744766899127058432.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -81301260786498325909455503360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(     16260252157334550486817177600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      6358175523006282580472365056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(    -31790877614890702901797191680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     63581755229710245411044720640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -63581755229774201803409195008.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(     31790877614957931440765927424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -6358175523009556926099881984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(     -1042323856220662118486114304.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      5211619281081356506319814656.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(    -10423238562155531002686996480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     10423238562172127031196647424.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -5211619281100977291317477376.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      1042323856223778271878184960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[33] = ( li_Real_s(     -1771950555642354291577978880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(      8859752778169012000441499648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(    -17719505556289207883633000448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     17719505556268726181030789120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(     -8859752778134143188189839360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      1771950555628040024573870080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(      7400499379403814750742642688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(    -37002496896848745108919549952.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(     74004993793530338062138081280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(    -74004993793486392781399130112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     37002496896761905680557801472.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(     -7400499379360921702631473152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(    -12299421503716052777484419072.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(     61497107518313878608291037184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(   -122994215036409702070561538048.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    122994215036404864219399323648.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(    -61497107518266423686436225024.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     12299421503673444502884843520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     10214773791147151381707620352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(    -51073868955529569891027779584.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    102147737910927831705420038144.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(   -102147737910980995291646263296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(     51073868955571711972697178112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(    -10214773791136202444918226944.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(     -4273527810549710591124045824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     21367639052668416150141403136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(    -42735278105302624294519439360.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(     42735278105348645453211631616.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(    -21367639052720080002507341824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(      4273527810555351085774536704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(       729626699357153039564668928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(     -3648133496772975816008007680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(      7296266993543343590434930688.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(     -7296266993554878566921928704.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(      3648133496787081450680745984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(      -729626699359719299703898112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[34] = ( li_Real_s(       312697156874973824103219200.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(     -1563485784364555838886510592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(      3126971568717537668623237120.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(     -3126971568712778982298222592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      1563485784356426324788641792.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(      -312697156871603546086178816.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(     -1459253398742451377777147904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(      7296266993672112894721916928.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(    -14592533987305850634611195904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     14592533987297065536705265664.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(     -7296266993653873096328740864.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      1459253398732992554121297920.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(      2710042026223837220570136576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(    -13550210131057330877206888448.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(     27100420262066947347814809600.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(    -27100420262070377824093470720.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     13550210131052693137160929280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(     -2710042026215772852536213504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(     -2501577254965779949399572480.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     12507886274781721352317435904.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(    -25015772549537107202126381056.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(     25015772549554967669007974400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(    -12507886274799337727617662976.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(      2501577254965546303178670080.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      1146556241855102550303309824.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(     -5732781209257492443132067840.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     11465562418509447745706655744.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(    -11465562418523125670356189184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(      5732781209273759717665013760.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(     -1146556241857689563724513280.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(      -208464771245676323565207552.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      1042323856225541476212277248.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(     -2084647712450987294912937984.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(      2084647712454279507604406272.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(     -1042323856229693919313526784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(       208464771246528273278042112.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];
        liVC[35] = ( li_Real_s(       -20846477124776349728243712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 0] +
                   ( li_Real_s(       104232385623014444420300800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 1] +
                   ( li_Real_s(      -208464771245073172717895680.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 2] +
                   ( li_Real_s(       208464771244689133922156544.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 3] +
                   ( li_Real_s(      -104232385622355166940364800.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 4] +
                   ( li_Real_s(        20846477124499573445754880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 5] +
                   ( li_Real_s(       104232385623480242213486592.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 6] +
                   ( li_Real_s(      -521161928114097779921387520.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 7] +
                   ( li_Real_s(      1042323856225112529238491136.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 8] +
                   ( li_Real_s(     -1042323856224506698331586560.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[ 9] +
                   ( li_Real_s(       521161928112762010732593152.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[10] +
                   ( li_Real_s(      -104232385622750166492643328.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[11] +
                   ( li_Real_s(      -208464771246525421419757568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[12] +
                   ( li_Real_s(      1042323856227607321121914880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[13] +
                   ( li_Real_s(     -2084647712451554642912870400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[14] +
                   ( li_Real_s(      2084647712452168720156983296.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[15] +
                   ( li_Real_s(     -1042323856227701054488182784.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[16] +
                   ( li_Real_s(       208464771246005043182174208.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[17] +
                   ( li_Real_s(       208464771246367366623264768.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[18] +
                   ( li_Real_s(     -1042323856228067604177092608.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[19] +
                   ( li_Real_s(      2084647712454300673203240960.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[20] +
                   ( li_Real_s(     -2084647712456153075418136576.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[21] +
                   ( li_Real_s(      1042323856230068852778598400.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[22] +
                   ( li_Real_s(      -208464771246516831485165568.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[23] +
                   ( li_Real_s(      -104232385623216359422820352.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[24] +
                   ( li_Real_s(       521161928114667807980912640.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[25] +
                   ( li_Real_s(     -1042323856229062524761276416.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[26] +
                   ( li_Real_s(      1042323856230390459929722880.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[27] +
                   ( li_Real_s(      -521161928116298452444381184.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[28] +
                   ( li_Real_s(       104232385623519171797057536.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[29] +
                   ( li_Real_s(        20846477124667510961995776.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[30] +
                   ( li_Real_s(      -104232385623124704820723712.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[31] +
                   ( li_Real_s(       208464771246279749290426368.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[32] +
                   ( li_Real_s(      -208464771246592079312191488.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[33] +
                   ( li_Real_s(       104232385623526078104469504.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[34] +
                   ( li_Real_s(       -20846477124757082504953856.0 ) / li_Real_s(   -300189270608550490089696788480.0 ) ) * liVS[35];

        /* Prepare interpolated value computation */
        liTX1 = ( liX + li_Real_s( 2.0 ) ) - liPXmm;
        liTX2 = liTX1 * liTX1; 
        liTX3 = liTX1 * liTX2; 
        liTX4 = liTX1 * liTX3; 
        liTX5 = liTX1 * liTX4;
        liTY1 = ( liY + li_Real_s( 2.0 ) ) - liPYmm; 
        liTY2 = liTY1 * liTY1; 
        liTY3 = liTY1 * liTY2; 
        liTY4 = liTY1 * liTY3; 
        liTY5 = liTY1 * liTY4;

        /* Compute interpolated value */
        liIV = liVC[ 0]                 + 
               liVC[ 1] * liTY1         + 
               liVC[ 2] * liTY2         + 
               liVC[ 3] * liTY3         + 
               liVC[ 4] * liTY4         + 
               liVC[ 5] * liTY5         +
               liVC[ 6] * liTX1         + 
               liVC[ 7] * liTY1 * liTX1 + 
               liVC[ 8] * liTY2 * liTX1 + 
               liVC[ 9] * liTY3 * liTX1 + 
               liVC[10] * liTY4 * liTX1 + 
               liVC[11] * liTY5 * liTX1 +
               liVC[12] * liTX2         + 
               liVC[13] * liTY1 * liTX2 + 
               liVC[14] * liTY2 * liTX2 + 
               liVC[15] * liTY3 * liTX2 + 
               liVC[16] * liTY4 * liTX2 + 
               liVC[17] * liTY5 * liTX2 +
               liVC[18] * liTX3         + 
               liVC[19] * liTY1 * liTX3 + 
               liVC[20] * liTY2 * liTX3 + 
               liVC[21] * liTY3 * liTX3 + 
               liVC[22] * liTY4 * liTX3 + 
               liVC[23] * liTY5 * liTX3 +
               liVC[24] * liTX4         + 
               liVC[25] * liTY1 * liTX4 + 
               liVC[26] * liTY2 * liTX4 + 
               liVC[27] * liTY3 * liTX4 + 
               liVC[28] * liTY4 * liTX4 + 
               liVC[29] * liTY5 * liTX4 +
               liVC[30] * liTX5         + 
               liVC[31] * liTY1 * liTX5 + 
               liVC[32] * liTY2 * liTX5 + 
               liVC[33] * liTY3 * liTX5 + 
               liVC[34] * liTY4 * liTX5 + 
               liVC[35] * liTY5 * liTX5;

        /* Return interpolated value */
        return( liIV );

    }
