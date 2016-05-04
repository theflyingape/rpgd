/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * rpgd.c
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

FILE *fp, *logfp, *user_fp;
struct sockaddr_un sockaddr;
struct B gang_master, userid_index, userlevel_index, username_index;
struct gang gang;
struct user *temp, *user;
struct userid userid;
struct userlevel userlevel;
struct username username;
struct ServerTable *table;
struct RPGserver *server;
struct LockTable {
	BYTE client[5];
	pid_t procid;
	BYTE entry[4][5];
	JDATE connectd;
	BYTE connectt[7];
	BYTE *pending;
	UBYTE timeout;
} LockTable[32];
UBYTE MaxClient = 32, client;
int Debug, DebugLevel, Loop = TRUE, online = 0, sockfd;
JDATE horolog;
BYTE thedate[9], thetime[7], today[9], yesterday[9];

void UnPackString(UBYTE *result, UBYTE *str, UBYTE size)
{
	struct radix {
		UBYTE one:6;
		UBYTE two:6;
		UBYTE three:6;
		UBYTE four:6;
	} radix;
	int i, j;

	memset(result, 0, size);
	for(i = 0, j = 0; i < size; i += 3) {
		memcpy(&radix, &str[i], 3);
		result[j + 0] = radix.one + ' ';
		result[j + 1] = radix.two + ' ';
		result[j + 2] = radix.three + ' ';
		result[j + 3] = radix.four + ' ';
		j += 4;
	}
	result[j - 1] = '\0';
}

UWORD CheckOne(char *str)
{
	UWORD i, result = 0;

	for(i = 0; i < 12; i++) {
		result -= i;
		result -= (str[i] - i) * (str[23 - i] - i);
	}
	return(result);
}

UWORD CheckTwo(char *str)
{
	UWORD i,result = 0;

	for(i = 0; i < 24; i += 2) {
		result += i;
		result += (str[i] + i) * (str[ i + 1] + i);
	}

	return(result);
}

void finputs(char *str, int n, FILE *file)
{
	if(!fgets(str, n, file))
		memset(str, 0, n);
	if(str[strlen(str) - 1] == '\n')
	str[strlen(str) - 1] = '\0';
}

void Broadcast(BYTE *str)
{
	int i;

	for(i = 0; i < MaxClient; i++)
		if(strlen(LockTable[i].client) && i != client) {
			if(LockTable[i].pending)
				LockTable[i].pending = realloc(LockTable[i].pending, strlen(LockTable[i].pending) + strlen(str) + 3);
			else
				LockTable[i].pending = calloc(1,strlen(str)+3);
			strcat(LockTable[i].pending, "\15\12");
			strcat(LockTable[i].pending, str);
		}
}

