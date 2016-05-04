/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * rpgweb.c
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

#define NORMAL OUT("")

struct ServerTable *table;
struct gang gm[3];
struct RPC *RPC[2][5];
struct userlevel userlevel;
struct username username;

DOUBLE d;
JDATE Julian;
BYTE *outbuf, *inbuf, *filename;
BYTE SYSID[5];
BYTE Today[9], Time[7], Inet[24];
BYTE dated[16], timed[8];
UBYTE acclvl, ANSI, chat, myecho, from, local, logoff, paws, rows, source, timedout;
BYTE reason[64], sigtrap[32];
BYTE **line;
UBYTE numline;

typedef struct {
  char *name;
  char *val;
} entry;

long getsize(char *line, char stop)
{
	long x;

	for(x = 0; line[x] && line[x] != stop; x++);

	return(x);
}

long getcsize(char *line, char *stop)
{
	long l, x;

	l = strlen(stop);
	for(x = 0; line[x] && strncmp(&line[x], stop, l); x++);

	return(x);
}

void getword(char *word, char *line, char stop)
{
	int x, y;

	for(x = 0; line[x] && line[x] != stop; x++)
		word[x] = line[x];
	word[x] = '\0';

	if(line[x])
		++x;
	y = 0;
	while((line[y++] = line[x++]));
}

void getcword(char *word, char *line, char *stop)
{
	int l, x, y;

	l = strlen(stop);
	for(x = 0; line[x] && strncmp(&line[x], stop, l); x++)
		word[x] = line[x];
	word[x] = '\0';

	if(line[x])
	  if(line[x+l])
		x+=l;
	y = 0;
	while((line[y++] = line[x++]));
}

void plustospace(char *str)
{
  int x;
  for(x=0; str[x]; x++)
    if(str[x]=='+')
      str[x]=' ';
}

char x2c(char *what)
{
  char digit;

  digit=(what[0]>='A' ? ((what[0] & 0xdf)-'A')+10 : (what[0]-'0'));
  digit*=16;
  digit+=(what[1]>='A' ? ((what[1] & 0xdf)-'A')+10 : (what[1]-'0'));
  return(digit);
}

void unescape_url(char *url)
{
  int x,y;

  for(x=0,y=0; url[y]; ++x,++y)
    if((url[x]=url[y])=='%') {
      url[x]=x2c(&url[y+1]);
      y+=2;
    }
  url[x]='\0';
}

char *bracket(int n)
{
	static char r[128];

	sprintf(r, "%s%s<%s%d%s>", (n<10) ? " " : "", fore(BLACK), fore(WHITE), n, fore(BLACK));
	return(r);
}

void broadcast(char *msg)
{
	char msgbuf[sizeof(struct user)];

	sprintf(msgbuf, "%s[%s%s%s]%s %s %s", fore(CYN), fore(CYAN), PLAYER.ID, fore(CYN), fore(CYAN), PLAYER.Handle, msg);
	RPGserver(SERVER_BROADCAST, (UBYTE *)msgbuf);
}

