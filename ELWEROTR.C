/*****************************************************************************
 *                                                                           *
 *    Erotica Version 1.41                                                   *
 *                                                                           *
 *    ELWEROTR.C                                                             *
 *    Erotica's Array of Rooms                                               *
 *                                                                           *
 *    By - Mike Polzin                               6/02/93                 *
 *    Designed by Theresa Thorsen                                            *
 *    v1.41 - Major BBS v10 conversion - R. Hadsall  4/18/2023               *
 *          - v10/WG32 single project  - R. Hadsall  8/10/2024               *
 *                                                                           *
 * Copyright (C) 2005-2024 Rick Hadsall.  All Rights Reserved.               *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as published  *
 * by the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.     *
 *                                                                           *
 * Additional Terms for Contributors:                                        *
 * 1. By contributing to this project, you agree to assign all right, title, *
 *    and interest, including all copyrights, in and to your contributions   *
 *    to Rick Hadsall and Elwynor Technologies.                              *
 * 2. You grant Rick Hadsall and Elwynor Technologies a non-exclusive,       *
 *    royalty-free, worldwide license to use, reproduce, prepare derivative  *
 *    works of, publicly display, publicly perform, sublicense, and          *
 *    distribute your contributions                                          *
 * 3. You represent that you have the legal right to make your contributions *
 *    and that the contributions do not infringe any third-party rights.     *
 * 4. Rick Hadsall and Elwynor Technologies are not obligated to incorporate *
 *    any contributions into the project.                                    *
 * 5. This project is licensed under the AGPL v3, and any derivative works   *
 *    must also be licensed under the AGPL v3.                               *
 * 6. If you create an entirely new project (a fork) based on this work, it  *
 *    must also be licensed under the AGPL v3, you assign all right, title,  *
 *    and interest, including all copyrights, in and to your contributions   *
 *    to Rick Hadsall and Elwynor Technologies, and you must include these   *
 *    additional terms in your project's LICENSE file(s).                    *
 *                                                                           *
 * By contributing to this project, you agree to these terms.                *
 *                                                                           *
 *****************************************************************************/

#include "stdio.h"
#include "elweroti.h"
#include "elwerot.h"
#include "elwerotp.h"

struct ertroom ertroom_list[78]={

     {"on Erotica Street",                   /*  Room 000   */
     ERT1,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,1,255,255,255,62,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",0,0},

