/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * common.c
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

FILE *fp;
char daytbl[7][3] = {
	{'S','u','n'},{'M','o','n'},{'T','u','e'},{'W','e','d'},{'T','h','u'},{'F','r','i'},{'S','a','t'}
};
char montbl[12][3] = {
	{'J','a','n'},{'F','e','b'},{'M','a','r'},{'A','p','r'},{'M','a','y'},{'J','u','n'},
	{'J','u','l'},{'A','u','g'},{'S','e','p'},{'O','c','t'},{'N','o','v'},{'D','e','c'}
};

#define ISLEAP (((year%4)==0) && (((year%100)!=0) || ((year%400)==0)))

void Delay(int ticks)
{
	struct timespec t;

	t.tv_sec=ticks/50;
	t.tv_nsec=(ticks%50)*15000000L;
	nanosleep(&t,NULL);
}

int dice(int n)
{
	double d;
	int r;

	d=n;
	if(n>0)
		r=1+(int)(d*rand()/(RAND_MAX+1.0));
	else
		r=1;
	return(r);
}

void strlwr(BYTE *src)
{
	for(; *src; src++)
		*src = tolower(*src);
}

void strupr(BYTE *src)
{
	for(; *src; src++)
		*src = toupper(*src);
}

void swmem(BYTE *dest, BYTE *src, int s)
{
	int i;
	BYTE c;

	for(i = 0; i < s; i++) {
		c = dest[i];
		dest[i] = src[i];
		src[i] = c;
	}
}

void GetSystemDate(BYTE *result)
{
	LONG t;

	time(&t);
	strftime(result, 9, "%Y%m%d", localtime(&t));
}

void GetSystemTime(BYTE *result)
{
	struct tm *tp;
	LONG t;

	time(&t);
	tp=localtime(&t);
	strftime(result,7,"%H%M%S",tp);
}

void jdate(BYTE *result, JDATE days)
{
	UWORD md[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
	UWORD month,day,year;

	year=(UWORD)(days/1461*4) + (UWORD)(days%1461/365);
	days=days-(year*365L+year/4)+1;
	month=0;
	while(days>md[month+1]-(ISLEAP && month==0 ? 1 : 0) && month<11)
		month++;
	days-=md[month++];
	day=days;
	if(ISLEAP && month<3)
		day++;
	sprintf(result,"%04u%02u%02u",year,month,day);
}

void fdate(BYTE *result, JDATE days)
{
	BYTE date[9];

	jdate(date, days);
	sprintf(result, "%.3s %.2s-%.3s-%.4s", daytbl[(days - 1) % 7], &date[6], montbl[10 * (date[4] - '0') + (date[5] - '0') - 1], &date[0]);
}

void ftime(BYTE *result, BYTE *time)
{
	ULONG t;
	BYTE hour[3], ap;

	t = atol(time) / 10000L;
	ap = t < 12 ? 'a' : 'p';
	sprintf(hour, "%2lu", (t > 12 ? t - 12 : (t < 1 ? 12 : t)));
	sprintf(result, "%s:%.2s%cm", hour, &time[2], ap);
}

void julian(JDATE *result, BYTE *str)
{
	UWORD md[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
	ULONG month,day,year;
	BYTE date[9],*p;

	GetSystemDate(date);
	sscanf(date,"%04ld%02ld%02ld",&year,&month,&day);
	if((p=strchr(str,'/'))) {
		month=atoi(str);
		day=atoi(++p);
		if((p=strchr(p,'/')))
			year=atoi(++p);
	}
	else
	if((p=strchr(str,'-'))) {
		month=atoi(str);
		day=atoi(++p);
		if(!day) {
			day=month;
			for(month=0; month<12 && strncasecmp(p,montbl[month],3); month++);
			month++;
		}
		if((p=strchr(p,'-')))
			year=atoi(++p);
	}
	else
	if(atol(str)>18991231L)
		sscanf(str,"%04ld%02ld%02ld",&year,&month,&day);
	else
		sscanf(str,"%02ld%02ld%ld",&month,&day,&year);
	month=(month<1) ? 1 : (month>12) ? 12 : month;
	day=(day<1) ? 1 : (day>31) ? 31 : day;
	if(year<100)
		year+=1900;
	*result=year*365L+year/4+md[month-1]+day-1;
	if(ISLEAP && month<3)
		(*result)--;
}

void gold(DOUBLE *result, BYTE *str)
{
	DOUBLE gold;
	BYTE *piece;

	*result = 0.;
	gold = 0.;
	for(piece = str; *piece; piece++) {
		switch(toupper(*piece)) {
			case 'C':
				*result += gold;
				break;
			case 'S':
				*result += gold * 1e+05;
				break;
			case 'G':
				*result += gold * 1e+09;
				break;
			case 'P':
				*result += gold * 1e+13;
				break;
		}
		if(isdigit(*piece)) {
			gold *= 10.;
			gold += (*piece - '0');
		}
		else
			gold = 0.;
	}
	*result += gold;
}

void proper(BYTE *str)
{
	int i, f = TRUE;

	while(*str) {
		if(!isalnum(*str))
			memmove(str, &str[1], strlen(str));
		else
			break;
	}
	while((i = strlen(str))) {
		if(!isalnum(str[i - 1]))
			str[i - 1] = '\0';
		else
			break;
	}
	i = 0;
	while(i < (int)(strlen(str) - 1)) {
		if(!isalnum(str[i]) && !isalnum(str[i + 1]))
			memmove(&str[i], &str[i + 1], strlen(&str[i]));
		else
			i++;
	}
	strlwr(str);
	for(i = 0; i < strlen(str); i++) {
		if(f) {
			str[i] = toupper(str[i]);
			f = FALSE;
		}
		if(str[i] < 'A' && str[i] != '\'')
			f = TRUE;
	}
}

int cuss(BYTE *str)
{
	int i,result=FALSE;
	char buffer[120];
	char *cuss;

	strncpy(buffer,str,sizeof(buffer));
	buffer[sizeof(buffer)-1]='\0';
	for(i=0; i<(int)strlen(buffer); i++) {
		if(buffer[i]=='$')
			buffer[i]='s';
		while(ispunct(buffer[i]) || isspace(buffer[i]))
			memmove(&buffer[i],&buffer[i+1],strlen(&buffer[i]));
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'a'))) {
		if(!strncasecmp(cuss,"asshole",7))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'c'))) {
		if(!strncasecmp(cuss,"cock",4))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'c'))) {
		if(!strncasecmp(cuss,"cunt",4))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'f'))) {
		if(!strncasecmp(cuss,"fuck",4))
			result=TRUE;
		if(!strncasecmp(cuss,"fck",3))
			result=TRUE;
		if(!strncasecmp(cuss,"fuk",3))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'p'))) {
		if(!strncasecmp(cuss,"phuc",4))
			result=TRUE;
		if(!strncasecmp(cuss,"phuk",4))
			result=TRUE;
		cuss++;
	}
	return(result);
}

