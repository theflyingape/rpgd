/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * sysop.c
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

void Sysop(void)
{
	static struct RPGmenu RPGmenu = { "Sysop",RED,8,
			{	{ 'B',"Blessed/Cursed Users" },
				{ 'G',"Gang record" },
				{ 'N',"New record" },
				{ 'R',"Reroll" },
				{ 'S',"System record" },
				{ 'T',"Tax record" },
				{ 'U',"User record" },
				{ 'Y',"Your Scout" }
			}
	};

	char c;
	int f,i,j;

	from='@';
	while((c=option(&RPGmenu))!='Q') {
		switch(c) {
			case 'B':
				sprintf(outbuf,"%s%s ID   Player's Handle           Class    Lvl    Last On     Blessed/Cursed%s",back(BLU),fore(WHITE),back(BLK));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s%s--------------------------------------------------------------------------%s",back(BLU),fore(GRY),back(BLK));
				OUT(outbuf);NL;
				memset(&ENEMY,0,sizeof(struct user));
				while(RPGserver(SERVER_NEXTUSER,(UBYTE *)&ENEMY)) {
					LoadRPC(RPC[1][0]);
					fdate(dated,ENEMY.LastDate);
					sprintf(outbuf,"%s%-4.4s  %-22s  %-9.9s  %3u  %-11.11s      ",
						(strncmp(ENEMY.ID,PLAYER.ID,sizeof(ENEMY.ID)) ? fore(GRY) : fore(CYAN)),
						ENEMY.ID,ENEMY.Handle,
						CLASS(RPC[1][0])->Name,ENEMY.Level,
						&dated[4]);
					if(strlen(ENEMY.Blessed)) {
						OUT(outbuf);
						sprintf(outbuf,"%sBlessed",fore(YELLOW));
						OUT(outbuf);
						NORMAL;NL;
					}
					if(strlen(ENEMY.Cursed)) {
						OUT(outbuf);
						sprintf(outbuf,"%sCursed",fore(GREEN));
						OUT(outbuf);
						NORMAL;NL;
					}
				}
				break;
			case 'G':
				memset(&gm[1],0,sizeof(struct gang));
				i=1;
				while(RPGserver(SERVER_NEXTGANG,(UBYTE *)&gm[1])) {
					if(i%(rows-1)==0) {
						if((c=more())=='N')
							break;
						i=1;
					}
					sprintf(outbuf,"%s%s %.20s",bracket(i),fore(GRY),gm[1].Name);
					OUT(outbuf);NL;
					sprintf(line[i++],"%.20s",gm[1].Name);
				}
				NL;
				sprintf(prompt,"%sWhich gang do you want to edit: ",fore(CYN));
				OUT(prompt);
				if(ins(3))
					break;
				NL;
				j=atoi(inbuf);
				if(j<1 || j>=i)
					break;
				sprintf(gm[1].Name,"%.20s",line[j]);
				if(!RPGserver(SERVER_GETGANG,(UBYTE *)&gm[1]))
					break;
				FOREVER {
					cls();
					sprintf(outbuf,"     %sGang name:%s %.20s",fore(CYN),fore(WHITE),gm[1].Name);
					OUT(outbuf);NL;
					for(j=0; j<4; j++) {
						if(strlen(gm[0].MemberID[j])) {
							strcpy(GANG(1,j).ID,gm[1].MemberID[j]);
							if(!RPGserver(SERVER_GETUSER,(UBYTE *)&GANG(1,j)))
								memset(RPC[1][j],0,sizeof(struct RPC));
						}
						sprintf(outbuf,"%s %sMember #%d:%s %-4s %s",bracket(j+1),fore(CYN),j+1,fore(WHITE),gm[1].MemberID[j],RPC[1][j]->user.Handle);
						OUT(outbuf);NL;
					}
					sprintf(prompt,"%s<#>Field, <D>elete: ",fore(CYN));
					OUT(prompt);
					if(ins(2))
						break;
					if(toupper(*inbuf)=='D') {
						OUT("eleting... ");
						sprintf(outbuf,"[%d]",RPGserver(SERVER_DELGANG,(UBYTE *)&gm[1]));
						OUT(outbuf);NL;
						paused();
						break;
					}
					if((f=atoi(inbuf))<1)
						break;
					if(f<5) {
						sprintf(prompt,"  %sID: ",fore(CYN));
						OUT(prompt);
						if(ins(4))
							break;
						NL;
						strcpy(gm[1].MemberID[f-1],inbuf);
						RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[1]);
					}
				}
				break;
			case 'N':
				FOREVER {
					cls();
					OUT("New User Starting Record");NL;
					sprintf(outbuf,"<1> Bank:  %s",money(NEWREC->Bank,ANSI));
					OUT(outbuf);NL;
					sprintf(outbuf,"<2> Money: %s",money(NEWREC->Gold,ANSI));
					OUT(outbuf);NL;
					sprintf(outbuf,"<3> Weapon: %s",NEWREC->Weapon);
					OUT(outbuf);NL;
					sprintf(outbuf,"<4> Armor: %s",NEWREC->Armor);
					OUT(outbuf);NL;
					sprintf(prompt,"%sField: ",fore(CYN));
					OUT(prompt);
					if(ins(2))
						break;
					if((f=atoi(inbuf))<1)
						break;
					sprintf(prompt,"  %sValue: ",fore(CYN));
					OUT(prompt);
					if(ins(30))
						break;
					NL;
					switch(f) {
						case 1:
							gold(&d,inbuf);
							NEWREC->Bank=d;
							break;
						case 2:
							gold(&d,inbuf);
							NEWREC->Gold=d;
							break;
						case 3:
							strcpy(NEWREC->Weapon,inbuf);
							break;
						case 4:
							strcpy(NEWREC->Armor,inbuf);
							break;
					}
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)NEWREC);
				break;
			case 'R':
				beep();
				sprintf(outbuf,"%sReroll all players and restart the game (Y/N)? ",fore(YELLOW));
				OUT(outbuf);
				c=inkey('N','Y');
				NL;NL;
				if(c=='Y') {
					memset(&ENEMY,0,sizeof(struct user));
					for(i=0; RPGserver(SERVER_NEXTUSER,(UBYTE *)&ENEMY); i++) {
						sprintf(outbuf,"%c\10",spin[i%4]);
						OUT(outbuf);
						if(ENEMY.ID[0]!='_') {
							ReKeySequence(&ENEMY);
							reroll(&ENEMY);
							RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
						}
					}
					ReKeySequence(&PLAYER);
					reroll(&PLAYER);
					SYSREC->BirthDate=Julian+1;
					SYSREC->Gold=0.;
					RPGserver(SERVER_PUTUSER,(UBYTE *)SYSREC);
					IRSREC->Bank=10000.;
					RPGserver(SERVER_PUTUSER,(UBYTE *)IRSREC);
					acclvl=PLAYER.Access;
					strcpy(reason,"reroll");
					logoff=TRUE;
				}
				break;
			case 'S':
				FOREVER {
					cls();
					OUT("System Record");NL;
					sprintf(outbuf,"%s %sSite Name:%s %s",bracket(1),fore(CYN),fore(WHITE),SYSREC->Name);
					OUT(outbuf);NL;
					fdate(dated,SYSREC->BirthDate);
					sprintf(outbuf,"%s %sGame Play:%s %s",bracket(2),fore(CYN),fore(WHITE),dated);
					OUT(outbuf);NL;
					NL;
					fdate(dated,SYSREC->CreateDate);
					sprintf(outbuf,"%sCreated:%s %s",fore(CYN),fore(WHITE),dated);
					OUT(outbuf);
					sprintf(outbuf,"   %sImmortal Level:%s %d",fore(CYN),fore(WHITE),SYSREC->Level);
					OUT(outbuf);NL;
					NL;
					sprintf(prompt,"%sField: ",fore(CYN));
					OUT(prompt);
					if(ins(2))
						break;
					if((f=atoi(inbuf))<1)
						break;
					sprintf(prompt,"  %sValue: ",fore(CYN));
					OUT(prompt);
					if(ins(30))
						break;
					NL;
					switch(f) {
						case 1:
							strncpy(SYSREC->Name,inbuf,sizeof(SYSREC->Name));
							break;
						case 2:
							julian(&SYSREC->BirthDate,inbuf);
							break;
					}
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)SYSREC);
				break;
			case 'T':
				FOREVER {
					cls();
					OUT("Taxman Record");NL;
					sprintf(outbuf,"%s %sTaxman's Handle:%s %s",bracket(1),fore(CYN),fore(WHITE),IRSREC->Handle);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s %sMoney:%s %s",bracket(2),fore(CYN),fore(WHITE),money(IRSREC->Bank,ANSI));
					OUT(outbuf);NL;
					NL;
					sprintf(prompt,"%sField: ",fore(CYN));
					OUT(prompt);
					if(ins(2))
						break;
					if((f=atoi(inbuf))<1)
						break;
					sprintf(prompt,"  %sValue: ",fore(CYN));
					OUT(prompt);
					if(ins(30))
						break;
					NL;
					switch(f) {
						case 1:
							strncpy(IRSREC->Handle,inbuf,sizeof(IRSREC->Handle));
							break;
						case 2:
							gold(&d,inbuf);
							IRSREC->Bank=d;
							break;
					}
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)IRSREC);
				break;
			case 'U':
				sprintf(prompt,"%sUser (?=list): ",fore(CYN));
				if(!GetRPC(RPC[1][0]))
					break;
				FOREVER {
					cls();
					sprintf(outbuf,"%sPlayer's Handle:%s %s",fore(CYN),fore(WHITE),ENEMY.Handle);
					OUT(outbuf);NL;
					sprintf(outbuf,"%sYour REAL Name.:%s %s",fore(CYN),fore(WHITE),ENEMY.Name);
					OUT(outbuf);NL;
					sprintf(outbuf,"%sE-mail Address.:%s %s",fore(CYN),fore(WHITE),ENEMY.Email);
					OUT(outbuf);NL;
					fdate(inbuf,ENEMY.CreateDate);
					sprintf(outbuf,"%s     Create Date.:%s %s",fore(CYN),fore(WHITE),inbuf);
					OUT(outbuf);NL;
					fdate(dated,ENEMY.LastDate);
					sprintf(inbuf,"%06lu",ENEMY.LastTime*100L);
					ftime(timed,inbuf);
					sprintf(outbuf,"%sLast Date/Time on:%s %s %s (%s)",fore(CYN),fore(WHITE),dated,timed,ENEMY.RemoteClient);
					OUT(outbuf);NL;
					fdate(inbuf,ENEMY.ExpireDate);
					sprintf(outbuf,"%s %sExpire Date.:%s %s",bracket(1),fore(CYN),fore(WHITE),inbuf);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s %sAccess Level:%s %u - %s",bracket(2),fore(CYN),fore(WHITE),ENEMY.Access,ACCESS(ENEMY.Access)->Name);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s %sPassword....:%s %s",bracket(3),fore(CYN),fore(WHITE),ENEMY.Password);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s %sCalls Today.:%s %hu",bracket(4),fore(CYN),fore(WHITE),ENEMY.CallsToday);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s %sImmortalized:%s %u",bracket(5),fore(CYN),fore(WHITE),ENEMY.Immortal);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s %sGang member.:%s %s",bracket(6),fore(CYN),fore(WHITE),ENEMY.Gang);
					OUT(outbuf);NL;
					sprintf(prompt,"%s<#>Field, <D>elete: ",fore(CYN));
					OUT(prompt);
					if(ins(2))
						break;
					if(toupper(*inbuf)=='D') {
						OUT("eleting... ");
						sprintf(outbuf,"[%d]",RPGserver(SERVER_DELUSER,(UBYTE *)&ENEMY));
						OUT(outbuf);NL;
						paused();
						break;
					}
					if((f=atoi(inbuf))<1)
						break;
					sprintf(prompt,"  %sValue: ",fore(CYN));
					OUT(prompt);
					if(ins(30))
						break;
					NL;
					switch(f) {
						case 1:
							julian(&ENEMY.ExpireDate,inbuf);
							break;
						case 2:
							ENEMY.Access=atoi(inbuf);
							break;
						case 3:
							proper(inbuf);
							strupr(inbuf);
							strncpy(ENEMY.Password,inbuf,sizeof(ENEMY.Password));
							break;
						case 4:
							ENEMY.CallsToday=atoi(inbuf);
							break;
						case 5:
							ENEMY.Immortal=atoi(inbuf);
							break;
						case 6:
							proper(inbuf);
							strncpy(ENEMY.Gang, inbuf, sizeof(ENEMY.Gang));
							break;
					}
					RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
					if(strncmp(PLAYER.ID,ENEMY.ID,sizeof(PLAYER.ID))==0)
						memcpy(&PLAYER,&ENEMY,sizeof(struct user));
				}
				break;
			case 'Y':
				charstats(RPC[1][0]);
				break;
		}
	}
	from='Q';
}
