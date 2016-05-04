extern	struct ServerTable *table;
extern	struct gang gm[3];
extern	struct RPC *RPC[2][5];
extern	struct userlevel userlevel;
extern	struct username username;
extern	struct callers callers;
extern	struct termios current;

extern	struct led {
	BYTE dev[32];
	int mins, cols;
} led;

extern	BYTE mybox[2][11];

extern	struct card {
  BYTE *Name;
  UBYTE Value;
} card[13],*deck[52];

extern	int bandit[3];
extern	int slotcol[8];
extern	char *slotval[8];
extern	char spin[4];
extern	int wheel[3][16];

extern	BYTE keycolor[4];
extern	BYTE keytype[4];
extern	BYTE keycode[4];

extern	char *numlev[11];

extern	DOUBLE d;
extern	JDATE Julian;
extern	UWORD nest;
extern	BYTE *Keybuf, *outbuf, *prompt, *inbuf, *filename;
extern	BYTE SYSID[5];
extern	BYTE Today[9],Time[7],Inet[24];
extern	BYTE dated[16],timed[8];
extern	UBYTE acclvl, ANSI, chat, myecho, from, local, logoff, paws, rows, source, timedout;
extern	UBYTE arena, bail, brawl, charity, dungeon, joust, naval, party, realestate, security, tiny;
extern	BYTE reason[64],sigtrap[32];
extern	BYTE **line;
extern	UBYTE numline;
