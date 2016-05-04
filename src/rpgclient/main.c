/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Robert Hurst 2014 <robert@hurst-ri.us>
 * 
 * rpgd is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * rpgd is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "all.h"
#include "global.h"

void skillplus(void)
{
	int i;

	Delay(25);
	sprintf(outbuf, "%sYou have earned a gift to give your character:", fore(YELLOW));
	OUT(outbuf); NORMAL;
	NL; Delay(50);
	sprintf(outbuf, "%s %sIncrease maximum strength from %s%u ", bracket(1), fore(BRN), fore(WHITE), PLAYER.MyMaxSTR);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyMaxSTR < 90 ? "[WEAK]" : PLAYER.MyMaxSTR < 95 ? "-Average-"  : PLAYER.MyMaxSTR < 99 ? "*Strong*" : "=Excellent="); NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease maximum intellect from %s%u ", bracket(2), fore(BRN), fore(WHITE), PLAYER.MyMaxINT);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyMaxINT < 90 ? "[STUPID]" : PLAYER.MyMaxINT < 95 ? "-Average-" : PLAYER.MyMaxINT < 99 ? "*Smart*" : "=Excellent="); NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease maximum dexterity from %s%u ", bracket(3), fore(BRN), fore(WHITE), PLAYER.MyMaxDEX);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyMaxDEX < 90 ? "[SLOW]" : PLAYER.MyMaxDEX < 95 ? "-Average-" : PLAYER.MyMaxDEX < 99 ? "*Swift*" : "=Excellent="); NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease maximum charisma from %s%u ", bracket(4), fore(BRN), fore(WHITE), PLAYER.MyMaxCHA);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyMaxCHA < 90 ? "[REPULSIVE]" : PLAYER.MyMaxCHA < 95 ? "-Average-" : PLAYER.MyMaxCHA < 99 ? "*Appealing*" : "=Excellent="); NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease melee skill from %s%u ", bracket(5), fore(BRN), fore(WHITE), PLAYER.MyMelee);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyMelee == 0 ? "[NONE]" : PLAYER.MyMelee == 1 ? "-Average-" : PLAYER.MyMelee == 2 ? "+Good+" : PLAYER.MyMelee == 3 ? "*Powerful*" : "=Excellent="); NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease backstab skill from %s%u ", bracket(6), fore(BRN), fore(WHITE), PLAYER.MyBackstab);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyBackstab == 0 ? "[RARE]" : PLAYER.MyBackstab == 1 ? "-Average-" : PLAYER.MyBackstab == 2 ? "+Good+" : PLAYER.MyBackstab == 3 ? "*Powerful*" : "=Excellent="); NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease poison skill from %s%u ", bracket(7), fore(BRN), fore(WHITE), PLAYER.MyPoison);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MyPoison == 0 ? "[NONE]" : PLAYER.MyPoison == 1 ? "-Average-" : PLAYER.MyPoison == 2 ? "+Good+" : PLAYER.MyPoison == 3 ? "*Powerful*" : "=Excellent="); NL;
	Delay(10);
	if(PLAYER.MyMagic < 2) {
		sprintf(outbuf, "%s %sIncrease magic skill from %s%u ", bracket(8), fore(BRN), fore(WHITE), PLAYER.MyMagic);
		OUT(outbuf); NORMAL;
		OUT(PLAYER.MyMagic == 0 ? "[NONE]" : PLAYER.MyMagic == 1 ? "-Wands-" : "???");
	}
	else {
		sprintf(outbuf, "     %sYour magic skills are %s%u ", fore(BRN), fore(WHITE), PLAYER.MyMagic);
		OUT(outbuf); NORMAL;
		OUT(PLAYER.MyMagic == 2 ? "+Scrolls+" : PLAYER.MyMagic == 3 ? "*Spells*" : "=Enchanted="); NL;
		sprintf(outbuf, "%s %sIncrease maximum mana from %s%u ", bracket(8), fore(BRN), fore(WHITE), PLAYER.SP);
		OUT(outbuf); NORMAL;
	}
	NL;
	Delay(10);
	sprintf(outbuf, "%s %sIncrease stealing skill from %s%u ", bracket(9), fore(BRN), fore(WHITE), PLAYER.MySteal);
	OUT(outbuf); NORMAL;
	OUT(PLAYER.MySteal == 0 ? "[RARE]" : PLAYER.MySteal == 1 ? "-Average-" : PLAYER.MySteal == 2 ? "+Good+" : PLAYER.MySteal == 3 ? "*Masterful*" : "=Excellent="); NL;
	Delay(10);
	i = 0;
	while(i < 1 || i > 9) {
		sprintf(prompt, "%sChoose which: ", fore(CYN));
		OUT(prompt);
		if(ins(1))
		strcpy(inbuf, "5");
		i = atoi(inbuf);
		NL; Delay(5);
	}
	NL; Delay(5);
	switch(i) {
	case 1:
		news("can get even Stronger.");
		PLAYER.MyMaxSTR += 10;
		if(PLAYER.MyMaxSTR > 100)
			PLAYER.MyMaxSTR = 100;
		sprintf(outbuf, "%sMaximum Strength is now %u.", fore(BRED), PLAYER.MyMaxSTR);
		break;
	case 2:
		news("can get even Wiser.");
		PLAYER.MyMaxINT += 10;
		if(PLAYER.MyMaxINT > 100)
			PLAYER.MyMaxINT = 100;
		sprintf(outbuf, "%sMaximum Intellect is now %u.", fore(GREEN), PLAYER.MyMaxINT);
		break;
	case 3:
		news("can get even Quicker.");
		PLAYER.MyMaxDEX += 10;
		if(PLAYER.MyMaxDEX > 100)
			PLAYER.MyMaxDEX = 100;
		sprintf(outbuf, "%sMaximum Dexterity is now %u.", fore(MAGENTA), PLAYER.MyMaxDEX);
		break;
	case 4:
		news("can get even Nicer.");
		PLAYER.MyMaxCHA += 10;
		if(PLAYER.MyMaxCHA > 100)
			PLAYER.MyMaxCHA = 100;
		sprintf(outbuf, "%sMaximum Charisma is now %u.", fore(YELLOW), PLAYER.MyMaxCHA);
		break;
	case 5:
		news("got more Powerful.");
		switch(++PLAYER.MyMelee) {
		case 1:
			sprintf(outbuf, "%sYou can finally enter through Tiny's front door.", fore(CYAN));
			break;
		case 2:
			sprintf(outbuf, "%sSo you want to be a hero, eh?", fore(BLUE));
			break;
		case 3:
			sprintf(outbuf, "%sJust what this world needs, another fighter.", fore(BRED));
			break;
		case 4:
			sprintf(outbuf, "%sWatch out for blasts, you brute!", fore(BRN));
			break;
		case 5:
			sprintf(outbuf, "%sYou and Tiny can go out work-out together.", fore(BLACK));
			break;
		default:
			sprintf(outbuf, "%sThere is none more powerful than you.", fore(BLACK));
			break;
		}
		break;
	case 6:
		news("watch your Back now.");
		switch(++PLAYER.MyBackstab) {
		case 1:
			sprintf(outbuf, "%sA backstab is in your future.", fore(CYAN));
			break;
		case 2:
			sprintf(outbuf, "%sYou will be given a backstab more regularly now.", fore(CYAN));
			break;
		case 3:
			sprintf(outbuf, "%sYou will deal a more significant, first blow.", fore(YELLOW));
			break;
		case 4:
			sprintf(outbuf, "%sEveryone will be watching over their backs now.", fore(BRED));
			break;
		default:
			sprintf(outbuf, "%sDropping any opponent on the first shot is possible.", fore(BLACK));
			break;
		}
		break;
	case 7:
		news("Apothecary visits have more meaning.");
		switch(++PLAYER.MyPoison) {
		case 1:
			sprintf(outbuf, "%sThe Apothecary will see you now, bring money.", fore(CYAN));
			break;
		case 2:
			sprintf(outbuf, "%sYour poisons can achieve 2x its potency now.", fore(CYAN));
			break;
		case 3:
			sprintf(outbuf, "%sYour poisons can achieve 3x its potency now.", fore(YELLOW));
			break;
		case 4:
			sprintf(outbuf, "%sYour poisons can achieve 4x its potency now.", fore(BRED));
			break;
		default:
			sprintf(outbuf, "%sYou have the ability to vaporize your weapon.", fore(BLACK));
			break;

		}
		break;
	case 8:
		news("became more friendly with the old mage.");
		switch(PLAYER.MyMagic) {
		case 0:
			PLAYER.MyMagic++;
			sprintf(outbuf, "%sThe old mage will see you now, bring money.", fore(CYAN));
			break;
		case 1:
			PLAYER.MyMagic++;
			sprintf(outbuf, "%sYour wands have turned into scrolls.", fore(CYAN));
			PLAYER.SP += 15 + dice(500);
			ONLINE->SP = PLAYER.SP;
			break;
		default:
			sprintf(outbuf, "%sMore mana is better.", fore(BLACK));
			PLAYER.SP += 512;
			ONLINE->SP += 512;
			PLAYER.SP += dice(3583 / PLAYER.MyMagic);
			break;
		}
		break;
	case 9:
		news("try to avoid in the Square.");
		switch(++PLAYER.MySteal) {
		case 1:
			sprintf(outbuf, "%sYour fingers are starting to itch.", fore(CYAN));
			break;
		case 2:
			sprintf(outbuf, "%sYour eyes widen at the chance for unearned loot.", fore(CYAN));
			break;
		case 3:
			sprintf(outbuf, "%sWelcome to the Thieves guild: go pick a pocket or two!", fore(BLUE));
			break;
		default:
			sprintf(outbuf, "%sYou convince yourself that no lock cannot be picked.", fore(BLACK));
			break;
		}
		break;
	}
	OUT(outbuf);
	NL; Delay(5);
	NORMAL;
	NL; Delay(5);
}