char *fore(int code)
{
	static int sp = 0;
	static char result[16][32];
	static char *color[16] = {
		"Black", "Darkred", "Green", "Brown", "Darkblue", "Indigo", "Darkcyan", "Silver", "Gray", "Red", "Lime", "Yellow", "Blue", "Purple", "Cyan", "White"
	};

	sp++;
	if(sp > 15)
		sp = 0;
	if(ANSI)
		sprintf(result[sp], "<FONT color=\"%s\">", color[code]);
	else
		strcpy(result[sp], "");

	return(result[sp]);
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

char *money(double n,int a)
{
	static char fnum[256];
	double r,s;
	int h,i;

	h=ANSI;
	ANSI=a;
	modf(n,&r);
	if(n<1e+05)
		sprintf(fnum,"%s%.5g%sc",fore(WHITE),r,fore(RED));
	else
	if(n<1e+09) {
		modf(n/1e+05,&r);
		sprintf(fnum,"%s%.5g%ss",fore(WHITE),r,fore(CYAN));
		modf(n-r*1e+05,&s);
		if(s>0.) {
			i=strlen(fnum);
			sprintf(&fnum[i],"%s,%.5g%sc",fore(WHITE),s,fore(RED));
		}
	}
	else
	if(n<1e+13) {
		modf(n/1e+09,&r);
		sprintf(fnum,"%s%.4g%sg",fore(WHITE),r,fore(YELLOW));
		modf((n-r*1e+09)/1e+05,&s);
		if(s>0.) {
			i=strlen(fnum);
			sprintf(&fnum[i],"%s,%.5g%ss",fore(WHITE),s,fore(CYAN));
		}
	}
	else {
		modf(n/1e+13,&r);
		sprintf(fnum,"%s%.8g%sp",fore(WHITE),r,fore(MAGENTA));
		modf((n-r*1e+13)/1e+09,&s);
		if(s>0.) {
			i=strlen(fnum);
			sprintf(&fnum[i],"%s,%.5g%sg",fore(WHITE),s,fore(YELLOW));
		}
	}
	strcat(fnum,fore(GRY));
	ANSI=h;
	return(fnum);
}

void output(void *str)
{
	if(sigtrap[0]!='?') {
		printf(str);
		fflush(stdout);
	}
}

int type(char *name,int paws)
{
	FILE *fp;

	if(!(fp = fopen(name, "rb")))
		return(FALSE);
	NORMAL;
	while(!feof(fp)) {
		if(fgets(inbuf,INBUF_SIZE,fp)) {
			OUT(inbuf);
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

void LoadRPC(struct RPC *rpc)
{
	char origin[9],name[32];

	OriginName(rpc->user.Class, origin, name);
	for(rpc->class_origin = NUMCLASS - 1; rpc->class_origin > 0 && strcasecmp(origin, table->class[rpc->class_origin]->Origin); rpc->class_origin--);
	for(rpc->class_type = MAXCLASS(rpc->class_origin) - 1; rpc->class_type > 0 && strcasecmp(name, CLASS(rpc)->Name); rpc->class_type--);

	OriginName(rpc->user.Weapon, origin, name);
	for(rpc->weapon_origin = 0; rpc->weapon_origin < NUMWEAPON && strcasecmp(origin, table->weapon[rpc->weapon_origin]->Origin); rpc->weapon_origin++);
	if(rpc->weapon_origin < NUMWEAPON)
		for(rpc->weapon_type = 0; rpc->weapon_type < table->weapon[rpc->weapon_origin]->NumWeapon && strcasecmp(name, WEAPON(rpc)->Name); rpc->weapon_type++);
	else
		rpc->weapon_type = atoi(name);

	OriginName(rpc->user.Armor, origin, name);
	for(rpc->armor_origin = 0; rpc->armor_origin < NUMARMOR && strcasecmp(origin, table->armor[rpc->armor_origin]->Origin); rpc->armor_origin++);
	if(rpc->armor_origin < NUMARMOR)
		for(rpc->armor_type = 0; rpc->armor_type < table->armor[rpc->armor_origin]->NumArmor && strcasecmp(name, ARMOR(rpc)->Name); rpc->armor_type++);
	else
		rpc->armor_type = atoi(name);

	switch(rpc->user.Gender) {
		case 'M':
			rpc->He="He";
			rpc->His="His";
			rpc->he="he";
			rpc->his="his";
			rpc->him="him";
			break;
		case 'F':
			rpc->He="She";
			rpc->His="Her";
			rpc->he="she";
			rpc->his="her";
			rpc->him="her";
			break;
		default:
			rpc->He="It";
			rpc->His="Its";
			rpc->he="it";
			rpc->his="its";
			rpc->him="it";
			break;
	}
	rpc->BP=rpc->user.HP/10;
	rpc->HP=rpc->user.HP;
	rpc->SP=rpc->user.SP;
	rpc->HULL=rpc->user.Hull;
	rpc->STR=rpc->user.STR;
	rpc->INT=rpc->user.INT;
	rpc->DEX=rpc->user.DEX;
	rpc->CHA=rpc->user.CHA;
	rpc->ToAC=0;
	rpc->ToWC=0;
	if(strlen(rpc->user.Blessed)) {
		if((rpc->STR+=10)>100)
			rpc->STR=100;
		if((rpc->INT+=10)>100)
			rpc->INT=100;
		if((rpc->DEX+=10)>100)
			rpc->DEX=100;
		if((rpc->CHA+=10)>100)
			rpc->CHA=100;
	}
	if(strlen(rpc->user.Cursed)) {
		if((BYTE)(rpc->STR-=10)<1)
			rpc->STR=10;
		if((BYTE)(rpc->INT-=10)<1)
			rpc->INT=10;
		if((BYTE)(rpc->DEX-=10)<1)
			rpc->DEX=10;
		if((BYTE)(rpc->CHA-=10)<1)
			rpc->CHA=10;
	}
	rpc->Confused=FALSE;
}

int RPGserver(char command,UBYTE *p)
{
	struct sockaddr_un rpgd;
	pid_t procid;
	int sockfd,l;
	UBYTE flag;

	ANSI=2;

	if((sockfd=socket(PF_UNIX,SOCK_STREAM,0))<0) {
		NL;
		sprintf(outbuf, "%srpgd socket error.", fore(BRED));
		OUT(outbuf);NL;
		NORMAL;NL;
		flag=FALSE;
		logoff=TRUE;
		return(flag);
	}
	strcpy(rpgd.sun_path,"tmp/rpgd");
	rpgd.sun_family=PF_UNIX;
	if(connect(sockfd,(struct sockaddr *)&rpgd,strlen(rpgd.sun_path)+sizeof(rpgd.sun_family))<0) {
		close(sockfd);
		NL;
		sprintf(outbuf, "%s-=:)) rpgd connection refused ((:=-", fore(RED));
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
		if(command == SERVER_GETUSERLEVEL)
			write(sockfd,p,sizeof(struct userlevel));
		else
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
				NL;
				sprintf(outbuf, "%s%s is already engaged elsewhere.", fore(CYAN), p);
				OUT(outbuf); NL;
				NORMAL; NL;
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
	int i, j;

	table = calloc(1, sizeof(struct ServerTable));
	LoadETC(table);
	if(!(outbuf = calloc(1,4 * OUTBUF_SIZE)))
		return(FALSE);
	if(!(inbuf = calloc(1,INBUF_SIZE)))
		return(FALSE);
	if(!(filename = calloc(1,128)))
		return(FALSE);
	for(i = 0; i < 2; i++)
		for(j = 0; j < 5; j++)
			if(!(RPC[i][j] = calloc(1,sizeof(struct RPC))))
				return(FALSE);
	line = calloc(100, sizeof(char *));
	for(i = 0; i < 100; i++)
		line[i] = calloc(1, 128);

	if(!RPGserver(SERVER_CONNECT, NULL))
		return(FALSE);
	strcpy(IRSREC->ID, "_IRS");
	if(!RPGserver(SERVER_GETUSER, (UBYTE *)IRSREC))
		return(FALSE);
	strcpy(NEWREC->ID, "_NEW");
	if(!RPGserver(SERVER_GETUSER, (UBYTE *)NEWREC))
		return(FALSE);
	strcpy(SYSREC->ID, "_SYS");
	if(!RPGserver(SERVER_GETUSER, (UBYTE *)SYSREC))
		return(FALSE);
	strcpy(SYSID, SYSREC->ID);

	ANSI=2;

	return(TRUE);
}

void Shutdown(void)
{
	int i, j;

	if(table) {
		UnloadETC(table);
		free(table);
		table = '\0';
	}

	if(line) {
		for(i = 0; i < 100; i++)
			if(line[i])
				free(line[i]);
		free(line);
		line = '\0';
	}

	for(i = 0; i < 2; i++)
		for(j = 0; j < 5; j++)
			if(RPC[i][j]) {
				free(RPC[i][j]);
				RPC[i][j] = '\0';
			}

	if(filename) {
		free(filename);
		filename = '\0';
	}
	if(inbuf) {
		free(inbuf);
		inbuf = '\0';
	}
	if(outbuf) {
		free(outbuf);
		outbuf = '\0';
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
			s = "error";
			break;
	}
	sprintf(sigtrap, "?%s trap",s);
	logoff = TRUE;
}

void ListofHOF(int o, int t)
{
	OUT("<BODY>"); NL;

	sprintf(outbuf, "<H2>%s<A HREF=\"rpgweb?cmd=F\">Hall of Fame</A>%s</H2>", fore(WHITE), fore(GRY));
	OUT(outbuf); NL;

	if(o < 1 || o > NUMCLASS) {
		for(o = 0; o < NUMCLASS; o++) {
			sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=F&int1=%d\">%s</A>", bracket(o + 1), fore(GRY), o + 1, table->class[o]->Origin);
			OUT(outbuf); NL;
		}
		OUT("</BODY>"); NL;
		return;
	}
	o--;

	if(t < 1 || t > table->class[o]->NumCharacter) {
		sprintf(outbuf, "<H3>%s%s%s</H3>", fore(WHITE), table->class[o]->Origin, fore(GRY));
		OUT(outbuf); NL;
		OUT("<PRE>"); NL;
		for(t = 0; t < table->class[o]->NumCharacter; t++) {
			if(o == 0 && t == 0)
				sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=F&int1=%d&int2=%d\">All-Time</A>", bracket(t + 1), fore(GRY), o + 1, t + 1);
			else
				sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=F&int1=%d&int2=%d\">%s</A>", bracket(t + 1), fore(GRY), o + 1, t + 1, table->class[o]->Character[t]->Name);
			OUT(outbuf); NL;
		}
		OUT("</PRE>"); NL;
		OUT("</BODY>"); NL;
		return;
	}
	t--;
	sprintf(outbuf, "<H3>%s<A HREF=\"rpgweb?cmd=F&int1=%d\">%s</A> - %s%s</H3>", fore(WHITE), o + 1, table->class[o]->Origin, fore(YELLOW), (o == 0 && t == 0) ? "All-Time" : (char *)table->class[o]->Character[t]->Name);
	OUT(outbuf); NL;

	OUT("<PRE>"); NL;

	sprintf(outbuf, "%s     Least Calls      ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Calls[0].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Calls[0].Who)) / 2, "", table->class[o]->Character[t]->Calls[0].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Calls[0].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s     Least Kills      ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Kills[0].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Kills[0].Who)) / 2, "", table->class[o]->Character[t]->Kills[0].Who);
	sprintf(outbuf,"%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Kills[0].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s     Least Killed     ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Killed[0].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Killed[0].Who)) / 2, "", table->class[o]->Character[t]->Killed[0].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Killed[0].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s    Least Retreats    ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Retreats[0].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Retreats[0].Who)) / 2, "", table->class[o]->Character[t]->Retreats[0].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Retreats[0].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s   Most Knock-Outs    ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Brawls[0].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Brawls[0].Who)) / 2, "", table->class[o]->Character[t]->Brawls[0].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Brawls[0].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s   Most Joust Wins    ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Jousts[0].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Jousts[0].Who)) / 2, "", table->class[o]->Character[t]->Jousts[0].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Jousts[0].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}

void ListofHOL(int o, int t)
{
	OUT("<BODY>"); NL;

	sprintf(outbuf, "<H2>%s<A HREF=\"rpgweb?cmd=L\">Hall of Lame</A>%s</H2>", fore(WHITE), fore(GRY));
	OUT(outbuf); NL;

	if(o < 1 || o > NUMCLASS) {
		for(o = 0; o < NUMCLASS; o++) {
			sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=L&int1=%d\">%s</A>", bracket(o + 1), fore(GRY), o + 1, table->class[o]->Origin);
			OUT(outbuf); NL;
		}
		OUT("</BODY>"); NL;
		return;
	}
	o--;

	if(t < 1 || t > table->class[o]->NumCharacter) {
		sprintf(outbuf, "<H3>%s%s%s</H3>", fore(WHITE), table->class[o]->Origin, fore(GRY));
		OUT(outbuf); NL;
		OUT("<PRE>"); NL;
		for(t = 0; t < table->class[o]->NumCharacter; t++) {
			if(o == 0 && t == 0)
				sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=L&int1=%d&int2=%d\">All-Time</A>", bracket(t + 1), fore(GRY), o + 1, t + 1);
			else
				sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=L&int1=%d&int2=%d\">%s</A>", bracket(t + 1), fore(GRY), o + 1, t + 1, table->class[o]->Character[t]->Name);
			OUT(outbuf); NL;
		}
		OUT("</PRE>"); NL;
		OUT("</BODY>"); NL;
		return;
	}
	t--;
	sprintf(outbuf, "<H3>%s<A HREF=\"rpgweb?cmd=L&int1=%d\">%s</A> - %s%s</H3>", fore(WHITE), o + 1, table->class[o]->Origin, fore(YELLOW), (o == 0 && t == 0) ? "All-Time" : (char *)table->class[o]->Character[t]->Name);
	OUT(outbuf); NL;

	OUT("<PRE>"); NL;

	sprintf(outbuf, "%s      Most Calls      ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Calls[1].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Calls[1].Who)) / 2, "", table->class[o]->Character[t]->Calls[1].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Calls[1].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s      Most Kills      ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Kills[1].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Kills[1].Who)) / 2, "", table->class[o]->Character[t]->Kills[1].Who);
	sprintf(outbuf,"%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Kills[1].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s      Most Killed     ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Killed[1].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Killed[1].Who)) / 2, "", table->class[o]->Character[t]->Killed[1].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Killed[1].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s     Most Retreats    ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Retreats[1].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Retreats[1].Who)) / 2, "", table->class[o]->Character[t]->Retreats[1].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Retreats[1].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s   Most Knocked-Out   ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Brawls[1].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Brawls[1].Who)) / 2, "", table->class[o]->Character[t]->Brawls[1].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Brawls[1].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	sprintf(outbuf, "%s  Most Joust Losses   ", fore(CYAN));
	OUT(outbuf);
	fdate(dated, table->class[o]->Character[t]->Jousts[1].When);
	sprintf(line[0], "%s", &dated[4]);
	sprintf(outbuf, "%s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->class[o]->Character[t]->Jousts[1].Who)) / 2, "", table->class[o]->Character[t]->Jousts[1].Who);
	sprintf(outbuf, "%s%-22s", fore(WHITE), line[0]);
	OUT(outbuf);
	sprintf(line[0], "%4u", table->class[o]->Character[t]->Jousts[1].What);
	sprintf(outbuf, "  %s%s", fore(WHITE), line[0]);
	OUT(outbuf); NL; NL;

	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}

void HiScores(int n)
{
	RPGserver(SERVER_GETHISCORE, (UBYTE *)table->HiScore);

	OUT("<PRE>"); NL;
	sprintf(outbuf, "         %s-------------", fore(BLUE));
	OUT(outbuf); NL;
	sprintf(outbuf, "  --=:))%s Hi-Score List %s((:=--", fore(YELLOW), fore(BLUE));
	OUT(outbuf); NL;
	sprintf(outbuf, "         -------------");
	OUT(outbuf); NORMAL; NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[0])) / 2, "", table->HiScore->Who[0]);
	sprintf(outbuf, "%s%s    <A HREF=\"rpgweb?cmd=H&int1=1\">The One to Beat</A>%s    %-22s %9.2f %srating", bracket(1), fore(CYAN), fore(WHITE), line[0], table->HiScore->Rating, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[1])) / 2, "", table->HiScore->Who[1]);
	sprintf(outbuf, "%s%s  <A HREF=\"rpgweb?cmd=H&int1=2\">The Sultan of Slash</A>%s  %-22s %9.9g %shits", bracket(2), fore(CYAN), fore(WHITE), line[0], table->HiScore->HP, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[2])) / 2, "", table->HiScore->Who[2]);
	sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=H&int1=3\">The Admirable Admiral</A>%s %-22s %9.9g %shulls", bracket(3), fore(CYAN), fore(WHITE), line[0], table->HiScore->Hull, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[3])) / 2, "", table->HiScore->Who[3]);
	sprintf(outbuf, "%s%s   <A HREF=\"rpgweb?cmd=H&int1=4\">The Serial Killer</A>%s   %-22s %9.2f %skills", bracket(4), fore(CYAN), fore(WHITE), line[0], table->HiScore->Kills, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[4])) / 2, "", table->HiScore->Who[4]);
	sprintf(outbuf, "%s%s  <A HREF=\"rpgweb?cmd=H&int1=5\">Most Popular Player</A>%s  %-22s %9.2f %scalls/day", bracket(5), fore(CYAN), fore(WHITE), line[0], table->HiScore->Calls, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[5])) / 2, "", table->HiScore->Who[5]);
	sprintf(outbuf, "%s%s  <A HREF=\"rpgweb?cmd=H&int1=6\">The Easiest Target</A>%s   %-22s %9.2f %skilled", bracket(6), fore(CYAN), fore(WHITE), line[0], table->HiScore->Killed, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[6])) / 2, "", table->HiScore->Who[6]);
	sprintf(outbuf, "%s%s   <A HREF=\"rpgweb?cmd=H&int1=7\">The Runaway Train</A>%s   %-22s %9.2f %sretreats", bracket(7), fore(CYAN), fore(WHITE), line[0], table->HiScore->Retreats, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[7])) / 2, "", table->HiScore->Who[7]);
	sprintf(outbuf, "%s%s  <A HREF=\"rpgweb?cmd=H&int1=8\">The Knock-out King</A>%s   %-22s %9.2f %sbrawls", bracket(8), fore(CYAN), fore(WHITE), line[0], table->HiScore->KO, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[8])) / 2, "", table->HiScore->Who[8]);
	sprintf(outbuf, "%s%s  <A HREF=\"rpgweb?cmd=H&int1=9\">The Joust Champion</A>%s   %-22s %9.2f %sjousts", bracket(9), fore(CYAN), fore(WHITE), line[0], table->HiScore->Joust, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[9])) / 2, "", table->HiScore->Who[9]);
	sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=H&int1=10\">The Prince of Thieves</A>%s %-22s %9.2f %ssteals", bracket(10), fore(CYAN), fore(WHITE), line[0], table->HiScore->Stole, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[10])) / 2, "", table->HiScore->Who[10]);
	sprintf(outbuf, "%s%s  <A HREF=\"rpgweb?cmd=H&int1=11\">Ace of High Rollers</A>%s  %-22s %9.2f %sgambles", bracket(11), fore(CYAN), fore(WHITE), line[0], table->HiScore->Gamble, fore(CYAN));
	OUT(outbuf); NL;
	sprintf(line[0], "%*s%s", (int)(22 - strlen(table->HiScore->Who[11])) / 2, "", table->HiScore->Who[11]);
	sprintf(outbuf, "%s%s <A HREF=\"rpgweb?cmd=H&int1=12\">I'd Rather Be Fishing</A>%s %-22s %9.2f %sfishes", bracket(12), fore(CYAN), fore(WHITE), line[0], table->HiScore->Fish, fore(CYAN));
	OUT(outbuf); NL; NL;

	if(n > 0 && n < 13) {
		double d, topv[21];
		int i, j, t;
		char topid[21][5];

		t = 0;
		memset(&RPC[1][1]->user, 0, sizeof(struct user));
		for(i = 0; i < 8192 && RPGserver(SERVER_NEXTUSER, (UBYTE *)&RPC[1][1]->user); i++) {
			if(RPC[1][1]->user.ID[0] == '_' || RPC[1][1]->user.History.Calls < 10)
				continue;
			switch(n) {
				case 1:
					topv[t] = RPC[1][1]->user.Rating;
					break;
				case 2:
					topv[t] = RPC[1][1]->user.History.HP;
					break;
				case 3:
					topv[t] = RPC[1][1]->user.History.Hull;
					break;
				case 4:
					topv[t] = (double)RPC[1][1]->user.History.Kills / RPC[1][1]->user.History.Calls;
					break;
				case 5:
					topv[t] = (double)RPC[1][1]->user.History.Calls / (Julian - RPC[1][1]->user.CreateDate + 1);
					break;
				case 6:
					topv[t] = (double)RPC[1][1]->user.History.Killed / RPC[1][1]->user.History.Calls;
					break;
				case 7:
					topv[t] = (double)RPC[1][1]->user.History.Retreats / RPC[1][1]->user.History.Calls;
					break;
				case 8:
					topv[t] = (double)RPC[1][1]->user.History.KO / RPC[1][1]->user.History.Calls;
					break;
				case 9:
					topv[t] = (double)RPC[1][1]->user.History.Joust / RPC[1][1]->user.History.Calls;
					break;
				case 10:
					topv[t] = (double)RPC[1][1]->user.History.Stole / RPC[1][1]->user.History.Calls;
					break;
				case 11:
					topv[t] = (double)RPC[1][1]->user.History.Gamble / RPC[1][1]->user.History.Calls;
					break;
				case 12:
					topv[t] = (double)RPC[1][1]->user.History.Fish / RPC[1][1]->user.History.Calls;
					break;
			}
			if(topv[t] < .005)
				continue;
			strcpy(topid[t], RPC[1][1]->user.ID);
			j = t;
			if(t < 20)
				t++;
			while(j > 0 && topv[j] > topv[j - 1]) {
				strcpy(outbuf, topid[j - 1]);
				strcpy(topid[j - 1], topid[j]);
				strcpy(topid[j], outbuf);
				d = topv[j - 1];
				topv[j - 1] = topv[j];
				topv[j] = d;
				j--;
			}
		}
		sprintf(outbuf, "<H3>%sTop 20 %s</H3>", fore(WHITE),
				n == 1 ? "Rated" :
				n == 2 ? "Hitters" :
				n == 3 ? "Gunners" :
				n == 4 ? "Killers" :
				n == 5 ? "Callers" :
				n == 6 ? "Targets" :
				n == 7 ? "Cowards" :
				n == 8 ? "Fists" :
				n == 9 ? "Jousters" :
				n == 10 ? "Thieves" :
				n == 11 ? "Gamblers" :
				n == 12 ? "Fishers" :
				"?");
		OUT(outbuf); NORMAL; NL; NL;
		for(i = 0; i < t; i++) {
			strcpy(RPC[1][1]->user.ID, topid[i]);
			RPGserver(SERVER_GETUSER, (UBYTE *)&RPC[1][1]->user);
			sprintf(outbuf, "%s%2d.%s  %-22s  ", fore(CYN), i + 1, fore(WHITE), RPC[1][1]->user.Handle);
			OUT(outbuf);
			sprintf(outbuf, "%s", fore(i == 0 ? YELLOW : i == 1 ? CYAN : i == 2 ? BRED : GRY));
			OUT(outbuf);
			if(n == 2 || n == 3)
				sprintf(outbuf, "%9.9g", topv[i]);
			else
				sprintf(outbuf, "%9.3f", topv[i]);
			OUT(outbuf); NL;
		}
	}

	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}


