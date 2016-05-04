/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * newuser.c
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

void emulation(void)
{
	sprintf(outbuf, "%sWhich terminal emulation are you using?", fore(CYN));
	OUT(outbuf); NL; NL;
	sprintf(outbuf, " %s%s VT220 w/DEC graphics", bracket(1), fore(GRY));
	OUT(outbuf); NL;
	sprintf(outbuf, " %s%s ANSI w/IBM color graphics", bracket(2), fore(GRY));
	OUT(outbuf); NL;
	sprintf(outbuf, " %s%s Windows RPGclient w/sound", bracket(3), fore(GRY));
	OUT(outbuf); NL; NL;
	sprintf(outbuf, "%sSelect: ", fore(CYN));
	OUT(outbuf);
	FOREVER {
		if(ins(1))
			break;
		ANSI = atoi(inbuf);
		PLAYER.Emulation = ANSI;
		if(ANSI >= 1 && ANSI <= 3)
			break;
		eraser();
	}
	FOREVER {
		cls();
		for(PLAYER.Rows = 99; PLAYER.Rows > 21; PLAYER.Rows--) {
			OUT(bracket(PLAYER.Rows)); NL;
		}
		for(; PLAYER.Rows > 1; PLAYER.Rows--) {
			OUT(" .."); NL;
		}
		sprintf(outbuf, "%sEnter the number on the top of your screen (22-99): ", fore(CYN));
		OUT(outbuf);
		if(ins(2))
			break;
		PLAYER.Rows = atoi(inbuf);
		if(PLAYER.Rows >= 22 && PLAYER.Rows <= 99)
			break;
		beep();
	}
	rows = PLAYER.Rows;
}

int edituser(int p)
{
	int flag = TRUE, i;
	char *token;

	plot(p + 2, 23);
	CLL;
	switch(p) {
		case 1:
			if(ins(22))
				return(FALSE);
			proper(inbuf);
			if(strlen(inbuf) > 1) {
				strncpy(PLAYER.Handle, inbuf, sizeof(PLAYER.Handle));
				plot(p + 2, 23);
				OUT(PLAYER.Handle); CLL;
				strncpy(username.Handle, PLAYER.Handle, sizeof(username.Handle));
				if(!(flag = RPGserver(SERVER_GETUSERNAME, (UBYTE *)&username))) {
					strncpy(username.Handle, PLAYER.Handle, sizeof(username.Handle));
					for(token = strtok(username.Handle, " "); token; token = strtok(NULL," "))
						for(i = 0; i < NUMACCESS; i++)
							if(!strcasecmp(token, ACCESS(i)->Name))
								flag = TRUE;
					strncpy(username.Handle, PLAYER.Handle, sizeof(username.Handle));
					for(token = strtok(username.Handle, " "); token; token = strtok(NULL," "))
						for(i = 0; i < NUMARENA; i++)
							if(!strcasecmp(token, ARENA(i)->Name))
								flag = TRUE;
					strncpy(username.Handle, PLAYER.Handle, sizeof(username.Handle));
					for(token = strtok(username.Handle, " "); token; token = strtok(NULL," "))
						for(i = 0; i < NUMDUNGEON; i++)
							if(!strcasecmp(token, DUNGEON(i)->Name))
								flag = TRUE;
				}
				if(flag) {
					beep();
					sprintf(outbuf, "  %s<- already taken", fore(YELLOW));
					OUT(outbuf);
					Delay(50);
				}
			}
			break;
		case 2:
			strcpy(inbuf, PLAYER.Name);
			if(ins(30))
				return(FALSE);
			proper(inbuf);
			if(strlen(inbuf) > 4) {
				strncpy(PLAYER.Name, inbuf, sizeof(PLAYER.Name));
				flag = FALSE;
			}
			plot(p + 2, 23);
			OUT(PLAYER.Name); CLL;
			break;
		case 3:
			if(ins(10))
				return(FALSE);
			if(strlen(inbuf)) {
				julian(&PLAYER.BirthDate, inbuf);
				if(Julian - PLAYER.BirthDate > 3652L && Julian - PLAYER.BirthDate < 36525L) {	//  ages 10-100
					fdate(inbuf, PLAYER.BirthDate);
					plot(p + 2, 23);
					OUT(inbuf); CLL;
					flag = FALSE;
				}
			}
			break;
		case 4:
			if(ins(1))
				return(FALSE);
			strupr(inbuf);
			PLAYER.Sex = inbuf[0];
			if(PLAYER.Sex == 'M' || PLAYER.Sex == 'F') {
				plot(p + 2, 23);
				OUT(inbuf);
				flag = FALSE;
			}
			break;
	}
	if(cd())
		flag = FALSE;
	return(flag);
}