BYTE *GetFile(BYTE *filename)
{
	BYTE *buffer = NULL;
	int size;

	if((fp = fopen(filename, "r"))) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if((buffer = calloc(1, size + 2))) {
			fseek(fp, 0, SEEK_SET);
			fread(buffer, size, 1, fp);
			strcpy(&buffer[size], "\n");
		}
		else {
			fclose(fp);
			fp = NULL;
		}
	}
	return(buffer);
}

void OriginName(char *src, char *origin, char *name)
{
	char *dot,field[41];

	strcpy(field,src);
	if((dot=strpbrk(field,"."))) {
		*dot='\0';
		strcpy(origin,field);
		strcpy(name,++dot);
	}
	else {
		*origin='\0';
		*name='\0';
	}

}

void LoadETC(struct ServerTable *table)
{
	JDATE horolog;
	int i, n, o, t;
	BYTE field[128];
	BYTE *buffer,*c;
	char result[4][32];
	char today[9];

	GetSystemDate(today);
	julian(&horolog, today);

	IRSREC = calloc(1, USER_RECORD_SIZE);
	NEWREC = calloc(1, USER_RECORD_SIZE);
	SYSREC = calloc(1, USER_RECORD_SIZE);

	table->HiScore = calloc(1,sizeof(struct HiScore));

	table->NumAccess = 0;
	table->access = NULL;
	if((buffer = GetFile("etc/access"))) {
		memset(field, 0, sizeof(field));
		n = 0;
		for(c = buffer; *c; c++) {
			if(*c == '^' || *c == '\n' || *c == ';') {
				if(*c == '^' || strlen(field))
					n++;
				for(i = strlen(field) - 1; i > 0 && isspace(field[i]); i--)
					field[i] = '\0';
				switch(n) {
					case 1:
						table->access = realloc(table->access, (table->NumAccess + 1) * sizeof(struct access *));
						table->access[table->NumAccess] = calloc(1, sizeof(struct access));
						sprintf(table->access[table->NumAccess]->Name, "%.15s", field);
						break;
					case 2:
						table->access[table->NumAccess]->Calls = atoi(field);
						break;
					case 3:
						table->access[table->NumAccess]->Minutes = atoi(field);
						break;
					case 4:
						table->access[table->NumAccess]->Promote = atoi(field);
						break;
					case 5:
						table->access[table->NumAccess]->Verify = toupper(field[0]);
						break;
					case 6:
						table->access[table->NumAccess]->RolePlay = toupper(field[0]);
						break;
					case 7:
						table->access[table->NumAccess]->Weekend = toupper(field[0]);
						break;
					case 8:
						table->access[table->NumAccess]->Sysop = toupper(field[0]);
						break;
				}
				memset(field, 0, sizeof(field));
				if(n > 0 && *c != '^') {
					table->NumAccess++;
					n = 0;
				}
				if(*c == ';')
					while(*c != '\n' && *c != '\0')
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)] = *c;
		}
		free(buffer);
		fclose(fp);
	}

	table->NumArmor = 0;
	table->armor = NULL;
	if((buffer = GetFile("etc/armor"))) {
		memset(field, 0, sizeof(field));
		n = 0;
		for(c = buffer; *c; c++) {
			if(*c == '^' || *c == '\n' || *c == ';') {
				if(*c == '^' || strlen(field))
					n++;
				for(i = strlen(field) - 1; i > 0 && isspace(field[i]); i--)
					field[i] = '\0';
				switch(n) {
					case 1:
						if(field[0] == '*') {
							if(table->armor)
								table->NumArmor++;
							table->armor = realloc(table->armor, (table->NumArmor + 1) * sizeof(struct armor *));
							table->armor[table->NumArmor] = calloc(1, sizeof(struct armor));
							sprintf(table->armor[table->NumArmor]->Origin, "%.8s", &field[1]);
							n = 0;
							break;
						}
						table->armor[table->NumArmor]->Item = realloc(table->armor[table->NumArmor]->Item, (table->armor[table->NumArmor]->NumArmor + 1) * sizeof(struct armor_item *));
						table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor] = calloc(1, sizeof(struct armor_item));
						sprintf(table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->Name, "%.23s", field);
						table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoMelee = -1;
						table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoMagic = -1;
						break;
					case 2:
						gold(&table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->Value, field);
						break;
					case 3:
						table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->Class = atoi(field);
						break;
					case 4:
						strcpy(table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoClass, field);
						break;
					case 5:
						if(strlen(field))
							table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoMelee = atoi(field);
							break;
					case 6:
						if(strlen(field))
							table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoMagic = atoi(field);
						break;
					case 7:
						strcpy(table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoBearer, field);
						break;
					case 8:
						strcpy(table->armor[table->NumArmor]->Item[table->armor[table->NumArmor]->NumArmor]->EgoCurse, field);
						break;
				}
				memset(field, 0, sizeof(field));
				if(n > 0 && *c != '^') {
					table->armor[table->NumArmor]->NumArmor++;
					n = 0;
				}
				if(*c == ';')
					while(*c != '\n' && *c != '\0')
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)] = *c;
		}
		if(table->armor)
			table->NumArmor++;
		free(buffer);
		fclose(fp);
	}

	table->NumClass = 0;
	table->class = NULL;
	if((buffer = GetFile("etc/class"))) {
		memset(field, 0, sizeof(field));
		n = 0;
		for(c = buffer; *c; c++) {
			if(*c == '^' || *c == '\n' || *c == ';') {
				if(*c == '^' || strlen(field))
					n++;
				for(i = strlen(field) - 1; i > 0 && isspace(field[i]); i--)
					field[i] = '\0';
				switch(n) {
					case 1:
						if(field[0] == '*') {
							if(table->class)
								table->NumClass++;
							table->class = realloc(table->class, (table->NumClass + 1) * sizeof(struct class *));
							table->class[table->NumClass] = calloc(1, sizeof(struct class));
							sprintf(table->class[table->NumClass]->Origin, "%.8s", &field[1]);
							n = 0;
							break;
						}
						table->class[table->NumClass]->Character = realloc(table->class[table->NumClass]->Character, (table->class[table->NumClass]->NumCharacter + 1) * sizeof(struct class_item *));
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter] = calloc(1, sizeof(struct class_character));
						sprintf(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Name, "%.9s", field);
						for(i = 0; i < 2; i++) {
							strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Calls[i].Who, "-- no one yet --");
							table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Calls[i].When = horolog;
							strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Kills[i].Who, "-- no one yet --");
							table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Kills[i].When = horolog;
							strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Killed[i].Who, "-- no one yet --");
							table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Killed[i].When = horolog;
							strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Retreats[i].Who, "-- no one yet --");
							table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Retreats[i].When = horolog;
							strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Brawls[i].Who, "-- no one yet --");
							table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Brawls[i].When = horolog;
							strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Jousts[i].Who, "-- no one yet --");
							table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Jousts[i].When = horolog;
						}
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Calls[0].What = 999;
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Kills[0].What = 999;
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Killed[0].What = 999;
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Retreats[0].What = 999;
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Brawls[1].What = 0;
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Jousts[1].What = 0;
						strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->melee.Who, "-- no one yet --");
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->melee.When = horolog;
						strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->blast.Who, "-- no one yet --");
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->blast.When = horolog;
						strcpy(table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->bigblast.Who, "-- no one yet --");
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->bigblast.When = horolog;
						break;
					case 2:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Melee = atoi(field);
						break;
					case 3:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Backstab = atoi(field);
						break;
					case 4:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Poison = atoi(field);
						break;
					case 5:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Magic = atoi(field);
						break;
					case 6:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->Steal = atoi(field);
						break;
					case 7:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BaseSTR = atoi(field);
						break;
					case 8:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BaseINT = atoi(field);
						break;
					case 9:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BaseDEX = atoi(field);
						break;
					case 10:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BaseCHA = atoi(field);
						break;
					case 11:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BonusSTR = atoi(field);
						break;
					case 12:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BonusINT = atoi(field);
						break;
					case 13:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BonusDEX = atoi(field);
						break;
					case 14:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->BonusCHA = atoi(field);
						break;
					case 15:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->MaxSTR = atoi(field);
						break;
					case 16:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->MaxINT = atoi(field);
						break;
					case 17:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->MaxDEX = atoi(field);
						break;
					case 18:
						table->class[table->NumClass]->Character[table->class[table->NumClass]->NumCharacter]->MaxCHA = atoi(field);
						break;
				}
				memset(field, 0, sizeof(field));
				if(n > 0 && *c != '^') {
					table->class[table->NumClass]->NumCharacter++;
					n = 0;
				}
				if(*c == ';')
					while(*c != '\n' && *c != '\0')
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)] = *c;
		}
		if(table->class)
			table->NumClass++;
		free(buffer);
		fclose(fp);
	}

	table->NumMagic=0;
	table->magic=NULL;
	if((buffer=GetFile("etc/magic"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]='\0';
				switch(n) {
					case 1:
						table->magic=realloc(table->magic,(table->NumMagic+1)*sizeof(struct magic *));
						table->magic[table->NumMagic]=calloc(1,sizeof(struct magic));
						sprintf(table->magic[table->NumMagic]->Name,"%.19s",field);
						break;
					case 2:
						table->magic[table->NumMagic]->Power[0]=atoi(field);
						break;
					case 3:
						table->magic[table->NumMagic]->Power[1]=atoi(field);
						break;
					case 4:
						gold(&table->magic[table->NumMagic]->Cost[0],field);
						break;
					case 5:
						gold(&table->magic[table->NumMagic]->Cost[1],field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					table->NumMagic++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!='\0')
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	table->NumPoison=0;
	table->poison=NULL;
	if((buffer=GetFile("etc/poison"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]='\0';
				switch(n) {
					case 1:
						table->poison=realloc(table->poison,(table->NumPoison+1)*sizeof(struct poison *));
						table->poison[table->NumPoison]=calloc(1,sizeof(struct poison));
						sprintf(table->poison[table->NumPoison]->Name,"%.19s",field);
						break;
					case 2:
						gold(&table->poison[table->NumPoison]->Cost[0],field);
						break;
					case 3:
						gold(&table->poison[table->NumPoison]->Cost[1],field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					table->NumPoison++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!='\0')
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

  table->NumRealEstate=0;
  table->realestate=NULL;
  if((buffer=GetFile("etc/real estate"))) {
    memset(field,0,sizeof(field));
    n=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  table->realestate=realloc(table->realestate,(table->NumRealEstate+1)*sizeof(struct realestate *));
	  table->realestate[table->NumRealEstate]=calloc(1,sizeof(struct realestate));
	  sprintf(table->realestate[table->NumRealEstate]->Name,"%.19s",field);
	  break;
	case 2:
	  gold(&table->realestate[table->NumRealEstate]->Value,field);
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^') {
	  table->NumRealEstate++;
	  n=0;
	}
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }

  table->NumSecurity=0;
  table->security=NULL;
  if((buffer=GetFile("etc/security"))) {
    memset(field,0,sizeof(field));
    n=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
					case 1:
					  table->security=realloc(table->security,(table->NumSecurity+1)*sizeof(struct security *));
					  table->security[table->NumSecurity]=calloc(1,sizeof(struct security));
					  sprintf(table->security[table->NumSecurity]->Name,"%.19s",field);
					  break;
	case 2:
	  gold(&table->security[table->NumSecurity]->Value,field);
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^') {
	  table->NumSecurity++;
	  n=0;
	}
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }

  table->NumWeapon=0;
  table->weapon=NULL;
  if((buffer=GetFile("etc/weapon"))) {
    memset(field,0,sizeof(field));
    n=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  if(field[0]=='*') {
	    if(table->weapon)
	      table->NumWeapon++;
	    table->weapon=realloc(table->weapon,(table->NumWeapon+1)*sizeof(struct weapon *));
	    table->weapon[table->NumWeapon]=calloc(1,sizeof(struct weapon));
	    sprintf(table->weapon[table->NumWeapon]->Origin,"%.8s",&field[1]);
	    n=0;
	    break;
	  }
	  table->weapon[table->NumWeapon]->Item=realloc(table->weapon[table->NumWeapon]->Item,(table->weapon[table->NumWeapon]->NumWeapon+1)*sizeof(struct weapon_item *));
	  table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]=calloc(1,sizeof(struct weapon_item));
	  sprintf(table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->Name,"%.23s",field);
	  table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoMelee=-1;
	  table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoMagic=-1;
	  break;
	case 2:
	  gold(&table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->Value,field);
	  break;
	case 3:
	  table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->Class=atoi(field);
	  break;
	case 4:
	  table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->Action = malloc(strlen(field)+1);
	  strcpy(table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->Action, field);
	  break;
	case 5:
	  strcpy(table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoClass,field);
	  break;
	case 6:
	  if(strlen(field))
	    table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoMelee=atoi(field);
	  break;
	case 7:
	  if(strlen(field))
	    table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoMagic=atoi(field);
	  break;
	case 8:
	  strcpy(table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoBearer,field);
	  break;
	case 9:
	  strcpy(table->weapon[table->NumWeapon]->Item[table->weapon[table->NumWeapon]->NumWeapon]->EgoCurse,field);
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^') {
	  table->weapon[table->NumWeapon]->NumWeapon++;
	  n=0;
	}
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    if(table->weapon)
      table->NumWeapon++;
    free(buffer);
    fclose(fp);
  }

  table->NumArena=0;
  table->arena=NULL;
  if((buffer=GetFile("etc/arena"))) {
    memset(field,0,sizeof(field));
    n=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  table->arena=realloc(table->arena,(table->NumArena+1)*sizeof(struct arena *));
	  table->arena[table->NumArena]=calloc(1,sizeof(struct arena));
	  sprintf(table->arena[table->NumArena]->Name,"%.19s",field);
	  break;
	case 2:
	  sprintf(table->arena[table->NumArena]->Class,"%.18s",field);
	  break;
	case 3:
	  table->arena[table->NumArena]->Level=atoi(field);
	  break;
	case 4:
	  sprintf(table->arena[table->NumArena]->Weapon,"%.33s",field);
	  break;
	case 5:
	  sprintf(table->arena[table->NumArena]->Armor,"%.33s",field);
	  break;
	case 6:
	  gold(&table->arena[table->NumArena]->Gold,field);
	  break;
	case 7:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=HEAL_SPELL;
	  break;
	case 8:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=TELEPORT_SPELL;
	  break;
	case 9:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=BLAST_SPELL;
	  break;
	case 10:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=CONFUSION_SPELL;
	  break;
	case 11:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=TRANSMUTE_SPELL;
	  break;
	case 12:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=CURE_SPELL;
	  break;
	case 13:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=ILLUSION_SPELL;
	  break;
	case 14:
	  if(field[0]=='Y')
	    table->arena[table->NumArena]->Spell|=DISINTEGRATE_SPELL;
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^') {
	  table->NumArena++;
	  n=0;
	}
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }

  table->NumDungeon=0;
  table->dungeon=NULL;
  if((buffer=GetFile("etc/dungeon"))) {
    memset(field,0,sizeof(field));
    n=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  table->dungeon=realloc(table->dungeon,(table->NumDungeon+1)*sizeof(struct dungeon *));
	  table->dungeon[table->NumDungeon]=calloc(1,sizeof(struct dungeon));
	  sprintf(table->dungeon[table->NumDungeon]->Name,"%.19s",field);
	  break;
	case 2:
	  sprintf(table->dungeon[table->NumDungeon]->Class,"%.18s",field);
	  break;
	case 3:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->Spell|=HEAL_SPELL;
	  break;
	case 4:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->Spell|=TELEPORT_SPELL;
	  break;
	case 5:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->Spell|=BLAST_SPELL;
	  break;
	case 6:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->XSpell|=ARMOR_RUSTING_XSPELL;
	  break;
	case 7:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->XSpell|=WEAPON_DECAY_XSPELL;
	  break;
	case 8:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->XSpell|=BIG_BLAST_XSPELL;
	  break;
	case 9:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->XSpell|=MANA_STEALING_XSPELL;
	  break;
	case 10:
	  if(field[0]=='Y')
	    table->dungeon[table->NumDungeon]->XSpell|=LIFE_STEALING_XSPELL;
	  break;
	case 11:
	  if(field[0] == 'Y')
	    table->dungeon[table->NumDungeon]->XSpell |= SUPER_SHIELD_XSPELL;
	  break;
	case 12:
	  if(field[0] == 'Y')
	    table->dungeon[table->NumDungeon]->XSpell |= SUPER_HONE_XSPELL;
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^') {
	  table->NumDungeon++;
	  n=0;
	}
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }

  table->NumNaval=0;
  table->naval=NULL;
  if((buffer=GetFile("etc/naval"))) {
    memset(field,0,sizeof(field));
    n=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  table->naval=realloc(table->naval,(table->NumNaval+1)*sizeof(struct naval *));
	  table->naval[table->NumNaval]=calloc(1,sizeof(struct naval));
	  sprintf(table->naval[table->NumNaval]->Name,"%.19s",field);
	  break;
	case 2:
	  table->naval[table->NumNaval]->INT=atoi(field);
	  break;
	case 3:
	  table->naval[table->NumNaval]->Hull=atoi(field);
	  break;
	case 4:
	  table->naval[table->NumNaval]->Shot=atoi(field);
	  break;
	case 5:
	  table->naval[table->NumNaval]->Power=atoi(field);
	  break;
	case 6:
	  table->naval[table->NumNaval]->Ram=toupper(field[0]);
	  break;
	case 7:
	  gold(&table->naval[table->NumNaval]->Value,field);
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^') {
	  table->NumNaval++;
	  n=0;
	}
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }

  if((buffer=GetFile("etc/hof"))) {
    memset(field,0,sizeof(field));
    n=0;o=0;t=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  OriginName(field,result[0],result[1]);
	  for(o=table->NumClass-1; o>0 && strcasecmp(result[0],table->class[o]->Origin); o--);
	  for(t=table->class[o]->NumCharacter-1; t>0 && strcasecmp(result[1],table->class[o]->Character[t]->Name); t--);
	  break;
	case 2:
	  strcpy(table->class[o]->Character[t]->Calls[0].Who,field);
	  break;
	case 3:
	  table->class[o]->Character[t]->Calls[0].What=atoi(field);
	  break;
	case 4:
	  julian(&table->class[o]->Character[t]->Calls[0].When,field);
	  break;
	case 5:
	  strcpy(table->class[o]->Character[t]->Kills[0].Who,field);
	  break;
	case 6:
	  table->class[o]->Character[t]->Kills[0].What=atoi(field);
	  break;
	case 7:
	  julian(&table->class[o]->Character[t]->Kills[0].When,field);
	  break;
	case 8:
	  strcpy(table->class[o]->Character[t]->Killed[0].Who,field);
	  break;
	case 9:
	  table->class[o]->Character[t]->Killed[0].What=atoi(field);
	  break;
	case 10:
	  julian(&table->class[o]->Character[t]->Killed[0].When,field);
	  break;
	case 11:
	  strcpy(table->class[o]->Character[t]->Retreats[0].Who,field);
	  break;
	case 12:
	  table->class[o]->Character[t]->Retreats[0].What=atoi(field);
	  break;
	case 13:
	  julian(&table->class[o]->Character[t]->Retreats[0].When,field);
	  break;
	case 14:
	  strcpy(table->class[o]->Character[t]->Brawls[0].Who,field);
	  break;
	case 15:
	  table->class[o]->Character[t]->Brawls[0].What=atoi(field);
	  break;
	case 16:
	  julian(&table->class[o]->Character[t]->Brawls[0].When,field);
	  break;
	case 17:
	  strcpy(table->class[o]->Character[t]->Jousts[0].Who,field);
	  break;
	case 18:
	  table->class[o]->Character[t]->Jousts[0].What=atoi(field);
	  break;
	case 19:
	  julian(&table->class[o]->Character[t]->Jousts[0].When,field);
	  break;
	case 20:
	  strcpy(table->class[o]->Character[t]->melee.Who,field);
	  break;
	case 21:
	  table->class[o]->Character[t]->melee.What=atoi(field);
	  break;
	case 22:
	  julian(&table->class[o]->Character[t]->melee.When,field);
	  break;
	case 23:
	  strcpy(table->class[o]->Character[t]->blast.Who,field);
	  break;
	case 24:
	  table->class[o]->Character[t]->blast.What=atoi(field);
	  break;
	case 25:
	  julian(&table->class[o]->Character[t]->blast.When,field);
	  break;
	case 26:
	  strcpy(table->class[o]->Character[t]->bigblast.Who,field);
	  break;
	case 27:
	  table->class[o]->Character[t]->bigblast.What=atoi(field);
	  break;
	case 28:
	  julian(&table->class[o]->Character[t]->bigblast.When,field);
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^')
	  n=0;
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }
  else
    PutHOF(table);

  if((buffer=GetFile("etc/hol"))) {
    memset(field,0,sizeof(field));
    n=0;o=0;t=0;
    for(c=buffer; *c; c++) {
      if(*c=='^' || *c=='\n' || *c==';') {
	if(*c=='^' || strlen(field))
	  n++;
	for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
	  field[i]='\0';
	switch(n) {
	case 1:
	  OriginName(field,result[0],result[1]);
	  for(o=table->NumClass-1; o>0 && strcasecmp(result[0],table->class[o]->Origin); o--);
	  for(t=table->class[o]->NumCharacter-1; t>0 && strcasecmp(result[1],table->class[o]->Character[t]->Name); t--);
	  break;
	case 2:
	  strcpy(table->class[o]->Character[t]->Calls[1].Who,field);
	  break;
	case 3:
	  table->class[o]->Character[t]->Calls[1].What=atoi(field);
	  break;
	case 4:
	  julian(&table->class[o]->Character[t]->Calls[1].When,field);
	  break;
	case 5:
	  strcpy(table->class[o]->Character[t]->Kills[1].Who,field);
	  break;
	case 6:
	  table->class[o]->Character[t]->Kills[1].What=atoi(field);
	  break;
	case 7:
	  julian(&table->class[o]->Character[t]->Kills[1].When,field);
	  break;
	case 8:
	  strcpy(table->class[o]->Character[t]->Killed[1].Who,field);
	  break;
	case 9:
	  table->class[o]->Character[t]->Killed[1].What=atoi(field);
	  break;
	case 10:
	  julian(&table->class[o]->Character[t]->Killed[1].When,field);
	  break;
	case 11:
	  strcpy(table->class[o]->Character[t]->Retreats[1].Who,field);
	  break;
	case 12:
	  table->class[o]->Character[t]->Retreats[1].What=atoi(field);
	  break;
	case 13:
	  julian(&table->class[o]->Character[t]->Retreats[1].When,field);
	  break;
	case 14:
	  strcpy(table->class[o]->Character[t]->Brawls[1].Who,field);
	  break;
	case 15:
	  table->class[o]->Character[t]->Brawls[1].What=atoi(field);
	  break;
	case 16:
	  julian(&table->class[o]->Character[t]->Brawls[1].When,field);
	  break;
	case 17:
	  strcpy(table->class[o]->Character[t]->Jousts[1].Who,field);
	  break;
	case 18:
	  table->class[o]->Character[t]->Jousts[1].What=atoi(field);
	  break;
	case 19:
	  julian(&table->class[o]->Character[t]->Jousts[1].When,field);
	  break;
	}
	memset(field,0,sizeof(field));
	if(n>0 && *c!='^')
	  n=0;
	if(*c==';')
	  while(*c!='\n' && *c!='\0')
	    c++;
      }
      else
	if(strlen(field) || !isspace(*c))
	  field[strlen(field)]=*c;
    }
    free(buffer);
    fclose(fp);
  }
  else
    PutHOL(table);
}

