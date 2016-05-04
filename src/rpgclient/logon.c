/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * logon.c
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


/*
char *mystrchr(const char *s, int c)
{
	do {
		if (*s == c)
			return (char*)s;
	} while (*s++);
	return (0);
}
*/

void TaxMan(void)
{
	double tax;
	int i;
	char c;

	JOUST(PLAYER.Level);
	d *= 1000.;
	if(PLAYER.Gold + PLAYER.Bank > d) {
		NL;
		tax = PLAYER.Gold + PLAYER.Bank - d;
		sprintf(outbuf, "%s, the tax collector, looks at your bulging money purse", IRSREC->Handle);
		OUT(outbuf); NL;
		OUT("and says, \"Ah, it is time to pay your taxes!\"  You check out the two burly"); NL;
		sprintf(outbuf, "guards who stand ready to enforce %s's will.", SYSREC->Handle);
		OUT(outbuf); NL; NL;
		Delay(100);
		sprintf(outbuf, "The tax will cost you %s.", money(tax,ANSI));
		OUT(outbuf); NL; NL;
		Delay(100);
		sprintf(prompt, "%sWill you pay the tax (Y/N)? ", fore(CYN));
		do {
			OUT(prompt);
			c = inkey('Y', 'Y');
			NL;
		} while(c != 'Y' && c != 'N');
		NL;
		if(c == 'N') {
			sound2("taxman", 0);
			OUT("The tax collector orders his guards, \"Run this outlaw through!\""); NL; NL;
			Delay(100);
			for(i = 0; i < 2; i++) {
				memset(RPC[1][i], 0, sizeof(struct RPC));
				sprintf(RPC[1][i]->user.Handle, "City Guard #%d", i + 1);
				sprintf(RPC[1][i]->user.Class, "%s.%s", table->class[0]->Origin, table->class[0]->Character[dice(MAXCLASS(0))-1]->Name);
				RPC[1][i]->user.Level = PLAYER.Level / 2 + 1;
				RPC[1][i]->user.Sex = 'I';
				sprintf(RPC[1][i]->user.Weapon, "NATURAL.%u", RPC[1][i]->user.Level/2 + dice(RPC[1][i]->user.Level/2));
				sprintf(RPC[1][i]->user.Armor, "NATURAL.%u", RPC[1][i]->user.Level/4 + dice(RPC[1][i]->user.Level/4));
				CreateRPC(RPC[1][i]);
				RPC[1][i]->user.Poison = (UWORD)~0;
				if(RPC[1][i]->SP)
					RPC[1][i]->user.Spell = HEAL_SPELL | BLAST_SPELL;
			}
			from = '\0';
			Battle();
			if(RPC[1][0]->HP > 0 || RPC[1][1]->HP > 0 || !ONLINE->HP)
				c = 'Y';
		}
		memcpy(&ENEMY, IRSREC, sizeof(struct user));
		if(c == 'N') {
			RPC[1][0]->user.Level = PLAYER.Level * 3 / 4 + 1;
			strcpy(ENEMY.Class, "IMMORTAL.Paladin");
			for(i = 0; i < MAXWEAPON(0) && tax >= table->weapon[0]->Item[i]->Value; i++);
			i += dice(3) - 3;
			i = (i < 1) ? 1 : (i >= MAXWEAPON(0)) ? MAXWEAPON(0) - 1 : i;
			sprintf(ENEMY.Weapon, "%s.%s", table->weapon[0]->Origin, table->weapon[0]->Item[i]->Name);
			for(i = 0; i < MAXARMOR(0) && tax >= table->armor[0]->Item[i]->Value; i++);
			i += dice(3) - 3;
			i = (i < 1) ? 1 : (i >= MAXARMOR(0)) ? MAXARMOR(0) - 1 : i;
			sprintf(ENEMY.Armor, "%s.%s", table->armor[0]->Origin, table->armor[0]->Item[i]->Name);
			CreateRPC(RPC[1][0]);
			ENEMY.WCmod = 0; ENEMY.ACmod = 0;
			RPC[1][0]->ToWC = 0; RPC[1][0]->ToAC = 0;
			RPC[1][0]->user.Poison = (UWORD)~0;
			if(RPC[1][0]->SP)
				RPC[1][0]->user.Spell = HEAL_SPELL | BLAST_SPELL | CONFUSION_SPELL | TRANSMUTE_SPELL | CURE_SPELL;
			OUT("The tax collector ");
			switch(dice(3)) {
				case 1:
					OUT("mutters, \"Good help is hard to find these days...\"");
					break;
				case 2:
					OUT("sighs, \"If you want a job done right...\"");
					break;
				case 3:
					OUT("swears, \"That's gonna cost you.\"");
					break;
			}
			NL; NL;
			Delay(100);
			sprintf(outbuf, "He pulls out his %s.", WEAPON(RPC[1][0])->Name);
			OUT(outbuf); NL; NL;
			Delay(100);
			sprintf(outbuf, "He is wearing his %s.", ARMOR(RPC[1][0])->Name);
			OUT(outbuf); NL; NL;
			Delay(100);
			from = 'L';
			Battle();
			from = '\0';
			if(ONLINE->HP > 0) {
				IRSREC->Gold = tax;
				IRSREC->Bank -= tax;
				if(IRSREC->Bank < 0.) {
					IRSREC->Gold += IRSREC->Bank;
					IRSREC->Bank = 0.;
				}
				if(IRSREC->Gold > 0.) {
					PLAYER.Gold += IRSREC->Gold;
					sprintf(outbuf, "You take %s he was carrying.", money(IRSREC->Gold,ANSI));
					OUT(outbuf); NL;
				}
			}
			if(RPC[1][0]->HP > 0) {
				c = 'Y';
				if(PLAYER.Gold > 0.) {
					IRSREC->Bank += PLAYER.Gold;
					sprintf(outbuf, "%s takes %s you were carrying.", RPC[1][0]->He, money(PLAYER.Gold,ANSI));
					OUT(outbuf); NL;
					PLAYER.Gold = 0.;
				}
				ExchangeWeapon(RPC[1][0], ONLINE);
				ExchangeArmor(RPC[1][0], ONLINE);
				OUT("\"Thanks for the taxes!\""); NL; NL;
				Delay(100);
			}
		}
		if(c == 'Y') {
			PLAYER.Bank -= tax;
			if(PLAYER.Bank < 0.) {
				PLAYER.Gold += PLAYER.Bank;
				PLAYER.Bank = 0.;
			}
			if(PLAYER.Gold < 0.) {
				PLAYER.Loan -= PLAYER.Gold;
				PLAYER.Gold = 0.;
			}
			ENEMY.Bank += tax;
			OUT("...and you pay the tax."); NL;
			paws =! PLAYER.Expert;
			Delay(100);
		}
		RPGserver(SERVER_PUTUSER, (UBYTE *)IRSREC);
	}
}

