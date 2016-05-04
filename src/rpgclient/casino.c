/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * casino.c
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

double Bet(void)
{
	sprintf(prompt, "%sBet [MAX=%s%s]? ", fore(CYN), money(PLAYER.Gold, ANSI), fore(CYN));
	OUT(prompt);
	if(ins(16))
		return(0.);
	NL; NL;
	gold(&d, inbuf);
	if(!strcasecmp(inbuf, "max") || *inbuf == '=') {
		sound("max", 64, 0);
		d = PLAYER.Gold;
	}
	if(d < 1. || d > PLAYER.Gold || ACCESS(acclvl)->RolePlay != 'Y')
		d = 0.;
	PLAYER.Gold -= d;
	if(d)
		paws = !PLAYER.Expert;
	return(d);
}

int ShowHand(struct card *hand[7],int cards)
{
	int ace,i,value;

	OUT(" hand: ");
	ace=0;
	value=0;
	for(i=0; i<cards; i++) {
		sprintf(outbuf,"%s[%s%s%s] ",fore(RED),fore(WHITE),hand[i]->Name,fore(RED));
		OUT(outbuf);
		value+=(hand[i]->Value<11) ? hand[i]->Value : (hand[i]->Value!=14) ? 10 : 1;
		if(hand[i]->Value==14)
			ace++;
	}
	for(i=0; i<ace && value+10<22; i++)
		value+=10;
	sprintf(outbuf,"%s= %d",fore(GRY),value);
	OUT(outbuf);
	NL;
	Delay(50);
	return(value);
}

void Shuffle(void)
{
	struct card *swap;
	int i,j;

	OUT("Shuffling new deck...");
	for(i=0; i<4; i++)
		for(j=0; j<13; j++)
			deck[13*i+j]=&card[j];
	for(i=0; i<52; i++) {
		swap=deck[i];
		j=dice(52)-1;
		deck[i]=deck[j];
		deck[j]=swap;
	}
	Delay(10);
	OUT("Ok.");NL;NL;
}