void UnloadETC(struct ServerTable *table)
{
	int i,j;

	if(table) {
		if(table->access) {
			for(i=0; i<table->NumAccess; i++)
				free(table->access[i]);
			free(table->access);
			table->access='\0';
		}

		if(table->armor) {
			for(i=0; i<table->NumArmor; i++) {
				for(j=0; j<table->armor[i]->NumArmor; j++)
					free(table->armor[i]->Item[j]);
				free(table->armor[i]->Item);
				free(table->armor[i]);
			}
			free(table->armor);
			table->armor='\0';
		}

		if(table->class) {
			for(i=0; i<table->NumClass; i++) {
				for(j=0; j<table->class[i]->NumCharacter; j++)
					free(table->class[i]->Character[j]);
				free(table->class[i]->Character);
				free(table->class[i]);
			}
			free(table->class);
			table->class='\0';
		}

		if(table->arena) {
			for(i=0; i<table->NumArena; i++)
				free(table->arena[i]);
			free(table->arena);
			table->arena='\0';
		}

		if(table->dungeon) {
			for(i=0; i<table->NumDungeon; i++)
				free(table->dungeon[i]);
			free(table->dungeon);
			table->dungeon='\0';
		}

		if(table->naval) {
			for(i=0; i<table->NumNaval; i++)
				free(table->naval[i]);
			free(table->naval);
			table->naval='\0';
		}

		if(table->poison) {
			for(i=0; i<table->NumPoison; i++)
				free(table->poison[i]);
			free(table->poison);
			table->poison='\0';
		}

		if(table->realestate) {
			for(i=0; i<table->NumRealEstate; i++)
				free(table->realestate[i]);
			free(table->realestate);
			table->realestate='\0';
		}

		if(table->security) {
			for(i=0; i<table->NumSecurity; i++)
				free(table->security[i]);
			free(table->security);
			table->security='\0';
		}

		if(table->weapon) {
			for(i=0; i<table->NumWeapon; i++) {
				for(j=0; j<table->weapon[i]->NumWeapon; j++) {
					free(table->weapon[i]->Item[j]->Action);
					free(table->weapon[i]->Item[j]);
				}
				free(table->weapon[i]->Item);
				free(table->weapon[i]);
			}
			free(table->weapon);
			table->weapon='\0';
		}

		if(table->HiScore) {
			free(table->HiScore);
			table->HiScore='\0';
		}

		if(SYSREC) {
		  	free(SYSREC);
			SYSREC='\0';
		}
		if(NEWREC) {
			free(NEWREC);
			NEWREC='\0';
		}
		if(IRSREC) {
			IRSREC='\0';
			free(IRSREC);
		}
	}
}

