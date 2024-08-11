/*****************************************************************************
 *                                                                           *
 *    Erotica Version 1.41                                                   *
 *                                                                           *
 *    ELWEROT.C                                                              *
 *    Erotica Game Service Handler                                           *
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
#include "majorbbs.h"
#include "elwerot.h"
#include "elweroti.h"
#include "elwerotp.h"

typedef INT (*IVOIDFUNC)(VOID);

#define VERSION "1.41" // v1.4 was WG32, v1.41 is BBSv10

struct module ertmod={        /*    module interface block            */
     "",                      /*    name used to refer to this module */
     ert_intro,               /*    user logon supplemental routine   */
     erotica_input,           /*    input routine if selected         */
     dfsthn,                  /*    status-input routine if selected  */
     NULL,                    /*    "injoth" routine for this module  */
     NULL,                    /*    user logoff supplemental routine  */
     erotica_hang_up,         /*    hangup (lost carrier) routine     */
     NULL,                    /*    midnight cleanup routine          */
     erotica_delete,          /*    delete-account routine            */
     erotica_clean_up         /*    finish-up (sys shutdown) routine  */
};

#define FALSE 0
#define TPCSIZ 40
#define ERTSIZ 1921

struct ertuser {              /*    Erotica mail data block           */
     CHAR text[ERTSIZ];       /*    Text Buffer                       */
     CHAR topic[TPCSIZ];      /*    Topic Buffer                      */
};

#define ertptr ((struct ertuser *)vdaptr)

static
INT ertstt;                   /* Erotica state number                 */

SHORT ert_struct_pos = NROOMS;

SHORT ert_evnt1, ert_evnt2, ert_evnt3, ert_evnt4, ert_tag_msg, item_counter;

DFAFILE *ertbb;
HMCVFILE ertmsg;

struct inact_ply inactply;
struct ertplayer *ertplayer,*ertply;
struct ertplayer *ertoth;

static
SHORT nrooms;          /* scratch vbl for use by ert_out_near & ert_outfar */

static
struct ertroom *rooms[17];/* scratch array for use by ert_out_near & ert_outfar*/

static
struct ertplayer *pspp;/* scratch vbl used by ert_init_see & ert_see_nxtp */

VOID EXPORT
init__elwerot(VOID)
{
	
   stzcpy(ertmod.descrp, gmdnam("ELWEROT.MDF"), MNMSIZ);
   ertstt = register_module(&ertmod);

   ertplayer = (struct ertplayer *)alcmem(nterms*sizeof(struct ertplayer));
   dclvda(sizeof(struct ertuser));
   setmem(ertplayer, nterms*sizeof(struct ertplayer), 0);

   ertmsg = opnmsg("elwerot.mcv");
   ert_tag_msg = ynopt(ERTTAG);
   item_counter = (SHORT)numopt(ITEMSPD, 1, 150);

   ertbb = dfaOpen("elwerot.dat", sizeof(struct ertplayer), NULL);

   ert_evnt1 = 0;
   ert_evnt2 = 0;
   ert_evnt3 = 0;
   ert_evnt4 = 0;
   init_hiscore();
   ert_init_hiscore();
   ert_init_inactive();
   rtkick(5, ert_timed);
   
   shocst(spr("ELW Erotica v%s", VERSION), "(C) Copyright 2024 Elwynor Technologies - www.elwynor.com");
}

GBOOL erotica_input(VOID)
{
     SHORT go_to, temp;
     CHAR  temp2[7];

     setmbk(ertmsg);
     dfaSetBlk(ertbb);
     ertply=&ertplayer[usrnum];
     if (!hasmkey(ERTKEY))
     {
	prfmsg(SORRY);
	outprf(usrnum);
	return(0);
     }
     if (sameas(margv[0], "exit") || sameas(margv[0], "x"))
     {
	if (ertply->completed == FALSE)
	{
	   if (usaptr->ansifl&ANSON)
	      prfmsg(EXITANS);
	   else
	      prfmsg(EXIT2);
	   ert_high();
	   btupmt(usrnum,0);
           return(0);
	}
	else
	if (ertply->flags&INWLD)
	{
	   ert_relink();
	   ertleave_room(ertply,getmsg(EXIT1));
	   ert_del_name(ertply->charac);
	   ert_insert(ertply->charac, ertply->points);
	   if (usaptr->ansifl&ANSON)
	      prfmsg(EXITANS);
	   else
	      prfmsg(EXIT2);
	   ert_high();
	   ertply->flags&=~INWLD;
	   btupmt(usrnum,0);
	   ertply=&ertplayer[usrnum];
	   setmbk(ertmsg);
           dfaSetBlk(ertbb);
	   ert_prep();
           if (dfaQueryEQ(usaptr->userid, 0))
           {
              dfaGetEQ(NULL,usaptr->userid,0);
              dfaUpdate(ertply);
           }
	   setmem(ertply,sizeof(struct ertplayer),0);
	   ertply->saved = TRUE;
	   return(0);
	}
     }
     switch (usrptr->substt)
     {
     case 0:
	       temp = (SHORT)ert_rnd_num(3,1);
	       if (usaptr->ansifl&ANSON)
	       {
		  if (temp == 1)
		     prfmsg(WELC1);
		  else
		  if (temp == 2)
		     prfmsg(WELC2);
		  else
		     prfmsg(WELC3);
	       }
	       else
		  prfmsg(WELC4);
	       prfmsg(HITENTR);
	       outprf(usrnum);
	       prfmsg(ENTR3);
	       ert_prnt_all();
	       btupmt(usrnum,'>');
	       usrptr->substt=9;
	       break;
     case 1:
	  prfmsg(INTRO2);
	  outprf(usrnum);
	  usrptr->substt=2;
	  break;
     case 2:
	  prfmsg(INTRO3);
	  outprf(usrnum);
	  usrptr->substt=6;
	  break;
     case 3:
	  temp = (SHORT)ert_rnd_num(NROOMS-1,1);
	  prfmsg(ENTR6);
	  ert_prnt_all();
	  ertply->flags|=INWLD;
	  ert_link();
	  ert_unlink();
	  ertply->saved = FALSE;
	  ertgo_room(ertply,&ertroom_list[temp],getmsg(ENTR7));
	  usrptr->substt=5;
	  break;
     case 4:
	  prfmsg(INTRO3);
	  outprf(usrnum);
	  ertply->saved = FALSE;
	  usrptr->substt=5;
	  break;
     case 5:
	  ertply->saved = FALSE;
	  switch (margc)
	  {
	  case 0:
	       margv[0]="nada";
	  case 1:
	       margv[1]="nobody";
	  case 2:
	       margv[2]="nothing";
	  case 3:
	       margv[3]="noway";
	  case 4:
	       margv[4]="nowhere";
	  case 5:
	       margv[5]="nohow";
	  }
	  ert_parse();
	  break;
     case 6:
	   ert_init(usrnum);
	   prfmsg(CHARNAME);
	   outprf(usrnum);
	   usrptr->substt=7;
	   break;
     case 7:
	  if (strlen(margv[0]) < 3 || strlen(margv[0]) > 14 || margc > 1)
	  {
	     usrptr->substt=7;
	     prfmsg(NAMEPROB);
	     prfmsg(CHARNAME);
	     outprf(usrnum);
	     break;
	  }
	  else
	  {
	     zonkhl(margv[0]);
             strcpy(ertply->charac, margv[0]);
             if (dfaQueryEQ(ertply->charac, 1))
             {
                usrptr->substt=7;
                prfmsg(DIFFNAME);
                prfmsg(CHARNAME);
                outprf(usrnum);
                break;
             }
             else
             {
                prfmsg(GETPREF);
                outprf(usrnum);
                usrptr->substt=8;
             }
	     break;
	  }
     case 8:
	  go_to = 8;
	  if (margc > 0)
	  {
	     if (sameas(margv[0],"1"))
	     {
		strcpy(ertply->plyr_pref,"straight");
		ertply->aura|=STRAIG;
		ertply->pref_num = 1;
		go_to = 3;
	     }
	     if (sameas(margv[0],"2"))
	     {
		strcpy(ertply->plyr_pref,usaptr->sex == 'M' ? "gay" : "lesbian");
		ertply->aura|=GAYLES;
		ertply->pref_num = 2;
		go_to = 3;
	     }
	     if (sameas(margv[0],"3"))
	     {
		strcpy(ertply->plyr_pref,"bisexual");
		ertply->aura|=BISEXU;
		ertply->pref_num = 3;
		go_to = 3;
	     }
	     if (go_to == 8)
	     {
		prfmsg(BADIN);
		prfmsg(GETPREF);
		outprf(usrnum);
	     }
	     if (go_to != 8)
	     {
		if (sameas(ertply->hisher, "his"))
		   ertply->aura|=MALE;
		else
		   ertply->aura|=FEMALE;
		prfmsg(CREATED);
		outprf(usrnum);
		ertply->completed=TRUE;
	     }
	  }
	  else
	  {
	     go_to = 8;
	     prfmsg(BADIN);
	     prfmsg(GETPREF);
	     outprf(usrnum);
	  }
          if (go_to == 3)
          {
             dfaInsert(ertply);
          }
	  usrptr->substt=go_to;
	  break;
     case 9:
          if (dfaAcqEQ(ertply,usaptr->userid,0))
	  {
	       if (ertply->completed == FALSE)
	       {
		  prfmsg(INTRO1);
		  outprf(usrnum);
		  usrptr->substt=1;
		  break;
	       }
	       ert_rest();
	       ertply->flags&=~INWLD;
	       ertinv();
	       ertply->channel= (SHORT)usrnum;
	       ertply->flags|=INWLD;
	       ert_unlink();
	       if (ertply->flags&SYSOP)
	       {
		    ertgo_room(ertply,ertply->ertroom,getmsg(ENTR4));
	       }
	       else
	       {
		    ertgo_room(ertply,ertply->ertroom,getmsg(ENTR5));
	       }
	       ertply->saved = FALSE;
	       usrptr->substt=5;
	  }
	  else
	  {
	       prfmsg(INTRO1);
	       outprf(usrnum);
	       ertply->ertroom=&ertroom_list[0];
	       ert_init(usrnum);
	       ert_prep();
//               dfaInsert(ertply);
	       ert_rest();
	       usrptr->substt=1;
	  }
	  btupmt(usrnum,'>');
	  break;
     case 10:
	  if (sameto(margv[0], ertply->ertroom->key) && strlen(margv[0]) == 5 &&
	      !sameto("xxxx", ertply->ertroom->key)) /* key, if it's meant to be 4 characters, needs to be CHAR[5] -RH 8/10/2024 */
	  {
	     prfmsg(DOOROPN);
	     outprf(usrnum);
	     ertleave_room(ertply,getmsg(EXDOOR));
	     ertgo_room(ertply,&ertroom_list[ertply->keyroom],getmsg(ENDOOR));
	  }
	  else
	  {
	     prfmsg(NOOPEN);
	     outprf(usrnum);
	     ertlstitms(ertply);
	  }
	  usrptr->substt = 5;
	  break;
     case 11:
	  if (margc > 0)
	  {
	     if (sameas(margv[0], "q"))
	     {
		prfmsg(COLOR1);
		if (ertply->flags&BRIEF)
                   prfmsg(BRFDES1,(CHAR *)(ertply->ertroom->brdesc));
		else
		   prfmsg(ertply->ertroom->desc);
		prfmsg(COLOROFF);
		ertseeexit();
		ert_see_itms(ertply);
		ert_see_pyrs(ertply);
		outprf(usrnum);
		go_to = 5;
	     }
	     else
	     if (atoi(margv[0]) > ertply->nitems || atoi(margv[0]) < 1)
	     {
		prfmsg(GETREAL);
		outprf(usrnum);
		ertlstitms(ertply);
		go_to = 11;
	     }
	     else
	     {
		go_to = 11;
		temp = (SHORT)check_type(ertply, (SHORT)atoi(margv[0]));
		if (temp == TRUE)
		{
		   ert_attack(ertply);
		   if (ertply->limbo == TRUE)
		   {
		      go_to = 12;
		   }
		   else
		      ertlstitms(ertply);
		}
		else
		{
		   prfmsg(NOUSE);
		   outprf(usrnum);
		   ertlstitms(ertply);
		}
	     }
	  }
	  else
	  {
	     prfmsg(TRYEH);
	     outprf(usrnum);
	     ertlstitms(ertply);
	     go_to = 11;
	  }
	  usrptr->substt = go_to;
	  break;
     case 12:
	   ertply->limbo = FALSE;
	   if (margc > 0)
	   {
	      if (sameas(margv[0], "1"))
	      {
		 ertsex();
		 go_to = 5;
	      }
	      else
	      if (sameas(margv[0], "2"))
	      {
		 ertoralu();
		 go_to = 5;
	      }
	      else
	      if (sameas(margv[0], "3"))
	      {
		 ertuoral();
		 go_to = 5;
	      }
	      else
	      {
		 prfmsg(NOWHAT, ertply->arouse, ertply->arouse,
							   ertply->arouse);
		 outprf(usrnum);
		 go_to = 12;
	      }
	   }
	   else
	   {
	      prfmsg(NOWHAT, ertply->arouse, ertply->arouse, ertply->arouse);
	      outprf(usrnum);
	      go_to = 12;
	   }
	   usrptr->substt = go_to;
	   break;
     case 13:
	  go_to = 13;
	  if (margc > 0)
	  {
	     if (sameas(margv[0],"1"))
	     {
		strcpy(ertply->plyr_pref,"straight");
		ertply->aura|=STRAIG;
		ertply->pref_num = 1;
		go_to = 5;
	     }
	     if (sameas(margv[0],"2"))
	     {
		strcpy(ertply->plyr_pref,usaptr->sex == 'M' ? "gay" : "lesbian");
		ertply->aura|=GAYLES;
		ertply->pref_num = 2;
		go_to = 5;
	     }
	     if (sameas(margv[0],"3"))
	     {
		strcpy(ertply->plyr_pref,"bisexual");
		ertply->aura|=BISEXU;
		ertply->pref_num = 3;
		go_to = 5;
	     }
	     if (go_to == 13)
	     {
		prfmsg(BADIN);
		prfmsg(CHNGPRF);
		outprf(usrnum);
	     }
	     if (go_to != 13)
	     {
		if (sameas(ertply->hisher, "his"))
		   ertply->aura|=MALE;
		else
		   ertply->aura|=FEMALE;
		strcpy(temp2, usaptr->sex == 'M' ? "male" : "female");
		prfmsg(OKCHD, ertply->plyr_pref, temp2);
		outprf(usrnum);
		if (ertply->points > 25)
		   ertply->points = ertply->points - 25L;
		else
		   ertply->points = 0;
	     }
	  }
	  else
	  {
	     go_to = 13;
	     prfmsg(BADIN);
	     prfmsg(GETPREF);
	     outprf(usrnum);
	  }
	  usrptr->substt=go_to;
	  break;
     case 14:
	  prfmsg(COLOR1);
	  if (ertply->flags&BRIEF)
            prfmsg(BRFDES1,(CHAR *)(ertply->ertroom->brdesc));
	  else
	    prfmsg(ertply->ertroom->desc);
	  prfmsg(COLOROFF);
	  ertseeexit();
	  ert_see_itms(ertply);
	  ert_see_pyrs(ertply);
	  outprf(usrnum);
	  usrptr->substt = 5;
     }
     return(1);
}

SHORT ert_init(INT in_usrnum)
{
//   struct ertplayer *ertply;
   struct usracc *lusaptr;
   ertply = &(ertplayer[in_usrnum]);

   ertply=&ertplayer[in_usrnum];
   lusaptr=uacoff(in_usrnum);
   setmem(ertply,sizeof(struct ertplayer),0);
   strcpy(ertply->plyrid,lusaptr->userid);
   ertply->descrpt=(lusaptr->sex == 'M' ? NORMM : NORMF);
   strcpy(ertply->hisher,lusaptr->sex == 'M' ? "his" : "her");
   ertply->hits=ertply->maxhit=20;
   ertply->aura=SEEOBJ+SEEPYR+PICOBJ;
   ertply->flags=ONSYS+(sameas(lusaptr->userid,"Sysop") ? SYSOP : 0);
   ertply->channel= (SHORT)in_usrnum;
   ertply->room_num = (SHORT)0;
   ertply->points=0;
   ertply->dollars=25;
   ertply->bank_dollars=0;
   ertply->limbo = 0;
   strcpy(ertply->plyr_pref,"straight");
   ertply->aura|=STRAIG;
   ertply->pref_num = 1;
   ertply->completed = FALSE;
   ertply->saved = FALSE;
   return(0);
}

VOID erotica_hang_up(VOID)
{
   ertply=&ertplayer[usrnum];
   if (ertply->flags&ONSYS)
   {
      setmbk(ertmsg);
      dfaSetBlk(ertbb);
      if (ertply->saved == FALSE)
      {
	 if (ertply->flags&INWLD)
	 {
	    ert_relink();
	    ertleave_room(ertply,getmsg(EXIT3));
	 }
	 ert_prep();
         if (dfaQueryEQ(usaptr->userid, 0))
         {
            dfaGetEQ(NULL,usaptr->userid,0);
            dfaUpdate(ertply);
         }
      }
      setmem(ertply,sizeof(struct ertplayer),0);
      ertply->saved = TRUE;
   }
}

SHORT ert_prep(VOID)
{
     SHORT i;

     ertply->ertroom=(struct ertroom *)(ertply->ertroom-ertroom_list);
     for (i=0 ; i < ertply->nitems ; i++)
     {
	ertply->items[i]=(struct item *)(ertply->items[i]-ertitems);
     }
     return(0);
}

SHORT ert_rest(VOID)
{
     SHORT i;

     ertply->ertroom=&ertroom_list[(int)ertply->ertroom];
     for (i=0 ; i < ertply->nitems ; i++)
     {
	ertply->items[i]=&ertitems[(int)ertply->items[i]];
     }
     return(0);
}

VOID erotica_delete(CHAR *userid)
{
     dfaSetBlk(ertbb);
     if (dfaAcqEQ(NULL,userid,0))
     {
        dfaDelete();
     }
}

VOID erotica_clean_up(VOID)
{
     clsmsg(ertmsg);
     dfaClose(ertbb);
}

GBOOL ert_intro(VOID)
{
   ertply=&ertplayer[usrnum];
   ertply->saved = TRUE;
   setmbk(ertmsg);
   if (ert_tag_msg == 1)
   {
      if (hasmkey(ERTKEY))
      {
	 prfmsg(PLAYIT);
	 outprf(usrnum);
      }
   }
   return(0);
}

