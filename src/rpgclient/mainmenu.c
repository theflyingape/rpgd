/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * mainmenu.c
 * Copyright (C) Robert Hurst 2012 <robert@hurst-ri.us>
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

void charstats(struct RPC *rpc)
{
	static char *magic[] = { "Wand", "Scroll", "Spell", "Magic" };
	int bg, i;
	char c = 'N', *p;

	if(ACCESS(acclvl)->RolePlay == 'Y' || from == '@') {
		JOUST(PLAYER.Level);
		modf(d / 10., &d);
		if(from == '@')
			d = 0.;
		sprintf(prompt, "%sScout another user for %s %s(Y/N)? ", fore(CYN), money(d, ANSI), fore(CYN));
		OUT(prompt);
		c = inkey('N', 'N');
	}
	if(c == 'Y') {
		NL; NL;
		sprintf(prompt, "%sScout user? ", fore(CYN));
		if(!GetRPC(rpc))
			return;
		PLAYER.Gold -= d;
		if(PLAYER.Gold < 0.) {
			PLAYER.Bank += PLAYER.Gold;
			PLAYER.Gold = 0.;
			if(PLAYER.Bank < 0.) {
				PLAYER.Loan -= PLAYER.Bank;
				PLAYER.Bank = 0.;
			}
		}
	}
	else
		memcpy(rpc, ONLINE, sizeof(struct RPC));
	
	bg = ANSI == 3 ? CYN : GRY;
	cls();
	if(ANSI > 1) {
		OUT(back(BLK));
		OUT(fore(BLU));
	}
	else
		NORMAL;
	memset(line[52], '-', 52);
	sprintf(line[1], "%s", rpc->user.Handle);
	i = 20 - strlen(line[1]);
	sprintf(outbuf, "+%.*s=:))", 11 + i / 2, line[52]);
	OUT(outbuf);
	if(ANSI > 1) {
		OUT(back(BLU));
		OUT(fore(YELLOW));
	}
	else
		REVERSE;
	sprintf(outbuf, " %s ", line[1]);
	OUT(outbuf);
	if(ANSI > 1) {
		OUT(back(BLK));
		OUT(fore(BLU));
	}
	else
		NORMAL;
	sprintf(outbuf, "((:=%.*s+", 11 + i / 2 + i % 2, line[52]);
	OUT(outbuf); NL;
	fdate(line[0], rpc->user.BirthDate);
	sprintf(outbuf, "%s|%s    %sTitle:%s %-18s %sBorn:%s %s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), ACCESS(rpc->user.Access)->Name, fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%s (%c)", CLASS(rpc)->Name, rpc->user.Gender);
	sprintf(line[1], "%.8g", rpc->user.Experience);
	sprintf(outbuf, "%s|%s    %sClass:%s %-19s %sExp:%s %-15s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), line[1], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%u.%02u", rpc->user.Immortal, rpc->user.Level);
	sprintf(line[1], "%.8g", EXP(rpc->user.Level));
	sprintf(outbuf, "%s|%s %sImmortal:%s %-18s %sNeed:%s %-15s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), line[1], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%3u (%u,%u)", rpc->STR, rpc->user.STR, rpc->user.MyMaxSTR);
	sprintf(line[1], "%s", money(rpc->user.Gold, FALSE));
	sprintf(outbuf, "%s|%s      %sStr:%s%-19s %sHand:%s %s%*s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), money(rpc->user.Gold, ANSI), (int)(15 - strlen(line[1])), "", back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%3u (%u,%u)", rpc->INT, rpc->user.INT, rpc->user.MyMaxINT);
	sprintf(line[1], "%s", money(rpc->user.Bank, FALSE));
	sprintf(outbuf, "%s|%s      %sInt:%s%-19s %sBank:%s %s%*s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), money(rpc->user.Bank, ANSI), (int)(15 - strlen(line[1])), "", back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%3u (%u,%u)", rpc->DEX, rpc->user.DEX, rpc->user.MyMaxDEX);
	sprintf(line[1],"%s",money(rpc->user.Loan, FALSE));
	sprintf(outbuf, "%s|%s      %sDex:%s%-19s %sLoan:%s %s%*s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), money(rpc->user.Loan, ANSI), (int)(15 - strlen(line[1])), "", back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%3u (%u,%u)", rpc->CHA, rpc->user.CHA, rpc->user.MyMaxCHA);
	sprintf(outbuf, "%s|%s      %sCha:%s%-40s  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%u/%u  (%s)", rpc->HP, rpc->user.HP, rpc->user.MyMelee == 0 ? "weak" : rpc->user.MyMelee == 1 ? "normal" : rpc->user.MyMelee == 2 ? "advanced" : rpc->user.MyMelee == 3 ? "warrior" : rpc->user.MyMelee == 4 ? "brutal" : "divine");
	sprintf(outbuf, "%s|%s       %sHP:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	if(rpc->user.SP) {
		sprintf(line[0], "%u/%u  (%s)", rpc->SP, rpc->user.SP, rpc->user.MyMagic == 2 ? "wizardry" : rpc->user.MyMagic == 3 ? "arcane" : "divine");
		sprintf(outbuf, "%s|%s       %sSP:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
		OUT(outbuf); NL;
	}
	sprintf(line[0], "%s (%+d,%+d)", WEAPON(rpc)->Name, rpc->user.WCmod, rpc->ToWC);
	sprintf(outbuf, "%s|%s   %sWeapon:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(line[0], "%s (%+d,%+d)", ARMOR(rpc)->Name, rpc->user.ACmod, rpc->ToAC);
	sprintf(outbuf, "%s|%s    %sArmor:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	if(rpc->user.Poison) {
		strcpy(line[0], "");
		for(i = 0; i < NUMPOISON; i++)
			if(rpc->user.Poison & (UWORD)pow(2., (double)i)) {
				sprintf(line[1], "%s%d", strlen(line[0]) ? "," : "", i + 1);
				strcat(line[0], line[1]);
			}
		sprintf(outbuf, "%s|%s  %sPoisons:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], back(BLK), fore(BLU));
		OUT(outbuf); NL;
	}
	if(rpc->user.Spell) {
		strcpy(line[0], "");
		for(i = 0; i<(NUMMAGIC < 16 ? NUMMAGIC : 16); i++)
			if(rpc->user.Spell & (UWORD)pow(2., (double)i)) {
				sprintf(line[1], "%s%.*s", strlen(line[0]) ? "," : "", strlen(MAGIC(i)->Name) < 7 ? (int)strlen(MAGIC(i)->Name) : 3, MAGIC(i)->Name);
				strcat(line[0], line[1]);
			}
		strcpy(line[1], "");
		if(strlen(line[0]) > 40) {
			for(p = line[0], i = 40; i > 0 && p[i] != ','; i--);
			p[i] = '\0';
			strcpy(line[1], &p[i + 1]);
		}
		sprintf(outbuf, "%s|%s %s%7ss:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), magic[rpc->user.MyMagic - 1], fore(WHITE), line[0], back(BLK), fore(BLU));
		OUT(outbuf); NL;
		if(strlen(line[1])) {
			sprintf(outbuf, "%s|%s           %s%-40s %s%s|", fore(BLU), back(BLU), fore(WHITE), line[1], back(BLK), fore(BLU));
			OUT(outbuf); NL;
		}
	}
	if(rpc->user.XSpell) {
		strcpy(line[0], "");
		for(i = 0; i < NUMMAGIC - 16; i++)
			if(rpc->user.XSpell & (UWORD)pow(2., (double)i)) {
				sprintf(line[1], "%s%s", strlen(line[0]) ? "," : "",MAGIC(i + 16)->Name);
				strcat(line[0], line[1]);
			}
		strcpy(line[1], "");
		if(strlen(line[0]) > 40) {
			for(p = line[0], i = 40; i > 0 && p[i] != ','; i--);
			p[i] = '\0';
			strcpy(line[1], &p[i + 1]);
		}
		sprintf(outbuf, "%s|%s %s%7ss:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), magic[rpc->user.MyMagic-1], fore(WHITE), line[0], back(BLK), fore(BLU));
		OUT(outbuf); NL;
		if(strlen(line[1])) {
			sprintf(outbuf, "%s|%s           %s%-40s %s%s|", fore(BLU), back(BLU), fore(WHITE), line[1], back(BLK), fore(BLU));
			OUT(outbuf); NL;
		}
	}
	sprintf(outbuf, "%s|%s %sDwelling:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), REALESTATE(rpc->user.RealEstate)->Name, back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s|%s %sSecurity:%s %-40s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), SECURITY(rpc->user.Security)->Name, back(BLK), fore(BLU));
	OUT(outbuf); NL;
	if(strlen(rpc->user.Gang)) {
		strcpy(gm[0].Name, rpc->user.Gang);
		if(RPGserver(SERVER_GETGANG, (UBYTE *)&gm[0])) {
			sprintf(line[0], "%.20s", gm[0].Name);
			memset(line[1], 0, 16);
			for(i = 0; i < 4; i++)
				if(strcmp(gm[0].MemberID[i], rpc->user.ID)) {
					strcpy(RPC[1][1]->user.ID, gm[0].MemberID[i]);
					RPGserver(SERVER_GETUSER, (UBYTE *)&RPC[1][1]->user);
					if(strlen(gm[0].MemberID[i])) {
						if(strncmp(rpc->user.Gang,RPC[1][1]->user.Gang, 20))
							strlwr(gm[0].MemberID[i]);
					}
					else
						strcpy(gm[0].MemberID[i], "*");
					sprintf(outbuf, "%c%s", (strlen(line[1]) ? ',' : ' '), gm[0].MemberID[i]);
					strcat(line[1], outbuf);
				}
			sprintf(outbuf, "%s|%s    %sParty:%s %-20s %swith%s%-15s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), line[1], back(BLK), fore(BLU));
			OUT(outbuf); NL;
		}
	}
	if(rpc->user.Hull) {
		sprintf(line[0], "%d:%d", rpc->HULL, rpc->user.Hull);
		sprintf(line[1], "%d:%d  %s", rpc->user.Cannon, rpc->user.Hull/50, rpc->user.Ram == 'Y' ? "(RAM)" : "");
		sprintf(outbuf, "%s|%s  %sWarship:%s %-9s        %sCannon:%s %-15s %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), line[1], back(BLK), fore(BLU));
		OUT(outbuf); NL;
	}
	if(strlen(rpc->user.Blessed)) {
		sprintf(outbuf, "%s|%s     %sNote:%s You were blessed by %-4s                 %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), rpc->user.Blessed, back(BLK), fore(BLU));
		OUT(outbuf); NL;
	}
	if(strlen(rpc->user.Cursed)) {
		sprintf(outbuf, "%s|%s     %sNote:%s You were cursed by %-4s                  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE), rpc->user.Cursed, back(BLK), fore(BLU));
		OUT(outbuf); NL;
	}
	sprintf(outbuf, "%s+%.52s+", fore(BLU), line[52]);
	OUT(outbuf); NL;
	if((c = more()) != 'Y')
		return;

	if(ANSI > 1) {
		OUT(back(BLK));
		OUT(fore(BLU));
	}
	else
		NORMAL;
	sprintf(line[0], "%*s%s", (int)(9 - strlen(table->class[rpc->class_origin]->Character[rpc->class_type]->Name)) / 2, "", table->class[rpc->class_origin]->Character[rpc->class_type]->Name);
	sprintf(outbuf, "%s|%s %sCurrent Stats  %-9s  All-Time  History %s%s|", fore(BLU), back(BLU), fore(YELLOW), line[0], back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s|%s    %sCalls:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE),
	  rpc->user.Current.Calls,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Calls[0].What, table->class[rpc->class_origin]->Character[rpc->class_type]->Calls[1].What,
	  table->class[0]->Character[0]->Calls[0].What, table->class[0]->Character[0]->Calls[1].What,
	  rpc->user.History.Calls,
	  back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s|%s    %sKills:%s %3u   %3u/%-4u  %3u/%-4u  %6lu  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE),
	  rpc->user.Current.Kills,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Kills[0].What, table->class[rpc->class_origin]->Character[rpc->class_type]->Kills[1].What,
	  table->class[0]->Character[0]->Kills[0].What, table->class[0]->Character[0]->Kills[1].What,
	  rpc->user.History.Kills,
	  back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf,"%s|%s   %sKilled:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE),
	  rpc->user.Current.Killed,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Killed[0].What, table->class[rpc->class_origin]->Character[rpc->class_type]->Killed[1].What,
	  table->class[0]->Character[0]->Killed[0].What, table->class[0]->Character[0]->Killed[1].What,
	  rpc->user.History.Killed,
	  back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s|%s %sRetreats:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE),
	  rpc->user.Current.Retreats,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Retreats[0].What, table->class[rpc->class_origin]->Character[rpc->class_type]->Retreats[1].What,
	  table->class[0]->Character[0]->Retreats[0].What, table->class[0]->Character[0]->Retreats[1].What,
	  rpc->user.History.Retreats,
	  back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s|%s      %sKOs:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE),
	  rpc->user.Current.KOs,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Brawls[0].What, table->class[rpc->class_origin]->Character[rpc->class_type]->Brawls[1].What,
	  table->class[0]->Character[0]->Brawls[0].What, table->class[0]->Character[0]->Brawls[1].What,
	  rpc->user.History.KO,
	  back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s|%s   %sJousts:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s%s|", fore(BLU), back(BLU), fore(bg), fore(WHITE),
	  rpc->user.Current.JoustWin,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Jousts[0].What, table->class[rpc->class_origin]->Character[rpc->class_type]->Jousts[1].What,
	  table->class[0]->Character[0]->Jousts[0].What, table->class[0]->Character[0]->Jousts[1].What,
	  rpc->user.History.Joust,
	  back(BLK), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s+%.45s+", fore(BLU), line[52]);
	OUT(outbuf); NL;
	paws = !PLAYER.Expert;
}

void MainMenu(void)
{
	static struct RPGmenu RPGmenu = { "Main", BLU, 16,
			{	{ '@', "Sysop Functions" },
				{ 'A', "Arena: Fight and Joust" },
				{ 'D', "Deep Dank Dungeon" },
				{ 'E', "Electronic Mail and Feedback" },
				{ 'G', "Gambling Casino" },
				{ 'L', "List of Top Users: Fame & Lame" },
				{ 'M', "Most Wanted List" },
				{ 'N', "Naval Adventures" },
				{ 'P', "Party/Gang Wars" },
				{ 'R', "Rob/Burglarize other users" },
				{ 'S', "Public Square (Shops, etc.)" },
				{ 'T', "Tiny's Tavern" },
				{ 'U', "User Configuration" },
				{ 'X', "X-terminate: Reroll character" },
				{ 'Y', "Your Statistics" },
				{ 'Z', "System Status" }
			}
	};
	FILE *fp;
	DOUBLE v, y;
	int i, immortal, max, rob = 0, t;
	char c;
	
	while(!cd()) {
		RPGserver(SERVER_GETUSER, (UBYTE *)IRSREC);
		RPGserver(SERVER_GETUSER, (UBYTE *)NEWREC);
		RPGserver(SERVER_GETUSER, (UBYTE *)SYSREC);
    	for(i = 1; i < 5; i++)
    		memset(RPC[0][i], 0, sizeof(struct RPC));
    	for(i = 0; i < 5; i++)
    		memset(RPC[1][i], 0, sizeof(struct RPC));
		from = '\0';
		switch((c=option(&RPGmenu))) {
			case '@':
				if(ACCESS(PLAYER.Access)->Sysop == 'Y')
					Sysop();
				break;

			case 'A':
				from = 'M';
				Arena();
				break;

			case 'D':
				if(dungeon) {
					if(ACCESS(acclvl)->RolePlay == 'Y' && PLAYER.Novice == 'Y' && dice(PLAYER.Level / 2) == 1) {
						if(ONLINE->HP < PLAYER.HP) {
							OUT("> You forgot to buy all your Hit Points."); NL; NL;
						}
						if(ONLINE->INT < 50) {
							OUT("> Since your character has low Intellect, the auto-mapping feature is not on."); NL; NL;
						}
						if(PLAYER.Gold == 0. && PLAYER.Bank > 0.) {
							OUT("> Take a small amount of money with you the next time."); NL; NL;
						}
						if(PLAYER.Gold > 1. && PLAYER.Bank == 0.) {
							OUT("> Taking all your money with you?  Good luck!"); NL; NL;
						}
						OUT("> Try using your Cursor Arrow keys to move instead of N, S, E, W."); NL; NL;
						paused();
					}
					dungeon--;
					nest = 0;
					Dungeon(PLAYER.Level - 1);
				}
				else {
					OUT("You have run out of dungeon turns."); NL;
					paws = !PLAYER.Expert;
				}
				break;

			case 'E':
				sprintf(prompt, "%sSend mail to (%s=Sysop): ", fore(CYN), SYSID);
				if(GetRPC(RPC[1][0])) {
					Editor(RPC[1][0], 99);
					for(i = 1; i < 999; i++) {
						sprintf(outbuf, "%s.%03d", ENEMY.ID, i);
						sprintf(filename, "mail/%s", outbuf);
						if(!(fp = fopen(filename, "r")))
							break;
						fclose(fp);
					}
					WriteMail(filename);
				}
				break;

			case 'G':
				Casino();
				break;

			case 'L':
				Lists();
				break;

			case 'M':
				RPGserver(SERVER_GETHISCORE, (UBYTE *)table->HiScore);
				cls();
				for(immortal = table->NumClass - 1; immortal > 0 && strcasecmp("IMMORTAL", table->class[immortal]->Origin); immortal--);
				sprintf(outbuf,"%s%s ID   Player's Handle           Class    Lvl  Status  Party               ", back(BLU), fore(WHITE));
				OUT(outbuf); NORMAL; NL;
				sprintf(outbuf, "%s%s--------------------------------------------------------------------------", back(BLU), fore(GRY));
				OUT(outbuf); NORMAL; NL;
				strcpy(userlevel.Current, "999");
				i = 2;
				while(RPGserver(SERVER_GETUSERLEVEL,(UBYTE *)&userlevel)) {
					i++;
//					if(atoi(userlevel.Current)==1 && i==rows)
//						break;
					if(i % rows == 0) {
						i++;
						if((c = more()) == 'N')
							break;
					}
					strcpy(ENEMY.ID, userlevel.ID);
					RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY);
					LoadRPC(RPC[1][0]);
					OriginName(RPC[1][0]->user.Class, line[0],line[1]);
					max = (strcasecmp(table->class[RPC[1][0]->class_origin]->Origin, "IMMORTAL")) ? FALSE : RPC[1][0]->class_type + 1;
					sprintf(outbuf, "%s%-4.4s  %-22.22s  %-9.9s  %3u  %-6.6s  %s%.20s"
						, strncmp(ENEMY.ID,PLAYER.ID,sizeof(PLAYER.ID))
						   ? fore(max == table->class[RPC[1][0]->class_origin]->NumCharacter
						     ? YELLOW
							 : strncmp(ENEMY.Handle,table->HiScore->Who[0],sizeof(ENEMY.Handle))
							   ? GRY
						       : MAGENTA)
						   : fore(CYAN)
						, ENEMY.ID, ENEMY.Handle
						, line[1], ENEMY.Level
						, strcmp(ENEMY.Status, "jail") ? (strlen(ENEMY.Status) ? " Dead " : "Alive!") : " Jail "
						, strncmp(ENEMY.Gang, PLAYER.Gang, sizeof(PLAYER.Gang)) ? back(BLK) : ANSI > 1 ? back(RED) : "\33[7m"
						, strlen(ENEMY.Gang) ? (char *)ENEMY.Gang : "None");
					OUT(outbuf);
					NORMAL; NL;
				}
				if(c != 'N')
					paused();
				break;

			case 'N':
				Naval();
				break;

			case 'P':
				Party();
				break;

			case 'Q':
				if(!cd()) {
					beep();
					sprintf(outbuf,"%sAre you sure (Y/N)? ",fore(YELLOW));
					OUT(outbuf);
					c=inkey('N','Y');
					NL;NL;
					if(c=='Y') {
						logoff=TRUE;
						strcpy(reason,"had something better to do");
					}
				}
				break;

			case 'R':
				if(ACCESS(acclvl)->RolePlay == 'Y' && PLAYER.Novice != 'Y') {
					OUT("It is a hot, moonless night."); NL;
					OUT("A city guard walks down another street."); NL; NL;
					modf((value(WEAPON(ONLINE)->Value + ARMOR(ONLINE)->Value, ONLINE->CHA) + PLAYER.Gold + PLAYER.Bank - PLAYER.Loan) / (6. + ONLINE->user.MySteal), &y);
					sprintf(prompt, "%sWho are you going to rob? ", fore(CYN));
					if(GetRPC(RPC[1][0])) {
						if(!strcmp(PLAYER.ID, ENEMY.ID))
							break;
						if((BYTE)(PLAYER.Level - ENEMY.Level) > 3) {
							OUT("You can only rob someone higher or up to three levels below you."); NL;
							paws = !PLAYER.Expert;
							break;
						}
						if(ENEMY.Novice == 'Y') {
							OUT("You can not rob from a novice player."); NL;
							paws = !PLAYER.Expert;
							break;
						}
						sprintf(outbuf, "You case %s's joint out.", ENEMY.Handle);
						OUT(outbuf); NL;
						Delay(75);
						modf((value(WEAPON(RPC[1][0])->Value + ARMOR(RPC[1][0])->Value + ENEMY.Gold, ONLINE->CHA)) / 5., &v);
						v++;
						if(RPC[1][0]->weapon_origin == 0 && RPC[1][0]->weapon_type == 0)
							v = 0.;
						if(RPC[1][0]->armor_origin == 0 && RPC[1][0]->armor_type == 0)
							v = 0.;
						if(dice(ONLINE->INT) > 5 && v < y) {
							OUT("But you decide it is not worth the effort."); NL;
							paws = !PLAYER.Expert;
							break;
						}

						sprintf(outbuf, "The goods are in %s%s protected by %s%s.", AN(REALESTATE(ENEMY.RealEstate)->Name), REALESTATE(ENEMY.RealEstate)->Name, AN(SECURITY(ENEMY.Security)->Name), SECURITY(ENEMY.Security)->Name);
						OUT(outbuf); NL; NL;
						sprintf(prompt, "%sAttempt to steal (Y/N)? ", fore(CYN));
						OUT(prompt);
						c = inkey('N', 'N');
						NL;

						if(c == 'Y') {
							NL;
							if(!RPGserver(SERVER_LOCK, (UBYTE *)ENEMY.ID))
								break;
							OUT("You slide into the shadows and make your attempt");
							Delay(50);
							max = 5 * (ENEMY.Security + 1) + ENEMY.RealEstate + rob;
							// max += (BYTE)(ENEMY.Level - PLAYER.Level) / (CLASS(ONLINE)->Steal + 1);
							// rob++;
							t = (long)(ONLINE->user.MySteal * ONLINE->DEX * ONLINE->INT) / 10000L;
							for(i = 0; i < ONLINE->user.MySteal; i++) {
								OUT(".");
								Delay(50);
								if(dice(100) < 100)
									t += dice(PLAYER.Level + 1);
								else
									t += 5 * ENEMY.Security;
							}
							NL;
							if(strcasecmp(PLAYER.Email, ENEMY.Email) == 0)
								t = 0;
							Delay(50);
							if(t > max) {
								PLAYER.Gold += v;
								PLAYER.History.Stole++;
								sprintf(outbuf, "You make it out with %s worth of stuff!", money(v, ANSI));
								OUT(outbuf); NL;
								Delay(100);
								sprintf(outbuf, "%srobbed %s", fore(CYN), ENEMY.Handle);
								news(outbuf);
								ENEMY.Gold = 0.;
								if(RPC[1][0]->weapon_origin) {
									RPC[1][0]->weapon_type = table->weapon[RPC[1][0]->weapon_origin]->Item[RPC[1][0]->weapon_type]->Class / 2;
									RPC[1][0]->weapon_origin = 0;
								}
								else
									if(RPC[1][0]->weapon_type)
										RPC[1][0]->weapon_type--;
								sprintf(ENEMY.Weapon, "%s.%s", table->weapon[0]->Origin, table->weapon[0]->Item[RPC[1][0]->weapon_type]->Name);
								ENEMY.WCmod = 0;
								if(RPC[1][0]->armor_origin) {
									RPC[1][0]->armor_type = table->armor[RPC[1][0]->armor_origin]->Item[RPC[1][0]->armor_type]->Class / 2;
									RPC[1][0]->armor_origin = 0;
								}
								else
									if(RPC[1][0]->armor_type)
										RPC[1][0]->armor_type--;
								sprintf(ENEMY.Armor,"%s.%s", table->armor[0]->Origin, table->armor[0]->Item[RPC[1][0]->armor_type]->Name);
								ENEMY.ACmod = 0;
								RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
								numline = 0;
								sprintf(line[numline++], "%s robbed you!", PLAYER.Handle);
								note(ENEMY.ID);
							}
							else {
								sound("arrested", 64, 1);
								OUT("A guard catches you and throws you into jail!"); NL;
								Delay(75);
								OUT("You might be released by your next call."); NL; NL;
								Delay(75);
								numline = 0;
								sprintf(line[numline++], "%s was caught robbing you!", PLAYER.Handle);
								note(ENEMY.ID);
								sprintf(reason, "caught robbing %s", ENEMY.Handle);
								strcpy(PLAYER.Status, "jail");
								logoff = TRUE;
							}
						}
					}
				}
				else {
					OUT("You are not allowed to rob other users."); NL;
					paws = !PLAYER.Expert;
				}
				break;

			case 'S':
				from = 'M';
				Square();
				break;

			case 'T':
				if(ACCESS(acclvl)->RolePlay == 'Y' && tiny == 3) {
					OUT("Tiny went to Butler Hospital.  The Tavern will re-open later."); NL;
					paws = !PLAYER.Expert;
					break;
				}
				Tavern();
				break;

			case 'U':
				do {
					cls();
					sprintf(outbuf, "%s%s  Suppress Menus: %s%s", bracket(1), fore(CYN), fore(WHITE), PLAYER.Expert ? "ON" : "OFF");
					OUT(outbuf); NL;
					sprintf(outbuf, "%s%s  Emulation/Rows: %s%s/%u", bracket(2), fore(CYN), fore(WHITE), PLAYER.Emulation == 1 ? "DEC-VT220" : PLAYER.Emulation == 2 ? "IBM-ANSI" : PLAYER.Emulation == 3 ? "WIN-rpgclient" : "???", PLAYER.Rows);
					OUT(outbuf); NL;
					sprintf(outbuf, "%s%s  Logon Password: %s%s", bracket(3), fore(CYN), fore(WHITE), "???");
					OUT(outbuf); NL; NL;
					sprintf(prompt, "%sSelect (1-3): ", fore(CYN));
					OUT(prompt);
					if(ins(1))
	  					break;
					NL; NL;
					i = atoi(inbuf);
					switch(i) {
						case 1:
							PLAYER.Expert = PLAYER.Expert ? FALSE : TRUE;
							break;
						case 2:
							emulation();
							break;
						case 3:
							sprintf(prompt, "%sNew password: ", fore(CYN));
							OUT(prompt);
							myecho = FALSE;
							ins(sizeof(PLAYER.Password)); NL;
							if(strlen(inbuf) > 2) {
								strupr(inbuf);
								strcpy(line[0], inbuf);
								sprintf(prompt, "%sRe-enter to verify: ", fore(CYN));
								OUT(prompt);
								myecho = FALSE;
								ins(sizeof(PLAYER.Password)); NL;
								if(!strcasecmp(line[0], inbuf)) {
									strncpy(PLAYER.Password, inbuf, sizeof(PLAYER.Password));
									OUT("Ok."); NL;
									Delay(50);
								}
								else
									beep();
							}
							myecho = TRUE;
							break;
					}
				} while(i);
				break;

			case 'X':
				if(ACCESS(acclvl)->RolePlay == 'Y') {
					beep();
					sprintf(outbuf, "%sAre you sure (Y/N)? ", fore(YELLOW));
					OUT(outbuf);
					c = inkey('N', 'Y');
					NL; NL;
					if(c == 'Y') {
						PLAYER.History.Retreats += PLAYER.Level;
						reroll(&PLAYER);
						LoadRPC(ONLINE);
						assign(200);
						PLAYER.Coward = TRUE;
					}
				}
				break;

			case 'Y':
				from = 'M';
				charstats(RPC[1][0]);
				break;

			case 'Z':
				type("msg/System", TRUE);
				//execl("/usr/bin/uptime",">&/dev/null",NULL);
				paws = !PLAYER.Expert;
				break;

			default:
				beep();
				break;
		}
	}
}