void assign(int max)
{
	int i,p;
	UBYTE *v,def=0;

	NORMAL;NL;
	while(ONLINE->class_origin==0 && ONLINE->class_type==0) {
		sound2("reroll",0);
		OUT("You have been rerolled.  You must pick a class.");NL;NL;
		reroll(&PLAYER);
		RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
		LoadRPC(ONLINE);
		for(i=0; i<MAXCLASS(0) && strcasecmp(table->class[ONLINE->class_origin]->Character[i]->Name,"Novice"); i++);
		p=i;
		if(PLAYER.Novice!='Y' || p==MAXCLASS(0)) {
			for(i=(p==MAXCLASS(0)) ? 1 : 2; i<MAXCLASS(0); i++) {
				ONLINE->class_type=i;
				sprintf(outbuf,"%s%s  %s",bracket(i-(p==MAXCLASS(0) ? 0 : 1)),fore(WHITE),CLASS(ONLINE)->Name);
				OUT(outbuf);NL;
			}
			sprintf(prompt,"%sEnter class (1-%u): ",fore(CYN),MAXCLASS(0)-(p==MAXCLASS(0) ? 1 : 2));
			NL;OUT(prompt);
			if(ins(2))
				strcpy(inbuf,"1");
			NL;NL;
			i=atoi(inbuf);
			if(p!=MAXCLASS(0) && i>0)
				i++;
		}
		else {
			OUT("Since you are a new user here, you are automatically assigned a character");NL;
			OUT("class.  At the Main Menu, press <Y> to see all your character information.");NL;NL;
			paused();
		}
		ONLINE->class_type=(i>0 && i<MAXCLASS(0)) ? i : FALSE;
		sprintf(PLAYER.Class,"%s.%s",table->class[ONLINE->class_origin]->Origin,CLASS(ONLINE)->Name);
	}
	PLAYER.MyMaxSTR=CLASS(ONLINE)->MaxSTR;
	PLAYER.MyMaxINT=CLASS(ONLINE)->MaxINT;
	PLAYER.MyMaxDEX=CLASS(ONLINE)->MaxDEX;
	PLAYER.MyMaxCHA=CLASS(ONLINE)->MaxCHA;
	PLAYER.MyMelee=CLASS(ONLINE)->Melee;
	PLAYER.MyBackstab=CLASS(ONLINE)->Backstab;
	PLAYER.MyPoison=CLASS(ONLINE)->Poison;
	PLAYER.MyMagic=CLASS(ONLINE)->Magic;
	PLAYER.MySteal=CLASS(ONLINE)->Steal;
	if(PLAYER.MyMagic<2) {
		PLAYER.SP=FALSE;
		PLAYER.Spell=FALSE;
	}
	if(PLAYER.MyPoison<1)
		PLAYER.Poison=FALSE;
	cls();
	sprintf(filename,"pix/Player/%s",CLASS(ONLINE)->Name);
	type_pix(filename,PLAYER.Gender);
	sprintf(filename,"help/%s",CLASS(ONLINE)->Name);
	type(filename,FALSE);
	if(PLAYER.Novice=='Y') {
		PLAYER.STR=CLASS(ONLINE)->BaseSTR;
		PLAYER.INT=CLASS(ONLINE)->BaseINT;
		PLAYER.DEX=CLASS(ONLINE)->BaseDEX;
		PLAYER.CHA=CLASS(ONLINE)->BaseCHA;
		paused();
	}
	else
		do {
			max = (max < 200) ? 200 : (max > 260) ? 260 : max;
			NL;
			sprintf(outbuf,"%sYou have %s%d%s ability points to distribute between 4 abilities: Strength,",fore(BRN),fore(YELLOW),max,fore(BRN));
			OUT(outbuf);NL;
			sprintf(outbuf,"Intellect, Dexterity, and Charisma.  Each ability must be between %s20%s and %s80%s.",fore(YELLOW),fore(BRN),fore(YELLOW),fore(BRN));
			OUT(outbuf);NL;NL;
			p=max;
			v=&PLAYER.STR;
			for(i=0; i<4; i++) {
				if(i>0 && i<3) {
					sprintf(outbuf,"You have %d ability points left.",p);
					OUT(outbuf);NL;NL;
				}
				switch(i) {
					case 0:
						def=CLASS(ONLINE)->BaseSTR;
						sprintf(outbuf,"%sEnter your Strength  %s: ",fore(CYN),bracket(def));
						break;
					case 1:
						def=CLASS(ONLINE)->BaseINT;
						sprintf(outbuf,"%sEnter your Intellect %s: ",fore(CYN),bracket(def));
						break;
					case 2:
						def=CLASS(ONLINE)->BaseDEX;
						if(def+20>p)
							def=p-20;
						sprintf(outbuf,"%sEnter your Dexterity %s: ",fore(CYN),bracket(def));
						break;
					case 3:
						def=p;
						sprintf(outbuf,"%sEnter your Charisma  %s: ",fore(CYN),bracket(def));
						break;
				}
				OUT(outbuf);
				FOREVER {
					if(ins(2))
						sprintf(inbuf,"%u",def);
					v[i]=atoi(inbuf);
					if(!strlen(inbuf)) {
						v[i]=def;
						sprintf(outbuf,"%s%u%s",fore(WHITE),def,fore(GRY));
						OUT(outbuf);
					}
					if(v[i]>=20 && v[i]<=80)
						break;
					eraser();
				}
				NL;NL;
				p-=v[i];
			}
		} while(p);
	RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
	LoadRPC(ONLINE);
}

void beep(void)
{
	OUT("\7");
}

char *back(int code)
{
	static int sp = 0;
	static char result[16][6];

	sp++;
	if(sp > 15)
		sp = 0;
	if(ANSI > 1)
		sprintf(result[sp], "\33[%dm", 40 + (code & 7));
	else
		result[sp][0] = '\0';
	return(result[sp]);
}

char *bracket(int n)
{
	static char r[32];

	sprintf(r, "%s%s<%s%d%s>", (n < 10) ? " " : "", fore(BLACK), fore(WHITE), n, fore(BLACK));
	return(r);
}

void broadcast(char *msg)
{
	UBYTE msgbuf[sizeof(struct user)];

	sprintf((BYTE *)msgbuf, "%s[%s%s%s]%s %s %s", fore(CYN), fore(CYAN), PLAYER.ID, fore(CYN), fore(CYAN), PLAYER.Handle, msg);
	RPGserver(SERVER_BROADCAST, (UBYTE *)msgbuf);
}

void bump(char *where)
{
	if(PLAYER.Gold>0. && ACCESS(acclvl)->RolePlay=='Y' && PLAYER.Novice!='Y') {
		RPGserver(SERVER_RNDUSER,(UBYTE *)&ENEMY);
		if(strcmp(PLAYER.ID,ENEMY.ID) && !strlen(ENEMY.Status)) {
			LoadRPC(RPC[1][0]);		//	if he is smarter than your luck...
			if(dice(ENEMY.Level)*RPC[1][0]->user.MySteal/2>PLAYER.Level && dice(RPC[1][0]->INT)>dice(ONLINE->CHA)) {
				sprintf(outbuf,"You bump into %s on the %s.",ENEMY.Handle,where);
				OUT(outbuf);NL;
				Delay(50);			//	if he is quicker than your attention...
				if(dice(RPC[1][0]->DEX>dice(PLAYER.INT))) {
					ENEMY.Gold+=PLAYER.Gold;
					RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
					numline=0;
					sprintf(line[numline++],"You picked %s's pocket and got %s!",PLAYER.Handle,money(PLAYER.Gold,ENEMY.Emulation));
					note(ENEMY.ID);
					PLAYER.Gold=0.;
				}
				sprintf(outbuf,"You exchange greetings with %s and go on your way.",RPC[1][0]->him);
				OUT(outbuf);NL;
				Delay(50);
				paws=!PLAYER.Expert;
			}
		}
	}
}

int cd(void)
{
	return(logoff);
}

