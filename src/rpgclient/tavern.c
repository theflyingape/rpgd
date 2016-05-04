/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * tavern.c
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

int Punch(struct RPC *rpc)
{
	int power;

	power = (rpc->user.Level + rpc->STR / 10) / 2;
	return(power + dice(power));
}

void KnockOut(struct RPC *rpc,struct RPC *enemy)
{
	int m;

	sprintf(outbuf, "%s knock%s %s out!", (rpc == ONLINE ? rpc->He : rpc->user.Handle), (rpc != ONLINE ? "s" : ""), (enemy == ONLINE ? enemy->him : enemy->user.Handle));
	OUT(outbuf); NL; NL;
	modf(pow(2., enemy->user.Level - 2.) * 1000. /9., &d);
	rpc->user.Experience += d;
	if(rpc != ONLINE && rpc->user.Loan > 0.) {
		rpc->user.Loan -= enemy->user.Gold;
		if(rpc->user.Loan < 0.) {
			rpc->user.Gold -= rpc->user.Loan;
			rpc->user.Loan = 0.;
		}
	}
	else
		rpc->user.Gold += enemy->user.Gold;
	rpc->user.Current.KOs++;
	rpc->user.History.KO++;
	RPGserver(SERVER_PUTUSER, (UBYTE *)&rpc->user);
	if(rpc == ONLINE) {
		if(enemy->user.Gold) {
			sprintf(outbuf, "You get %s %s was carrying.", money(enemy->user.Gold, ANSI), enemy->he);
			OUT(outbuf); NL;
		}
		sprintf(outbuf, "You get %.8g experience.", d);
		OUT(outbuf); NL; NL;
	}
	if(enemy == ONLINE) {
		m = abs(ONLINE->BP);
		while(m > 10)
			m /= 2;
		sprintf(outbuf, "You are unconscious for %d minute%s", m, (m != 1 ? "s" : ""));
		OUT(outbuf);
/*  dial-up modem days are over ... this used to penalize player's online time
		if(m > PLAYER.LastTime) {
			PLAYER.LastDate--;
			PLAYER.LastTime += (2400 - m);
		}
		else
			PLAYER.LastTime -= m;
		if(PLAYER.LastTime % 100 > 59)
			PLAYER.LastTime -= 40;
*/		while(m) {
			OUT(".");
			Delay(50);
			m--;
		}
		NL; NL;
	}
	enemy->user.Gold = 0.;
	enemy->user.Current.KOed++;
	RPGserver(SERVER_PUTUSER, (UBYTE *)&enemy->user);
}

void Brawl(struct RPC *rpc,struct RPC *enemy)
{
	int attempts, damage, standby;

	if(dice(100) >= (50 + rpc->DEX / 2)) {
		sprintf(outbuf, "%s duck%s %s punch.", (enemy == ONLINE) ? enemy->He : enemy->user.Handle, (enemy != ONLINE ? "s" : ""), rpc->his);
		OUT(outbuf); NL; NL;
		Delay(50);
		standby = (rpc == ONLINE) ? 0 : 1;
		for(attempts = 0; attempts < 5; attempts++)
			if(RPGserver(SERVER_RNDUSER, (UBYTE *)&RPC[standby][1]->user)) {
				if(!strlen(RPC[standby][1]->user.Status) && strcmp(RPC[standby][1]->user.Handle, PLAYER.Handle) && strcmp(RPC[standby][1]->user.Handle, ENEMY.Handle)) {
					LoadRPC(RPC[standby][1]);
					sprintf(outbuf, "%s hit%s %s!", rpc->He, (rpc != ONLINE ? "s" : ""), RPC[standby][1]->user.Handle);
					OUT(outbuf); NL; NL;
					Delay(50);
					if((RPC[standby][1]->BP -= Punch(rpc)) < 1)
						KnockOut(rpc, RPC[standby][1]);
					else {
						sprintf(outbuf, "Uh oh!  Here comes %s!", RPC[standby][1]->user.Handle);
						OUT(outbuf); NL; NL;
						Delay(50);
						Brawl(RPC[standby][1],rpc);
					}
					break;
				}
			}
	}
	else {
		damage = Punch(rpc);
		sprintf(outbuf, "%s punch%s %s for %d points.", rpc->He, (rpc != ONLINE ? "es" : ""), enemy->him, damage);
		OUT(outbuf); NL; NL;
		if((enemy->BP -= damage)<1)
			KnockOut(rpc, enemy);
	}
}