void PutCallers(struct callers *callers)
{
	int i;

	if((fp=fopen("etc/callers","w"))) {
		for(i=0; i<5; i++) {
			fprintf(fp,"%s\n",callers->Last[i]);
			fprintf(fp,"%s\n",callers->Reason[i]);
		}
		fclose(fp);
	}
}

void PutHOF(struct ServerTable *table)
{
	int o,t;
	char dated[9];

	if((fp=fopen("etc/hof","w"))) {
		for(o=0; o<table->NumClass; o++)
			for(t=0; t<table->class[o]->NumCharacter; t++) {
				fprintf(fp,"%s.%s",table->class[o]->Origin,table->class[o]->Character[t]->Name);
				jdate(dated,table->class[o]->Character[t]->Calls[0].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Calls[0].Who,table->class[o]->Character[t]->Calls[0].What,dated);
				jdate(dated,table->class[o]->Character[t]->Kills[0].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Kills[0].Who,table->class[o]->Character[t]->Kills[0].What,dated);
				jdate(dated,table->class[o]->Character[t]->Killed[0].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Killed[0].Who,table->class[o]->Character[t]->Killed[0].What,dated);
				jdate(dated,table->class[o]->Character[t]->Retreats[0].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Retreats[0].Who,table->class[o]->Character[t]->Retreats[0].What,dated);
				jdate(dated,table->class[o]->Character[t]->Brawls[0].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Brawls[0].Who,table->class[o]->Character[t]->Brawls[0].What,dated);
				jdate(dated,table->class[o]->Character[t]->Jousts[0].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Jousts[0].Who,table->class[o]->Character[t]->Jousts[0].What,dated);
				jdate(dated,table->class[o]->Character[t]->melee.When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->melee.Who,table->class[o]->Character[t]->melee.What,dated);
				jdate(dated,table->class[o]->Character[t]->blast.When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->blast.Who,table->class[o]->Character[t]->blast.What,dated);
				jdate(dated,table->class[o]->Character[t]->bigblast.When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->bigblast.Who,table->class[o]->Character[t]->bigblast.What,dated);
				fprintf(fp,"\n");
			}
		fclose(fp);
	}
}