void chkexp(struct RPC *rec)
{
	const int SKILL_LEVEL = 50;
	int a = FALSE, b = FALSE;
	int i, m, n;
	int ahp = 0, asp = 0, astr = rec->user.STR, aint = rec->user.INT, adex = rec->user.DEX, acha = rec->user.CHA;

	a = rec->user.Novice != 'Y' && (rec->user.Level < SKILL_LEVEL);
	n = 0;

	while(rec->user.Experience >= EXP(rec->user.Level) && ACCESS(acclvl)->RolePlay == 'Y') {
		rec->user.Level++; n++;

		if(a && rec->user.Level == SKILL_LEVEL)
			b = TRUE;

		if(ACCESS(rec->user.Access)->Promote == rec->user.Level) {
			rec->user.Access++;
			if(rec == ONLINE) {
				NORMAL;
				sprintf(outbuf, "%sThe king is pleased with your accomplishments and makes you %s%s!", fore(YELLOW), AN(ACCESS(PLAYER.Access)->Name), ACCESS(PLAYER.Access)->Name);
				NL; Delay(5);
				OUT(outbuf);
				NL; Delay(5);
			}
		}
		ahp += rec->user.Level + dice(rec->user.Level) + rec->user.STR / 10 + (rec->user.STR > 90 ? rec->user.STR - 90 : 0);
		if(rec->user.MyMagic > 1)
			asp += rec->user.Level + dice(rec->user.Level) + rec->user.INT / 10 + (rec->user.INT > 90 ? rec->user.INT - 90 : 0);

		i = CLASS(rec)->BonusSTR;
		if(rec->user.STR + i > rec->user.MyMaxSTR)
			if((i = rec->user.MyMaxSTR - rec->user.STR) < 0)
				i = 0;
		if(i) {
			rec->user.STR += i;
			m = rec->user.MyMaxSTR;
			if(strlen(rec->user.Blessed))
				if((m += 10) > 100)
					m = 100;
			if(strlen(rec->user.Cursed))
				m -= 10;
			if(rec->STR + i > m)
				if((i = m - rec->STR) < 0)
					i = 0;
			rec->STR += i;
		}

		i = CLASS(rec)->BonusINT;
		if(rec->user.INT + i > rec->user.MyMaxINT)
			if((i = rec->user.MyMaxINT - rec->user.INT) < 0)
				i = 0;
		if(i) {
			rec->user.INT += i;
			m = rec->user.MyMaxINT;
			if(strlen(rec->user.Blessed))
				if((m += 10) > 100)
					m = 100;
			if(strlen(rec->user.Cursed))
				m -= 10;
			if(rec->INT + i > m)
				if((i = m - rec->INT) < 0)
					i = 0;
			rec->INT += i;
		}

		i = CLASS(rec)->BonusDEX;
		if(rec->user.DEX + i > rec->user.MyMaxDEX)
			if((i = rec->user.MyMaxDEX - rec->user.DEX) < 0)
				i = 0;
		if(i) {
			rec->user.DEX += i;
			m = rec->user.MyMaxDEX;
			if(strlen(rec->user.Blessed))
				if((m += 10) > 100)
					m = 100;
			if(strlen(rec->user.Cursed))
				m -= 10;
			if(rec->DEX + i > m)
				if((i = m - rec->DEX) < 0)
					i = 0;
			rec->DEX += i;
		}

		i = CLASS(rec)->BonusCHA;
		if(rec->user.CHA + i > rec->user.MyMaxCHA)
			if((i = rec->user.MyMaxCHA - rec->user.CHA) < 0)
				i = 0;
		if(i) {
			rec->user.CHA += i;
			m = rec->user.MyMaxCHA;
			if(strlen(rec->user.Blessed))
				if((m += 10) > 100)
					m = 100;
			if(strlen(rec->user.Cursed))
				m -= 10;
			if(rec->CHA + i > m)
				if((i = m - rec->CHA) < 0)
					i = 0;
			rec->CHA += i;
		}
	}

	if(ahp || rec->user.Level > SYSREC->Level) {
		rec->user.ExpLevel = rec->user.Level;
		if(rec == ONLINE) {
			sound("level", 64, 0);
			NL; Delay(5);
			sprintf(outbuf,"      %s-=%s>%s*%s<%s=-", fore(MAG), fore(BLU), fore(YELLOW), fore(BLU), fore(MAG));
			OUT(outbuf);
			NL; Delay(5);
			NL; Delay(5);
			sprintf(outbuf, "%sWelcome to level %hu!", fore(YELLOW), rec->user.Level);
			OUT(outbuf);
			NL; Delay(5);
			NL; Delay(5);
		}
		if(rec->user.Level <= SYSREC->Level) {
			if(ahp) {
				rec->user.HP += ahp;
				rec->HP += ahp;
				if(rec == ONLINE) {
					sprintf(outbuf, "%s%+6d%s Hit points", fore(WHITE), ahp, fore(GRY));
					OUT(outbuf);
					NL; Delay(5);
				}
			}
			if(asp) {
				rec->user.SP += asp;
				rec->SP += asp;
				if(rec == ONLINE) {
					sprintf(outbuf, "%s%+6d%s Spell power", fore(WHITE), asp, fore(GRY));
					OUT(outbuf);
					NL; Delay(5);
				}
			}
			if(rec == ONLINE) {
				if((i = PLAYER.STR - astr)) {
					sprintf(outbuf, "%s%+6d%s Strength", fore(WHITE), i, fore(GRY));
					OUT(outbuf);
					NL; Delay(5);
				}
				if((i = PLAYER.INT - aint)) {
					sprintf(outbuf, "%s%+6d%s Intellect", fore(WHITE), i, fore(GRY));
					OUT(outbuf);
					NL; Delay(5);
				}
				if((i = PLAYER.DEX - adex)) {
					sprintf(outbuf, "%s%+6d%s Dexterity", fore(WHITE), i, fore(GRY));
					OUT(outbuf);
					NL; Delay(5);
				}
				if((i = PLAYER.CHA - acha)) {
					sprintf(outbuf, "%s%+6d%s Charisma", fore(WHITE), i, fore(GRY));
					OUT(outbuf);
					NL; Delay(5);
				}
				NL;
				if(a && b)
					skillplus();
				if(n >= 25)
					skillplus();
			}
			if(strlen(rec->user.ID) && rec->user.ID[0] != '_')
				RPGserver(SERVER_PUTUSER, (UBYTE *)&rec->user);
			if(rec == ONLINE)
				paused();
		}
		else
			if(rec == ONLINE)
				immortalize();
		displayview();
	}
}

void cls(void)
{
	if(paws)
		paused();
	OUT("\33[H\33[J");
}