     {"on the corner of Bourbon and Erotica",/*  Room 001   */
     ERT2,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     0,25,2,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",1,0},

     {"on Bourbon Street",                   /*  Room 002   */
     ERT3,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     3,255,4,1,255,255,
     3,
     {1,0,0,0,0,0},
     {30,0,0,0,0,0},
     1,
     "xxxx",2,0},

     {"in the Wet 'N Wild bar",              /*  Room 003   */
     ERT4,
     ertpub,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,2,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",3,0},

     {"on Bourbon Street",                   /*  Room 004   */
     ERT5,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,5,7,2,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",4,0},

     {"in Madame's Romp & Play",             /*  Room 005   */
     ERT6,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     4,255,255,6,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",5,0},

     {"in Madame's private room",            /*  Room 006   */
     ERT7,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,5,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",6,0},

     {"at the intersection of Bourbon Street and Center Street.", /*  Room 007   */
     ERT8,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,18,8,4,255,72,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",7,0},

     {"on Bourbon Street",                   /*  Room 008   */
     ERT9,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     9,10,255,7,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",8,0},

     {"in Porn-O-Rama",                      /*  Room 009   */
     ERT10,
     ertwpn_shop,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,8,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",9,0},

     {"on Wisconsin Avenue",                 /*  Room 010   */
     ERT11,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     8,13,11,12,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",10,0},

     {"in Hoops bar",                        /*  Room 011   */
     ERT12,
     ertpub,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,10,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",11,0},

     {"in the Meltdown Motel",               /*  Room 012   */
     ERT13,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,10,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",12,0},

     {"at the intersection of Wisconsin Avenue and Sunset Strip", /*  Room 013   */
     ERT14,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     10,255,255,14,255,67,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",13,0},

     {"on Sunset Strip",                     /*  Room 014   */
     ERT15,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     16,15,13,31,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",14,0},

     {"in the Wax 'em Down massage parlor",  /*  Room 015   */
     ERT16,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     14,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",15,0},

     {"in Adam and Eve's Sex Store",         /*  Room 016   */
     ERT17,
     ertarmr_shop,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,14,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",16,0},

     {"in Dr. Techno's Night Club",          /*  Room 017   */
     ERT18,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,19,18,20,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",17,0},

     {"on Center Street",                    /*  Room 018   */
     ERT21,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     7,23,21,17,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",18,0},

     {"in Dr. Techno's Lounge",              /*  Room 019   */
     ERT19,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     17,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",19,0},

     {"in Dr. Techno's Bar",                 /*  Room 020   */
     ERT20,
     ertpub,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,17,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",20,0},

     {"in an alley",                         /*  Room 021   */
     ERT22,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,22,255,18,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",21,0},

     {"in a garage",                         /*  Room 022   */
     ERT23,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     21,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",22,0},

     {"on Center Street",                    /*  Room 023   */
     ERT24,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     18,255,255,24,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",23,0},

     {"in the library",                      /*  Room 024   */
     ERT27,
     ert_lbrry,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,23,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",24,0},

     {"on Erotica Street",                   /*  Room 025   */
     ERT28,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     1,28,26,52,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",25,0},

     {"in the casino",                       /*  Room 026   */
     ERT25,
     ert_casino,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,27,255,25,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",26,0},

     {"in the casino bar",                   /*  Room 027   */
     ERT26,
     ertpub,
     NULL,
     0,
     {255,255,255,255,255,255},
     26,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",27,0},

     {"on Erotica Street",                   /*  Room 028   */
     ERT30,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     25,30,29,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",28,0},

     {"in the city bank",                    /*  Room 029   */
     ERT31,
     ertbank,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,28,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",29,0},

     {"on Erotica Street",                   /*  Room 030   */
     ERT32,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     28,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",30,0},

     {"on Sunset Strip",                     /*  Room 031   */
     ERT33,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     33,32,14,35,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",31,0},

     {"in a cafe",                           /*  Room 032   */
     ERT34,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     31,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",32,0},

     {"in Renee's Lingere",                  /*  Room 033   */
     ERT35,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,31,255,255,34,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",33,0},

     {"in a play room",                      /*  Room 034   */
     ERT36,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,255,255,33,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",34,0},

     {"in a hallway",                        /*  Room 035   */
     ERT37,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     38,41,31,36,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",35,0},

     {"at the receptionists desk",           /*  Room 036   */
     ERT38,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     37,40,35,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",36,0},

     {"in the presidents office",            /*  Room 037   */
     ERT39,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,36,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",37,0},

     {"in a hallway",                        /*  Room 038   */
     ERT40,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,35,39,255,48,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",38,0},

     {"in the Men's room",                   /*  Room 039   */
     ERT41,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,38,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",39,0},

     {"in a lunch room",                     /*  Room 040   */
     ERT42,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     36,255,41,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",40,0},

     {"in a hallway",                        /*  Room 041   */
     ERT43,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     35,255,42,40,46,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",41,0},

     {"in the womens room",                  /*  Room 042   */
     ERT44,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,41,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",42,0},

     {"in a computer room",                  /*  Room 043   */
     ERT45,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     44,255,46,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",43,0},

     {"in a secretary's office",             /*  Room 044   */
     ERT46,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     45,43,47,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",44,0},

     {"in a sales office",                   /*  Room 045   */
     ERT47,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,44,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",45,0},

     {"in a hallway",                        /*  Room 046   */
     ERT48,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     47,255,49,43,255,41,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",46,0},

     {"in a hallway",                        /*  Room 047   */
     ERT49,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     48,46,50,44,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",47,0},

     {"in a hallway",                        /*  Room 048   */
     ERT50,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,47,51,45,255,38,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",48,0},

     {"in a stock room",                     /*  Room 049   */
     ERT51,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,46,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",49,0},

     {"in an office",                        /*  Room 050   */
     ERT52,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     51,255,255,47,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",50,0},

     {"in an abandon room",                 /*  Room 051   */
     ERT53,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,50,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",51,0},

     {"on Eden Street",                     /*  Room 052   */
     ERT54,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     53,77,25,55,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",52,0},

     {"in Applegate's Massage Parlor",      /*  Room 053   */
     ERT55,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     54,52,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",53,0},

     {"in a massage room",                  /*  Room 054   */
     ERT56,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,53,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",54,0},

     {"on Eden Street",                     /*  Room 055   */
     ERT57,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,56,52,57,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",55,0},

     {"in Eden's Place",                    /*  Room 056   */
     ERT58,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     55,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",56,0},

     {"on Eden Street",                     /*  Room 057   */
     ERT59,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,55,255,255,58,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",57,0},

     {"at the entrace to the subway",       /*  Room 058   */
     ERT60,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,59,255,57,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",58,0},

     {"in the old subway",                  /*  Room 059   */
     ERT61,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     75,74,60,58,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",59,0},

     {"in the subway",                      /*  Room 060   */
     ERT62,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     61,63,68,59,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",60,0},

     {"in the subway",                      /*  Room 061   */
     ERT63,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     62,60,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",61,0},

     {"at the entrace to an old subway",    /*  Room 062   */
     ERT64,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,61,255,255,0,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",62,0},

     {"in the heart of the subway",         /*  Room 063   */
     ERT65,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     60,76,64,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",63,0},

     {"in the subway",                      /*  Room 064   */
     ERT66,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,65,63,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",64,0},

     {"in a littered subway",               /*  Room 065   */
     ERT67,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,66,255,64,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",65,0},

     {"in the subway",                      /*  Room 066   */
     ERT68,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     65,255,67,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",66,0},

     {"at an entrace to the old subway",    /*  Room 067   */
     ERT69,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,255,255,66,13,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",67,0},

     {"in the subway",                      /*  Room 068   */
     ERT70,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     69,255,255,60,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",68,0},

     {"in a stinky subway",                 /*  Room 069   */
     ERT71,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,68,70,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",69,0},

     {"in the subway near an old cafeteria",/*  Room 070   */
     ERT72,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     71,73,255,69,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",70,0},

     {"in the subway tunnels",              /*  Room 071   */
     ERT73,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     72,70,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",71,0},

     {"at the entrance to the old subway",  /*  Room 072   */
     ERT74,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,71,255,255,7,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",72,0},

     {"in an abandon cafeteria",            /*  Room 073   */
     ERT75,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     70,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",73,0},

     {"in a womens bathroom",               /*  Room 074   */
     ERT76,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     59,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",74,0},

     {"in the mens bathroom",               /*  Room 075   */
     ERT77,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     255,59,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",75,0},

     {"near an old newspaper stand",        /*  Room 076   */
     ERT78,
     ert_null,
     NULL,
     0,
     {255,255,255,255,255,255},
     63,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",76,0},

     {"near an old newspaper stand",        /*  Room 077   */
     ERT79,
     ert_medic,
     NULL,
     0,
     {255,255,255,255,255,255},
     52,255,255,255,255,255,
     3,
     {0,0,0,0,0,0},
     {0,0,0,0,0,0},
     0,
     "xxxx",77,0}

};