void PutHOL(struct ServerTable *table)
{
	int o,t;
	char dated[9];

	if((fp=fopen("etc/hol","w"))) {
		for(o=0; o<table->NumClass; o++)
			for(t=0; t<table->class[o]->NumCharacter; t++) {
				fprintf(fp,"%s.%s",table->class[o]->Origin,table->class[o]->Character[t]->Name);
				jdate(dated,table->class[o]->Character[t]->Calls[1].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Calls[1].Who,table->class[o]->Character[t]->Calls[1].What,dated);
				jdate(dated,table->class[o]->Character[t]->Kills[1].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Kills[1].Who,table->class[o]->Character[t]->Kills[1].What,dated);
				jdate(dated,table->class[o]->Character[t]->Killed[1].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Killed[1].Who,table->class[o]->Character[t]->Killed[1].What,dated);
				jdate(dated,table->class[o]->Character[t]->Retreats[1].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Retreats[1].Who,table->class[o]->Character[t]->Retreats[1].What,dated);
				jdate(dated,table->class[o]->Character[t]->Brawls[1].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Brawls[1].Who,table->class[o]->Character[t]->Brawls[1].What,dated);
				jdate(dated,table->class[o]->Character[t]->Jousts[1].When);
				fprintf(fp,"^%s^%u^%s",table->class[o]->Character[t]->Jousts[1].Who,table->class[o]->Character[t]->Jousts[1].What,dated);
				fprintf(fp,"\n");
			}
		fclose(fp);
	}
}