void MostWanted(void)
{
	int i, immortal, max;

	RPGserver(SERVER_GETHISCORE, (UBYTE *)table->HiScore);

	sprintf(outbuf, "<H2>%sMost Wanted List%s</H2>", fore(CYAN), fore(WHITE));
	OUT(outbuf); NL;
	OUT("<PRE>"); NL;

	for(immortal = table->NumClass - 1; immortal > 0 && strcasecmp("IMMORTAL", table->class[immortal]->Origin); immortal--);
	sprintf(outbuf, " ID   Player's Handle           Class    Lvl  Status  Party               ");
	OUT(outbuf); NL;
	sprintf(outbuf, "--------------------------------------------------------------------------");
	OUT(outbuf); NL;
	strcpy(userlevel.Current, "999");
	i = 2;
	while(RPGserver(SERVER_GETUSERLEVEL, (UBYTE *)&userlevel) && i < 100) {
		i++;
		strcpy(ENEMY.ID, userlevel.ID);
		RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY);
		LoadRPC(RPC[1][0]);
		OriginName(RPC[1][0]->user.Class, line[0], line[1]);
		max = (strcasecmp(table->class[RPC[1][0]->class_origin]->Origin, "IMMORTAL")) ? FALSE : RPC[1][0]->class_type + 1;
		sprintf(outbuf, "%s<A HREF=\"rpgweb?cmd=Y&player=%s\">%-4.4s</A>  %-22.22s  %-9.9s  %3u  %s%-6.6s  %s%.20s"
		, fore(max == table->class[RPC[1][0]->class_origin]->NumCharacter
		    ? YELLOW
			: strncmp(ENEMY.Handle,table->HiScore->Who[0],sizeof(ENEMY.Handle))
			  ? GRY
			  : MAGENTA
			)
		, ENEMY.ID, ENEMY.ID, ENEMY.Handle, line[1], ENEMY.Level
		, fore(strcmp(ENEMY.Status, "jail") ? (strlen(ENEMY.Status) ? BLACK : WHITE) : GRY), strcmp(ENEMY.Status, "jail") ? (strlen(ENEMY.Status) ? " Dead " : "Alive!") : " Jail "
		, fore(GRY), strlen(ENEMY.Gang) ? (char *)ENEMY.Gang : "None");
		OUT(outbuf); NL;
	}
	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}

