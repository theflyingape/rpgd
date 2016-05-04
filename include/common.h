#define	BLK		0
#define	RED		1
#define	GRN		2
#define	BRN		3
#define	BLU		4
#define	MAG		5
#define	CYN		6
#define	GRY		7
#define	BLACK	8
#define	BRED		9
#define	GREEN	10
#define	YELLOW	11
#define	BLUE		12
#define	MAGENTA	13
#define	CYAN	14
#define	WHITE	15

extern	int cuss(BYTE *);
extern	void Delay(int);
extern	int dice(int);
extern	void fdate(BYTE *, JDATE);
extern	void ftime(BYTE *, BYTE *);
extern	void GetSystemDate(BYTE *);
extern	void GetSystemTime(BYTE *);
extern	void gold(DOUBLE *, BYTE *);
extern	void jdate(BYTE *, JDATE);
extern	void julian(JDATE *, BYTE *);
extern	void plot(int, int);
extern	void proper(BYTE *);
extern	void strlwr(BYTE *);
extern	void strupr(BYTE *);
extern	void swmem(BYTE *, BYTE *, int);
extern	void OriginName(char *,char *,char *);
extern	void LoadETC(struct ServerTable *);
extern	void UnloadETC(struct ServerTable *);
extern	void PutCallers(struct callers *);
extern	void PutHOF(struct ServerTable *);
extern	void PutHOL(struct ServerTable *);
extern	void ReKeySequence(struct user *);
extern	void reroll(struct user *);