void ReKeySequence(struct user *rpc)
{
	static UBYTE keys[24][4] = {
		{ 'C','S','G','P' },{ 'C','S','P','G' },{ 'C','G','S','P' },{ 'C','G','P','S' },{ 'C','P','S','G' },{ 'C','P','G','S' },
		{ 'S','C','G','P' },{ 'S','C','P','G' },{ 'S','G','C','P' },{ 'S','G','P','C' },{ 'S','P','C','G' },{ 'S','P','G','C' },
		{ 'G','C','S','P' },{ 'G','C','P','S' },{ 'G','S','C','P' },{ 'G','S','P','C' },{ 'G','P','C','S' },{ 'G','P','S','C' },
		{ 'P','C','S','G' },{ 'P','C','G','S' },{ 'P','S','C','G' },{ 'P','S','G','C' },{ 'P','G','C','S' },{ 'P','G','S','C' }
	};
	memcpy(rpc->KeySequence,&keys[dice(24)-1][0],4);
	memset(rpc->KeyHints,0,sizeof(rpc->KeyHints));
}

void reroll(struct user *rpc)
{
	strcpy(rpc->Status,NEWREC->Status);
	strcpy(rpc->Class,NEWREC->Class);
	rpc->Gender=rpc->Sex;
	rpc->Level=1;
	rpc->ExpLevel=1;
	rpc->Experience=NEWREC->Experience;
	rpc->Gold=NEWREC->Gold;
	rpc->Bank=NEWREC->Bank;
	if(rpc->Novice=='Y')
		rpc->Bank*=10.;
	rpc->Loan=NEWREC->Loan;
	rpc->HP=NEWREC->HP;
	rpc->SP=NEWREC->SP;
	rpc->STR=NEWREC->STR;
	rpc->INT=NEWREC->INT;
	rpc->DEX=NEWREC->DEX;
	rpc->CHA=NEWREC->CHA;
	memset(rpc->Blessed,0,sizeof(rpc->Blessed));
	memset(rpc->Cursed,0,sizeof(rpc->Cursed));
	strcpy(rpc->Weapon,NEWREC->Weapon);
	rpc->WCmod=NEWREC->WCmod;
	strcpy(rpc->Armor,NEWREC->Armor);
	rpc->ACmod=NEWREC->ACmod;
	rpc->RealEstate=NEWREC->RealEstate;
	rpc->Security=NEWREC->Security;
	rpc->Spell=NEWREC->Spell;
	rpc->XSpell=NEWREC->XSpell;
	rpc->Poison=NEWREC->Poison;
	rpc->Hull=NEWREC->Hull;
	rpc->Cannon=NEWREC->Cannon;
	rpc->Ram=NEWREC->Ram;
	rpc->Current.Calls=0;
	rpc->Current.Kills=0;
	rpc->Current.Killed=0;
	rpc->Current.Retreats=0;
	rpc->Current.KOs=0;
	rpc->Current.KOed=0;
	rpc->Current.JoustWin=0;
	rpc->Current.JoustLoss=0;
	rpc->Bounty=0.;
	memset(rpc->BountyWho,0,sizeof(rpc->BountyWho));
	//  added 22-Jan-2005 RH
	rpc->MyMaxSTR = NEWREC->MyMaxSTR;
	rpc->MyMaxINT = NEWREC->MyMaxINT;
	rpc->MyMaxDEX = NEWREC->MyMaxDEX;
	rpc->MyMaxCHA = NEWREC->MyMaxCHA;
	rpc->MyMelee = NEWREC->MyMelee;
	rpc->MyBackstab = NEWREC->MyBackstab;
	rpc->MyPoison = NEWREC->MyPoison;
	rpc->MyMagic = NEWREC->MyMagic;
	rpc->MySteal = NEWREC->MySteal;
}