void Casino(void)
{
	static struct RPGmenu RPGmenu = { "Casino",BLU,8,
							{	{ 'B',"Blackjack" },
								{ 'C',"Craps" },
								{ 'G',"Greyhound race" },
								{ 'H',"High card" },
								{ 'I',"Instant Cash machine" },
								{ 'K',"Keno" },
								{ 'R',"Roulette" },
								{ 'S',"One-armed Bandit" }
							}
	};
	static char *dog[9] = {
		"Armored Tank","Knight Rider","Wham's Wings","Go For Grunt",
		"Let It Ride","Stinky","Lucky Lois","Rhody Runner","Beetle Bomb"
	};
	struct card *dealer[7],*player[7];
	double bet,loan,dd;
	int balls[81],post[10],d1,d2,i,pile,point,value;
	char c;

	sound2("casino",0);
	while((c=option(&RPGmenu))!='Q') {
		bet=0.;
		loan=0.;
		switch(c) {
			case 'B':
				Shuffle();
				if((bet=Bet())==0.)
					break;
				pile=0;
				d1=0;
				d2=0;
				player[d1++]=deck[pile++];
				dealer[d2++]=deck[pile++];
				player[d1++]=deck[pile++];
				dealer[d2++]=deck[pile++];
				sprintf(outbuf,"%sDealer's hand: %s[%sDOWN%s] [%s%s%s]",fore(GRN),fore(RED),fore(WHITE),fore(RED),fore(WHITE),dealer[1]->Name,fore(RED));
				OUT(outbuf);NL;
				OUT(fore(GRN));
				OUT("Player's");
				point=ShowHand(player,d1);
				if(point==21) {
					sound("cheer",64,0);
					sprintf(outbuf,"Blackjack!  You win %s!",money(2.*bet,ANSI));
					OUT(outbuf);NL;NL;
					Delay(50);
					PLAYER.Gold+=3.*bet;
					OUT(fore(GRN));
					OUT("Dealer's");
					value=ShowHand(dealer,d2);
					if(value==21) {
						sound("boo",64,0);
						OUT("Dealer has Blackjack!  Your bet was a loser!");NL;
						PLAYER.Gold-=3.*bet;
						break;
					}
					PLAYER.History.Gamble++;
					break;
				}
				loan=bet;
				FOREVER {
					NL;
					if(d1==2 && PLAYER.Gold>=bet)
						OUT("<D>ouble down or ");
					OUT("<H>it or <S>tay? ");
					if(!(c=inkey('\0','S')))
						break;
					NL;
					if(d1==2 && PLAYER.Gold>=bet && c=='D') {
						PLAYER.Gold-=bet;
						bet*=2.;
						c='H';
					}
					if(c=='H') {
						player[d1++]=deck[pile++];
						NL;
						OUT(fore(GRN));
						OUT("Player's");
						point=ShowHand(player,d1);
						if(point>21) {
							sound("boo",64,0);
							OUT("You bust!");NL;
							break;
						}
						if(loan<bet)
							break;
						if(d1==5) {
							sound("cheer",64,0);
							sprintf(outbuf,"Five card charley!  You win %s!",money(2.*bet,ANSI));
							OUT(outbuf);NL;
							PLAYER.Gold+=3.*bet;
							PLAYER.History.Gamble++;
							break;
						}
						if(point==21)
							break;
					}
					if(c=='S')
						break;
				}
				NL;
				OUT(fore(GRN));
				OUT("Dealer's");
				value=ShowHand(dealer,d2);
				if(d1<5 && value==21) {
					sound("boo",64,0);
					OUT("Dealer has Blackjack!  Your bet was a loser!");NL;
					Delay(50);
					break;
				}
				do {
					if(value<17) {
						dealer[d2++]=deck[pile++];
						OUT(fore(GRN));
						OUT("Dealer's");
						value=ShowHand(dealer,d2);
					}
					Delay(30);
				} while(value<17 && value<22);
				NL;
				if(point>21 || d1==5)
					break;
				if(value>21) {
					sound("cheer",64,0);
					sprintf(outbuf,"Dealer bust!  You win %s!",money(bet,ANSI));
					OUT(outbuf);NL;
					PLAYER.Gold+=2.*bet;
					PLAYER.History.Gamble++;
					break;
				}
				if(value<point) {
					sound("cheer",64,0);
					sprintf(outbuf,"You win %s!",money(bet,ANSI));
					OUT(outbuf);NL;
					PLAYER.Gold+=2.*bet;
					PLAYER.History.Gamble++;
					break;
				}
				if(value==point) {
					OUT("It's a push.");NL;
					PLAYER.Gold+=bet;
					break;
				}
				if(value>point) {
					sound("boo",64,0);
					OUT("Dealer wins.");NL;
					break;
				}
				break;
			case 'C':
				if((bet=Bet()) == 0.)
					break;
				OUT("Rolling dice for your point");
				d1=dice(6);
				d2=dice(6);
				point = d1 + d2;
				for(i = 0; i < 5; i++) {
					Delay(5);
					OUT(".");
				}
				Delay(25);
				sprintf(outbuf, "%s[%s%d%s] [%s%d%s] %s= %d", fore(BLU), fore(CYAN), d1, fore(BLU), fore(CYAN), d2, fore(BLU), fore(WHITE), d1 + d2);
				OUT(outbuf); NL; NL;
				Delay(50);
				if(point == 7 || point == 11) {
					sound("cheer", 64, 0);
					sprintf(outbuf, "A natural!  You win %s!", money(2. *bet, ANSI));
					OUT(outbuf); NL;
					PLAYER.Gold += 3. * bet;
					PLAYER.History.Gamble++;
					break;
				}
				if(point == 2 || point == 3 || point == 12) {
					sound("boo", 64, 0);
					OUT("Crapped out!  You lose."); NL;
					break;
				}
				cls();
				sprintf(outbuf, "%sYour point to make is: %s%d", fore(CYN), fore(WHITE), point);
				OUT(outbuf); NL;
				NORMAL; NL;
				OUT("Press RETURN to roll dice and try to make your point"); NL;
				OUT("or bet on another number for additional payoffs:"); NL; NL;
				OUT("  [2] or [12] pays 35:1"); NL;
				OUT("  [3] or [11] pays 17:1"); NL;
				OUT("  [4] or [10] pays 11:1"); NL;
				OUT("  [5] or  [9] pays  8:1"); NL;
				OUT("  [6] or  [8] pays  6:1"); NL;
				OUT("  [7] to break pays 5:1"); NL;
				do {
					plot(13,1);
					sprintf(prompt, "%sRoll dice: ", fore(CYN));
					OUT(prompt); CLL;
					if(ins(2))
						break;
					NL; CLL;
					i = atoi(inbuf);
					if(i > 1 && i < 13)
						loan = Bet();
					else
						i = 0;
					CLL;
					d1 = dice(6);
					d2 = dice(6);
					sprintf(outbuf, "%s[%s%d%s] [%s%d%s] %s= %d   ", fore(BLU), fore(CYAN), d1, fore(BLU), fore(CYAN), d2, fore(BLU), fore(WHITE), d1 + d2);
					OUT(outbuf);
					NL; CLL; Delay(5);
					if(d1 + d2 == 7) {
						sound("boo", 64, 0);
						OUT("Crapped out!  You lose."); NL;
					}
					if(d1 + d2 == point) {
						sound("cheer", 64, 0);
						sprintf(outbuf, "You've made your point!  You win %s!", money(bet, ANSI));
						OUT(outbuf); NL;
						PLAYER.Gold += 2. * bet;
						PLAYER.History.Gamble++;
					}
					NL; CLL; Delay(5);
					if(i != 0 && d1 + d2 != i) {
						sound("boo", 64, 0);
						OUT("You lose on the side bet.");
					}
					if(d1 + d2 == i) {
						i = (i == 2 || i == 12) ? 35 : (i == 3 || i == 11) ? 17 : (i == 4 || i == 10) ? 11 : (i == 5 || i == 9) ? 8 : (i == 6 || i == 8) ? 6 : 5;
						sound("cheer", 64, 0);
						sprintf(outbuf, "You've made your side bet!  You win %s!", money(loan * i, ANSI));
						OUT(outbuf);
						PLAYER.History.Gamble++;
						PLAYER.Gold += loan * (i + 1);
					}
					NL; CLL; Delay(5);
				} while(d1 + d2 != point && d1 + d2 != 7);
				break;
			case 'G':
				if((bet = Bet()) == 0.)
					break;
				cls();
				OUT(fore(BLU));
				OUT("[==============*=========+=========+=========+=========+=========F");
				NL;
				for(i = 0; i < 9; i++) {
					post[i] = 0;
					sprintf(outbuf, "%s[%s%s %-12.12s :%s%d%49.49s", fore(BLU), back(BLU), fore(WHITE), dog[i], back(BLK), i + 1, "|");
					OUT(outbuf);
					NL;
				}
				OUT(fore(BLU));
				OUT("[==============*=========+=========+=========+=========+=========F");
				NL;
				OUT(fore(CYN));
				OUT("Which dog (1-9)? ");
				if(ins(1))
					break;
				point = atoi(inbuf) - 1;
				if(point < 0 || point > 8) {
					PLAYER.Gold += bet;
					break;
				}
				OUT(fore(CYN));
				OUT("   <W>in, <P>lace, or <S>how? ");
				if(!(c = inkey('\0','\0')))
					break;
				if(c != 'W' && c != 'P' && c != 'S') {
					PLAYER.Gold += bet;
					break;
				}
				OUT(fore(YELLOW));
				OUT("   They're off!");
				OUT(fore(GRY));
				loan = 0.;
				i = 0;
				do {
					d1 = dice(9) - 1;
					if(post[d1] < 50) {
						Delay(1);
						plot(2 + d1, 17 + post[d1]);
						sprintf(outbuf, "%s%c%s%d", fore(BLACK), post[d1] % 2 ? '`' : ',', fore(d1 % 9 + 1), d1 + 1);
						OUT(outbuf);
						post[d1]++;
						if(post[d1] == 50) {
							i++;
							OUT(fore(i == 1 ? CYAN : i == 2 ? YELLOW : MAGENTA));
							OUT(i == 1 ? " *1st*" : i == 2 ? " =2nd=" : " -3rd-");
							OUT(fore(GRY));
							if(d1 == point)
								if(i == 1 || (i == 2 && c != 'W') || (i == 3 && c == 'S'))
									loan = bet*((c == 'W') ? 8. : (c == 'P') ? 4. : 2.);
							Delay(50);
						}
					}
				} while(i < 3);
				plot(14,1);
				if(loan) {
					sound("cheer", 64, 0);
					sprintf(outbuf, "You win %s!", money(loan, ANSI));
					OUT(outbuf);
					PLAYER.Gold += loan + bet;
					PLAYER.History.Gamble++;
				}
				else {
					sound("boo", 64, 0);
					OUT("You lose.");
				}
				CLL; NL;
				break;
			case 'H':
				Shuffle();
				if((bet=Bet())==0.)
					break;
				OUT(fore(CYN));
				OUT("Pick any card (1-52)? ");
				if(ins(2))
					break;
				d1=atoi(inbuf);
				if(d1<1 || d1>52) {
					PLAYER.Gold+=bet;
					NL;
					break;
				}
				d1--;
				sprintf(outbuf," - %s[%s%s%s]",fore(RED),fore(WHITE),deck[d1]->Name,fore(RED));
				OUT(outbuf);
				NL;
				NORMAL;
				Delay(50);
				do {
					d2=dice(52)-1;
				} while(d2==d1);
				sprintf(outbuf,"Dealer picks card #%d",d2+1);
				OUT(outbuf);
				Delay(50);
				sprintf(outbuf," - %s[%s%s%s]",fore(RED),fore(WHITE),deck[d2]->Name,fore(RED));
				OUT(outbuf);
				NL;
				NORMAL;
				Delay(50);
				NL;
				if(deck[d1]->Value>deck[d2]->Value) {
					sound("cheer",64,0);
					sprintf(outbuf,"You win %s!",money(bet,ANSI));
					OUT(outbuf);NL;
					PLAYER.Gold+=2.*bet;
					PLAYER.History.Gamble++;
				}
				if(deck[d1]->Value==deck[d2]->Value) {
					OUT("We tie.");NL;
					PLAYER.Gold+=bet;
				}
				if(deck[d1]->Value<deck[d2]->Value) {
					sound("boo",64,0);
					OUT("You lose.");NL;
				}
				break;
			case 'I':
				bump("way to the ATM");
				sprintf(outbuf,"%sEnter PIN:%s ",fore(CYN),fore(WHITE));
				OUT(outbuf);
				for(i=0; i<6; i++) {
					Delay(dice(37)+3);
					OUT("#");
				}
				NL;
				while(!cd()) {
					cls();
					sprintf(outbuf,"%sAutomatic Teller Machine",fore(CYAN));
					OUT(outbuf);NL;NL;
					sprintf(outbuf,"%sMoney in hand: %s",fore(GRN),money(PLAYER.Gold,ANSI));
					OUT(outbuf);NL;
					sprintf(outbuf,"%sMoney in bank: %s",fore(GRN),money(PLAYER.Bank,ANSI));
					OUT(outbuf);NL;
					sprintf(outbuf,"%sMoney on loan: %s",fore(GRN),money(PLAYER.Loan,ANSI));
					OUT(outbuf);NL;NL;
					sprintf(prompt,"%s[%sATM%s]%s Option (Q=Quit): ",fore(GRN),fore(YELLOW),fore(GRN),fore(CYN));
					OUT(prompt);
					if((c=inkey('\0','Q'))=='Q')
						break;
					NL;NL;
					if(ACCESS(acclvl)->RolePlay=='Y')
						switch(c) {
							case 'D':
								sprintf(outbuf,"%sDeposit%s [MAX=%s]? ",fore(CYN),fore(GRY),money(PLAYER.Gold,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									sound("max",64,0);
									d=PLAYER.Gold;
								}
								else
								if(d==PLAYER.Gold && strlen(inbuf)>3) {
									OUT("You may also type MAX in place of the entire amount.");NL;NL;
									Delay(100);
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
								}
								break;
							case 'W':
								sprintf(outbuf,"%sWithdrawal%s [MAX=%s]? ",fore(CYN),fore(GRY),money(PLAYER.Bank,ANSI));
								OUT(outbuf);
								if(ins(16))
									break;
								NL;NL;
								gold(&d,inbuf);
								if(!strcasecmp(inbuf,"max") || *inbuf=='=') {
									sound("max",64,0);
									d=PLAYER.Bank;
								}
								if(d>0. && d<=PLAYER.Bank) {
									PLAYER.Bank-=d;
									PLAYER.Gold+=d;
								}
								break;
							default:
								beep();
								OUT("<D>eposit or <W>ithdrawal.");NL;
								paws=TRUE;
								break;
						}
				}
				break;
			case 'K':
				sprintf(prompt,"%sHow many numbers (1-10)? ",fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				point=atoi(inbuf);
				if(point<1 || point>10)
					break;
				sprintf(outbuf,"%sKENO PAYOUT for a %d spot game:",fore(GRN),point);
				OUT(outbuf);NL;NL;
				sprintf(outbuf,"%sMATCH     PRIZE%s",fore(GREEN),fore(CYN));
				OUT(outbuf);NL;
				switch(point) {
					case 1:
						OUT("   1         $1");NL;
						strcpy(line[0],"4");
						break;
					case 2:
						OUT("   2         $9");NL;
						strcpy(line[0],"16.6");
						break;
					case 3:
						OUT("   3        $20");NL;
						OUT("   2          2");NL;
						strcpy(line[0],"6.55");
						break;
					case 4:
						OUT("   4        $50");NL;
						OUT("   3          5");NL;
						OUT("   2          1");NL;
						strcpy(line[0],"3.86");
						break;
					case 5:
						OUT("   5       $400");NL;
						OUT("   4         10");NL;
						OUT("   3          2");NL;
						strcpy(line[0],"10.33");
						break;
					case 6:
						OUT("   6      $1000");NL;
						OUT("   5         50");NL;
						OUT("   4          5");NL;
						OUT("   3          1");NL;
						strcpy(line[0],"6.19");
						break;
					case 7:
						OUT("   7      $4000");NL;
						OUT("   6         75");NL;
						OUT("   5         15");NL;
						OUT("   4          2");NL;
						OUT("   3          1");NL;
						strcpy(line[0],"4.22");
						break;
					case 8:
						OUT("   8     $10000");NL;
						OUT("   7        500");NL;
						OUT("   6         40");NL;
						OUT("   5         10");NL;
						OUT("   4          2");NL;
						strcpy(line[0],"9.79");
						break;
					case 9:
						OUT("   9     $25000");NL;
						OUT("   8       2500");NL;
						OUT("   7        100");NL;
						OUT("   6         20");NL;
						OUT("   5          5");NL;
						OUT("   4          1");NL;
						strcpy(line[0],"6.55");
						break;
					case 10:
						OUT("  10    $100000");NL;
						OUT("   9       4000");NL;
						OUT("   8        400");NL;
						OUT("   7         25");NL;
						OUT("   6         10");NL;
						OUT("   5          2");NL;
						OUT(" none         5");NL;
						strcpy(line[0],"9.04");
						break;
				}
				NL;
				sprintf(outbuf,"%sodds of winning a prize in this game are 1:%s.",fore(GRY),line[0]);
				OUT(outbuf);NL;NL;
				if((bet=Bet())==0.)
					break;
				i=0;
				do {
					FOREVER {
						d1=dice(80);
						for(d2=0; d2<i && post[d2]!=d1; d2++);
						if(d2==i)
							break;
					}
					sprintf(prompt,"%sPick #%d [%02d]: ",fore(CYN),i+1,d1);
					OUT(prompt);
					if(ins(2))
						break;
					d2=atoi(inbuf);
					if(!strlen(inbuf)) {
						d2=d1;
						sprintf(outbuf,"%s%02d",fore(WHITE),d2);
						OUT(outbuf);
					}
					NL;
					if(d2>=1 && d2<=80) {
						for(d1=0; d1<i && post[d1]!=d2; d1++);
						if(d1==i)
							post[i++]=d2;
						else
							beep();
					}
					else
						beep();
				} while(i<point);
				NL;
				sprintf(outbuf,"%sHere comes those lucky numbers!",fore(YELLOW));
				OUT(outbuf);NL;NL;
				Delay(50);
				for(i=1; i<=80; i++)
					balls[i]=i;
				value=0;
				for(i=0; i<20; i++) {
					while(!balls[(d1=dice(80))]);
					balls[d1]=0;
					for(pile=0; pile<point && post[pile]!=d1; pile++);
					if(pile==point)
						sprintf(outbuf," %s[%s %s%02d%s %s]  ",fore(BLU),back(GRY),fore(RED),d1,fore(BLU),back(BLK));
					else {
						sprintf(outbuf,"%s*%s[%s %s%02d%s %s]%s* ",fore(YELLOW),fore(BLU),back(GRY),fore(RED),d1,fore(BLU),back(BLK),fore(YELLOW));
						beep();
						value++;
					}
					OUT(outbuf);
					Delay(25);
					if((i+1)%5==0) {
						NL;NL;
					}
				}
				loan=0.;
				switch(point) {
					case 1:
						if(value==1)
							loan=bet*2.;
						break;
					case 2:
						if(value==2)
							loan=bet*9.;
						break;
					case 3:
						if(value==3)
							loan=bet*20.;
						if(value==2)
							loan=bet*2.;
						break;
					case 4:
						if(value==4)
							loan=bet*50.;
						if(value==3)
							loan=bet*5.;
						if(value==2)
							loan=bet*1.;
						break;
					case 5:
						if(value==5)
							loan=bet*400.;
						if(value==4)
							loan=bet*10.;
						if(value==3)
							loan=bet*2.;
						break;
					case 6:
						if(value==6)
							loan=bet*1000.;
						if(value==5)
							loan=bet*50.;
						if(value==4)
							loan=bet*5.;
						if(value==3)
							loan=bet*1.;
						break;
					case 7:
						if(value==7)
							loan=bet*4000.;
						if(value==6)
							loan=bet*75.;
						if(value==5)
							loan=bet*15.;
						if(value==4)
							loan=bet*2.;
						if(value==3)
							loan=bet*1.;
						break;
					case 8:
						if(value==8)
							loan=bet*10000.;
						if(value==7)
							loan=bet*500.;
						if(value==6)
							loan=bet*40.;
						if(value==5)
							loan=bet*10.;
						if(value==4)
							loan=bet*2.;
						break;
					case 9:
						if(value==9)
							loan=bet*25000.;
						if(value==8)
							loan=bet*2500.;
						if(value==7)
							loan=bet*100.;
						if(value==6)
							loan=bet*20.;
						if(value==5)
							loan=bet*5.;
						if(value==4)
							loan=bet*1.;
						break;
					case 10:
						if(value==10)
							loan=bet*100000.;
						if(value==9)
							loan=bet*4000.;
						if(value==8)
							loan=bet*400.;
						if(value==7)
							loan=bet*25.;
						if(value==6)
							loan=bet*10.;
						if(value==5)
							loan=bet*2.;
						if(value==0)
							loan=bet*5.;
						break;
				}
				if(loan) {
					sound("cheer",64,0);
					sprintf(outbuf,"%sYou win %s!",fore(WHITE),money(loan,ANSI));
					OUT(outbuf);
					PLAYER.Gold+=loan;
					PLAYER.History.Gamble++;
				}
				else {
					sound("boo",64,0);
					OUT("You lose.");
				}
				NL;
				break;
			case 'R':
				if((bet=Bet())==0.)
					break;
				sprintf(prompt,"%sWhich number (1-36), <O>dd, or <E>ven? ",fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				loan=0.;
				point=atoi(inbuf);
				c=toupper(inbuf[0]);
				if((point<1 || point>36) && c!='O' && c!='E') {
					PLAYER.Gold+=bet;
					break;
				}
				OUT("The wheel spins");
				for(i=0; i<4; i++) {
					OUT(fore(GRY));
					for(d1=0; d1<5; d1++) {
						value=dice(36);
						OUT(".");
						Delay(5);
					}
					sprintf(outbuf,"%s[%s%d%s]",fore(i<3 ? BLU : RED),fore(YELLOW),value,fore(i<3 ? BLU : RED));
					OUT(outbuf);
					Delay(5);
				}
				Delay(20);
				NORMAL;NL;NL;
				if(value==point)
					loan=35.*bet;
				if(c=='O' && value%2==1)
					loan=bet;
				if(c=='E' && value%2==0)
					loan=bet;
				if(loan) {
					sound("cheer",64,0);
					sprintf(outbuf,"You win %s!",money(loan,ANSI));
					OUT(outbuf);
					PLAYER.Gold+=loan+bet;
					PLAYER.History.Gamble++;
				}
				else {
					sound("boo",64,0);
					OUT("You lose.");
				}
				NL;
				break;
			case 'S':
				OUT("Any 2 Cherry.....$2        3 Orange........$50"); NL;
				OUT("3 Cherry.........$5        3 Bell.........$100"); NL;
				OUT("3 Plum..........$10        3 Gold.........$400"); NL;
				OUT("3 Lime..........$20        3 Wild.........$500"); NL;
				OUT("3 *WILD* pays same coin line bet in Monster Jackpot: ");
				loan = SYSREC->Gold;
				modf(loan / 1e+13, &bet);
				sprintf(outbuf, "%s%.8g%sp", fore(WHITE), bet, fore(MAGENTA));
				OUT(outbuf);
				NORMAL; OUT(",");
				loan -= bet * 1e+13;
				modf(loan / 1e+09, &bet);
				sprintf(outbuf, "%s%.4g%sg", fore(WHITE), bet, fore(YELLOW));
				OUT(outbuf);
				NORMAL; OUT(",");
				loan -= bet * 1e+09;
				modf(loan / 1e+05, &bet);
				sprintf(outbuf, "%s%.4g%ss", fore(WHITE), bet, fore(CYAN));
				OUT(outbuf); NORMAL;
				OUT(",");
				loan -= bet * 1e+05;
				bet = loan;
				sprintf(outbuf, "%s%.5g%sc", fore(WHITE), bet, fore(BRED));
				OUT(outbuf);
				NORMAL; NL;

				if((bet = Bet()) == 0.)
					break;
				OUT("You insert the money, pull the arm, and the wheels spin...");
				Delay(25); NL;
				for(i = 0; i < 3; i++) {
					bandit[i] = SYSREC->KeyHints[0][i];
					d2 = 16 + dice(16);
					for(d1 = 0; d1 < d2; d1++) {
						bandit[i]++;
						bandit[i] %= 16;
						sprintf(outbuf, "%c\10", spin[d1 % 4]);
						OUT(outbuf);
						Delay(1);
					}
					sprintf(outbuf, "%s[%s%s%s] ", fore(BLU), fore(slotcol[wheel[i][bandit[i]]]), slotval[wheel[i][bandit[i]]], fore(BLU));
					OUT(outbuf);
					NORMAL;
					Delay(10);
					SYSREC->KeyHints[0][i] = bandit[i];
				}
				NL; Delay(10);

				if(wheel[0][bandit[0]] == WILD
				&& wheel[1][bandit[1]] == WILD
				&& wheel[2][bandit[2]] == WILD) {
					for(d1 = 0; d1 < 8; d1++) {
						for(d2 = 0; d2 < 8; d2++) {
							beep();
							if((d1 + d2) % 2)
								OUT("\33[5m");
							else
								NORMAL;
							OUT(fore((d1 + d2) % 8 + 8));
							OUT("YOU WIN! ");
							Delay(5);
						}
						NORMAL; NL;
						Delay(10);
					}

					d = SYSREC->Gold;
					dd = bet;
					bet *= 500.;

					modf(dd / 1e+13, &loan);
					if(loan > 0.) {
						modf(d / 1e+13, &loan);
						bet += loan * 1e+13;
						dd -= loan * 1e+13;
					}
					modf(d / 1e+13, &loan);
					d -= loan * 1e+13;

					modf(dd / 1e+09, &loan);
					if(loan > 0.) {
						modf(d / 1e+09, &loan);
						bet += loan * 1e+09;
						dd -= loan * 1e+09;
					}
					modf(d / 1e+09, &loan);
					d -= loan * 1e+09;

					modf(dd / 1e+05, &loan);
					if(loan > 0.) {
						modf(d / 1e+05, &loan);
						bet += loan * 1e+05;
						dd -= loan * 1e+05;
					}
					modf(d / 1e+05, &loan);
					d -= loan * 1e+05;

					bet += d;
				}
				else
				if((wheel[0][bandit[0]] == BOOM || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == BOOM || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == BOOM || wheel[2][bandit[2]] == WILD)) {
					for(d1 = 0; d1 < 8; d1++) {
						for(d2 = 0; d2 < 8; d2++) {
							beep();
							OUT(fore((d1 + d2) % 8 + 8));
							OUT("YOU DIE! ");
							Delay(5);
						}
						NORMAL; NL;
						Delay(10);
					}
					strcpy(reason, "tilted by the slot machine");
					logoff = TRUE;
				}
				else
				if((wheel[0][bandit[0]] == GOLD || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == GOLD || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == GOLD || wheel[2][bandit[2]] == WILD)) {
					for(d1 = 0; d1 < 8; d1++) {
						for(d2 = 0; d2 < 8; d2++) {
							beep();
							OUT(fore((d1 + d2) % 8 + 8));
							OUT("YOU WIN! ");
							Delay(5);
						}
						NORMAL; NL;
						Delay(10);
					}
					bet *= 400.;
				}
				else
				if((wheel[0][bandit[0]] == BELL || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == BELL || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == BELL || wheel[2][bandit[2]] == WILD)) {
					for(i = 0; i < 8; i++) {
						beep();
						OUT(fore(8 + i));
						OUT("YOU WIN! ");
						Delay(5);
					}
					NORMAL; NL;
					Delay(10);
					bet *= 100.;
				}
				else
				if((wheel[0][bandit[0]] == ORANGE || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == ORANGE || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == ORANGE || wheel[2][bandit[2]] == WILD)) {
					beep();
					bet *= 50.;
				}
				else
				if((wheel[0][bandit[0]] == LIME || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == LIME || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == LIME || wheel[2][bandit[2]] == WILD)) {
					beep();
					bet *= 20.;
				}
				else
				if((wheel[0][bandit[0]] == PLUM || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == PLUM || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == PLUM || wheel[2][bandit[2]] == WILD))
					bet *= 10.;
				else
				if((wheel[0][bandit[0]] == CHERRY || wheel[0][bandit[0]] == WILD)
				&& (wheel[1][bandit[1]] == CHERRY || wheel[1][bandit[1]] == WILD)
				&& (wheel[2][bandit[2]] == CHERRY || wheel[2][bandit[2]] == WILD))
					bet *= 5.;
				else
				if((((wheel[0][bandit[0]] == CHERRY || wheel[0][bandit[0]] == WILD)
				&& ((wheel[1][bandit[1]] == CHERRY || wheel[1][bandit[1]] == WILD)
				 || (wheel[2][bandit[2]] == CHERRY || wheel[2][bandit[2]] == WILD)))
				||  ((wheel[1][bandit[1]] == CHERRY || wheel[1][bandit[1]] == WILD)
				 && (wheel[2][bandit[2]] == CHERRY || wheel[2][bandit[2]] == WILD))))
					bet *= 2.;
				else {
					sound("boo", 64, 0);
					OUT("You lose.");
					NORMAL; NL; Delay(5);
					SYSREC->Gold += bet;
					bet = 0.;
				}
				if(bet > 0.) {
					sound("cheer", 64, 0);
					sprintf(outbuf, "You win %s!", money(bet, ANSI));
					OUT(outbuf);
					NORMAL; NL; Delay(5);
					PLAYER.Gold += bet;
					PLAYER.History.Gamble++;
					SYSREC->Gold -= bet;
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)SYSREC);
				break;
		}
	}
}