void NewUser(void)
{
	int p;

	title("newuser");
	memcpy(&PLAYER, NEWREC, sizeof(struct user));
	memset(PLAYER.ID, 0, sizeof(PLAYER.ID));
	PLAYER.CreateDate = Julian;
	PLAYER.ExpireDate = Julian;
	PLAYER.LastDate = Julian;
	PLAYER.Novice = 'Y';

	emulation();

	sound2("newuser", 0);
	cls();
	sprintf(outbuf, "     %s--=:) %sNew User Registration%s (:=--", fore(BRN), fore(YELLOW), fore(BRN));
	OUT(outbuf); NL; NL;
	sprintf(outbuf, "%s%s Player's Handle: ", bracket(1), fore(CYN));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s%s Your REAL Name.: %s%s", bracket(2), fore(CYN), fore(BLACK), PLAYER.Name);
	OUT(outbuf); NL;
	sprintf(outbuf, "%s%s Date of Birth..: ", bracket(3), fore(CYN));
	OUT(outbuf); NL;
	sprintf(outbuf, "%s%s Gender (M/F)...: ", bracket(4), fore(CYN));
	OUT(outbuf); NL;
	for(p = 1; p <= 4; p++)
		while(edituser(p))
			beep();
	NL; NL;
	sprintf(outbuf, "%sSelect field # to change or <RETURN> to save: ", fore(CYN));
	OUT(outbuf);
	do {
		plot(8, 47);CLL;
		ins(1);
		p = atoi(inbuf);
		if(p >= 1 && p <= 4)
			while(edituser(p))
				beep();
	} while(p);
	NL; NL;
	if(cd())
		return;

	PLAYER.Access = 1;

	for(p = 0; p < strlen(PLAYER.Handle) && strlen(PLAYER.ID) < 3; p++) {
		PLAYER.ID[strlen(PLAYER.ID)] = toupper(PLAYER.Handle[p]);
		while(++p < strlen(PLAYER.Handle) && PLAYER.Handle[p] != ' ');
	}
	if(strlen(PLAYER.ID) < 2) {
		sprintf(PLAYER.ID, "%.3s", PLAYER.Handle);
		strupr(PLAYER.ID);
	}
	strncpy(ENEMY.ID, PLAYER.ID, sizeof(ENEMY.ID));
	for(p = '2'; RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY) || !strcmp(ENEMY.ID, "NEW"); p++) {
		if(isdigit(PLAYER.ID[strlen(PLAYER.ID) - 1]))
			PLAYER.ID[strlen(PLAYER.ID) - 1] = '\0';
		sprintf(PLAYER.ID, "%.3s%c", PLAYER.ID, p);
		strncpy(ENEMY.ID, PLAYER.ID, sizeof(ENEMY.ID));
	}
	sprintf(outbuf, "%sYour User ID is:%s %s", fore(CYN), fore(WHITE), PLAYER.ID);
	NL; OUT(outbuf); NL;
	LoadRPC(ONLINE);
	RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
	numline = 0;
	sprintf(line[numline++], "[%s] %s signed on as a new user", PLAYER.ID, PLAYER.Handle);
	if(!strlen(SYSID)) {
		strcpy(SYSID, PLAYER.ID);
		strcpy(SYSREC->Handle, PLAYER.Handle);
		RPGserver(SERVER_PUTUSER, (UBYTE *)SYSREC);
		PLAYER.Access = NUMACCESS - 1;
	}
	note(SYSID);
	paused();
	cls();
	type("msg/Intro",TRUE);
}