VOID ert_init_hiscore(VOID)
{
   CHAR name1[31];
   CHAR name2[31];

   dfaSetBlk(ertbb);
   ertply=&ertplayer[1];
   //if (dfaAcqLO(ertply, 0) != NULL)    /* rick 4/18/23: VS complains that int differs from void * - this returns GBOOL */
   if (dfaAcqLO(ertply, 0))
   {
      strcpy(name1, ertply->plyrid);
      dfaAcqHI(ertply, 0);
      if (ertply->completed == TRUE)
	 ert_insert(ertply->charac, ertply->points);
      strcpy(name2, ertply->plyrid);
      while ((!sameas(name2, name1)))
      {
         dfaAcqPR(ertply);
	 if (ertply->completed == TRUE)
	    ert_insert(ertply->charac, ertply->points);
	 strcpy(name2, ertply->plyrid);
      }
   }
}

VOID init_hiscore(VOID)
{
   SHORT i;

   for (i = 0; i < 11; i++)
   {
      strcpy(high_score[i].hname, ".............................");
      high_score[i].hscore = 0;
   }
}

VOID ert_init_inactive(VOID)
{
   CHAR name1[31];
   CHAR name2[31];
   dfaSetBlk(ertbb);
   ertply=&ertplayer[1];
   dfaSetBlk(ertbb);

   //if (dfaAcqLO(ertply, 0) != NULL)   /* rick 4/18/23: VS complains that int differs from void * - this returns GBOOL */
   if (dfaAcqLO(ertply, 0))
   {
      strcpy(name1, ertply->plyrid);
      dfaAcqHI(ertply, 0);
      ert_link();
      strcpy(name2, ertply->plyrid);
      while ((!sameas(name2, name1)))
      {
         dfaAcqPR(ertply);
	 if (ertply->completed == TRUE)
	    ert_link();
	 strcpy(name2, ertply->plyrid);
      }
   }
}

VOID ert_link(VOID)
{
   SHORT new, current, previous;

   current  = ertply->room_num;
   previous = current;
   new      = ert_struct_pos + 1;

   ertroom_list[ertply->room_num].num_nlv++;

   strcpy(ert_inact_lst[new].iname, ertply->charac);
   strcpy(ert_inact_lst[new].iplyrid, ertply->plyrid);
   ertply->aura=SEEOBJ+SEEPYR+PICOBJ;
   if (sameto(ertply->hisher, "his"))
      ert_inact_lst[new].aura|=MALE;
    else
      ert_inact_lst[new].aura|=FEMALE;
   ert_inact_lst[new].hits  = ertply->hits;
   ert_inact_lst[new].pref_num = ertply->pref_num;

   while(ert_inact_lst[current].ilink != 0)
   {
      current  = ert_inact_lst[current].ilink;
      previous = current;
   }
   ertply->hold_pos = new;
   ert_inact_lst[previous].ilink = new;
   ert_struct_pos++;
}

VOID ert_unlink(VOID)
{
   SHORT current, previous;
   SHORT hold_prev=-1, hold_current=-1, temp, found; // added defaults for hold_* because compiler thinks that even with if (found) check you could still use them uninitialized - RH 8/10/2024

   current  = ertply->room_num;
   previous = current;
   found    = FALSE;

   ertroom_list[ertply->room_num].num_nlv--;

   while(ert_inact_lst[current].ilink != 0)
   {
      if (sameas(ert_inact_lst[current].iplyrid, ertply->plyrid))
      {
        hold_prev    = previous;
        hold_current = current;
        found        = TRUE;
      }
      previous = current;
      current = ert_inact_lst[current].ilink;
   }
   
   if (found == FALSE && (sameas(ert_inact_lst[current].iplyrid, ertply->plyrid)))
   {
      ert_inact_lst[current].ilink  = 0;
      ert_inact_lst[previous].ilink = 0;
      ertply->hold_pos              = current;
   }
   else if ( (found) && (hold_prev >= 0) && (hold_current >= 0) )
   {
      temp = ert_inact_lst[hold_current].ilink;
      ert_inact_lst[hold_current].ilink = 0;
      ert_inact_lst[hold_prev].ilink    = temp;
      ertply->hold_pos                  = hold_current;
   }
   // what if not found and the player isn't on the ert_inact_lst for the current room?
   // this is a logical gap here - RH 8/10/2024
}

VOID ert_relink(VOID)
{
   SHORT current;
   current = ertply->room_num;

   ertroom_list[ertply->room_num].num_nlv++;

   while(ert_inact_lst[current].ilink != 0)
   {
      current = ert_inact_lst[current].ilink;
   }
   ert_inact_lst[current].ilink = ertply->hold_pos;
   ert_inact_lst[current].hits  = ertply->hits;
}

SHORT ertprn(VOID)
{
   SHORT i;

   prfmsg(SYSLST1);
   for (i = NROOMS + 1; i < 1000; i++)
   {
      if (sameto(" ",ert_inact_lst[i].iname))
	 break;
      prfmsg(SYSLST2, ert_inact_lst[i].iplyrid, ert_inact_lst[i].iname);
      outprf(usrnum);
   }
   return(0);
}

/***************************************************************************
*                                                                          *
*     Miscellaneous game routines                                          *
*                                                                          *
***************************************************************************/