void Newspapers(void)
{
	OUT("<HEAD>"); NL;
	OUT("<TITLE>RPGD: Tiny's Tavern Newspapers</TITLE>"); NL;
	OUT("</HEAD>"); NL;
	OUT("<BODY bgcolor=\"Black\" text=\"Silver\" link=\"White\" vlink=\"Silver\">"); NL;

	sprintf(outbuf, "<H2>%sToday's Newspaper%s</H2>", fore(YELLOW), fore(WHITE));
	OUT(outbuf); NL;
	OUT("<PRE>"); NL;
	type("msg/Today", FALSE);
	OUT("</PRE>"); NL;

	sprintf(outbuf, "<H2>%sYesterday's Newspaper%s</H2>", fore(CYAN), fore(WHITE));
	OUT(outbuf); NL;
	OUT("<PRE>"); NL;
	type("msg/Yesterday", FALSE);
	OUT("</PRE>"); NL;

	OUT("</BODY>"); NL;
}

void PastWinners(void)
{
	OUT("<BODY bgcolor=\"Black\" text=\"Silver\" link=\"White\" vlink=\"Silver\">"); NL;

	sprintf(outbuf, "<H2>%sPast Winners List%s</H2>", fore(GREEN), fore(WHITE));
	OUT(outbuf); NL;
	OUT("<PRE>"); NL;
	type("msg/Winners", FALSE);
	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}