void HiScore(struct user *rec)
{
	double n;
	if(rec->ID[0] == '_' || rec->History.Calls < 10)
		return;
	rec->Rating = (1000. * rec->Wins
			+ 5. * (100. * rec->Immortal + rec->Level)
			+ rec->History.Joust + rec->History.KO
			+ rec->History.Calls
			- 5. * rec->History.Killed
			- rec->History.Retreats
			- rec->History.Kills / 5.)
			/ (horolog - rec->CreateDate + 1);
//			/ rec->History.Calls;
	if(rec->Rating >= table->HiScore->Rating) {
		table->HiScore->Rating = rec->Rating;
		strncpy(table->HiScore->Who[0], rec->Handle, sizeof(rec->Handle));
	}
	if(rec->History.HP >= table->HiScore->HP) {
		table->HiScore->HP = rec->History.HP;
		strncpy(table->HiScore->Who[1], rec->Handle, sizeof(rec->Handle));
	}
	if(rec->History.Hull >= table->HiScore->Hull) {
		table->HiScore->Hull = rec->History.Hull;
		strncpy(table->HiScore->Who[2], rec->Handle, sizeof(rec->Handle));
	}
 	n = (double)rec->History.Kills / rec->History.Calls;
	if(n >= table->HiScore->Kills) {
		table->HiScore->Kills = n;
		strncpy(table->HiScore->Who[3], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Calls / (horolog - rec->CreateDate + 1);
	if(n >= table->HiScore->Calls) {
		table->HiScore->Calls = n;
		strncpy(table->HiScore->Who[4], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Killed / rec->History.Calls;
	if(n >= table->HiScore->Killed) {
		table->HiScore->Killed = n;
		strncpy(table->HiScore->Who[5], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Retreats / rec->History.Calls;
	if(n >= table->HiScore->Retreats) {
		table->HiScore->Retreats = n;
		strncpy(table->HiScore->Who[6], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.KO / rec->History.Calls;
	if(n >= table->HiScore->KO) {
		table->HiScore->KO = n;
		strncpy(table->HiScore->Who[7], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Joust / rec->History.Calls;
	if(n >= table->HiScore->Joust) {
		table->HiScore->Joust = n;
		strncpy(table->HiScore->Who[8], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Stole / rec->History.Calls;
	if(n >= table->HiScore->Stole) {
		table->HiScore->Stole = n;
		strncpy(table->HiScore->Who[9], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Gamble / rec->History.Calls;
	if(n >= table->HiScore->Gamble) {
		table->HiScore->Gamble = n;
		strncpy(table->HiScore->Who[10], rec->Handle, sizeof(rec->Handle));
	}
	n = (double)rec->History.Fish / rec->History.Calls;
	if(n >= table->HiScore->Fish) {
		table->HiScore->Fish = n;
		strncpy(table->HiScore->Who[11], rec->Handle, sizeof(rec->Handle));
	}
}

int DelGang(struct gang *gp)
{
	memcpy(&gang, gp, sizeof(gang));
	Bget(&gang_master, (char *)&gang);
	if(gang_master.Bstatus == B_SUCCESSFUL) {
		Bdelete(&gang_master, (char *)&gang);
		return(TRUE);
	}
	return(FALSE);
}

int GetGang(struct gang *gp)
{
	memcpy(&gang, gp, sizeof(gang));
	Bget(&gang_master, (char *)&gang);
	if(gang_master.Bstatus == B_SUCCESSFUL) {
		memcpy(gp, &gang, sizeof(gang));
		return(TRUE);
	}
	memset(gp, 0, sizeof(gang));
	return(FALSE);
}

int PutGang(struct gang *gp)
{
	memcpy(&gang, gp, sizeof(gang));
	if(GetGang(&gang)) {
		memcpy(&gang, gp, sizeof(gang));
		Bupdate(&gang_master, (char *)&gang);
	}
	else {
		memcpy(&gang, gp, sizeof(gang));
		Bstore(&gang_master, (char *)&gang);
	}
	return(TRUE);
}

int DelUser(struct user *rec)
{
	char filename[32];
	int result;

	fprintf(logfp, "DelUser(\"%s\") ", rec->ID);
	fflush(logfp);
	sprintf(filename, "log/%s", rec->ID);
	remove(filename);
	sprintf(filename, "rm -f mail/%s.*", rec->ID);
	system(filename);
	sprintf(filename, "tmp/%s", rec->ID);
	remove(filename);

	memcpy(userid.ID, rec->ID, sizeof(rec->ID));
	userid.record = 0;
	Bget(&userid_index, (char *)&userid);
	fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
	fread((char *)user, USER_RECORD_SIZE, 1, user_fp);
	if(userid_index.Bstatus == B_SUCCESSFUL) {
		sprintf(userlevel.Current, "%03u", user->Level);
		memcpy(userlevel.ID, user->ID, sizeof(user->ID));
		Bget(&userlevel_index, (char *)&userlevel);
		if(userlevel_index.Bstatus == B_SUCCESSFUL)
			Bdelete(&userlevel_index, (char *)&userlevel);
		else {
			fprintf(logfp, "DelUser(\"%s\") -> %s\n -- Userlevel index error [%s]\n", rec->ID, Berror(&userlevel_index), userlevel.ID);
			fflush(logfp);
		}
		memcpy(username.Handle, user->Handle, sizeof(user->Handle));
		Bget(&username_index, (char *)&username);
		if(username_index.Bstatus == B_SUCCESSFUL)
			Bdelete(&username_index, (char *)&username);
		else {
			fprintf(logfp, "DelUser(\"%s\") -> %s\n -- Username index error [%s]\n", rec->ID, Berror(&username_index), username.Handle);
			fflush(logfp);
		}
		//	save link to next available record pointer...
		memset(user, 0, USER_RECORD_SIZE);
		user->LastTime = SYSREC->LastTime;
		fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
		fwrite((char *)user, USER_RECORD_SIZE, 1, user_fp);
		//	save the next available record pointer...
		SYSREC->LastTime = userid.record;
		fseek(user_fp, 0, SEEK_SET);
		fwrite((char *)SYSREC, USER_RECORD_SIZE, 1, user_fp);
		Bdelete(&userid_index, (char *)&userid);
		fprintf(logfp, "-> %s\n", Berror(&userid_index));
		fflush(logfp);
		//	return next user
		fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
		fread((char *)user, USER_RECORD_SIZE, 1, user_fp);
		result = TRUE;
	}
	else {
		result = FALSE;
		fprintf(logfp, "DelUser(\"%s\") -> %s\n -- User ID error [%s]\n", rec->ID, Berror(&userid_index), userid.ID);
		fflush(logfp);
	}
	return(result);
}

int GetUser(struct user *rec)
{
	memcpy(userid.ID, rec->ID, sizeof(rec->ID));
	userid.record = 0;
	Bget(&userid_index, (char *)&userid);
	if(userid_index.Bstatus) {
		memset(rec, 0, sizeof(struct user));
		return(FALSE);
	}
	fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
	fread((char *)temp, USER_RECORD_SIZE, 1, user_fp);
	memcpy(rec, temp, sizeof(struct user));
	return(TRUE);
}

int NextUser(struct user *rec)
{
	if(strlen(rec->ID)) {
		memcpy(userid.ID, rec->ID, sizeof(rec->ID));
		userid.record = 0;
		Bget(&userid_index, (char *)&userid);
		if(userid_index.Bstatus == B_SUCCESSFUL)
			Bnext(&userid_index, (char *)&userid);
	}
	else
		Bfirst(&userid_index, (char *)&userid);
	if(userid_index.Bstatus) {
		memset(rec, 0, sizeof(struct user));
		return(FALSE);
	}
	fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
	fread((char *)temp, USER_RECORD_SIZE, 1, user_fp);
	memcpy(rec, temp, sizeof(struct user));
	return(TRUE);
}

int PrevUser(struct user *rec)
{
	if(strlen(rec->ID)) {
		memcpy(userid.ID, rec->ID, sizeof(rec->ID));
		userid.record = 0;
		Bget(&userid_index, (char *)&userid);
		if(userid_index.Bstatus == B_SUCCESSFUL)
			Bprev(&userid_index, (char *)&userid);
	}
	else
		Blast(&userid_index, (char *)&userid);
	if(userid_index.Bstatus) {
		memset(rec, 0, sizeof(struct user));
		return(FALSE);
	}
	fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
	fread((char *)temp, USER_RECORD_SIZE, 1, user_fp);
	memcpy(rec, temp, sizeof(struct user));
	return(TRUE);
}

void PutUser(struct user *rec)
{
	memcpy(userid.ID, rec->ID, sizeof(rec->ID));
	userid.record = 0;
	Bget(&userid_index, (char *)&userid);
	if(userid_index.Bstatus == B_SUCCESSFUL) {
		fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
		fread((char *)temp, USER_RECORD_SIZE, 1, user_fp);
		fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
		fwrite((char *)rec, USER_RECORD_SIZE, 1, user_fp);
		if(rec->Level != temp->Level && rec->ID[0] != '_') {
			sprintf(userlevel.Current, "%03u", temp->Level);
			memcpy(userlevel.ID, temp->ID, sizeof(temp->ID));
			if(DebugLevel > 1) {
				fprintf(logfp, "   [%s] level was: %s   is: %u", userlevel.ID, userlevel.Current, rec->Level);
				fflush(logfp);
			}
			Bget(&userlevel_index, (char *)&userlevel);
			if(DebugLevel > 1) {
				fprintf(logfp, " [Bget() -> %s]", Berror(&userlevel_index));
				fflush(logfp);
			}
			if(userlevel_index.Bstatus == B_SUCCESSFUL) {
				Bdelete(&userlevel_index, (char *)&userlevel);
				if(DebugLevel > 1) {
					fprintf(logfp, " [Bdelete() -> %s]", Berror(&userlevel_index));
					fflush(logfp);
				}
			}
			sprintf(userlevel.Current, "%03u", rec->Level);
			memcpy(userlevel.ID, rec->ID, sizeof(rec->ID));
			Bstore(&userlevel_index, (char *)&userlevel);
			if(DebugLevel > 1) {
				fprintf(logfp," [Bstore() -> %s]\n", Berror(&userlevel_index));
				fflush(logfp);
			}
		}
	}
	else {
		memcpy(userid.ID, rec->ID, sizeof(rec->ID));
		userid.record = SYSREC->LastTime;
		fseek(user_fp, userid.record * USER_RECORD_SIZE, userid.record ? SEEK_SET : SEEK_END);
		if(userid.record) {
			fread((char *)temp, USER_RECORD_SIZE, 1, user_fp);
			SYSREC->LastTime = temp->LastTime;
			fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
		}
		else
			userid.record = ftell(user_fp) / USER_RECORD_SIZE;
		fwrite((char *)rec, USER_RECORD_SIZE, 1, user_fp);
		fseek(user_fp, 0, SEEK_SET);
		fwrite((char *)SYSREC, USER_RECORD_SIZE, 1, user_fp);
		Bstore(&userid_index, (char *)&userid);
		if(rec->ID[0] != '_') {		//  don't include system-records
			sprintf(userlevel.Current, "%03u", rec->Level);
			memcpy(userlevel.ID, rec->ID, sizeof(rec->ID));
			Bstore(&userlevel_index, (char *)&userlevel);
			memcpy(username.Handle, rec->Handle, sizeof(rec->Handle));
			memcpy(username.ID, rec->ID, sizeof(rec->ID));
			Bstore(&username_index, (char *)&username);
		}
	}
}

int RndUser(struct user *rec)
{
	ULONG node, key;

	do {
		do {
			node = dice((userid_index.Bhp->filesize - sizeof(struct Bheader)) / userid_index.Bhp->nodesize) - 1;
			Bgetnode(&userid_index, node * userid_index.Bhp->nodesize + sizeof(struct Bheader), 2);
		} while(*userid_index.Bnp[2]->numkeys == 0);
		key = dice(*userid_index.Bnp[2]->numkeys) - 1;
		memcpy(userid.ID, userid_index.Bnp[2]->key[key], sizeof(temp->ID));
		Bget(&userid_index, (char *)&userid);
		fseek(user_fp, userid.record * USER_RECORD_SIZE, SEEK_SET);
		fread((char *)temp, USER_RECORD_SIZE, 1, user_fp);
		memcpy(rec, temp, sizeof(struct user));
	} while(rec->ID[0] == '_');
	return(TRUE);
}

void LoadTables(void)
{
	struct B old_gang, new_gang;
	FILE *old_fp, *new_fp;
	char dated[16];
	int i, n;

	table=calloc(1, sizeof(struct ServerTable));
	LoadETC(table);
	temp = calloc(1,USER_RECORD_SIZE);
	user = calloc(1,USER_RECORD_SIZE);

	//	sanity check users
	if((old_fp = fopen("USER.master", "r+"))) {
		fread((char *)SYSREC, USER_RECORD_SIZE, 1, old_fp);
		if((new_fp = fopen("USER.new", "w+"))) {
			SYSREC->LastTime = 0;
			fwrite((char *)SYSREC, USER_RECORD_SIZE, 1, new_fp);
			for(n = 1; fread((char *)user, USER_RECORD_SIZE, 1, old_fp); n++) {
				if(strlen(user->ID)) {
					fwrite((char *)user, USER_RECORD_SIZE, 1, new_fp);
				}
			}
			fclose(old_fp);
			fclose(new_fp);
			remove("USER.old");
			rename("USER.master", "USER.old");
			rename("USER.new", "USER.master");
			user_fp = fopen("USER.master", "r+");
		}
	}
	else {
		user_fp = fopen("USER.master", "w+");
		strcpy(SYSREC->ID, "_SYS");
		strcpy(SYSREC->Handle, "");
		strcpy(SYSREC->Name, "Hack & Slash");
		SYSREC->BirthDate = horolog;
		SYSREC->CreateDate = horolog;
		SYSREC->ExpireDate = horolog + 365;
		SYSREC->Level = 33;
		fwrite((char *)SYSREC, USER_RECORD_SIZE, 1, user_fp);
		fprintf(logfp, "Creating system profile: %s\n", SYSREC->Name);
		fflush(logfp);
	}

	fseek(user_fp, 0, SEEK_SET);
	fread((char *)SYSREC, USER_RECORD_SIZE, 1, user_fp);
	// allow game to resume from rpgd re-start after someone wins the game:
	if(SYSREC->BirthDate > horolog) {
		SYSREC->BirthDate = horolog;
		fseek(user_fp, 0, SEEK_SET);
		fwrite((char *)SYSREC, USER_RECORD_SIZE, 1, user_fp);
	}

	Bcreate(&userid_index, "USERID.index", sizeof(struct userid));
	Bopen(&userid_index, "USERID.index");
	if(userid_index.Bstatus) {
		fprintf(logfp, "Error opening userid index.\n");
		fflush(logfp);
	}
	strcpy(userid.ID, "_SYS");
	userid.record = 0;
	Bstore(&userid_index, (char *)&userid);
	Bcreate(&userlevel_index, "USERLEVEL.index", sizeof(struct userlevel));
	Bopen(&userlevel_index, "USERLEVEL.index");
	if(userlevel_index.Bstatus) {
		fprintf(logfp, "Error opening userlevel index.\n");
		fflush(logfp);
	}
	Bcreate(&username_index, "USERNAME.index", sizeof(struct username));
	Bopen(&username_index, "USERNAME.index");
	if(username_index.Bstatus) {
		fprintf(logfp, "Error opening username index.\n");
		fflush(logfp);
	}

	for(n = 1; fread((char *)user, USER_RECORD_SIZE, 1, user_fp); n++) {
		if(strlen(user->ID)) {
			memcpy(userid.ID, user->ID, sizeof(user->ID));
			userid.record = n;
			Bstore(&userid_index, (char *)&userid);
			if(user->ID[0] != '_') {
				sprintf(userlevel.Current, "%03u", user->Level);
				memcpy(userlevel.ID, user->ID, sizeof(user->ID));
				Bstore(&userlevel_index, (char *)&userlevel);
				memcpy(username.Handle, user->Handle, sizeof(user->Handle));
				memcpy(username.ID, user->ID, sizeof(user->ID));
				Bstore(&username_index, (char *)&username);
				HiScore(user);
			}
		}
	}

	strcpy(NEWREC->ID, "_NEW");
	if(!GetUser(NEWREC)) {
		strcpy(NEWREC->ID, "_NEW");
		strcpy(NEWREC->Handle, "New User Record");
		NEWREC->ExpireDate = 365;
		NEWREC->Access = 1;
		NEWREC->Emulation = 1;
		NEWREC->Rows = 24;
		sprintf(NEWREC->Class, "%s.%s", table->class[0]->Origin, table->class[0]->Character[0]->Name);
		NEWREC->Level = 1;
		NEWREC->ExpLevel = 1;
		NEWREC->Gold = 1.;
		NEWREC->Bank = 5.;
		NEWREC->HP = 15;
		NEWREC->SP = 15;
		NEWREC->STR = 50;
		NEWREC->INT = 50;
		NEWREC->DEX = 50;
		NEWREC->CHA = 50;
		sprintf(NEWREC->Weapon, "%s.%s", table->weapon[0]->Origin, table->weapon[0]->Item[1]->Name);
		sprintf(NEWREC->Armor, "%s.%s", table->armor[0]->Origin, table->armor[0]->Item[1]->Name);
		NEWREC->Ram = 'N';
		NEWREC->Novice = 'Y';
		PutUser(NEWREC);
		fprintf(logfp, "Creating new user profile: %s\n", user->Handle);
		fflush(logfp);
	}

	strcpy(IRSREC->ID, "_IRS");
	if(!GetUser(IRSREC)) {
		memcpy(IRSREC, NEWREC, sizeof(struct user));
		strcpy(IRSREC->ID, "_IRS");
		strcpy(IRSREC->Handle, "Irwin R. Scheister");
		strcpy(IRSREC->Class, "IMMORTAL.Paladin");
		IRSREC->Sex = 'M';
		IRSREC->Gender = 'M';
		IRSREC->Bank = 10000.;
		IRSREC->CreateDate = horolog;
		fprintf(logfp, "Creating taxman: %s\n", IRSREC->Handle);
		fflush(logfp);
	}
	strcpy(IRSREC->Class, "IMMORTAL.Paladin");
	PutUser(IRSREC);

	strcpy(user->ID, "_TT");
	if(!GetUser(user)) {
		memcpy(user, NEWREC, sizeof(struct user));
		strcpy(user->ID, "_TT");
		strcpy(user->Handle, "Tiny, the 7'2\" barkeep");
		user->Sex = 'M';
		user->Gender = 'M';
		user->Level= SYSREC->Level * 4 / 5;
		user->Spell = HEAL_SPELL | CURE_SPELL;
		user->CreateDate = horolog;
		fprintf(logfp, "Creating tavern owner: %s\n", user->Handle);
		fflush(logfp);
	}
	strcpy(user->Class, "TAVERN.Ogre");
	PutUser(user);

	//	sanity check gangs
	Bopen(&old_gang, "GANG.master");
	if(old_gang.Bstatus == B_SUCCESSFUL) {
		Bcreate(&new_gang, "GANG.new", sizeof(struct gang));
		Bopen(&new_gang, "GANG.new");
		Bfirst(&old_gang, (char *)&gang);
		while(old_gang.Bstatus == B_SUCCESSFUL) {
			for(i = 0; i < 4; i++) {
				strcpy(user->ID, gang.MemberID[i]);
				if(!GetUser(user) && gang.MemberID[i][0] != '_')
					memset(gang.MemberID[i], 0, sizeof(gang.MemberID[i]));
			}
			if(strlen(gang.MemberID[0]))
				Bstore(&new_gang, (char *)&gang);
			Bnext(&old_gang, (char *)&gang);
		}
		Bclose(&old_gang);
		Bclose(&new_gang);
		remove("GANG.old");
		rename("GANG.master", "GANG.old");
		rename("GANG.new", "GANG.master");
	}
	Bopen(&gang_master, "GANG.master");
	if(gang_master.Bstatus) {
		Bcreate(&gang_master, "GANG.master", sizeof(struct gang));
		Bopen(&gang_master, "GANG.master");
		if(gang_master.Bstatus) {
			fprintf(logfp, "Error opening gang master.\n");
			fflush(logfp);
		}
	}
	fseek(user_fp, 0, SEEK_SET);
	for(n = 1; fread((char *)user, USER_RECORD_SIZE, 1, user_fp); n++) {
		if(strlen(user->ID) && user->ID[0] != '_' && strlen(user->Gang)) {
			memset(&gang, 0, sizeof(gang));
			strncpy(gang.Name, user->Gang, 20);
			Bget(&gang_master, (char *)&gang);
			if(gang_master.Bstatus) {
				memset(&gang, 0, sizeof(gang));
				gang.BannerType = dice(256) - 1;
				gang.BannerFore = 8 + dice(8) - 1;
				gang.BannerBack = dice(8) - 1;
				gang.BannerTrim = dice(8) - 1;
				strncpy(gang.Name, user->Gang, 20);
				strcpy(gang.MemberID[0], user->ID);
				Bstore(&gang_master, (char *)&gang);
			}
			else {
				for(i = 0; i < 4; i++) {
					if(!strcmp(gang.MemberID[i], user->ID))
						break;
					if(!strlen(gang.MemberID[i])) {
						strcpy(gang.MemberID[i], user->ID);
						Bupdate(&gang_master, (char *)&gang);
						break;
					}
				}
			}
		}
	}
	memset(&gang, 0, sizeof(gang));
	strcpy(gang.Name, "Monster Mash");
	Bget(&gang_master, (char *)&gang);
	if(gang_master.Bstatus) {
		//	create missing monsters
		memset(&gang, 0, sizeof(gang));
		strcpy(gang.Name, "Monster Mash");
		for(i = 0; i < 4; i++)
			sprintf(gang.MemberID[i], "_MM%d", i + 1);
		gang.BannerType = 0;
		gang.BannerFore = BLK;
		gang.BannerBack = GRN;
		gang.BannerTrim = BLACK;
		Bstore(&gang_master, (char *)&gang);
		fprintf(logfp, "Creating %s\n", gang.Name);
		fflush(logfp);
	}

	strcpy(SYSREC->ID, "_SYS");
	if(GetUser(SYSREC)) {
		SYSREC->Expert = FALSE;
		SYSREC->ExpireDate = horolog;
		SYSREC->Level = 99;
	}
	else {
		fprintf(logfp, "?GetUser(SYSREC) error!\n");
		fflush(logfp);
	}
	PutUser(SYSREC);
	fdate(dated, SYSREC->BirthDate);
	fprintf(logfp, "Current system profile: %s [Game %s %s]\n", SYSREC->Name, (SYSREC->BirthDate > horolog ? "will start" : "started"), dated);
	fflush(logfp);
}

void UnloadTables(void)
{
	fprintf(logfp, "closing files");
	fflush(logfp);

	Bclose(&userid_index);
	fprintf(logfp, ".");
	fflush(logfp);

	Bclose(&userlevel_index);
	fprintf(logfp, ".");
	fflush(logfp);

	Bclose(&username_index);
	fprintf(logfp, ".");
	fflush(logfp);

	Bclose(&gang_master);
	fprintf(logfp, ".");
	fflush(logfp);

	SYSREC->Expert = TRUE;
	fseek(user_fp, 0, SEEK_SET);
	fwrite((char *)SYSREC, USER_RECORD_SIZE, 1, user_fp);
	fclose(user_fp);
	fprintf(logfp, ".");

	fprintf(logfp, " freeing memory... ");
	fflush(logfp);

	if(user)
		free(user);
	if(temp)
		free(temp);
	if(table) {
		UnloadETC(table);
		free(table);
	}

	for(client = 0; client<MaxClient; client++)
		if(LockTable[client].pending)
			free(LockTable[client].pending);

	fprintf(logfp, "Ok.\n");
	fclose(logfp);
}

void ProcessServerSignal(int sockfd, pid_t clientid, char *pid, char command, char *parameter)
{
	int i, j, l, m, rec;
	UBYTE result;

	l = 0;
	result = FALSE;
	if(command) {
		for(client = 0; client < MaxClient; client++) {
			if(online > 0 && strlen(LockTable[client].client) > 0) {
				m = 1440 * (horolog - LockTable[client].connectd) + (atol(thetime) / 10000 * 60) + ((atol(thetime) / 100) % 100) - (atol(LockTable[client].connectt) / 10000 * 60) - ((atol(LockTable[client].connectt) / 100) % 100);
				if(LockTable[client].timeout || (!strcasecmp(LockTable[client].client, pid) && command == SERVER_CONNECT)) {
					if(LockTable[client].procid) {
						if(Debug) {
							if(command == SERVER_CONNECT)
								fprintf(logfp, "   Client #%d: [%s] reconnected after %d minutes (KILL %d).\n", client + 1, LockTable[client].client, m, LockTable[client].procid);
							else
								fprintf(logfp, "   Client #%d: [%s] timed-out after %d minutes (KILL %d).\n", client + 1, LockTable[client].client, m, LockTable[client].procid);
							fflush(logfp);
						}
						shutdown(sockfd, 0);
						close(sockfd);
						sockfd = 0;
						kill(LockTable[client].procid, SIGKILL);
					}
					if(LockTable[client].pending)
						free(LockTable[client].pending);
					memset(&LockTable[client], 0, sizeof(struct LockTable));
					online--;
					if(DebugLevel > 0) {
						fprintf(logfp, "***Online users: %d\n", online);
						fflush(logfp);
					}
				}
				else
					if(m > 9) {
						if(LockTable[client].procid) {
							if(Debug) {
								fprintf(logfp,"   Client #%d: [%s] timed-out after %d minutes (HUP %d).\n", client + 1, LockTable[client].client, m, LockTable[client].procid);
								fflush(logfp);
							}
							shutdown(sockfd, 0);
							close(sockfd);
							sockfd = 0;
							kill(LockTable[client].procid, SIGHUP);
						}
						LockTable[client].timeout = TRUE;
					}
			}
		}
		for(client = 0; client < MaxClient; client++) {
			if(!strcasecmp(LockTable[client].client, pid)) {
				LockTable[client].connectd = horolog;
				strcpy(LockTable[client].connectt, thetime);
				break;
			}
		}

		if(DebugLevel > 1)
			if(strlen(pid)) {
				if(client < MaxClient)
					fprintf(logfp, "-> Client #%d\n", client + 1);
				else
					fprintf(logfp, "-> Unknown client\n");
				fflush(logfp);
			}

		switch(command) {
			case SERVER_SHUTDOWN:
				Loop = FALSE;
				break;
			case SERVER_CONNECT:
				if(online >= MaxClient)
					break;
				if(strlen(pid)) {
					if(client == MaxClient) {
						online++;
						if(DebugLevel > 0) {
							fprintf(logfp, "***Online users: %d\n", online);
							fflush(logfp);
						}
						for(client = 0; client < MaxClient && strlen(LockTable[client].client); client++);
					}
					if(client < MaxClient) {
						result = TRUE;
						memset(&LockTable[client], 0, sizeof(struct LockTable));
						strcpy(LockTable[client].client, pid);
						LockTable[client].procid = clientid;
						LockTable[client].connectd = horolog;
						strcpy(LockTable[client].connectt, thetime);
						LockTable[client].timeout = 0;
						if(DebugLevel > 0) {
							fprintf(logfp, "   Client #%d: [%s] connected.\n", client + 1, pid);
							fflush(logfp);
						}
					}
				}
				else
					result = TRUE;
				break;
			case SERVER_DISCONNECT:
				result = TRUE;
				if(client < MaxClient && strlen(pid)) {
					if(DebugLevel > 0) {
						fprintf(logfp, "   Client #%d: [%s] disconnected.\n", client + 1, pid);
						fflush(logfp);
					}
					if(LockTable[client].pending)
						free(LockTable[client].pending);
					memset(&LockTable[client], 0, sizeof(struct LockTable));
					online--;
					if(DebugLevel > 0) {
						fprintf(logfp, "***Online users: %d\n", online);
						fflush(logfp);
					}
				}
				break;
			case SERVER_BROADCAST:
				Broadcast(parameter);
				result = TRUE;
				break;
			case SERVER_DELUSER:
				result = DelUser((struct user *)parameter);
				l = sizeof(struct user);
				break;
			case SERVER_GETUSER:
				result = GetUser((struct user *)parameter);
				l = sizeof(struct user);
				break;
			case SERVER_NEXTUSER:
				result = NextUser((struct user *)parameter);
				l = sizeof(struct user);
				break;
			case SERVER_PREVUSER:
				result = PrevUser((struct user *)parameter);
				l = sizeof(struct user);
				break;
			case SERVER_PUTUSER:
				memset(user, 0, USER_RECORD_SIZE);
				if(((struct user *)parameter)->Gold < 1.)
					((struct user *)parameter)->Gold = 0.;
				if(((struct user *)parameter)->Gold >= 1e22)
					((struct user *)parameter)->Gold = 1e22-1.;
				if(((struct user *)parameter)->Bank < 1.)
					((struct user *)parameter)->Bank = 0.;
				if(((struct user *)parameter)->Bank >= 1e22)
					((struct user *)parameter)->Bank = 1e22-1.;
				if(((struct user *)parameter)->Loan < 1.)
					((struct user *)parameter)->Loan = 0.;
				if(((struct user *)parameter)->Loan >= 1e22)
					((struct user *)parameter)->Loan = 1e22-1.;
				memcpy(user, parameter, sizeof(struct user));
				HiScore(user);
				PutUser(user);
				result = TRUE;
				l = sizeof(struct user);
				if(!strncmp(user->ID, IRSREC->ID, 4))
					memcpy(IRSREC, user, sizeof(struct user));
				if(!strncmp(user->ID, NEWREC->ID, 4))
					memcpy(NEWREC, user, sizeof(struct user));
				if(!strncmp(user->ID, SYSREC->ID, 4))
					memcpy(SYSREC, user, sizeof(struct user));
				break;
			case SERVER_RNDUSER:
				result = RndUser((struct user *)parameter);
				l = sizeof(struct user);
				break;
			case SERVER_GETUSERLEVEL:
				Bget(&userlevel_index, parameter);
				Bprev(&userlevel_index, parameter);
				result = (userlevel_index.Bstatus == B_END_OF_FILE) ? FALSE : TRUE;
				l = sizeof(struct userlevel);
				break;
			case SERVER_GETUSERNAME:
				Bget(&username_index, parameter);
				result = (username_index.Bstatus) ? FALSE : TRUE;
				l = sizeof(struct username);
				break;
			case SERVER_LOCK:
				result = TRUE;
				rec = 1;
				for(i = 0; i < MaxClient; i++) {
					if(!strcasecmp(LockTable[i].client, parameter)) {
						rec = (i == client) ? -1 : 0;
						break;
					}
					for(j = 0; j < 4; j++)
						if(!strcasecmp(LockTable[i].entry[j], parameter))
							rec = 0;
				}
				if(rec > 0) {
					if(client < MaxClient)
						for(i = 0; i < 4; i++)
							if(!strlen(LockTable[client].entry[i])) {
								strcpy(LockTable[client].entry[i], parameter);
								break;
							}
				}
				if(!rec)
					result = FALSE;
				break;
			case SERVER_UNLOCK:
				if(client < MaxClient) {
					if(LockTable[client].pending) {
						l = strlen(LockTable[client].pending) + 1;
						result = TRUE;
					}
					memset(&LockTable[client].entry, 0, sizeof(LockTable[client].entry));
				}
				break;
			case SERVER_DELGANG:
				result = DelGang((struct gang *)parameter);
				l = sizeof(struct gang);
				break;
			case SERVER_GETGANG:
				result = GetGang((struct gang *)parameter);
				l = sizeof(struct gang);
				break;
			case SERVER_NEXTGANG:
				Bget(&gang_master, parameter);
				if(!gang_master.Bstatus)
					Bnext(&gang_master, parameter);
				result = (gang_master.Bstatus == B_END_OF_FILE) ? FALSE : TRUE;
				l = sizeof(struct gang);
				break;
			case SERVER_PREVGANG:
				Bget(&gang_master, parameter);
				Bprev(&gang_master, parameter);
				result = (gang_master.Bstatus == B_END_OF_FILE) ? FALSE : TRUE;
				l = sizeof(struct gang);
				break;
			case SERVER_PUTGANG:
				result = PutGang((struct gang *)parameter);
				l = sizeof(struct gang);
				break;
			case SERVER_GETHISCORE:
				l = sizeof(struct HiScore);
				memcpy(parameter, table->HiScore, l);
				result = TRUE;
				break;
			case SERVER_WONGAME:
				fprintf(logfp, "*** [%s] won the game!\n", pid);
				fflush(logfp);
				for(i = 0; i < MaxClient; i++) {
					if(strlen(LockTable[i].client) > 0) {
						if(strcasecmp(LockTable[i].client, pid)) {
							if(LockTable[i].procid) {
								if(Debug) {
									fprintf(logfp, "   Client #%d: [%s] logged-out (KILL %d).\n", i+1, LockTable[i].client, LockTable[i].procid);
									fflush(logfp);
								}
								kill(LockTable[i].procid, SIGKILL);
							}
							if(LockTable[i].pending)
								free(LockTable[i].pending);
							memset(&LockTable[i], 0, sizeof(struct LockTable));
							online--;
						}
					}
				}
				memset(user, 0, sizeof(struct user));
				for(i = 0; NextUser((struct user *)user); i++) {
					if(user->ID[0] != '_') {
						ReKeySequence(user);
						reroll(user);
						memset(user->Email, 0, sizeof(user->Email));
						PutUser(user);
					}
				}
				l = 0;
				result = TRUE;
				break;
			default:
				break;
		}
	}
	if(DebugLevel>1) {
		fprintf(logfp, "-> Result (%s) with %d bytes ", result ? "SUCCESS" : "FAIL", l);
		fflush(logfp);
	}
	if(sockfd) {
		write(sockfd, &result, sizeof(result));
		write(sockfd, &l, sizeof(l));
		if(l > 0) {
			// send pending broadcasts back to client
			if(command == SERVER_UNLOCK) {
				write(sockfd, LockTable[client].pending, l);
				if(DebugLevel > 1) {
					fprintf(logfp, "\"%s\"...", LockTable[client].pending);
					fflush(logfp);
				}
				free(LockTable[client].pending);
				LockTable[client].pending = NULL;
			}
			else {
				write(sockfd, parameter, l);
				if(DebugLevel > 1) {
					fprintf(logfp, "\"%s\"...", parameter);
					fflush(logfp);
				}
			}
		}
		if(DebugLevel > 1) {
			fprintf(logfp, "\n");
			fflush(logfp);
		}
		shutdown(sockfd, 0);
		close(sockfd);
		sockfd = 0;
	}
}

void signalHUP(int signum)
{
	char *s;

	switch(signum) {
		case SIGHUP:
			s = "SIGHUP";
			break;
		case SIGINT:
			s = "SIGINT";
			break;
		case SIGIOT:
			s = "SIGIOT";
			break;
		case SIGPIPE:
			s = "SIGPIPE";
			break;
		case SIGQUIT:
			s = "SIGQUIT";
			break;
		case SIGTERM:
			s = "SIGTERM";
			break;
		default:
			s = "Error";
			break;
	}
	fprintf(logfp, "\n*** %s trap; rpgd shutting down: ", s);
	fflush(logfp);
	Loop = FALSE;
	close(sockfd);
	remove(sockaddr.sun_path);
}

int main(int argc, char **argv)
{
	struct passwd *passwd;
	struct group *group;
	gid_t newgid;
	uid_t newuid;
	mode_t mode;
	pid_t procid, clientid;
	struct sockaddr_un fromaddr;
	int newsockfd;
	int fromlen, i, j;
	char pid[5], command, parameter[USER_RECORD_SIZE];
	char dated[16];
	char daemon=FALSE;

	Debug = FALSE;
	DebugLevel = 0;
	for(i = 1; i < argc; i++) {
		//	0 = process info/errors, 1 = user activity, 2 = all
		if(strncmp(argv[i], "-d", 2) == 0) {
			Debug = TRUE;
			DebugLevel = atoi(&argv[i][2]);
		}
		//	detach process
		if(strncmp(argv[i], "-D", 2) == 0)
			daemon = TRUE;
	}
	if((fp = fopen("/etc/rpgd.conf", "r"))) {
		while(fgets(parameter, USER_RECORD_SIZE, fp)) {
			while(strlen(parameter) > 0 && isspace(parameter[strlen(parameter) - 1]))
				parameter[strlen(parameter) - 1] = '\0';
			if(strncmp(parameter, "HOME=", 5) == 0)
				chdir(&parameter[5]);
			if(strncmp(parameter, "GROUP=",6) == 0) {
				if(!(group = getgrnam(&parameter[6]))) {
					fprintf(stderr, "?Invalid group: \"%s\"\n", &parameter[6]);
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
		}
		fclose(fp);
	}

	if(daemon)
		if((procid = fork()))
			exit(0);
	procid = getpid();
	//	should any of these signals be raised, shutdown gracefully... we hope!
	signal(SIGHUP, (sighandler_t)signalHUP);
	signal(SIGINT, (sighandler_t)signalHUP);
	signal(SIGIOT, (sighandler_t)signalHUP);
	signal(SIGPIPE, (sighandler_t)signalHUP);
	signal(SIGQUIT, (sighandler_t)signalHUP);
	signal(SIGTERM, (sighandler_t)signalHUP);
	atexit(UnloadTables);

	GetSystemDate(today);
	julian(&horolog, today);
	strcpy(yesterday, today);
	fdate(dated, horolog);

	mode = 0;
	umask(mode);
	if((logfp = fopen("log/rpgd.log", "a"))) {
		setbuffer(logfp, NULL, 0);
		fcntl((long)logfp, F_SETFL, O_NONBLOCK);
		fprintf(logfp, "rpgd v2.5.7\n(C)1999-2014 Robert Hurst\nAll Rights Reserved.\n\nStarting up %s as process #%d\n", dated, procid);
		fflush(logfp);
	}
	if((sockfd = socket(PF_UNIX,SOCK_STREAM, 0)) < 0) {
		fprintf(logfp, "socket() error\n");
		fflush(logfp);
		exit(1);
	}
	strcpy(sockaddr.sun_path, "tmp/rpgd");
	sockaddr.sun_family = PF_UNIX;
	remove(sockaddr.sun_path);
	if((bind(sockfd, (struct sockaddr *)&sockaddr, strlen(sockaddr.sun_path) + sizeof(sockaddr.sun_family))) < 0) {
		fprintf(logfp, "bind() error\n");
		fflush(logfp);
		exit(1);
	}

	mode = 007;
	umask(mode);
	LoadTables();

	while(Loop) {
		if(DebugLevel > 1) {
			fprintf(logfp, "Listening...");
			fflush(logfp);
		}
		listen(sockfd, 10);
		fromlen = sizeof(fromaddr);
		if((newsockfd = accept(sockfd, (struct sockaddr *)&fromaddr, &fromlen)) < 0) {
			fprintf(logfp, "accept() error\n");
			fflush(logfp);
			continue;
		}
		if(DebugLevel > 1) {
			fprintf(logfp, "accepted.\n");
			fflush(logfp);
		}
		read(newsockfd, &clientid, sizeof(clientid));
		read(newsockfd, &pid, sizeof(pid));
		if(DebugLevel > 1) {
			fprintf(logfp, "-> [%d] %s", clientid, pid);
			fflush(logfp);
		}
		read(newsockfd, &command, 1);
		if(DebugLevel > 1) {
			fprintf(logfp, "   command: %hd", command);
			fflush(logfp);
		}
		read(newsockfd, parameter, sizeof(parameter));
		if(DebugLevel > 1) {
			fprintf(logfp, "   parameter: %s\n", parameter);
			fflush(logfp);
		}

		GetSystemDate(today);
		GetSystemTime(thetime);
		julian(&horolog, today);
		ProcessServerSignal(newsockfd, clientid, pid, command, parameter);

		if(strcmp(today, yesterday)) {
			if(Debug) {
				fprintf(logfp, "\n-=[%s]=-\n\n", today);
				fprintf(logfp, "***Online users: %d\n", online);
				fflush(logfp);
				for(i = 0; i < MaxClient; i++)
					if(strlen(LockTable[i].client)) {
						jdate(thedate, LockTable[i].connectd);
						if(DebugLevel) {
							fprintf(logfp, "   Client #%d: [%s] still connected since %s %s.\n", i+1, LockTable[i].client, thedate, LockTable[i].connectt);
							fflush(logfp);
						}
					}
			}
			strcpy(yesterday, today);
			remove("msg/Yesterday");
			rename("msg/Today", "msg/Yesterday");
			remove("msg/Yesterday.IBM");
			rename("msg/Today.IBM", "msg/Yesterday.IBM");
			for(i = 1; i < 8192; i++) {
				fseek(user_fp, i * USER_RECORD_SIZE, SEEK_SET);
				if(!fread((char *)user, USER_RECORD_SIZE, 1, user_fp))
					break;
				if(strlen(user->ID)) {
					if(user->ID[0] != '_') {
						if(user->Gold > 0.) {
							if(user->Loan > 0.) {
								user->Loan -= user->Gold;
								if(user->Loan < 0.) {
									user->Gold -= user->Loan;
									user->Loan = 0.;
								}
							}
							user->Bank += user->Gold;
							user->Gold = 0.;
							fseek(user_fp, i * USER_RECORD_SIZE, SEEK_SET);
							fwrite((char *)user, USER_RECORD_SIZE, 1, user_fp);
						}
						if((WORD)(horolog - user->LastDate) - user->Level - user->Immortal > 20 || horolog > user->ExpireDate) {
							if(strlen(user->Gang)) {
								sprintf(gang.Name, "%.20s", user->Gang);
								Bget(&gang_master, (char *)&gang);
								if(!gang_master.Bstatus) {
									for(j = 0; j < 4 && strcmp(user->ID, gang.MemberID[j]); j++);
									if(!j) {
										for(j = 0; j < 4; j++) {
											strcpy(user->ID, gang.MemberID[i]);
											if(!GetUser(user)) {
												memset(gang.MemberID[i], 0, sizeof(gang.MemberID[i]));
												PutUser(user);
											}
										}
										fprintf(logfp, "Auto-deleting gang, \"%.20s\"\n", gang.Name);
										fflush(logfp);
										Bdelete(&gang_master, (char *)&gang);
										fseek(user_fp, i * USER_RECORD_SIZE, SEEK_SET);
										fread((char *)user, USER_RECORD_SIZE, 1, user_fp);
									}
									else {
										memset(gang.MemberID[j], 0, sizeof(gang.MemberID[j]));
										Bupdate(&gang_master, (char *)&gang);
									}
								}
							}
							fprintf(logfp, "Auto-deleting player, \"%.22s\"\n", user->Handle);
							fflush(logfp);
							DelUser(user);
						}
					}
				}
			}
			Loop = FALSE;
		}
	}
	exit(0);
}
