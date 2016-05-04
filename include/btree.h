/*****************************\
 *   B Tree Header File V4   *
 *                           *
 *  Created by Robert Hurst  *
 *   on September 6, 1990    *
\*****************************/
#define B_VERSION			4
#define B_MAXLEVELS			12
#define B_MAXKEYS			4
#define B_NODE(sub1,sub2)	(*b->Bnp[sub1]->node[sub2])
#define B_NUMKEYS(sub1)		(*b->Bnp[sub1]->numkeys)
#define B_SUCCESSFUL		0
#define B_CREATE_ERROR		1
#define B_OPEN_ERROR		2
#define B_KEY_NOT_FOUND		3
#define B_STORE_ERROR		4
#define B_UPDATE_ERROR		5
#define B_DELETE_ERROR		6
#define B_STACK_TOO_DEEP	7
#define B_END_OF_FILE		8

struct B {
	FILE *Bfh;								//	File Handle
	struct Bheader *Bhp;					//	Pointer to B-Tree Header structure
	struct Bnode *Bnp[3];					//	Pointer to B-Tree Node structure
	unsigned long filepos[3];				//	Last file cursor position accessed
	int keypos[3];							//	Last node position accessed (0-4)
	unsigned char Bsp;						//	Stack pointer
	unsigned long Bnodes[B_MAXLEVELS + 1];	//	Stack storage for nodes
	unsigned char Bkeys[B_MAXLEVELS + 1];	//	Stack storage for keys
	unsigned char Bstatus;					//	I/O status of last operation
};

struct Bheader {
	char id[4];								//	File identification code ("MWBT")
	unsigned char version;					//	B-Tree version number
	unsigned char keysize;					//	Number of bytes in key
	unsigned long filesize;					//	Number of bytes in file
	unsigned long nodesize;					//	Number of bytes in node
	unsigned long nodes;					//	Number of active nodes in file
	unsigned long keys;						//	Number of active keys in file
	unsigned long root;						//	Pointer to root node
	unsigned long free;						//	Pointer to next free node
};

struct Bnode {
	unsigned long *node[B_MAXKEYS + 1];		//	Left node pointers (0-4)
	unsigned char *numkeys;					//	Number of active keys in this node
	char *key[B_MAXKEYS];					//	Pointer to key data
};

void Bputheader(struct B *);
void Bgetnode(struct B *, unsigned long, unsigned char);
void Bputnode(struct B *, unsigned long, unsigned char);
unsigned long Bfree(struct B *);
char *Berror(struct B *);
void Bcreate(struct B *, char *, unsigned short);
void Bclose(struct B *);
void Bopen(struct B *, char *);
void Bnext(struct B *, char *);
void Bget(struct B *, char *);
void Bprev(struct B *, char *);
void Bfirst(struct B *, char *);
void Blast(struct B *, char *);
void Bstore(struct B *, char *);
void Bdelete(struct B *, char *);
void Bupdate(struct B *, char *);
