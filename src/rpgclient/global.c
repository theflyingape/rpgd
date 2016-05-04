/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * global.c
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

struct ServerTable *table;
struct gang gm[3];
struct RPC *RPC[2][5];
struct userlevel userlevel;
struct username username;
struct callers callers;
struct termios current;

struct led {
	char dev[32];
	int mins, cols;
} led;

UBYTE mybox[2][11] = {
	{ 'q','m','v','j','t','n','u','l','w','k','x' },
	{ 'Ä','À','Á','Ù','Ã','Å','´','Ú','Â','¿','³' }
};

struct card {
   BYTE *Name;
   UBYTE Value;
};
struct card card[13] = {
   { "Two", 2 },{ "Three", 3 },{ "Four", 4 },{ "Five", 5 },{ "Six", 6 },{ "Seven", 7 },
   { "Eight", 8 },{ "Nine", 9 },{ "Ten", 10 },{"Jack", 11 },{ "Queen", 12 },{ "King", 13 },
   { "Ace", 14 }
};
struct card *deck[52];
int bandit[3];
int slotcol[8] = { BLACK, BLACK, YELLOW, CYAN, YELLOW, GREEN, MAGENTA, BRED };
char *slotval[8] = { "*WILD*", "+BOOM+", "=GOLD=", "-BELL-", "ORANGE", " LIME ", " PLUM ", "CHERRY" };
char spin[4] = { '|', '/', '-', '\\' };
int wheel[3][16] = {
	{ GOLD, PLUM, CHERRY, BELL, LIME, PLUM, BOOM, CHERRY, ORANGE, LIME, WILD, PLUM, CHERRY, BELL, LIME, CHERRY },
	{ ORANGE, GOLD, CHERRY, ORANGE, PLUM, ORANGE, CHERRY, BOOM, BELL, ORANGE, CHERRY, PLUM, LIME, ORANGE, WILD, CHERRY },
	{ BELL, WILD, LIME, CHERRY, ORANGE, BOOM, PLUM, LIME, CHERRY, GOLD, PLUM, CHERRY, LIME, PLUM, LIME, CHERRY }
};

UBYTE keycolor[4] = { MAGENTA, YELLOW, CYAN, BRED };
UBYTE keytype[4] = { 'p', 'g', 's', 'c' };
UBYTE keycode[4];

char *numlev[11] = {
	"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
};

DOUBLE d;
JDATE Julian;
UWORD nest;
BYTE *Keybuf, *outbuf, *prompt, *inbuf, *filename;
BYTE SYSID[5];
BYTE Today[9],Time[7],Inet[24];
BYTE dated[16],timed[8];
UBYTE acclvl, ANSI, chat, myecho, from, local, logoff, paws, rows, source, timedout;
UBYTE arena, bail, brawl, charity, dungeon, joust, naval, party, realestate, security, tiny;
BYTE reason[64],sigtrap[32];
BYTE **line;
UBYTE numline;