void YourStats(void)
{
	static char *magic[] = { "Wand", "Scroll", "Spell", "Magic" };
	struct RPC *rpc;
	int bg = CYN, i;
	char *p;

	OUT("<BODY bgcolor=\"Black\" text=\"Silver\" link=\"White\" vlink=\"Silver\">"); NL;
	OUT("<PRE>"); NL;

	rpc = RPC[1][0];
	strcpy(ENEMY.ID, PLAYER.ID);
	if(RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY))
		LoadRPC(rpc);
	else {
		OUT("Try again later..."); NL;
		OUT("</PRE>"); NL;
		OUT("</BODY>"); NL;
		return;
	}

	memset(line[52],'-',52);
	sprintf(line[1],"%s",rpc->user.Handle);
	i=20-strlen(line[1]);
	OUT(fore(BLU));
	sprintf(outbuf,"+%.*s=:))",11+i/2,line[52]);
	OUT(outbuf);
	OUT(fore(YELLOW));
	sprintf(outbuf," %s ",line[1]);
	OUT(outbuf);
	OUT(fore(BLU));
	sprintf(outbuf,"((:=%.*s+",11+i/2+i%2,line[52]);
	OUT(outbuf);NL;

	fdate(line[0],rpc->user.BirthDate);
	sprintf(outbuf,"%s|    %sTitle:%s %-18s %sBorn:%s %s %s|",fore(BLU),fore(bg),fore(WHITE),ACCESS(rpc->user.Access)->Name,fore(bg),fore(WHITE),line[0],fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%s (%c)",CLASS(rpc)->Name,rpc->user.Gender);
	sprintf(line[1],"%.8g",rpc->user.Experience);
	sprintf(outbuf,"%s|    %sClass:%s %-19s %sExp:%s %-15s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(bg),fore(WHITE),line[1],fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%u.%02u",rpc->user.Immortal,rpc->user.Level);
	sprintf(line[1],"%.8g",EXP(rpc->user.Level));
	sprintf(outbuf,"%s| %sImmortal:%s %-18s %sNeed:%s %-15s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(bg),fore(WHITE),line[1],fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%3u (%u,%u)",rpc->STR,rpc->user.STR,rpc->user.MyMaxSTR);
	sprintf(line[1],"%s",money(rpc->user.Gold,FALSE));
	sprintf(outbuf, "%s|      %sStr:%s%-19s %sHand:%s %s%*s %s|", fore(BLU), fore(bg), fore(WHITE), line[0], fore(bg), fore(WHITE), money(rpc->user.Gold, ANSI), (int)(15 - strlen(line[1])), "", fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%3u (%u,%u)",rpc->INT,rpc->user.INT,rpc->user.MyMaxINT);
	sprintf(line[1],"%s",money(rpc->user.Bank,FALSE));
	sprintf(outbuf,"%s|      %sInt:%s%-19s %sBank:%s %s%*s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(bg),fore(WHITE),money(rpc->user.Bank,ANSI), (int)(15 - strlen(line[1])),"",fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%3u (%u,%u)",rpc->DEX,rpc->user.DEX,rpc->user.MyMaxDEX);
	sprintf(line[1],"%s",money(rpc->user.Loan,FALSE));
	sprintf(outbuf,"%s|      %sDex:%s%-19s %sLoan:%s %s%*s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(bg),fore(WHITE),money(rpc->user.Loan,ANSI), (int)(15 - strlen(line[1])),"",fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%3u (%u,%u)",rpc->CHA,rpc->user.CHA,rpc->user.MyMaxCHA);
	sprintf(outbuf,"%s|      %sCha:%s%-40s  %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0], "%u/%u  (%s)", rpc->HP, rpc->user.HP, rpc->user.MyMelee == 0 ? "weak" : rpc->user.MyMelee == 1 ? "normal" : rpc->user.MyMelee == 2 ? "advanced" : rpc->user.MyMelee == 3 ? "warrior" : rpc->user.MyMelee == 4 ? "brutal" : "divine");
	sprintf(outbuf,"%s|       %sHP:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(BLU));
	OUT(outbuf);NL;

	if(rpc->user.SP) {
		sprintf(line[0], "%u/%u  (%s)", rpc->SP, rpc->user.SP, rpc->user.MyMagic == 2 ? "wizardry" : rpc->user.MyMagic == 3 ? "arcane" : "divine");
		sprintf(outbuf,"%s|       %sSP:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(BLU));
		OUT(outbuf);NL;
	}

	sprintf(line[0],"%s (%+d,%+d)",WEAPON(rpc)->Name,rpc->user.WCmod,rpc->ToWC);
	sprintf(outbuf,"%s|   %sWeapon:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(BLU));
	OUT(outbuf);NL;

	sprintf(line[0],"%s (%+d,%+d)",ARMOR(rpc)->Name,rpc->user.ACmod,rpc->ToAC);
	sprintf(outbuf,"%s|    %sArmor:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(BLU));
	OUT(outbuf);NL;

	if(rpc->user.Poison) {
		strcpy(line[0],"");
		for(i=0; i<NUMPOISON; i++)
			if(rpc->user.Poison & (UWORD)pow(2.,(double)i)) {
				sprintf(line[1],"%s%d",strlen(line[0]) ? "," : "",i+1);
				strcat(line[0],line[1]);
			}
		sprintf(outbuf,"%s|  %sPoisons:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(BLU));
		OUT(outbuf);NL;
	}
	if(rpc->user.Spell) {
		strcpy(line[0],"");
		for(i=0; i<(NUMMAGIC<16 ? NUMMAGIC : 16); i++)
			if(rpc->user.Spell & (UWORD)pow(2.,(double)i)) {
				sprintf(line[1],"%s%.*s",strlen(line[0]) ? "," : "",strlen(MAGIC(i)->Name)<7 ? (int)strlen(MAGIC(i)->Name) : 3,MAGIC(i)->Name);
				strcat(line[0],line[1]);
			}
		strcpy(line[1],"");
		if(strlen(line[0])>40) {
			for(p=line[0],i=40; i>0 && p[i]!=','; i--);
			p[i]='\0';
			strcpy(line[1],&p[i+1]);
		}
		sprintf(outbuf,"%s| %s%7ss:%s %-40s %s|",fore(BLU),fore(bg),magic[rpc->user.MyMagic-1],fore(WHITE),line[0],fore(BLU));
		OUT(outbuf);NL;

		if(strlen(line[1])) {
			sprintf(outbuf,"%s|           %s%-40s %s|",fore(BLU),fore(WHITE),line[1],fore(BLU));
			OUT(outbuf);NL;
		}
	}

	if(rpc->user.XSpell) {
		strcpy(line[0],"");
		for(i=0; i<NUMMAGIC-16; i++)
			if(rpc->user.XSpell & (UWORD)pow(2.,(double)i)) {
				sprintf(line[1],"%s%s",strlen(line[0]) ? "," : "",MAGIC(i+16)->Name);
				strcat(line[0],line[1]);
			}
		strcpy(line[1],"");
		if(strlen(line[0])>40) {
			for(p=line[0],i=40; i>0 && p[i]!=','; i--);
			p[i]='\0';
			strcpy(line[1],&p[i+1]);
		}
		sprintf(outbuf,"%s| %s%7ss:%s %-40s %s|",fore(BLU),fore(bg),magic[rpc->user.MyMagic-1],fore(WHITE),line[0],fore(BLU));
		OUT(outbuf);NL;

		if(strlen(line[1])) {
			sprintf(outbuf,"%s|           %s%-40s %s|",fore(BLU),fore(WHITE),line[1],fore(BLU));
			OUT(outbuf);NL;
		}
	}

	sprintf(outbuf,"%s| %sDwelling:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),REALESTATE(rpc->user.RealEstate)->Name,fore(BLU));
	OUT(outbuf);NL;

	sprintf(outbuf,"%s| %sSecurity:%s %-40s %s|",fore(BLU),fore(bg),fore(WHITE),SECURITY(rpc->user.Security)->Name,fore(BLU));
	OUT(outbuf);NL;

	if(strlen(rpc->user.Gang)) {
		strcpy(gm[0].Name,rpc->user.Gang);
		if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
			sprintf(line[0],"%.20s",gm[0].Name);
			memset(line[1],0,16);
			for(i=0; i<4; i++)
				if(strcmp(gm[0].MemberID[i],rpc->user.ID)) {
					strcpy(RPC[1][1]->user.ID,gm[0].MemberID[i]);
					RPGserver(SERVER_GETUSER,(UBYTE *)&RPC[1][1]->user);
					if(strlen(gm[0].MemberID[i])) {
						if(strncmp(rpc->user.Gang,RPC[1][1]->user.Gang,20))
							strlwr(gm[0].MemberID[i]);
					}
					else
						strcpy(gm[0].MemberID[i],"*");
					sprintf(outbuf,"%c%s",(strlen(line[1]) ? ',' : ' '),gm[0].MemberID[i]);
					strcat(line[1],outbuf);
				}
			sprintf(outbuf,"%s|    %sParty:%s %-20s %swith%s%-15s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(bg),fore(WHITE),line[1],fore(BLU));
			OUT(outbuf);NL;
		}
	}

	if(rpc->user.Hull) {
		sprintf(line[0],"%d:%d",rpc->HULL,rpc->user.Hull);
		sprintf(line[1],"%d:%d  %s",rpc->user.Cannon,rpc->user.Hull/50,rpc->user.Ram=='Y' ? "(RAM)" : "");
		sprintf(outbuf,"%s|  %sWarship:%s %-9s        %sCannon:%s %-15s %s|",fore(BLU),fore(bg),fore(WHITE),line[0],fore(bg),fore(WHITE),line[1],fore(BLU));
		OUT(outbuf);NL;
	}

	if(strlen(rpc->user.Blessed)) {
		sprintf(outbuf,"%s|     %sNote:%s You were blessed by %-4s                 %s|",fore(BLU),fore(bg),fore(WHITE),rpc->user.Blessed,fore(BLU));
		OUT(outbuf);NL;
	}

	if(strlen(rpc->user.Cursed)) {
		sprintf(outbuf,"%s|     %sNote:%s You were cursed by %-4s                  %s|",fore(BLU),fore(bg),fore(WHITE),rpc->user.Cursed,fore(BLU));
		OUT(outbuf);NL;
	}
	sprintf(outbuf,"%s+%.52s+",fore(BLU),line[52]);
	OUT(outbuf);NL;

  sprintf(line[0], "%*s%s", (int)(9 - strlen(table->class[rpc->class_origin]->Character[rpc->class_type]->Name)) / 2, "", table->class[rpc->class_origin]->Character[rpc->class_type]->Name);
  sprintf(outbuf,"%s| %sCurrent Stats  %-9s  All-Time  History %s|",fore(BLU),fore(YELLOW),line[0],fore(BLU));
  OUT(outbuf);NL;

  sprintf(outbuf,"%s|    %sCalls:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s|",fore(BLU),fore(bg),fore(WHITE),
	  rpc->user.Current.Calls,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Calls[0].What,table->class[rpc->class_origin]->Character[rpc->class_type]->Calls[1].What,
	  table->class[0]->Character[0]->Calls[0].What,table->class[0]->Character[0]->Calls[1].What,
	  rpc->user.History.Calls,
	  fore(BLU));

  OUT(outbuf);NL;
  sprintf(outbuf,"%s|    %sKills:%s %3u   %3u/%-4u  %3u/%-4u  %6lu  %s|",fore(BLU),fore(bg),fore(WHITE),
	  rpc->user.Current.Kills,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Kills[0].What,table->class[rpc->class_origin]->Character[rpc->class_type]->Kills[1].What,
	  table->class[0]->Character[0]->Kills[0].What,table->class[0]->Character[0]->Kills[1].What,
	  rpc->user.History.Kills,
	  fore(BLU));
  OUT(outbuf);NL;

  sprintf(outbuf,"%s|   %sKilled:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s|",fore(BLU),fore(bg),fore(WHITE),
	  rpc->user.Current.Killed,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Killed[0].What,table->class[rpc->class_origin]->Character[rpc->class_type]->Killed[1].What,
	  table->class[0]->Character[0]->Killed[0].What,table->class[0]->Character[0]->Killed[1].What,
	  rpc->user.History.Killed,
	  fore(BLU));
  OUT(outbuf);NL;

  sprintf(outbuf,"%s| %sRetreats:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s|",fore(BLU),fore(bg),fore(WHITE),
	  rpc->user.Current.Retreats,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Retreats[0].What,table->class[rpc->class_origin]->Character[rpc->class_type]->Retreats[1].What,
	  table->class[0]->Character[0]->Retreats[0].What,table->class[0]->Character[0]->Retreats[1].What,
	  rpc->user.History.Retreats,
	  fore(BLU));
  OUT(outbuf);NL;

  sprintf(outbuf,"%s|      %sKOs:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s|",fore(BLU),fore(bg),fore(WHITE),
	  rpc->user.Current.KOs,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Brawls[0].What,table->class[rpc->class_origin]->Character[rpc->class_type]->Brawls[1].What,
	  table->class[0]->Character[0]->Brawls[0].What,table->class[0]->Character[0]->Brawls[1].What,
	  rpc->user.History.KO,
	  fore(BLU));
  OUT(outbuf);NL;

  sprintf(outbuf,"%s|   %sJousts:%s %3u   %3u/%-4u  %3u/%-4u  %6u  %s|",fore(BLU),fore(bg),fore(WHITE),
	  rpc->user.Current.JoustWin,
	  table->class[rpc->class_origin]->Character[rpc->class_type]->Jousts[0].What,table->class[rpc->class_origin]->Character[rpc->class_type]->Jousts[1].What,
	  table->class[0]->Character[0]->Jousts[0].What,table->class[0]->Character[0]->Jousts[1].What,
	  rpc->user.History.Joust,
	  fore(BLU));
  OUT(outbuf);NL;

  sprintf(outbuf,"%s+%.45s+",fore(BLU),line[52]);
  OUT(outbuf);NL;

	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}

void MemorableHits(void)
{
	int o, t;

	OUT("<BODY bgcolor=\"Black\" text=\"Silver\" link=\"White\" vlink=\"Silver\">"); NL;

	sprintf(outbuf, "<H2>%sMemorable Hits%s</H2>", fore(BRN), fore(WHITE));
	OUT(outbuf); NL;
	OUT("<PRE>"); NL;
	sprintf(outbuf, "%s  Class    Player                     Date     Melee/Blast/Big", fore(CYN));
	OUT(outbuf); NL;
	OUT("--------------------------------------------------------------"); NL;
	for(o = 0; o < NUMCLASS; o++) {
		for(t = 0; t < MAXCLASS(o); t++) {
			if(o || t)
				sprintf(outbuf, "%s%-9s", fore(GRY), table->class[o]->Character[t]->Name);
			else
				sprintf(outbuf,"%sAll-Time ", fore(GRY));
			OUT(outbuf);
			fdate(dated, table->class[o]->Character[t]->melee.When);
			sprintf(outbuf, "  %s%-22s  %s     %5u", fore(WHITE), table->class[o]->Character[t]->melee.Who, &dated[4], table->class[o]->Character[t]->melee.What);
			OUT(outbuf); NL;
			if(table->class[o]->Character[t]->blast.What) {
				fdate(dated, table->class[o]->Character[t]->blast.When);
				sprintf(outbuf, "           %-22s  %s     %5u", table->class[o]->Character[t]->blast.Who, &dated[4], table->class[o]->Character[t]->blast.What);
				OUT(outbuf); NL;
			}
			if(table->class[o]->Character[t]->bigblast.What) {
				fdate(dated, table->class[o]->Character[t]->bigblast.When);
				sprintf(outbuf, "           %-22s  %s     %5u", table->class[o]->Character[t]->bigblast.Who, &dated[4], table->class[o]->Character[t]->bigblast.What);
				OUT(outbuf); NL;
			}
			NL;
		}
	}
	OUT("</PRE>"); NL;
	OUT("</BODY>"); NL;
}

int main(int argc, char **argv)
{
	entry entry;
	FILE *fp;
	mode_t mode;
	char parameter[128];
	char cmd = '\0';
	char *query;
	long l;
	int i1 = 0, i2 = 0;

	if((query = getenv("CONTENT_LENGTH"))) {
		l = atoi(query) + 1;
		query = calloc(1, l);
		fgets(query, l, stdin);
	}
	else {
		if((query = getenv("QUERY_STRING"))) {
			l = strlen(query);
		}
		else {
			printf("Location: http://robert.hurst-ri.us/games/rpgd\n\n");
			exit(TRUE);
		}
	}

	//	if any of these signals are raised, logoff gracefully... we hope!
	memset(sigtrap, 0, sizeof(sigtrap));
	signal(SIGHUP, (sighandler_t)signalHUP);
	signal(SIGINT, (sighandler_t)signalHUP);
	signal(SIGIOT, (sighandler_t)signalHUP);
	signal(SIGPIPE, (sighandler_t)signalHUP);
	signal(SIGQUIT, (sighandler_t)signalHUP);
	signal(SIGTERM, (sighandler_t)signalHUP);
	atexit(Shutdown);

	mode = 007;
	umask(mode);

	if((fp=fopen("/etc/rpgd.conf","r"))) {
		while(fgets(parameter, sizeof(parameter), fp)) {
			while(strlen(parameter) > 0 && isspace(parameter[strlen(parameter) - 1]))
				parameter[strlen(parameter) - 1] = '\0';
			if(strncmp(parameter,"HOME=",5) == 0)
				chdir(&parameter[5]);
		}
		fclose(fp);
	}

	OUT("Content-Type: text/html\n\n");

	if(Init()) {
		while(*query) {
			entry.val = calloc(1, getsize(query, '&'));
			getword(entry.val, query, '&');
			plustospace(entry.val);
			unescape_url(entry.val);
			entry.name = calloc(1, getsize(entry.val, '='));
			getword(entry.name, entry.val, '=');
			if(strcasecmp(entry.name, "cmd") == 0) {
				cmd = entry.val[0];
			}
			if(strcasecmp(entry.name, "int1") == 0) {
				i1 = atoi(entry.val);
			}
			if(strcasecmp(entry.name, "int2") == 0) {
				i2 = atoi(entry.val);
			}
			if(strcasecmp(entry.name, "player") == 0) {
				strncpy(PLAYER.ID, entry.val, sizeof(PLAYER.ID));
			}
			free(entry.name);
			free(entry.val);
		}

		GetSystemDate(Today);
		GetSystemTime(Time);
		julian(&Julian, Today);

		OUT("<HTML>"); NL;
		OUT("<HEAD>"); NL;
		sprintf(outbuf, "<TITLE>RPGD: %s Player Status Menu</TITLE>", SYSREC->Name);
		OUT(outbuf); NL;
		OUT("</HEAD>"); NL;
		OUT("<BODY bgcolor=\"Black\" text=\"Silver\" link=\"White\" vlink=\"Silver\">"); NL;

		sprintf(outbuf, "<H2>%s%s Player Status Menu%s</H2>", fore(BLUE), SYSREC->Name, fore(GRY));
		OUT(outbuf); NL;
		OUT("<OL>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=H\">High Score List</A></LI>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=O\">Memorable Hits</A></LI>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=M\">Most Wanted List</A></LI>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=N\">Tiny's Tavern Newspapers</A></LI>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=W\">Past Winners List</A></LI>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=F\">Hall of Fame</A></LI>"); NL;
		OUT("<LI><A HREF=\"rpgweb?cmd=L\">Hall of Lame</A></LI>"); NL;
		OUT("</OL>"); NL;
		OUT("<HR>"); NL;
		OUT("</BODY>"); NL;
		switch(cmd) {
			case 'F':
				ListofHOF(i1, i2);
				break;
			case 'H':
				HiScores(i1);
				break;
			case 'L':
				ListofHOL(i1, i2);
				break;
			case 'M':
				MostWanted();
				break;
			case 'N':
				Newspapers();
				break;
			case 'O':
				MemorableHits();
				break;
			case 'W':
				PastWinners();
				break;
			case 'Y':
				YourStats();
				break;
		}
		OUT("</HTML>"); NL;
	}
	exit(0);
}