VOID ertsex(VOID)
{
   SHORT check, has_condom;

   has_condom = FALSE;
   check = (SHORT)ert_rnd_num(100, 1);

   if (ert_get_ply(ertply, "condom") != NULL) /* RH: This is a const char string literal */
      has_condom = TRUE;

   if (sameas(ertply->hisher, "his")) /* Player is a male */
   {
      switch (ertply->osex)
      {
	 case 0: /* Male */
	    prfmsg(SEXM2M, ertply->arouse, ertply->arouse, ertply->arouse,
			   ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
	 case 1: /* Female */
	    prfmsg(SEXMF, ertply->arouse, ertply->arouse, ertply->arouse,
			  ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
      }
   }
   else                               /* Player is a female */
   {
      switch (ertply->osex)
      {
	 case 0: /* Male */
	    prfmsg(SEXFM, ertply->arouse, ertply->arouse, ertply->arouse,
			  ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
	 case 1: /* Female */
	    prfmsg(SEXFF, ertply->arouse, ertply->arouse, ertply->arouse,
			  ertply->arouse, ertply->arouse, ertply->arouse,
			  ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
      }
   }
}

VOID ertuoral(VOID)
{
   SHORT check, has_condom;

   has_condom = FALSE;
   check = (SHORT)ert_rnd_num(100, 1);

   if (ert_get_ply(ertply, "condom") != NULL) /* RH: This is a const char string literal */
      has_condom = TRUE;

   if (sameas(ertply->hisher, "his")) /* Player is a male */
   {
      switch (ertply->osex)
      {
	 case 0: /* Male */
	    prfmsg(ORLM2M, ertply->arouse, ertply->arouse, ertply->arouse,
			   ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
	 case 1: /* Female */
	    prfmsg(ORLFF2M, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse, ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
      }
   }
   else                               /* Player is female */
   {
      switch (ertply->osex)
      {
	 case 0: /* Male */
	    prfmsg(ORLFM2F, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse, ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
	 case 1: /* Female */
	    prfmsg(ORLFF2F, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse, ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
      }
   }
}

VOID ertoralu(VOID)
{
   SHORT check, has_condom;

   has_condom = FALSE;
   check = (SHORT)ert_rnd_num(100, 1);

   if (ert_get_ply(ertply, "condom") != NULL)
      has_condom = TRUE;

   if (sameas(ertply->hisher, "his")) /* Player is a male */
   {
      switch (ertply->osex)
      {
	 case 0: /* Male */
	    prfmsg(ORLFM2M, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
	 case 1: /* Female */
	    prfmsg(ORL2M2F, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse, ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
      }
   }
   else
   {
      switch (ertply->osex)
      {
	 case 0: /* Male */
	    prfmsg(ORL2F2M, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse, ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
	 case 1: /* Female */
	    prfmsg(ORL2F2F, ertply->arouse, ertply->arouse, ertply->arouse,
			     ertply->arouse);
	    if (check >=70 && has_condom == FALSE)
	    {
	       prfmsg(DISEASE);
	       if (ertply->hits > (ertply->maxhit / 2))
		  ertply->hits = (ertply->maxhit / 2);
	       ertply->has_disease = TRUE;
	    }
	    else
	    if (check >= 70 && has_condom == TRUE)
	       prfmsg(NODISE);
	    outprf(usrnum);
	    break;
      }
   }
}

SHORT ert_high(VOID)
{
   SHORT i;

   prfmsg(HILINE);
   for (i = 0; i < 10; i++)
   {
      prfmsg(SCRLN, i + 1, high_score[i].hname, high_score[i].hscore);
      outprf(usrnum);
   }
   return(0);
}

VOID ert_insert(CHAR name[31], ULONG score)
{
   SHORT i, curpos;
   ULONG temp_score, temp_score2;
   CHAR temp_name[31], temp_name2[31];

   curpos = -1;

   for (i = 0; i < 10; i++)
   {
      if (score >= high_score[9-i].hscore)
      curpos = 9-i;
   }
   if (curpos != -1)
   {
      strcpy(temp_name2, name);
      temp_score2 = score;
      for (i = curpos; i < 10; i++)
      {
	 strcpy(temp_name, high_score[i].hname);
	 temp_score = high_score[i].hscore;
	 strcpy(high_score[i].hname, temp_name2);
	 high_score[i].hscore = temp_score2;
	 strcpy(temp_name2, temp_name);
	 temp_score2 = temp_score;
      }
   }
}

VOID ert_del_name(CHAR name[30])
{
   SHORT i, curpos;

   curpos = -1;

   for (i = 0; i < 10; i++)
   {
      if (sameas(name, high_score[i].hname))
      curpos = i;
   }
   if (curpos != -1)
   {
      for (i = curpos; i < 10; i++)
      {
	 strcpy(high_score[i].hname, high_score[i+1].hname);
	 high_score[i].hscore = high_score[i+1].hscore;
      }
   }
}


VOID erthitoth(SHORT nhits)
{
   SHORT i;

   if ((ertoth->hits-=nhits) <= 0)
      {
	 i = (SHORT)ert_rnd_num(NROOMS-1,1);
	 prfmsg(DIED, ertply->charac);
	 outprf(ertoth->channel);
	 clrprf();
	 prfmsg(YOUFND, ertoth->dollars, ertoth->charac);
	 outprf(usrnum);
	 ert_add_money(ertoth->dollars);
	 if (ertoth->has_disease == FALSE)
	    ertoth->hits = ertoth->maxhit;
	 else
	    ertoth->hits = (ertoth->maxhit / 2);
	 ertoth->dollars = 0;
	 ertleave_room(ertoth, getmsg(PDIED));
	 ertgo_room(ertoth, &ertroom_list[i], getmsg(BORNAG));
	 prfmsg(SBMTU, ertply->arouse);
	 outprf(usrnum);
	 prfmsg(SUBMIT, ertply->arouse, ertply->charac);
	 ert_room_out();
	 prfmsg(NOWHAT, ertply->arouse, ertply->arouse, ertply->arouse);
	 outprf(usrnum);
	 ertply->limbo = 1;
      }
}

VOID erthityou(SHORT nhits)
{
   SHORT i;

   if ((ertply->hits-=nhits) <= 0)
   {
      i = (SHORT)ert_rnd_num(NROOMS-1,1);
      prfmsg(DIED, ertply->arouse);
      outprf(usrnum);
      usrptr->substt = 5;
      ertply->dollars = 0;
      if (ertply->has_disease == FALSE)
	 ertply->hits = ertply->maxhit;
      else
	 ertply->hits = (ertply->maxhit / 2);
      ertleave_room(ertply, getmsg(PDIED));
      ertgo_room(ertply, &ertroom_list[i], getmsg(BORNAG));

   }
}

SHORT ert_lbrry(VOID)
{
   if (sameas(margv[0], "catalog"))
   {
      prfmsg(CATALOG);
      outprf(usrnum);
      return(0);
   }
   else
   if (sameas(margv[0], "read"))
   {
      prfmsg(JSTREAD, ertply->charac);
      ert_room_out();
      if (sameas(margv[1], "ref1"))
      {
	 prfmsg(REF1);
	 outprf(usrnum);
	 return(0);
      }
      if (sameas(margv[1], "ref2"))
      {
	 prfmsg(REF2);
	 outprf(usrnum);
	 return(0);
      }
      if (sameas(margv[1], "ref3"))
      {
	 prfmsg(REF3);
	 outprf(usrnum);
	 return(0);
      }
      if (sameas(margv[1], "ref4"))
      {
	 prfmsg(REF4);
	 outprf(usrnum);
	 return(0);
      }
   }
   return(1);
}

SHORT ert_medic(VOID)
{
   ert_del_the();
   ert_del_prep();
   if (margc == 2)
   {
      if ((sameas(margv[0], "read") || sameas(margv[0], "look"))
						&& sameas(margv[1], "sign"))
      {
	 prfmsg(DOCSIGN);
	 outprf(usrnum);
	 return(0);
      }
      else
      if (sameas(margv[0], "cure") && sameas(margv[1], "me"))
      {
	 if (ert_enough_money(10))
	 {
	    ertply->has_disease = FALSE;
	    ert_subtract_money(10);
	    prfmsg(DOC3);
	    outprf(usrnum);
	    return(0);
	 }
	 else
	 {
	   prfmsg(DOC2);
	   outprf(usrnum);
	   return(0);
	 }
      }
   }
   return(1);
}

SHORT ertpub(VOID)
{
   if (sameas(margv[0], "menu"))
   {
      prfmsg(PUBMENU);
      outprf(usrnum);
      return(0);
   }
   if (margc > 1)
   {
      if (sameas(margv[0], "buy"))
      {
	 if (ert_enough_money(1) && sameto(margv[1], "slippery-nipple"))
	 {
	    prfmsg(PUB1, "slippery-nipple");
	    outprf(usrnum);
	    prfmsg(PUB2, ertply->charac);
	    ert_room_out();
	    ert_subtract_money(1);
	    ert_add_hits(1);
	    return(0);
	 }
	 else
	 if (ert_enough_money(2) && sameas(margv[1], "sex-on-the-beach"))
	 {
	    prfmsg(PUB1, "sex-on-the-beach");
	    outprf(usrnum);
	    prfmsg(PUB4, ertply->charac);
	    ert_room_out();
	    ert_subtract_money(2);
	    ert_add_hits(2);
	    return(0);
	 }
	 else
	 if (ert_enough_money(1) && sameas(margv[1], "slow-screw"))
	 {
	    prfmsg(PUB1, "slow-screw");
	    outprf(usrnum);
	    prfmsg(PUB6, ertply->charac);
	    ert_room_out();
	    ert_subtract_money(1);
	    ert_add_hits(1);
	    return(0);
	 }
	 else
	 if (ert_enough_money(2) && sameas(margv[1], "blow-job"))
	 {
	    prfmsg(PUB1, "blow-job");
	    outprf(usrnum);
	    prfmsg(PUB8, ertply->charac);
	    ert_room_out();
	    ert_subtract_money(2);
	    ert_add_hits(2);
	    return(0);
	 }
	 else
	 if (ert_enough_money(2) && sameas(margv[1], "orgasm"))
	 {
	    prfmsg(PUB1, "orgasm");
	    outprf(usrnum);
	    prfmsg(PUB10, ertply->charac);
	    ert_room_out();
	    ert_subtract_money(2);
	    ert_add_hits(2);
	    return(0);
	 }
	 else
	 {
	    prfmsg(DONTK);
	    outprf(usrnum);
	    return(0);
	 }
      }
   }
   return(1);
}

SHORT ertarouse(VOID)
{
    struct monster *mnstpt;
    SHORT i, monpos=-1, struct_pos;

   if (margc > 1)
   {
      if (ertply->nitems < 1)
      {
	 prfmsg(NDITM);
	 outprf(usrnum);
      }
      else
      if ((struct_pos = (SHORT)ertfnlv(margv[1])) > FALSE)
      {
	 strcpy(ertply->arouse, ert_inact_lst[struct_pos].iname);
	 if (check_compatibility(ert_inact_lst[struct_pos].iname) == FALSE)
	 {
	       prfmsg(NCOMPT, ert_inact_lst[struct_pos].iname);
	       outprf(usrnum);
	 }
	 else
	 if (ert_inact_lst[struct_pos].hits < 1)
	 {
	    prfmsg(RECOVER);
	    outprf(usrnum);
	 }
	 else
	 {
	       prfmsg(ATTMPT, ert_inact_lst[struct_pos].iname);
	       outprf(usrnum);
	       ertlstitms(ertply);
	       usrptr->substt = 11;
	 }
      }
      else
      if ((mnstpt = ertfmnstr(ertply->ertroom, margv[1])) != NULL)
      {
	 for (i=0; i < MXITEM; i++)
	 {
	    if (mnstpt->mnstnum == ertply->ertroom->ertmlist[i])
	    {
	       monpos = i;
	       break;
	    }
	 }
	 if ((monpos >= 0) && (ertply->ertroom->ertmhps[monpos] <= 0))
	 {
	    prfmsg(RECOVER);
	    outprf(usrnum);
	 }
	 else
	 {
	    if (check_compatibility(mnstpt->mname) == FALSE)
	    {
	       prfmsg(NCOMPT, ert_nam(mnstpt->mname));
	       outprf(usrnum);
	    }
	    else
	    {
	       strcpy(ertply->arouse, mnstpt->mname);
	       prfmsg(ATTMPT, ert_nam(mnstpt->mname));
	       outprf(usrnum);
	       ertlstitms(ertply);
	       usrptr->substt = 11;
	    }
	 }
      }
      else
      if ((ertoth = ertfplyr(margv[1])) != NULL)
      {
	 if (check_compatibility(ertoth->charac) == FALSE)
	 {
	    prfmsg(NCOMPT, ert_nam(ertoth->charac));
	    outprf(usrnum);
	 }
	 else
	 if (sameas(ertoth->charac, ertply->charac))
	 {
	    prfmsg(NOMAST);
	    outprf(usrnum);
	 }
	 else
	 {
	    strcpy(ertply->arouse, ertoth->charac);
	    prfmsg(ATTMPT, ertoth->charac);
	    outprf(usrnum);
	    ertlstitms(ertply);
	    usrptr->substt = 11;
	 }
      }
      else
      {
	 prfmsg(NOSEE);
	 outprf(usrnum);
      }
   }
   else
   {
      prfmsg(ARSWHO);
      outprf(usrnum);
   }
   return(0);
}

SHORT check_compatibility(CHAR name[30])
{
   struct monster *mnstpt;
   SHORT preference=1, osex=0, temp=FALSE;  // these defaults are set because the if-else blocks before the switch do not cover all potential cases to set them! They may not be correct - RH 8/10/2024
   SHORT struct_pos;

   if ((mnstpt = ertfmnstr(ertply->ertroom, name)) != NULL)
   {
      preference = mnstpt->mpref;
      osex       = mnstpt->msex;
   }
   else if ((ertoth = ertfplyr(name)) != NULL)
   {
      preference = ertoth->pref_num;
      if (sameto(ertoth->hisher, "his"))
        osex = 0;
      else
        osex = 1;
   }
   else if ((struct_pos = (SHORT)ertfnlv(name)) > FALSE)
   {
      if (ert_inact_lst[struct_pos].aura&MALE)
        osex = 0;
      else
        osex = 1;
      preference = ert_inact_lst[struct_pos].pref_num;
   }

   ertply->osex = osex;

   switch (ertply->pref_num)
   {
     case 1: /* Straight */
       if (sameas(ertply->hisher, "his"))
       {
         if (preference == 1)
         {
           if (osex == 0)
             temp = FALSE;
           else
             temp = TRUE;
         }
         else if (preference == 2)
           temp = FALSE;
         else
         {
           if (osex == 0)
             temp = FALSE;
           else
             temp = TRUE;
         }
       }
       else  /* Female */
       {
         if (preference == 1)
         {
           if (osex == 0)
             temp = TRUE;
           else
             temp = FALSE;
         }
         else if (preference == 2)
           temp = FALSE;
         else if (preference == 3)
         {
           if (osex == 0)
             temp = TRUE;
           else
             temp = FALSE;
         }
       }
       break;
     case 2: /* Gay/Lesbian */
       if (sameas(ertply->hisher, "his"))
       {
         if (preference == 1)
         {
           temp = FALSE;
         }
         else if (preference == 2)
         {
           if (osex == 0)
             temp = TRUE;
           else
             temp = FALSE;
         }
         else
         {
           if (osex == 0)
             temp = TRUE;
           else
             temp = FALSE;
         }
       }
       else /* Female */
       {
         if (preference == 1)
         {
           temp = FALSE;
         }
         else if (preference == 2)
         {
           if (osex == 0)
             temp = FALSE;
           else
             temp = TRUE;
         }
         else
         {
           if (osex == 0)
             temp = FALSE;
           else
             temp = TRUE;
         }
       }
       break;
     case 3: /* Bisexual */
       if (sameas(ertply->hisher, "his"))
       {
         if (preference == 1)
         {
           if (osex == 0)
             temp = FALSE;
           else
             temp = TRUE;
         }
         else if (preference == 2)
         {
           if (osex == 0)
             temp = TRUE;
           else
             temp = FALSE;
         }
         else
           temp = TRUE;
       }
       else
       {
         if (preference == 1)
         {
           if (osex == 0)
             temp = TRUE;
           else
             temp = FALSE;
         }
         else if (preference == 2)
         {
           if (osex == 0)
             temp = FALSE;
           else
             temp = TRUE;
         }
         else
           temp = TRUE;
       }
   }
   return(temp);
}


SHORT check_type(struct ertplayer *ppt, SHORT num)
{
   struct item **items;
   struct item *itmptr;
   CHAR *ert_ilst();
   SHORT i, ret_val = 0;
   items=ppt->items;

   itmptr = ert_get_ply(ertply, ert_ilst(items[num-1]));
   ert_put_ply(ertply, itmptr);

   for (i=0; i < NMITEM; i++)
   {
      if (sameas(itmptr->name, ertitems[i].name))
      {
	 if (ertitems[i].item_def == OFFNSE)
	 {
	    ret_val = TRUE;
	 }
	 else
	 {
	    ret_val = FALSE;
	 }
      }
   }
   return(ret_val);
}

VOID ertlstitms(struct ertplayer *ppt)
{
   struct item **items;
   CHAR *ert_ilst();
   SHORT nitms,i;

   items=ppt->items;
   nitms=ppt->nitems;

   if (ppt->nitems > 0)
   {
      for (i=0 ; i < nitms-1 ; i++)
      {
	 prfmsg(ERTITMS, i + 1, ert_nam(ert_ilst(items[i])));
      }
      prfmsg(ERTITMS, i + 1, ert_nam(ert_ilst(items[nitms-1])));
   }
   prfmsg(QUITLN);
   outprf(usrnum);
}

CHAR *ert_ilst(struct item *itm)
{
   return(itm->name);
}

VOID ert_attack(struct ertplayer *ppt)
{
   struct item **items;
   struct item *itmptr;
   struct monster *mnstpt;

   SHORT i, monpos = -1 , hits, mwpn; // add a default for the moster position - RH 8/10/2024
   SHORT struct_pos;

   while (TRUE)
   {
     mwpn = (SHORT)ert_rnd_num(NMITEM, 1);
     if (ertitems[mwpn].item_def == OFFNSE)
       break;
   }

   items=ppt->items;
   itmptr = ert_get_ply(ertply, ert_ilst(items[ertply->nitems-1]));

   if ((mnstpt = ertfmnstr(ertply->ertroom, ertply->arouse)) != NULL)
   {
     for (i=0; i < MXITEM; i++)
     {
       if (mnstpt->mnstnum == ertply->ertroom->ertmlist[i])
       {
         monpos = i;
         break;
       }
     }
     // theoretically, monpos may not have been initialized, although in reality that shouldn't happen.
     if (monpos == -1)
     {
         monpos = MXITEM-1; // set it to the high point of the loop; this may not be right RH 8/10/2024
     }
     hits = itmptr->item_pwr;
     ertply->points = ertply->points + hits;
     prfmsg(GOTYA, itmptr->name, ert_nam(mnstpt->mname));
     outprf(usrnum);
     prfmsg(DIDEM, ertply->charac,ppt->hisher,itmptr->name,ert_nam(mnstpt->mname));
     ert_room_out();
     if (ertply->ertroom->ertmhps[monpos] - hits <= 0)
     {
       prfmsg(SBMTU, ert_nam(mnstpt->mname));
       outprf(usrnum);
       erttreasure_table(mnstpt->tt1, mnstpt->tt2);
       prfmsg(SUBMIT, ert_nam(mnstpt->mname), ertply->charac);
       ert_room_out();
       ertply->ertroom->ertmhps[monpos] = 0;
       prfmsg(NOWHAT,ert_nam(mnstpt->mname),ert_nam(mnstpt->mname),ert_nam(mnstpt->mname));
       outprf(usrnum);
       ertply->limbo = TRUE;
     }
     else
     {
       ppt->ertroom->ertmhps[monpos] = ertply->ertroom->ertmhps[monpos] - hits;
       hits = ertitems[mwpn].item_pwr;
       prfmsg(RBTATT, ert_nam(mnstpt->mname), ertitems[mwpn].name);
       outprf(usrnum);
       erthityou(hits);
     }
   }
   else if ((ertoth=ertfplyr(ertply->arouse)) != NULL)
   {
     if (check_defenses(itmptr->name))
     {
       hits = itmptr->item_pwr;
       ertply->points = ertply->points + (hits / 2);
       erthitoth(hits);
       prfmsg(GOTYA, itmptr->name, ertoth->charac);
       outprf(usrnum);
       prfmsg(ATTCKU, ertply->charac, ertply->hisher, itmptr->name);
       outprf(ertoth->channel);
       prfmsg(DIDEM, ppt->charac, ppt->hisher, itmptr->name, ertoth->charac);
       ert_out_evnt(ertoth);
     }
     else
     {
       prfmsg(DEFENSE, ertoth->charac, itmptr->name);
       outprf(usrnum);
     }
   }
   else if ((struct_pos = (SHORT)ertfnlv(ertply->arouse)) > FALSE)
   {
     if (check_odefenses(itmptr->name))
     {
       hits = itmptr->item_pwr;
       ertply->points = ertply->points + (hits / 2);
       prfmsg(GOTYA, itmptr->name, ertply->arouse);
       outprf(usrnum);
       prfmsg(DIDEM, ppt->charac, ppt->hisher, itmptr->name, ertply->arouse);
       ert_room_out();
       if (ert_inact_lst[struct_pos].hits - hits <= 0)
       {
         prfmsg(SBMTU, ertply->arouse);
         outprf(usrnum);
         prfmsg(SUBMIT, ertply->arouse, ertply->charac);
         ert_room_out();
         ert_inact_lst[struct_pos].hits = 0;
         prfmsg(NOWHAT, ertply->arouse, ertply->arouse, ertply->arouse);
         outprf(usrnum);
         ertply->limbo = 1;
       }
       else
       {
         ert_inact_lst[struct_pos].hits = ert_inact_lst[struct_pos].hits - hits;
         hits = ertitems[mwpn].item_pwr;
         prfmsg(RBTATT, ertply->arouse, ertitems[mwpn].name);
         outprf(usrnum);
         erthityou(hits);
       }
     }
     else
     {
       prfmsg(DEFENSE, ertply->arouse, itmptr->name);
       outprf(usrnum);
     }
   }
   else
   {
     prfmsg(NTHERE, ertply->arouse);
     outprf(usrnum);
     prfmsg(CRAZY, ertply->charac);
     ert_room_out();
   }
}

SHORT check_defenses(CHAR itm[20])
{
   if ((ertoth=ertfplyr(ertply->arouse)) != NULL)
   {
      if (ertoth->aura&HNDCUF && sameto("handcuffs", itm))
	 return(FALSE);
      else
      if (ertoth->aura&SEXCLN && sameto("spanish-fly", itm))
	 return(FALSE);
      else
      if (ertoth->aura&CHSTYB && sameto("vibrator", itm))
	 return(FALSE);
      else
      if (ertoth->aura&UNSUBM && sameto("whip", itm))
	 return(FALSE);
      else
      if (ertoth->aura&SCISSR && sameto("silk-scarf", itm))
	 return(FALSE);
      else
      if (ertoth->aura&GLASSE && sameto("playhouse-magazine", itm))
	 return(FALSE);
      else
	 return(TRUE);
   }
   return(FALSE);
}

SHORT check_odefenses(CHAR itm[20])
{
   int struct_pos;

   if ((struct_pos = ertfnlv(ertply->arouse)) > FALSE)
   {
      if (ert_inact_lst[struct_pos].aura&HNDCUF && sameto("handcuffs", itm))
	 return(FALSE);
      else
      if (ert_inact_lst[struct_pos].aura&SEXCLN && sameto("spanish-fly", itm))
	 return(FALSE);
      else
      if (ert_inact_lst[struct_pos].aura&CHSTYB && sameto("vibrator", itm))
	 return(FALSE);
      else
      if (ert_inact_lst[struct_pos].aura&UNSUBM && sameto("whip", itm))
	 return(FALSE);
      else
      if (ert_inact_lst[struct_pos].aura&SCISSR && sameto("silk-scarf", itm))
	 return(FALSE);
      else
      if (ert_inact_lst[struct_pos].aura&GLASSE && sameto("playhouse-magazine", itm))
	 return(FALSE);
      else
	 return(TRUE);
   }
   return(FALSE);
}

SHORT ertbank(VOID)
{
   if (sameas(margv[0], "deposit"))
   {
      if (atoi(margv[1]) > ertply->dollars || atoi(margv[1]) < 1)
      {
	 prfmsg(BANK1);
	 outprf(usrnum);
	 return(0);
      }
      else
      {
	 prfmsg(BANK2);
	 outprf(usrnum);
	 ertply->dollars = ertply->dollars - (SHORT)atoi(margv[1]);
	 ertply->bank_dollars = ertply->bank_dollars + (SHORT)atoi(margv[1]);
	 return(0);
      }
   }
   if (sameas(margv[0], "balance"))
   {
      prfmsg(BANK4, ertply->bank_dollars);
      outprf(usrnum);
      return(0);
   }
   if (sameas(margv[0], "withdraw"))
   {
      if ((SHORT)atoi(margv[1]) > ertply->bank_dollars || (SHORT)atoi(margv[1]) < 0)
      {
	 prfmsg(BANK5);
	 outprf(usrnum);
	 return(0);
      }
      else
      {
	 prfmsg(BANK6);
	 outprf(usrnum);
	 ertply->dollars = ertply->dollars + (SHORT)atoi(margv[1]);
	 ertply->bank_dollars = ertply->bank_dollars - (SHORT)atoi(margv[1]);
	 return(0);
      }
   }
   return(1);
}

SHORT ertmgc_shop(VOID)
{
   SHORT ret_val;

   ret_val = ertshops(MGC);
   return(ret_val);
}

SHORT ertgnrl_shop(VOID)
{
   SHORT ret_val;

   ret_val = ertshops(GENERIC);
   return(ret_val);
}

SHORT ertarmr_shop(VOID)
{
   SHORT ret_val;

   ret_val = ertshops(DEFNSE);
   return(ret_val);
}

SHORT ertwpn_shop(VOID)
{
   SHORT ret_val;

   ret_val = ertshops(OFFNSE);
   return(ret_val);
}

SHORT ertshops(SHORT shop_type)
{
   SHORT i, x;

   if (sameas(margv[0], "buy"))
      {
      if (ertply->nitems < MAXINV)
	 {
	    for (i=0; i < NMITEM; i++)
	    {
	       if (sameas(margv[1], ertitems[i].name))
	       {
		  x = ert_enough_money(ertitems[i].item_gld);
		  if (x == FALSE)
		  {
		     prfmsg(NOGOLD);
		     outprf(usrnum);
		     return(0);
		  }
		  if (x == TRUE)
		  {
		     if (ertitems[i].stock > 0)
		     {
			prfmsg(SOLD);
			outprf(usrnum);
			ertcheck_item(ertitems[i].name);
			ert_subtract_money(ertitems[i].item_gld);
			ertitems[i].stock = ertitems[i].stock - 1;
			ert_put_ply(ertply, &ertitems[i]);
			return(0);
		     }
		     else
		     {
			prfmsg(NOSTOCK);
			outprf(usrnum);
			return(0);
		     }
		  }
	       }
	    }
	    prfmsg(COJ1);
	    outprf(usrnum);
	    return(0);
	 }
	 prfmsg(COJ2);
	 outprf(usrnum);
	 return(0);
      }
      if (sameas(margv[0], "sell"))
      {
	 for (i=0; i < NMITEM; i++)
	 {
	    if (sameas(margv[1], ertitems[i].name) &&
		 ertitems[i].item_def == shop_type)
	    {
	       ert_del_the();
	       ert_del_prep();
	       if ((ert_get_ply(ertply, margv[1])) == NULL)
	       {
		  prfmsg(YOUDONT);
		  outprf(usrnum);
		  return(0);
	       }
	       else
	       {
		  ertcheck_item2(ertitems[i].name);
		  ertitems[i].stock = ertitems[i].stock + 1;
		  ert_add_money((ertitems[i].item_gld / 2));
		  prfmsg(SELL);
		  outprf(usrnum);
		  return(0);
	       }

	    }
	 }
	 prfmsg(NOPRICE);
	 outprf(usrnum);
	 return(0);
      }
      if (sameas(margv[0], "stock"))
      {
      btupmt(usrnum, 0);
      prfmsg(STOCKHDR);
      outprf(usrnum);
      for (i=0; i < NMITEM; i++)
	 {
	    if (shop_type == ertitems[i].item_def && ertitems[i].stock > 0)
	    {
	       prfmsg(STOCKMSG, ert_nam(ertitems[i].name), ertitems[i].stock,
		       ertitems[i].item_gld);
	       outprf(usrnum);
	    }
	 }
	 btupmt(usrnum, '>');
	 return(0);
      }
      return(1);
}

SHORT ert_rnd_num(SHORT max, SHORT rolls)
{
   SHORT total, num, i;
   UINT seed;
   total = 0;
   if (max < 1)
      return(0);
   seed = ert_rnd();
   srand(seed);
   for (i = 0; i < rolls ; i++)
   {
      num = rand() % max + 1;
      total = total + num;
   }
   return(total);
}

SHORT ert_enough_money(SHORT cost_dollars)

{
   if (ertply->dollars >= cost_dollars)
      return(1);
   else
      return(0);
}

VOID ert_subtract_money(SHORT cost_dollars)
{
      ertply->dollars -= cost_dollars;
}

VOID ert_add_money(SHORT dollars_add)
{
   if((dollars_add + ertply->dollars) > 25000)
      ertply->dollars = 25000;
   else
      ertply->dollars += dollars_add;
}

VOID ert_add_hits(SHORT num_add)
{
   if ((num_add + ertply->hits) > ertply->maxhit)
      ertply->hits = ertply->maxhit;
   else
      ertply->hits = ertply->hits + num_add;
   if (ertply->has_disease == TRUE)
   {
      if (ertply->hits > (ertply->maxhit / 2))
	 ertply->hits = (ertply->maxhit / 2);
   }
}

SHORT erttreasure_table(SHORT t_type1, SHORT t_type2)
{
   SHORT i;

   if (t_type1 == 0 && t_type2 == 0)
   {
      prfmsg(NOTHING);
      outprf(usrnum);
      return(0);
   }
   if (t_type2 > 0)
   {
      if (ertply->ertroom->nitems >= MXITEM)
      {
	 prfmsg(MNOHOLD);
      }
      else
      {
	 ert_itm_rm(ertply->ertroom, &ertitems[t_type2]);
	 prfmsg(MNDRP, ertitems[t_type2].name);
      }
   }
   if (t_type1 > 0)
   {
      i = (SHORT)ert_rnd_num(t_type1, 1);
      prfmsg(FOUNDGLD, i, ertply->arouse);
      ert_add_money(i);
   }
   outprf(usrnum);
   return(0);
}

SHORT ert_casino(VOID)
{
   SHORT num1, num2, num3, win=0, won;

   ert_del_the();
   ert_del_prep();
   if (margc == 2)
   {
      if (sameas(margv[0], "play") && sameas(margv[1], "slots"))
      {
	 if (ert_enough_money(1))
	 {
	    won = TRUE;
	    num1 = (SHORT)ert_rnd_num(9,1);
	    num2 = (SHORT)ert_rnd_num(9,1);
	    num3 = (SHORT)ert_rnd_num(9,1);
	    prfmsg(SLOTS, num1, num2, num3);
	    outprf(usrnum);
	    if ((num1 == num2) && (num2 == num3))
	       win = 40;
	    else
	    if (num2 == num3)
	       win = 10;
	    else
	    if (num1 == num2)
	       win = 10;
	     else
	     {
		won = FALSE;
		ert_subtract_money(1);
		prfmsg(LSTGLD);
	     }
	     if (won == TRUE)
	     {
		prfmsg(WONGLD, win);
		ert_add_money(win);
	     }
	    outprf(usrnum);
	    return(0);
	 }
	 else
	 {
	    prfmsg(NOGOLD);
	    outprf(usrnum);
	    return(0);
	 }
      }
   }
   return(1);
}

/*STATIC int
ertmail(VOID)
{
   setmsg(ertmsg);
   btupmt(usrnum, 0);
   if (margc == 1 && sameas(margv[0],"X"))
   {
      return(0);
   }
   do
   {
      bgncnc();
      cncchr();
      prfmsg(HELLO);
      outprf(usrnum);
      bgnedt(ERTSIZ, ertptr->text,
	     TPCSIZ, ertptr->topic, ertdun, ED_CLRTOP+ED_CLRTXT);
      break;
   }
   while (!endcnc());
   outprf(usrnum);
   return(1);
}

STATIC int
ertdun(int quitex)
{
   CHAR *cp;

   setmbk(ertmsg);
   if (quitex == 0)
   {
      for (cp = ertptr->text; *cp != '\0'; cp++)
      {
	 if (*cp == '\r')
	 {
	    *cp='\n';
	 }
      }
   // check file  here
   }
   // save here

   usrptr->state = ertstt;
   usrptr->substt = 5;
   prf("\rLeaving Editor...\r\r");
   prfmsg(COLOR1);
   if (ertply->flags&BRIEF)
      prfmsg(BRFDES1,(CHAR *)(ertply->ertroom->brdesc));
   else
      prfmsg(ertply->ertroom->desc);
   prfmsg(COLOROFF);
   ertseeexit();
   ert_see_itms(ertply);
   ert_see_pyrs(ertply);
   btupmt(usrnum,'>');
   outprf(usrnum);
   return(1);
} */

/***************************************************************************
*                                                                          *
*     Erotica Player Simple Commands                                       *
*                                                                          *
***************************************************************************/


SHORT ertapp(VOID)
{
   prfmsg(WLDAP1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDAP2);
   outprf(usrnum);
   return(0);
}

SHORT ertgig(VOID)
{
   prfmsg(WLDGI1, ertply->charac);
   ert_room_out();
   prfmsg(WLDGI2);
   outprf(usrnum);
   return(0);
}

SHORT ertjmp(VOID)
{
   prfmsg(WLDJM1, ertply->charac);
   ert_room_out();
   prfmsg(WLDJM2);
   outprf(usrnum);
   return(0);
}

SHORT ertlgh(VOID)
{
   prfmsg(WLDLG1, ertply->charac);
   ert_room_out();
   prfmsg(WLDLG2);
   outprf(usrnum);
   return(0);
}

SHORT ertshg(VOID)
{
   prfmsg(WLDSH1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDSH2);
   outprf(usrnum);
   return(0);
}

SHORT ertlie(VOID)
{
   prfmsg(WLDLI1, ertply->charac);
   ert_room_out();
   prfmsg(WLDLI2);
   outprf(usrnum);
   return(0);
}

SHORT ertwhe(VOID)
{
   prfmsg(WLDWH1, ertply->charac);
   ert_room_out();
   prfmsg(WLDWH2);
   outprf(usrnum);
   return(0);
}

SHORT ertsit(VOID)
{
   prfmsg(WLDSI1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSI2);
   outprf(usrnum);
   return(0);
}

SHORT ertgrv(VOID)
{
   prfmsg(WLDGRV1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDGRV2);
   outprf(usrnum);
   return(0);
}

SHORT ertsnr(VOID)
{
   prfmsg(WLDSNR1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSNR2);
   outprf(usrnum);
   return(0);
}

SHORT ertcac(VOID)
{
   prfmsg(WLDCAC1, ertply->charac);
   ert_room_out();
   prfmsg(WLDCAC2);
   outprf(usrnum);
   return(0);
}

SHORT ertswe(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDSWE1, ertply->charac, ertply->hisher);
      ert_room_out();
      prfmsg(WLDSWE2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertcho(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDCHO1, ertply->charac);
      ert_room_out();
      prfmsg(WLDCHO2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertbeg(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDBEG1, ertply->charac);
      ert_room_out();
      prfmsg(WLDBEG2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertchr(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDCHR1, ertply->charac);
      ert_room_out();
      prfmsg(WLDCHR2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertblu(VOID)
{
   prfmsg(WLDBLU1, ertply->charac);
   ert_room_out();
   prfmsg(WLDBLU2);
   outprf(usrnum);
   return(0);
}

SHORT ertgas(VOID)
{
   prfmsg(WLDGAS1, ertply->charac);
   ert_room_out();
   prfmsg(WLDGAS2);
   outprf(usrnum);
   return(0);
}

SHORT ertcow(VOID)
{
   prfmsg(WLDCOW1, ertply->charac);
   ert_room_out();
   prfmsg(WLDCOW2);
   outprf(usrnum);
   return(0);
}

SHORT ertbow(VOID)
{
   prfmsg(WLDBOW1, ertply->charac);
   ert_room_out();
   prfmsg(WLDBOW2);
   outprf(usrnum);
   return(0);
}

SHORT ertsmk(VOID)
{
   prfmsg(WLDSMK1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDSMK2);
   outprf(usrnum);
   return(0);
}

SHORT ertchv(VOID)
{
   prfmsg(WLDCHV1, ertply->charac);
   ert_room_out();
   prfmsg(WLDCHV2);
   outprf(usrnum);
   return(0);
}

SHORT ertboo(VOID)
{
   prfmsg(WLDBOO1, ertply->charac);
   ert_room_out();
   prfmsg(WLDBOO2);
   outprf(usrnum);
   return(0);
}

SHORT erthis(VOID)
{
   prfmsg(WLDHIS1, ertply->charac);
   ert_room_out();
   prfmsg(WLDHIS2);
   outprf(usrnum);
   return(0);
}

SHORT ertfrn(VOID)
{
   prfmsg(WLDFRN1, ertply->charac);
   ert_room_out();
   prfmsg(WLDFRN2);
   outprf(usrnum);
   return(0);
}

SHORT ertsnl(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDSNL1, ertply->charac);
      ert_room_out();
      prfmsg(WLDSNL2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertpot(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDPOT1, ertply->charac);
      ert_room_out();
      prfmsg(WLDPOT2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertgrw(VOID)
{
   prfmsg(WLDGRW1, ertply->charac);
   ert_room_out();
   prfmsg(WLDGRW2);
   outprf(usrnum);
   return(0);
}

SHORT ertgru(VOID)
{
   prfmsg(WLDGRU1, ertply->charac);
   ert_room_out();
   prfmsg(WLDGRU2);
   outprf(usrnum);
   return(0);
}

SHORT ertsml(VOID)
{
   prfmsg(WLDSML1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSML2);
   outprf(usrnum);
   return(0);
}

SHORT ertgrn(VOID)
{
   prfmsg(WLDGRN1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDGRN2);
   outprf(usrnum);
   return(0);
}

SHORT ertdan(VOID)
{
   prfmsg(WLDDAN1, ertply->charac);
   ert_room_out();
   prfmsg(WLDDAN2);
   outprf(usrnum);
   return(0);
}

SHORT ertsih(VOID)
{
   prfmsg(WLDSIH1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSIH2);
   outprf(usrnum);
   return(0);
}

SHORT ertsin(VOID)
{
   prfmsg(WLDSIN1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSIN2);
   outprf(usrnum);
   return(0);
}

SHORT ertsnz(VOID)
{
   prfmsg(WLDSNZ1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSNZ2);
   outprf(usrnum);
   return(0);
}

SHORT ertwhm(VOID)
{
   prfmsg(WLDWHM1, ertply->charac);
   ert_room_out();
   prfmsg(WLDWHM2);
   outprf(usrnum);
   return(0);
}

SHORT ertsmo(VOID)
{
   prfmsg(WLDSMO1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDSMO2);
   outprf(usrnum);
   return(0);
}

SHORT ertspi(VOID)
{
   prfmsg(WLDSPI1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSPI2);
   outprf(usrnum);
   return(0);
}

SHORT ertsnk(VOID)
{
   prfmsg(WLDSNK1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDSNK2);
   outprf(usrnum);
   return(0);
}

SHORT ertgon(VOID)
{
   prfmsg(WLDGON1, ertply->charac);
   ert_room_out();
   prfmsg(WLDGON2);
   outprf(usrnum);
   return(0);
}

SHORT ertmon(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDMON1, ertply->charac);
      ert_room_out();
      prfmsg(WLDMON2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertbur(VOID)
{
   prfmsg(WLDBUR1, ertply->charac);
   ert_room_out();
   prfmsg(WLDBUR2);
   outprf(usrnum);
   return(0);
}

SHORT ertfar(VOID)
{
   prfmsg(WLDFAR1, ertply->charac);
   ert_room_out();
   prfmsg(WLDFAR2);
   outprf(usrnum);
   return(0);
}

SHORT ertyaw(VOID)
{
   prfmsg(WLDYAW1, ertply->charac);
   ert_room_out();
   prfmsg(WLDYAW2);
   outprf(usrnum);
   return(0);
}

SHORT ertnod(VOID)
{
   prfmsg(WLDNOD1, ertply->charac);
   ert_room_out();
   prfmsg(WLDNOD2);
   outprf(usrnum);
   return(0);
}

SHORT ertgam(VOID)
{
   prfmsg(WLDGAM1, ertply->charac);
   ert_room_out();
   prfmsg(WLDGAM2);
   outprf(usrnum);
   return(0);
}

SHORT erttrd(VOID)
{
   prfmsg(WLDTRD1, ertply->charac);
   ert_room_out();
   prfmsg(WLDTRD2);
   outprf(usrnum);
   return(0);
}

SHORT ertpra(VOID)
{
   prfmsg(WLDPRA1, ertply->charac);
   ert_room_out();
   prfmsg(WLDPRA2);
   outprf(usrnum);
    return(0);
}

SHORT ertany(VOID)
{
   prfmsg(WLDANY1, ertply->charac);
   ert_room_out();
   prfmsg(WLDANY2);
   outprf(usrnum);
   return(0);
}

SHORT ertsic(VOID)
{
   prfmsg(WLDSIC1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSIC2);
   outprf(usrnum);
   return(0);
}

SHORT ertstm(VOID)
{
   prfmsg(WLDSTM1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSTM2);
   outprf(usrnum);
   return(0);
}

SHORT ertcry(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDCRY1, ertply->charac);
      ert_room_out();
      prfmsg(WLDCRY2);
      outprf(usrnum);
   }
   else
   {
      ertyell();
   }
   return(0);
}

SHORT ertcou(VOID)
{
   prfmsg(WLDCOU1, ertply->charac);
   ert_room_out();
   prfmsg(WLDCOU2);
   outprf(usrnum);
   return(0);
}

SHORT ertchu(VOID)
{
   prfmsg(WLDCHU1, ertply->charac,ertply->hisher);
   ert_room_out();
   prfmsg(WLDCHU2);
   outprf(usrnum);
   return(0);
}

SHORT ertwis(VOID)
{
   prfmsg(WLDWIS1, ertply->charac);
   ert_room_out();
   prfmsg(WLDWIS2);
   outprf(usrnum);
   return(0);
}

SHORT erthum(VOID)
{
   prfmsg(WLDHUM1, ertply->charac);
   ert_room_out();
   prfmsg(WLDHUM2);
   outprf(usrnum);
   return(0);
}

SHORT ertblk(VOID)
{
   prfmsg(WLDBLK1, ertply->charac, ertply->hisher);
   ert_room_out();
   prfmsg(WLDBLK2);
   outprf(usrnum);
   return(0);
}

SHORT ertsnf(VOID)
{
   if (margc == 1)
   {
      prfmsg(WLDSNF1, ertply->charac);
      ert_room_out();
      prfmsg(WLDSNF2);
      outprf(usrnum);
   }
   else
   {
      ertsay();
   }
   return(0);
}

SHORT ertsno(VOID)
{
   prfmsg(WLDSNO1, ertply->charac);
   ert_room_out();
   prfmsg(WLDSNO2);
   outprf(usrnum);
   return(0);
}

SHORT ertscore(VOID)
{
   prfmsg(PPTS, spr("%ld", ertply->points));
   outprf(usrnum);
   return(0);
}

SHORT ertchange(VOID)
{
   prfmsg(CHNGPRF);
   outprf(usrnum);
   usrptr->substt=13;
   return(0);
}

SHORT ertmap(VOID)
{
   if (ertply->ertroom->room_num <= 57 && ertply->ertroom->room_num >= 52)
      prfmsg(MAP2);
   else
   if (ertply->ertroom->room_num <= 76 && ertply->ertroom->room_num >= 58)
      prfmsg(MAP3);
   else
      prfmsg(MAP);
   outprf(usrnum);
   usrptr->substt = 14;
   return(0);
}

SHORT erthel(VOID)
{
   if (margc < 2)
   {
      prfmsg(HELP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"arouse"))
   {
      prfmsg(ARSHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"bank"))
   {
      prfmsg(BANKHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"casino"))
   {
      prfmsg(HLPCSNO);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"commands"))
   {
      prfmsg(CMDHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"diseases"))
   {
      prfmsg(DISEHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"info"))
   {
      prfmsg(INFHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"library"))
   {
      prfmsg(LIBHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"list"))
   {
      prfmsg(LSTHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1],"player"))
   {
      prfmsg(PLYHLP);
      outprf(usrnum);
   }
   else if (sameas(margv[1], "shops"))
   {
      prfmsg(HLPSHPS);
      outprf(usrnum);
   }
   else
   {
      prfmsg(HELP);
      outprf(usrnum);
   }
   return(0);
}

SHORT ertbrf(VOID)
{
   ertply->flags|=BRIEF;
   prfmsg(WLDBRF);
   outprf(usrnum);
   return(0);
}

SHORT ertlon(VOID)
{
   ertply->flags&=~BRIEF;
   prfmsg(WLDLON);
   outprf(usrnum);
   return(0);
}

SHORT ertdollars(VOID)
{
   prfmsg(GOLD1, ertply->dollars);
   outprf(usrnum);
   prfmsg(GOLD2, ertply->charac, ertply->hisher);
   ert_room_out();
   return(0);
}

SHORT ertstats(VOID)
{
   CHAR temp[7];

   strcpy(temp, usaptr->sex == 'M' ? "male" : "female");
   prfmsg(STATS, ertply->charac, ertply->plyr_pref, temp);
   if (ertply->has_disease == FALSE)
      prfmsg(DEFPTS, ertply->hits, ertply->maxhit);
   else
      prfmsg(DEFPTS2, ertply->hits, ertply->maxhit);
   prfmsg(MONEY, ertply->dollars);
   outprf(usrnum);
   prfmsg(PHYSCL, ertply->charac, ertply->hisher);
   ert_room_out();
   return(0);
}

/***************************************************************************
*                                                                          *
*     Erotica Room Specific Routines and Sysop Functions                   *
*                                                                          *
***************************************************************************/

SHORT ertcoj (VOID)                           /*  Summon Item (SysOp)       */
{
   SHORT i;

   if (margc == 2)
   {
      if (ertply->nitems < MAXINV) {
	 for (i=0 ; i < NMITEM ; i++)
	 {
	    if (sameto(margv[1],ertitems[i].name))
	    {
	       ert_put_ply(ertply,&ertitems[i]);
	       ertcheck_item(ertitems[i].name);
	       prfmsg(TAKEN);
	       outprf(usrnum);
	       return(0);
	    }
	 }
	 prfmsg(COJ1);
	 outprf(usrnum);
	 return(0);
      }
      prfmsg(COJ2);
      outprf(usrnum);
      return(0);
   }
   prfmsg(COJ3);
   outprf(usrnum);
   return(0);
}

SHORT ertgodspk(VOID)
{
   ert_say_delay();
   prfmsg(GODSPK1);
   outprf(usrnum);
   prfmsg(GODSPEAK);
   prfmsg(GODSPK2,margv[1]);
   ert_prnt_all();
   return(0);
}

SHORT ertquake(VOID)
{
   prfmsg(QUAKE);
   outprf(usrnum);
   prfmsg(EARTHQ);
   ert_prnt_all();
   return(0);
}

SHORT ertbrodcst(VOID)
{
   ert_say_delay();
   prfmsg(BCAST1);
   outprf(usrnum);
   prfmsg(BCAST2,margv[1]);
   ert_prnt_all();
   return(0);
}

SHORT ertfairy(VOID)
{
   if ((ertoth = plyr_ert(margv[1])) != NULL)
   {
      rstrin();
      prfmsg(FAIRYMSG,margv[2]);
      outprf(ertoth->channel);
      prfmsg(FAIRY1);
      outprf(usrnum);
    }
    else
    {
       prfmsg(FAIRY2,ert_nam(margv[1]));
       outprf(usrnum);
    }
    return(0);
}

SHORT ertvision(VOID)
{
   if ((ertoth = plyr_ert(margv[1])) != NULL)
   {
      rstrin();
      prfmsg(VISOMSG,margv[2]);
      outprf(ertoth->channel);
      prfmsg(VISION1);
      outprf(usrnum);
    }
    else
    {
       prfmsg(VISION2,ert_nam(margv[1]));
       outprf(usrnum);
    }
    return(0);
}

SHORT ert_null(VOID)
{
     return(1);
}

SHORT ert_exmn(VOID)
{
   if (sameas(margv[0],"look") || sameas(margv[0],"examine")
       || sameas(margv[0],"see") || sameas(margv[0],"inspect"))
       {
	  return(1);
       }
       else
       {
	  return(0);
       }
}

/***************************************************************************
*                                                                          *
*     Erotica Command List and Handler                                     *
*                                                                          *
****************************************************************************/

SHORT ert_rub(VOID)
{
     struct item *itmptr;

     ert_del_the();
     if (margc < 2)
     {
	  prfmsg(RUB1);
	  outprf(usrnum);
	  prfmsg(RUB2,ertply->charac);
	  ert_room_out();
     }
     else if ((itmptr=ert_get_ply(ertply,margv[1])) == NULL)
     {
	  prfmsg(RUB3,ert_aoran(margv[1]));
	  outprf(usrnum);
	  prfmsg(RUB4,ertply->charac);
	  ert_room_out();
     }
     else
     {
	  ert_put_ply(ertply,itmptr);
	  if (!(itmptr->flags&RUBABL))
	  {
	       ertcute();
	  }
	  else
	  {
               (*(IVOIDFUNC)(itmptr->rubcmd))();
	  }
     }
     return(0);
}

SHORT ert_thk(VOID)
{
     struct item *itmptr;

     ert_del_the();
     ert_del_prep();
     if (margc < 2)
     {
	  prfmsg(THNK1,ert_nam(margv[0]));
	  outprf(usrnum);
	  prfmsg(THNK2,ertply->charac,margv[0]);
	  ert_room_out();
     }
     else if ((itmptr=ert_loc_itm(margv[1])) == NULL)
     {
	  prfmsg(THNK3);
	  outprf(usrnum);
	  prfmsg(THNK4,ertply->charac);
	  ert_room_out();
     }
     else if (!(itmptr->flags&THKABL))
     {
	  prfmsg(THNK5,itmptr->name);
	  outprf(usrnum);
	  prfmsg(THNK6,ertply->charac);
	  ert_room_out();
     }
     else
     {
          (*(IVOIDFUNC)(itmptr->thkcmd))();
     }
     return(0);
}

/***************************************************************************
*                                                                          *
*     Erotica Spell Utilities and Timer Routines                           *
*                                                                          *
***************************************************************************/

VOID ert_timed(VOID)
{
     SHORT i,j;

     setmbk(ertmsg);

     if (ert_evnt4 < 1)
     {
	ert_evnt4++;
     }
     else
     {
	  ert_evnt4=0;
     }
     if (ert_evnt3 < 67)
     {
	  ert_evnt3++;
     }
     else
     {
	ert_evnt3=0;
     }
     if (ert_evnt2 < 15)
     {
	ert_evnt2++;
     }
     else
     {
	ert_evnt2 = 0;
	/* Put an event no. 2 in here */
     }
     if (ert_evnt1 < item_counter)
     {
	ert_evnt1++;
     }
     else
     {
	  ert_evnt1 = 0;
	  i = (SHORT)ert_rnd_num(NROOMS-1, 1);
	  j = (SHORT)ert_rnd_num(6, 1);
	  if (ertroom_list[i].nitems < 1)
	  {
	     ertroom_list[i].iitems[ertroom_list[i].nitems++] = (CHAR)j;  //rick 4/18/23 - VS warning C4244
	  }
     }
     rtkick(10, ert_timed);
}

/***************************************************************************
*                                                                          *
*     Erotica Player Interaction Commands                                  *
*                                                                          *
***************************************************************************/

SHORT ertsay(VOID)
{
     if (margc == 1)
     {
	  prfmsg(UNSURE,ertply->charac);
	  ert_room_out();
	  ert_xmit(NTG2SAY);
     }
     else if (pfnlvl >= 2)
     {
	  prfmsg(SHADES,ertply->charac);
	  ert_room_out();
	  ert_xmit(HEYKCL);
     }
     else
     {
	  ert_say_delay();
	  prfmsg(SAY1,ertply->charac,margv[0],margv[1]);
	  ert_room_out();
	  prfmsg(NEARBY);
	  ert_out_near();
	  ert_xmit(NOSAYB);
     }
     return(0);
}

SHORT ertwhs(VOID)
{

     if (margc < 3)
     {
	  prfmsg(LKSTRA,ertply->charac);
	  ert_room_out();
	  prfmsg(WHISPER);
	  outprf(usrnum);
     }
     else if (pfnlvl >= 2)
     {
	  prfmsg(BNASTY,ertply->charac);
	  ert_room_out();
	  ert_xmit(UNOTNI);
     }
     else
     {
	  if ((ertoth=ertfplyr(margv[2])) != NULL)
	  {
	       ert_whspr();
	       prfmsg(WHIS2U,ertply->charac,margv[3]);
	       outprf(ertoth->channel);
	       prfmsg(HEARSU,ertoth->charac);
	       outprf(usrnum);
	       prfmsg(WHIS2S,ertply->charac,ertoth->charac);
	       ert_out_evnt(ertoth);
	  }
	  else {
	       prfmsg(LKEXAS,ertply->charac);
	       ert_room_out();
	       prfmsg(SNOTHR,ert_nam(margv[2]));
	       outprf(usrnum);
	  }
     }
     return(0);
}

SHORT ertyell(VOID)
{
     CHAR *yelptr;

     if (margc == 1)
     {
	  prfmsg(SINGLD,ertply->charac,margv[0]);
	  ert_room_out();
	  prfmsg(SINGNR,margv[0]);
	  ert_out_near();
/*	  prfmsg(SINGDS,margv[0]);
	  ert_outfar();*/
	  ert_xmit(ULOUDE);
     }
     else if (pfnlvl >= 2)
     {
	  prfmsg(BRUDEV,ertply->charac);
	  ert_room_out();
	  ert_xmit(YELL1);
     }
     else
     {
	  ert_say_delay();
	  for (yelptr=margv[1] ; *yelptr != '\0' ; yelptr++)
	  {
	       *yelptr=(CHAR)toupper(*yelptr);
	  }
	  prfmsg(YELL2,ertply->charac,margv[0],margv[1]);
	  ert_room_out();
	  prfmsg(YELL3,margv[0],margv[1]);
	  ert_out_near();
/*	  prfmsg(SINGDS,margv[0]);
	  ert_outfar();*/
	  ert_xmit(ULOUDV);
     }
     return(0);
}

SHORT ertmov(VOID)
{
     if (margc < 2)
     {
	ert_xmit(ENJSLF);
	prfmsg(DAYDRM,ertply->charac);
	ert_room_out();
     }
     else if (sameas(margv[1],"n") || sameas(margv[1],"north"))
     {
	ert_north();
     }
     else if (sameas(margv[1],"s") || sameas(margv[1],"south"))
     {
	ert_south();
     }
     else if (sameas(margv[1],"e") || sameas(margv[1],"east"))
     {
	ert_east();
     }
     else if (sameas(margv[1],"w") || sameas(margv[1],"west"))
     {
	ert_west();
     }
     else if (sameas(margv[1],"u") || sameas(margv[1],"up"))
     {
	ertup();
     }
     else if (sameas(margv[1],"d") || sameas(margv[1],"down"))
     {
	ertdown();
     }
     else
     {
	ert_xmit(CANTDU);
	prfmsg(PONDPS,ertply->charac);
	ert_room_out();
     }
     return(0);
}

SHORT ert_mean(VOID)
{
     ert_xmit(STOPCN);
     prfmsg(ACRUDE,ertply->charac);
     ert_room_out();
     return(0);
}

CHAR_BUFFER(LOOKINGWISHFUL, 80, "***\r%s is looking wishful!\r");
CHAR_BUFFER(SURETHEENJOYING, 80, "...I'm sure the %s is enjoying it!\r");
CHAR_BUFFER(SUREYOURENJOYING, 80, "...I'm sure your %s is enjoying it!\r");
CHAR_BUFFER(NOWISNTTHATCUTE, 80, "...Now, isn't that cute!\r");
CHAR_BUFFER(LONGINGFORFRIEND, 80, "***\r%s is longing for a friend!\r");

SHORT ertcute(VOID)
{
    ertgut(LOOKINGWISHFUL, SURETHEENJOYING, SUREYOURENJOYING, NOWISNTTHATCUTE, LONGINGFORFRIEND);
    return(0);
}

CHAR_BUFFER(HASNASTYLOOK, 80, "***\r%s has a nasty look!\r");
CHAR_BUFFER(WHYWHATDIDDO, 80, "...Why, what did the %s do to you?\r");
CHAR_BUFFER(ALITTLEWEIRD, 80, "...A little weird, no?\r");
CHAR_BUFFER(NOWWHYDOTHAT, 80, "...Now why would you want to do that?\r");
CHAR_BUFFER(SAVAGELYLOOK, 80, "***\r%s is savagely looking around!\r");

SHORT ertbemean(VOID)
{
  ertgut(HASNASTYLOOK, WHYWHATDIDDO, ALITTLEWEIRD, NOWWHYDOTHAT, SAVAGELYLOOK);
  return(0);
}

SHORT ertgut(CHAR *stg1, CHAR *stg2, CHAR *stg3, CHAR *stg4, CHAR *stg5)
{
   struct item *itmptr;

   ert_del_the();
   ert_del_prep();
   if (margc == 1)
   {
      prfmsg(SWHOWT,ert_nam(margv[0]),ert_nam(margv[0]));
      outprf(usrnum);
      prf(stg1,ertply->charac);
      ert_room_out();
   }
   else if ((itmptr=ert_get_itm(ertply->ertroom,margv[1])) != NULL)
   {
      ert_itm_rm(ertply->ertroom,itmptr);
      prfmsg(ATTSTS,ertply->charac,margv[0],itmptr->name);
      ert_room_out();
      prf(stg2,itmptr->name);
      outprf(usrnum);
   }
   else if ((itmptr=ert_get_ply(ertply,margv[1])) != NULL)
   {
      ert_put_ply(ertply,itmptr);
      prfmsg(ATTSSS,ertply->charac,margv[0],ertply->hisher,itmptr->name);
      ert_room_out();
      prf(stg3,itmptr->name);
      outprf(usrnum);
   }
   else if ((ertoth=ertfplyr(margv[1])) != NULL)
   {
      if (sameas(margv[0],"kiss") || sameas(margv[0],"hug"))
      {
	 prfmsg(GIVUAS,ertply->charac,margv[0]);
	 outprf(ertoth->channel);
	 prfmsg(GIVSAS,ertply->charac,ertoth->charac,margv[0]);
	 ert_out_evnt(ertoth);
	 ert_xmit(AWWWWW);
      }
      else
      {
	 prfmsg(BESTSU,ertply->charac,ertply->hisher,margv[0]);
	 outprf(ertoth->channel);
	 prfmsg(BESTSS,
	 ertply->charac,ertply->hisher,margv[0],ertoth->charac);
	 ert_out_evnt(ertoth);
	 btuxmt(usrnum,stg4);
       }
   }
   else if (margc >= 2 && sameas(margv[1],"presence")
	    || sameas(margv[1],"invisible") || sameas(margv[1],"unseen")
	    || sameas(margv[1],"mystical") || sameas(margv[1],"magical"))
   {
      ert_xmit(FRCNTN);
      prfmsg(CLUTCH,ertply->charac);
      ert_room_out();
   }
   else
   {
      prf(stg5,ertply->charac);
      ert_room_out();
      prfmsg(SNOTHR,ert_nam(margv[1]));
      outprf(usrnum);
   }
   return(0);
}

SHORT ertgiv(VOID)
{
   ert_del_the();
   if (margc == 3)
   {
      ertgivutl(margv[1],margv[2]);
   }
   else if (margc == 4 && sameas(margv[2],"to"))
   {
      ertgivutl(margv[3],margv[1]);
   }
   else
   {
      ert_xmit(GIVMEB);
      prfmsg(FUMBLE,ertply->charac);
      ert_room_out();
   }
   return(0);
}

SHORT ertgivutl(CHAR *givee,CHAR *givwhat)
{
   struct item *itmptr,*itmpt2;

   if ((ertoth=ertfplyr(givee)) == NULL)
   {
      prfmsg(SNOTHR,ert_nam(givee));
      outprf(usrnum);
      prfmsg(SEETHN,ertply->charac);
      ert_room_out();
   }
   else if ((itmptr=ert_get_ply(ertply,givwhat)) == NULL)
   {
      prfmsg(ARENTH,ert_aoran(givwhat));
      outprf(usrnum);
      prfmsg(LK4SOM,ertply->charac);
      ert_room_out();
   }
   else if (ertoth->nitems >= MAXINV)
   {
      if (ertply->ertroom->nitems >= MAXINV)
      {
	 ert_put_ply(ertply,itmptr);
	 ert_xmit(NICTHT);
	 prfmsg(WRSMYS,ertply->charac);
	 ert_room_out();
      }
      else if (ert_rnd()&1)
      {
	 ert_itm_rm(ertply->ertroom,itmptr);
	 ertcheck_item2(itmptr->name);
	 prfmsg(DROPSS,ertply->charac,ertply->hisher,itmptr->name);
	 ert_room_out();
	 ert_xmit(OOOOPS);
      }
      else
      {
	 itmpt2=ert_get_ply(ertoth,ertoth->items[0]->name);
	 ertcheck_item4(itmpt2->name, givee);
	 ert_itm_rm(ertply->ertroom,itmpt2);
	 ert_put_ply(ertoth,itmptr);
	 ertcheck_item3(itmptr->name, givee);
	 prfmsg(GIVUDP,ertply->charac,ert_aoran(itmptr->name),
		itmpt2->name);
	 outprf(ertoth->channel);
	 prfmsg(GIVSDP,ertply->charac,ertoth->charac,ert_aoran(itmptr->name),
		ertoth->charac,ertoth->hisher,itmpt2->name);
		ert_out_evnt(ertoth);
	 prfmsg(UMADED,ertoth->charac,ertoth->hisher,itmpt2->name);
	 outprf(usrnum);
      }
   }
   else
   {
      ert_put_ply(ertoth,itmptr);
      ertcheck_item3(itmptr->name, givee);
      prfmsg(GIVEUS,ertply->charac,ert_aoran(itmptr->name));
      outprf(ertoth->channel);
      prfmsg(GIVESS,ertply->charac,ertoth->charac,ert_aoran(itmptr->name));
      ert_out_evnt(ertoth);
      ert_xmit(CONSDN);
   }
   return(0);
}

SHORT ertgetrou(VOID)
{
     ert_del_the();
     if (margc == 2)
     {
	ertgetrut(margv[1]);
     }
     else
     if (margc == 3 && sameas(margn[1]-2,"'s"))
     {
	*(margn[1]-2)='\0';
	ertgetput(margv[1],margv[2]);
     }
     else if (margc == 4 && sameto(margv[2],"from"))
     {
	ertgetput(margv[3],margv[1]);
     }
     else
     {
	ert_xmit(INTCNC);
	prfmsg(SFREAK,ertply->charac);
	ert_room_out();
     }
     return(0);
}

SHORT ertgetput(CHAR *who,CHAR *getwhat)
{
     struct item *itmptr;
     struct item *itmptr2;
     int temp;

     /* Player isn't even here*/
     if ((ertoth=ertfplyr(who)) == NULL)
     {
	prfmsg(SNOTHR,ert_nam(who));
	outprf(usrnum);
	prfmsg(SSEEIN,ertply->charac);
	ert_room_out();
     }
     /* Player isn't holding that item */
     else if ((itmptr=ert_get_ply(ertoth,getwhat)) == NULL)
     {
	prfmsg(SNOTHL,ertoth->charac,ert_aoran(getwhat));
	outprf(usrnum);
	prfmsg(LKSUSP,ertply->charac);
	ert_room_out();
     }
     /* You are already holding too much */
     else if (ertply->nitems >= MAXINV)
     {
	ert_put_ply(ertoth,itmptr);
	ert_xmit(HNDFUL);
	prfmsg(FUMBIT,ertply->charac,ertply->hisher);
	ert_room_out();
     }
     else if ((ert_rnd()&0x0E) != 0)
     {
	ert_put_ply(ertoth, itmptr);
	prfmsg(STEALU, ertply->charac, itmptr->name);
	outprf(ertoth->channel);
	prfmsg(STEALS, ertply->charac, ertoth->charac, itmptr->name);
	ert_out_evnt(ertoth);
	ert_xmit(UATTFL);
     }
     else
     if ((itmptr2 = ert_get_ply(ertoth, "dart-gun")) == NULL)
     {
	ertcheck_item(itmptr->name);
	ertcheck_item4(itmptr->name, who);
	ert_put_ply(ertply, itmptr);
	prfmsg(STOLEU, ertply->charac, itmptr->name);
	outprf(ertoth->channel);
	prfmsg(STOLES, ertply->charac, ertoth->charac, itmptr->name);
	ert_out_evnt(ertoth);
	ert_xmit(UGOTIT);
     }
     else
     {
	ert_put_ply(ertoth, itmptr2);
	ert_put_ply(ertoth, itmptr);
	prfmsg(YANAIL, ertply->charac);
	outprf(ertoth->channel);
	prfmsg(GOTNAIL, ertoth->charac, ertoth->hisher);
	outprf(usrnum);
	prfmsg(SMACK, ertoth->charac, ertply->charac, ertoth->hisher);
	ert_out_evnt(ertoth);
	if (ertply->hits > 2)
	   ertply->hits = ertply->hits / 2;
	temp = ert_rnd_num(NROOMS-1,1);
	ertleave_room(ertply,getmsg(ZAPP));
	ertgo_room(ertply,&ertroom_list[temp], getmsg(BORNAG));
     }
     return(0);
}

SHORT ertgetrut(CHAR *getwhat)
{
     struct item *itmptr;

     if ((itmptr=ert_get_itm(ertply->ertroom,getwhat)) == NULL)
     {
	  ert_xmit(WHUSEE);
	  prfmsg(BYHELP,ertply->charac);
	  ert_room_out();
     }
     else if (ertply->nitems >= MAXINV)
     {
	  ert_itm_rm(ertply->ertroom,itmptr);
	  ert_xmit(LILGRD);
	  prfmsg(GGLINT,ertply->charac,ertply->hisher);
	  ert_room_out();
     }
     else
     {
	  ertcheck_item(itmptr->name);
	  ert_put_ply(ertply, itmptr);
	  ert_xmit(ITYOUR);
	  prfmsg(STOOKS, ertply->charac, itmptr->name);
	  ert_room_out();
     }
     return(0);
}

VOID ertcheck_item(CHAR item[20])
{
   if (sameto(item, "handcuff-key"))
      ertply->aura|=HNDCUF;
   else
   if (sameto(item, "sexacillin"))
      ertply->aura|=SEXCLN;
   else
   if (sameto(item, "chastity-belt"))
      ertply->aura|=CHSTYB;
   else
   if (sameto(item, "unsubmissive-potion"))
      ertply->aura|=UNSUBM;
   else
   if (sameto(item, "scissors"))
      ertply->aura|=SCISSR;
   else
   if (sameto(item, "anti-glasses"))
      ertply->aura|=GLASSE;
}

VOID ertcheck_item2(CHAR item[20])
{
   if (sameto(item, "handcuff-key"))
      ertply->aura&=~HNDCUF;
   else
   if (sameto(item, "sexacillin"))
      ertply->aura&=~SEXCLN;
   else
   if (sameto(item, "chastity-belt"))
      ertply->aura&=~CHSTYB;
   else
   if (sameto(item, "unsubmissive-potion"))
      ertply->aura&=~UNSUBM;
   else
   if (sameto(item, "scissors"))
      ertply->aura&=~SCISSR;
   else
   if (sameto(item, "anti-glasses"))
      ertply->aura&=~GLASSE;
}

VOID ertcheck_item3(CHAR item[20], CHAR *who)   /* Give flag to other */
{
   ertoth = ertfplyr(who);

   if (sameto(item, "handcuff-key"))
      ertoth->aura|=HNDCUF;
   else
   if (sameto(item, "sexacillin"))
      ertoth->aura|=SEXCLN;
   else
   if (sameto(item, "chastity-belt"))
      ertoth->aura|=CHSTYB;
   else
   if (sameto(item, "unsubmissive-potion"))
      ertoth->aura|=UNSUBM;
   else
   if (sameto(item, "scissors"))
      ertoth->aura|=SCISSR;
   else
   if (sameto(item, "anti-glasses"))
      ertoth->aura|=GLASSE;
}

VOID ertcheck_item4(CHAR item[20], CHAR *who)   /* Take flag of other */
{
   ertoth = ertfplyr(who);

   if (sameto(item, "handcuff-key"))
      ertoth->aura&=~HNDCUF;
   else
   if (sameto(item, "sexacillin"))
      ertoth->aura&=~SEXCLN;
   else
   if (sameto(item, "chastity-belt"))
      ertoth->aura&=~CHSTYB;
   else
   if (sameto(item, "unsubmissive-potion"))
      ertoth->aura&=~UNSUBM;
   else
   if (sameto(item, "scissors"))
      ertoth->aura&=~SCISSR;
   else
   if (sameto(item, "anti-glasses"))
      ertoth->aura&=~GLASSE;
}

SHORT ert_north(VOID)
{
     if (ertply->ertroom->inorth > 32767)
     {
	prfmsg(ENTKEY);
	outprf(usrnum);
	ertply->keyroom = ertply->ertroom->iwest - 32767;
	usrptr->substt=10;
     }
     else
     if (ertply->ertroom->inorth != 255)
     {
	ertleave_room(ertply,getmsg(LNORTH));
	ertgo_room(ertply,&ertroom_list[ertply->ertroom->inorth],getmsg(ESOUTH));
     }
     else
     {
	prfmsg(BLUNDR,ertply->charac);
	ert_room_out();
	ert_xmit(NONORT);
     }
     return(0);
}

SHORT ert_south(VOID)
{
     if (ertply->ertroom->isouth > 32767)
     {
	prfmsg(ENTKEY);
	outprf(usrnum);
	ertply->keyroom = ertply->ertroom->iwest - 32767;
	usrptr->substt=10;
     }
     else
     if (ertply->ertroom->isouth != 255)
     {
	ertleave_room(ertply,getmsg(LSOUTH));
	ertgo_room(ertply,&ertroom_list[ertply->ertroom->isouth],getmsg(ENORTH));
     }
     else
     {
	prfmsg(BLUNDR,ertply->charac);
	ert_room_out();
	ert_xmit(NOSOUT);
     }
     return(0);
}

SHORT ert_east(VOID)
{
     if (ertply->ertroom->ieast > 32767)
     {
	prfmsg(ENTKEY);
	outprf(usrnum);
	ertply->keyroom = ertply->ertroom->iwest - 32767;
	usrptr->substt=10;
     }
     else
     if (ertply->ertroom->ieast != 255)
     {
	ertleave_room(ertply,getmsg(LEAST));
	ertgo_room(ertply,&ertroom_list[ertply->ertroom->ieast],getmsg(EWEST));
     }
     else
     {
	prfmsg(BLUNDR,ertply->charac);
	ert_room_out();
	ert_xmit(NOEAST);
     }
     return(0);
}

SHORT ert_west(VOID)
{
     if (ertply->ertroom->iwest > 32767)
     {
	prfmsg(ENTKEY);
	outprf(usrnum);
	ertply->keyroom = ertply->ertroom->iwest - 32767;
	usrptr->substt=10;
     }
     else
     if (ertply->ertroom->iwest != 255)
     {
	ertleave_room(ertply,getmsg(LWEST));
	ertgo_room(ertply,&ertroom_list[ertply->ertroom->iwest],getmsg(EEAST));
     }
     else
     {
	prfmsg(BLUNDR,ertply->charac);
	ert_room_out();
	ert_xmit(NOWEST);
     }
     return(0);
}

SHORT ertup(VOID)
{
   if (ertply->ertroom->iup > 32767)
   {
      prfmsg(ENTKEY);
      outprf(usrnum);
      ertply->keyroom = ertply->ertroom->iwest - 32767;
      usrptr->substt=10;
   }
   else
   if (ertply->ertroom->iup != 255)
   {
      ertleave_room(ertply, getmsg(WENTUP));
      ertgo_room(ertply,&ertroom_list[ertply->ertroom->iup], getmsg(CAMEUP));
   }
   else
   {
      prfmsg(BLUNDR, ertply->charac);
      ert_room_out();
      ert_xmit(NOUP);
   }
   return(0);
}

SHORT ertdown(VOID)
{
   if (ertply->ertroom->idown > 32767)
   {
      prfmsg(ENTKEY);
      outprf(usrnum);
      ertply->keyroom = ertply->ertroom->iwest - 32767;
      usrptr->substt=10;
   }
   else
   if (ertply->ertroom->idown != 255)
   {
      ertleave_room(ertply, getmsg(WENTDWN));
      ertgo_room(ertply,&ertroom_list[ertply->ertroom->idown], getmsg(CAMEDWN));
   }
   else
   {
      prfmsg(BLUNDR, ertply->charac);
      ert_room_out();
      ert_xmit(NODOWN);
   }
   return(0);
}

SHORT ertlokrou(VOID)
{
     struct item *itempt;
     struct monster *mnstpt;
     SHORT struct_pos;
     CHAR plyr_pref[9];

     ert_del_the();
     ert_del_prep();
     if (margc >= 2 && (mnstpt=ertfmnstr(ertply->ertroom, margv[1])) != NULL)
     {
	prfmsg(mnstpt->mdesc);
	outprf(usrnum);
     }
     else
     if (margc >= 2 && (itempt=ert_get_itm(ertply->ertroom, margv[1])) != NULL)
     {
	  ert_itm_rm(ertply->ertroom,itempt);
	  prfmsg(itempt->descip);
	  outprf(usrnum);
	  prfmsg(SEXMTS,ertply->charac,itempt->name);
	  ert_room_out();
     }
     else
     if (margc >= 2 && (itempt=ert_get_ply(ertply,margv[1])) != NULL)
     {
	  ert_put_ply(ertply,itempt);
	  prfmsg(itempt->descip);
	  outprf(usrnum);
	  prfmsg(SEXMSS,ertply->charac,ertply->hisher,itempt->name);
	  ert_room_out();
     }
     else
     if (margc >= 2 && (ertoth=ertfplyr(margv[1])) != NULL)
     {
	  prfmsg(ertoth->descrpt, ertoth->charac, ertoth->plyr_pref);
//	  prfmsg(POSESS, ert_nam(ertoth->hisher));
//	  ert_inv_utl(ertoth);
	  outprf(usrnum);
	  prfmsg(LKUCAR, ertply->charac);
	  outprf(ertoth->channel);
	  prfmsg(SEXAMS, ertply->charac, ertoth->charac);
	  ert_out_evnt(ertoth);
     }
     else
     if (margc >= 2 && ((struct_pos = (SHORT)ertfnlv(margv[1])) > FALSE))
     {
	if (ert_inact_lst[struct_pos].pref_num == 1)
	   strcpy(plyr_pref, "Straight");
	else
	if (ert_inact_lst[struct_pos].pref_num == 2)
	{
	   if (ert_inact_lst[struct_pos].aura&MALE)
	      strcpy(plyr_pref, "Gay");
	   else
	      strcpy(plyr_pref, "Lesbian");
	}
	else
	   strcpy(plyr_pref, "Bisexual");
	if (ert_inact_lst[struct_pos].aura&MALE)
	   prfmsg(NORMM, ert_inact_lst[struct_pos].iname, plyr_pref);
	else
	   prfmsg(NORMF, ert_inact_lst[struct_pos].iname, plyr_pref);
	outprf(usrnum);
	prfmsg(SEXAMS, ertply->charac, ert_inact_lst[struct_pos].iname);
	ert_out_evnt(ertoth);
	outprf(usrnum);
     }
     else
     if (margc >= 2 && sameto(margv[1],"brief"))
     {
	  prfmsg(COLOR1);
          prfmsg(BRFDES1,(CHAR *)(ertply->ertroom->brdesc));
	  prfmsg(COLOROFF);
	  ert_see_itms(ertply);
	  ert_see_pyrs(ertply);
	  outprf(usrnum);
	  prfmsg(GLIMPS,ertply->charac);
	  ert_room_out();
     }
     else
     if (margc >= 2 && sameas(margv[1],"presence")
	       || sameas(margv[1],"invisible")
	       || sameas(margv[1],"unseen")
	       || sameas(margv[1],"mystical")
	       || sameas(margv[1],"magical"))
	       {
	       ert_xmit(FRCNVI);
	       prfmsg(STARTA,ertply->charac);
	       ert_room_out();
     }
     else
     {
	  prfmsg(COLOR1);
	  prfmsg(ertply->ertroom->desc);
	  prfmsg(COLOROFF);
	  ertseeexit();
	  ert_see_itms(ertply);
	  ert_see_pyrs(ertply);
	  outprf(usrnum);
	  prfmsg(SCARFI,ertply->charac);
	  ert_room_out();
     }
     return(0);
}

CHAR_BUFFER(THESOUTH, 80, "the south");
CHAR_BUFFER(SOUTH, 80, "south");
CHAR_BUFFER(THENORTH, 80, "the north");
CHAR_BUFFER(NORTH, 80, "north");
CHAR_BUFFER(THEEAST, 80, "the east");
CHAR_BUFFER(EAST, 80, "east");
CHAR_BUFFER(THEWEST, 80, "the west");
CHAR_BUFFER(WEST, 80, "west");

SHORT ertshv(VOID)
{
     ert_del_prep();
     ert_del_the();
     if (margc == 3)
     {
	  if ((ertoth=ertfplyr(margv[1])) != NULL)
	  {
	       if (sameto(margv[2],"north")
		&& ertshvutl(THESOUTH,NORTH,&ertroom_list[ertply->ertroom->inorth],ertply->ertroom->inorth)) {
	       }
	       else if (sameto(margv[2],"south")
		&& ertshvutl(THENORTH,SOUTH,&ertroom_list[ertply->ertroom->isouth],ertply->ertroom->isouth)) {
	       }
	       else if (sameto(margv[2],"east")
		&& ertshvutl(THEWEST,EAST,&ertroom_list[ertply->ertroom->ieast],ertply->ertroom->ieast)) {
	       }
	       else if (sameto(margv[2],"west")
		&& ertshvutl(THEEAST,WEST,&ertroom_list[ertply->ertroom->iwest],ertply->ertroom->iwest)) {
	       }
	       else
	       {
		    prfmsg(HAVHAL,ertply->charac);
		    ert_room_out();
		    prfmsg(NOEXIT,margv[2]);
		    outprf(usrnum);
	       }
	  }
	  else
	  {
	       prfmsg(SSEEIN,ertply->charac);
	       ert_room_out();
	       prfmsg(SNOTHR,margv[1]);
	       outprf(usrnum);
	  }
     }
     else
     {
	  prfmsg(MEDEMR,ertply->charac);
	  ert_room_out();
	  ert_xmit(SEEDOC);
     }
     return(0);
}

SHORT ertshvutl(const char *from, const char *to, struct ertroom *entrp, USHORT room_to)
{
  if ((entrp == NOROOM) || (room_to > 30000))
    return(0);
  else
  {
    prfmsg(CAUGHT,ertoth->charac);
    outprf(usrnum);
    ertleave_room(ertoth,spr("been shoved %s",to));
    prfmsg(SHOVUS,ertply->charac,to);
    outprf(ertoth->channel);
    ertgo_room(ertoth,entrp,spr("been shoved from %s",from));
    return(1);
  }
}

SHORT ertdrprou(VOID)
{
struct item *itmptr;

    ert_del_the();
    ert_del_prep();
    if (margc >= 2)
    {
       if ((itmptr=ert_get_ply(ertply,margv[1])) != NULL)
       {
	  if (ertply->ertroom->nitems >= MXITEM)
	  {
	     ert_put_ply(ertply,itmptr);
	     ert_xmit(FRCPRV);
	     prfmsg(GRAPAF,ertply->charac);
	     ert_room_out();
	  }
	  else
	  {
	     ertcheck_item2(itmptr->name);
	     ert_itm_rm(ertply->ertroom,itmptr);
	     ert_xmit(UDROPT);
	     prfmsg(SDROPS,ertply->charac, ertply->hisher, itmptr->name);
	     ert_room_out();
	  }
       }
       else
       {
	  ert_xmit(NICEID);
	  prfmsg(ACTODD,ertply->charac);
	  ert_room_out();
       }
    }
    else
    {
       ert_xmit(NVRKNU);
       prfmsg(LILQUR,ertply->charac);
       ert_room_out();
    }
    return(0);
}

SHORT ertwnk(VOID)
{
     ert_del_prep();
     ert_del_the();
     if (margc == 1)
     {
	  prfmsg(WINK);
	  outprf(usrnum);
	  prfmsg(WINKGR,ertply->charac);
	  ert_room_out();
     }
     else if ((ertoth=ertfplyr(margv[1])) != NULL)
     {
	  ert_xmit(UWINKS);
	  prfmsg(WINKSL,ertply->charac);
	  outprf(ertoth->channel);
	  prfmsg(WINKSS,ertply->charac,ertoth->charac);
	  ert_out_evnt(ertoth);
     }
     else
     {
	  ert_xmit(UFLBLU);
	  prfmsg(FLBLUE,ertply->charac);
	  ert_room_out();
     }
     return(0);
}

SHORT ertglr(VOID)
{
     ert_del_prep();
     ert_del_the();
     if (margc == 1) {
	  ert_xmit(WHATUA);
	  prfmsg(GLARAN,ertply->charac);
	  ert_room_out();
     }
     else if ((ertoth=ertfplyr(margv[1])) != NULL) {
	  ert_xmit(ULKSOA);
	  prfmsg(GLARUR,ertply->charac);
	  outprf(ertoth->channel);
	  prfmsg(GLARSA,ertply->charac,ertoth->charac);
	  ert_out_evnt(ertoth);
     }
     else {
	  ert_xmit(EYEBRN);
	  prfmsg(AGLINT,ertply->charac);
	  ert_room_out();
     }
     return(0);
}

/***************************************************************************
*                                                                          *
*     Erotica Game Handler Utilities                                       *
*                                                                          *
***************************************************************************/

VOID ert_room_out(VOID)         /* send prf to all others in room */
{
     struct ertplayer *pp;

     for (pp=ertply->ertroom->roomhdr ; pp != NULL ; pp=pp->plink)
     {
	  if (pp != ertply)
	  {
	       outprf(pp->channel);
	  }
     }
     clrprf();
}

VOID ert_pwf(struct ertplayer *l_ertply,LONG auraf)              /*  send prf to others/with flags  */
{
     struct ertplayer *pp;

     for (pp=l_ertply->ertroom->roomhdr ; pp != NULL ; pp=pp->plink)
     {
	  if (pp != l_ertply && (pp->aura&auraf))
	  {
	       outprf(pp->channel);
	  }
     }
     clrprf();
}

VOID ert_out_evnt(struct ertplayer *doplyr)               /*  send prf w/ two exceptions  */
{
     struct ertplayer *pp;

     for (pp=ertply->ertroom->roomhdr ; pp != NULL ; pp=pp->plink) {
	  if (pp != ertply && pp != doplyr) {
	       outprf(pp->channel);
	  }
     }
     clrprf();
}

VOID ert_prnt_rm(struct ertroom *rmptr)                     /*  send prf to all in a room  */
{
     struct ertplayer *pp;

     for (pp=rmptr->roomhdr ; pp != NULL ; pp=pp->plink) {
	  outprf(pp->channel);
     }
     clrprf();
}

VOID ert_prnt_all(VOID)                       /*  send prf to all on game  */
{
     SHORT i;
     struct ertplayer *pp;

     for (i=0,pp=ertplayer ; i < nterms ; i++,pp++) {
	  if (pp->flags&INWLD)
	  {
	       outprf(pp->channel);
	  }
     }
     clrprf();
}

VOID ert_out_near(VOID)                      /*  send prf to nearby rooms */
{
     nrooms=1;
     rooms[0] = ertply->ertroom;
     ert_near_far(ertply->ertroom);
}

VOID ert_outfar(VOID)                   /*  send prf to far rooms */
{                                   /* must call ert_out_near before this!! */
   ert_near_far(&ertroom_list[ertply->ertroom->inorth]);
   ert_near_far(&ertroom_list[ertply->ertroom->isouth]);
   ert_near_far(&ertroom_list[ertply->ertroom->ieast]);
   ert_near_far(&ertroom_list[ertply->ertroom->iwest]);
}

VOID ert_near_far(struct ertroom *plyroom)              /* ert_out_near/ert_outfar utility */
{
   if (plyroom != NOROOM)
   {
      ertoutlist(&ertroom_list[plyroom->inorth], plyroom->inorth);
      ertoutlist(&ertroom_list[plyroom->isouth], plyroom->isouth);
      ertoutlist(&ertroom_list[plyroom->ieast], plyroom->ieast);
      ertoutlist(&ertroom_list[plyroom->iwest], plyroom->iwest);
   }
}

//static 
SHORT ertoutlist(struct ertroom *roompt, USHORT room_to)        /* ert_out_near/ert_outfar utility */
{
     struct ertroom **rmptpt;
     SHORT i;

     if ((roompt == NOROOM) || (room_to > 30000))
     {
	return(0);
     }
     else
     {
	  for (i = 0, rmptpt = rooms ; i < nrooms ; i++, rmptpt++)
	  {
	       if (roompt == *rmptpt)
	       {
		    return(0);
	       }
	  }
	  *rmptpt = roompt;
	  nrooms++;
	  ert_prnt_rm(roompt);
     }
     return(0);
}

LONG ert_see_flg(VOID)
{
     LONG retval;

     retval = SEEPYR;
     return(retval);
}

VOID ertleave_room(struct ertplayer *pp,CHAR *xitdesc)           /*  leave room function   */
{
     struct ertplayer **pyptpt;

     prf("***\r%s has just %s!\r",pp->charac,xitdesc);
     ert_pwf(pp,ert_see_flg());
     for (pyptpt=&(pp->ertroom->roomhdr) ; *pyptpt != pp ; )
     {
	  pyptpt=&((*pyptpt)->plink);
     }
     *pyptpt=pp->plink;
}

VOID ertgo_room(struct ertplayer *pp,struct ertroom *destrm,CHAR *entdesc)         /*  enter room function            */
{
     pp->plink=destrm->roomhdr;
     destrm->roomhdr=pp;
     pp->ertroom=destrm;
     prf("***\r%s has just %s!\r",pp->charac,entdesc);
     ert_pwf(pp,ert_see_flg());
     if (pp->flags&BRIEF)
     {
	prfmsg(COLOR1);
        prfmsg(YOUR,(CHAR *)(pp->ertroom->brdesc));
	prfmsg(COLOROFF);
     }
     else
     {
	prfmsg(COLOR1);
	prfmsg(pp->ertroom->desc);
	prfmsg(COLOROFF);
     }
     ertseeexit();
     ert_see_itms(pp);
     ert_see_pyrs(pp);
     if (ertply->ertroom->ertmnum > 0 && mnstr_list[ertply->ertroom->ertmlist[0]].sayflg == 1)
     {
	if (ert_rnd_num(3,1) == 1)
	   prfmsg(mnstr_list[ertply->ertroom->ertmlist[0]].comment);
     }
     outprf(pp->channel);
     ertply->room_num = ertply->ertroom->room_num;

}

VOID ertseeexit(VOID)
{
   prfmsg(EXITS);
   if (ertply->ertroom->inorth != 255)
      prf(" n");
   if (ertply->ertroom->isouth != 255)
      prf(" s");
   if (ertply->ertroom->ieast != 255)
      prf(" e");
   if (ertply->ertroom->iwest != 255)
      prf(" w");
   if (ertply->ertroom->iup != 255)
      prf(" u");
   if (ertply->ertroom->idown !=255)
      prf(" d");
   prf("\r");
}

VOID ert_see_itms(struct ertplayer *pp)                   /*  see items in room function  */
{
     CHAR pprobl[15];
     CHAR *iitems;
     SHORT nitms=0,i;

     (VOID)nitms;
     if (pp->aura&SEEOBJ)
     {
	  if (pp->ertroom->room_type == 1 || pp->ertroom->room_type == 9 ||
	      pp->ertroom->room_type == 10)
	     strcpy(pprobl, "on the floor");
	  else
	  if (pp->ertroom->room_type == 21)
	     strcpy(pprobl, "in the street");
	  else
	     strcpy(pprobl, "on the ground");
	  iitems=pp->ertroom->iitems;
	  switch (nitms=pp->ertroom->nitems)
	  {
	  case 0:
	       prfmsg(ITMLST1,pprobl);
	       break;
	  case 1:
	       prfmsg(ITMLST2,ert_nxt_itm(iitems[0]),pprobl);
	       break;
	  case 2:
	       prfmsg(ITMLST3,ert_nxt_itm(iitems[0]));
	       prfmsg(ITMLST4,ert_nxt_itm(iitems[1]),pprobl);
	       break;
	  default:
	       prfmsg(ITMLST5);
	       for (i=0 ; i < nitms-1 ; i++)
	       {
		    prfmsg(ITMLST6,ert_nxt_itm(iitems[i]));
	       }
	       prfmsg(ITMLST7,ert_nxt_itm(iitems[nitms-1]),pprobl);
	  }
     }
}

CHAR *ert_nxt_itm(SHORT itmno)       /* a/an and item name by item number  */
{
     CHAR *ert_and();

     return(ert_and(&ertitems[itmno]));
}

SHORT ertinv(VOID)                        /*  see items in room function     */
{
   prfmsg(INVPRT1);
   ert_inv_utl(ertply);
   outprf(usrnum);
   if (ertply->flags&INWLD)
   {
      prfmsg(INVPRT2,ertply->charac, ertply->hisher);
      ert_pwf(ertply,SEEPYR);
   }
   return(0);
}

VOID ert_inv_utl(struct ertplayer *ppt)
{
     struct item **items;
     CHAR *ert_and();
     int nitms=0,i;

     (VOID)nitms;
     items=ppt->items;
     switch (nitms=ppt->nitems) {
     case 0:
	  prfmsg(INVUTL1);
	  break;
     case 1:
	  prfmsg(INVUTL2,ert_and(items[0]));
	  break;
     case 2:
	  prfmsg(INVUTL3,ert_and(items[0]));
	  prfmsg(INVUTL4,ert_and(items[1]));
	  break;
     default:
	   for (i=0 ; i < nitms-1 ; i++)
	   {
	       prfmsg(INVUTL5,ert_and(items[i]));
	   }
	   prfmsg(INVUTL6,ert_and(items[nitms-1]));
     }
}

CHAR *ert_and(struct item *itm)                      /* "a" or "an" for object utlity   */
{

     static CHAR rtval[30];

     if (itm->flags&USEAN)
     {
	  strcpy(rtval,"an ");
     }
     else
     {
	  strcpy(rtval,"a ");
     }
     strcat(rtval,itm->name);
     return(rtval);
}

CHAR *ert_aoran(CHAR *noun)             /* "a" or "an" utility for any noun*/
                                        /* (returns "a foo" or "an akko")  */
{
     static CHAR totstg[40];

     switch (tolower(*noun))
     {
     case 'a':
     case 'e':
     case 'i':
     case 'o':
     case 'u':
	  strcpy(totstg,"an ");
	  break;
     default:
	  strcpy(totstg,"a ");
     }
     strcat(totstg,noun);
     return(totstg);
}

VOID ert_see_pyrs(struct ertplayer *pp)                    /*  see players in room function  */
{
     int npyr=0,i;

     (VOID)npyr;
     if (pp->aura&SEEPYR)
     {
	  ert_init_see(pp);
	  switch (npyr=ert_nxt_pyr(pp))
	  {
	  case 0:
	       if (pp->ertroom->ertmnum > 0 || pp->ertroom->num_nlv > 0)
	       {
		  ertseemnst(pp, 0);
		  break;
	       }
	       else
		  break;
	  case 1:
	       if (pp->ertroom->ertmnum > 0 || pp->ertroom->num_nlv > 0)
	       {
		  prfmsg(SEEP4,ert_see_nxtp(pp)->charac);
		  ertseemnst(pp, 1);
	       }
	       else
		  prfmsg(SEEP1,ert_see_nxtp(pp)->charac);
	       break;
	  case 2:
	       if (pp->ertroom->ertmnum > 0 || pp->ertroom->num_nlv > 0)
	       {
		  prfmsg(SEEP6, ert_see_nxtp(pp)->charac,
				ert_see_nxtp(pp)->charac);
		  ertseemnst(pp, 1);
	       }
	       else
	       prfmsg(SEEP2,ert_see_nxtp(pp)->charac,
		      ert_see_nxtp(pp)->charac);
	       break;
	  default:
	       prfmsg(SEEP3);
	       for (i=0 ; i < npyr-1 ; i++)
	       {
		    prfmsg(SEEP4,ert_see_nxtp(pp)->charac);
	       }
	       if (pp->ertroom->ertmnum > 0)
	       {
		    prfmsg(SEEP4,ert_see_nxtp(pp)->charac);
		    ertseemnst(pp,1);
	       }
	       else
		  prfmsg(SEEP5,ert_see_nxtp(pp)->charac);
	  }
     }
}

VOID ertseemnst(struct ertplayer *pp, SHORT flag) /* flag: 0 = don't print and, 1 = print and */
{
   CHAR *imnstrs;
   SHORT nmnstrs=0, i;

   (VOID)nmnstrs;
   imnstrs=pp->ertroom->ertmlist;
   switch (nmnstrs=pp->ertroom->ertmnum)
   {
      case 0:
	 if (pp->ertroom->num_nlv > 0)
	 {
	    ertseenlv(pp, 0);
	    break;
	 }
	 else
	    break;
      case 1:
	 if (flag == 1) // print and
	 {
	    if (pp->ertroom->num_nlv == 0)
	    {
	       prfmsg(SEEP5,ert_nam(ertanmnno(imnstrs[0]))); // and %s are here.
	    }
	    else
	    {
	       prfmsg(SEEP4,ert_nam(ertanmnno(imnstrs[0]))); // %s,
	       ertseenlv(pp, 1);
	    }
	 }
	 else           // don't print and
	 {
	    if (pp->ertroom->num_nlv == 0)
	    {
	       prfmsg(SEEP1, ert_nam(ertanmnno(imnstrs[0]))); // %s is here
	    }
	    else
	    {
	       prfmsg(SEEP4,ert_nam(ertanmnno(imnstrs[0])));
	       ertseenlv(pp,1);
	    }
	 }
	 break;
      default:
	 prfmsg(SEEP3); // {}
	 for (i = 0; i < nmnstrs-1; i++)
	 {
	    prfmsg(SEEP4, ert_nam(ertanmnno(imnstrs[i]))); // %s
	 }
	 if (pp->ertroom->num_nlv > 0)
	 {
	    prfmsg(SEEP4, ert_nam(ertanmnno(imnstrs[i])));
	    ertseenlv(pp,1);
	 }
	 else
	 prfmsg(SEEP5, ert_nam(ertanmnno(imnstrs[nmnstrs-1]))); // and %s are here
   }
}

VOID ertseenlv(struct ertplayer *pp, SHORT flag)
{
   SHORT nnlv=0, head, i;

   (VOID)nnlv;
   head    = pp->ertroom->room_num;

   switch (nnlv=pp->ertroom->num_nlv)
   {
      case 0:
	 break;
      case 1:
	 head = (SHORT)seenxnlv(head);
	 if (flag == 1)
	    prfmsg(SEEP5,ert_inact_lst[head].iname); // and %s are here.
	 else
	    prfmsg(SEEP1, ert_inact_lst[head].iname); // %s is here
	 break;
      default:
	 prfmsg(SEEP3); // {}
	 for (i = 0; i < nnlv-1; i++)
	 {
	    head = (SHORT)seenxnlv(head);
	    prfmsg(SEEP4, ert_inact_lst[head].iname); // %s
	 }
	 head = (SHORT)seenxnlv(head);
	 prfmsg(SEEP5, ert_inact_lst[head].iname); // and %s are here
   }
}

SHORT seenxnlv(SHORT current)
{
    return(ert_inact_lst[current].ilink);
}

CHAR *ertanmnno(SHORT mnum)
{
   CHAR *ertanmnmn();

   return(ertanmnmn(&mnstr_list[mnum]));
}

CHAR *ertanmnmn(struct monster *mnstr)
{
   return(mnstr->mname);
}

SHORT ert_nxt_pyr(struct ertplayer *pp)                     /* function: how many players seen */
{
     struct ertplayer *op;
     SHORT retval;

     for (op=pp->ertroom->roomhdr,retval=0 ; op != NULL ; op=op->plink)
     {
	  if (op != pp)
	  {
	       retval++;
	  }
     }
     return(retval);
}

VOID ert_init_see(struct ertplayer *pp)                  /* initialization of see players utl*/
{
     pspp=pp->ertroom->roomhdr;
}

struct ertplayer *ert_see_nxtp(struct ertplayer *pp)     /* link-list continuation: seeplyrs*/
{
     struct ertplayer *opspp;

     while (pspp != NULL)
     {
	  opspp=pspp;
	  pspp=pspp->plink;
	  if (opspp != pp)
	  {
	       return(opspp);
	  }
     }
     return(NULL);
}

SHORT ert_num_ply(struct ertroom *ertroom)                 /* number of players in room utl.  */
{
     struct ertplayer *pp;
     SHORT retval;

     for (pp=ertroom->roomhdr,retval=0 ; pp != NULL ; pp=pp->plink,retval++) {
     }
     return(retval);
}

VOID ert_say_delay(VOID)                 /* prepare input for "say" format  */
{
     SHORT i;

     for (i=1 ; i < margc-1 ; i++)
     {
	  *margn[i]=' ';
     }
}

VOID ert_whspr(VOID)
{
     SHORT i;

     for (i=3 ; i < margc-1 ; i++)
     {
	  *margn[i]=' ';
     }
}

struct item *ert_get_ply(struct ertplayer *who,CHAR *name)      /* get a player's item utlity      */
                                                                /* for use: get/drop/look/ertcute  */
{
     SHORT i,looplim;
     struct item **itmppt,*retval;

     for (i=0,itmppt=who->items,looplim=who->nitems ; i < looplim ; i++)
     {
	  if (sameto(name,(*itmppt)->name))
	  {
	       retval=*itmppt;
	       if (i != looplim-1)
	       {
		    *itmppt=who->items[looplim-1];
	       }
	       who->nitems--;
	       return(retval);
	  }
	  itmppt++;
     }
     return(NULL);
}

struct item *ert_get_itm(struct ertroom *where,CHAR *name)    /* get item in room utility        */
                                                              /* for use: get/look/ertcute etc.  */
{
     SHORT i,looplim;
     CHAR *itm;
     struct item *retval;

     for (i=0,itm=where->iitems,looplim=where->nitems ; i < looplim ; i++)
      {
	  if (sameto(name,ertitems[itm[i]].name))
	  {
	       retval=&ertitems[itm[i]];
	       if (i != looplim-1)
	       {
		    where->iitems[i]=where->iitems[looplim-1];
	       }
	       where->nitems--;
	       return(retval);
	  }
     }
     return(NULL);
}

struct monster *ertgetmnstr(struct ertroom *where, CHAR *name) /* this routine is never called - RH 8/10/2024 */
{
   SHORT i, looplim;
   CHAR *mnst;
   struct monster *retval;

   for(i=0,mnst=where->ertmlist,looplim=where->ertmnum;i < looplim; i++)
   {
      if (sameto(name, mnstr_list[mnst[i]].mname))
      {
	 retval=&mnstr_list[mnst[i]];
	 if (i != looplim - 1)
	 {
	    where->ertmlist[i]=where->ertmlist[looplim - 1];
	    where->ertmhps[i]=where->ertmhps[looplim - 1];
	 }
	 where->ertmnum--;
	 return(retval);
      }
   }
   return(NULL);
}

SHORT ert_put_ply(struct ertplayer *who,struct item *itmptr) /* None of the calls check the return value - RH 8/10/2024 */
{
     if (who->nitems >= MAXINV)
     {
	  return(0);
     }
     who->items[who->nitems++]=itmptr;
     return(1);
}

SHORT ert_itm_rm(struct ertroom *where,struct item *itmptr)  /* None of the calls check the return value - RH 8/10/2024 */
{
     if (where->nitems >= MAXINV)
     {
	  return(0);
     }
     where->iitems[where->nitems++]=(CHAR)(itmptr-ertitems);
     return(1);
}

SHORT ertputmnstr(struct ertroom *where, SHORT mnum, SHORT hits) /* This routine is never called - RH 8/10/2024 */
{
   if (where->ertmnum >= MXITEM)
   {
      return(0);
   }
   where->ertmlist[where->ertmnum++]= (CHAR)mnum; //rick 4/18/23 - VS warning C4244
   ertply->ertroom->ertmhps[ertply->ertroom->ertmnum - 1] = hits;  
   return(1);
}

struct item *ert_loc_itm(CHAR *itmnam)
{
     struct item *itmptr;

     if ((itmptr=ert_get_ply(ertply,itmnam)) == NULL)
     {
	  if ((itmptr=ert_get_itm(ertply->ertroom,itmnam)) == NULL)
	  {
	       return(NULL);
	  }
	  ert_itm_rm(ertply->ertroom,itmptr);
     }
     else
     {
	  ert_put_ply(ertply,itmptr);
     }
     return(itmptr);
}

SHORT ertfnlv(CHAR *name)
{
   SHORT current;
   current = ertply->ertroom->room_num;

   while(ert_inact_lst[current].ilink != 0)
   {
      if (sameto(name, ert_inact_lst[current].iname))
	 return(current);
      current = ert_inact_lst[current].ilink;
   }
   if (sameto(name, ert_inact_lst[current].iname))
      return(current);
   else
   return(FALSE);
}

struct ertplayer *ertfplyr(CHAR *name)
{
     struct ertplayer *pp;

     for (pp=ertply->ertroom->roomhdr ; pp != NULL ; pp=pp->plink)
     {
	  if ((ert_see_flg()&ertply->aura) && sameto(name,pp->charac))
	  {
	       return(pp);
	  }
     }
     return(NULL);
}

struct monster *ertfmnstr(struct ertroom *where,CHAR *name)
{
   int i, looplim;
   CHAR *mnst;
   struct monster *retval;

   for (i=0, mnst=where->ertmlist, looplim=where->ertmnum; i < looplim; i++)
   {
      if (sameto(name, mnstr_list[mnst[i]].mname))
      {
	 retval=&mnstr_list[mnst[i]];
	 return(retval);
      }
   }
   return(NULL);
}

struct ertplayer *plyr_ert(CHAR *name)
{
     SHORT i;
     struct ertplayer *pp;

     for (i=0,pp=ertplayer ; i < nterms ; i++,pp++)
     {
	  if ((pp->flags&INWLD) && sameas(name,pp->plyrid))
	  {
	       return(pp);
	  }
     }
     return(NULL);
}

VOID ert_del_the(VOID)
{
   SHORT i;

   for (i = 0 ; i < margc - 1 ; i++)
   {
      if (sameas(margv[i],"the") || sameas(margv[i],"a")
	  || sameas(margv[i],"an"))
      {
	 *margn[i]=' ';
	 margc--;
         movmem(&margv[i+1],&margv[i],(margc-i)*sizeof(CHAR *));
         movmem(&margn[i+1],&margn[i],(margc-i)*sizeof(CHAR *));
      }
   }
}

VOID ert_del_prep(VOID)
{
   SHORT i;

   for (i=0 ; i < margc-1 ; i++)
   {
      if (sameas(margv[i],"at") || sameas(margv[i],"in")
	  || sameas(margv[i],"to") || sameas(margv[i],"on")
	  || sameas(margv[i],"of") || sameas(margv[i],"under")
	  || sameas(margv[i],"inside"))
      {
	 *margn[i]=' ';
	 margc--;
         movmem(&margv[i+1],&margv[i],(margc-i)*sizeof(CHAR *));
         movmem(&margn[i+1],&margn[i],(margc-i)*sizeof(CHAR *));
      }
   }
}

CHAR *ert_nam(CHAR *stg)
{
     CHAR *tmp;
     static CHAR namstg[40];

     namstg[0]=(CHAR)toupper(*stg);
     for (tmp=namstg+1 ; *++stg != '\0' ; )
     {
	  *tmp++=(CHAR)tolower(*stg);
     }
     *tmp='\0';
     return(namstg);
}

VOID ert_xmit(SHORT msgno)
{
     CHAR *getmsg();

     btuxmt(usrnum,getmsg(msgno));
}

/***************************************************************************
*                                                                          *
*     Erotica Command List and Handler                                     *
*                                                                          *
***************************************************************************/

#define WLDASIZ (sizeof(ertcmdarr)/sizeof(struct ert_cmd))

struct ert_cmd ertcmdarr[]={
     {"abuse",   ertbemean,1},
     {"act",     ertany,1},
     {"admit",   ertsay,0},
     {"advise",  ertsay,0},
     {"agree",   ertsay,0},
     {"amble",   ertmov,1},
     {"announce",ertsay,0},
     {"answer",  ertsay,0},
     {"applaud", ertapp,1},
     {"argue",   ertsay,0},
     {"arouse",  ertarouse,1},
     {"ask",     ertsay,0},
     {"ass",     ert_mean,1},
     {"asshole", ert_mean,1},
     {"assume",  ertsay,0},
     {"babble",  ertsay,0},
     {"barf",    ertsic,1},
     {"bastard", ert_mean,1},
     {"bawl",    ertcry,1},
     {"be",      ertany,1},
     {"beat",    ertbemean,1},
     {"beg",     ertbeg,1},
     {"belch",   ertbur,1},
     {"bellow",  ertyell,1},
     {"bet",     ertgam,1},
     {"blink",   ertblk,1},
     {"blush",   ertblu,1},
     {"boast",   ertsay,0},
     {"boo",     ertboo,1},
     {"bow",     ertbow,1},
     {"brief",   ertbrf,0},
     {"broadcast",ertbrodcst,3},
     {"bug",     ertbemean,1},
     {"bump",    ertbemean,1},
     {"burn",    ertany,1},
     {"burp",    ertbur,1},
     {"buy",     erttrd,1},
     {"cackle",  ertcac,1},
     {"canter",  ertmov,1},
     {"change",  ertchange,1},
     {"chant",   ertsay,1},
     {"charm",   ertcute,1},
     {"chat",    ertsay,1},
     {"cheer",   ertchr,1},
     {"chivalry",ertchv,1},
     {"choke",   ertwhe,1},
     {"chortle", ertcho,1},
     {"chuckle", ertchu,1},
     {"clap",    ertapp,1},
     {"comfort", ertcute,1},
     {"command", ertsay,0},
     {"comment", ertsay,0},
     {"commune", ertpra,1},
     {"correct", ertsay,0},
     {"cough",   ertcou,1},
     {"cower",   ertcow,1},
     {"crawl",   ertmov,1},
     {"cry",     ertcry,1},
     {"cuddle",  ertcute,1},
     {"cunt",    ert_mean,1},
     {"curse",   ertswe,1},
     {"cuss",    ert_mean,1},
     {"d",       ertdown,0},
     {"dance",   ertdan,1},
     {"declare", ertsay,0},
     {"defend",  ertcute,0},
     {"demand",  ertsay,0},
     {"dick",    ert_mean,1},
     {"down",    ertdown,0},
     {"draw",    ertany,1},
     {"drop",    ertdrprou,0},
     {"e",       ert_east,0},
     {"east",    ert_east,0},
     {"embrace", ertcute,1},
     {"emulate", ertbemean,1},
     {"enchant", ertany,1},
     {"examine", ertlokrou,0},
     {"exchange",erttrd,1},
     {"explain", ertsay,0},
     {"fairy",   ertfairy,3},
     {"fart",    ertfar,1},
     {"fight",   ertbemean,0},
     {"find",    ertany,1},
     {"fondle",  ert_mean,1},
     {"forgive", ertcute,1},
     {"frown",   ertfrn,1},
     {"fuck",    ert_mean,1},
     {"full",    ertlon,1},
     {"fuss",    ertwhm,1},
     {"gag",     ertwhe,1},
     {"gamble",  ertgam,1},
     {"gasp",    ertgas,1},
     {"get",     ertgetrou,0},
     {"giggle",  ertgig,1},
     {"give",    ertgiv,0},
     {"glare",   ertglr,1},
     {"go",      ertmov,1},
     {"godspeak",ertgodspk,3},
     {"grab",    ertgetrou,0},
     {"grasp",   ertgetrou,0},
     {"grin",    ertgrn,1},
     {"gripe",   ertsay,1},
     {"groan",   ertgon,1},
     {"grovel",  ertgrv,1},
     {"growl",   ertgrw,1},
     {"grumble", ertsay,1},
     {"grunt",   ertgru,1},
     {"hand",    ertgiv,0},
     {"help",    erthel,0},
     {"high",    ert_high,0},
     {"hint",    ertsay,0},
     {"hiss",    erthis,1},
     {"hobble",  ertmov,1},
     {"hold",    ertcute,1},
     {"holler",  ertyell,1},
     {"hop",     ertmov,1},
     {"hug",     ertcute,1},
     {"hum",     erthum,1},
     {"i",       ertinv,0},
     {"impress", ertcute,1},
     {"incant",  ertsay,1},
     {"inquire", ertsay,0},
     {"insist",  ertsay,0},
     {"inv",     ertinv,0},
     {"inventory",ertinv,0},
     {"jostle",  ertbemean,1},
     {"jump",    ertjmp,1},
     {"kick",    ertbemean,1},
     {"kiss",    ertcute,1},
     {"laugh",   ertlgh,1},
     {"leap",    ertmov,1},
     {"lick",    ert_mean,1},
     {"lie",     ertlie,1},
     {"long",    ertlon,0},
     {"look",    ertlokrou,0},
     {"love",    ertcute,1},
//     {"mail",    ertmail,1},
     {"map",     ertmap,0},
     {"march",   ertmov,1},
     {"marry",   ertcute,1},
     {"masturbate",ert_mean,1},
     {"meditate",ert_thk,1},
     {"mention", ertsay,0},
     {"moan",    ertmon,1},
     {"money",   ertdollars,0},
     {"move",    ertmov,0},
     {"mumble",  ertsay,0},
     {"murmur",  ertsay,0},
     {"mutter",  ertsay,0},
     {"n",       ert_north,0},
     {"neck",    ert_mean,1},
     {"nod",     ertnod,1},
     {"nonbrief",ertlon,0},
     {"north",   ert_north,0},
     {"note",    ertsay,0},
     {"notice",  ertsay,0},
     {"obey",    ertcute,1},
     {"object",  ertsay,0},
     {"obtain",  ertgetrou,0},
     {"offer",   ertsay,0},
     {"pat",     ertcute,0},
     {"pee",     ert_mean,1},
     {"pester",  ertbemean,1},
     {"pet",     ertcute,1},
     {"pickpocket",ertgetrou,0},
     {"pilfer",  ertgetrou,0},
     {"pinch",   ertbemean,1},
     {"piss",    ert_mean,1},
     {"place",   ertany,1},
     {"plead",   ertbeg,1},
     {"please",  ertcute,1},
     {"poke",    ertbemean,1},
     {"pout",    ertpot,1},
     {"pray",    ertpra,1},
     {"predict", ertsay,0},
     {"project", ertany,1},
     {"promise", ertsay,0},
     {"propose", ertsay,0},
     {"protect", ertcute,1},
     {"protest", ertsay,1},
     {"puke",    ertsic,1},
     {"purchase",erttrd,1},
     {"push",    ertshv,1},
     {"pussy",   ert_mean,1},
     {"put",     ertany,1},
     {"quake",   ertquake,3},
     {"raise",   ertany,1},
     {"ramble",  ertmov,1},
     {"rape",    ert_mean,1},
     {"rave",    ertsay,0},
     {"remark",  ertsay,0},
     {"repeat",  ertsay,0},
     {"rob",     ertgetrou,0},
     {"romance", ertcute,1},
     {"run",     ertmov,0},
     {"s",       ert_south,0},
     {"say",     ertsay,0},
     {"scamper", ertmov,0},
     {"scold",   ertsay,0},
     {"score",   ertscore,0},
     {"scramble",ertmov,0},
     {"scream",  ertyell,1},
     {"screw",   ert_mean,1},
     {"scurry",  ertmov,0},
     {"seduce",  ertbemean,1},
     {"see",     ertlokrou,0},
     {"sell",    erttrd,1},
     {"shake",   ertcute,1},
     {"shit",    ertswe,1},
     {"shout",   ertyell,1},
     {"shove",   ertshv,1},
     {"shrug",   ertshg,1},
     {"shuffle", ertmov,0},
     {"sigh",    ertsih,1},
     {"sing",    ertsin,1},
     {"sit",     ertsit,1},
     {"skip",    ertmov,0},
     {"slap",    ertbemean,1},
     {"smile",   ertsml,1},
     {"smirk",   ertsmk,1},
     {"smoke",   ertsmo,1},
     {"snarl",   ertsnl,1},
     {"sneer",   ertsnr,1},
     {"sneeze",  ertsnz,1},
     {"snicker", ertsnk,1},
     {"sniff",   ertsnf,1},
     {"sniffle", ertsnf,1},
     {"snore",   ertsno,1},
     {"sob",     ertsay,1},
     {"south",   ert_south,0},
     {"speak",   ertsay,0},
     {"spell",   ertsay,0},
     {"spit",    ertspi,1},
     {"squeal",  ertyell,1},
     {"stammer", ertsay,1},
     {"state",   ertsay,0},
     {"statistics",ertstats,0},
     {"stats",   ertstats,0},
     {"steal",   ertgetrou,0},
     {"stomp",   ertstm,1},
     {"strike",  ertbemean,0},
     {"strip",   ert_mean,1},
     {"stroll",  ertmov,0},
     {"stutter", ertsay,1},
     {"suck",    ert_mean,1},
     {"suggest", ertsay,0},
     {"swear",   ertswe,1},
     {"sysget",  ertcoj,3},
     {"syslist", ertprn,3},
     {"take",    ertgetrou,0},
     {"talk",    ertsay,0},
     {"taste",   ertany,0},
     {"taunt",   ertbemean,1},
     {"tell",    ertsay,0},
     {"thank",   ertcute,1},
     {"threaten",ertsay,0},
     {"throw",   ertdrprou,0},
     {"tickle",  ertcute,1},
     {"tiptoe",  ertmov,0},
     {"torment", ertbemean,1},
     {"toss",    ertdrprou,0},
     {"trade",   erttrd,1},
     {"tramp",   ertmov,1},
     {"travel",  ertmov,0},
     {"trip",    ertbemean,1},
     {"u",       ertup,0},
     {"up",      ertup,0},
     {"urge",    ertsay,0},
     {"use",     ertany,1},
     {"utilize", ertany,1},
     {"utter",   ertsay,0},
     {"vision",  ertvision,3},
     {"w",       ert_west,0},
     {"wail",    ertyell,1},
     {"walk",    ertmov,0},
     {"wander",  ertmov,0},
     {"warn",    ertsay,0},
     {"wed",     ertcute,1},
     {"weep",    ertcry,1},
     {"west",    ert_west,0},
     {"wheeze",  ertwhe,1},
     {"whimper", ertwhm,1},
     {"whine",   ertsay,1},
     {"whisper", ertwhs,1},
     {"whistle", ertwis,1},
     {"wiggle",  ertmov,1},
     {"wink",    ertwnk,1},
     {"worship", ertcute,1},
     {"write",   ertany,1},
     {"yawn",    ertyaw,1},
     {"yell",    ertyell,1},
     {"yelp",    ertsay,1},
     {"zigzag",  ertany,1}
};

struct ert_cmd *ertbinary(CHAR *stgptr,struct ert_cmd table[], SHORT length)
{
     SHORT cond;
     struct ert_cmd *low, *mid, *high;

     low=&table[0];
     high=&table[length-1];
     while (low <= high)
     {
	  mid=low+((int)(high-low))/2;
	  if ((cond=(SHORT)strcmp(stgptr,mid->command)) < 0)
	  {
	       if (mid == low)
	       {
		    break;
	       }
	       high=mid-1;
	  }
	  else if (cond > 0)
	  {
	       if (mid == high)
	       {
		    break;
	       }
	       low=mid+1;
	  }
	  else
	  {
	       return(mid);
	  }
     }
     return(NULL);
}

VOID ert_parse(VOID)
{
     struct ert_cmd *areptr;
     CHAR *mv0ptr;

     if (margc == 0)
     {
	  if (usrptr->flags&ABOIP)
	  {
	       prf("\r");
	  }
	  else
	  {
	     prfmsg(TYHELP);
	  }
	  outprf(usrnum);
     }
     else {
	  for (mv0ptr=margv[0] ; *mv0ptr != '\0' ; mv0ptr++)
	  {
	     *mv0ptr=(CHAR)tolower(*mv0ptr);
	  }
          if ((*ertply->ertroom->rmcmd)())
	  {
	       if ((areptr=ertbinary(margv[0],ertcmdarr,WLDASIZ)) != NULL)
	       {
		  if(areptr->liveuse && !hasmkey(ERTKEY))
		  {
		     prfmsg(MUSTBL);
		     howbuy();
		     outprf(usrnum);
		  }
		  else
		  if (areptr->liveuse == 3 && !(usrptr->flags&MASTER))
		  {
		     prfmsg(HEYGIV);
		     outprf(usrnum);
		     prfmsg(LOOKCN,ertply->charac);
		     ert_room_out();
		  }
		  else
		  {
                     (areptr->routine)();
		  }
	       }
	       else
	       {
		    switch (margc)
		    {
		    case 1:
			 prfmsg(HEYGIV);
			 break;
		    case 2:
			 prfmsg(WHYTHT,margv[0],margv[1]);
			 break;
		    case 3:
			 prfmsg(WHAGAB);
			 break;
		    case 4:
			 prfmsg(SEREXP,margv[0],margv[1],margv[2],margv[3]);
			 break;
		    default:
			 prfmsg(UHHUH);
			 break;
		    }
		    outprf(usrnum);
		    prfmsg(LOOKCN,ertply->charac);
		    ert_room_out();
	       }
	  }
     }
}

unsigned ert_rnd(VOID)
{
     static unsigned seed=3569;
     LONG tmp;

     tmp = seed*62891L+871L;
     seed = (unsigned)tmp&0xFFFF;
     return(seed);
}

/***************************************************************************
*                                                                          *
*     Erotica Monster List/Array                                           *
*                                                                          *
***************************************************************************/

struct monster mnstr_list[3]={

{"dummy",                                 /* Monster 0 */
R1,
1,0,0,
0,
50,46,30,1,0},

{"sally",                              /* Robot 1 */
R1,
1,0,0,
0,
15,0,30,1,1},

{"bartender",                          /* Robot 2 */
R2,
1,0,0,
0,
5,3,30,1,2}
};

/***************************************************************************
*                                                                          *
*     Erotica Array of Items                                               *
*                                                                          *
***************************************************************************/

struct item ertitems[]={

     {"condom",                               /*  Item 000  */
     ITM001,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS,
     0,
     DEFNSE,
     200,
     1},

     {"handcuffs",                            /*  Item 001  */
     ITM002,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+OFFENS,
     5,
     OFFNSE,
     500,
     15},

     {"spanish-fly",                          /*  Item 002  */
     ITM003,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+OFFENS,
     4,
     OFFNSE,
     500,
     10},

     {"vibrator",                             /*  Item 003  */
     ITM004,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+OFFENS,
     5,
     OFFNSE,
     500,
     15},

     {"whip",                                 /*  Item 004  */
     ITM005,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+OFFENS,
     4,
     OFFNSE,
     500,
     10},

     {"silk-scarf",                           /*  Item 005  */
     ITM006,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+OFFENS,
     3,
     OFFNSE,
     500,
     6},

     {"playhouse-magazine",                   /*  Item 006  */
     ITM007,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+OFFENS,
     2,
     OFFNSE,
     500,
     5},

     {"handcuff-key",                         /*  Item 007  */
     ITM008,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS,
     0,
     DEFNSE,
     500,
     30},

     {"sexacillin",                           /*  Item 008  */
     ITM009,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS,
     0,
     DEFNSE,
     500,
     35},

     {"chastity-belt",                        /*  Item 009  */
     ITM010,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS,
     0,
     DEFNSE,
     10,
     30},

     {"unsubmissive-potion",                    /*  Item 010  */
     ITM011,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS+USEAN,
     0,
     DEFNSE,
     9,
     40},

     {"scissors",                             /*  Item 011  */
     ITM012,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS,
     0,
     DEFNSE,
     11,
     15},

     {"anti-glasses",                         /*  Item 012  */
     ITM013,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS+USEAN,
     0,
     DEFNSE,
     29,
     15},

     {"dart-gun",                             /*  Item 013  */
     ITM014,
     ert_null,
     ert_null,
     ert_null,
     PICKUP+DEFENS,
     0,
     DEFNSE,
     19,
     35}

};

struct inact_ply ert_inact_lst[1000]={

{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{0,"              ","                             ",0L,0,0},
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 },
{ 0,"              ","                             ",0L,0,0 }

};


struct hiscore high_score[11]={
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},
{"                             ",0L},

};

VOID zonkhl(CHAR *stg)                            /* "zonk" string for use w/ Btrieve     */
{
     CHAR *inpptr;
     INT space=1,format;

     format= isupperorlower(stg);
     for (inpptr=stg ; *inpptr != '\0' ; inpptr++) {
          if (format) {
               if (*inpptr == ' ') {
                    space=1;
               }
               else if (space) {
                    *inpptr=(CHAR)toupper(*inpptr);
                    space=0;
               }
               else {
                    *inpptr=(CHAR)tolower(*inpptr);
               }
          }
     }
     while (++inpptr-stg < UIDSIZ) {
          *inpptr='\0';
     }
}

SHORT isupperorlower(CHAR *stg)
{
     CHAR *ptr;

     for (ptr=stg ; *ptr != '\0' ; ptr++) {
          if (isalpha(*ptr) && !islower(*ptr)) {
               break;
          }
     }
     if (*ptr == '\0') {
          return(1);
     }
     for (ptr=stg ; *ptr != '\0' ; ptr++) {
          if (isalpha(*ptr) && !isupper(*ptr)) {
               break;
          }
     }
     if (*ptr == '\0') {
          return(1);
     }
     return(0);
}
