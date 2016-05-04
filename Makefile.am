## Process this file with automake to produce Makefile.in

## Created by Anjuta

ACLOCAL_AMFLAGS = \
	-I m4

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS = \
	 -Werror

bin_PROGRAMS = rpgd \
	rpgclient\
	rpgweb

rpgweb_SOURCES = \
	src/rpgweb/main.c\
	src/common.c

rpgweb_LDFLAGS = \
	-lm

rpgweb_CFLAGS = \
	-Iinclude

rpgclient_SOURCES = \
	src/rpgclient/arena.c\
	src/rpgclient/battle.c \
	src/rpgclient/casino.c \
	src/rpgclient/dungeon.c \
	src/rpgclient/editor.c \
	src/rpgclient/global.c \
	src/rpgclient/lists.c \
	src/rpgclient/logon.c \
	src/rpgclient/main.c \
	src/rpgclient/mainmenu.c \
	src/rpgclient/naval.c \
	src/rpgclient/newuser.c \
	src/rpgclient/party.c \
	src/rpgclient/square.c \
	src/rpgclient/sysop.c \
	src/rpgclient/tavern.c \
	src/rpgclient/term.c \
	src/common.c

rpgclient_LDFLAGS = \
	-lm

rpgclient_CFLAGS = \
	-Iinclude

rpgd_SOURCES = \
	src/rpgd/btree.c\
	src/rpgd/rpgd.c \
	src/common.c

rpgd_CFLAGS = \
	-Iinclude

rpgd_LDFLAGS = 

rpgd_LDADD = 

