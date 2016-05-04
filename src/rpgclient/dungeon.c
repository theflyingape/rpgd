/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * dungeon.c
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

#define LEVEL(l)		(deep_dank_dungeon[nest] + l)
#define NUMWALK(l)		(deep_dank_dungeon[nest] + l)->numwalk
#define ROOM(l, x, y)	(((deep_dank_dungeon[nest] + l)->room) + (y) * (deep_dank_dungeon[nest] + l)->MaxCol + x)
#define	MAP				1
#define	MAGIC_MAP		2
#define	GIFT_VIAL		1
#define	GIFT_POISON		2
#define	GIFT_MAGIC		3
#define	GIFT_XMAGIC		4
#define	GIFT_CHEST		5
#define GIFT_MAP		6
#define GIFT_ARMOR		7
#define	GIFT_WEAPON		8
#define GIFT_DETECT		9
#define	TRAP_DOOR		1
#define DEEP_DANK_DUNGEON	2
#define	WELL			3
#define WHEEL			4
#define	THIEF			5
#define	CLERIC			6
#define	WIZARD			7

struct room {
	UBYTE map:1;			//	explored (0/1)
	UBYTE occupant:3;		//	0=none, 1=trap door, 2=deeper dungeon, 3=well, 4=wheel, 5=thief, 6=cleric, 7=wizard
	UBYTE type:2;			//	0=Emp, 1=N-S, 2=W-E, 3=Cav
	UBYTE gift_id:2;		//	0=unknown, 1=not identified, 2=identified
	UBYTE gift_type:4;		//	0=none, 1=vial, 2=poison, 3=wand/scroll, 4=extra spell, 5=chest, 6=map, 7=armor, 8=weapon, 9=detect
	UBYTE gift_value:4;		//	0-15
	UBYTE level[3];			//	0=none, 1-99=monster level
	UBYTE monster[3];		//	monster name pointer (0-99)
};
struct deep_dank_dungeon {
	UBYTE Map;
	UBYTE MaxRow, MaxCol;
	long numwalk;
	struct room *room;
} *deep_dank_dungeon[11];

int dl, escall, escape, hx, hy, fx, fy, mymove;
BYTE dmap[21][80];

void chkroom(int x, int y)
{
	ROOM(dl, x, y)->map = FALSE;
	if(x + 1 < LEVEL(dl)->MaxCol)
		if(ROOM(dl, x + 1, y)->map && ROOM(dl, x, y)->type != 1 && ROOM(dl, x + 1, y)->type != 1)
			chkroom(x + 1, y);
	if(y + 1 < LEVEL(dl)->MaxRow)
		if(ROOM(dl, x, y + 1)->map && ROOM(dl, x, y)->type != 2 && ROOM(dl, x, y + 1)->type != 2)
			chkroom(x, y + 1);
	if(x - 1 >= 0)
		if(ROOM(dl, x - 1, y)->map && ROOM(dl, x, y)->type != 1 && ROOM(dl, x - 1, y)->type != 1)
			chkroom(x - 1, y);
	if(y - 1 >= 0)
		if(ROOM(dl, x, y - 1)->map && ROOM(dl, x, y)->type != 2 && ROOM(dl, x, y - 1)->type != 2)
			chkroom(x, y - 1);
}

int chklevel(void)
{
	int x, y;
	int flag = TRUE;

	chkroom(0, 0);
	for(y = 0; y < LEVEL(dl)->MaxRow; y++)
		for(x = 0; x < LEVEL(dl)->MaxCol; x++)
			if(ROOM(dl, x, y)->map)
				flag = FALSE;
	return(flag);
}

void clrmap(void)
{
	sprintf(outbuf, "\33[1;%ur\33[J", rows);
	OUT(outbuf);
	mymove = TRUE;
}

