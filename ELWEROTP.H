/*****************************************************************************
 *                                                                           *
 *    Erotica Version 1.41                                                   *
 *                                                                           *
 *    ELWEROTP.H                                                             *
 *    Erotica Function Prototype Header File                                 *
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

/* In ELWEROT.C */
VOID zonkhl(CHAR *stg);
SHORT isupperorlower(CHAR *stg);
GBOOL erotica_input(VOID);
GBOOL ert_intro(VOID);
VOID EXPORT init__elwerot(VOID);
SHORT ert_init(INT usrnum);
SHORT ert_prep(VOID);
SHORT ert_rest(VOID);
SHORT ertprn(VOID);
VOID erotica_delete(CHAR* userid);
VOID erotica_clean_up(VOID);
VOID erotica_hang_up(VOID);
VOID ert_init_hiscore(VOID);
VOID init_hiscore(VOID);
VOID ert_link(VOID);
VOID ert_unlink(VOID);
VOID ert_relink(VOID);
VOID ert_init_inactive(VOID);
VOID ertsex(VOID);
VOID ertuoral(VOID);
VOID ertoralu(VOID);
VOID ert_insert(CHAR name[31], ULONG score);
VOID ert_del_name(CHAR name[30]);
VOID erthitoth(SHORT nhits);
VOID erthityou(SHORT nhits);
VOID ertlstitms(struct ertplayer* ppt);
VOID ert_attack(struct ertplayer* ppt);
VOID ert_subtract_money(SHORT cost_dollars);
VOID ert_add_money(SHORT dollars_add);
VOID ert_add_hits(SHORT num_add);
SHORT ert_high(VOID);
SHORT ert_lbrry(VOID);
SHORT ert_medic(VOID);
SHORT ertpub(VOID);
SHORT ertarouse(VOID);
SHORT check_compatibility(CHAR name[30]);
SHORT check_type(struct ertplayer* ppt, SHORT num);
SHORT check_defenses(CHAR itm[20]);
SHORT check_odefenses(CHAR itm[20]);
SHORT ertbank(VOID);
SHORT ertmgc_shop(VOID);
SHORT ertgnrl_shop(VOID);
SHORT ertarmr_shop(VOID);
SHORT ertwpn_shop(VOID);
SHORT ertshops(SHORT shop_type);
SHORT ert_rnd_num(SHORT max, SHORT rolls);
SHORT erttreasure_table(SHORT t_type1, SHORT t_type2);
SHORT ert_casino(VOID);
SHORT ert_enough_money(SHORT cost_dollars);
CHAR *ert_ilst(struct item *itm);
SHORT ertapp(VOID);
SHORT ertgig(VOID);
SHORT ertjmp(VOID);
SHORT ertlgh(VOID);
SHORT ertshg(VOID);
SHORT ertlie(VOID);
SHORT ertwhe(VOID);
SHORT ertsit(VOID);
SHORT ertgrv(VOID);
SHORT ertsnr(VOID);
SHORT ertcac(VOID);
SHORT ertswe(VOID);
SHORT ertcho(VOID);
SHORT ertbeg(VOID);
SHORT ertchr(VOID);
SHORT ertblu(VOID);
SHORT ertgas(VOID);
SHORT ertcow(VOID);
SHORT ertbow(VOID);
SHORT ertsmk(VOID);
SHORT ertchv(VOID);
SHORT ertboo(VOID);
SHORT erthis(VOID);
SHORT ertfrn(VOID);
SHORT ertsnl(VOID);
SHORT ertpot(VOID);
SHORT ertgrw(VOID);
SHORT ertgru(VOID);
SHORT ertsml(VOID);
SHORT ertgrn(VOID);
SHORT ertdan(VOID);
SHORT ertsih(VOID);
SHORT ertsin(VOID);
SHORT ertsnz(VOID);
SHORT ertwhm(VOID);
SHORT ertsmo(VOID);
SHORT ertspi(VOID);
SHORT ertsnk(VOID);
SHORT ertgon(VOID);
SHORT ertmon(VOID);
SHORT ertbur(VOID);
SHORT ertfar(VOID);
SHORT ertyaw(VOID);
SHORT ertnod(VOID);
SHORT ertgam(VOID);
SHORT erttrd(VOID);
SHORT ertpra(VOID);
SHORT ertany(VOID);
SHORT ertsic(VOID);
SHORT ertstm(VOID);
SHORT ertcry(VOID);
SHORT ertcou(VOID);
SHORT ertchu(VOID);
SHORT ertwis(VOID);
SHORT erthum(VOID);
SHORT ertblk(VOID);
SHORT ertsnf(VOID);
SHORT ertsno(VOID);
SHORT ertscore(VOID);
SHORT ertchange(VOID);
SHORT ertmap(VOID);
SHORT erthel(VOID);
SHORT ertbrf(VOID);
SHORT ertlon(VOID);
SHORT ertdollars(VOID);
SHORT ertstats(VOID);
SHORT ertcoj(VOID);
SHORT ertgodspk(VOID);
SHORT ertquake(VOID);
SHORT ertbrodcst(VOID);
SHORT ertfairy(VOID);
SHORT ert_vision(VOID);
SHORT ert_null(VOID);
SHORT ert_exmn(VOID);
SHORT ert_rub(VOID);
SHORT ert_thk(VOID);
VOID ert_timed(VOID);
SHORT ertsay(VOID);
SHORT ertwhs(VOID);
SHORT ertyell(VOID);
SHORT ertmov(VOID);
SHORT ert_mean(VOID);
SHORT ertcute(VOID);
SHORT ertbemean(VOID);
SHORT ertgut(CHAR *stg1, CHAR *stg2, CHAR *stg3, CHAR *stg4, CHAR *stg5);
SHORT ertgiv(VOID);
SHORT ertgivutl(CHAR* givee, CHAR* givwhat);
SHORT ertgetrou(VOID);
SHORT ertgetput(CHAR* who, CHAR* getwhat);
SHORT ertgetrut(CHAR* getwhat);
SHORT ert_north(VOID);
SHORT ert_south(VOID);
SHORT ert_east(VOID);
SHORT ert_west(VOID);
SHORT ertup(VOID);
SHORT ertdown(VOID);
SHORT ertlokrou(VOID);
SHORT ertshv(VOID);
SHORT ertshvutl(const char* from, const char* to, struct ertroom* entrp, USHORT room_to);
SHORT ertdrprou(VOID);
SHORT ertwnk(VOID);
SHORT ertglr(VOID);
VOID ertcheck_item(CHAR item[20]);
VOID ertcheck_item2(CHAR item[20]);
VOID ertcheck_item3(CHAR item[20], CHAR* who);
VOID ertcheck_item4(CHAR item[20], CHAR* who);
VOID ert_room_out(VOID);
VOID ert_pwf(struct ertplayer* l_ertply, long auraf);
VOID ert_out_evnt(struct ertplayer* doplyr);
VOID ert_prnt_rm(struct ertroom* rmptr);
VOID ert_prnt_all(VOID);
VOID ert_out_near(VOID);
VOID ert_outfar(VOID);
VOID ert_near_far(struct ertroom* plyroom);
VOID ertleave_room(struct ertplayer* pp, CHAR* xitdesc);
VOID ertgo_room(struct ertplayer* pp, struct ertroom* destrm, CHAR* entdesc);
VOID ertseeexit(VOID);
VOID ert_see_itms(struct ertplayer* pp);
VOID ert_inv_utl(struct ertplayer* ppt);
VOID ert_see_pyrs(struct ertplayer* pp);
VOID ertseemnst(struct ertplayer* pp, SHORT flag);
VOID ertseenlv(struct ertplayer* pp, SHORT flag);
VOID ert_init_see(struct ertplayer* pp);
VOID ert_say_delay(VOID);
VOID ert_whspr(VOID);
VOID ert_del_the(VOID);
VOID ert_del_prep(VOID);
VOID ert_xmit(SHORT msgno);
SHORT seenxnlv(SHORT current);
SHORT ert_nxt_pyr(struct ertplayer* pp);
SHORT ert_num_ply(struct ertroom* ertroom);
SHORT ert_put_ply(struct ertplayer* who, struct item* itmptr);
SHORT ert_itm_rm(struct ertroom* where, struct item* itmptr);
SHORT ertputmnstr(struct ertroom* where, SHORT mnum, SHORT hits);
SHORT ertinv(VOID);
SHORT ertfnlv(CHAR *name);
SHORT ertoutlist(struct ertroom *roompt, USHORT room_to);
LONG ert_see_flg(VOID);
CHAR *ert_nxt_itm(SHORT itmno);
CHAR *ert_and(struct item* itm);
CHAR *ert_aoran(CHAR* noun);
CHAR *ertanmnno(SHORT mnum);
CHAR *ertanmnmn(struct monster* mnstr);
CHAR *ert_nam(CHAR* stg);;
struct ertplayer *ert_see_nxtp(struct ertplayer *pp);
struct item *ert_get_ply(struct ertplayer *who, CHAR *name);
struct item *ert_get_itm(struct ertroom *where, CHAR *name);
struct monster *ertgetmnstr(struct ertroom *where, CHAR *name);
struct item *ert_loc_itm(CHAR *itmnam);
struct ertplayer *ertfplyr(CHAR *name);
struct monster *ertfmnstr(struct ertroom *where, CHAR *name);
struct ertplayer *plyr_ert(CHAR *name);
struct ert_cmd *ertbinary(CHAR *stgptr, struct ert_cmd table[], SHORT length);
VOID ert_parse(VOID);
unsigned ert_rnd(VOID);