void Tavern(void)
{
	static struct RPGmenu RPGmenu = { "Tavern", BRN, 9,
					{	{ 'B', "Brawl another user" },
						{ 'E', "Eavesdrop on the arguments" },
						{ 'J', "Jump into the arguments" },
						{ 'G', "Guzzle beer" },
						{ 'L', "List user bounties" },
						{ 'P', "Post your own bounty" },
						{ 'S', "Swear at Tiny" },
						{ 'T', "Today's news" },
						{ 'Y', "Yesterday's news" }
					}
	};
	FILE *newfp, *oldfp;
	DOUBLE m, tax;
	int i, j;
	char *s;
	char c;

	if(ACCESS(acclvl)->RolePlay == 'Y' && PLAYER.Novice == 'Y' && dice(PLAYER.Level / 2) == 1) {
		if(PLAYER.Gold > 0.) {
			OUT("> Carrying extra money around here is only good for posting a bounty"); NL;
			OUT("  on someone or buying drinks & tips from Tiny."); NL; NL;
		}
		paws = !PLAYER.Expert;
	}
	JOUST(PLAYER.Level);
	d *= 3. + PLAYER.Level / 10;
	if(ACCESS(acclvl)->RolePlay == 'Y' && PLAYER.Gold > d) {
		NL;
		tax = PLAYER.Gold - d;
		sprintf(outbuf, "%s%s, the tax collector, spots your bulging money purse and", fore(CYN), IRSREC->Handle);
		OUT(outbuf); NL;
		sprintf(outbuf, "says, \"%sAh, it is time to pay your taxes!%s\"  You check out the two burly", fore(WHITE), fore(CYN));
		OUT(outbuf); NL;
		sprintf(outbuf, "guards who stand ready to enforce %s's will.", SYSREC->Handle);
		OUT(outbuf); NL; NL;
		Delay(100);
		sprintf(outbuf, "The tax will cost you %s.", money(tax, ANSI));
		OUT(outbuf); NL; NL;
		Delay(100);
		PLAYER.Gold -= tax;
		ENEMY.Bank += tax;
		paws = !PLAYER.Expert;
	}
	sprintf(outbuf, "tavern%d", dice(4));
	sound2(outbuf, 0);

	while((c = option(&RPGmenu)) != 'Q') {
		for(i = 1; i < 5; i++)
			memset(RPC[0][i], 0, sizeof(struct RPC));
		for(i = 0; i < 5; i++)
			memset(RPC[1][i], 0, sizeof(struct RPC));

		switch(c) {
			case 'B':
				if(!brawl) {
					OUT("You are out of brawls."); NL;
					paws = !PLAYER.Expert;
					break;
				}
				while(c != 'Y') {
					ONLINE->BP = PLAYER.HP / 10;
					sprintf(prompt, "%sPick a fight with what user (?=list)? ", fore(CYN));
					if(!GetRPC(RPC[1][0]))
						break;
					if(!strcmp(PLAYER.ID,ENEMY.ID)) {
						OUT("You want to hit yourself?"); NL; NL;
						continue;
					}
					if((BYTE)(PLAYER.Level-ENEMY.Level)>3) {
						OUT("You can only brawl someone higher or up to three levels below you."); NL; NL;
						continue;
					}
					sprintf(outbuf, "%sName:%s %-22s      You:", fore(GRN), fore(WHITE), ENEMY.Handle);
					OUT(outbuf); NL;
					sprintf(outbuf, "%sLevel:%s %-22d     %-2d", fore(GRN), fore(WHITE), ENEMY.Level, PLAYER.Level);
					OUT(outbuf); NL;
					sprintf(outbuf, "%sKnock out points:%s %-15d %-3d", fore(GRN), fore(WHITE), RPC[1][0]->BP, ONLINE->BP);
					OUT(outbuf); NL; NL;
					sprintf(prompt, "%sAre you sure (Y/N)? ", fore(CYN));
					OUT(prompt);
					c = inkey('N','N');
					NL; NL;
				}
				if(c != 'Y')
					break;
				if(!RPGserver(SERVER_LOCK, (UBYTE *)ENEMY.ID))
					break;
				brawl--;
				if((ONLINE->DEX / 2 + dice(ONLINE->DEX / 2)) > (RPC[1][0]->DEX / 2 + dice(RPC[1][0]->DEX / 2))) {
					OUT("You get the first punch."); NL; NL;
					Brawl(ONLINE, RPC[1][0]);
					if(ONLINE->BP > 0 && RPC[1][0]->BP > 0)
						Brawl(RPC[1][0], ONLINE);
				}
				else {
					sprintf(outbuf, "%s gets the first punch.", RPC[1][0]->He);
					OUT(outbuf); NL; NL;
					Brawl(RPC[1][0], ONLINE);
				}
				while(ONLINE->BP > 0 && RPC[1][0]->BP > 0) {
					sprintf(prompt, "%s<P> Punch %s, <G> Give it up, <Y> Your status: ", fore(CYN), RPC[1][0]->him);
					OUT(prompt);
					c = inkey('P', 'G');
					NL; NL;
					if(c == 'G') {
						OUT("We can't all be Rocky, eh?"); NL; NL;
						break;
					}
					if(c == 'Y') {
						sprintf(outbuf, "Your knock out points: %d", ONLINE->BP);
						OUT(outbuf); NL; NL;
					}
					if(c != 'P')
						continue;
					Brawl(ONLINE, RPC[1][0]);
					if(ONLINE->BP > 0 && RPC[1][0]->BP > 0)
						Brawl(RPC[1][0], ONLINE);
				}
				if(!PLAYER.Expert)
					paused();
				break;

			case 'E':
				type("msg/Tavern", TRUE);
				break;

			case 'J':
				if(ACCESS(acclvl)->RolePlay != 'Y') {
					OUT("The mob yells, \"Get outta here, wimp!\""); NL; NL;
					break;
				}
				Editor(RPC[1][0], 5);
				if(numline) {
					if((oldfp = fopen("msg/Tavern", "r"))) {
						j = 0;
						while(fgets(inbuf, INBUF_SIZE, oldfp))
							if(!strncmp(inbuf, "-=>*<=-", 7))
								j++;
						fclose(oldfp);
						newfp = fopen("msg/Tavern.new", "w");
						oldfp = fopen("msg/Tavern", "r");
						if(j > 9) {
							j = 2;
							while(fgets(inbuf, INBUF_SIZE, oldfp))
								if(!strncmp(inbuf, "-=>*<=-", 7)) {
									j--;
									if(!j)
										break;
								}
							fputs(inbuf, newfp);
						}
						while(fgets(inbuf, INBUF_SIZE, oldfp))
							fputs(inbuf, newfp);
						fclose(newfp);
						fclose(oldfp);
						remove("msg/Tavern");
						rename("msg/Tavern.new", "msg/Tavern");
					}
					if((newfp = fopen("msg/Tavern", "a"))) {
						fprintf(newfp, "\33[0;37m\n");
						fprintf(newfp, "-=>*<=-\n\n");
						fprintf(newfp, "%s says,\n", PLAYER.Handle);
						for(i = 0; i < numline; i++)
							fprintf(newfp, "  %s\n", line[i]);
						fclose(newfp);
					}
				}
				break;

			case 'G':
				OUT("Tiny, the 7'2\" barkeep, pours you a beer."); NL; NL;
				sprintf(prompt, "%sHow much will you pay? ", fore(CYN));
				OUT(prompt);
				if(ins(16))
					break;
				NL; NL;
				modf(atof(inbuf), &d);
				if(d < 1. || d > PLAYER.Gold) {
					OUT("Tiny pours the beer on you and kicks you out of his bar."); NL;
					Delay(100);
					return;
				}
				OUT("He grunts and hands you your beer."); NL;
				PLAYER.Gold -= d;
				sprintf(outbuf, "%sHe says, \"%s", fore(CYN), fore(GRY));
				i = d;
				if(i > 21)
					i = dice(21);
				switch(i) {
					case 1:
						strcat(outbuf, "More stamina will yield more hit points");
						break;
					case 2:
						strcat(outbuf, "More intellect will yield more spell power");
						break;
					case 3:
						strcat(outbuf, "You can sell items for better prices with higher charisma");
						break;
					case 4:
						strcat(outbuf, "You don't miss as often with higher agility");
						break;
					case 5:
						strcat(outbuf, "You can do more damage in battle with higher stamina");
						break;
					case 6:
						strcat(outbuf, "Spells don't fail as often with higher intellect");
						break;
					case 7:
						strcat(outbuf, "Higher agility yields higher jousting ability");
						break;
					case 8:
						strcat(outbuf, "Fishing can get better results from higher charisma");
						break;
					case 9:
						strcat(outbuf, "Real Estate and Security help protect your investments");
						break;
					case 10:
						strcat(outbuf, "Higher baud rates yield faster screen displays");
						break;
					case 11:
						strcat(outbuf, "Crying will not change the world");
						break;
					case 12:
						strcat(outbuf, "Backstabs swish more than you wish");
						break;
					case 13:
						strcat(outbuf, "Dungeon maps fall more into the hands of the lucky");
						break;
					case 14:
						strcat(outbuf, "Higher intellect calculates opponent's hit points more accurately");
						break;
					case 15:
						strcat(outbuf, "You need at least 50 Intellect points for Magic Mapping");
						break;
					case 16:
						strcat(outbuf, "Put a \1 to add RED letters to any message you post.");
						break;
					case 17:
						strcat(outbuf, "Put a \2 to add GREEN letters to any message you post.");
						break;
					case 18:
						strcat(outbuf, "Put a \4 to add BLUE letters to any message you post.");
						break;
					case 19:
						strcat(outbuf, "Put a \10 to add BOLD letters to any message you post.");
						break;
					case 20:
						strcat(outbuf, "Put a \100 to add BLINKING letters to any message you post.");
						break;
					default:
						strcat(outbuf, "I'll have more hints tomorrow");
						break;
				}
				strcat(outbuf, ".");
				strcat(outbuf, fore(CYN));
				strcat(outbuf, "\"");
				OUT(outbuf); NL;
				paused();
				break;

			case 'L':
				cls();
				sprintf(outbuf, "               %s--=:))%s Tiny's Bounty List %s((:=--%s", fore(GRN), fore(WHITE), fore(GRN), fore(GRY));
				OUT(outbuf); NL; NL;
				strcpy(userlevel.Current, "999");
				while(RPGserver(SERVER_GETUSERLEVEL, (UBYTE *)&userlevel) && atoi(userlevel.Current) > 1) {
					strcpy(ENEMY.ID, userlevel.ID);
					RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY);
					if(ENEMY.Bounty > 0.) {
						strcpy(RPC[1][1]->user.ID, ENEMY.BountyWho);
						RPGserver(SERVER_GETUSER, (UBYTE *)&RPC[1][1]->user);
						sprintf(outbuf, "%22s is worth %s to %s", ENEMY.Handle, money(ENEMY.Bounty, ANSI), RPC[1][1]->user.Handle);
						OUT(outbuf); NL;
					}
				}
				paws = !PLAYER.Expert;
				break;

			case 'P':
				if(PLAYER.Gold < 1. || ACCESS(acclvl)->RolePlay != 'Y') {
					OUT("You'll need cash to post a bounty."); NL;
					paws = !PLAYER.Expert;
					break;
				}
				sprintf(prompt, "%sPost a bounty on who? ", fore(CYN));
				if(!GetRPC(RPC[1][0]))
					break;
				if(ENEMY.ExpLevel < 10 || strcmp(PLAYER.RemoteClient, ENEMY.RemoteClient) == 0) {
					OUT("The crowd laughs at your gesture."); NL;
					Delay(50);
					OUT("Tiny snorts, \"Be for real.\""); NL;
					Delay(50);
					paws = !PLAYER.Expert;
					break;
				}
				if(ENEMY.Bounty > 0.) {
					sprintf(outbuf, "%s already has a %s bounty on %s.", ENEMY.Handle, money(ENEMY.Bounty, ANSI), RPC[1][0]->him);
					OUT(outbuf); NL;
					paws = !PLAYER.Expert;
					break;
				}
				JOUST(ENEMY.ExpLevel);
				m = 5. * d + WEAPON(RPC[1][0])->Value / 10. + ARMOR(RPC[1][0])->Value;
				strcpy(line[0], money(m, 0));
				if((s = strchr(line[0], ',')))
					*s = '\0';
				c = *(line[0] + strlen(line[0]) - 1);
				i = atoi(line[0]);
				i = (i < 5) ? 1 : (i < 10) ? 5 : (i < 50) ? 10 : (i < 100) ? 50 : (i < 500) ? 100 : (i < 1000) ? 500 : (i < 5000) ? 1000 : 5000;
				sprintf(line[0], "%d%c" , i, c);
				gold(&m, line[0]);
				if(m > PLAYER.Gold)
					m = PLAYER.Gold;
				sprintf(prompt, "%sBounty [MAX=%s%s]? ", fore(CYN), money(m, ANSI), fore(CYN));
				OUT(prompt);
				ins(16);
				NL; NL;
				gold(&d, inbuf);
				if(!strcasecmp(inbuf, "max") || *inbuf == '=') {
					sound("max", 64, 0);
					d = m;
				}
				if(d < 1. || d > m)
					d = 0.;
				if(d > 0.) {
					OUT("Tiny posts your bounty for all to see."); NL;
					paws = !PLAYER.Expert;
					PLAYER.Gold -= d;
					ENEMY.Bounty = d;
					strcpy(ENEMY.BountyWho, PLAYER.ID);
					RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
					sprintf(outbuf, "%sposted a bounty on %s", fore(CYN), ENEMY.Handle);
					news(outbuf);
				}
				break;

			case 'S':
				if(ACCESS(acclvl)->RolePlay != 'Y')
					break;
				tiny++;
				paws = !PLAYER.Expert;
				OUT("You call to Tiny, then ");
				if(tiny == 1) {
					OUT("yell, \"Freak!\""); NL; NL;
					Delay(100);
					switch(dice(2)) {
						case 1:
							OUT("Tiny stares off into empty space, ignoring your wimpy comment.");
							break;
						case 2:
							OUT("Tiny points at his massive, flexed bicep and laughs at your jest.");
							break;
					}
					NL;
					break;
				}
				if(tiny == 2) {
					OUT("thumb your nose."); NL; NL;
					Delay(100);
					switch(dice(2)) {
						case 1:
							OUT("Annoyed, Tiny looks down at his furry feet and counts, \"100, 99, 98,...\"");
							break;
						case 2:
							OUT("The former Champion Ogre glances you over and murmurs to himself, \"Not good for");NL;
							OUT("business.\"");
							break;
					}
					NL;
					break;
				}
				if(PLAYER.Gender == 'M')
					OUT("jest, \"What you looking at, wart-face!\"");
				else
				if(PLAYER.Gender == 'F')
					OUT("yell, \"Yo, mama-boy, get me a clean glass!\"");
				else
					OUT("snarl, \"You uglier than me, fat boy!\"");
				NL;
				if(!cd())
					Delay(100);
				OUT("Uh, oh!");
				sound("challenge", 64, 1);
				if(!cd())
					Delay(50);
				OUT("  Here comes Tiny!");
				sound2("tiny", 0);
				if(!cd())
					Delay(50);
				OUT("  And he doesn't look friendly..."); NL; NL;
				if(!cd())
					Delay(50);
				switch(dice(3)) {
					case 1:
						OUT("\"When I'm through with you, your mama won't be able to identify the remains.\"");
						break;
					case 2:
						OUT("He lets off a heavy sigh and says, \"I am getting too old for this.\"");
						break;
					case 3:
						OUT("\"Never rub another man's rhubarb!\"");
						break;
				}
				NL; NL;
				strcpy(ENEMY.ID, "_TT");
				RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY);
				LoadRPC(RPC[1][0]);
				if(!cd())
					Delay(100);
				OUT("As he towels his hands dry from washing the day's glasses, he warns, \"Another"); NL;
				OUT("fool did something like that to me, once, and got all busted up.");
				if(strlen(ENEMY.Status)) {
					strcpy(RPC[1][4]->user.ID, ENEMY.Status);
					if((RPGserver(SERVER_GETUSER, (UBYTE *)&RPC[1][4]->user))) {
						OUT("  I think it"); NL;
						sprintf(outbuf, "was %s, and it took me a week to clean up the blood!\"", RPC[1][4]->user.Handle);
						OUT(outbuf);
						memset(RPC[1][4], 0, sizeof(struct RPC));
					}
					else
						memset(ENEMY.Status, 0, sizeof(ENEMY.Status));
				}
				NL; NL;
				if(!cd())
					Delay(300);
				OUT("He points to a weapon hanging over the mantlepiece and says, \"Lookee there, he"); NL;
				sprintf(outbuf, "tried to use that %s on me, but it wasn't good enough.\"", WEAPON(RPC[1][0])->Name);
				OUT(outbuf); NL; NL;
				if(!cd())
					Delay(300);
				RPGserver(SERVER_RNDUSER, (UBYTE *)&RPC[1][4]->user);
				if(strcmp(PLAYER.ID,RPC[1][4]->user.ID) && !strlen(RPC[1][4]->user.Status)) {
					OUT("The patrons move in closer to witness the forthcoming slaughter, except for"); NL;
					sprintf(outbuf,"%s who is busy raiding the bar of its beer and nuts.", RPC[1][4]->user.Handle);
					OUT(outbuf); NL; NL;
					memset(RPC[1][4], 0, sizeof(struct RPC));
					if(!cd())
						Delay(225);
				}
				sprintf(outbuf, "You hear %s cry, \"I'll pay fifteen-to-one on the challenger!\"", IRSREC->Handle);
				OUT(outbuf); NL; NL;
				if(!cd())
					Delay(150);
				OUT("The crowd roars with laughter...");
				if(!cd())
					Delay(75);
				OUT(" you are not amused."); NL; NL;
				if(!cd())
					Delay(75);
				OUT("When Tiny removes his tunic to reveal a massive, but heavily scarred chest,"); NL;
				if((PLAYER.Spell & TELEPORT_SPELL) && (ONLINE->user.MyMagic == 1 || (ONLINE->user.MyMagic > 1 && ONLINE->SP >= MAGIC(7)->Power[(ONLINE->user.MyMagic == 4) ? 1 : 0]))) {
					switch(ONLINE->user.MyMagic) {
						case 1:
							OUT("you start fumbling through your pockets for that Teleport Wand...");
							break;
						case 2:
							OUT("you unroll a parchment titled, \"Teleportation is the safest way to travel.\"");
							break;
						default:
							OUT("you try to recall the words for Teleportation...");
							break;
					}
					NL; NL;
					if(!cd())
						Delay(210);
					sprintf(prompt, "%sAttempt to Teleport (Y/N)? ", fore(CYN));
					OUT(prompt);
					if( !(c = inkey('N', 'N')))
						break;
					NL; NL;
					if(c == 'Y') {
						if(ONLINE->user.MyMagic == 1 && dice(100 + 2 * (10)) < 100) {
							OUT("Your wand smokes as you cast the spell."); NL; NL;
							PLAYER.Spell -= TELEPORT_SPELL;
							RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
							Delay(25);
						}
						if(ONLINE->user.MyMagic == 2 && dice(5) == 1) {
							OUT("Your scroll burns as you cast the spell."); NL; NL;
							PLAYER.Spell -= TELEPORT_SPELL;
							RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
							Delay(25);
						}
						if(dice(100) > ONLINE->INT-1) {
							OUT("Fssst!  Your spell fails!"); NL; NL;
							Delay(25);
						}
						else
							break;
					}
				}
				else {
					OUT("you look for an exit, but there is none to be found..."); NL; NL;
					if(!cd())
						Delay(150);
				}
				OUT("Tiny puts a finger in your face and says, ");
				switch(dice(3)) {
					case 1:
						OUT("\"Want a shot at the title?\"");
						break;
					case 2:
						OUT("\"Better drop me on the first shot.\"");
						break;
					case 3:
						OUT("\"Let's see what you got!\"");
						break;
				}
				NL; NL;
				if(!cd())
					Delay(150);
				strcpy(ENEMY.ID, "_TT");
				ENEMY.Gender = 'M';
				strcpy(ENEMY.Class, "TAVERN.Ogre");
				ENEMY.Level = SYSREC->Level * 4 / 5;
				ENEMY.Spell = HEAL_SPELL | CURE_SPELL;
				ENEMY.XSpell = MANA_STEALING_XSPELL;
				CreateRPC(RPC[1][0]);
				ENEMY.Level = SYSREC->Level;
				ENEMY.WCmod = 50 - WEAPON(RPC[1][0])->Class;
				RPC[1][0]->SP = MAGIC(12)->Power[(RPC[1][0]->user.MyMagic < 4) ? 0 : 1];
				ENEMY.Gold = 1.;
				from = 'T';
				Battle();
				if(RPC[1][0]->HP) {
					sound("morph", 64, 1);
					if(WEAPON(ONLINE)->Class < 10) {
						sprintf(outbuf, "He laughs at your futility and throws your carcass into the fireplace...");
						OUT(outbuf); NL;
						Delay(100);
						broadcast("creamated");
						RPGserver(SERVER_DELUSER,(UBYTE *)&PLAYER);
						sprintf(outbuf, "NEVER TO BE SEEN AGAIN.");
						OUT(outbuf); NL;
						RPGserver(SERVER_DISCONNECT, (UBYTE *)PLAYER.ID);
						memset(PLAYER.ID, 0, sizeof(PLAYER.ID));
						term_exit();
						exit(0);
					}
					else if(WEAPON(ONLINE)->Class < 30) {
						sprintf(outbuf, "He laughs at your futility and throws your %s out the window...", WEAPON(ONLINE)->Name);
						OUT(outbuf); NL;
					}
					else {
						sprintf(outbuf, "He picks up your %s and triumphantly waves it around to", WEAPON(ONLINE)->Name);
						OUT(outbuf); NL;
						OUT("the cheering crowd.  He struts towards the mantelpiece to hang his new trophy."); NL; NL;
						strcpy(ENEMY.Status, PLAYER.ID);
						strcpy(ENEMY.Weapon, PLAYER.Weapon);
						sprintf(PLAYER.Weapon, "%s.%s", table->weapon[0]->Origin, table->weapon[0]->Item[0]->Name);
					}
					Delay(100);
					sprintf(outbuf, "%s\"%sDrinks are on the house!%s\"", fore(CYN), fore(WHITE), fore(CYN));
					OUT(outbuf); NL; NL;
					ONLINE->weapon_origin = 0; ONLINE->weapon_type = 0;
					PLAYER.WCmod = 0; ONLINE->ToWC = 0;
					PLAYER.Coward = TRUE;
				}
				else {
					memset(ENEMY.Status, 0, sizeof(ENEMY.Status));
					modf(EXP(ENEMY.ExpLevel - 1) / 3., &d);
					PLAYER.Experience += d;
					sprintf(outbuf, "You get %.8g experience.", d);
					OUT(outbuf); NL;
					PLAYER.Gold += ENEMY.Gold;
					sprintf(outbuf, "You get %s %s was carrying.", money(ENEMY.Gold,ANSI), RPC[1][0]->he);
					OUT(outbuf); NL;
					ExchangeWeapon(ONLINE, RPC[1][0]);
					sprintf(outbuf, "%sdefeated %s%s", fore(CYN), fore(WHITE), ENEMY.Handle);
					news(outbuf);
					if(PLAYER.Level < 60)
						skillplus();
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
				Delay(50);
				return;
				break;
			case 'T':
				type("msg/Today",TRUE);
				break;
			case 'Y':
				type("msg/Yesterday",TRUE);
				break;
		}
	}
	JOUST(PLAYER.Level);
	d *= 3. + PLAYER.Level / 10;
	if(ACCESS(acclvl)->RolePlay == 'Y' && PLAYER.Gold > d) {
		NL;
		tax = PLAYER.Gold - d;
		sprintf(outbuf, "%s%s, the tax collector, spots your bulging money purse and", fore(CYN), IRSREC->Handle);
		OUT(outbuf); NL;
		sprintf(outbuf, "says, \"%sAh, it is time to pay your taxes!%s\"  You check out the two burly", fore(WHITE), fore(CYN));
		OUT(outbuf); NL;
		sprintf(outbuf, "guards who stand ready to enforce %s's will.", SYSREC->Handle);
		OUT(outbuf); NL; NL;
		Delay(100);
		sprintf(outbuf, "The tax will cost you %s.", money(tax, ANSI));
		OUT(outbuf); NL; NL;
		Delay(100);
		PLAYER.Gold -= tax;
		ENEMY.Bank += tax;
		paws = !PLAYER.Expert;
	}
}