void drawroom(int x,int y)
{
	BYTE dot, *mon;

	if(ANSI == 2 || local)
		dot = '\372';
	else {
		OUT("\16");
		dot = '~';
	}

	if(!LEVEL(dl)->Map) {
		plot(y * 2 + 1, x * 6 + 1);
		sprintf(outbuf, "%s%.7s", fore(BLACK), &dmap[y * 2][6 * x]);
		OUT(outbuf);
	}

	plot(y * 2 + 2, x * 6 + 1);
	sprintf(outbuf, "%s%c", fore(BLACK), dmap[y * 2 + 1][6 * x]);
	OUT(outbuf);
	if(ROOM(dl, x, y)->map)
		sprintf(outbuf, "%s  %c  ", fore(WHITE), dot);
	else
		strcpy(outbuf, "     ");
	if(ROOM(dl, x, y)->map || LEVEL(dl)->Map == MAGIC_MAP || LEVEL(dl)->Map == GIFT_DETECT) {
		mon = NULL;
		if(ROOM(dl, x, y)->level[0]) {
			mon = ROOM(dl, x, y)->level[1] ? "Mob" : "Mon";
			sprintf(outbuf, "%s%s %s %s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(ROOM(dl, x, y)->occupant ? GREEN : RED), mon, (ANSI == 1 || ANSI == 3 ? "\16" : ""));
		}
		switch(ROOM(dl, x, y)->occupant) {
			case 0:
				break;
			case DEEP_DANK_DUNGEON:
				if(!mon)
					mon = "DDD";
				sprintf(outbuf, "%s%s(%s%s%s)%s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(BLU), fore(BLUE), mon, fore(BLU), (ANSI == 1 || ANSI == 3 ? "\16" : ""));
				break;
			case CLERIC:
				if(!mon)
					mon = "Clr";
				sprintf(outbuf, "%s%s(%s%s%s)%s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(BRN), fore(YELLOW), mon, fore(BRN), (ANSI == 1 || ANSI == 3 ? "\16" : ""));
				break;
			case WIZARD:
				if(!mon)
					mon = "Wiz";
				sprintf(outbuf, "%s%s(%s%s%s)%s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(MAG), fore(MAGENTA), mon, fore(MAG), (ANSI == 1 || ANSI == 3 ? "\16" : ""));
				break;
			default:
				if(LEVEL(dl)->Map == MAGIC_MAP || LEVEL(dl)->Map == GIFT_DETECT)
					sprintf(outbuf, "%s\33[5m  ?  \33[m", fore(CYAN));	// Amiga Term doesn't support "\33[25m"
				break;
		}
	}
	OUT(outbuf);
	sprintf(outbuf, "%s%c", fore(BLACK), dmap[y * 2 + 1][6 * (x + 1)]);
	OUT(outbuf);

	if(!LEVEL(dl)->Map) {
		plot(y * 2 + 3, x * 6 + 1);
		sprintf(outbuf, "%s%.7s", fore(BLACK), &dmap[y * 2 + 2][6 * x]);
		OUT(outbuf);
	}
	if(ANSI == 1 || ANSI == 3)
		OUT("\17");
}

void drawyou(void)
{
	plot(hy * 2 + 2, hx * 6 + 2);
	REVERSE;
	sprintf(outbuf, "%s-YOU-", fore(WHITE));
	OUT(outbuf);
	NORMAL;
	plot(rows, 1);
}

void drawmap(void)
{
	int r, c;
	BYTE dot, *mon;

	clrmap();
	if(ANSI == 2 || local)
		dot = '\372';
	else {
		OUT("\16");
		dot = '~';
	}

	if(LEVEL(dl)->Map) {
		for(r = 0; r < LEVEL(dl)->MaxRow * 2 + 1; r++) {
			if(r % 2) {
				for(c = 0; c < LEVEL(dl)->MaxCol; c++) {
					sprintf(outbuf, "%s%c", fore(BLACK), dmap[r][6 * c]);
					OUT(outbuf);
					if(ROOM(dl, c, r / 2)->map)
						sprintf(outbuf, "%s  %c  ", fore(WHITE), dot);
					else
						strcpy(outbuf, "     ");
					if(ROOM(dl, c, r / 2)->map || LEVEL(dl)->Map == MAGIC_MAP || LEVEL(dl)->Map == GIFT_DETECT) {
						mon = NULL;
						if(ROOM(dl, c, r / 2)->level[0]) {
							mon = ROOM(dl, c, r / 2)->level[1] ? "Mob" : "Mon";
							sprintf(outbuf, "%s%s %s %s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(ROOM(dl, c, r / 2)->occupant ? GREEN : RED), mon, (ANSI == 1 || ANSI == 3 ? "\16" : ""));
						}
						switch(ROOM(dl, c, r / 2)->occupant) {
							case 0:
								break;
							case DEEP_DANK_DUNGEON:
								if(!mon)
									mon = "DDD";
								sprintf(outbuf, "%s%s(%s%s%s)%s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(BLU), fore(BLUE), mon, fore(BLU), (ANSI == 1 || ANSI == 3 ? "\16" : ""));
								break;
							case CLERIC:
								if(!mon)
									mon = "Clr";
								sprintf(outbuf, "%s%s(%s%s%s)%s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(BRN), fore(YELLOW), mon, fore(BRN), (ANSI == 1 || ANSI == 3 ? "\16" : ""));
								break;
							case THIEF:
								if((strcasecmp("Thief", CLASS(ONLINE)->Name) == 0 && LEVEL(dl)->Map == MAGIC_MAP) || LEVEL(dl)->Map == GIFT_DETECT)
									sprintf(outbuf, "%s\33[5m  &  \33[m", fore(BLACK));
								break;
							case WELL:
								if((nest > 2 && LEVEL(dl)->Map == GIFT_DETECT) || (ACCESS(PLAYER.Access)->Sysop == 'Y'))
									sprintf(outbuf, "%s\33[5m  *  \33[m", fore(MAGENTA));
								break;
							case WHEEL:
								if((nest > 1 && LEVEL(dl)->Map == GIFT_DETECT) || (ACCESS(PLAYER.Access)->Sysop == 'Y'))
									sprintf(outbuf, "%s\33[5m  @  \33[m", fore(MAGENTA));
								break;
							case WIZARD:
								if(!mon)
									mon = "Wiz";
								sprintf(outbuf, "%s%s(%s%s%s)%s", (ANSI == 1 || ANSI == 3 ? "\17" : ""), fore(MAG), fore(MAGENTA), mon, fore(MAG), (ANSI == 1 || ANSI == 3 ? "\16" : ""));
								break;
							default:
								if(LEVEL(dl)->Map == MAGIC_MAP || LEVEL(dl)->Map == GIFT_DETECT)
									sprintf(outbuf, "%s\33[5m  ?  \33[m", fore(CYAN));	// Amiga Term doesn't support "\33[25m"
								break;
						}
					}
					if(LEVEL(dl)->Map == GIFT_DETECT && ROOM(dl, c, r / 2)->gift_type)
						REVERSE;
					OUT(outbuf);
					if((LEVEL(dl)->Map == GIFT_DETECT || ACCESS(PLAYER.Access)->Sysop == 'Y') && ROOM(dl, c, r / 2)->gift_type)
						NORMAL;
				}
				sprintf(outbuf, "%s%c", fore(BLACK), dmap[r][6 * LEVEL(dl)->MaxCol]);
				OUT(outbuf);
			}
			else {
				OUT(fore(BLACK));
				OUT(&dmap[r][0]);
			}
			CLL; NL;
		}
	}
	else
		for(r = 0; r < LEVEL(dl)->MaxRow; r++)
			for(c = 0; c < LEVEL(dl)->MaxCol; c++)
				if(ROOM(dl, c, r)->map)
					drawroom(c, r);

	if(!local)
		if(ANSI == 1 || ANSI == 3)
			OUT("\17");
	sprintf(outbuf, "\33[%u;%ur", 2 * (LEVEL(dl)->MaxRow + 1), rows);
	OUT(outbuf);
}

void renderdmap(void)
{
	int b, c, r, x, y;

	b = (ANSI == 2 || local) ? 1 : 0;
	memset(dmap, 0, sizeof(dmap));
	memset(&dmap[0][1], mybox[b][0], 6 * LEVEL(dl)->MaxCol - 1);
	for(y = 1; y < 2 * LEVEL(dl)->MaxRow; y++)
		sprintf(&dmap[y][0], "%c%*s%c", mybox[b][10], 6 * LEVEL(dl)->MaxCol - 1, "", mybox[b][10]);
	memset(&dmap[2 * LEVEL(dl)->MaxRow][1], mybox[b][0], 6 * LEVEL(dl)->MaxCol - 1);

	for(y = 0; y < LEVEL(dl)->MaxRow; y++) {
		for(x = 0; x < LEVEL(dl)->MaxCol; x++) {
			if(ROOM(dl, x, y)->type == 1) {
				r = 2 * y; c = 6 * x;
				if(dmap[r][c] == ' ')
					dmap[r][c] = mybox[b][10];
				else
				if(dmap[r][c] == mybox[b][3])
					dmap[r][c] = mybox[b][6];
				else
				if(dmap[r][c] == mybox[b][2])
					dmap[r][c] = mybox[b][5];
				else
				if(dmap[r][c] == mybox[b][1])
					dmap[r][c] = mybox[b][4];
				else
				if(dmap[r][c] == mybox[b][0])
					dmap[r][c] = mybox[b][c > 0 && dmap[r][c - 1] == ' ' ? 7 : dmap[r][c + 1] == ' ' ? 9 : 8];

				dmap[r + 1][c] = mybox[b][10];

				if(dmap[r + 2][c] == ' ')
					dmap[r + 2][c] = mybox[b][10];
				else
				if(dmap[r + 2][c] == mybox[b][0])
					dmap[r + 2][c] = mybox[b][c > 0 && dmap[r + 2][c - 1] == ' ' ? 1 : dmap[r + 2][c + 1] == ' ' ? 3 : 2];

				c = 6 * (x + 1);
				if(dmap[r][c] == ' ')
					dmap[r][c] = mybox[b][10];
				else
				if(dmap[r][c] == mybox[b][0])
					dmap[r][c] = mybox[b][dmap[r][c - 1] == ' ' ? 7 : dmap[r][c + 1]==' ' ? 9 : 8];
				else
				if(dmap[r][c] == mybox[b][1])
					dmap[r][c] = mybox[b][4];
				else
				if(dmap[r][c] == mybox[b][2])
					dmap[r][c] = mybox[b][5];
				else
				if(dmap[r][c] == mybox[b][3])
					dmap[r][c] = mybox[b][6];

				dmap[r + 1][c] = mybox[b][10];
				dmap[r + 2][c] = mybox[b][r + 2 < 2 * LEVEL(dl)->MaxRow ? 10 : 2];
			}
			if(ROOM(dl, x, y)->type == 2) {
				r = 2 * y; c = 6 * x;
				if(dmap[r][c] == ' ')
					dmap[r][c] = mybox[b][0];
				else
				if(dmap[r][c] == mybox[b][3])
					dmap[r][c] = mybox[b][2];
				else
				if(dmap[r][c] == mybox[b][6])
					dmap[r][c] = mybox[b][5];
				else
				if(dmap[r][c] == mybox[b][9])
					dmap[r][c] = mybox[b][8];
				else
				if(dmap[r][c] == mybox[b][10])
					dmap[r][c] = mybox[b][r > 0 && dmap[r - 1][c] == ' ' ? 7 : dmap[r + 1][c] == ' ' ? 1 : 4];

				memset(&dmap[r][c + 1], mybox[b][0], 5);

				if(dmap[r][c + 6] == ' ')
					dmap[r][c + 6] = mybox[b][0];
				else
				if(dmap[r][c + 6] == mybox[b][1])
					dmap[r][c + 6] = mybox[b][2];
				else
				if(dmap[r][c + 6] == mybox[b][10])
					dmap[r][c + 6] = mybox[b][dmap[r + 1][c + 6] == mybox[b][10] ? 6 : 3];

				if(dmap[r + 2][c] == mybox[b][10])
					dmap[r + 2][c] = mybox[b][c > 0 && dmap[r + 2][c - 1] == ' ' ? 1 : 4];
				else
				if(dmap[r + 2][c] == ' ')
					dmap[r + 2][c] = mybox[b][0];

				memset(&dmap[r + 2][c + 1], mybox[b][0], 5);

				if(dmap[r + 2][c + 6] == ' ')
					dmap[r + 2][c + 6] = mybox[b][0];
				else
				if(dmap[r + 2][c + 6] == mybox[b][10])
					dmap[r + 2][c + 6] = mybox[b][dmap[r + 3][c + 6] == mybox[b][10] ? 6 : 3];
			}
		}
	}
	dmap[0][0] = mybox[b][7];
	sprintf(&dmap[0][6 * LEVEL(dl)->MaxCol], "%c", mybox[b][9]);
	dmap[2 * LEVEL(dl)->MaxRow][0] = mybox[b][1];
	sprintf(&dmap[2 * LEVEL(dl)->MaxRow][6 * LEVEL(dl)->MaxCol], "%c", mybox[b][3]);
	NUMWALK(dl) = 0;
}

int putmonster(int x, int y)
{
	int i, j, l;

	for(i = 0; i < 3 && ROOM(dl, x, y)->level[i]; i++);
	if(i == 3)
		return(FALSE);
	for(j = 0, l = 0; j < 4; j++, l += dice(7));
	l /= j;
	switch(l) {
		case 1:
			l = dice(dl);
			break;
		case 2:
			l = dl - 3 - dice(3);
			break;
		case 3:
			l = dl - dice(3);
			break;
		case 4:
			l = dl;
			break;
		case 5:
			l = dl + dice(3);
			break;
		case 6:
			l = dl + 3 + dice(3);
			break;
		case 7:
			l = dl + dice(100 - dl);
			break;
	}
	l = (l < 1) ? 1 : (l > 99) ? 99 : l;
	l = (i == 1) ? l / 2 + dice(l / 2 + 1) : (i == 2) ? dice(l + 1) : l;
	l = (l < 1) ? 1 : (l > 99) ? 99 : l;
	ROOM(dl, x, y)->level[i] = l;
	j = l * NUMDUNGEON / 100 + dice(3) - 2;
	j = (j < 0) ? 0 : (j >= NUMDUNGEON) ? NUMDUNGEON - 1 : j;
	ROOM(dl, x, y)->monster[i] = j;
	return(TRUE);
}

void teleport(void)
{
	char c;

	OUT(fore(YELLOW));
	OUT("What do you wish to do?"); NL; NL;
	NORMAL;
	OUT("<U> Teleport up 1 level"); NL;
	if(dl < 99) {
		OUT("<D> Teleport down 1 level"); NL;
	}
	OUT("<O> Teleport out of this dungeon"); NL;
	OUT("<R> Random teleport"); NL; NL;
	sprintf(outbuf, "%sTime Left:%s %d min.", fore(MAG), fore(GRY), timeleft());
	OUT(outbuf); NL;
	do {
		NL;
		sprintf(prompt, "%sTeleport #%s.%d: ", fore(CYN), numlev[nest], dl + 1);
		OUT(prompt);
		c = inkey('\0', 'O');
	} while(!strchr("UDOR", c));
	NL; NL;
	if(c == 'U') {
		if(dl > 0) {
			NUMWALK(dl)++;
			dl--;
		}
		if(LEVEL(dl)->room) {
			renderdmap();
			c = 'R';
		}
	}
	if(c == 'D') {
		if(dl < 99) {
			NUMWALK(dl) *= dl / 10 + 1;
			dl++;
		}
		if(LEVEL(dl)->room) {
			renderdmap();
			c = 'R';
		}
	}
	if(c == 'O') {
		escape = TRUE;
		sound2("OFF", 1);
	}
	if(c == 'R') {
		hx = dice(LEVEL(dl)->MaxCol) - 1; hy = dice(LEVEL(dl)->MaxRow) - 1;
		fx = hx; fy = hy;
		mymove = TRUE;
	}
	sound("teleport", 64, 0);
}

void GenLevel(void)
{
	int i, j, m, n, x, y;
	int wow = 1;

	LEVEL(dl)->MaxRow = 6 + dice(dl / 32 + 1);	//	7-10 high
	while(LEVEL(dl)->MaxRow < 10 && dice(ONLINE->CHA / (4 * (ONLINE->user.MyBackstab + 1))) == 1)
		LEVEL(dl)->MaxRow++;
	LEVEL(dl)->MaxCol = 6 + dice(dl / 16 + 1);	//	7-13 wide
	while(LEVEL(dl)->MaxCol < 13 && dice(ONLINE->CHA / (4 * (ONLINE->user.MyBackstab + 1))) == 1)
		LEVEL(dl)->MaxCol++;
	LEVEL(dl)->room = calloc(LEVEL(dl)->MaxRow * LEVEL(dl)->MaxCol, sizeof(struct room));

	do {
		for(i = 0; i < LEVEL(dl)->MaxRow; i++)
			for(j = 0; j < LEVEL(dl)->MaxCol; j++) {
				while((n = (dice(4) + dice(4)) / 2 - 1) == 3);
				n = (n == 0) ? 3 : (n == 1) ? 0 : dice(2);
				ROOM(dl, j, i)->type = n;
				ROOM(dl, j, i)->map = TRUE;
			}
	} while(!chklevel());

	m = LEVEL(dl)->MaxRow * LEVEL(dl)->MaxCol / 6 + dice(dl / 11) + nest / 2 + dice(nest / 2);
	for(n = 0; n < m; n++) {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		putmonster(x, y);
	}

	//	bonus for the more experienced player
	if(dice(PLAYER.Immortal) > dl && dice(PLAYER.Wins) > nest && PLAYER.Novice != 'Y') {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		ROOM(dl, x, y)->gift_type = GIFT_DETECT;
		if((dice(100 * dl + 1) / nest) == 1)
			wow = LEVEL(dl)->MaxRow * LEVEL(dl)->MaxCol;
	}

	m = LEVEL(dl)->MaxRow * LEVEL(dl)->MaxCol / 10;
	if(dice(100 - dl) > nest)
		m += dice(dl / 16 + 2);
	for(n = 0; n < m; n++) {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		ROOM(dl, x, y)->occupant = TRAP_DOOR;
	}

	if(dice((102 - dl) / 3 + nest) == 1 && PLAYER.Novice != 'Y') {
		for(i = 0; i < wow; i++) {
			x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
			ROOM(dl, x, y)->occupant = WELL;
		}
		wow = 1;
	}

	if(dice((102 - dl) / 3 + nest) == 1 && PLAYER.Novice != 'Y') {
		for(i = 0; i < wow; i++) {
			x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
			ROOM(dl, x, y)->occupant = WHEEL;
		}
		wow = 1;
	}

	if(nest < 10 && nest < PLAYER.Immortal && PLAYER.Novice != 'Y') {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		ROOM(dl, x, y)->occupant = DEEP_DANK_DUNGEON;
	}

	m = dice(nest / 4) + wow - 1;
	for(n = 0; n < m; n++) {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		ROOM(dl, x, y)->occupant = THIEF;
	}

	do {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
	} while(ROOM(dl, x, y)->occupant);
	ROOM(dl, x, y)->occupant = CLERIC;

	do {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
	} while(ROOM(dl, x, y)->occupant);
	ROOM(dl, x, y)->occupant = WIZARD;

	wow = 1;
	//	bonus for the more experienced player
	if(dice(PLAYER.Immortal) > dl && PLAYER.Novice != 'Y')
		if((dice(100 * dl + 1) / nest) == 1)
			wow = LEVEL(dl)->MaxRow * LEVEL(dl)->MaxCol;

	m = dice(dl / 33) + dice(nest / 3) + wow - 2;
	for(n = 0; n < m; n++) {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		if(dice(nest + 10) > nest) {
			ROOM(dl, x, y)->gift_id = FALSE;
			ROOM(dl, x, y)->gift_type = GIFT_VIAL;
			j = dice(126 + nest);
			for(i = 0; i < 16 && j > 0; i++)
				j -= i + 1;
			ROOM(dl, x, y)->gift_value = 16 - i;
			continue;
		}
		if(dice(nest + 5) > nest && ONLINE->user.MyPoison) {
			ROOM(dl, x, y)->gift_type = GIFT_POISON;
			j = dice(126 + nest);
			for(i = 0; i < 16 && j > 0; i++)
				j -= i + 1;
			ROOM(dl, x, y)->gift_value = 16 - i;
			continue;
		}
		if(dice(nest + 5) > nest && (ONLINE->user.MyMagic == 1 || ONLINE->user.MyMagic == 2)) {
			ROOM(dl, x, y)->gift_type = GIFT_MAGIC;
			j = dice(126 + nest);
			for(i = 0; i < 16 && j > 0; i++)
				j -= i + 1;
			ROOM(dl, x, y)->gift_value = 16 - i;
			continue;
		}
		if(dice(nest + 3) > nest && (ONLINE->user.MyMagic == 1 || ONLINE->user.MyMagic == 2)) {
			ROOM(dl, x, y)->gift_type = GIFT_XMAGIC;
			j = dice(11 + nest);
			for(i = 0; i < 8 && j > 0; i++)
				j -= i + 1;
			ROOM(dl, x, y)->gift_value = 8 - i;
			continue;
		}
		if(dice(nest + ONLINE->user.MyMagic + 4) > nest) {
			ROOM(dl, x, y)->gift_type = GIFT_CHEST;
			ROOM(dl, x, y)->gift_value = dice(nest + 6) - 1;
			continue;
		}
		if(dice(nest * (ONLINE->user.MyMagic + 3)) - ONLINE->user.MyMagic > nest) {
			ROOM(dl, x, y)->gift_type = GIFT_ARMOR;
			ROOM(dl, x, y)->gift_value = dice(nest + 6) - 1;
			continue;
		}
		if(dice(nest * (ONLINE->user.MyMagic + 2)) - ONLINE->user.MyMagic > nest) {
			ROOM(dl, x, y)->gift_type = GIFT_WEAPON;
			ROOM(dl, x, y)->gift_value = dice(nest + 6) - 1;
			continue;
		}
	}

	m = nest - 1;
	for(i = 0; i < LEVEL(dl)->MaxCol; i++)
		for(j = 0; j < LEVEL(dl)->MaxRow; j++)
			if(ROOM(dl, i, j)->gift_type == GIFT_DETECT)
				m = 0;

	if(m) {
		x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
		ROOM(dl, x, y)->gift_type = GIFT_MAP;
	}

	renderdmap();
	drawmap();

	hx = dice(LEVEL(dl)->MaxCol) - 1; hy = dice(LEVEL(dl)->MaxRow) - 1;
	fx = hx; fy = hy;
	mymove = TRUE;

	if(!logoff) {
		sprintf(outbuf, "is entering Dungeon %s, level %d", numlev[nest], dl + 1);
		broadcast(outbuf);
	}

	if((int)PLAYER.Level / 9 - nest > PLAYER.Security) {
		NL;
		OUT("The feeling of insecurity overwhelms you."); NL;
		paused();
	}
}

void ExamineRoom(void)
{
	static char *wol[] = { " Grace ", " Doom! ", "Fortune", " Taxes ", " Power ", " Death ", " =Key= ", " Morph ", "+Skill+" };
	double g;
	UWORD t;
	int i, j, m, n, z;
	char c, *s = '\0', *v = '\0';

	if(ROOM(dl, hx, hy)->level[0]) {
		clrmap();
		OUT("There is something lurking in this chamber...");
		Delay(25);
		for(i = 1; i < 5; i++)
			if(RPC[0][i]->HP < 1)
				memset(RPC[0][i], 0, sizeof(struct RPC));
		for(i = 0; i < 5; i++)
			memset(RPC[1][i], 0, sizeof(struct RPC));
		for(i = 0, m = 0; i < 3 && ROOM(dl, hx, hy)->level[i]; i++) {
			strcpy(RPC[1][m]->user.Handle, DUNGEON(ROOM(dl, hx, hy)->monster[m])->Name);
			strcpy(RPC[1][m]->user.Class, DUNGEON(ROOM(dl, hx, hy)->monster[m])->Class);
			RPC[1][m]->user.Spell = DUNGEON(ROOM(dl, hx, hy)->monster[m])->Spell;
			RPC[1][m]->user.XSpell = DUNGEON(ROOM(dl, hx, hy)->monster[m])->XSpell;
			RPC[1][m]->user.Level = ROOM(dl, hx, hy)->level[m];
			RPC[1][m]->user.Gender = 'I';

			g = 1.;

			n = MAXWEAPON(0) * dl / 100 + dice(3) - 2;
			if(ONLINE->weapon_origin == 0)
				n = (n + ONLINE->weapon_type) / 2;
			n = (n < 1) ? 1 : (n >= MAXWEAPON(0)) ? MAXWEAPON(0) - 1 : n;
			if(dice(PLAYER.Level / 4 + 12 - ONLINE->CHA / 10) == 1) {
				j = 0;
				if(ONLINE->weapon_origin == 0) {
					n = ONLINE->weapon_type + dice(3) - 2;
					n = (n < 1) ? 1 : (n >= MAXWEAPON(0)) ? MAXWEAPON(0) - 1 : n;
				}
				if(dice(111 - nest - dl) == 1) {
					for(; j < NUMWEAPON && strcasecmp(table->weapon[j]->Origin, "DUNGEON"); j++);
					if(j < NUMWEAPON)
						n = dice(MAXWEAPON(j)) - 1;
				}
				sprintf(RPC[1][m]->user.Weapon, "%s.%s", table->weapon[j]->Origin, table->weapon[j]->Item[n]->Name);
				if(dice(ONLINE->CHA + nest) <= nest)
					RPC[1][m]->user.WCmod = (RPC[1][m]->weapon_origin < NUMWEAPON) ? 1 - dice(WEAPON(RPC[1][m])->Class) : 0;
			}
			else {
				n += dice(nest);
				sprintf(RPC[1][m]->user.Weapon, "NATURAL.%u", n);
			}
			n = (n < 1) ? 1 : (n >= MAXWEAPON(0)) ? MAXWEAPON(0) - 1 : n;
			modf(value(table->weapon[0]->Item[n]->Value, dice(ONLINE->CHA) / 5 + 5), &d);
			g += d;

			n = MAXARMOR(0) * dl / 100 + dice(3) - 2;
			if(ONLINE->armor_origin == 0)
				n = (n + ONLINE->armor_type) / 2;
			n = (n < 1) ? 1 : (n >= MAXARMOR(0)) ? MAXARMOR(0) - 1 : n;
			if(dice(PLAYER.Level / 3 + 12 - ONLINE->CHA / 10) == 1) {
				j = 0;
				if(ONLINE->armor_origin == 0) {
					n = ONLINE->armor_type + dice(3) - 2;
					n = (n < 1) ? 1 : (n >= MAXARMOR(0)) ? MAXARMOR(0) - 1 : n;
				}
				if(dice(111 - nest - dl) == 1) {
					for(; j < NUMARMOR && strcasecmp(table->armor[j]->Origin, "DUNGEON"); j++);
					if(j < NUMARMOR)
						n = dice(MAXARMOR(j)) - 1;
				}
				sprintf(RPC[1][m]->user.Armor, "%s.%s", table->armor[j]->Origin, table->armor[j]->Item[n]->Name);
				if(dice(ONLINE->CHA + nest) <= nest)
					RPC[1][m]->user.ACmod = (RPC[1][m]->armor_origin < NUMARMOR) ? 1 - dice(ARMOR(RPC[1][m])->Class) : 0;
			}
			else {
				n += dice(nest);
				sprintf(RPC[1][m]->user.Armor, "NATURAL.%u", n);
			}
			n = (n < 1) ? 1 : (n >= MAXARMOR(0)) ? MAXARMOR(0) - 1 : n;
			modf(value(table->armor[0]->Item[n]->Value, dice(ONLINE->CHA) / 5 + 5), &d);
			g += d;

			//	cap the money...
			modf(value(table->weapon[0]->Item[MAXWEAPON(0) - 2]->Value / (dl + 1.), PLAYER.Level), &d);
			if(g > d)
				g = d;

			g *= dice(nest);

			if(g > 1e+05) {
				modf(g / 1e+05, &g);
				g *= 1e+05;
				if(g > 1e+09) {
					modf(g / 1e+09, &g);
					g *= 1e+09;
					if(g > 1e+13) {
						modf(g / 1e+13, &g);
						g *= 1e+13;
					}
				}
			}
			RPC[1][m]->user.Gold = g;

			CreateRPC(RPC[1][m]);

			if(!RPC[1][m]->class_origin && !RPC[1][m]->class_type) {
				strcpy(RPC[1][m]->user.Class, PLAYER.Class);
				CreateRPC(RPC[1][m]);
			}
			RPC[1][m]->user.STR -= (5 - nest / 2); RPC[1][m]->STR -= (5 - nest / 2);
			RPC[1][m]->user.INT -= (5 - nest / 2); RPC[1][m]->INT -= (5 - nest / 2);
			RPC[1][m]->user.DEX -= (5 - nest / 2); RPC[1][m]->DEX -= (5 - nest / 2);
			RPC[1][m]->user.CHA -= (5 - nest / 2); RPC[1][m]->CHA -= (5 - nest / 2);
			RPC[1][m]->user.HP /= 4;
			RPC[1][m]->HP /= 4;
			n = 5 - dice(nest / 3);
			RPC[1][m]->user.SP /= n;
			RPC[1][m]->SP /= n;
			for(n = 0; n < NUMMAGIC; n++)
				if(dice(ONLINE->CHA / 2 + 5 * n + nest) <= nest) {
					if(n < 16)
						RPC[1][m]->user.Spell |= (UWORD)pow(2., (double)n);
					if(n > 15 && n < 24)
						RPC[1][m]->user.XSpell |= (UBYTE)pow(2., (double)n - 16);
				}
			for(n = 0; n < NUMPOISON; n++)
				if(dice(ONLINE->CHA / (RPC[1][m]->user.MyPoison + n + 1)) == 1)
					RPC[1][m]->user.Poison |= (UWORD)pow(2., (double)n);

			NL; NL;
			sprintf(filename, "pix/Dungeon/%s", RPC[1][m]->user.Handle);
			type(filename, FALSE);
			sprintf(outbuf, "It's %s%s... ", AN(RPC[1][m]->user.Handle), RPC[1][m]->user.Handle);
			OUT(outbuf);
			Delay(25);
			for(j = 1; j < 4 && RPC[0][j]->HP; j++);
			if(j < 4 && dice((dl / 5 + 5) * (100 - ONLINE->CHA + nest)) == 1) {
				OUT("and it is charmed by your presence!");
				Delay(100);
				memcpy(RPC[0][j], RPC[1][m], sizeof(struct RPC));
				memset(RPC[1][m], 0, sizeof(struct RPC));
				NL; NL;
				strupr(RPC[0][j]->user.Handle);
				sprintf(prompt, "%sWhat shall you call him by? ", fore(CYN));
				OUT(prompt);
				ins(22);
				proper(inbuf);
				if(strlen(inbuf)) {
					strcpy(RPC[0][j]->user.Handle, inbuf);
					RPC[0][j]->user.Gender = 'M';
					LoadRPC(RPC[0][j]);
				}
				continue;
			}
			OUT("and it doesn't look friendly!");
			Delay(25);
			if(RPC[1][m]->weapon_origin < NUMWEAPON) {
				NL; NL;
				sprintf(outbuf, "%s is carrying %s%s.", RPC[1][m]->He, AN(WEAPON(RPC[1][m])->Name), WEAPON(RPC[1][m])->Name);
				OUT(outbuf);
			}
			if(RPC[1][m]->armor_origin < NUMARMOR) {
				NL; NL;
				sprintf(outbuf, "%s is wearing %s%s.", RPC[1][m]->He, AN(ARMOR(RPC[1][m])->Name), ARMOR(RPC[1][m])->Name);
				OUT(outbuf);
			}
			m++;
		}

		NL; NL;
		Battle();

		if(ONLINE->HP) {
			for(i = 0, j = 0; i < 3 && ROOM(dl, hx, hy)->level[i]; i++) {
				if(RPC[1][i]->HP < 0) {	//  teleported away
					m = dice(LEVEL(dl)->MaxCol) - 1; n = dice(LEVEL(dl)->MaxRow) - 1;
					putmonster(m, n);
					ROOM(dl, hx, hy)->level[i] = 0;
					ROOM(dl, hx, hy)->monster[i] = 0;
				}
				else
				if(RPC[1][i]->HP) {		//  left standing
					ROOM(dl, hx, hy)->level[j] = ROOM(dl, hx, hy)->level[i];
					ROOM(dl, hx, hy)->monster[j] = ROOM(dl, hx, hy)->monster[i];
					j++;
				}
				else {		//  defeated
					if(RPC[1][i]->user.ExpLevel > 0) {
						modf(EXP(RPC[1][i]->user.ExpLevel - 1.) / (20. - (1.5 * (double)nest)), &d);
						PLAYER.Experience += d;
						sprintf(outbuf, "You get %.8g experience.", d);
						OUT(outbuf); NL; NL;
					}
					if(RPC[1][i]->user.Gold > 0.) {
						PLAYER.Gold += RPC[1][0]->user.Gold;
						sprintf(outbuf, "You get %s the %s was carrying.", money(RPC[1][i]->user.Gold, ANSI), RPC[1][i]->user.Handle);
						OUT(outbuf); NL; NL;
						RPC[1][i]->user.Gold = 0.;
						Delay(25);
					}
					ExchangeWeapon(ONLINE, RPC[1][i]);
					ExchangeArmor(ONLINE, RPC[1][i]);
					if(LEVEL(dl)->Map != MAGIC_MAP && LEVEL(dl)->Map != GIFT_DETECT && dice((15 - ONLINE->CHA / 10) / 2) == 1) {
						n = (dice(dl / 33 + 2) > 1 ? MAP : MAGIC_MAP);
						if(LEVEL(dl)->Map < n) {
							LEVEL(dl)->Map = (ACCESS(PLAYER.Access)->Sysop == 'Y') ? GIFT_DETECT : n;
							sprintf(outbuf, "%sYou find a %smap!", fore(YELLOW), (LEVEL(dl)->Map == MAGIC_MAP ? "magic " : ""));
							OUT(outbuf); NL; NL;
							NORMAL;
							Delay(25);
						}
					}
					ROOM(dl, hx, hy)->level[i] = 0;
					ROOM(dl, hx, hy)->monster[i] = 0;
				}
				Delay(25);
			}
		}
		else
			if(PLAYER.Gold > 0.) {
				SYSREC->Gold += PLAYER.Gold;
				RPGserver(SERVER_PUTUSER, (UBYTE *)SYSREC);
				sprintf(outbuf, "The %s takes %s you were carrying.", RPC[1][0]->user.Handle, money(PLAYER.Gold, ANSI));
				OUT(outbuf); NL; NL;
				PLAYER.Gold = 0.;
			}
		if(ROOM(dl, hx, hy)->level[0]) {
			if(hx == fx && hy == fy) {
				if(hy > 0 && ROOM(dl, hx, hy)->type != 2 && ROOM(dl, hx, hy - 1)->type != 2)
					fy--;
				else
				if(hy + 1 < LEVEL(dl)->MaxRow && ROOM(dl, hx, hy)->type != 2 && ROOM(dl, hx, hy + 1)->type != 2)
					fy++;
				else
				if(hx + 1 < LEVEL(dl)->MaxCol && ROOM(dl, hx, hy)->type != 1 && ROOM(dl, hx + 1, hy)->type != 1)
					fx++;
				else
				if(hx > 0 && ROOM(dl, hx, hy)->type != 1 && ROOM(dl, hx-1, hy)->type != 1)
					fx--;
			}
			swmem((BYTE *)&hx, (BYTE *)&fx, sizeof(hx));
			swmem((BYTE *)&hy, (BYTE *)&fy, sizeof(hy));
		}
	}

	if(!cd())
		switch(ROOM(dl, hx, hy)->occupant) {
			case CLERIC:
				if(ONLINE->HP < PLAYER.HP)
					type("PIX/Dungeon/Cleric", FALSE);
				OUT("There is an old cleric in this room."); NL;
				if(ONLINE->HP >= PLAYER.HP) {
					OUT("He says, \"I will pray for you.\""); NL; NL;
					break;
				}
				JOUST(dl);
				modf(d / 6. / PLAYER.HP * (PLAYER.HP - ONLINE->HP), &d);
				if(d < 1.)
					d = 1.;
				d *= nest;
				if(strcasecmp("Cleric", CLASS(ONLINE)->Name) == 0)
					d = 0.;
				if(d > 1e+05) {
					modf(d / 1e+05, &d);
					d *= 1e+05;
					if(d > 1e+09) {
						modf(d / 1e+09, &d);
						d *= 1e+09;
						if(d > 1e+13) {
							modf(d / 1e+13, &d);
							d *= 1e+13;
						}
					}
				}
				sprintf(outbuf, "He says, \"I can heal all your wounds for %s.\"",(d ? money(d, ANSI) : "you, brother"));
				OUT(outbuf); NL;
				if(d) {
					do {
						sprintf(prompt, "%sWill you pay (Y/N)? ", fore(CYN));
						OUT(prompt);
						c = inkey('Y', 'N');
						NL;
					} while(c != 'Y' && c != 'N');
				}
				else
					c = 'Y';
				NL;
				if(PLAYER.Gold < d) {
					OUT("He says, \"Not!\""); NL; NL;
					break;
				}
				if(c == 'Y') {
					sound("shimmer", 64, 0);
					OUT("He casts a Cure spell on you."); NL; NL;
					PLAYER.Gold -= d;
					ONLINE->HP = PLAYER.HP;
				}
				break;
			case WIZARD:
				clrmap();
				OUT(fore(MAGENTA));
				OUT("You encounter a wizard in this room."); NL; NL;
				teleport();
				break;
			case TRAP_DOOR:
				if(dice(100 - dl) > 1) {
					OUT("You have stepped onto a trapdoor!"); NL; NL;
					Delay(25);
					n = (dice(120) < ONLINE->DEX);
					for(m = 1; m < 5; m++)
						if(RPC[0][m]->HP) {
							if(dice(120) < RPC[0][m]->DEX) {
								sprintf(outbuf, "%s manages to catch the edge and stop %sself from falling.", RPC[0][m]->user.Handle, RPC[0][m]->him);
								OUT(outbuf); NL;
								if(!n)
									memset(RPC[0][m], 0, sizeof(struct RPC));
							}
							else {
								sprintf(outbuf, "%s%s falls down a level!", fore(YELLOW), RPC[0][m]->user.Handle);
								OUT(outbuf); NL;
								if(n)
									memset(RPC[0][m], 0, sizeof(struct RPC));
							}
							Delay(25);
						}
					if(n) {
						OUT("You manage to catch the edge and stop yourself from falling."); NL;
						Delay(25); NL;
						ROOM(dl, hx, hy)->occupant = FALSE;
						break;
					}
					OUT(fore(YELLOW));
					OUT("You've fallen down a level!"); NL;
					NORMAL; NL;
					emptykey();
					mymove = TRUE;
					dl++;
					if(LEVEL(dl)->room) {
						renderdmap();
						hx = dice(LEVEL(dl)->MaxCol) - 1;
						hy = dice(LEVEL(dl)->MaxRow) - 1;
						fx = hx; fy = hy;
					}
				}
				else {
					OUT(fore(YELLOW));
					OUT("There is a ");
					switch(dice(3)) {
						case 1:
							OUT("fairie");
							break;
						case 2:
							OUT("pixie");
							break;
						case 3:
							OUT("sprite");
							break;
					}
					OUT(" in this room!");
					NORMAL; NL;
					Delay(50);
					if(dice(dl + 50 / nest) < ONLINE->CHA) {
						OUT("You've enchanted her by your demeanor."); NL;
						Delay(50);
						JOUST(dl);
						d *= nest * dice(nest);
						if(ONLINE->HP < PLAYER.HP / 2) {
							ONLINE->HP += PLAYER.HP / 4 + dice(PLAYER.HP / 4);
							sound("heal", 64, 0);
							OUT("She helps you convalesce by showering you with invigorating beams.");
						}
						else
						if(ONLINE->SP < PLAYER.SP / 2) {
							ONLINE->SP += PLAYER.SP / 4 + dice(PLAYER.SP / 4);
							sound("shimmer", 64, 0);
							OUT("She endows you with restored spell power.");
						}
						else
						if(dice(100 - dl + 1) > 1 && (PLAYER.Gold + PLAYER.Bank) < d) {
							modf(d * dice(dl) / (100. - ONLINE->CHA + nest), &d);
							PLAYER.Gold += d;
							sound("max", 64, 0);
							OUT("She deposits a gemstone in your coin pouch.");
						}
						else
						if(dice(WEAPON(ONLINE)->Class + PLAYER.WCmod) / 2 <= nest) {
							do {
								PLAYER.WCmod++;
							} while(dice(nest + 2) < nest);
							sound("hone", 64, 0);
							OUT("She graces your weapon by a light touch from a tiny finger.");
						}
						else
						if(dice(ARMOR(ONLINE)->Class + PLAYER.ACmod) / 2 <= nest) {
							do {
								PLAYER.ACmod++;
							} while(dice(nest + 2) < nest);
							sound("shield", 64, 0);
							OUT("She coats your armor with a twinkling dust.");
						}
						else
						if(ONLINE->CHA < 100) {
							ONLINE->CHA++;
							OUT("She makes you feel more charismatic.");
						}
						else
						if(ONLINE->DEX < 100) {
							ONLINE->DEX++;
							OUT("She makes you feel more dexterous.");
						}
						else
						if(ONLINE->INT < 100) {
							ONLINE->INT++;
							OUT("She makes you feel more intelligent.");
						}
						else
						if(ONLINE->STR < 100) {
							ONLINE->STR++;
							OUT("She makes you feel stronger.");
						}
						else {
							OUT("She flies away from your magnificence.");
						}
					}
					else
						OUT("She is repulsed by your scars and flees in disgust.");
					NL; NL;
					Delay(50);
					ROOM(dl, hx, hy)->occupant = FALSE;
					break;
				}
				if(dice(100 + nest * dl / 10) > ONLINE->DEX) {
					if(ONLINE->weapon_origin || ONLINE->weapon_type) {
						if(dice(ONLINE->CHA / 10 + nest) <= nest)
							PLAYER.WCmod -= dice(nest / 3);
						ONLINE->ToWC -= dice(WEAPON(ONLINE)->Class / 10 + 1);
						sprintf(outbuf, "Your %s is damaged from the fall!", WEAPON(ONLINE)->Name);
						OUT(outbuf); NL;
					}
				}
				if(dice(100 + nest * dl / 10) > ONLINE->DEX) {
					if(ONLINE->armor_origin || ONLINE->armor_type) {
						if(dice(ONLINE->CHA / 10 + nest) <= nest)
							PLAYER.ACmod -= dice(nest / 3);
						ONLINE->ToAC -= dice(ARMOR(ONLINE)->Class / 10 + 1);
						sprintf(outbuf, "Your %s is damaged from the fall!", ARMOR(ONLINE)->Name);
						OUT(outbuf); NL;
					}
				}
				NL;
				break;
			case THIEF:
				sprintf(outbuf, "%sThere is a thief in this chamber!", fore(YELLOW));
				OUT(outbuf); NL;
				Delay(50);
				NORMAL;
				ROOM(dl, hx, hy)->occupant = FALSE;
				i = dice(LEVEL(dl)->MaxCol) - 1; j = dice(LEVEL(dl)->MaxRow) - 1;
				//  allow some honor among thieves . . .
				if((strcasecmp("Thief", CLASS(ONLINE)->Name) == 0 && dice(nest / 2 + 1) > 1 ) || dice(16 - nest) == 1)
					ROOM(dl, hx, hy)->occupant = THIEF;
				else
					if(!ROOM(dl, i, j)->occupant)
						ROOM(dl, i, j)->occupant = THIEF;
				if(ROOM(dl, hx, hy)->occupant != THIEF) {
					sprintf(outbuf, "%sHe surprises you!%s", fore(WHITE), fore(GRY));
					OUT(outbuf);
					Delay(25);
					OUT("  As he passes by, he steals ");
					z = ONLINE->CHA + nest + 1;
					if(PLAYER.Level / 9 - nest > PLAYER.Security)
						z /= PLAYER.Level;
					if((ONLINE->weapon_origin || ONLINE->weapon_type) && dice(z) == 1) {
						sprintf(outbuf, "your %s %s(%s%+d%s,%s%+d%s)%s!", WEAPON(ONLINE)->Name, fore(MAG), fore(PLAYER.WCmod > 0 ? YELLOW : PLAYER.WCmod < 0 ? BRED : GRY), PLAYER.WCmod, fore(GRY), fore(ONLINE->ToWC > 0 ? YELLOW : ONLINE->ToWC < 0 ? BRED : GRY), ONLINE->ToWC, fore(MAG), fore(GRY));
						sprintf(PLAYER.Weapon, "%s.%s", table->weapon[0]->Origin, table->weapon[0]->Item[0]->Name);
						ONLINE->weapon_origin = 0; ONLINE->weapon_type = 0;
						PLAYER.WCmod = 0; ONLINE->ToWC = 0;
					}
					else
					if(LEVEL(dl)->Map && dice(ONLINE->CHA / 10 + nest + 1) - 1 <= nest / 2) {
						LEVEL(dl)->Map = FALSE;
						strcpy(outbuf, "your map!");
						mymove = TRUE;
					}
					else
					if(PLAYER.Spell && (ONLINE->user.MyMagic == 1 || ONLINE->user.MyMagic == 2) && dice(ONLINE->CHA / 10 + nest + 1) - 1 <= nest / 2) {
						do {
							i = dice(NUMMAGIC < 16 ? NUMMAGIC : 16) - 1;
							t = pow(2., (double)i);
						} while(!(PLAYER.Spell & t));
						PLAYER.Spell -= t;
						sprintf(outbuf, "your %s of %s!", (ONLINE->user.MyMagic == 1 ? "wand" : "scroll"), MAGIC(i)->Name);
					}
					else
					if(PLAYER.Poison && dice(ONLINE->CHA / 10 + nest + 1) - 1 <= nest / 2) {
						do {
							i = dice(NUMPOISON) - 1;
							t = pow(2., (double)i);
						} while(!(PLAYER.Poison & t));
						PLAYER.Poison -= t;
						sprintf(outbuf, "your vial of %s!", POISON(i)->Name);
					}
					else {
						strcpy(line[0], money(PLAYER.Gold, 0));
						if((s = strchr(line[0],','))) {
							*s = '\0';
							if(dice(100) < ONLINE->CHA / 2)
								strcpy(line[0], s + 1);
						}
						gold(&d, line[0]);
						PLAYER.Gold -= d;
						strcpy(line[0], money(d, 0));
						if(d)
							c = *(line[0] + strlen(line[0]) - 1);
						else
							c='\0';
						sprintf(outbuf, "your pouch of %s pieces!",(c == 'c' ? "copper" : c == 's' ? "silver" : c == 'g' ? "gold" : c == 'p' ? "platinum" : "Reese's"));
					}
					OUT(outbuf); NL;
					Delay(25); NL;
				}
				else {
					sprintf(outbuf, "%sYou surprise him!%s", fore(WHITE), fore(GRY));
					OUT(outbuf); NL;
					Delay(25);
					OUT("He vanishes into thin air."); NL;
					sound("teleport", 64, 1);
					Delay(25); NL;
					ROOM(dl, hx, hy)->occupant = FALSE;
				}
				break;
			case DEEP_DANK_DUNGEON:
				if(nest < 10 && nest < PLAYER.Immortal && PLAYER.Novice != 'Y') {
					OUT(fore(BLUE));
					OUT("You've found a portal to a deep, dank dungeon."); NL;
					NORMAL;
					Delay(25);
					c = 'Y';
					if(dice(90 + 10 * nest) < ONLINE->DEX) {
						do {
							sprintf(prompt, "%sDescend even deeper (Y/N)? ", fore(CYN));
							OUT(prompt);
							c = inkey('Y','N');
							NL;
						} while(c != 'Y' && c != 'N');
					}
					ROOM(dl, hx, hy)->occupant = FALSE;
					if(c == 'Y') {
						NUMWALK(dl) *= 2;
						mymove = TRUE;
						sprintf(outbuf, "%sYou vanish into the other dungeon...", fore(WHITE));
						OUT(outbuf);
						sound("teleport", 64, 0);
						Delay(100);
						Dungeon(dl);
						if(LEVEL(dl)->room == NULL)
							GenLevel();
						renderdmap();
						hx = dice(LEVEL(dl)->MaxCol) - 1;
						hy = dice(LEVEL(dl)->MaxRow) - 1;
						fx = hx; fy = hy;
						escape = FALSE;
						mymove = TRUE;
					}
				}
				break;
			case WELL:
				sound2("OFF", 1);
				clrmap();
				Delay(50);
				OUT(fore(MAGENTA));
				OUT("You have found a legendary Wishing Well."); NL; NL;
				sound("well", 64, 0);
				emptykey();
				OUT(fore(YELLOW));
				OUT("What do you wish to do?"); NL; NL;
				NORMAL;
				sprintf(outbuf, "%s<%sB%s>%s Bless yourself", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
				OUT(outbuf); NL;
				if(nest > 7) {
					sprintf(outbuf, "%s<%sC%s>%s Curse another", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest < 10) {
					sprintf(outbuf, "%s<%sD%s>%s Delve deeper into the dank dungeon", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest > 4) {
					sprintf(outbuf, "%s<%sF%s>%s Fix all your damage", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest > 2) {
					sprintf(outbuf, "%s<%sG%s>%s Grant another call", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest > 5) {
					sprintf(outbuf, "%s<%sK%s>%s Key hint(s)", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest > 3) {
					sprintf(outbuf, "%s<%sL%s>%s Loot another player's money", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest > 6) {
					sprintf(outbuf, "%s<%sM%s>%s Magical spell(s) or device(s)", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				sprintf(outbuf, "%s<%sO%s>%s Teleport all the way out", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
				OUT(outbuf); NL;
				if(nest > 1) {
					sprintf(outbuf, "%s<%sR%s>%s Resurrect all the dead players", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				sprintf(outbuf, "%s<%sT%s>%s Teleport to another level", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
				OUT(outbuf); NL;
				if(nest > 8) {
					sprintf(outbuf, "%s<%sW%s>%s Wreak havoc with players", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				if(nest > 9) {
					sprintf(outbuf, "%s<%sX%s>%s Exterminate another player", fore(BLACK), fore(WHITE), fore(BLACK), fore(CYN));
					OUT(outbuf); NL;
				}
				switch(nest) {
					case 1:
						s = "BDOT";
						break;
					case 2:
						s = "BDORT";
						break;
					case 3:
						s = "BDGORT";
						break;
					case 4:
						s = "BDGLORT";
						break;
					case 5:
						s = "BDFGLORT";
						break;
					case 6:
						s = "BDFGKLORT";
						break;
					case 7:
						s = "BDFGKLMORT";
						break;
					case 8:
						s = "BCDFGKLMORT";
						break;
					case 9:
						s = "BCDFGKLMORTW";
						break;
					case 10:
						s = "BCFGKLMORTWX";
						break;
				}
				do {
					NL;
					sprintf(prompt, "%sWhat is thy bidding, my master? ", fore(CYN));
					OUT(prompt);
					c = inkey('\0', 'C');
				} while(!strchr(s, c));
				NL;
				ROOM(dl, hx, hy)->occupant = FALSE;
				switch(c) {
					case 'B':
						sound("shimmer", 64, 0);
						if(strlen(PLAYER.Cursed)) {
							memset(PLAYER.Cursed, 0, sizeof(PLAYER.Cursed));
							sprintf(outbuf, "%sYou feel the %sblack%s cloud leave you.", fore(GRY), fore(BLACK), fore(GRY));
							sprintf(outbuf, "%sremoved a cursed", fore(MAGENTA));
							news(outbuf);
						}
						else {
							strcpy(PLAYER.Blessed, "wish");
							sprintf(outbuf, "%sYou feel a shining aura surround you.%s", fore(YELLOW), fore(GRY));
							sprintf(outbuf, "%swished for a blessing", fore(MAGENTA));
							news(outbuf);
						}
						if((ONLINE->STR += 10) > 100)
							ONLINE->STR = 100;
						if((ONLINE->INT += 10) > 100)
							ONLINE->INT = 100;
						if((ONLINE->DEX += 10) > 100)
							ONLINE->DEX = 100;
						if((ONLINE->CHA += 10) > 100)
							ONLINE->CHA = 100;
						OUT(outbuf); NL;
						break;
					case 'C':
						sprintf(prompt, "%sCurse user? ", fore(CYN));
						if((GetRPC(RPC[1][0]) && strncmp(PLAYER.Gang, ENEMY.Gang, 20) != 0) || (ACCESS(PLAYER.Access)->Sysop == 'Y')) {
							sprintf(outbuf, "%sYou send a %sblack%s cloud to %s.", fore(GRY), fore(BLACK), fore(GRY), ENEMY.Handle);
							OUT(outbuf); NL;
							memset(ENEMY.Blessed, 0, sizeof(ENEMY.Blessed));
							strcpy(ENEMY.Cursed, PLAYER.ID);
							RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
							numline = 0;
							sprintf(line[numline++], "%s cursed you!", PLAYER.Handle);
							note(ENEMY.ID);
							sprintf(outbuf, "%scursed %s, a level %u %s", fore(MAGENTA), ENEMY.Handle, ENEMY.Level, CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						else {
							sprintf(outbuf, "%sA %sblack%s cloud appears and hovers over YOU!", fore(GRY), fore(BLACK), fore(GRY));
							OUT(outbuf); NL;
							memset(PLAYER.Blessed, 0, sizeof(PLAYER.Blessed));
							strcpy(PLAYER.Cursed, "well");
							if((ONLINE->STR -= 10) < 1)
								ONLINE->STR = 0;
							if((ONLINE->INT -= 10) < 1)
								ONLINE->INT = 0;
							if((ONLINE->DEX -= 10) < 1)
								ONLINE->DEX = 0;
							if((ONLINE->CHA -= 10) < 1)
								ONLINE->CHA = 0;
						}
						sound("morph", 64, 0);
						break;
					case 'D':
						mymove = TRUE;
						if(nest < 10 && nest < PLAYER.Immortal && PLAYER.Novice != 'Y') {
							int dl2, hx2, hy2, fx2, fy2;
							dl2 = dl;
							hx2 = hx; hy2 = hy;
							fx2 = fx; fy2 = fy;
							sound("teleport", 64, 0);
							Dungeon(dl);
							dl = dl2;
							hx = hx2; hy = hy2;
							fx = fx2; fy = fy2;
							escape = FALSE;
							mymove = TRUE;
							renderdmap();
						}
						else {
							OUT("A deep laughter bellows..."); NL;
							sound("morph", 64, 0);
						}
						break;
					case 'F':
						if(ONLINE->STR < PLAYER.STR)
							ONLINE->STR = PLAYER.STR;
						if(ONLINE->INT < PLAYER.INT)
							ONLINE->INT = PLAYER.INT;
						if(ONLINE->DEX < PLAYER.DEX)
							ONLINE->DEX = PLAYER.DEX;
						if(ONLINE->CHA < PLAYER.CHA)
							ONLINE->CHA = PLAYER.CHA;
						if(PLAYER.ACmod < 0)
							PLAYER.ACmod = 0;
						if(PLAYER.WCmod < 0)
							PLAYER.WCmod = 0;
						if(ONLINE->ToAC < 0)
							ONLINE->ToAC = 0;
						if(ONLINE->ToWC < 0)
							ONLINE->ToWC = 0;
						if(ONLINE->HP < PLAYER.HP)
							ONLINE->HP = PLAYER.HP;
						if(ONLINE->SP < PLAYER.SP)
							ONLINE->SP = PLAYER.SP;
						if(ONLINE->HULL < PLAYER.Hull)
							ONLINE->HULL = PLAYER.Hull;
						OUT("You are completely healed and all damage has been repaired."); NL;
						sound("shimmer", 64, 0);
						break;
					case 'G':
						if(PLAYER.CallsToday) {
							PLAYER.CallsToday--;
							OUT("You are granted another call for the day.");
							sprintf(outbuf, "%swished for an extra call", fore(MAGENTA));
							news(outbuf);
							sound("shimmer", 64, 0);
						}
						else {
							OUT("A deep laughter bellows...");
							sound("morph", 64, 0);
						}
						NL;
						break;
					case 'K':
						if((n = dice(nest)) > 3)
							n = 3;
						for(i = 0; i < n; i++) {
							keyhint();
							sound("shimmer", 64, 0);
							Delay(100);
						}
						break;
					case 'L':
						sprintf(prompt, "%sLoot which user? ", fore(CYN));
						for(i = 0; !GetRPC(RPC[1][0]) && i < nest; i++);
						if(strlen(ENEMY.ID)) {
							d = ENEMY.Gold + ENEMY.Bank;
							sprintf(outbuf, "You steal %s from %s.", money(d, ANSI), ENEMY.Handle);
							OUT(outbuf); NL;
							sound("max", 64, 0);
							PLAYER.Gold += d;
							ENEMY.Gold = 0.;
							ENEMY.Bank = 0.;
							RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
							numline = 0;
							sprintf(line[numline++], "%s wished for your %s.", PLAYER.Handle, money(d, ANSI));
							note(ENEMY.ID);
							sprintf(outbuf, "%slooted %s, a level %u %s", fore(MAGENTA), ENEMY.Handle, ENEMY.Level, CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						break;
					case 'M':
						n = dice(NUMMAGIC / 2) + dice(nest);
						for(i = 0; i < n; i++) {
							j = 0;
							if(i == 0) {
								for(m = 0; m < (NUMMAGIC < 16 ? NUMMAGIC : 16); m++)
									if(!(PLAYER.Spell & 1 << m)) {
										j = m + 1;
										PLAYER.Spell |= 1 << m;
										break;
									}
								if(!j)
									for(m = 16; m < NUMMAGIC; m++)
										if(!(PLAYER.XSpell & 1 << (m - 16))) {
											j = m + 1;
											PLAYER.XSpell |= 1 << (m - 16);
											break;
										}
							}
							else {
								m = dice(NUMMAGIC) - 1;
								if(m < 16) {
									if(!(PLAYER.Spell & 1 << m)) {
										j = m + 1;
										PLAYER.Spell |= 1 << m;
									}
								}
								else {
									if(!(PLAYER.XSpell & 1 << (m - 16))) {
										j = m + 1;
										PLAYER.XSpell |= 1 << (m - 16);
									}
								}
							}
							switch(j ? ONLINE->user.MyMagic : 0) {
								case 1:
									sound("max", 64, 0);
									sprintf(outbuf, "A Wand of %s appears in your hand.", MAGIC(j - 1)->Name);
									break;
								case 2:
									sound("max", 64, 0);
									sprintf(outbuf, "A Scroll of %s appears in your hand.", MAGIC(j - 1)->Name);
									break;
								case 3:
									sound("shimmer", 64, 0);
									sprintf(outbuf, "The Spell of %s is revealed to you.", MAGIC(j - 1)->Name);
									break;
								case 4:
									sound("shimmer", 64, 0);
									sprintf(outbuf, "The Spell of %s is revealed to you.", MAGIC(j - 1)->Name);
									break;
								default:
									if(i)
										strcpy(outbuf, "");
									else {
										strcpy(outbuf, "A deep laughter bellows.");
										sound("morph", 64, 0);
									}
									break;
							}
							if(strlen(outbuf)) {
								OUT(outbuf); NL;
								Delay(100);
							}
						}
						break;
					case 'O':
						sound("teleport", 64, 0);
						escall = TRUE;
						mymove = FALSE;
						break;
					case 'R':
						sound("resurrect", 64, 0);
						numline = 0;
						sprintf(line[numline++], "%s wished you back from the dead.", PLAYER.Handle);
						strcpy(userlevel.Current, "999");
						while(RPGserver(SERVER_GETUSERLEVEL, (UBYTE *)&userlevel)) {
							strcpy(ENEMY.ID, userlevel.ID);
							RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY);
							if(strlen(ENEMY.Status) && strcmp(ENEMY.Status,  "jail")) {
								sprintf(outbuf, "Resurrecting %s...",  ENEMY.Handle);
								OUT(outbuf);
								memset(ENEMY.Status, 0, sizeof(ENEMY.Status));
								RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
								note(ENEMY.ID);
								NL;
							}
						}
						sprintf(outbuf, "%swished all the dead resurrected", fore(MAGENTA));
						news(outbuf);
						break;
					case 'T':
						if((i = dl * 2 / 3 - dice(nest)) < 1)
							i = 1;
						if((j = dl * 3 / 2 + dice(nest)) > 100)
							j = 100;
						do {
							sprintf(prompt, "%sLevel (%d-%d): ", fore(CYN), i, j);
							OUT(prompt);
							if(ins(3))
								break;
							NL;
							dl = atoi(inbuf);
						} while(dl < i || dl > j);
						dl--;
						if(LEVEL(dl)->room) {
							renderdmap();
							hx = dice(LEVEL(dl)->MaxCol) - 1;
							hy = dice(LEVEL(dl)->MaxRow) - 1;
							fx = hx; fy = hy;
						}
						mymove = TRUE;
						sound("teleport", 64, 0);
						break;
					case 'W':
						sound("morph", 64, 0);
						strcpy(userlevel.Current, "999");
						i = dice(nest);
						j = 0;
						while(i > 0 && RPGserver(SERVER_GETUSERLEVEL, (UBYTE *)&userlevel)) {
							strcpy(ENEMY.ID, userlevel.ID);
							RPGserver(SERVER_GETUSER, (UBYTE *)&ENEMY);
							if(ENEMY.ID[0] != '_' && strncmp(ENEMY.ID, PLAYER.ID, sizeof(PLAYER.ID)) && dice(ENEMY.Level + nest) > nest) {
								sprintf(outbuf, "%s... ", ENEMY.Handle);
								OUT(outbuf);
								Delay(25);
								numline = 0;
								sprintf(line[numline++], "%s wreaked havoc on you!", PLAYER.Handle);
								switch(dice(3)) {
									case 1:
										OUT("got impoverished!");
										ENEMY.Bank = 0.;
										if(ENEMY.RealEstate)
											ENEMY.RealEstate--;
										if(ENEMY.Security)
											ENEMY.Security--;
										strcpy(line[numline++], "You were impoverished.");
										break;
									case 2:
										OUT("lost their boat!");
										ENEMY.Hull = 0;
										ENEMY.Cannon = 0;
										ENEMY.Ram = 'N';
										strcpy(line[numline++], "You lost your boat.");
										break;
									case 3:
										OUT("lost their spells & poisons!");
										ENEMY.Spell = 0;
										ENEMY.XSpell = 0;
										ENEMY.Poison = 0;
										strcpy(line[numline++], "You lost any spells & poisons.");
										break;
								}
								RPGserver(SERVER_PUTUSER, (UBYTE *)&ENEMY);
								note(ENEMY.ID);
								NL;
								Delay(25);
								i--; j++;
							}
						}
						sprintf(outbuf, "%swreaked havoc on %d adventurers!", fore(GREEN), j);
						news(outbuf);
						break;
					case 'X':
						sprintf(prompt, "%sExterminate which user? ", fore(CYN));
						for(i = 0; !GetRPC(RPC[1][0]) && i < nest; i++);
						if(strlen(ENEMY.ID)) {
							sound("morph", 64, 0);
							sprintf(outbuf, "%sexterminated %s, a level %u %s", fore(MAGENTA), ENEMY.Handle, ENEMY.Level, CLASS(RPC[1][0])->Name);
							news(outbuf);
							ReKeySequence(&ENEMY);
							reroll(&ENEMY);
							RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
							numline = 0;
							sprintf(line[numline++], "%s exterminated you!", PLAYER.Handle);
							note(ENEMY.ID);
							if (ENEMY.Gender != 'I') {
								PLAYER.MyMaxCHA -= (PLAYER.MyMaxCHA > 2 ? 2 : 0);
								PLAYER.CHA -= (PLAYER.CHA > 5 ? 5 : 0);
								ONLINE->CHA -= (ONLINE->CHA > 10 ? 10 : 0);
								if (strncmp(PLAYER.Gang, ENEMY.Gang, 20) == 0) {
									PLAYER.MyMaxCHA -= (PLAYER.MyMaxCHA > 3 ? 3 : 0);
									PLAYER.CHA -= (PLAYER.CHA > 5 ? 5 : 0);
									ONLINE->CHA -= (ONLINE->CHA > 10 ? 10 : 0);
								}
							}
						}
						break;
				}
				NL; NL;
				break;
			case WHEEL:
				sound2("OFF", 1);
				clrmap();
				Delay(50);
				OUT(fore(MAGENTA));
				OUT("You have found a Mystical Wheel of Life."); NL; NL;
				sound("wol", 64, 0);
				Delay(50);
				switch(dice(5)) {
					case 1:
						v = "cryptic";
						break;
					case 2:
						v = "familiar";
						break;
					case 3:
						v = "foreign";
						break;
					case 4:
						v = "speaking out";
						break;
					case 5:
						v = "strange";
						break;
				}
				sprintf(outbuf, "%sThe runes are %s to you.", fore(YELLOW), v);
				OUT(outbuf); NL; NL;
				Delay(50);
				do {
					sprintf(prompt, "%sWill you spin it (Y/N)? ", fore(CYN));
					OUT(prompt);
					c = inkey('N', 'N');
					NL;
				} while(c != 'Y' && c != 'N');
				NL;
				if(c == 'Y') {
					z = (nest < 3) ? 4 : (nest < 6) ? 6 : (nest < 9) ? 8 : 9;
					for(i = 0, j = 10, t = 0; i < 5; i++, j += 5) {
						n = ONLINE->STR / 5 - 5 * i + dice(5) + 1;
						for(m = 0; m < n; m++) {
							t = dice(z);
							beep();
							sprintf(outbuf, "\15%c", spin[m % 4]);
							OUT(outbuf);
							Delay(j);
						}
					}
					n = dice(ONLINE->STR / 20) + 2;
					for(i = 1; i <= n; i++) {
						t = dice(z);
						if(ACCESS(PLAYER.Access)->Sysop == 'Y' && i == n) {
							t = dice(9);
							t -= t % 2;
						}
						beep();
						sprintf(outbuf, "%s[%s%s%s] \15", fore(BLU), fore(slotcol[t % 6]), wol[t % z], fore(BLU));
						OUT(outbuf);
						Delay(25 * i);
					}
					NORMAL;
					switch(t % z) {
						case 0:
							if(strlen(PLAYER.Cursed)) {
								memset(PLAYER.Cursed, 0, sizeof(PLAYER.Cursed));
								if((ONLINE->STR += 10) > PLAYER.MyMaxSTR)
									ONLINE->STR = PLAYER.MyMaxSTR;
								if((ONLINE->INT += 10) > PLAYER.MyMaxINT)
									ONLINE->INT = PLAYER.MyMaxINT;
								if((ONLINE->DEX += 10) > PLAYER.MyMaxDEX)
									ONLINE->DEX = PLAYER.MyMaxDEX;
								if((ONLINE->CHA += 10) > PLAYER.MyMaxCHA)
									ONLINE->CHA = PLAYER.MyMaxCHA;
							}
							else {
								if(PLAYER.MyMaxSTR < 100) PLAYER.MyMaxSTR++;
								if(PLAYER.MyMaxINT < 100) PLAYER.MyMaxINT++;
								if(PLAYER.MyMaxDEX < 100) PLAYER.MyMaxDEX++;
								if(PLAYER.MyMaxCHA < 100) PLAYER.MyMaxCHA++;
								if((PLAYER.STR += 20) > PLAYER.MyMaxSTR)
									PLAYER.STR = PLAYER.MyMaxSTR;
								if(PLAYER.STR > ONLINE->STR)
									ONLINE->STR = PLAYER.STR;
								if((PLAYER.INT += 20) > PLAYER.MyMaxINT)
									PLAYER.INT = PLAYER.MyMaxINT;
								if(PLAYER.INT > ONLINE->INT)
									ONLINE->INT = PLAYER.INT;
								if((PLAYER.DEX += 20) > PLAYER.MyMaxDEX)
									PLAYER.DEX = PLAYER.MyMaxDEX;
								if(PLAYER.DEX > ONLINE->DEX)
									ONLINE->DEX = PLAYER.DEX;
								if((PLAYER.CHA += 20) > PLAYER.MyMaxCHA)
									PLAYER.CHA = PLAYER.MyMaxCHA;
								if(PLAYER.CHA > ONLINE->CHA)
									ONLINE->CHA = PLAYER.CHA;
							}
							break;
						case 1:
							if(strlen(PLAYER.Blessed)) {
								memset(PLAYER.Blessed, 0, sizeof(PLAYER.Blessed));
								if((ONLINE->STR -= 10) < PLAYER.STR)
									ONLINE->STR = PLAYER.STR;
								if((ONLINE->INT -= 10) < PLAYER.INT)
									ONLINE->INT = PLAYER.INT;
								if((ONLINE->DEX -= 10) < PLAYER.DEX)
									ONLINE->DEX = PLAYER.DEX;
								if((ONLINE->CHA -= 10) < PLAYER.CHA)
									ONLINE->CHA = PLAYER.CHA;
							}
							else {
								if(PLAYER.MyMaxSTR > 0) PLAYER.MyMaxSTR--;
								if(PLAYER.MyMaxINT > 0) PLAYER.MyMaxINT--;
								if(PLAYER.MyMaxDEX > 0) PLAYER.MyMaxDEX--;
								if(PLAYER.MyMaxCHA > 0) PLAYER.MyMaxCHA--;
								if((BYTE)(PLAYER.STR -= 20) < 0)
									PLAYER.STR = 0;
								if((BYTE)(ONLINE->STR -= 20) < 0)
									ONLINE->STR = 0;
								if((BYTE)(PLAYER.INT -= 20) < 0)
									PLAYER.INT = 0;
								if((BYTE)(ONLINE->INT -= 20) < 0)
									ONLINE->INT = 0;
								if((BYTE)(PLAYER.DEX -= 20) < 0)
									PLAYER.DEX = 0;
								if((BYTE)(ONLINE->DEX -= 20) < 0)
									ONLINE->DEX = 0;
								if((BYTE)(PLAYER.CHA -= 20) < 0)
									PLAYER.CHA = 0;
								if((BYTE)(ONLINE->CHA -= 20) < 0)
									ONLINE->CHA = 0;
							}
							break;
						case 2:
							JOUST(dl);
							d += value(WEAPON(ONLINE)->Value, ONLINE->CHA);
							d += value(ARMOR(ONLINE)->Value, ONLINE->CHA);
							d *= dl + 1;
							PLAYER.Gold += d;
							break;
						case 3:
							PLAYER.Gold = 0.;
							PLAYER.Bank = 0.;
							JOUST(dl);
							d += value(WEAPON(ONLINE)->Value, ONLINE->CHA);
							d += value(ARMOR(ONLINE)->Value, ONLINE->CHA);
							d *= dl + 1;
							PLAYER.Loan += d;
							break;
						case 4:
							ONLINE->HP += PLAYER.HP / 2 + dice(PLAYER.HP / 2);
							ONLINE->SP += PLAYER.SP / 2 + dice(PLAYER.SP / 2);
							PLAYER.WCmod += dice(WEAPON(ONLINE)->Class);
							ONLINE->ToWC += WEAPON(ONLINE)->Class / 2 + 1;
							PLAYER.ACmod += dice(ARMOR(ONLINE)->Class);
							ONLINE->ToAC += ARMOR(ONLINE)->Class / 2 + 1;
							sound("hone", 64, 0);
							break;
						case 5:
							ONLINE->HP = 0;
							ONLINE->SP = 0;
							sound("killed", 64, 1);
							logoff = TRUE;
							strcpy(reason, "Wheel of Death");
							break;
						case 6:
							keyhint();
							sound("shimmer", 64, 0);
							Delay(100);
							break;
						case 7:
							sound("morph", 64, 0);
							n = 0;
							for(i = 0; i < NUMCLASS; i++)
								n += MAXCLASS(i);
							n = dice(n) - 1;
							for(i = 0; i < NUMCLASS && n >= MAXCLASS(i); i++)
								n -= MAXCLASS(i);
							ONLINE->class_origin = i;
							ONLINE->class_type = n;
							sprintf(ONLINE->user.Class, "%s.%s", table->class[ONLINE->class_origin]->Origin, CLASS(ONLINE)->Name);
							ONLINE->user.Level = 0;
							ONLINE->user.Gender = (dice(2) == 1) ? 'M' : 'F';
							CreateRPC(ONLINE);
							sprintf(outbuf, "%s%s was morphed into a level %u %s!", fore(YELLOW), ONLINE->user.Handle, ONLINE->user.Level, CLASS(ONLINE)->Name);
							news(outbuf);
							sprintf(outbuf, "You morph yourself into a level %u %s!", PLAYER.Level, CLASS(ONLINE)->Name);
							NL;
							OUT(outbuf); NL;
							break;
						case 8:
							sound("level", 64, 0);
							skillplus();
							break;
					}
				}
				NL;
				Delay(50);
				ROOM(dl, hx, hy)->occupant = FALSE;
				break;
		}
	if(!cd() && !escape && LEVEL(dl)->room) {
		if(mymove) {
			paused();
			drawmap();
		}
		else {
			switch(ROOM(dl, hx, hy)->gift_type) {
				case GIFT_VIAL:
					sound("bubbles", 64, 0);
					if(ONLINE->user.MyMagic < 2)
						switch(ROOM(dl, hx, hy)->gift_value) {
							case 2:
								ROOM(dl, hx, hy)->gift_value = 0;
								break;
							case 3:
								ROOM(dl, hx, hy)->gift_value = 1;
								break;
						}
					switch(ROOM(dl, hx, hy)->gift_value) {
						case 0:
							v = "Vial of Slaad Secretions";
							break;
						case 1:
							v = "Potion of Cure Light Wounds";
							break;
						case 2:
							v = "Flask of Fire Water";
							break;
						case 3:
							v = "Potion of Mana";
							break;
						case 4:
							v = "Vial of Weakness";
							break;
						case 5:
							v = "Potion of Stamina";
							break;
						case 6:
							v = "Vial of Stupidity";
							break;
						case 7:
							v = "Potion of Wisdom";
							break;
						case 8:
							v = "Vial of Clumsiness";
							break;
						case 9:
							v = "Potion of Agility";
							break;
						case 10:
							v = "Vile Vial";
							break;
						case 11:
							v = "Potion of Charm";
							break;
						case 12:
							v = "Vial of Crack";
							break;
						case 13:
							v = "Potion of Augment";
							break;
						case 14:
							v = "Beaker of Death";
							break;
						case 15:
							v = "Elixir of Restoration";
							break;
					}
					sprintf(outbuf, "%sOn the ground, you find ", fore(CYAN));
					OUT(outbuf);
					if(PLAYER.Novice != 'Y' && (ROOM(dl, hx, hy)->gift_id == 2 || (ROOM(dl, hx, hy)->gift_id == 0 && dice(100) > (int)(ONLINE->INT / 20 * (1 << ONLINE->user.MyPoison) + (ONLINE->INT > 90 ? 2 * (ONLINE->INT % 90) : 0))))) {
						ROOM(dl, hx, hy)->gift_id = 2;
						switch(dice(3)) {
							case 1:
								s = "a bottle containing ";
								break;
							case 2:
								s = "a flask of some ";
								break;
							case 3:
								s = "a vial holding ";
								break;
						}
						OUT(s);
						switch(dice(5)) {
							case 1:
								s = "bubbling";
								break;
							case 2:
								s = "clear";
								break;
							case 3:
								s = "dark";
								break;
							case 4:
								s = "sparkling";
								break;
							case 5:
								s = "tainted";
								break;
						}
						OUT(s);
						OUT(", ");
						switch(dice(5)) {
							case 1:
								s = "amber";
								break;
							case 2:
								s = "blue";
								break;
							case 3:
								s = "crimson";
								break;
							case 4:
								s = "green";
								break;
							case 5:
								s = "purple";
								break;
						}
						OUT(s);
						OUT(" potion");
					}
					else {
						ROOM(dl, hx, hy)->gift_id = 1;
						s = v; v = NULL;
						OUT(s);
					}
					OUT("."); NL;
					if(dice(100) + nest < (50 + ONLINE->INT / 2)) {
						sprintf(prompt, "%sWill you drink it (Yes/No/Toss)? ", fore(CYN));
						do {
							OUT(prompt);
							c = inkey('Y','N');
							NL;
						} while(c!='Y' && c!='N' && c!='T');
					}
					else {
						Delay(25);
						OUT("You quaff it without hesitation."); NL;
						c = 'Y';
					}
					if(c == 'T' || c == 'Y') {
						ROOM(dl, hx, hy)->gift_type = 0;
						if(v) {
							s = v;
							sprintf(outbuf, "It was %s%s.", AN(s), s);
							OUT(outbuf); NL; NL;
						}
					}
					if(c == 'Y') {
						sound("quaff", 64, 0);
						Delay(25);
						switch(ROOM(dl, hx, hy)->gift_value) {
							case 0:
								sound("hurt", 64, 0);
								ONLINE->HP -= dice(PLAYER.HP / 2);
								if(ONLINE->HP < 1) {
									logoff = TRUE;
									sprintf(reason, "quaffed %s%s", AN(s), s);
								}
								break;
							case 1:
								sound("yum", 64, 0);
								ONLINE->HP += dice(PLAYER.HP - ONLINE->HP);
								break;
							case 2:
								ONLINE->SP -= dice(ONLINE->SP);
								break;
							case 3:
								sound("shimmer", 64, 0);
								ONLINE->SP += dice(PLAYER.SP - ONLINE->SP);
								break;
							case 4:
								if(PLAYER.STR > 0 && ONLINE->STR < 20)
									PLAYER.STR--;
								else
									ONLINE->STR -= dice(10);
								break;
							case 5:
								ONLINE->STR += dice(10);
								if(ONLINE->STR > 100) {
									ONLINE->STR = 100;
									PLAYER.STR++;
									if(PLAYER.STR > 100)
										PLAYER.STR = 100;
								}
								break;
							case 6:
								if(PLAYER.INT > 0 && ONLINE->INT < 20)
									PLAYER.INT--;
								else
									ONLINE->INT -= dice(10);
								break;
							case 7:
								ONLINE->INT += dice(10);
								if(ONLINE->INT > 100) {
									ONLINE->INT = 100;
									PLAYER.INT++;
									if(PLAYER.INT > 100)
										PLAYER.INT = 100;
								}
								break;
							case 8:
								if(PLAYER.DEX > 0 && ONLINE->DEX < 20)
									PLAYER.DEX--;
								else
									ONLINE->DEX -= dice(10);
								break;
							case 9:
								ONLINE->DEX += dice(10);
								if(ONLINE->DEX > 100) {
									ONLINE->DEX = 100;
									PLAYER.DEX++;
									if(PLAYER.DEX > 100)
										PLAYER.DEX = 100;
								}
								break;
							case 10:
								if(PLAYER.CHA > 0 && ONLINE->CHA < 20)
									PLAYER.CHA--;
								else
									ONLINE->CHA -= dice(10);
								break;
							case 11:
								ONLINE->CHA += dice(10);
								if(ONLINE->CHA > 100) {
									ONLINE->CHA = 100;
									PLAYER.CHA++;
									if(PLAYER.CHA > 100)
										PLAYER.CHA = 100;
								}
								break;
							case 12:
								PLAYER.MyMaxSTR -= (PLAYER.MyMaxSTR > 70) ? dice(5) : 1;
								PLAYER.MyMaxINT -= (PLAYER.MyMaxINT > 70) ? dice(5) : 1;
								PLAYER.MyMaxDEX -= (PLAYER.MyMaxDEX > 70) ? dice(5) : 1;
								PLAYER.MyMaxCHA -= (PLAYER.MyMaxCHA > 70) ? dice(5) : 1;
								PLAYER.STR -= (PLAYER.STR > 20) ? 5 + dice(5) : 1;
								PLAYER.INT -= (PLAYER.INT > 20) ? 5 + dice(5) : 1;
								PLAYER.DEX -= (PLAYER.DEX > 20) ? 5 + dice(5) : 1;
								PLAYER.CHA -= (PLAYER.CHA > 20) ? 5 + dice(5) : 1;
								ONLINE->STR -= (ONLINE->STR > 20) ? 10 + dice(10) : (ONLINE->STR > 0) ? 1 : 0;
								ONLINE->INT -= (ONLINE->INT > 20) ? 10 + dice(10) : (ONLINE->INT > 0) ? 1 : 0;
								ONLINE->DEX -= (ONLINE->DEX > 20) ? 10 + dice(10) : (ONLINE->DEX > 0) ? 1 : 0;
								ONLINE->CHA -= (ONLINE->CHA > 20) ? 10 + dice(10) : (ONLINE->CHA > 0) ? 1 : 0;
								break;
							case 13:
								PLAYER.MyMaxSTR += (PLAYER.MyMaxSTR < 95) ? dice(5) : (PLAYER.MyMaxSTR < 100) ? 1 : 0;
								PLAYER.MyMaxINT += (PLAYER.MyMaxINT < 95) ? dice(5) : (PLAYER.MyMaxINT < 100) ? 1 : 0;
								PLAYER.MyMaxDEX += (PLAYER.MyMaxDEX < 95) ? dice(5) : (PLAYER.MyMaxDEX < 100) ? 1 : 0;
								PLAYER.MyMaxCHA += (PLAYER.MyMaxCHA < 95) ? dice(5) : (PLAYER.MyMaxCHA < 100) ? 1 : 0;
								PLAYER.STR += (PLAYER.STR < PLAYER.MyMaxSTR) ? dice(PLAYER.MyMaxSTR - PLAYER.STR) : 0;
								PLAYER.INT += (PLAYER.INT < PLAYER.MyMaxINT) ? dice(PLAYER.MyMaxINT - PLAYER.INT) : 0;
								PLAYER.DEX += (PLAYER.DEX < PLAYER.MyMaxDEX) ? dice(PLAYER.MyMaxDEX - PLAYER.DEX) : 0;
								PLAYER.CHA += (PLAYER.CHA < PLAYER.MyMaxCHA) ? dice(PLAYER.MyMaxCHA - PLAYER.CHA) : 0;
								ONLINE->STR += (ONLINE->STR < 100) ? dice(100 - ONLINE->STR) : 0;
								ONLINE->INT += (ONLINE->INT < 100) ? dice(100 - ONLINE->INT) : 0;
								ONLINE->DEX += (ONLINE->DEX < 100) ? dice(100 - ONLINE->DEX) : 0;
								ONLINE->CHA += (ONLINE->CHA < 100) ? dice(100 - ONLINE->CHA) : 0;
								break;
							case 14:
								ONLINE->HP = 0;
								ONLINE->SP = 0;
								sound("killed", 64, 1);
								logoff = TRUE;
								sprintf(reason, "quaffed a %s", s);
								break;
							case 15:
								if(ONLINE->HP < PLAYER.HP)
									ONLINE->HP = PLAYER.HP;
								if(ONLINE->SP < PLAYER.SP)
									ONLINE->SP = PLAYER.SP;
								break;
						}
					}
					break;
				case GIFT_POISON:
					if(!(PLAYER.Poison & 1 << ROOM(dl, hx, hy)->gift_value)) {
						PLAYER.Poison |= 1 << ROOM(dl, hx, hy)->gift_value;
						sprintf(outbuf, "%sYou find a vial of %s!", fore(YELLOW), POISON(ROOM(dl, hx, hy)->gift_value)->Name);
						OUT(outbuf); NL; NL;
						ROOM(dl,hx,hy)->gift_type = 0;
					}
					break;
				case GIFT_MAGIC:
					if(!(PLAYER.Spell & 1 << ROOM(dl, hx, hy)->gift_value)) {
						PLAYER.Spell |= 1 << ROOM(dl, hx, hy)->gift_value;
						if(ONLINE->user.MyMagic == 1)
							sprintf(outbuf, "%sYou find a Wand of %s!", fore(YELLOW), MAGIC(ROOM(dl, hx, hy)->gift_value)->Name);
						else
							sprintf(outbuf, "%sYou find a %s scroll!", fore(YELLOW), MAGIC(ROOM(dl, hx, hy)->gift_value)->Name);
						OUT(outbuf); NL; NL;
						ROOM(dl,hx,hy)->gift_type = 0;
					}
					break;
				case GIFT_XMAGIC:
					if(!(PLAYER.XSpell & 1 << ROOM(dl, hx, hy)->gift_value)) {
						PLAYER.XSpell |= 1 << ROOM(dl, hx, hy)->gift_value;
						if(ONLINE->user.MyMagic == 1)
							sprintf(outbuf, "%sYou find a Wand of %s!", fore(YELLOW), MAGIC(16 + ROOM(dl, hx, hy)->gift_value)->Name);
						else
							sprintf(outbuf, "%sYou find a %s scroll!", fore(YELLOW), MAGIC(16 + ROOM(dl, hx, hy)->gift_value)->Name);
						OUT(outbuf); NL; NL;
						ROOM(dl,hx,hy)->gift_type = 0;
					}
					break;
				case GIFT_CHEST:
					JOUST(dl);
					d += value(WEAPON(ONLINE)->Value, ONLINE->CHA);
					d += value(ARMOR(ONLINE)->Value, ONLINE->CHA);
					d *= ROOM(dl, hx, hy)->gift_value;
					PLAYER.Gold += d;
					if(d > 0.)
						sprintf(outbuf, "%sYou find a treasure chest holding %s!", fore(YELLOW), money(d, ANSI));
					else
						sprintf(outbuf, "%sYou find an empty, treasure chest.", fore(YELLOW));
					OUT(outbuf); NL; NL;
					sound("max", 64, 0);
					ROOM(dl, hx, hy)->gift_type = 0;
					break;
				case GIFT_MAP:
					if(LEVEL(dl)->Map != MAGIC_MAP && LEVEL(dl)->Map != GIFT_DETECT) {
						LEVEL(dl)->Map = (ACCESS(PLAYER.Access)->Sysop == 'Y') ? GIFT_DETECT : MAGIC_MAP;
						drawmap();
						drawyou();
						sprintf(outbuf, "%sYou find a magic map!", fore(YELLOW));
						OUT(outbuf); NL; NL;
						ROOM(dl, hx, hy)->gift_type = 0;
					}
					break;
				case GIFT_ARMOR:
					clrmap();
					Delay(50);
					sprintf(outbuf, "%sYou find an armor gift shop!", fore(YELLOW));
					OUT(outbuf); NL;
					NORMAL; NL;
					while(ROOM(dl, hx, hy)->gift_type) {
						d = value(ARMOR(ONLINE)->Value, ONLINE->CHA);
						if(PLAYER.ACmod != 0)
							modf(d * (ARMOR(ONLINE)->Class + PLAYER.ACmod) / ARMOR(ONLINE)->Class, &d);
						if(ONLINE->ToAC < 0)
							modf(d * (ARMOR(ONLINE)->Class + ONLINE->ToAC) / ARMOR(ONLINE)->Class, &d);
						if(ONLINE->armor_origin == 0 && ONLINE->armor_type == 0)
							d = 0.;
						sprintf(outbuf, "You own a class %s%s %s", bracket(ARMOR(ONLINE)->Class), fore(GRY), ARMOR(ONLINE)->Name);
						OUT(outbuf);
						if(PLAYER.ACmod || ONLINE->ToAC) {
							sprintf(outbuf, " %s(%s%+d%s,%s%+d%s)", fore(MAG), fore(PLAYER.ACmod > 0 ? YELLOW : PLAYER.ACmod < 0 ? BRED : GRY), PLAYER.ACmod, fore(GRY), fore(ONLINE->ToAC > 0 ? YELLOW : ONLINE->ToAC < 0 ? BRED : GRY), ONLINE->ToAC, fore(MAG));
							OUT(outbuf);
						}
						sprintf(outbuf, " %sworth %s.", fore(GRY), money(d, ANSI));
						OUT(outbuf); NL;
						if(ONLINE->armor_origin == 0 && ONLINE->armor_type == 0 && (PLAYER.ACmod < 0 || ONLINE->ToAC < 0)) {
							sprintf(outbuf, "%sYou look like a leper; go get yourself cured.", fore(YELLOW));
							OUT(outbuf); NL;
							ROOM(dl, hx, hy)->gift_type = 0;
							break;
						}
						for(j = 0; j < NUMARMOR && strcasecmp(table->armor[j]->Origin, "GIFT"); j++);
						for(i = 0; i < MAXARMOR(j); i++) {
							sprintf(outbuf, "%s%s %-24.24s %10s", bracket(i+1), fore(GRY), table->armor[j]->Item[i]->Name, money(table->armor[j]->Item[i]->Value, ANSI));
							OUT(outbuf); NL;
						}
						NL;
						sprintf(prompt, "%sBuy which? ", fore(CYN));
						OUT(prompt);
						if(ins(2)) {
							ROOM(dl, hx, hy)->gift_type = 0;
							break;
						}
						NL; NL;
						if(!strlen(inbuf)) {
							ROOM(dl, hx, hy)->gift_type = 0;
							break;
						}
						i = atoi(inbuf);
						if(i > 0 && i <= MAXARMOR(j)) {
							i--;
							if(PLAYER.Gold + d >= table->armor[j]->Item[i]->Value) {
								PLAYER.Coward = FALSE;
								PLAYER.Gold += d;
								PLAYER.ACmod = dice(nest);
								ONLINE->armor_origin = j;
								ONLINE->armor_type = i;
								ONLINE->ToAC = dice(nest);
								sprintf(PLAYER.Armor, "%s.%s", table->armor[j]->Origin, table->armor[j]->Item[i]->Name);
								PLAYER.Gold -= table->armor[j]->Item[i]->Value;
								RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
								sound("click", 64, 0);
								OUT("Done."); NL;
								ROOM(dl, hx, hy)->gift_type = 0;
							}
							else {
								OUT("You don't have enough money!"); NL;
							}
						}
					}
					drawmap();
					break;
				case GIFT_WEAPON:
					clrmap();
					Delay(50);
					sprintf(outbuf, "%sYou find a weapon gift shop!", fore(YELLOW));
					OUT(outbuf); NL;
					NORMAL; NL;
					while(ROOM(dl, hx, hy)->gift_type) {
						d = value(WEAPON(ONLINE)->Value, ONLINE->CHA);
						if(PLAYER.WCmod != 0)
							modf(d * (WEAPON(ONLINE)->Class + PLAYER.WCmod) / WEAPON(ONLINE)->Class, &d);
						if(ONLINE->ToWC < 0)
							modf(d * (WEAPON(ONLINE)->Class + ONLINE->ToWC) / WEAPON(ONLINE)->Class, &d);
						if(ONLINE->weapon_origin == 0 && ONLINE->weapon_type == 0)
							d = 0.;
						sprintf(outbuf, "You own a class %s%s %s", bracket(WEAPON(ONLINE)->Class), fore(GRY), WEAPON(ONLINE)->Name);
						OUT(outbuf);
						if(PLAYER.WCmod || ONLINE->ToWC) {
							sprintf(outbuf, " %s(%s%+d%s,%s%+d%s)", fore(MAG), fore(PLAYER.WCmod > 0 ? YELLOW : PLAYER.WCmod < 0 ? BRED : GRY), PLAYER.WCmod, fore(GRY), fore(ONLINE->ToWC > 0 ? YELLOW : ONLINE->ToWC < 0 ? BRED : GRY), ONLINE->ToWC, fore(MAG));
							OUT(outbuf);
						}
						sprintf(outbuf, " %sworth %s.", fore(GRY), money(d, ANSI));
						OUT(outbuf); NL;
						if(ONLINE->weapon_origin == 0 && ONLINE->weapon_type == 0 && (PLAYER.WCmod < 0 || ONLINE->ToWC < 0)) {
							sprintf(outbuf, "%sYou look like a leper; go get yourself cured.", fore(YELLOW));
							OUT(outbuf); NL;
							ROOM(dl, hx, hy)->gift_type = 0;
							break;
						}
						for(j = 0; j < NUMWEAPON && strcasecmp(table->weapon[j]->Origin, "GIFT"); j++);
						for(i = 0; i < MAXWEAPON(j); i++) {
							sprintf(outbuf, "%s%s %-24.24s %10s", bracket(i + 1), fore(GRY), table->weapon[j]->Item[i]->Name, money(table->weapon[j]->Item[i]->Value, ANSI));
							OUT(outbuf); NL;
						}
						NL;
						sprintf(prompt, "%sBuy which? ", fore(CYN));
						OUT(prompt);
						if(ins(2)) {
							ROOM(dl, hx, hy)->gift_type = 0;
							break;
						}
						NL; NL;
						if(!strlen(inbuf)) {
							ROOM(dl, hx, hy)->gift_type = 0;
							break;
						}
						i = atoi(inbuf);
						if(i > 0 && i <= MAXWEAPON(j)) {
							i--;
							if(PLAYER.Gold + d >= table->weapon[j]->Item[i]->Value) {
								PLAYER.Coward = FALSE;
								PLAYER.Gold += d;
								PLAYER.WCmod = dice(nest);
								ONLINE->weapon_origin = j;
								ONLINE->weapon_type = i;
								ONLINE->ToWC = dice(nest);
								sprintf(PLAYER.Weapon, "%s.%s", table->weapon[j]->Origin, table->weapon[j]->Item[i]->Name);
								PLAYER.Gold -= table->weapon[j]->Item[i]->Value;
								RPGserver(SERVER_PUTUSER, (UBYTE *)&PLAYER);
								sound("click", 64, 0);
								OUT("Done."); NL;
								ROOM(dl, hx, hy)->gift_type = 0;
							}
							else {
								OUT("You don't have enough money!"); NL;
							}
						}
					}
					drawmap();
					break;
				case GIFT_DETECT:
					LEVEL(dl)->Map = GIFT_DETECT;
					drawmap();
					drawyou();
					sprintf(outbuf, "%sYou find an enchanted map!", fore(YELLOW));
					OUT(outbuf); NL; NL;
					ROOM(dl, hx, hy)->gift_type = 0;
					break;
			}
		}
	}
}

void Dungeon(int level)
{
	int help = TRUE, i, t, x, y, z;
	char *oof, *s='\0';
	char c;

	dl = (level < 0) ? 0 : (level > 99) ? 99 : level;
	if(!(deep_dank_dungeon[++nest] = calloc(100, sizeof(struct deep_dank_dungeon))))
		return;
	title("Dungeon");

	if(nest == 1) {
		if(ANSI == 1 || ANSI == 3)
			OUT("\33\51\60");
	}

	escall = FALSE;
	escape = FALSE;
	t = -5;

	while(!cd() && !escall && !escape) {
		if(LEVEL(dl)->room == NULL) {
			GenLevel();
			i = (1440 * (Julian - PLAYER.LastDate) + (atol(Time) / 10000 * 60 + (atol(Time) / 100) % 100 - PLAYER.LastTime / 100 * 60 - PLAYER.LastTime % 100));
			if(i > (t + 4)) {
				t = i;
				sprintf(outbuf, "dungeon%d", dice(9));
				sound2(outbuf, 1);
			}
		}

		if((ONLINE->weapon_origin || ONLINE->weapon_type) && (int)WEAPON(ONLINE)->Class + PLAYER.WCmod + ONLINE->ToWC < 0) {
			sprintf(outbuf, "Your %s is damaged beyond repair; you toss it aside.", WEAPON(ONLINE)->Name);
			OUT(outbuf); NL; NL;
			Delay(25);
			sprintf(PLAYER.Weapon, "%s.%s", table->weapon[0]->Origin, table->weapon[0]->Item[0]->Name);
			ONLINE->weapon_origin = 0; ONLINE->weapon_type = 0;
			PLAYER.WCmod = 0; ONLINE->ToWC = 0;
		}
		if((ONLINE->armor_origin || ONLINE->armor_type) && (int)ARMOR(ONLINE)->Class + PLAYER.ACmod + ONLINE->ToAC < 0) {
			sprintf(outbuf, "Your %s is damaged beyond repair; you toss it aside.", ARMOR(ONLINE)->Name);
			OUT(outbuf); NL; NL;
			Delay(25);
			sprintf(PLAYER.Armor, "%s.%s", table->armor[0]->Origin, table->armor[0]->Item[0]->Name);
			ONLINE->armor_origin = 0; ONLINE->armor_type = 0;
			PLAYER.ACmod = 0; ONLINE->ToAC = 0;
		}

		if(mymove) {
			mymove = FALSE;
			x = dice(LEVEL(dl)->MaxCol) - 1; y = dice(LEVEL(dl)->MaxRow) - 1;
			if(dice((int)(ROOM(dl, x, y)->type == 0 ? 2 : ROOM(dl, x, y)->type == 3 ? 1 : 4) * ONLINE->CHA / 5 - NUMWALK(dl) / 10) == 1) {
				switch(dice(5)) {
					case 1:
						s = "Your skin crawls";
						break;
					case 2:
						s = "Your pulse quickens";
						break;
					case 3:
						s = "You feel paranoid";
						break;
					case 4:
						s = "Your grip tightens";
						break;
					case 5:
						s = "You stand ready";
						break;
				}
				strcpy(outbuf, s);
				strcat(outbuf, " when you hear a ");
				switch(dice(5)) {
					case 1:
						sound("creak", 64, 0);
						s = "eerie, creaking noise";
						break;
					case 2:
						sound("thunder", 64, 0);
						s = "clap of thunder";
						break;
					case 3:
						sound("ghostly", 64, 0);
						s = "soft, ghostly sound";
						break;
					case 4:
						sound("growl", 64, 0);
						s = "growl from a hungry beast";
						break;
					case 5:
						sound("laugh", 64, 0);
						s = "hideous laugh";
						break;
				}
				strcat(outbuf, s);
				if(abs(x - hx) < 3 && abs(y - hy) < 3)
					s = " nearby!";
				else
				if(abs(x - hx) < 6 && abs(y - hy) < 6)
					s = " in the distance.";
				else
					s = " echoing through the corridors.";
				strcat(outbuf, s);
				if(putmonster(x, y)) {
					plot(rows, 1);
					OUT(outbuf); NL; NL;
					if(LEVEL(dl)->Map == MAGIC_MAP || LEVEL(dl)->Map == GIFT_DETECT)
						drawroom(x, y);
					if(ROOM(dl, x, y)->occupant==CLERIC) {
						plot(rows - 1, 1);
						NORMAL;
						sound("agony", 32, 0);
						Delay(50);
						OUT("You hear a dying cry of agony!!"); NL;
						Delay(100);
						ROOM(dl, x, y)->occupant = FALSE;
						if(LEVEL(dl)->Map == MAGIC_MAP || LEVEL(dl)->Map == GIFT_DETECT)
							drawroom(x, y);
					}
				}
			}
			drawroom(hx, hy);
			drawyou();
			if(ONLINE->INT > 49)
				ROOM(dl, hx, hy)->map = TRUE;
			ExamineRoom();
			continue;
		}

		plot(rows, 1);
		if(PLAYER.Level < SYSREC->Level)
			chkexp(ONLINE);
		RPGserver(SERVER_UNLOCK, NULL);
		if(help) {
			strcpy(prompt, fore(CYN));
			if(hy > 0 && ROOM(dl, hx, hy)->type != 2 && ROOM(dl, hx, hy - 1)->type != 2)
				strcat(prompt, "<N>orth, ");
			if(hy + 1 < LEVEL(dl)->MaxRow && ROOM(dl, hx, hy)->type != 2 && ROOM(dl, hx, hy + 1)->type != 2)
				strcat(prompt, "<S>outh, ");
			if(hx + 1 < LEVEL(dl)->MaxCol && ROOM(dl, hx, hy)->type != 1 && ROOM(dl, hx + 1, hy)->type != 1)
				strcat(prompt, "<E>ast, ");
			if(hx > 0 && ROOM(dl, hx, hy)->type != 1 && ROOM(dl, hx - 1, hy)->type != 1)
				strcat(prompt, "<W>est, ");
			strcat(prompt, "<M>ap, ");
			if(PLAYER.Spell)
				strcat(prompt, "<C>ast, ");
			if(PLAYER.Poison)
				strcat(prompt, "<P>oison, ");
			strcat(prompt, "<Y> Status: ");
			help = (PLAYER.Novice == 'Y');
		}
		else
			sprintf(prompt, "%s:", fore(CYN));
		OUT(prompt);

		mymove = FALSE;
		oof = NULL;
		c = inkey('\0','\0');
		NL; NL;

		switch(c) {
			case 'N':
				if(hy > 0 && ROOM(dl, hx, hy)->type != 2 && ROOM(dl, hx, hy - 1)->type != 2) {
					drawroom(hx, hy);
					fx = hx; fy = hy;
					hy--;
					mymove = TRUE;
				}
				else
					oof = "north";
				break;
			case 'S':
				if(hy + 1 < LEVEL(dl)->MaxRow && ROOM(dl, hx, hy)->type != 2 && ROOM(dl, hx, hy + 1)->type != 2) {
					drawroom(hx, hy);
					fx = hx; fy = hy;
					hy++;
					mymove = TRUE;
				}
				else
					oof = "south";
				break;
			case 'E':
				if(hx + 1 < LEVEL(dl)->MaxCol && ROOM(dl, hx, hy)->type != 1 && ROOM(dl, hx + 1, hy)->type != 1) {
					drawroom(hx, hy);
					fx = hx; fy = hy;
					hx++;
					mymove = TRUE;
				}
				else
					oof = "east";
				break;
			case 'W':
				if(hx > 0 && ROOM(dl, hx, hy)->type != 1 && ROOM(dl, hx - 1, hy)->type != 1) {
					drawroom(hx, hy);
					fx = hx; fy = hy;
					hx--;
					mymove = TRUE;
				}
				else
					oof = "west";
				break;
			case 'M':
				drawmap();
				break;
			case 'C':
				x = Cast(ONLINE, NULL);
				if(ONLINE->HP < 1)
					logoff = TRUE;
				if(x == TELEPORT_SPELL) {
					clrmap();
					teleport();
				}
				if(x == -TELEPORT_SPELL) {
					hx = dice(LEVEL(dl)->MaxCol) - 1; hy = dice(LEVEL(dl)->MaxRow) - 1;
					fx = hx; fy = hy;
					mymove = TRUE;
				}
				break;
			case 'P':
				PoisonWeapon(ONLINE);
				break;
			case 'Y':
				yourstats();
				break;
			default:
				help = TRUE;
				break;
		}
		if(mymove) {
			NUMWALK(dl)++;
			z = (int)(ONLINE->CHA + 1) * (int)(ONLINE->INT + 1) / 10 + ONLINE->DEX / nest;
			if(PLAYER.Level / 9 - nest > PLAYER.Security)
				z /= PLAYER.Level;
			if(dice(z + nest) == 1) {
				plot(rows, 1);
				switch(dice(5)) {
					case 1:
						sound("splat", 64, 0);
						sprintf(outbuf, "%sA bat flies by and soils your %s.", fore(BLU), ARMOR(ONLINE)->Name);
						PLAYER.ACmod -= dice(nest);
						break;
					case 2:
						sound("drop", 64, 0);
						sprintf(outbuf, "%sA drop of acid water lands on your %s.", fore(BLUE), WEAPON(ONLINE)->Name);
						PLAYER.WCmod -= dice(nest);
						break;
					case 3:
						sound("hurt", 64, 0);
						sprintf(outbuf, "%sYou trip on the rocky surface and hurt yourself.", fore(BRN));
						ONLINE->HP -= dice(dl + 1);
						if(ONLINE->HP < 1)
							strcpy(reason, "fell down");
						break;
					case 4:
						sprintf(outbuf, "%sYou are attacked by a swarm of bees.", fore(BRED));
						for(x = 0, y = dice(dl) + 1; x < y; x++)
							ONLINE->HP -= dice(dl + 1);
						if(ONLINE->HP < 1)
							strcpy(reason, "killer bees");
						break;
					case 5:
						sound2("OFF", 1);
						sound("boom", 64, 0);
						sprintf(outbuf, "%sA bolt of lightning strikes you.", fore(WHITE));
						PLAYER.ACmod -= dice(ARMOR(ONLINE)->Class / 2);
						ONLINE->ToAC -= dice(ARMOR(ONLINE)->Class / 2);
						PLAYER.WCmod -= dice(WEAPON(ONLINE)->Class / 2);
						ONLINE->ToWC -= dice(WEAPON(ONLINE)->Class / 2);
						ONLINE->HP -= dice(PLAYER.HP / 2);
						if(ONLINE->HP < 1)
							strcpy(reason, "struck by lightning");
						break;
				}
				OUT(outbuf);
				NORMAL; NL;
				Delay(100);
				if(ONLINE->HP < 1) {
					sprintf(outbuf, "%sYou take too many hits and die!", fore(WHITE));
					OUT(outbuf); NL; NL;
					Delay(5);
					logoff = TRUE;
				}
			}
		}

		if(oof) {
			emptykey();
			sound("wall", 64, 0);
			sprintf(outbuf, "%sOof!  There is a wall to the %s.", fore(YELLOW), oof);
			OUT(outbuf); NL; NL;
			ONLINE->HP -= dice(dl + 1);
			if(ONLINE->HP < 1) {
				sprintf(outbuf, "%sYou take too many hits and die!", fore(WHITE));
				OUT(outbuf); NL; NL;
				Delay(5);
				logoff = TRUE;
				strcpy(reason, "banged head against a wall");
			}
		}

		timeleft();
	}

	if(!escape && nest == 1) {
		if(ANSI == 1 || ANSI == 3)
			OUT("\33\51\102");
		Delay(10);
		sprintf(outbuf, "\33[1;%ur", rows);
		OUT(outbuf);
		plot(rows, 1);
	}

	for(x = 1; x < 5; x++)
		memset(RPC[0][x], 0, sizeof(struct RPC));
	for(x = 0; x < 5; x++)
		memset(RPC[1][x], 0, sizeof(struct RPC));
	for(x = 0; x < 100; x++)
		if(LEVEL(x)->room)
			free(LEVEL(x)->room);
	free(deep_dank_dungeon[nest--]);
}
