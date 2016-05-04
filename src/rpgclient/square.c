/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * square.c
 * Copyright (C) Robert Hurst 2010 <robert@hurst-ri.us>
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

void Square(void)
{
	static struct RPGmenu RPGmenu = { "Square", BRN, 11,
					{	{ 'A', "Armoury" },
						{ 'W', "Weapons Shoppe" },
						{ 'R', "Real Estate" },
						{ 'S', "Security" },
						{ 'M', "Mages Guild" },
						{ 'V', "Visit the Apothecary" },
						{ 'B', "Ye Olde Stone Bank" },
						{ 'H', "Butler Hospital" },
						{ 'P', "Pick pockets" },
						{ 'J', "Jail House" },
						{ 'G', "Goto the arena" }
					}
	};
	double loan;
	long i, lo, hi;
	BYTE *who = '\0', *s;
	char c;

	bump("outskirts of town");

	if(ACCESS(acclvl)->RolePlay=='Y' && PLAYER.Novice=='Y' && dice(PLAYER.Level/2)==1) {
		if(ONLINE->HP<PLAYER.HP) {
			OUT("> You are battle weary.  Heal yourself at the hospital.");NL;NL;
			paws=!PLAYER.Expert;
		}
		for(i=0; i<NUMPOISON && (PLAYER.Poison & (UWORD)pow(2.,(double)i)); i++);
		if(i<NUMPOISON && ONLINE->user.MyPoison && PLAYER.Gold>=POISON(i)->Cost[(ONLINE->user.MyPoison>1) ? 0 : 1]) {
			OUT("> You might want to buy a poison for your weapon.");NL;NL;
			paws=!PLAYER.Expert;
		}
		if(PLAYER.Security+1<NUMSECURITY && PLAYER.Gold>=SECURITY(PLAYER.Security+1)->Value) {
			OUT("> Now is a good time to buy some Security to protect yourself from being");NL;
			OUT("  burglarized.");NL;NL;
			paws=!PLAYER.Expert;
		}
		if(PLAYER.RealEstate+1<NUMREALESTATE && PLAYER.Gold>=REALESTATE(PLAYER.RealEstate+1)->Value) {
			OUT("> You can increase your standing with the community by moving to a better.");NL;
			OUT("  neighborhood.  Visit the local Real Estate Agency for more details.");NL;NL;
			paws=!PLAYER.Expert;
		}
	}

	while((c=option(&RPGmenu))!='Q') {
		switch(c) {
			case 'A':
				d=value(ARMOR(ONLINE)->Value,ONLINE->CHA);
				if(PLAYER.ACmod!=0)
					modf(d*(ARMOR(ONLINE)->Class+PLAYER.ACmod)/ARMOR(ONLINE)->Class,&d);
				if(ONLINE->ToAC<0)
					modf(d*(ARMOR(ONLINE)->Class+ONLINE->ToAC)/ARMOR(ONLINE)->Class,&d);
				if(ONLINE->armor_origin==0 && ONLINE->armor_type==0)
					d=0.;
				sprintf(outbuf,"You own a class %s%s %s",bracket(ARMOR(ONLINE)->Class),fore(GRY),ARMOR(ONLINE)->Name);
				OUT(outbuf);
				if(PLAYER.ACmod || ONLINE->ToAC) {
					sprintf(outbuf," %s(%s%+d%s,%s%+d%s)",fore(MAG),fore(PLAYER.ACmod>0 ? YELLOW : PLAYER.ACmod<0 ? BRED : GRY),PLAYER.ACmod,fore(GRY),fore(ONLINE->ToAC>0 ? YELLOW : ONLINE->ToAC<0 ? BRED : GRY),ONLINE->ToAC,fore(MAG));
					OUT(outbuf);
				}
				sprintf(outbuf," %sworth %s.",fore(GRY),money(d,ANSI));
				OUT(outbuf);NL;
				if(ONLINE->armor_origin==0 && ONLINE->armor_type==0 && (PLAYER.ACmod<0 || ONLINE->ToAC<0)) {
					sprintf(outbuf,"%sYou look like a leper; go get yourself cured.",fore(YELLOW));
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(strlen(ARMOR(ONLINE)->EgoClass)) {
					sprintf(outbuf,"%sIt works well against any %s.",fore(CYN),strcmp(ARMOR(ONLINE)->EgoClass,"*") ? (char *)ARMOR(ONLINE)->EgoClass : "foe");
					OUT(outbuf);NL;
				}
				if(ARMOR(ONLINE)->EgoMelee>=0) {
					sprintf(outbuf,"%sIt has better temperament against %s-classes.",fore(CYN),ARMOR(ONLINE)->EgoMelee==0 ? "wimp" : ARMOR(ONLINE)->EgoMelee==1 ? "ordinary" : ARMOR(ONLINE)->EgoMelee==2 ? "tough" : ARMOR(ONLINE)->EgoMelee==3 ? "fighting" : "brute");
					OUT(outbuf);NL;
				}
				if(ARMOR(ONLINE)->EgoMagic>=0) {
					sprintf(outbuf,"%sIt resists better against %s-users.",fore(CYN),ARMOR(ONLINE)->EgoMagic==0 ? "non-magic" : ARMOR(ONLINE)->EgoMagic==1 ? "Wand" : ARMOR(ONLINE)->EgoMagic==2 ? "Scroll" : ARMOR(ONLINE)->EgoMagic==3 ? "Spell" : "Enchanted");
					OUT(outbuf);NL;
				}
				if(strlen(ARMOR(ONLINE)->EgoBearer)) {
					sprintf(outbuf,"%sWhen worn by any %s, it makes them tougher to beat.",fore(CYN),strcmp(ARMOR(ONLINE)->EgoBearer,"*") ? (char *)ARMOR(ONLINE)->EgoBearer : "adventurer");
					OUT(outbuf);NL;
				}
				if(strlen(ARMOR(ONLINE)->EgoCurse)) {
					sprintf(outbuf,"%sBut, no %s wants to be caught wearing it.",fore(CYN),strcmp(ARMOR(ONLINE)->EgoCurse,"*") ? (char *)ARMOR(ONLINE)->EgoCurse : "poor soul");
					OUT(outbuf);NL;
				}
				NL;
				if(ONLINE->armor_origin==0) {
					if(!(lo=ONLINE->armor_type))
						lo++;
				}
				else
					lo=1;
				for(; lo>1 && PLAYER.Gold+d<table->armor[0]->Item[lo]->Value; lo--);
				if(ONLINE->armor_origin==0) {
					if(!(hi=ONLINE->armor_type))
						hi++;
				}
				else
					hi=MAXARMOR(0)-1;
				for(; hi<MAXARMOR(0)-1; hi++)
					if(PLAYER.Gold+d<table->armor[0]->Item[hi]->Value)
						break;
				sprintf(prompt,"%sStart list at %s%s: ",fore(CYN),bracket(lo),fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				if(!strlen(inbuf)) {
					sprintf(outbuf,"%s%ld",fore(WHITE),lo);
					OUT(outbuf);
				}
				else
					lo=atoi(inbuf);
				if(lo<1)
					lo=1;
				if(lo>=MAXARMOR(0))
					lo=MAXARMOR(0)-1;
				NL;
				sprintf(prompt,"%s  End list at %s%s: ",fore(CYN),bracket(hi),fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				if(!strlen(inbuf)) {
					sprintf(outbuf,"%s%ld",fore(WHITE),hi);
					OUT(outbuf);
				}
				else
					hi=atoi(inbuf);
				if(hi<lo)
					hi=lo;
				if(hi>=MAXARMOR(0))
					hi=MAXARMOR(0)-1;
				NL;NL;
				for(i=lo; i<MAXARMOR(0) && i<=hi; i++) {
					sprintf(outbuf,"%s%s %-24.24s %10s",bracket(i),fore(GRY),table->armor[0]->Item[i]->Name,money(table->armor[0]->Item[i]->Value,ANSI));
					OUT(outbuf);NL;
				}
				NL;
				sprintf(prompt,"%sBuy which? ",fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				i=atoi(inbuf);
				if(i>=lo && i<=hi && ACCESS(acclvl)->RolePlay=='Y') {
					if(PLAYER.Gold+d>=table->armor[0]->Item[i]->Value) {
						if(ONLINE->armor_origin==0 && i<ONLINE->armor_type/2)
							PLAYER.Coward=TRUE;
						if(ONLINE->armor_origin>0 || i>ONLINE->armor_type*2)
							PLAYER.Coward=FALSE;
						PLAYER.Gold+=d;
						PLAYER.ACmod=0;
						ONLINE->armor_origin=0;
						ONLINE->armor_type=i;
						ONLINE->ToAC=0;
						sprintf(PLAYER.Armor,"%s.%s",table->armor[0]->Origin,table->armor[0]->Item[i]->Name);
						PLAYER.Gold-=table->armor[0]->Item[i]->Value;
						RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
						sound("click",64,0);
						OUT("Done.");NL;
					}
					else {
						OUT("You don't have enough money!");NL;
					}
					paws=!PLAYER.Expert;
				}
				break;
			case 'W':
				d=value(WEAPON(ONLINE)->Value,ONLINE->CHA);
				if(PLAYER.WCmod!=0)
					modf(d*(WEAPON(ONLINE)->Class+PLAYER.WCmod/(ONLINE->user.MyPoison+1))/WEAPON(ONLINE)->Class,&d);
				if(ONLINE->ToWC<0)
					modf(d*(WEAPON(ONLINE)->Class+ONLINE->ToWC)/WEAPON(ONLINE)->Class,&d);
				if(ONLINE->weapon_origin==0 && ONLINE->weapon_type==0)
					d=0.;
				if(d>WEAPON(ONLINE)->Value)
					d=WEAPON(ONLINE)->Value;
				sprintf(outbuf,"You own a class %s%s %s",bracket(WEAPON(ONLINE)->Class),fore(GRY),WEAPON(ONLINE)->Name);
				OUT(outbuf);
				if(PLAYER.WCmod || ONLINE->ToWC) {
					sprintf(outbuf," %s(%s%+d%s,%s%+d%s)",fore(MAG),fore(PLAYER.WCmod>0 ? YELLOW : PLAYER.WCmod<0 ? BRED : GRY),PLAYER.WCmod,fore(GRY),fore(ONLINE->ToWC>0 ? YELLOW : ONLINE->ToWC<0 ? BRED : GRY),ONLINE->ToWC,fore(MAG));
					OUT(outbuf);
				}
				sprintf(outbuf," %sworth %s.",fore(GRY),money(d,ANSI));
				OUT(outbuf);NL;
				if(ONLINE->weapon_origin==0 && ONLINE->weapon_type==0 && (PLAYER.WCmod<0 || ONLINE->ToWC<0)) {
					sprintf(outbuf,"%sYour hands are broken; go get them healed.",fore(YELLOW));
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(strlen(WEAPON(ONLINE)->EgoClass)) {
					sprintf(outbuf,"%sIt hacks well against any %s.",fore(CYN),strcmp(WEAPON(ONLINE)->EgoClass,"*") ? (char *)WEAPON(ONLINE)->EgoClass : "foe");
					OUT(outbuf);NL;
				}
				if(WEAPON(ONLINE)->EgoMelee>=0) {
					sprintf(outbuf,"%sIt has a nastier sting against %s-classes.",fore(CYN),WEAPON(ONLINE)->EgoMelee==0 ? "wimp" : WEAPON(ONLINE)->EgoMelee==1 ? "ordinary" : WEAPON(ONLINE)->EgoMelee==2 ? "tough" : WEAPON(ONLINE)->EgoMelee==3 ? "fighting" : "brute");
					OUT(outbuf);NL;
				}
				if(WEAPON(ONLINE)->EgoMagic>=0) {
					sprintf(outbuf,"%sIt is bitter towards %s-users.",fore(CYN),WEAPON(ONLINE)->EgoMagic==1 ? "non-magic" : WEAPON(ONLINE)->EgoMagic==1 ? "Wand" : WEAPON(ONLINE)->EgoMagic==2 ? "Scroll" : WEAPON(ONLINE)->EgoMagic==3 ? "Spell" : "Enchanted");
					OUT(outbuf);NL;
				}
				if(strlen(WEAPON(ONLINE)->EgoBearer)) {
					sprintf(outbuf,"%sWhen wielded by any %s, it makes them tougher to beat.",fore(CYN),strcmp(WEAPON(ONLINE)->EgoBearer,"*") ? (char *)WEAPON(ONLINE)->EgoBearer : "adventurer");
					OUT(outbuf);NL;
				}
				if(strlen(WEAPON(ONLINE)->EgoCurse)) {
					sprintf(outbuf,"%sBut, no %s wants to be caught wielding this.",fore(CYN),strcmp(WEAPON(ONLINE)->EgoCurse,"*") ? (char *)WEAPON(ONLINE)->EgoCurse : "poor soul");
					OUT(outbuf);NL;
				}
				NL;
				if(ONLINE->weapon_origin==0) {
					if(!(lo=ONLINE->weapon_type))
						lo++;
				}
				else
					lo=1;
				for(; lo>1 && PLAYER.Gold+d<table->weapon[0]->Item[lo]->Value; lo--);
				if(ONLINE->weapon_origin==0) {
					if(!(hi=ONLINE->weapon_type))
						hi++;
				}
				else
					hi=MAXWEAPON(0)-1;
				for(; hi<MAXWEAPON(0)-1; hi++)
					if(PLAYER.Gold+d<table->weapon[0]->Item[hi]->Value)
						break;
				sprintf(prompt,"%sStart list at %s%s: ",fore(CYN),bracket(lo),fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				if(!strlen(inbuf)) {
					sprintf(outbuf,"%s%ld",fore(WHITE),lo);
					OUT(outbuf);
				}
				else
				if(atoi(inbuf)>0)
					lo=atoi(inbuf);
				if(lo>=MAXWEAPON(0))
					lo=MAXWEAPON(0)-1;
				NL;
				sprintf(prompt,"%s  End list at %s%s: ",fore(CYN),bracket(hi),fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				if(!strlen(inbuf)) {
					sprintf(outbuf,"%s%ld",fore(WHITE),hi);
					OUT(outbuf);
				}
				else
					hi=atoi(inbuf);
				if(hi<lo)
					hi=lo;
				if(hi>=MAXWEAPON(0))
					hi=MAXWEAPON(0)-1;
				NL;NL;
				for(i=lo; i<MAXWEAPON(0) && i<=hi; i++) {
					sprintf(outbuf,"%s%s %-24.24s %10s",bracket(i),fore(GRY),table->weapon[0]->Item[i]->Name,money(table->weapon[0]->Item[i]->Value,ANSI));
					OUT(outbuf);NL;
				}
				NL;
				sprintf(prompt,"%sBuy which? ",fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				i=atoi(inbuf);
				if(i>=lo && i<=hi && ACCESS(acclvl)->RolePlay=='Y') {
					if(PLAYER.Gold+d>=table->weapon[0]->Item[i]->Value) {
						if(ONLINE->weapon_origin==0 && i<ONLINE->weapon_type/2)
							PLAYER.Coward=TRUE;
						if(ONLINE->weapon_origin>0 || i>ONLINE->weapon_type*2)
							PLAYER.Coward=FALSE;
						PLAYER.Gold+=d;
						PLAYER.WCmod=0;
						ONLINE->weapon_origin=0;
						ONLINE->weapon_type=i;
						ONLINE->ToWC=0;
						sprintf(PLAYER.Weapon,"%s.%s",table->weapon[0]->Origin,table->weapon[0]->Item[i]->Name);
						PLAYER.Gold-=table->weapon[0]->Item[i]->Value;
						RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
						sound("click",64,0);
						OUT("Done.");NL;
					}
					else {
						OUT("You don't have enough money!");NL;
					}
					paws=!PLAYER.Expert;
				}
				break;
			case 'R':
				d = value(REALESTATE(PLAYER.RealEstate)->Value, ONLINE->CHA);
				sprintf(outbuf, "You own %s%s worth %s.", AN(REALESTATE(PLAYER.RealEstate)->Name), REALESTATE(PLAYER.RealEstate)->Name, money(d, ANSI));
				OUT(outbuf); NL; NL;
				for(lo = (int)PLAYER.RealEstate - realestate; lo > 1 && PLAYER.Gold + d< REALESTATE(lo)->Value; lo--);
				if(lo < 1)
					lo = 1;
				for(hi = PLAYER.RealEstate; hi < NUMREALESTATE - 1; hi++)
					if(PLAYER.Gold + d < REALESTATE(hi)->Value)
						break;
				for(i = lo; i < NUMREALESTATE && i <= hi; i++) {
					sprintf(outbuf, "%s%s %-20.20s %10s", bracket(i), fore(GRY), REALESTATE(i)->Name, money(REALESTATE(i)->Value, ANSI));
					OUT(outbuf); NL;
				}
				NL;
				sprintf(prompt, "%sBuy which? ", fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL; NL;
				i = atoi(inbuf);
				if(i >= lo && i <= hi && ACCESS(acclvl)->RolePlay == 'Y') {
					if(PLAYER.Gold + d >= REALESTATE(i)->Value) {
						PLAYER.Gold += d;
						PLAYER.Gold -= REALESTATE(i)->Value;
						PLAYER.RealEstate = i;
						RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
						sound("click", 64, 0);
						OUT("Done."); NL;
						if(i == lo && realestate > 0)
							realestate--;
					}
					else {
						OUT("You don't have enough money!"); NL;
					}
					paws = !PLAYER.Expert;
				}
				break;
			case 'S':
				d = value(SECURITY(PLAYER.Security)->Value, ONLINE->CHA);
				sprintf(outbuf, "You own %s%s worth %s.", AN(SECURITY(PLAYER.Security)->Name), SECURITY(PLAYER.Security)->Name, money(d, ANSI));
				OUT(outbuf); NL; NL;
				for(lo = (int)PLAYER.Security - security; lo > 1 && PLAYER.Gold + d < SECURITY(lo)->Value; lo--);
				if(lo < 1)
					lo = 1;
				for(hi = PLAYER.Security; hi < NUMSECURITY - 1; hi++)
					if(PLAYER.Gold + d < SECURITY(hi)->Value)
						break;
				for(i = lo; i < NUMSECURITY && i <= hi; i++) {
					sprintf(outbuf, "%s%s %-20.20s %10s", bracket(i), fore(GRY), SECURITY(i)->Name, money(SECURITY(i)->Value, ANSI));
					OUT(outbuf); NL;
				}
				NL;
				sprintf(prompt, "%sBuy which? ", fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL; NL;
				i = atoi(inbuf);
				if(i >= lo && i <= hi && ACCESS(acclvl)->RolePlay == 'Y') {
					if(PLAYER.Gold + d >= SECURITY(i)->Value) {
						PLAYER.Gold += d;
						PLAYER.Gold -= SECURITY(i)->Value;
						PLAYER.Security = i;
						RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
						sound("click", 64, 0);
						OUT("Done."); NL;
						if(i == lo && security > 0)
							security--;
					}
					else {
						OUT("You don't have enough money!"); NL;
					}
					paws = !PLAYER.Expert;
				}
				break;
			case 'M':
				if(!ONLINE->user.MyMagic || PLAYER.Spell==(UWORD)~0) {
					OUT("The old mage says, \"Get outta here!\"");NL;
					paws=!PLAYER.Expert;
					break;
				}
				switch(ONLINE->user.MyMagic) {
					case 1:
						OUT("The old mage offers to sell you a magic wand.");
						break;
					case 2:
						OUT("The old mage offers to make you a scroll, for a price.");
						break;
					case 3:
						OUT("The old mage offers to teach you a spell, for a price.");
						break;
					case 4:
						OUT("The old mage wants to teach you a spell, for a price.");
						break;
				}
				NL;NL;
				lo=(ONLINE->user.MyMagic>1) ? 0 : 1;
				for(i=0; i<(NUMMAGIC<16 ? NUMMAGIC : 16); i++) {
					if(!(PLAYER.Spell & (UWORD)pow(2.,(double)i))) {
						sprintf(outbuf,"%s%s %-20.20s %10s",bracket(i+1),fore(GRY),MAGIC(i)->Name,money(MAGIC(i)->Cost[lo],ANSI));
						OUT(outbuf);NL;
						if(PLAYER.Gold<MAGIC(i)->Cost[lo])
							break;
					}
				}
				sprintf(prompt,"%sBuy which? ",fore(CYN));
				NL;OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				i=atoi(inbuf);
				if(i<1 || i>(NUMMAGIC<16 ? NUMMAGIC : 16))
					break;
				i--;
				if(!(PLAYER.Spell & (UWORD)pow(2.,(double)i)) && ACCESS(acclvl)->RolePlay=='Y') {
					if(PLAYER.Gold>=MAGIC(i)->Cost[lo]) {
						PLAYER.Spell|=(UWORD)pow(2.,(double)i);
						PLAYER.Gold-=MAGIC(i)->Cost[lo];
						RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
						sound("click",64,0);
						OUT("The deed is done.");NL;
					}
					else {
						OUT("You don't have enough money!");NL;
					}
					paws=!PLAYER.Expert;
				}
				break;
			case 'V':
				if(!ONLINE->user.MyPoison || PLAYER.Poison==(UWORD)~0) {
					OUT("The Apothecary says, \"Get outta here!\"");NL;
					break;
				}
				OUT("You enter the back door of the shop.");NL;NL;
				lo=(ONLINE->user.MyPoison>1) ? 0 : 1;
				for(i=0; i<NUMPOISON; i++) {
					if(!(PLAYER.Poison & (UWORD)pow(2.,(double)i))) {
						sprintf(outbuf,"%s%s %-20.20s %10s",bracket(i+1),fore(GRY),POISON(i)->Name,money(POISON(i)->Cost[lo],ANSI));
						OUT(outbuf);NL;
						if(PLAYER.Gold<POISON(i)->Cost[lo])
							break;
					}
				}
				sprintf(prompt,"%sBuy which? ",fore(CYN));
				NL;OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				i=atoi(inbuf);
				if(i<1 || i>NUMPOISON)
					break;
				i--;
				if(!(PLAYER.Poison & (UWORD)pow(2.,(double)i)) && ACCESS(acclvl)->RolePlay=='Y') {
					if(PLAYER.Gold>=POISON(i)->Cost[lo]) {
						PLAYER.Poison|=(UWORD)pow(2.,(double)i);
						PLAYER.Gold-=POISON(i)->Cost[lo];
						RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
						sound("click",64,0);
						OUT("The Apothecary slips you the vial.");NL;
					}
					else {
						OUT("You don't have enough money!");NL;
					}
					paws=!PLAYER.Expert;
				}
				break;
			case 'B':
				while(!cd()) {
					cls();
					sprintf(outbuf,"%sWelcome to Ye Olde Stone Bank",fore(CYAN));
					OUT(outbuf);NL;NL;
					sprintf(outbuf,"%sMoney in hand: %s",fore(GRN),money(PLAYER.Gold,ANSI));
					OUT(outbuf);NL;
					sprintf(outbuf,"%sMoney in bank: %s",fore(GRN),money(PLAYER.Bank,ANSI));
					OUT(outbuf);NL;
					sprintf(outbuf,"%sMoney on loan: %s",fore(GRN),money(PLAYER.Loan,ANSI));
					OUT(outbuf);NL;NL;
					sprintf(prompt,"%s[%sBank%s]%s Option (Q=Quit): ",fore(GRN),fore(YELLOW),fore(GRN),fore(CYN));
					OUT(prompt);
					c=inkey('\0','Q');
					NL;NL;
					if(c=='Q')
						break;
					if(ACCESS(acclvl)->RolePlay=='Y')
						switch(c) {
							case 'D':
								sprintf(outbuf,"%sDeposit%s [MAX=%s]? ",fore(CYN),fore(GRY),money(PLAYER.Gold,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								s = "click";
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									s="max";
									d=PLAYER.Gold;
								}
								else
								if(d==PLAYER.Gold && strlen(inbuf)>3) {
									OUT("You may also type MAX in place of the entire amount.");NL;
									paws=!PLAYER.Expert;
								}
								if(d>0. && d<=PLAYER.Gold) {
									PLAYER.Gold-=d;
									if(PLAYER.Loan) {
										PLAYER.Loan-=d;
										d=0.;
										if(PLAYER.Loan<0.) {
											d=-PLAYER.Loan;
											PLAYER.Loan=0.;
										}
									}
									PLAYER.Bank+=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
									sound(s,64,0);
								}
								break;
							case 'L':
								loan=value(REALESTATE(PLAYER.RealEstate)->Value,ONLINE->CHA);
								loan+=value(REALESTATE(PLAYER.Security)->Value,ONLINE->CHA);
								loan-=PLAYER.Loan;
								if(loan<1.) {
									sprintf(outbuf,"%sYour credit is not THAT good!",fore(YELLOW));
									OUT(outbuf);NL;
									paws=!PLAYER.Expert;
									break;
								}
								sprintf(outbuf,"%sLoan%s [MAX=%s]? ",fore(CYN),fore(GRY),money(loan,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								s="click";
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									s="max";
									d=loan;
								}
								if(d>0. && d<=loan) {
									PLAYER.Loan+=d;
									PLAYER.Gold+=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
									sound(s,64,0);
								}
								break;
							case 'R':
								OUT("You attempt to sneak into the vault...");
								c = (PLAYER.Level / 5 + 1) * PLAYER.MySteal;
								if(dice(100) > c) {
									strcpy(PLAYER.Status, "jail");
									logoff = TRUE;
									strcpy(reason, "caught getting into the vault");
									Delay(100);
									NL; NL;
									sound("arrested", 64, 0);
									OUT("A guard catches you and throws you into jail!"); NL;
									Delay(75);
									OUT("You might be released by your next call."); NL; NL;
									Delay(75);
									break;
								}
								d = PLAYER.Level + 1;
								d = pow(d, 8.);
								loan = d * dice(90) + d * dice(10);
								memset(&ENEMY, 0, sizeof(struct user));
								for(i = 0; RPGserver(SERVER_NEXTUSER, (UBYTE *)&ENEMY) && !cd(); i++) {
									loan += ENEMY.Bank;
									if(i % 5 == 0) {
										sprintf(outbuf, "%c\10", spin[i / 10 % 4]);
										OUT(outbuf);
										Delay(5);
									}
								}
								NL; NL;
								sound("max", 64, 0);
								sprintf(outbuf, "You steal %s!", money(loan, ANSI));
								OUT(outbuf); NL;
								Delay(150);
								NL;
								OUT("You try to make your way out of the vault...");
								Delay(150);
								c /= 9 - (PLAYER.MySteal + 1);
								c++;
								if(dice(100) > c) {
									strcpy(PLAYER.Status, "jail");
									logoff = TRUE;
									strcpy(reason, "caught inside the vault");
									OUT("something jingles!"); NL;
									Delay(50);
									NL;
									sound("arrested", 64, 0);
									OUT("A guard laughs as he closes the vault door on you!"); NL;
									Delay(75);
									OUT("You might be released by your next call.");NL; NL;
									Delay(75);
									break;
								}
								NL;
								OUT("You make good with your escape!"); NL;
								for(i = 0; RPGserver(SERVER_NEXTUSER, (UBYTE *)&ENEMY); i++) {
									if(ENEMY.Bank > 0.) {
										numline = 0;
										sprintf(line[numline++], "%s robbed the bank and took %s from your account!", PLAYER.Handle, money(ENEMY.Bank, ENEMY.Emulation));
										note(ENEMY.ID);
										ENEMY.Bank = 0.;
										RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
									}
									if(i % 5 == 0) {
										sprintf(outbuf, "%c\10", spin[i / 10 % 4]);
										OUT(outbuf);
										Delay(5);
									}
								}
								PLAYER.Gold += loan;
								PLAYER.History.Stole++;
								RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
								break;
							case 'W':
								sprintf(outbuf,"%sWithdrawal%s [MAX=%s]? ",fore(CYN),fore(GRY),money(PLAYER.Bank,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								s="click";
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									s="max";
									d=PLAYER.Bank;
								}
								if(d>0. && d<=PLAYER.Bank) {
									PLAYER.Bank-=d;
									PLAYER.Gold+=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
									sound(s,64,0);
								}
								break;
							case 'T':
								if(ACCESS(acclvl)->Sysop!='Y')
									break;
								sprintf(outbuf,"%sTreasury withdrawal%s [MAX=%s]? ",fore(CYN),fore(GRY),money(IRSREC->Bank,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								s="click";
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									s="max";
									d=IRSREC->Bank;
								}
								if(d>0. && d<=IRSREC->Bank) {
									IRSREC->Bank-=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)IRSREC);
									PLAYER.Gold+=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
									sound(s,64,0);
								}
								break;
							case 'V':
								if(ACCESS(acclvl)->Sysop!='Y')
									break;
								sprintf(outbuf,"%sVault deposit%s [MAX=%s]? ",fore(CYN),fore(GRY),money(PLAYER.Gold,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								s="click";
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									s="max";
									d=PLAYER.Gold;
								}
								if(d>0. && d<=PLAYER.Gold) {
									IRSREC->Bank+=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)IRSREC);
									PLAYER.Gold-=d;
									RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
									sound(s,64,0);
								}
								break;
							default:
								beep();
								OUT("<D>eposit, <W>ithdrawal, <L>oan, or <R>ob the bank.");NL;
								paws=TRUE;
								break;
						}
				}
				break;
			case 'H':
				if(ONLINE->armor_origin==0 && ONLINE->armor_type==0 && (PLAYER.ACmod<0 || ONLINE->ToAC<0)) {
					d=(abs(PLAYER.ACmod)+abs(ONLINE->ToAC))*100000.;
					sprintf(prompt,"%sHeal your skin for %s (Y/N)? ",fore(CYN),money(d,ANSI));
					OUT(prompt);
					c=inkey('N','N');
					NL;NL;
					if(c=='Y') {
						PLAYER.ACmod=0;ONLINE->ToAC=0;
						PLAYER.Gold-=d;
						if(PLAYER.Gold<0.) {
							PLAYER.Bank+=PLAYER.Gold;
							PLAYER.Gold=0.;
							if(PLAYER.Bank<0.) {
								PLAYER.Loan-=PLAYER.Bank;
								PLAYER.Bank=0.;
							}
						}
					}
					paws=!PLAYER.Expert;
				}
				if(ONLINE->weapon_origin==0 && ONLINE->weapon_type==0 && (PLAYER.WCmod<0 || ONLINE->ToWC<0)) {
					d=(abs(PLAYER.WCmod)+abs(ONLINE->ToWC))*100000.;
					sprintf(prompt,"%sFix your hands for %s (Y/N)? ",fore(CYN),money(d,ANSI));
					OUT(prompt);
					c=inkey('N','N');
					NL;NL;
					if(c=='Y') {
						sound("click",64,0);
						PLAYER.WCmod=0;ONLINE->ToWC=0;
						PLAYER.Gold-=d;
						if(PLAYER.Gold<0.) {
							PLAYER.Bank+=PLAYER.Gold;
							PLAYER.Gold=0.;
							if(PLAYER.Bank<0.) {
								PLAYER.Loan-=PLAYER.Bank;
								PLAYER.Bank=0.;
							}
						}
					}
					paws=!PLAYER.Expert;
				}
				hi=PLAYER.HP-ONLINE->HP;
				if(hi<1) {
					OUT("You don't need any hit points.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				OUT("Welcome to Butler Hospital.");NL;NL;
				sprintf(outbuf,"Hit points cost %u each.",PLAYER.Level);
				OUT(outbuf);NL;
				sprintf(outbuf,"You need %ld hit points.",hi);
				OUT(outbuf);NL;
				if(PLAYER.Gold<1000000.)
					lo=PLAYER.Gold/PLAYER.Level;
				else
					lo=hi;
				if(lo<hi)
					sprintf(prompt,"%ld",lo);
				else
					strcpy(prompt,"all your");
				sprintf(outbuf,"You can afford %s hit points.",prompt);
				OUT(outbuf);NL;
				if(lo<hi) {
					if(PLAYER.Novice=='Y')
						sprintf(outbuf,"Normally, you would be billed for the remaining %ld hit points.",hi-lo);
					else
						sprintf(outbuf,"You can be billed for the remaining %ld hit points.",hi-lo);
					OUT(outbuf);NL;
				}
				NL;
				if(hi>0) {
					sprintf(prompt,"%sHow many do you want%s [MAX=%ld]? ",fore(CYN),fore(GRY),hi);
					OUT(prompt);
					if(ins(5))
						break;
					NL;NL;
					i=atoi(inbuf);
					s="click";
					if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
						s="max";
						i=hi;
					}
					if(i<1 || i>hi)
						break;
					ONLINE->HP+=i;
					sound(s,64,0);
					if(!(*inbuf=='=' && ACCESS(acclvl)->Sysop=='Y')) {
						PLAYER.Gold-=(DOUBLE)i*PLAYER.Level;
						if(PLAYER.Gold<0.) {
							PLAYER.Bank+=PLAYER.Gold;
							PLAYER.Gold=0.;
							if(PLAYER.Bank<0.) {
								if(PLAYER.Novice!='Y')
									PLAYER.Loan-=PLAYER.Bank;
								PLAYER.Bank=0.;
							}
						}
					}
					sprintf(outbuf,"Hit points = %u",PLAYER.HP);
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
				}
				break;
			case 'P':
				if(ACCESS(acclvl)->RolePlay == 'Y' && PLAYER.Novice != 'Y') {
					OUT("You attempt to pick a passerby's pocket...");
					Delay(100);
					NL; NL;
					JOUST(PLAYER.Level);
					modf(5. * d / dice(10), &d);
					who = "somebody";
					RPGserver(SERVER_RNDUSER, (UBYTE *)&ENEMY);
					if(strcmp(PLAYER.ID, ENEMY.ID) && !strlen(ENEMY.Status) && ENEMY.Gold > 0.) {
						d = ENEMY.Gold;
						who = ENEMY.Handle;
					}
					sound("max", 64, 0);
					sprintf(outbuf, "You pick %s's pocket and steal %s!", who, money(d, ANSI));
					OUT(outbuf); NL; NL;
					Delay(100);
					if(dice(100) > 16 * PLAYER.MySteal + PLAYER.Level / 10 + PLAYER.DEX / 10) {
						strcpy(reason, "caught picking a pocket");
						logoff = TRUE;
						strcpy(PLAYER.Status, "jail");
						sound("arrested", 64, 0);
						OUT("A guard catches you and throws you into jail!"); NL;
						Delay(75);
						OUT("You might be released by your next call."); NL; NL;
						Delay(75);
						if(who == ENEMY.Handle) {
							numline = 0;
							sprintf(line[numline++], "%s was caught picking your pocket!", PLAYER.Handle);
							note(ENEMY.ID);
						}
						break;
					}
					PLAYER.Gold += d;
					PLAYER.History.Stole++;
					if(who == ENEMY.Handle) {
						numline = 0;
						sprintf(line[numline++], "Somebody picked your pocket and stole %s!", money(d, ENEMY.Emulation));
						note(ENEMY.ID);
						ENEMY.Gold = 0.;
						RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
					}
				}
				break;
			case 'J':
				if(bail) {
					OUT("A deputy greets you in front of the County Jail.");NL;NL;
					switch(dice(5)) {
						case 1:
							who="cur";
							break;
						case 2:
							who="knave";
							break;
						case 3:
							who="scum";
							break;
						case 4:
							who="toad";
							break;
						case 5:
							who="villain";
							break;
					}
					sprintf(prompt,"%sWhat %s do you want to bail out? ",fore(CYN),who);
					if(GetRPC(RPC[1][0])) {
						if(strcmp(ENEMY.Status,"jail")) {
							sprintf(outbuf,"%s is not in jail.",ENEMY.Handle);
							OUT(outbuf);NL;
							paws=!PLAYER.Expert;
							break;
						}
						JOUST(ENEMY.Level);
						modf(d*(100-PLAYER.CHA+1)/100.+1.,&d);
						sprintf(outbuf,"It will cost you %s to bail out %s.",money(d,ANSI),ENEMY.Handle);
						OUT(outbuf);NL;NL;
						sprintf(prompt,"%sWill you pay (Y/N)? ",fore(CYN));
						OUT(prompt);
						c=inkey('N','N');
						NL;

						if(c=='Y') {
							NL;
							if(!RPGserver(SERVER_LOCK, (UBYTE *)ENEMY.ID))
								break;
							if(PLAYER.Gold>=d) {
								bail--;
								PLAYER.Gold-=d;
								memset(ENEMY.Status,0,sizeof(ENEMY.Status));
								RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
								sound("click",64,0);
								numline=0;
								sprintf(line[numline++],"%s paid %s to bail you out of jail.",PLAYER.Handle,money(d,ENEMY.Emulation));
								note(ENEMY.ID);
								sprintf(outbuf,"bailed %s out of jail",ENEMY.Handle);
								news(outbuf);
								sprintf(outbuf,"%s is set free.",ENEMY.Handle);
								OUT(outbuf);NL;
							}
							else {
								OUT("You don't have enough money!");NL;
							}
							paws=!PLAYER.Expert;
						}
					}
					break;
				}
				OUT("The jail house is closed for the day.");NL;
				paws=!PLAYER.Expert;
				break;
			case 'G':
				if(from=='A')
					return;
				from='S';
				Arena();
				if(from=='Q')
					return;
				break;
		}
	}
	from='Q';
}
