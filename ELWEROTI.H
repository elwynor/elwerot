/*****************************************************************************
 *                                                                           *
 *    Erotica Version 1.41                                                   *
 *                                                                           *
 *    ELWEROTI.H                                                             *
 *    Erotica Header Definitions File                                        *
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

#include "gcomm.h"

#define MXITEM  6                     /*  Maximum number of items           */
#define MAXINV  6                     /*  Maximum number of items carryable */
#define PIDSIZ  15                    /*  size of player ID storage area    */
#define SEXID   4                     /*  size of his/her identifier        */
#define OFFNSE  1                     /*  Item definitions                  */
#define DEFNSE  2                     /*  Item definitions                  */
#define GENERIC 3                     /*  Item definitions                  */
#define MGC     4                     /*  Item definitions                  */

#define NROOMS  77                    /*  number of rooms in city           */
#define NOROOM  (&ertroom_list[255])  /*  pointer to no room at all         */
#define NMITEM  14                    /* Number of items in Item structure  */

#ifndef CHAR_BUFFER
#define CHAR_BUFFER(name, size, lit) CHAR name[size] = lit
#endif

struct monster {                      /*** Monster setup/structure          */
      CHAR  *mname;                   /*  Robot Players Name                */
      SHORT mdesc;                    /*  Robot Players Description         */
      SHORT mpref;                    /*  Robot Players sexual preference   */
      SHORT inlimbo;                  /*  Robot Player in limbo flag        */
      SHORT sayflg;                   /*  Robot Player says something flag  */
      SHORT comment;                  /*  Robot Players action/comments     */
      SHORT tt1;                      /*  Treasure type one                 */
      SHORT tt2;                      /*  Treasure type two                 */
      SHORT mhpts;                    /*  Robot Players Defense Points      */
      SHORT msex;                     /*  Robot Players Sex, 0 = M, 1 = F   */
      SHORT mnstnum;                  /*  Robot Players Reference Number    */
};

struct ertroom {                      /***  Rooms setup/structure           */
     CHAR *brdesc;                    /*  Brief room description            */
     SHORT desc;                      /*  Complete room description         */
     SHORT (*rmcmd)(VOID);            /*  Commands routines for room        */
     struct ertplayer *roomhdr;       /*  linked list hdr of players in room*/
     SHORT nitems;                    /*  number of items in rooms right now*/
     CHAR iitems[MXITEM];             /*  Items in room                     */
     USHORT inorth;                   /*  Room to north                     */
     USHORT isouth;                   /*  Room to south                     */
     USHORT ieast;                    /*  Room to east                      */
     USHORT iwest;                    /*  Room to west                      */
     USHORT iup;                      /*  Room up                           */
     USHORT idown;                    /*  Room down                         */
     SHORT room_type;                 /*  Type of room, forest, swamp, etc  */
     CHAR ertmlist[MXITEM];           /*  Monsters in room                  */
     SHORT ertmhps[MXITEM];           /*  Monster hit points                */
     SHORT ertmnum;                   /*  Number of monsters in room        */
     CHAR key[5];                     /*  Key for door if locked            */
     SHORT room_num;                  /*  Room's easy reference number      */
     SHORT num_nlv;                   /*  Number of non-live players in room*/
};

struct ert_cmd {
     CHAR *command;
     SHORT (*routine)(VOID);
     SHORT liveuse;
};

struct ertplayer {                    /***  Player structure                */
     CHAR plyrid[30];                 /*  Player's Name                     */
     CHAR charac[PIDSIZ];             /*  Characters name                   */
     struct ertplayer *plink;         /*  forward-link in room linked list  */
     SHORT channel;                   /*  channel this player is on         */
     SHORT flags;                     /*  Player's game-control flags       */
     SHORT descrpt;                   /*  Player's description              */
     CHAR hisher[4];                  /*  "his" or "her" (or "its")         */
     struct ertroom *ertroom;         /*  Player's current room             */
     SHORT hits;                      /*  Player's hit points               */
     SHORT maxhit;                    /*  player's max hit count            */
     SHORT spluse;                    /*  Seconds to go before can rest     */
     SHORT completed;                 /*  Has the player completed rolling? */
     SHORT crstimer;                  /*  Seconds to go before curse gone   */
     SHORT prttimer;                  /*  Seconds to go b4 protection gone  */
     SHORT pwrtimer;                  /*  Seconds to go b4 super-power gone */
     SHORT nitems;                    /*  Number of items held by player    */
     SHORT dollars;                   /*  Player's dollars                  */
     USHORT bank_dollars;             /*  Player's dollars in bank          */
     struct item *items[MAXINV];      /*  Player's items being held         */
     LONG aura;                       /*  Player's "aura"                   */
     ULONG points;                    /*  Player's total points accumulated */
     CHAR plyr_pref[9];               /*  Player's sexual preference        */
     SHORT pref_num;                  /*  Player's sexual preference number */
     SHORT keyroom;                   /*  Room to go to for key             */
     SHORT room_num;                  /*  Room number player is in          */
     SHORT hold_pos;                  /*  Players position in list          */
     CHAR arouse[22];                 /*  Name of person being aroused      */
     SHORT limbo;                     /*  Aroused person nailed?            */
     SHORT osex;                      /*  Other persons sex                 */
     SHORT saved;                     /*  Player is saved                   */
     SHORT has_disease;               /*  Player has a disease              */
     CHAR space[120];
};

                                      /* "flags" bit definitions            */
