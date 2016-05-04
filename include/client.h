//	*****************************
//	*    RPGBBS Deluxe Client   *
//	*                           *
//	*  Written by Robert Hurst  *
//	*****************************
#define	NUMACCESS		table->NumAccess
#define	ACCESS(n)		table->access[n]
#define	NUMARENA		table->NumArena
#define	ARENA(n)		table->arena[n]
#define	NUMARMOR		table->NumArmor
#define	MAXARMOR(n)		table->armor[n]->NumArmor
#define	ARMOR(n)		table->armor[n->armor_origin]->Item[n->armor_type]
#define	NUMCLASS		table->NumClass
#define	MAXCLASS(n)		table->class[n]->NumCharacter
#define	CLASS(n)		table->class[n->class_origin]->Character[n->class_type]
#define	NUMDUNGEON		table->NumDungeon
#define	DUNGEON(n)		table->dungeon[n]
#define	NUMMAGIC		table->NumMagic
#define	MAGIC(n)		table->magic[n]
#define	NUMNAVAL		table->NumNaval
#define	NAVAL(n)		table->naval[n]
#define	NUMPOISON		table->NumPoison
#define	POISON(n)		table->poison[n]
#define	NUMREALESTATE	table->NumRealEstate
#define	REALESTATE(n)	table->realestate[n]
#define	NUMSECURITY		table->NumSecurity
#define	SECURITY(n)		table->security[n]
#define	CALLERS			table->callers
#define	NUMWEAPON		table->NumWeapon
#define	MAXWEAPON(n)	table->weapon[n]->NumWeapon
#define	WEAPON(n)		table->weapon[n->weapon_origin]->Item[n->weapon_type]

#define	IRSREC			table->irsrec
#define	NEWREC			table->newrec
#define	SYSREC			table->sysrec

#define	ONLINE			RPC[0][0]
#define	PLAYER			RPC[0][0]->user
#define	ENEMY			RPC[1][0]->user
#define	GANG(i,j)		RPC[i][j]->user

#define	KEYBUF_SIZE		256
#define	OUTBUF_SIZE		256
#define	PROMPT_SIZE		256
#define	INBUF_SIZE		256

#define	CLL				output("\33[K")
#define	INPUT(t)		input(Keybuf, &source, KEYBUF_SIZE, t)
#define	NL				output("\15\12")
#define	NORMAL			output("\33[0;37m")
#define	OUT(s)			output(s)
#define	REVERSE			output("\33[7m")
#define	RUBOUT			output("\10 \10")
#define	SAMELINE		output("\15")

#define	AN(s)			(strchr("AaEeIiOoUu", s[0]) ? "an " : "a ")
#define	HE(n)			n->he

#define WILD			0
#define BOOM			1
#define	GOLD			2
#define	BELL			3
#define	ORANGE			4
#define	LIME			5
#define	PLUM			6
#define	CHERRY			7

#define	EXP(n)			1000. * pow(2., n - 1.)
#define	JOUST(n)		modf(pow(2., (double)(n - 1) / 2.) * 10. * (101. - n) / 100., &d);

struct callers {
	BYTE Last[5][23];
	BYTE Reason[5][64];
};

struct RPC {
	struct user user;
	UBYTE class_origin,class_type;
	UBYTE weapon_origin,weapon_type;
	UBYTE armor_origin,armor_type;
	WORD BP,HP,SP,HULL;
	BYTE ToAC,ToWC;
	BYTE *He,*His;
	BYTE *he,*his,*him;
	UBYTE STR,INT,DEX,CHA;
	UBYTE Confused;
};

struct RPGitem {
 	char choice;
 	char *name;
};
struct RPGmenu {
	char *option;
	char color;
	int items;
	struct RPGitem item[20];
};

extern	void skillplus(void);
extern	void assign(int);
extern	void beep(void);
extern	char *back(int);
extern	char *bracket(int);
extern	void broadcast(char *);
extern	void bump(char *);
extern	int cd(void);
extern	void charstats(struct RPC *);
extern	void chkexp(struct RPC *);
extern	void cls(void);
extern	void displayview(void);
extern	void finputs(char *,int,FILE *);
extern	void emulation(void);
extern	void emptykey(void);
extern	void eraser(void);
extern	char *fore(int);
extern	char getkey(void);
extern	void immortalize(void);
extern	char inkey(char,char);
extern	void input(BYTE *, UBYTE *, ULONG, ULONG);
extern	int ins(int);
extern	void jazz(char *);
extern	void keyhint(void);
extern	char *money(double,int);
extern	char more(void);
extern	void news(char *);
extern	void note(char *);
extern	char option(struct RPGmenu *);
extern	void output(void *);
extern	void paused(void);
extern	void sound(char *,int,int);
extern	void sound2(char *,int);
extern	int timeleft(void);
extern	void title(char *);
extern	int type(char *,int);
extern	int type_pix(char *,char);
extern	DOUBLE value(DOUBLE,UBYTE);
extern	int GetRPC(struct RPC *);
extern	void LoadRPC(struct RPC *);
extern	int RPGserver(char, UBYTE *);
extern	void yourstats(void);
extern	void CreateRPC(struct RPC *);
extern	void ExchangeWeapon(struct RPC *,struct RPC *);
extern	void ExchangeArmor(struct RPC *,struct RPC *);

extern	void Arena(void);
extern	int Cast(struct RPC *,struct RPC *);
extern	void PoisonWeapon(struct RPC *);
extern	void Battle(void);
extern	void Casino(void);
extern	void Dungeon(int);
extern	void Editor(struct RPC *rpc,int);
extern	int ReadMail(char *);
extern	void WriteMail(char *);
extern	void Lists(void);
extern	int Logon(void);
extern	void MainMenu(void);
extern	void Naval(void);
extern	void NewUser(void);
extern	void Party(void);
extern	void Square(void);
extern	void Sysop(void);
extern	void Tavern(void);

extern	void term_init(void);
extern	void term_character(void);
extern	void term_exit(void);
