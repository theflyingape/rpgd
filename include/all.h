#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <ctype.h>
#include <fcntl.h>
#include <grp.h>
#include <math.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

typedef	char			BYTE;
typedef	unsigned char	UBYTE; 
typedef	short			WORD;
typedef	unsigned short	UWORD;
typedef	long			LONG;
typedef	unsigned long	ULONG;
typedef	double			DOUBLE;
typedef	ULONG			JDATE;
typedef	void			(*sighandler_t)(int);

#ifndef FALSE
#define	FALSE			0
#endif
#ifndef TRUE
#define	TRUE			~0
#endif
#ifndef FOREVER
#define	FOREVER			for(;;)
#endif

#include "btree.h"
#include "rpgd.h"
#include "client.h"
#include "common.h"