#define BRIEF 1                       /*   brief room description mode      */
#define INWLD 2                       /*   player is playing WilderLands    */
#define ONSYS 4                       /*   player has been in game this ses.*/
#define SYSOP 8                       /*   player status set to SysOp       */

                                      /* "aura" bit definitions             */
#define DISEAS 0x00000001L            /* Player has Herpes                  */
#define BISEXU 0x00000002L            /* Player has Ghonnorea               */
#define STRAIG 0x00000004L            /* Player has Crabs                   */
#define SEEOBJ 0x00000008L            /* Player can see objects normally    */
#define SEEPYR 0x00000010L            /* Player can see players normally    */
#define PROTEC 0x00000020L            /* Player is wearing protection       */
#define HNDCUF 0x00000040L            /* Player has a hand-cuff key         */
#define SEXCLN 0x00000080L            /* Player has sexacillin              */
#define CHSTYB 0x00000100L            /* Player has a chastity belt         */
#define UNSUBM 0x00000200L            /* Player has an unsubmissive potion  */
#define SCISSR 0x00000400L            /* Player has Scissors                */
#define GLASSE 0x00000800L            /* Player has Anti-glasses            */
#define SEEIN2 0x00001000L            /* can see invisible flavor 2's       */
#define PICOBJ 0x00002000L            /* can pick up objects no sweat       */
#define GAYLES 0x00004000L            /* play "marriage bit" set            */
#define MALE   0x00008000L            /* player has completed first task    */
#define FEMALE 0x00010000L            /* player has completed second task   */
#define TASK03 0x00020000L            /* player has completed third task    */
#define SCRPRO 0x00040000L            /* protection from being scryed       */
#define STLPRO 0x00080000L            /* protection from being robbed       */
#define BLESSD 0x00100000L
#define GOODBY 0x00200000L

struct item {                         /***  Item structure                  */
     CHAR *name;                      /*  Item name                         */
     SHORT descip;                    /*  Item description                  */
     SHORT (*rubcmd)();               /*  Commands routines for rubbing     */
     SHORT (*aimcmd)();               /*  Commands routines for aiming      */
     SHORT (*thkcmd)();               /*  Commands routines for thinking on */
     SHORT flags;                     /*  Item bit 'flags'                  */
     SHORT item_pwr;                  /*  "Power" of item                   */
     SHORT item_def;                  /*  Item definition                   */
     SHORT stock;                     /*  Amount of item in stock           */
     SHORT item_gld;                  /*  Value of item in dollars          */
};

struct inact_ply {                    /*** Inactive player template         */
      SHORT ilink;                    /* Foreward link in room              */
      CHAR iname[15];                 /* Players name                       */
      CHAR iplyrid[30];               /* Players REAL userid - for Email    */
      LONG aura;
      SHORT hits;
      SHORT pref_num;
};
                                     /*   "flags" bit definitions         */
struct hiscore {                     /*** High Score Structure              */
      CHAR hname[30];                /* Players Alias in Game               */
      ULONG hscore;                  /* Players Current Score in Game       */
};

#define PICKUP  1                      /*   item can be picked up           */
#define SURVIV  2                      /*   item can survive player's death */
#define DEFENS  4                      /*   item has a blade                */
#define OFFENS  8                      /*   item can be used by a mage      */
#define USEAN   16                     /*   item name starts with a vowel   */
#define RUBABL  32                     /*   item activated by rubbing       */
#define THKABL  64                     /*   item activated by thinking of it*/
#define AIMABL  128                    /*   item activated by aiming it     */

extern struct ertplayer* ertplayer;    /* main player data array of structs */
extern struct ertplayer *ertply;       /* ptr to current player's player data */
extern struct ertplayer *ertoth;       /* ptr to other player's player data */

unsigned ert_rnd();

extern struct item ertitems[];
extern struct ertroom ertroom_list[];
extern struct monster mnstr_list[];
extern struct inact_ply ert_inact_lst[];
extern struct hiscore high_score[];

struct monster *getmnstr();
struct item *ert_get_ply();
struct item *ert_get_itm();
struct item *ert_loc_itm();
struct ertplayer *ertfplyr();
struct monster *ertfmnstr();
struct ertplayer *plyr_ert();