void displaykeys(int n)
{
	int i;
	char c, keys[4] = { 'P', 'G', 'S', 'C' };

	for(i = 0; i < 3; i++)
		if((c = PLAYER.KeyHints[n][i]))
			switch(c) {
				case 'P':
					keys[0] = '\0';
					break;
				case 'G':
					keys[1] = '\0';
					break;
				case 'S':
					keys[2] = '\0';
					break;
				case 'C':
					keys[3] = '\0';
					break;
			}
	for(i = 0; i < 4; i++) {
		if((c = keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf, "%s{%sP%s}  ", fore(MAG), fore(MAGENTA), fore(MAG));
					break;
				case 'G':
					sprintf(outbuf, "%s{%sG%s}  ", fore(BRN), fore(YELLOW), fore(BRN));
					break;
				case 'S':
					sprintf(outbuf, "%s{%sS%s}  ", fore(CYN), fore(CYAN), fore(CYN));
					break;
				case 'C':
					sprintf(outbuf, "%s{%sC%s}  ", fore(RED), fore(BRED), fore(RED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
	for(i = 0; i < 4; i++) {
		if((c = keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf, " %s|   ", fore(MAG));
					break;
				case 'G':
					sprintf(outbuf, " %s|   ", fore(BRN));
					break;
				case 'S':
					sprintf(outbuf, " %s|   ", fore(CYN));
					break;
				case 'C':
					sprintf(outbuf, " %s|   ", fore(RED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
	for(i = 0; i < 4; i++) {
		if((c = keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf, " %s|%s-  ", fore(MAG), fore(MAGENTA));
					break;
				case 'G':
					sprintf(outbuf, " %s|%s-  ", fore(BRN), fore(YELLOW));
					break;
				case 'S':
					sprintf(outbuf, " %s|%s-  ", fore(CYN), fore(CYAN));
					break;
				case 'C':
					sprintf(outbuf, " %s|%s-  ", fore(RED), fore(BRED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
	for(i = 0; i < 4; i++) {
		if((c = keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf, " %s|%s=  ", fore(MAG), fore(MAGENTA));
					break;
				case 'G':
					sprintf(outbuf, " %s|%s=  ", fore(BRN), fore(YELLOW));
					break;
				case 'S':
					sprintf(outbuf, " %s|%s=  ", fore(CYN), fore(CYAN));
					break;
				case 'C':
					sprintf(outbuf, " %s|%s=  ", fore(RED), fore(BRED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
}

void displayview(void)
{
/*	int i,j,AC,WC;

	if(viewwindow) {
		AC=0;
		for(i=0; i<2; i++)
			for(j=1; j<5; j++)
				if(strlen(RPC[i][j]->user.Handle) && j>AC)
					AC=j;
		ChangeWindowBox(viewwindow,viewwindow->LeftEdge,viewwindow->TopEdge,viewwindow->Width,4*SERCON.TextFont->tf_YSize+AC*(3*SERCON.TextFont->tf_YSize+1));
		Delay(5);
		SetRast(viewwindow->RPort,SERCON.Window->WScreen->BitMap.Depth<4 ? 3 : WHITE);
		SetAPen(viewwindow->RPort,SERCON.Window->WScreen->BitMap.Depth<4 ? 2 : BLU);
		SetDrMd(viewwindow->RPort,JAM1);
		for(j=0; j<5; j++) {
			memset(line[1],0,128);
			memset(line[2],0,128);
			memset(line[3],0,128);
			for(i=0; i<2; i++) {
				if(strlen(RPC[i][j]->user.Handle)) {
					sprintf(outbuf,"%s - %u %s",RPC[i][j]->user.Handle,RPC[i][j]->user.Level,CLASS(RPC[i][j])->Name);
					sprintf(line[0],"%-38.38s",outbuf);
					strcat(line[1],line[0]);
			 		WC=(RPC[i][j]->weapon_origin<NUMWEAPON) ? WEAPON(RPC[i][j])->Class : RPC[i][j]->weapon_type;
			 		AC=(RPC[i][j]->armor_origin<NUMARMOR) ? ARMOR(RPC[i][j])->Class : RPC[i][j]->armor_type;
					sprintf(outbuf,"  WC:%u%+d  AC:%u%+d  HP:%d  SP:%d",WC,RPC[i][j]->user.WCmod+RPC[i][j]->ToWC,AC,RPC[i][j]->user.ACmod+RPC[i][j]->ToAC,RPC[i][j]->HP,RPC[i][j]->SP);
					sprintf(line[0],"%-38.38s",outbuf);
					strcat(line[2],line[0]);
					sprintf(outbuf,"  STR:%u  INT:%u  DEX:%u",RPC[i][j]->STR,RPC[i][j]->INT,RPC[i][j]->DEX);
					sprintf(line[0],"%-38.38s",outbuf);
					strcat(line[3],line[0]);
				}
				else {
					sprintf(line[0],"%-38s","");
					strcat(line[1],line[0]);
					strcat(line[2],line[0]);
					strcat(line[3],line[0]);
				}
			}
			for(i=1; i<=3; i++) {
				Move(viewwindow->RPort,0,j*(3*SERCON.TextFont->tf_YSize+1)+SERCON.TextFont->tf_YSize*i);
				Text(viewwindow->RPort,line[i],strlen(line[i]));
			}
		}
	}
*/
}

void emptykey(void)
{
	Delay(10);
	do {
		INPUT(0);
	} while(source);
	memset(Keybuf, 0, KEYBUF_SIZE);
}

void eraser(void)
{
	int i;

	beep();
	for(i=0; i<strlen(inbuf); i++)
		RUBOUT;
}

void finputs(char *str, int n, FILE *file)
{
	if(!fgets(str, n, file))
		memset(str, 0, n);
	if(str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';
}

char *fore(int code)
{
	static int sp = 0;
	static char result[16][10];

	sp++;
	if(sp > 15)
		sp = 0;
	if(ANSI == 3) {
//		if(code == 8)	// PowerTCP telnet does not support bold colors
//			code = 4;
		if(code & 8)
			sprintf(result[sp], "\33[1;%dm", 30 + (code & 7));
		else
			sprintf(result[sp], "\33[22;%dm", 30 + (code & 7));
	}
	else
	if(ANSI == 2) {
		if(code & 8)
			sprintf(result[sp], "\33[1;%dm", 30 + (code & 7));
		else
			sprintf(result[sp], "\33[22;%dm", 30 + (code & 7));
	}
	else
	if(ANSI == 1) {
		if(code & 8)
			strcpy(result[sp], (code == 8) ? "\33[2m" : "\33[1m");
		else
			strcpy(result[sp], "\33[22m");
	}
	else
		result[sp][0] = '\0';
	return(result[sp]);
}

char getkey(void)
{
	int i;
	char c = '\0';

	//  Ctrl-X yields cancel type ahead
	if(Keybuf[0] == '\30')
		emptykey();

	if(!Keybuf[0] && !logoff) {
		for(i = 1; i <= 4; i <<= 1) {
			INPUT(timedout);
			if(source || cd())
				break;
			beep();
			timedout >>= 1;
		}
		if(i > 4) {
			sprintf(outbuf, "%s*** Inactive Timeout ***%s", fore(YELLOW), fore(GRY));
			NL; NL;
			OUT(outbuf); NL; NL;
			logoff = TRUE;
			strcpy(reason, "went to sleep");
		}
	}
	else
		source = 1;
	if(source == 1 || source == 2) {
		c = Keybuf[0];
		memmove(Keybuf, &Keybuf[1], strlen(Keybuf));
	}
//	sprintf(outbuf," [%d] ",c);
//	OUT(outbuf);
	return(c);
}

int hallcheck(int o,int t)
{
	int h=FALSE,l=FALSE;

	sprintf(outbuf,"%sYou have reached immortality fame list:",fore(YELLOW));
	OUT(outbuf);
	Delay(50);
	if(PLAYER.Current.Calls<=table->class[o]->Character[t]->Calls[0].What) {
		table->class[o]->Character[t]->Calls[0].What=PLAYER.Current.Calls;
		table->class[o]->Character[t]->Calls[0].When=Julian;
		strcpy(table->class[o]->Character[t]->Calls[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...with the least calls %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Calls,fore(CYN));
		NL;OUT(outbuf);
		h=1;
	}
	if(PLAYER.Current.Kills<=table->class[o]->Character[t]->Kills[0].What) {
		table->class[o]->Character[t]->Kills[0].What=PLAYER.Current.Kills;
		table->class[o]->Character[t]->Kills[0].When=Julian;
		strcpy(table->class[o]->Character[t]->Kills[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...with the least kills %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Kills,fore(CYN));
		NL;OUT(outbuf);
		h=1;
	}
	if(PLAYER.Current.Killed<=table->class[o]->Character[t]->Killed[0].What) {
		table->class[o]->Character[t]->Killed[0].What=PLAYER.Current.Killed;
		table->class[o]->Character[t]->Killed[0].When=Julian;
		strcpy(table->class[o]->Character[t]->Killed[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...getting killed the least %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Killed,fore(CYN));
		NL;OUT(outbuf);
		h=1;
	}
	if(PLAYER.Current.Retreats<=table->class[o]->Character[t]->Retreats[0].What) {
		table->class[o]->Character[t]->Retreats[0].What=PLAYER.Current.Retreats;
		table->class[o]->Character[t]->Retreats[0].When=Julian;
		strcpy(table->class[o]->Character[t]->Retreats[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...with the least retreats %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Retreats,fore(CYN));
		NL;OUT(outbuf);
		h=1;
	}
	if(PLAYER.Current.KOs>=table->class[o]->Character[t]->Brawls[0].What) {
		table->class[o]->Character[t]->Brawls[0].What=PLAYER.Current.KOs;
		table->class[o]->Character[t]->Brawls[0].When=Julian;
		strcpy(table->class[o]->Character[t]->Brawls[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...winning the most brawls %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.KOs,fore(CYN));
		NL;OUT(outbuf);
		h=1;
	}
	if(PLAYER.Current.JoustWin>=table->class[o]->Character[t]->Jousts[0].What) {
		table->class[o]->Character[t]->Jousts[0].What=PLAYER.Current.JoustWin;
		table->class[o]->Character[t]->Jousts[0].When=Julian;
		strcpy(table->class[o]->Character[t]->Jousts[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...winning the most jousts %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.JoustWin,fore(CYN));
		NL;OUT(outbuf);
		h=1;
	}
	if(h)
		PutHOF(table);
	else {
		sprintf(outbuf," %sNone.",fore(WHITE));
		OUT(outbuf);
	}
	NL;
	Delay(50);

	sprintf(outbuf,"%sYou have reached immortality lame list:",fore(YELLOW));
	OUT(outbuf);
	Delay(50);
	if(PLAYER.Current.Calls>=table->class[o]->Character[t]->Calls[1].What) {
		table->class[o]->Character[t]->Calls[1].What=PLAYER.Current.Calls;
		table->class[o]->Character[t]->Calls[1].When=Julian;
		strcpy(table->class[o]->Character[t]->Calls[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...with the most calls %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Calls,fore(CYN));
		NL;OUT(outbuf);
		l=1;
	}
	if(PLAYER.Current.Kills>=table->class[o]->Character[t]->Kills[1].What) {
		table->class[o]->Character[t]->Kills[1].What=PLAYER.Current.Kills;
		table->class[o]->Character[t]->Kills[1].When=Julian;
		strcpy(table->class[o]->Character[t]->Kills[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...with the most kills %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Kills,fore(CYN));
		NL;OUT(outbuf);
		l=1;
	}
	if(PLAYER.Current.Killed>=table->class[o]->Character[t]->Killed[1].What) {
		table->class[o]->Character[t]->Killed[1].What=PLAYER.Current.Killed;
		table->class[o]->Character[t]->Killed[1].When=Julian;
		strcpy(table->class[o]->Character[t]->Killed[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...getting killed the most %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Killed,fore(CYN));
		NL;OUT(outbuf);
		l=1;
	}
	if(PLAYER.Current.Retreats>=table->class[o]->Character[t]->Retreats[1].What) {
		table->class[o]->Character[t]->Retreats[1].What=PLAYER.Current.Retreats;
		table->class[o]->Character[t]->Retreats[1].When=Julian;
		strcpy(table->class[o]->Character[t]->Retreats[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...with the most retreats %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Retreats,fore(CYN));
		NL;OUT(outbuf);
		l=1;
	}
	if(PLAYER.Current.KOed>=table->class[o]->Character[t]->Brawls[1].What) {
		table->class[o]->Character[t]->Brawls[1].What=PLAYER.Current.KOed;
		table->class[o]->Character[t]->Brawls[1].When=Julian;
		strcpy(table->class[o]->Character[t]->Brawls[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...losing the most brawls %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.KOed,fore(CYN));
		NL;OUT(outbuf);
		l=1;
	}
	if(PLAYER.Current.JoustLoss>=table->class[o]->Character[t]->Jousts[1].What) {
		table->class[o]->Character[t]->Jousts[1].What=PLAYER.Current.JoustLoss;
		table->class[o]->Character[t]->Jousts[1].When=Julian;
		strcpy(table->class[o]->Character[t]->Jousts[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%s...losing the most jousts %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.JoustLoss,fore(CYN));
		NL;OUT(outbuf);
		l=1;
	}
	if(l)
		PutHOL(table);
	else {
		sprintf(outbuf," %sNone.",fore(WHITE));
		OUT(outbuf);
	}
	NL;

	if(h+l)
		paused();

	return(h+l);
}

void immortalize(void)
{
	FILE *fp;
	int h,i,immortal,j,level,max;
	char c;

	sound2("victory",0);
	if(PLAYER.Novice=='Y') {
		PLAYER.Novice='N';
		sprintf(outbuf,"%sYou are no longer a novice player.  Welcome to the next level of play.",fore(YELLOW));
		OUT(outbuf);NL;NL;
		paused();
	}
	h=1;
	sprintf(outbuf,"%sChecking your statistics against All-Time Fame/Lame lists...",fore(CYN));
	OUT(outbuf);NL;
	if(hallcheck(0,0))
		h++;
	sprintf(outbuf,"%sChecking your statistics against the %s Fame/Lame lists...",fore(CYN),CLASS(ONLINE)->Name);
	OUT(outbuf);NL;
	if(hallcheck(ONLINE->class_origin,ONLINE->class_type))
		h++;

	memset(PLAYER.Email, 0, sizeof(PLAYER.Email));
	PLAYER.Immortal++;
	OUT("You have become so powerful that you are now immortal and you leave your ");NL;
	OUT("worldly possessions behind.");NL;
	IRSREC->Bank+=PLAYER.Bank+PLAYER.Gold;
	RPGserver(SERVER_PUTUSER,(UBYTE *)IRSREC);

	memset(&ENEMY,0,sizeof(struct user));
	while(RPGserver(SERVER_NEXTUSER,(UBYTE *)&ENEMY)) {
		if(ENEMY.Bounty>0. && !strcmp(PLAYER.ID,ENEMY.BountyWho)) {
			ENEMY.Bounty=0.;
			memset(ENEMY.BountyWho,0,sizeof(ENEMY.BountyWho));
			RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
		}
	}
	sprintf(prompt,"%sPress RETURN to start over in this world: ",fore(CYN));
	OUT(prompt);
	ins(1);NL;NL;

	for(immortal=table->NumClass-1; immortal>0 && strcasecmp("IMMORTAL",table->class[immortal]->Origin); immortal--);
	max=(strcasecmp(table->class[ONLINE->class_origin]->Origin,"IMMORTAL")) ? FALSE : ONLINE->class_type+1;
	h+=max;
	if(max==table->class[ONLINE->class_origin]->NumCharacter) {
		sound("cheer",64,0);
		sprintf(outbuf,"%sCONGRATULATIONS!!%s  You have won the game!",fore(YELLOW),fore(GRY));
		OUT(outbuf);NL;
		Delay(100);
		PLAYER.Wins++;
		RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
		if((fp=fopen("msg/Winners","a"))) {
			fdate(dated,Julian);
			fprintf(fp,"%s  %-22.22s  (%lu days)\n",dated,PLAYER.Handle,PLAYER.LastDate-(PLAYER.CreateDate>SYSREC->BirthDate ? PLAYER.CreateDate : SYSREC->BirthDate)+1);
			fclose(fp);
			OUT("Your character has now joined the Winner's List");NL;
			Delay(100);
		}
		sprintf(outbuf,"%sThe board will now be reset...%s",fore(BRN),fore(YELLOW));
		OUT(outbuf);
		RPGserver(SERVER_WONGAME,(UBYTE *)&PLAYER);
		OUT("Happy Hunting!");NL;
		Delay(100);
		SYSREC->BirthDate=Julian+1;
		SYSREC->Gold=0.;
		RPGserver(SERVER_PUTUSER,(UBYTE *)SYSREC);
		IRSREC->Bank=10000.;
		RPGserver(SERVER_PUTUSER,(UBYTE *)IRSREC);
		ReKeySequence(&PLAYER);
		reroll(&PLAYER);
		RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
		strcpy(reason,"won the game!");
		for(i=0; i<5; i++)
			if(strcasecmp(callers.Last[i],PLAYER.Handle)==0) {
				strcpy(callers.Reason[i],reason);
				break;
			}
		logoff = TRUE;
		return;
	}

	OUT("Ol' Mighty One!  Solve the Ancient Riddle of the Keys and you will become");NL;
	OUT("an immortal being.");NL;
	for(i=0; i<4; i++)
		for(j=0; j<3; j++)
			if((c=PLAYER.KeyHints[i][j])) {
				if(strchr("CSGP",c)) {
					sprintf(outbuf,"Key #%d is NOT %s.",i+1,c=='C' ? "Copper" : c=='S' ? "Silver" : c=='G' ? "Gold" : "Platinum");
					OUT(outbuf);NL;
				}
				else
					PLAYER.KeyHints[i][j]='\0';
			}
	for(i=0; i<h; i++)
		keyhint();

	NL;
	level=0;
	reroll(&PLAYER);
	LoadRPC(ONLINE);
	while(!cd() && level<=max) {
		displaykeys(level);
		sprintf(prompt,"%sInsert key #%d? ",fore(CYN),level+1);
		do {
			NL;OUT(prompt);
			c=inkey('?','\0');
		} while(!strchr("CSGP",c));
		OUT("...you insert and twist the key...");
		Delay(100);
		if(PLAYER.KeySequence[level]==c) {
			sound("max",64,0);
			sprintf(outbuf,"%s{%sClick!%s}",fore(CYN),fore(CYAN),fore(CYN));
			OUT(outbuf);NL;
			sprintf(PLAYER.Class,"%s.%s",table->class[immortal]->Origin,table->class[immortal]->Character[level]->Name);
			sprintf(outbuf,"You are now %s%s.",AN(table->class[immortal]->Character[level]->Name),table->class[immortal]->Character[level]->Name);
			OUT(outbuf);NL;
			LoadRPC(ONLINE);
			level++;
			paws=!PLAYER.Expert;
			for(i=level; i<4; i++)
				for(j=0; j<3; j++)
					if(!PLAYER.KeyHints[i][j] || !strchr("CSGP",PLAYER.KeyHints[i][j])) {
						PLAYER.KeyHints[i][j]=c;
						break;
					}
		}
		else {
			sound("thunder",64,0);
			sprintf(outbuf,"%s^%sBoom!%s^",fore(BLACK),fore(WHITE),fore(BLACK));
			OUT(outbuf);NL;
			for(i=0; i<3; i++)
				if(!PLAYER.KeyHints[level][i] || !strchr("CSGP",PLAYER.KeyHints[level][i])) {
					PLAYER.KeyHints[level][i]=c;
					break;
				}
			break;
		}
	}
	if(level)
		ReKeySequence(&PLAYER);
	if((max = 200 + dice(PLAYER.Immortal)) > 240)
		max = 240;
	max += 10 * PLAYER.Wins;
	assign(max);
	PLAYER.CallsToday=0;
	sprintf(outbuf,"%sYou are rewarded with %u more calls for today.",fore(YELLOW),ACCESS(PLAYER.Access)->Calls);
	OUT(outbuf);NL;
	NORMAL;NL;
	strcpy(reason,"became immortal");
	logoff = TRUE;
}

char inkey(char def, char loss)
{
	char str[18];
	char c, esc = FALSE;

	if(def == 'Y' || loss == 'Y' || def == 'N' || loss == 'N')
		emptykey();

	timedout = 240;
	do {
		c = getkey();
		c = toupper(c);
		if(c == '\15' && def)
			c = def;
		else
		if(cd()) {
			c = loss;
			strcpy(reason, "lost carrier");
			logoff = TRUE;
			break;
		}
		else
		if(c == '\30') {
			c = '\0';
			emptykey();
		}
		else
		if(c == '\33' || c == '[' || c == '\233') {
			c = '\0';
			esc = TRUE;
		}
		else
		if(c < ' ' || c > 'Z')
			c = '\0';
		else
		if(esc)
			c = (c == 'O') ? '\0' : (c == 'A') ? 'N' : (c == 'B') ? 'S' : (c == 'C') ? 'E' : (c == 'D') ? 'W' : c;
	} while(!c);

	sprintf(str, "%s%c%s", fore(WHITE), c, fore(GRY));
	OUT(str);

	return(c);
}

void input(BYTE *buffer, UBYTE *source, ULONG num, ULONG t)
{
	fd_set rfds;
	struct timeval tv;
	int i;
	char c;

	memset(buffer, 0, num);
	*source = 1;
	c = '\15';
	i = 0;

	do {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		tv.tv_sec = t;
		tv.tv_usec = 50;
		if(!select(1, &rfds, NULL, NULL, &tv)) {
			*source = 0;
			break;
		}
		i++;
		fread(&c, 1, 1, stdin);
	} while(c < 1 && i < 13);
	*buffer = (i < 13) ? c : i;
}

int ins(int max)
{
	int escape,i;
	char c[2]={ '\0','\0' };

	if(!timedout)
		timedout=240;
	OUT(fore(WHITE));
	memset(inbuf,0,INBUF_SIZE);
	escape=FALSE;
	while(!escape) {
	 	if((c[0]=getkey())<1) {
	 		c[0]=-1;
	 		break;
	 	}
		switch(c[0]) {
			case 8:
				if((i=strlen(inbuf))) {
					inbuf[i-1]='\0';
					RUBOUT;
				}
				break;
			case 13:
 				c[0]='\0';
 				escape=TRUE;
				break;
			case 24:
				for(i=strlen(inbuf); i>0; i--)
					RUBOUT;
				inbuf[0]='\0';
				break;
			case 127:
				if((i=strlen(inbuf))) {
					inbuf[i-1]='\0';
					RUBOUT;
				}
				break;
			default:
				if((i=strlen(inbuf))<max && c[0]>=' ') {
					strcat(inbuf,c);
					if(!myecho)
						c[0]='.';
					OUT(c);
				}
				else
					beep();
				break;
		}
	}
	OUT(fore(GRY));
	if(cuss(inbuf)) {
		logoff = TRUE;
		memset(inbuf,0,INBUF_SIZE);
		beep();
		NL;
		sprintf(outbuf,"%sSwearing is not allowed here!",fore(YELLOW));
		OUT(outbuf);NORMAL;NL;
		strcpy(reason,"potty mouth");
	}
	i=c[0];
	timedout=0;
	return(i);
}

void jazz(char *str)
{
	int i,j,n;
	char cc[10],ca[4];

	for(i=0; i<strlen(str); i++) {
		if(str[i]=='\\') {
			n=0;
			for(j=i+1; j<strlen(str) && isdigit(str[j]); j++) {
				n*=10;
				n+=(str[j]-'0');
			}
			if(n%10<8) {
				strcpy(cc,"\33[");
				if(n/10%10)
					strcat(cc,"1;");
				else
					strcat(cc,"0;");
				sprintf(ca,"%dm",30+(n%10));
				strcat(cc,ca);
				for(n=i; n<j; n++)
					memmove(&str[i],&str[i+1],strlen(str)-i);
				memmove(&str[i+strlen(cc)],&str[i],strlen(str)-i+1);
				strncpy(&str[i],cc,strlen(cc));
			}
		}
	}
}

void keyhint(void)
{
	static BYTE *keys[] = { "Copper", "Silver", "Gold", "Platinum" };
	int i, j = 3, k, l;
	char c;

	for(i = 0; i < 4; i++)
		if(!strchr("CSGP", PLAYER.KeySequence[i]))
			ReKeySequence(&PLAYER);

	for(k = 0; k < ONLINE->CHA && j == 3; k++) {
		i = dice(4) - 1;
		for(j = 0; j < 3; j++)
			if(!PLAYER.KeyHints[i][j] || !strchr("CSGP", PLAYER.KeyHints[i][j])) {
				PLAYER.KeyHints[i][j] = '\0';
				break;
			}
	}
	if(k == ONLINE->CHA)
		return;
	do {
		do {
			l = dice(4) - 1;
			c = *keys[l];
		} while(c == PLAYER.KeySequence[i]);
		for(k = 0; k < j && PLAYER.KeyHints[i][k] != c; k++);
		if(k == j)
			PLAYER.KeyHints[i][j] = c;
	} while(!PLAYER.KeyHints[i][j]);
	sprintf(outbuf, "Key #%d is NOT %s", i + 1, keys[l]);
	OUT(outbuf); NL;
}

char *money(double n,int a)
{
	static char fnum[50];
	double r, s;
	int h, i;

	h = ANSI;
	ANSI = a;
	modf(n, &r);
	if(n < 1e+05)
		sprintf(fnum, "%s%.5g%sc", fore(WHITE), r, fore(RED));
	else
	if(n < 1e+09) {
		modf(n / 1e+05, &r);
		sprintf(fnum, "%s%.5g%ss", fore(WHITE), r, fore(CYAN));
		modf(n - r * 1e+05, &s);
		if(s > 0.) {
			i = strlen(fnum);
			sprintf(&fnum[i], "%s,%.5g%sc", fore(WHITE), s, fore(RED));
		}
	}
	else
	if(n < 1e+13) {
		modf(n/1e+09, &r);
		sprintf(fnum, "%s%.4g%sg", fore(WHITE), r, fore(YELLOW));
		modf((n - r * 1e+09) / 1e+05, &s);
		if(s > 0.) {
			i = strlen(fnum);
			sprintf(&fnum[i], "%s,%.5g%ss", fore(WHITE), s, fore(CYAN));
		}
	}
	else {
		modf(n / 1e+13, &r);
		sprintf(fnum, "%s%.8g%sp", fore(WHITE), r, fore(MAGENTA));
		modf((n - r * 1e+13) / 1e+09, &s);
		if(s > 0.) {
			i = strlen(fnum);
			sprintf(&fnum[i], "%s,%.5g%sg", fore(WHITE), s, fore(YELLOW));
		}
	}
	strcat(fnum, fore(GRY));
	ANSI = h;
	return(fnum);
}

char more(void)
{
	char c;

	NORMAL;REVERSE;
	OUT("-more-");
	NORMAL;
	do {
		c=inkey('Y','N');
		c=(c==' ') ? 'Y' : (c=='Q') ? 'N' : c;
	} while(c!='N' && c!='Y');
	SAMELINE;CLL;
	return(c);
}

void news(char *item)
{
	FILE *fp;

	sprintf(filename,"tmp/%s",PLAYER.ID);
	if((fp=fopen(filename,"a"))) {
		fprintf(fp,"    %s\33[0;37m\n",item);
		fclose(fp);
	}
}

void note(char *who)
{
	FILE *fp;
	int i,j;
	char timestamp[20];
	char *p;

	if(strcmp(PLAYER.ID,who) && *who!='_') {
		sprintf(filename,"log/%s",who);
		if((fp=fopen(filename,"a"))) {
			GetSystemDate(Today);
			GetSystemTime(Time);
			julian(&Julian,Today);
			fdate(timestamp,Julian);
			timestamp[10]=' ';
			ftime(&timestamp[11],Time);
			for(i=0; i<numline; i++) {
				p=line[i];
				if(strlen(p)>60) {
					for(j=60; j>0 && p[j]!=' '; j--);
					p[j++]='\0';
					strcpy(outbuf,&p[j]);
				}
				else
					outbuf[0]='\0';
				fprintf(fp,"%18.18s  %s\n",timestamp,p);
				memset(timestamp,0,sizeof(timestamp));
				if(strlen(outbuf))
					fprintf(fp,"                    %s\n",outbuf);
			}
			fprintf(fp,"\n");
			fclose(fp);
		}
	}
}

char option(struct RPGmenu *RPGmenu)
{
	static char c;
	char menufile[41];
	int i;

	title(RPGmenu->option);
	chkexp(ONLINE);
	if(ONLINE->HP<1) {
		if(!strlen(reason))
			strcpy(reason,"fought bravely?");
		logoff = TRUE;
	}
	timeleft();
	if(cd())
		return('Q');
	if(!PLAYER.Expert || c=='?') {
		cls();
		sprintf(menufile,"menu/%s",RPGmenu->option);
		if(!type(menufile,FALSE)) {
			for(i=0; i<RPGmenu->items; i++) {
				sprintf(outbuf,"%s<%s%c%s>%s %s",fore(RPGmenu->color),fore(WHITE),RPGmenu->item[i].choice,fore(RPGmenu->color),fore(CYN),RPGmenu->item[i].name);
				OUT(outbuf);NL;
			}
		}
	}
	RPGserver(SERVER_UNLOCK,NULL);
	if(!strcasecmp(RPGmenu->option,"main")) {
		sprintf(outbuf,"%sTime Left:%s %d min.",fore(MAG),fore(GRY),timeleft());
		NL;OUT(outbuf);
	}
	sprintf(prompt,"%s[%s%s%s]%s Option (Q=Quit): ",fore(RPGmenu->color),fore(YELLOW),RPGmenu->option,fore(RPGmenu->color),fore(CYN));
	NL;OUT(prompt);
	while((c=inkey('?','Q'))) {
		for(i=0; i<RPGmenu->items && c!=RPGmenu->item[i].choice; i++);
		if(i==RPGmenu->items && c!='?' && c!='Q') {
			beep();
			RUBOUT;
		}
		else
			break;
	}
	NL;NL;
	return(c);
}

void output(void *str)
{
	struct sockaddr_un rpgclient;
	int sockfd,l;

	if(sigtrap[0]!='?') {
		printf(str);
		fflush(stdout);
	}
	if((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) >= 0) {
		sprintf(rpgclient.sun_path, "tmp/rpgwatch");
		rpgclient.sun_family = PF_UNIX;
		if(connect(sockfd, (struct sockaddr *)&rpgclient, strlen(rpgclient.sun_path) + sizeof(rpgclient.sun_family)) >= 0) {
			l = strlen(str);
			write(sockfd, PLAYER.ID, sizeof(PLAYER.ID));
			write(sockfd, &l, sizeof(l));
			write(sockfd, str, l);
		}
		shutdown(sockfd, 3);
		close(sockfd);
	}
}

void paused(void)
{
	char c;

	NORMAL; REVERSE;
	OUT("-pause-");
	NORMAL;
	timedout = 240;
	c = getkey();
	SAMELINE; CLL;
	paws = FALSE;
}

void plot(int row, int col)
{
	BYTE str[11];

	sprintf(str, "\33[%d;%dH", row, col);
	output(str);
}

void sndplay(char *volume, char *sndfile)
{
	pid_t pid;

	pid = fork();
	if(pid == 0) {
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
		execlp("play", volume, sndfile, NULL);
		_exit(EXIT_FAILURE);
	}
}

void sndplay2(char *sndfile)
{
	static pid_t pid;
	struct passwd *passwd;
	uid_t newuid;

	if(pid)
		kill(pid, SIGHUP);

	pid = fork();
	memset(sigtrap, 0, sizeof(sigtrap));
	signal(SIGHUP, (sighandler_t)SIG_DFL);
	signal(SIGINT, (sighandler_t)SIG_DFL);
	signal(SIGIOT, (sighandler_t)SIG_DFL);
	signal(SIGPIPE, (sighandler_t)SIG_DFL);
	signal(SIGQUIT, (sighandler_t)SIG_DFL);
	signal(SIGTERM, (sighandler_t)SIG_DFL);
	atexit((void *)_exit);
	if(pid == 0) {
		if((passwd = getpwnam("root"))) {
			newuid = passwd->pw_uid;
			if(!seteuid(newuid)) {
				fclose(stdin);
				fclose(stdout);
				fclose(stderr);
				execlp("timidity", sndfile, NULL);
				_exit(EXIT_FAILURE);
			}
		}
	}
}

void sound(char *name,int vol,int sync)
{
	FILE *fp;
	double vold;

	if(vol > 0) {
		if(local) {
			sprintf(filename, "snd/%s.wav", name);
			if((fp = fopen(filename, "rb"))) {
				fclose(fp);
				vold = vol / 64.;
				sprintf(line[0], "-v%f", vold);
				sprintf(line[1], "%s", filename);
				sndplay((char *)line[0], (char *)line[1]);
			}
		}
		else {
			if(ANSI == 3) {
				Delay(10);
				sprintf(filename, "\33[[%s;%d]", name, sync);
				OUT(filename);
				Delay(10);
			}
		}
	}
}

void sound2(char *name,int loop)
{
	FILE *fp;

	if(local) {
		sprintf(filename, "snd/%s.mid", name);
		if((fp = fopen(filename, "rb"))) {
			fclose(fp);
			sprintf(line[1], "%s", filename);
			sndplay2((char *)line[1]);
		}
	}
	else if(ANSI == 3) {
		Delay(10);
		sprintf(filename, "\33[{%s;%d}", name, loop);
		OUT(filename);
		Delay(10);
	}
}

int timeleft(void)
{
	int result;

	GetSystemDate(Today);
	GetSystemTime(Time);
	julian(&Julian,Today);

	if(ACCESS(PLAYER.Access)->Sysop=='Y')
		result=1440;
	else
		result=ACCESS(acclvl)->Minutes-(1440*(Julian-PLAYER.LastDate)+(atol(Time)/10000*60+(atol(Time)/100)%100-PLAYER.LastTime/100*60-PLAYER.LastTime%100));
	if(result<0 && !cd()) {
		beep();NL;
		sprintf(outbuf,"%sYour %d minute time limit has expired!",fore(YELLOW),ACCESS(acclvl)->Minutes);
		OUT(outbuf);NL;
		NORMAL;NL;
		logoff = TRUE;
		strcpy(reason,"was hogging the internet");
	}
	return(result);
}

void title(char *str)
{
	static char title[96];

	strcpy(title,"rpgd");
	if(strlen(PLAYER.Name)) {
		sprintf(outbuf," | %.30s",PLAYER.Name);
		strcat(title,outbuf);
	}
	strcat(title," | ");
	strcat(title,str);
}

int type(char *name,int paws)
{
	FILE *fp;
	int lp;
	char c;

	sprintf(inbuf,"%s%s",name,ANSI==1 ? "" : ".IBM");
	if(!(fp=fopen(inbuf,"rb"))) {
		if(!(fp=fopen(name,"rb")))
			return(FALSE);
	}
	c='Y';
	lp=0;
	NORMAL;
	while(!feof(fp) && c!='N') {
		if(++lp%rows) {
			if(fgets(inbuf,INBUF_SIZE,fp)) {
				OUT(inbuf);
				OUT("\15");
			}
		}
		else {
			if((c=more())=='Y')
				paws=TRUE;
			else
				paws=FALSE;
		}
	}
	fclose(fp);
	NORMAL;
	if(paws)
		paused();
	return(TRUE);
}

int type_pix(char *name,char sex)
{
	FILE *fp;

	sprintf(inbuf,"%s%s",name,ANSI==1 ? "" : ".IBM");
	if(!(fp=fopen(inbuf,"rb"))) {
		if(ANSI>1) {
			sprintf(inbuf,"%s.IBM",name);
			fp=fopen(inbuf,"rb");
		}
	}
	if(!fp)
		if(!(fp=fopen(name,"rb")))
			return(FALSE);

	NORMAL;
	while(!feof(fp)) {
		if(fgets(inbuf,INBUF_SIZE,fp)) {
			OUT(inbuf);
			OUT("\15");
		}
	}
	fclose(fp);
	NORMAL;
	return(TRUE);
}

DOUBLE value(DOUBLE cost,UBYTE charisma)
{
	double r;

	modf(cost * charisma / 100., &r);
	return(r);
}

int GetRPC(struct RPC *rec)
{
	int i, hi, lo, result;
	char c, saveprompt[80];

	strcpy(saveprompt, prompt);
	memset(rec, 0, sizeof(struct RPC));
	FOREVER {
		RPGserver(SERVER_UNLOCK, NULL);
		myecho = TRUE;
		result = FALSE;
		OUT(saveprompt);
		if(ins(sizeof(PLAYER.Handle)))
			break;
		NL;
		if(*inbuf == '?' && PLAYER.Level) {
			lo = (PLAYER.Level > 3) ? PLAYER.Level - 3 : 1;
			sprintf(prompt, "%sStarting level %s%s: ", fore(CYN), bracket(lo), fore(CYN));
			OUT(prompt);
			if(ins(2))
				break;
			if(!strlen(inbuf)) {
				sprintf(outbuf, "%s%d", fore(WHITE), lo);
				OUT(outbuf);
			}
			else
				lo = atoi(inbuf);
			NL;
			hi = (PLAYER.Level < 97) ? PLAYER.Level + 3 : 99;
			sprintf(prompt, "%s  Ending level %s%s: ", fore(CYN), bracket(hi), fore(CYN));
			OUT(prompt);
			if(ins(2))
				break;
			if(!strlen(inbuf)) {
				sprintf(outbuf, "%s%d", fore(WHITE), hi);
				OUT(outbuf);
			}
			else
				hi = atoi(inbuf);
			NL;
			RPGserver(SERVER_GETHISCORE, (UBYTE *)table->HiScore);
			sprintf(outbuf, "%s%s ID   Player's Handle           Class    Lvl    Last On     Access Level  %s", back(BLU), fore(WHITE), back(BLK));
			OUT(outbuf); NL;
			sprintf(outbuf, "%s%s--------------------------------------------------------------------------%s", back(BLU), fore(GRY), back(BLK));
			OUT(outbuf);
			NORMAL; NL;
			i = 2;
			sprintf(userlevel.Current, "%03u", hi + 1);
			while(RPGserver(SERVER_GETUSERLEVEL, (UBYTE *)&userlevel)) {
				if(atoi(userlevel.Current) < lo || atoi(userlevel.Current) > hi)
					break;
				if(++i % rows == 0) {
					i++;
					if((c = more()) == 'N')
						break;
				}
				strcpy(rec->user.ID, userlevel.ID);
				RPGserver(SERVER_GETUSER, (UBYTE *)&rec->user);
				LoadRPC(rec);
				fdate(dated, rec->user.LastDate);
				sprintf(outbuf, "%s%-4.4s  %-22s  %-9.9s  %3u  %-11.11s  %-15.15s",
					(strlen(rec->user.Status) ? fore(BLACK)
					 : (strncmp(rec->user.Handle, table->HiScore->Who[0], sizeof(rec->user.Handle)) == 0) ? fore(MAGENTA)
					 : (strncmp(rec->user.ID, PLAYER.ID, sizeof(rec->user.ID)) ? fore(GRY)
					 : fore(CYAN))),
					rec->user.ID, rec->user.Handle,
					CLASS(rec)->Name, rec->user.Level,
					&dated[4], ACCESS(rec->user.Access)->Name);
				OUT(outbuf);
				NORMAL; NL;
			}
			NL;
			continue;
		}
		if(strlen(inbuf) < sizeof(PLAYER.ID)) {
			strupr(inbuf);
			strcpy(rec->user.ID, inbuf);
			if(*inbuf == '_' && ACCESS(PLAYER.Access)->Sysop != 'Y')
				continue;
			if(RPGserver(SERVER_GETUSER, (UBYTE *)&rec->user))
				result = TRUE;
		}
		if(!result) {
			proper(inbuf);
			strcpy(username.Handle, inbuf);
			if(RPGserver(SERVER_GETUSERNAME, (UBYTE *)&username)) {
				strcpy(rec->user.ID, username.ID);
				if(RPGserver(SERVER_GETUSER, (UBYTE *)&rec->user))
					result = TRUE;
			}
		}
//	removed 2-Aug-2012
//		if(result)
//			if(rec != ONLINE)
//				if(!RPGserver(SERVER_LOCK, (UBYTE *)rec->user.ID))
//					continue;
		break;
	}
	NL;
	if(result)
		LoadRPC(rec);
	else
		memset(rec, 0, sizeof(struct RPC));
	displayview();
	return(result);
}

void LoadRPC(struct RPC *rpc)
{
	char origin[9], name[32];

	OriginName(rpc->user.Class,origin, name);
	for(rpc->class_origin = NUMCLASS - 1; rpc->class_origin > 0 && strcasecmp(origin, table->class[rpc->class_origin]->Origin); rpc->class_origin--);
	for(rpc->class_type = MAXCLASS(rpc->class_origin) - 1; rpc->class_type > 0 && strcasecmp(name,CLASS(rpc)->Name); rpc->class_type--);
 
	OriginName(rpc->user.Weapon,origin, name);
	for(rpc->weapon_origin = 0; rpc->weapon_origin < NUMWEAPON && strcasecmp(origin, table->weapon[rpc->weapon_origin]->Origin); rpc->weapon_origin++);
	if(rpc->weapon_origin < NUMWEAPON)
		for(rpc->weapon_type = 0; rpc->weapon_type < table->weapon[rpc->weapon_origin]->NumWeapon && strcasecmp(name, WEAPON(rpc)->Name); rpc->weapon_type++);
	else
		rpc->weapon_type = atoi(name);

	OriginName(rpc->user.Armor, origin,name);
	for(rpc->armor_origin = 0; rpc->armor_origin < NUMARMOR && strcasecmp(origin, table->armor[rpc->armor_origin]->Origin); rpc->armor_origin++);
	if(rpc->armor_origin < NUMARMOR)
		for(rpc->armor_type = 0; rpc->armor_type<table->armor[rpc->armor_origin]->NumArmor && strcasecmp(name,ARMOR(rpc)->Name); rpc->armor_type++);
	else
		rpc->armor_type = atoi(name);

	if(rpc == ONLINE)
		switch(rpc->user.Gender) {
			case 'M':
				rpc->He = "You";
				rpc->His = "Your";
				rpc->he = "you";
				rpc->his = "your";
				rpc->him = "you";
				break;
			case 'F':
				rpc->He = "You";
				rpc->His = "Your";
				rpc->he = "you";
				rpc->his = "your";
				rpc->him = "you";
				break;
			default:
				rpc->He = "You";
				rpc->His = "Your";
				rpc->he = "you";
				rpc->his = "your";
				rpc->him = "you";
				break;
		}
	else
		switch(rpc->user.Gender) {
			case 'M':
				rpc->He = "He";
				rpc->His = "His";
				rpc->he = "he";
				rpc->his = "his";
				rpc->him = "him";
				break;
			case 'F':
				rpc->He = "She";
				rpc->His = "Her";
				rpc->he = "she";
				rpc->his = "her";
				rpc->him = "her";
				break;
			default:
				rpc->He = "It";
				rpc->His= "Its";
				rpc->he = "it";
				rpc->his = "its";
				rpc->him ="it";
				break;
		}
	rpc->BP = rpc->user.HP / 10;
	rpc->HP = rpc->user.HP;
	rpc->SP = rpc->user.SP;
	rpc->HULL = rpc->user.Hull;
	rpc->STR = rpc->user.STR;
	rpc->INT = rpc->user.INT;
	rpc->DEX = rpc->user.DEX;
	rpc->CHA = rpc->user.CHA;
	rpc->ToAC = 0;
	rpc->ToWC = 0;
	if(strlen(rpc->user.Blessed)) {
		if((rpc->STR += 10) > 100)
			rpc->STR = 100;
		if((rpc->INT += 10) > 100)
			rpc->INT = 100;
		if((rpc->DEX += 10) > 100)
			rpc->DEX = 100;
		if((rpc->CHA += 10) > 100)
			rpc->CHA = 100;
	}
	if(strlen(rpc->user.Cursed)) {
		if((BYTE)(rpc->STR -= 10) < 1)
			rpc->STR = 10;
		if((BYTE)(rpc->INT -= 10) < 1)
			rpc->INT = 10;
		if((BYTE)(rpc->DEX -= 10) < 1)
			rpc->DEX = 10;
		if((BYTE)(rpc->CHA -= 10) < 1)
			rpc->CHA = 10;
	}
	rpc->Confused=FALSE;
	//  added 22-Jan-2005 RH
	if(rpc->user.MyMaxSTR > 100)
		rpc->user.MyMaxSTR = CLASS(rpc)->MaxSTR;
	if(rpc->user.MyMaxINT > 100)
		rpc->user.MyMaxINT = CLASS(rpc)->MaxINT;
	if(rpc->user.MyMaxDEX > 100)
		rpc->user.MyMaxDEX = CLASS(rpc)->MaxDEX;
	if(rpc->user.MyMaxCHA > 100)
		rpc->user.MyMaxCHA = CLASS(rpc)->MaxCHA;
	if(rpc->user.MyMelee > 8)
		rpc->user.MyMelee = CLASS(rpc)->Melee;
	if(rpc->user.MyBackstab > 8)
		rpc->user.MyBackstab = CLASS(rpc)->Backstab;
	if(rpc->user.MyPoison > 8)
		rpc->user.MyPoison = CLASS(rpc)->Poison;
	if(rpc->user.MyMagic > 4)
		rpc->user.MyMagic = CLASS(rpc)->Magic;
	if(rpc->user.MySteal > 8)
		rpc->user.MySteal = CLASS(rpc)->Steal;
}

int RPGserver(char command, UBYTE *p)
{
	struct sockaddr_un rpgd;
	pid_t procid;
	int sockfd,l;
	UBYTE flag;

	if((sockfd=socket(PF_UNIX,SOCK_STREAM,0))<0) {
		NL;beep();
		sprintf(outbuf,"%srpgd socket error.",fore(BRED));
		OUT(outbuf);NL;
		NORMAL;NL;
		flag=FALSE;
		logoff=TRUE;
		return(flag);
	}
	strcpy(rpgd.sun_path, "tmp/rpgd");
	rpgd.sun_family = PF_UNIX;
	if(connect(sockfd, (struct sockaddr *)&rpgd, strlen(rpgd.sun_path) + sizeof(rpgd.sun_family)) < 0) {
		close(sockfd);
		ANSI = 2;
		NL; beep();
		sprintf(outbuf, "%s%s-=:))%s%s rpgd connection refused %s%s((:=-", back(BLK), fore(RED), back(RED), fore(WHITE), back(BLK), fore(RED));
		OUT(outbuf); NL;
		NORMAL; NL;
		flag = FALSE;
		logoff = TRUE;
		return(flag);
	}
	procid=getpid();
	write(sockfd,&procid,sizeof(procid));
	write(sockfd,PLAYER.ID,sizeof(PLAYER.ID));
	write(sockfd,&command,sizeof(command));
	if(p)
		write(sockfd,p,sizeof(struct user));
	else
		write(sockfd,"",1);
	read(sockfd,&flag,sizeof(flag));
	read(sockfd,&l,sizeof(l));
	if(command!=SERVER_UNLOCK)
		if(l>0)
			read(sockfd,p,l);
//	sprintf(outbuf,"READ flag: %hd   l: %d   p: \"%s\"",flag,l,p);
//	NL;OUT(outbuf);NL;
	switch(command) {
		case SERVER_CONNECT:
			break;
		case SERVER_LOCK:
			if(!flag) {
				NL;beep();
				sprintf(outbuf,"%s%s is already engaged elsewhere.",fore(CYAN),p);
				OUT(outbuf);NL;
				NORMAL;NL;
			}
			break;
		case SERVER_UNLOCK:
			if(flag) {
				while(l>0) {
					read(sockfd,outbuf,l<OUTBUF_SIZE ? l : OUTBUF_SIZE-1);
					outbuf[OUTBUF_SIZE-1]='\0';
					OUT(outbuf);
					l-=(OUTBUF_SIZE-1);
				}
				NORMAL;NL;NL;
			}
			break;
	}
	shutdown(sockfd,3);
	close(sockfd);
	return(flag);
}

int Init(void)
{
	int i,j;

	table=calloc(1,sizeof(struct ServerTable));
	LoadETC(table);
	if(!(Keybuf=calloc(1,KEYBUF_SIZE)))
		return(FALSE);
	if(!(outbuf=calloc(1,OUTBUF_SIZE)))
		return(FALSE);
	if(!(prompt=calloc(1,PROMPT_SIZE)))
		return(FALSE);
	if(!(inbuf=calloc(1,INBUF_SIZE)))
		return(FALSE);
	if(!(filename=calloc(1,128)))
		return(FALSE);
	for(i=0; i<2; i++)
		for(j=0; j<5; j++)
			if(!(RPC[i][j]=calloc(1,sizeof(struct RPC))))
				return(FALSE);
	line=calloc(100,sizeof(char *));
	for(i=0; i<100; i++)
		line[i]=calloc(1,128);

	if(!RPGserver(SERVER_CONNECT,NULL))
		return(FALSE);
	strcpy(IRSREC->ID,"_IRS");
	if(!RPGserver(SERVER_GETUSER,(UBYTE *)IRSREC))
		return(FALSE);
	strcpy(NEWREC->ID,"_NEW");
	if(!RPGserver(SERVER_GETUSER,(UBYTE *)NEWREC))
		return(FALSE);
	strcpy(SYSREC->ID,"_SYS");
	if(!RPGserver(SERVER_GETUSER,(UBYTE *)SYSREC))
		return(FALSE);
	strcpy(SYSID,SYSREC->ID);

	GetSystemTime(Time);
	srand(atoi(Time));

	return(TRUE);
}

void Shutdown(void)
{
	int i,j;

	for(i=0; i<100; i++)
		if(line[i])
			free(line[i]);
	if(line)
		free(line);
	for(i=0; i<2; i++)
		for(j=0; j<5; j++)
			if(RPC[i][j])
				free(RPC[i][j]);
	if(filename)
		free(filename);
	if(inbuf)
		free(inbuf);
	if(prompt)
		free(prompt);
	if(outbuf)
		free(outbuf);
	if(Keybuf)
		free(Keybuf);

	UnloadETC(table);
	term_exit();
}

void signalHUP(int signum)
{
	static int i = 0;
	char *s;

	switch(signum) {
		case SIGHUP:
			s="SIGHUP";
			break;
		case SIGINT:
			s="SIGINT";
			break;
		case SIGIOT:
			s="SIGIOT";
			break;
		case SIGPIPE:
			s="SIGPIPE";
			break;
		case SIGQUIT:
			s="SIGQUIT";
			break;
		case SIGSEGV:
			s="SIGSEGV";
			break;
		case SIGTERM:
			s="SIGTERM";
			break;
		default:
			s="error";
			break;
	}
	sprintf(sigtrap,"?%s trap",s);
	if(logoff) {
		strcat(sigtrap," after logoff, =unrecoverable=");
		switch(dice(3)) {
			case 1:
				PLAYER.Bank = 0.;
				if(PLAYER.RealEstate)
					PLAYER.RealEstate--;
				if(PLAYER.Security)
					PLAYER.Security--;
				break;
			case 2:
				if(PLAYER.Hull) {
					PLAYER.Hull = 0;
					PLAYER.Cannon = 0;
					PLAYER.Ram = 'N';
				}
				else
					reroll(&PLAYER);
				break;
			case 3:
				PLAYER.Spell = 0;
				PLAYER.XSpell = 0;
				PLAYER.Poison = 0;
				break;
		}
	}
	else {
		NL; beep(); NL;
		OUT(sigtrap); NL;
	}
	news(sigtrap);
	logoff = TRUE;
	if(i > 1) {
		RPGserver(SERVER_DISCONNECT, (UBYTE *)PLAYER.ID);
		memset(PLAYER.ID, 0, sizeof(PLAYER.ID));
		term_exit();
		exit(1);
	}
	i++;
}

int main(int argc, char **argv)
{
	FILE *fp;
	struct passwd *passwd;
	struct group *group;
	mode_t mode;
	gid_t newgid;
	uid_t newuid;
	char parameter[128];

	local = isatty(0);
	if(argc > 1 && strncmp(argv[1], "-s", 2) == 0)
		local = FALSE;
	memset(&led, 0, sizeof(struct led));
	if((fp = fopen("/etc/rpgd.conf", "r"))) {
		while(fgets(parameter, sizeof(parameter), fp)) {
			while(strlen(parameter) > 0 && isspace(parameter[strlen(parameter) - 1]))
				parameter[strlen(parameter) - 1] = '\0';
			if(strncmp(parameter, "HOME=", 5) == 0)
				chdir(&parameter[5]);
			if(strncmp(parameter,"GROUP=", 6) == 0) {
				if(!(group = getgrnam(&parameter[6]))) {
					fprintf(stderr,"?Invalid group: \"%s\"\n", &parameter[6]);
					exit(2);
				}
				newgid = group->gr_gid;
				if(setegid(newgid)) {
					fprintf(stderr, "?Invalid gid: %d\n", newgid);
					exit(1);
				}
			}
			if(strncmp(parameter,"USER=", 5) == 0) {
				if(!(passwd = getpwnam(&parameter[5]))) {
					fprintf(stderr, "?Invalid user: \"%s\"\n", &parameter[5]);
					exit(1);
				}
				newuid = passwd->pw_uid;
				if(seteuid(newuid)) {
					fprintf(stderr, "?Invalid uid: %d\n", newuid);
					exit(1);
				}
			}
			if(strncmp(parameter, "DEV=", 4) == 0) {
				strcpy(led.dev, &parameter[4]);
			}
			if(strncmp(parameter, "MINS=", 5) == 0) {
				led.mins = atoi(&parameter[5]);
			}
			if(strncmp(parameter, "COLS=", 5) == 0) {
				led.cols = atoi(&parameter[5]);
			}
		}
		fclose(fp);
	}

	//	if any of these signals are raised, logoff gracefully... we hope!
	memset(sigtrap,0,sizeof(sigtrap));
	signal(SIGHUP,(sighandler_t)signalHUP);
	signal(SIGINT,(sighandler_t)signalHUP);
	signal(SIGIOT,(sighandler_t)signalHUP);
	signal(SIGPIPE,(sighandler_t)signalHUP);
	signal(SIGQUIT,(sighandler_t)signalHUP);
	signal(SIGSEGV,(sighandler_t)signalHUP);
	signal(SIGTERM,(sighandler_t)signalHUP);
	atexit(Shutdown);

	mode=007;
	umask(mode);

	if(Init()) {
		if(Logon())
			if(strlen(PLAYER.ID)) {
				broadcast("logged off");
				RPGserver(SERVER_DISCONNECT, (UBYTE *)PLAYER.ID);
				memset(PLAYER.ID,0,sizeof(PLAYER.ID));
			}
		term_exit();
	}
	exit(0);
}