void GetCallers(void)
{
	FILE *fp;
	int i;

	if((fp = fopen("etc/callers", "r"))) {
		for(i = 0; i < 5; i++) {
			finputs(inbuf, INBUF_SIZE, fp);
			strcpy(callers.Last[i], inbuf);
			finputs(inbuf, INBUF_SIZE, fp);
			strcpy(callers.Reason[i], inbuf);
		}
		fclose(fp);
	}
}

int Logon(void)
{
	struct sockaddr_in iaddr;
//	struct hostent *hostent;
	FILE *fp, *newfp;
	UWORD t;
	int i, max, retry = 3;
	char timestamp[20];
	char c;
	char *m, *p;

	title("logon");
	term_init();
	term_character();

	strcpy(username.Handle, SYSREC->Handle);
	RPGserver(SERVER_GETUSERNAME, (UBYTE *)&username);
	strcpy(SYSID, username.ID);
	for(i = 0; i < 5; i++) {
		memset(RPC[0][i], 0, sizeof(struct RPC));
		memset(RPC[1][i], 0, sizeof(struct RPC));
	}
	ANSI = 2;
	chat = FALSE;
	myecho = TRUE;
	logoff = FALSE;
	paws = FALSE;
	rows = 25;
	memset(inbuf, 0, INBUF_SIZE);
	if(local)
		sound("dungeon", 48, 0);

	cls();
	type("msg/Logon", FALSE);
	GetSystemDate(Today);
	GetSystemTime(Time);
	julian(&Julian, Today);
	t = atol(Time) / 100;
	memset(Inet, 0, sizeof(Inet));
	i = sizeof(iaddr);
	getpeername(0, (struct sockaddr *)&iaddr, &i);
	strncpy(Inet, inet_ntoa(iaddr.sin_addr), sizeof(Inet));
	if(local)
		strcpy(Inet, "127.0.0.1");
//	if((hostent = gethostbyaddr(PLAYER.RemoteClient, strlen(PLAYER.RemoteClient), AF_INET))) {
//		strncpy(PLAYER.RemoteClient, hostent->h_name, sizeof(PLAYER.RemoteClient));
//	}

	while(retry) {
		NL;
		sprintf(prompt, "%sWho dares to enter my dank domain <or NEW>? \5", fore(CYN));
		timedout = 30;
		if(GetRPC(ONLINE)) {
			RPGserver(SERVER_DISCONNECT, (UBYTE *)PLAYER.ID);
			ANSI = PLAYER.Emulation;
			rows = PLAYER.Rows;
			sprintf(outbuf, "%s%s, enter your password: ", fore(CYN), PLAYER.Handle);
			OUT(outbuf);
			myecho = FALSE;
			timedout = 30;
			ins(9); NL;
			myecho = TRUE;
			if(!strcasecmp(inbuf, PLAYER.Password))
				break;
		}
		else
		if(cd()) {
			retry = 0;
			continue;
		}
		else
		if(!strcasecmp(inbuf, "new")) {
			NewUser();
			if(!strlen(PLAYER.ID))
				retry = 0;
			break;
		}
		else {
			beep();
			OUT(fore(GRY));
			OUT("Invalid response."); NL;
		}
		NL;
		switch(--retry) {
			case 2:
				OUT("The guards eye you suspiciously.");
				break;
			case 1:
				OUT("The guards aim their crossbows at you.");
				break;
			default:
				OUT("The last thing you ever feel is several quarrels cutting deep into your chest."); NL;
				Delay(50);
				break;
		}
		NL;
	}
	if(!retry)
		return(FALSE);

	if(!RPGserver(SERVER_CONNECT, NULL))
		return(FALSE);
	broadcast("logged on");

	i = 1440 * (Julian - PLAYER.LastDate) + (t / 100 * 60 + t % 100) - (PLAYER.LastTime / 100 * 60 + PLAYER.LastTime % 100);
	if(PLAYER.CallsToday > 0 && !strcmp(PLAYER.RemoteClient, Inet) && i < 2) {
		beep();
		sprintf(outbuf, "You were last on just %d minutes ago.", i);
		OUT(outbuf); NL;
		sprintf(outbuf, "Please wait at least %d minutes between calls.", 2);
		OUT(outbuf); NL;
		Delay(50);
		return(TRUE);
	}

	if(SYSREC->LastDate != Julian) {
		SYSREC->LastDate = Julian;
		SYSREC->CallsToday = 0;
	}
	SYSREC->CallsToday++;
	SYSREC->History.Calls++;
	if(SYSREC->CallsToday >= SYSREC->Current.Calls)
		SYSREC->Current.Calls = SYSREC->CallsToday;
	RPGserver(SERVER_PUTUSER, (UBYTE *)SYSREC);

	if(strlen(PLAYER.Email) < 8) {
		while(strlen(PLAYER.Email) < 8 && !cd()) {
			beep(); NL;
			OUT("Your account requires a validated e-mail address."); NL;
			OUT("Enter your e-mail address now: ");
			timedout = 90;
			ins(50); NL;
			proper(inbuf);
			strcpy(PLAYER.Email, inbuf);
			OUT("Re-enter to verify: ");
			timedout = 90;
			ins(50); NL;
			proper(inbuf);
			if(strcmp(PLAYER.Email, inbuf)) {
				beep();
				memset(PLAYER.Email, 0, sizeof(PLAYER.Email));
				continue;
			}
			memset(&ENEMY, 0, sizeof(struct user));
			t = 0;
			for(i = 0; RPGserver(SERVER_NEXTUSER, (UBYTE *)&ENEMY); i++) {
				if(strcmp(PLAYER.ID, ENEMY.ID)) {
					if(strcasecmp(PLAYER.Email, ENEMY.Email) == 0 && ENEMY.Access > 3)
						t++;
					if(strcasecmp(PLAYER.RemoteClient, ENEMY.RemoteClient) == 0 && ENEMY.Access > 3)
						t++;
				}
			}
			if(t > 3)
				PLAYER.Access = (PLAYER.Access < 8) ? 12 : 2;
			else {
				PLAYER.ExpireDate = NEWREC->ExpireDate + Julian;
				if(PLAYER.Access < 3)
					PLAYER.Access = 1;
			}
			RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
		}
		OUT("The king orders a scribe to issue your new keys to enter these gates."); NL;
		sprintf(PLAYER.Password, "%c%u%c", 'A' + dice(26) - 1, SYSREC->CallsToday, 'A' + dice(26) - 1);
		RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
		sprintf(filename, "tmp/%s", PLAYER.ID);
		if((newfp = fopen(filename, "w"))) {
			fprintf(newfp, "Subject: Your RPGd account information\n\n");
			fprintf(newfp, "This e-mail is being sent to you because it was used in registering to play\n");
			fprintf(newfp, "our online game.  You may sign in with the following account information:\n");
			fprintf(newfp, "\n");
			fprintf(newfp, "Your account ID is: %s - %s\n", PLAYER.ID, PLAYER.Handle);
			fprintf(newfp, "Your account Password is: %s\n", PLAYER.Password);
			fprintf(newfp, "\n");
			fprintf(newfp, "If this e-mail was sent to you by mistake, please take no action, \n");
			fprintf(newfp, "as this game account will automatically delete in a few days.\n\n");
			fprintf(newfp, "For the latest developments of RPGd, visit us at:\n\n");
			fprintf(newfp, "\thttp://robert.hurst-ri.us/games/rpgd\n\n");
			fprintf(newfp, "There, you can download the free Windows client software, or play\n");
			fprintf(newfp, "online with your Java-enabled web browser.\n");
			fprintf(newfp, "Linux users can check out my blog article to craft a nice client:\n");
			fprintf(newfp, "http://robert.hurst-ri.us/2011/10/29/linux-hack-slash-client\n");
			fclose(newfp);
			sprintf(inbuf, "sendmail -f webmaster@hurst-ri.us \"%s\" <%s", PLAYER.Email, filename);
			system(inbuf);
		}
		OUT("\21");
		return(TRUE);
	}
	else {
		//	Serf's up!!
		if(PLAYER.Access == 1)
			PLAYER.Access = 4;
	}

	if(PLAYER.LastDate != Julian)
		PLAYER.CallsToday = 0;

	// lucky Sunday & Wednesday
	fdate(dated, Julian);
	if((strncmp(dated, "Sun", 3) == 0 || strncmp(dated, "Wed", 3) == 0) && PLAYER.LastDate == Julian && PLAYER.LastTime < 1200 && t >= 1200)
		PLAYER.CallsToday = 0;

	if(PLAYER.CallsToday > ACCESS(PLAYER.Access)->Calls && !local) {
		beep(); NL;
		OUT("You have already used all your calls for today.  Please call back tomorrow!"); NL;
		sound("boo", 64, 0);
		Delay(25);
		return(TRUE);
	}
	if(ACCESS(PLAYER.Access)->Weekend == 'Y' && (Julian - 1) % 7 !=0 && (Julian - 1) % 7 != 6) {
		beep(); NL;
		OUT("You are restricted to Weekend play only.  Please call back then!"); NL;
		sound("boo", 64, 0);
		Delay(25);
		return(TRUE);
	}

	title("logon");
	sound2("logon", 0);
	emptykey();
	cls();
	sprintf(outbuf, "%s--=:))%s%s %s %s%s((:=--", fore(RED), fore(WHITE), back(RED), SYSREC->Name, back(BLK), fore(RED));
	OUT(outbuf); NL; NL;
	sprintf(outbuf, "%sCaller#:%s %u", fore(CYN), fore(WHITE), SYSREC->History.Calls);
	OUT(outbuf); NL;
	sprintf(outbuf, "%s Online:%s %s", fore(CYN), fore(WHITE), PLAYER.Handle);
	OUT(outbuf); NL;
	sprintf(outbuf, "%s Access:%s %s", fore(CYN), fore(WHITE), ACCESS(PLAYER.Access)->Name);
	OUT(outbuf);
	acclvl = 0; m = NULL;
	if(Julian >= PLAYER.ExpireDate) {
		sprintf(outbuf, " %s(%sEXPIRED%s)", fore(BLACK), fore(BRED), fore(BLACK));
		OUT(outbuf);
		m = "deleted user";
	}
	else
	if(PLAYER.CallsToday >= ACCESS(PLAYER.Access)->Calls || Julian < SYSREC->BirthDate) {
		PLAYER.CallsToday++;
		sprintf(outbuf, " %s(%sVISITING%s)", fore(BLACK), fore(YELLOW), fore(BLACK));
		OUT(outbuf);
		m = "visitor";
	}
	else {
		PLAYER.CallsToday++;
		PLAYER.Current.Calls++;
		PLAYER.History.Calls++;
		if(!strcmp(PLAYER.Status, "jail") && dice(100) > PLAYER.CHA / (CLASS(ONLINE)->Steal + 1)) {
			JOUST(PLAYER.Level);
			modf(d * (100 - PLAYER.CHA + 1) / 100., &d);
			NORMAL; NL; NL;
			OUT("You are still locked-up in jail."); NL;
			sprintf(outbuf, "It will cost you %s to get bailed-out and to continue play.", money(d, ANSI));
			OUT(outbuf); NL; NL;
			sprintf(prompt, "%sWill you pay (Y/N)? ", fore(CYN));
			OUT(prompt);
			c = inkey('Y', 'N');
			NL; NL;
			if(c == 'Y') {
				OUT("You are set free to continue your adventuring."); NL;
				acclvl = PLAYER.Access;
				memset(PLAYER.Status, 0, sizeof(PLAYER.Status));
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
			else {
				OUT("You are left brooding with your fellow cellmates."); NL;
				m = "prisoner";
			}
		}
		else {
			acclvl = PLAYER.Access;
			memset(PLAYER.Status, 0, sizeof(PLAYER.Status));
		}
	}
	NL;
	if(ACCESS(acclvl)->RolePlay == 'Y') {
		arena = 3;
		bail = 1;
		brawl = 3;
		charity = 1;
		dungeon = 3; nest = 0;
		joust = 3;
		naval = 3;
		party = 1;
		realestate = 1;
		security = 1;
		tiny = 0;
		PLAYER.ExpLevel = PLAYER.Level;
		if(PLAYER.Level == 1 && PLAYER.Current.JoustLoss > 2 * PLAYER.Current.JoustWin) {
			PLAYER.Current.JoustWin = 0;
			PLAYER.Current.JoustLoss = 0;
			OUT("Your jousting stats have been reset."); NL;
		}
		m = NULL;
	}
	else {
		arena = 0;
		bail = 0;
		brawl = 0;
		charity = 0;
		dungeon = 0; nest = 0;
		joust = 0;
		naval = 0;
		party = 0;
		realestate = 0;
		security = 0;
		tiny = 3;
	}

	if(ACCESS(acclvl)->RolePlay == 'Y')
		if(!ONLINE->class_origin && !ONLINE->class_type) {
			ReKeySequence(&PLAYER);
			reroll(&PLAYER);
			max = 200;
			if(ACCESS(PLAYER.Access)->Sysop == 'Y') {
				if((max = 200 + dice(PLAYER.Immortal)) > 240)
					max = 240;
				max += 10 * PLAYER.Wins;
			}
			assign(max);
		}

	fdate(dated, PLAYER.LastDate);
	sprintf(inbuf, "%06lu", PLAYER.LastTime * 100L);
	ftime(timed, inbuf);
	sprintf(outbuf, "%sLast on:%s %s %s  %s(%s%.24s%s)", fore(CYN), fore(WHITE), dated, timed, fore(BLACK), fore(BRN), Inet, fore(BLACK));
	OUT(outbuf); NL; NL;
	PLAYER.LastDate = Julian;
	PLAYER.LastTime = t;
	strncpy(PLAYER.RemoteClient, Inet, sizeof(PLAYER.RemoteClient));
	ftime(timed, Time);
	sprintf(filename, "tmp/%s", PLAYER.ID);
	if((newfp = fopen(filename, "w"))) {
		fprintf(newfp, "%s-=>%s %s %s<=-\n", fore(BLU), fore(YELLOW), (timed[0] == ' ' ? &timed[1] : timed), fore(BLU));
		fprintf(newfp, "%s%s%s logged on as a ", fore(CYAN), PLAYER.Handle, fore(CYN));
		if(m)
			fprintf(newfp, "%s\n", m);
		else
			fprintf(newfp, "level %s%u %s\33[0;37m\n", fore(WHITE), PLAYER.Level, CLASS(ONLINE)->Name);
		fclose(newfp);
	}

	if(acclvl) {
		sprintf(outbuf, "%sYou have %d calls remaining.", fore(CYN), ACCESS(acclvl)->Calls - PLAYER.CallsToday);
		OUT(outbuf); NL; NL;
		jdate(dated, PLAYER.BirthDate);
		if(!strncmp(&Today[4], &dated[4], 4) && PLAYER.CallsToday == 1) {
			JOUST(PLAYER.Level);
			i = atol(Today) / 10000 - atol(dated) / 10000;
			d *= (double)i;
			sprintf(outbuf, "%sHappy %d%s Birthday!!  You receive %s%s as a present.", fore(CYAN), i, (i % 10 == 1 ? "st" : i % 10 == 2 ? "nd" : i % 10 == 3 ? "rd" : "th"), money(d, ANSI), fore(CYAN));
			OUT(outbuf); NL; NL;
			PLAYER.Gold += d;
			sprintf(outbuf, "%scelebrating %s%d%s%s birthday", fore(CYN), fore(YELLOW), i, (i % 10 == 1 ? "st" : i % 10 == 2 ? "nd" : i % 10 == 3 ? "rd" : "th"), fore(CYN));
			news(outbuf);
		}
	}
	strcpy(gm[0].Name, PLAYER.Gang);
	if(!RPGserver(SERVER_GETGANG, (UBYTE *)&gm[0]))
		memset(PLAYER.Gang, 0, sizeof(PLAYER.Gang));
	RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);

	GetCallers();
	sprintf(outbuf, "%sLast callers were:%s ", fore(CYN), fore(GRY));
	OUT(outbuf);
	for(i = 0; i < 5; i++) {
		sprintf(outbuf, "%s %s(%s%s%s)%s", callers.Last[i], fore(CYN), fore(GRY), callers.Reason[i], fore(CYN), fore(GRY));
		OUT(outbuf); NL;
		OUT("                   ");
	}
	NL;
	memset(reason, 0, sizeof(reason));
	if(Julian < SYSREC->BirthDate) {
		beep();
		fdate(dated, SYSREC->BirthDate);
		sprintf(outbuf, "%sThe game will resume %s*%s%s%s*", fore(CYN), fore(YELLOW), fore(CYAN), dated, fore(YELLOW));
		OUT(outbuf); NL; NL;
	}
	emptykey();
	paused();

	sprintf(filename, "log/%s", PLAYER.ID);
	if(type(filename, TRUE))
		remove(filename);
	if(cd())
		return(TRUE);

	cls();
	sprintf(outbuf, "       %s------------", fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "--=:))%s Announcement %s((:=--", fore(YELLOW), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "       ------------");
	OUT(outbuf); NORMAL; NL;
	type("msg/Announcement", FALSE); NL;
	if(ACCESS(PLAYER.Access)->Sysop == 'Y') {
		sprintf(prompt, "%sChange Announcement (Y/N)? ", fore(CYN));
		OUT(prompt);
		if(inkey('N', 'N') == 'Y') {
			NL; NL;
			Editor(NULL, 10);
			if(numline) {
				if((fp = fopen("msg/Announcement", "w"))) {
					fdate(dated, Julian);
					ftime(timed, Time);
					fprintf(fp, "%sDate:%s %s %s\n", fore(CYN), fore(GRY), dated, timed);
					fprintf(fp, "%sFrom:%s %s%s\n\n", fore(CYN), fore(GRY), PLAYER.Handle, fore(WHITE));
					for(i = 0; i < numline; i++)
						fprintf(fp, "%s\n", line[i]);
					fclose(fp);
				}
			}
		}
		NL;
	}
	sprintf(outbuf, "       %s------------", fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "--=:))%s Auto-Message %s((:=--", fore(YELLOW), fore(BLU));
	OUT(outbuf); NL;
	sprintf(outbuf, "       ------------");
	OUT(outbuf); NORMAL; NL;
	type("msg/Auto-Message", FALSE); NL;
	if(ACCESS(acclvl)->RolePlay == 'Y') {
		sprintf(prompt, "%sChange Auto-Message (Y/N)? ", fore(CYN));
		OUT(prompt);
		if(inkey('N', 'N') == 'Y') {
			NL; NL;
			Editor(NULL, 5);
			if(numline) {
				if((fp = fopen("msg/Auto-Message", "w"))) {
					fdate(dated, Julian);
					ftime(timed, Time);
					fprintf(fp, "%sDate:%s %s %s\n", fore(CYN), fore(GRY), dated, timed);
					fprintf(fp, "%sFrom:%s %s%s\n\n", fore(CYN), fore(GRY), PLAYER.Handle, fore(WHITE));
					sprintf(outbuf, "%schanged the Auto-Message to read:", fore(CYN));
					news(outbuf);
					for(i = 0; i < numline; i++) {
						fprintf(fp, "%s\n", line[i]);
						news(line[i]);
					}
					fclose(fp);
				}
			}
		}
		NL;
	}
	else {
		paused();
		NL;
	}

	if(cd())
		return(TRUE);
	for(i = 1; i < 999 && !cd(); i++) {
		sprintf(outbuf, "%s.%03d", PLAYER.ID, i);
		sprintf(filename, "mail/%s", outbuf);
		if(ReadMail(filename)) {
			remove(filename);
			sprintf(prompt, "%sReply (Y/N)? ", fore(CYN));
			do {
				OUT(prompt);
				c = inkey('N', 'N');
				NL;
			} while(c != 'Y' && c != 'N');
			NL;
			if(c == 'Y') {
				Editor(RPC[1][0], 99);
				for(retry = 1; retry < 999; retry++) {
					sprintf(outbuf, "%s.%03d", ENEMY.ID, retry);
					sprintf(filename, "mail/%s", outbuf);
					if(!(fp = fopen(filename, "r")))
						break;
					fclose(fp);
				}
				WriteMail(filename);
			}
		}
		else
			break;
	}

	if(ACCESS(acclvl)->RolePlay == 'Y')
		TaxMan();

	MainMenu();

	title("logoff");

	GetSystemDate(Today);
	GetSystemTime(Time);
	julian(&Julian, Today);
	fdate(dated, Julian);
	ftime(timed, Time);
	if(PLAYER.LastDate != Julian)
		PLAYER.CallsToday = 0;
	PLAYER.LastDate = Julian;
	PLAYER.LastTime = atol(Time) / 100;
	RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);

	memset(outbuf, 0, OUTBUF_SIZE);
	if(ACCESS(acclvl)->RolePlay == 'Y') {
		sprintf(outbuf, "%slogged off as a level %s%u %s", fore(CYN), fore(WHITE), PLAYER.Level, CLASS(ONLINE)->Name);
		news(outbuf);
		sprintf(outbuf, "%s(%s%s%s)\n", fore(CYN), fore(WHITE), reason, fore(CYN));
	}

	news(outbuf);
	if(ACCESS(acclvl)->RolePlay == 'Y') {

		strcpy(filename, "msg/Today.IBM");
		if((newfp = fopen(filename, "a"))) {
			sprintf(filename, "tmp/%s", PLAYER.ID);
			if((fp = fopen(filename, "r"))) {
				while(fgets(inbuf, INBUF_SIZE, fp))
					fputs(inbuf, newfp);
				fclose(fp);
			}
			fclose(newfp);
		}

		strcpy(filename, "msg/Today");
		if((newfp = fopen(filename, "a"))) {
			sprintf(filename, "tmp/%s", PLAYER.ID);
			if((fp = fopen(filename,"r"))) {
				while(fgets(inbuf, INBUF_SIZE, fp)) {
					p = inbuf;
					while((p = strchr(p, '\33'))) {
						if((m = strchr(p, 'm'))) {
							strcpy(outbuf, m + 1);
							strcpy(p, outbuf);
						}
					}
					fputs(inbuf, newfp);
				}
				fclose(fp);
			}
			fclose(newfp);
		}

		GetCallers();
		strcpy(callers.Last[4], callers.Last[3]); strcpy(callers.Reason[4], callers.Reason[3]);
		strcpy(callers.Last[3], callers.Last[2]); strcpy(callers.Reason[3], callers.Reason[2]);
		strcpy(callers.Last[2], callers.Last[1]); strcpy(callers.Reason[2], callers.Reason[1]);
		strcpy(callers.Last[1], callers.Last[0]); strcpy(callers.Reason[1], callers.Reason[0]);
		strcpy(callers.Last[0], PLAYER.Handle); strcpy(callers.Reason[0], reason);
		PutCallers(&callers);

		if(strlen(led.dev)) {
			if((fp = fopen(led.dev, "w"))) {
				//	bag of money
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\0'
					, '\16', '\04', '\16', '\37', '\37', '\37', '\16', '\0');
				//	weapon
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\1'
					, '\00', '\20', '\10', '\05', '\02', '\05', '\00', '\0');
				//	armor
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\2'
					, '\25', '\33', '\21', '\21', '\21', '\12', '\4', '\0');
				//	no security
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\3'
					, '\16', '\21', '\20', '\37', '\37', '\37', '\37' ,'\0');
				//	strong magic
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\4'
					, '\04', '\25', '\04', '\33', '\04', '\25', '\04', '\0');
				//	blessed
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\5'
					, '\16', '\37', '\25', '\37', '\25', '\33', '\16', '\0');
				//	cursed
				fprintf(fp, "\376N%c%c%c%c%c%c%c%c%c", '\6'
					, '\16', '\37', '\25', '\37', '\33', '\25', '\16', '\0');
				//	Brightness 50%, Blink off, Display on for n-minutes,
				//	Autowrap on, Scroll off, clear display
				fprintf(fp, "\376Y\2\376T\376B%c\376C\376R\376X", led.mins);
				ftime(timestamp, Time);
				sprintf(outbuf, "%s %s at %s",PLAYER.Handle, reason, timestamp[0] == ' ' ? &timestamp[1] : timestamp);
				p = outbuf;
				while(p) {
					if(strlen(p) > led.cols) {
						for(i = led.cols; i > 0 && !isspace(p[i]) && (!ispunct(p[i]) || p[i] == ':'); i--);
						p[i] = '\0';
						fprintf(fp, "%s", p);
						if(i < led.cols)
							fprintf(fp, "\n");
						p += i + 1;
					}
					else {
						fprintf(fp, "%s", p);
						p = '\0';
					}
				}
				fputc(' ', fp);
				if(PLAYER.Gold + PLAYER.Bank >= 1e+05 && PLAYER.Level < 10)
					fputc('\0', fp);
				if(PLAYER.Gold + PLAYER.Bank >= 1e+09 && PLAYER.Level < 20)
					fputc('\0', fp);
				if(PLAYER.Gold + PLAYER.Bank >= 1e+13 && PLAYER.Level < 30)
					fputc('\0', fp);
				if(PLAYER.Gold + PLAYER.Bank >= 1e+14)
					fputc('\0', fp);
				if(PLAYER.Gold + PLAYER.Bank >= 1e+15)
					fputc('\0', fp);
				if(PLAYER.Gold + PLAYER.Bank >= 1e+16)
					fputc('\0', fp);
				if(ONLINE->weapon_origin)
					fputc('\1', fp);
				if(PLAYER.WCmod > 2 * CLASS(ONLINE)->Poison)
					fprintf(fp, "+\1");
				if(PLAYER.WCmod < -2)
					fprintf(fp, "-\1");
				if(ONLINE->armor_origin)
					fputc('\2', fp);
				if(PLAYER.ACmod > 2 * CLASS(ONLINE)->Magic)
					fprintf(fp, "+\2");
				if(PLAYER.ACmod < -2)
					fprintf(fp, "-\2");
				if(PLAYER.Level / 9 > PLAYER.Security)
					fputc('\3', fp);
				if(PLAYER.Spell > CONFUSION_SPELL)
					fputc('\4', fp);
				if(PLAYER.XSpell)
					fputc('\4', fp);
				if(strlen(PLAYER.Blessed))
					fputc('\5', fp);
				if(strlen(PLAYER.Cursed))
					fputc('\6', fp);
				fclose(fp);
			}
		}
	}
	sprintf(filename, "tmp/%s", PLAYER.ID);
	remove(filename);
	if(logoff) {
		sound("logoff", 48, 0);
		type("msg/Logoff", FALSE);
		Delay(75);
		sprintf(outbuf, "%srpgd-2.5.7 written by Robert Hurst", fore(BLACK));
		OUT(outbuf); NL;
		Delay(50);
		NORMAL;
		OUT("Visit us at http://robert.hurst-ri.us/games/rpgd"); NL;
		Delay(50);
		if(ONLINE->HP > 0)
			sound2("logoff", 0);
		OUT("\21");
	}
	broadcast(reason);
	return(TRUE);
}
