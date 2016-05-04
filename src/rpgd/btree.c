/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * btree.c
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

/**********************************\
 *  Support functions for B-Tree  *
\**********************************/
void Bputheader(struct B *b)
{
	fseek(b->Bfh, 0, SEEK_SET);
	fwrite(b->Bhp, sizeof(struct Bheader), 1, b->Bfh);
	fflush(b->Bfh);
}

void Bgetnode(struct B *b, unsigned long fp, unsigned char bp)
{
	if(fp < sizeof(struct Bheader) || fp > b->Bhp->filesize) {
		b->filepos[bp] = 0;
		memset(b->Bnp[bp]->node[0], 0, b->Bhp->nodesize);
		return;
	}

	b->filepos[bp] = fp;
	fseek(b->Bfh, fp, SEEK_SET);
	fread(b->Bnp[bp]->node[0], b->Bhp->nodesize, 1, b->Bfh);
}

void Bputnode(struct B *b, unsigned long fp, unsigned char bp)
{
	if(fp) {
		fseek(b->Bfh, fp,SEEK_SET);
		fwrite(b->Bnp[bp]->node[0], b->Bhp->nodesize, 1, b->Bfh);
		fflush(b->Bfh);
		if((unsigned long)ftell(b->Bfh) > b->Bhp->filesize)
			b->Bhp->filesize = ftell(b->Bfh);
	}
}

unsigned long Bfree(struct B *b)
{
	if(b->Bhp->free) {
		Bgetnode(b,b->Bhp->free,2);
		b->Bhp->free=B_NODE(2,0);
		return(b->filepos[2]);
	}
	else
		return(b->Bhp->filesize);
}

/****************************\
 *  Return B Error Message  *
\****************************/
char *Berror(struct B *b)
{
	static char *result;

	switch(b->Bstatus) {
		case B_SUCCESSFUL:
			result = "Successful";
			break;
		case B_CREATE_ERROR:
			result = "Create error";
			break;
		case B_OPEN_ERROR:
			result = "Open error";
			break;
		case B_KEY_NOT_FOUND:
			result = "Key not found";
			break;
		case B_STORE_ERROR:
			result = "Store error";
			break;
		case B_UPDATE_ERROR:
			result = "Update error";
			break;
		case B_DELETE_ERROR:
			result = "Delete error";
			break;
		case B_STACK_TOO_DEEP:
			result = "Stack too deep";
			break;
		case B_END_OF_FILE:
			result = "End of file reached";
			break;
		default:
			result = "Unknown error";
			break;
	}
	return(result);
}

/***********************\
 *  Create new B-Tree  *
\***********************/
void Bcreate(struct B *b, char *filename, unsigned short kl)
{
	struct Bheader bh;

	if((b->Bfh = fopen(filename, "wb+")) == 0) {
		b->Bstatus = B_CREATE_ERROR;
		return;
	}

	strncpy(bh.id, "MWBT", 4);
	bh.version = B_VERSION;
	bh.filesize = sizeof(bh);
	bh.nodesize = B_MAXKEYS * kl + (B_MAXKEYS + 1) * sizeof(long) + sizeof(char);
	bh.keysize = kl;
	bh.nodes = 0;
	bh.keys = 0;
	bh.root = 0;
	bh.free = 0;
	fwrite(&bh, sizeof(bh), 1, b->Bfh);

	fclose(b->Bfh);
	b->Bfh = NULL;
	b->Bstatus = B_SUCCESSFUL;
}

/******************\
 *  Close B-Tree  *
\******************/
void Bclose(struct B *b)
{
	int i;

	if(strncmp("MWBT", b->Bhp->id, 4) == 0)
		Bputheader(b);
	fclose(b->Bfh);
	b->Bfh = NULL;
	if(b->Bhp)
		free(b->Bhp);
	for(i = 0; i < 3; i++) {
		if(b->Bnp[i]->node[0])
			free(b->Bnp[i]->node[0]);
		if(b->Bnp[i])
			free(b->Bnp[i]);
	}
	b->Bstatus = B_SUCCESSFUL;
}

/****************************\
 *  Open B-Tree for access  *
\****************************/
void Bopen(struct B *b, char *filename)
{
	int i,j;

	memset(b, 0, sizeof(struct B));
	if((b->Bfh = fopen(filename, "rb+")) == 0) {
		b->Bstatus = B_OPEN_ERROR;
		return;
	}
	//	See if we can get rid of I/O buffering... every write needs to commit
	//	setbuffer(b->Bfh, NULL, 0);
	setvbuf(b->Bfh, NULL, _IONBF, 0);
	fcntl((long)b->Bfh, F_SETFL, O_NONBLOCK);
	b->Bhp = (struct Bheader *) malloc(sizeof(struct Bheader));
	fread(b->Bhp, sizeof(struct Bheader), 1, b->Bfh);

	if(strncmp("MWBT", b->Bhp->id, 4)) {
		Bclose(b);
		b->Bstatus = B_OPEN_ERROR;
		return;
	}
	for(i = 0; i < 3; i++) {
		b->Bnp[i] = (struct Bnode *)calloc(1, sizeof(struct Bnode));
		b->Bnp[i]->node[0] = (unsigned long *)calloc(1, b->Bhp->nodesize);
		for(j = 1; j <= B_MAXKEYS; j++)
			b->Bnp[i]->node[j] = (unsigned long *)(b->Bnp[i]->node[j-1] + 1);
		b->Bnp[i]->numkeys = (unsigned char *)(b->Bnp[i]->node[B_MAXKEYS] + 1);
		for(j = 0; j < B_MAXKEYS; j++)
			b->Bnp[i]->key[j] = (char *)(b->Bnp[i]->numkeys + sizeof(char) + b->Bhp->keysize * j);
	}
	b->Bstatus = B_SUCCESSFUL;
}

/******************************************\
 *  Get next key in sequence from B-Tree  *
\******************************************/
void Bnext(struct B *b, char *key)
{
	int flag;

	b->Bstatus = B_SUCCESSFUL;
	if(b->keypos[0] + 1 < B_NUMKEYS(0) && B_NODE(0, b->keypos[0] + 1) == 0) {
		b->keypos[0]++;
		memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
		return;
	}
	flag = 0;
	if(b->keypos[0] + 1 <= B_NUMKEYS(0)) {
		b->keypos[0]++;
		while(B_NODE(0, b->keypos[0])) {	//	Get leftmost key
			flag = 1;
			if(b->Bsp > B_MAXLEVELS) {
				b->Bstatus = B_STACK_TOO_DEEP;
				b->Bsp = 0;
				b->filepos[0] = 0;
				memset(key, 0, b->Bhp->keysize);
				return;
			}
			b->Bnodes[b->Bsp] = b->filepos[0];
			b->Bkeys[b->Bsp++] = b->keypos[0];
			b->filepos[0] = B_NODE(0, b->keypos[0]);
			b->keypos[0] = 0;
			Bgetnode(b, b->filepos[0], 0);
		}
	}
	if(flag) {
		memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
		return;
	}
	while(b->Bsp) {					//	Load next key from parent
		b->filepos[0] = b->Bnodes[--b->Bsp];
		b->keypos[0] = b->Bkeys[b->Bsp];
		Bgetnode(b, b->filepos[0], 0);
		if(b->keypos[0] < B_NUMKEYS(0)) {
			memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
			return;
		}
	}
	memset(key, 0, b->Bhp->keysize);
	b->Bstatus=B_END_OF_FILE;
}

/*************************************\
 *  Search B-Tree for specified key	 *
\*************************************/
void Bget(struct B *b, char *key)
{
	unsigned long fp;
	unsigned char flag, holdsp = 0;
	int result;

	b->Bstatus = B_KEY_NOT_FOUND;
	b->Bsp = 0;
	b->filepos[0] = b->Bhp->root;
	b->keypos[0] = 0;
	if(b->filepos[0] == 0) {
		memset(key, 0, b->Bhp->keysize);
		return;
	}
	fp = b->filepos[0];
	while(fp) {
		Bgetnode(b, fp, 0);
		b->keypos[0] = 0;
		flag = 0;
		while(b->keypos[0] < B_NUMKEYS(0)) {
			result = strncasecmp(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
			if(result > 0)
				b->keypos[0]++;
			else {
				flag = (result == 0) ? 1 : 0;
				break;
			}
		}
		if(flag) {
			holdsp = b->Bsp;
			b->filepos[1] = b->filepos[0];
			b->keypos[1] = b->keypos[0];
			b->Bstatus = B_SUCCESSFUL;
		}
		fp = B_NODE(0,b->keypos[0]);
		if(fp) {
			if(b->Bsp > B_MAXLEVELS) {
				b->Bstatus = B_STACK_TOO_DEEP;
				return;
			}
			b->Bnodes[b->Bsp] = b->filepos[0];
			b->Bkeys[b->Bsp++] = b->keypos[0];
		}
	}
	if(b->Bstatus == B_SUCCESSFUL) {
		Bgetnode(b, b->filepos[1], 0);
		b->keypos[0] = b->keypos[1];
		b->Bsp = holdsp;
		memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
	}
	else {
		b->keypos[0]--;
		Bnext(b, key);
		if(b->Bstatus == B_SUCCESSFUL)
			b->Bstatus = B_KEY_NOT_FOUND;
	}
}

/**********************************************\
 *  Get previous key in sequence from B-Tree  *
\**********************************************/
void Bprev(struct B *b, char *key)
{
	int flag;

	b->Bstatus = B_SUCCESSFUL;
	if(b->keypos[0] > 0 && B_NODE(0, b->keypos[0]) == 0) {
		b->keypos[0]--;
		memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
		return;
	}
	flag = 0;
	if(b->keypos[0] >= 0 && b->keypos[0] <= B_NUMKEYS(0)) {
		while(B_NODE(0, b->keypos[0])) {	//	Get rightmost key
			flag = 1;
			if(b->Bsp > B_MAXLEVELS) {
				b->Bstatus = B_STACK_TOO_DEEP;
				b->Bsp = 0;
				b->filepos[0] = 0;
				memset(key, 0, b->Bhp->keysize);
				return;
			}
			b->Bnodes[b->Bsp] = b->filepos[0];
			b->Bkeys[b->Bsp++] = b->keypos[0];
			b->filepos[0] = B_NODE(0,b->keypos[0]);
			Bgetnode(b, b->filepos[0], 0);
			b->keypos[0] = B_NUMKEYS(0);
		}
	}
	if(flag) {
		b->keypos[0]--;
		memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
		return;
	}
	while(b->Bsp) {					//	Load next key from parent
		b->filepos[0] = b->Bnodes[--b->Bsp];
		b->keypos[0] = b->Bkeys[b->Bsp];
		Bgetnode(b, b->filepos[0], 0);
		if(b->keypos[0]) {
			b->keypos[0]--;
			memcpy(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
			return;
		}
	}
	memset(key, 0, b->Bhp->keysize);
	Bget(b, key);
	memset(key, 0, b->Bhp->keysize);
	b->keypos[0]--;
	b->Bstatus = B_END_OF_FILE;
}

/*******************************\
 *  Get first key in sequence  *
\*******************************/
void Bfirst(struct B *b, char *key)
{
	memset(key, 0, b->Bhp->keysize);
	Bget(b, key);
	if(b->Bstatus == B_KEY_NOT_FOUND)
		b->Bstatus = B_SUCCESSFUL;
}

/******************************\
 *  Get last key in sequence  *
\******************************/
void Blast(struct B *b, char *key)
{
	memset(key, 0xff, b->Bhp->keysize);
	Bget(b, key);
	if(b->Bstatus == B_END_OF_FILE)
		Bprev(b, key);
}

/*************************\
 *  Add a key to B-Tree  *
\*************************/
void Bstore(struct B *b, char *key)
{
	unsigned long fp, lnp, rnp, np[6];
	int i, result;

	if(*key == '\0') {
		b->Bstatus = B_STORE_ERROR;
		return;
	};
	b->Bsp = 0;
	b->filepos[0] = b->Bhp->root;
	if(b->filepos[0]) {
		fp = b->filepos[0];
		while(fp) {
			Bgetnode(b, fp, 0);
			b->keypos[0] = 0;
			while(b->keypos[0] < B_NUMKEYS(0)) {
				result = strncasecmp(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize);
				if(result < 0)
					break;
				b->keypos[0]++;
			}
			fp = B_NODE(0, b->keypos[0]);
			if(fp) {
				if(b->Bsp > B_MAXLEVELS) {
					b->filepos[0] = 0;
					b->Bstatus = B_STACK_TOO_DEEP;
					return;
				}
				b->Bnodes[b->Bsp] = b->filepos[0];
				b->Bkeys[b->Bsp++] = b->keypos[0];
			}
		}
	}
	else {
		b->Bhp->root = Bfree(b);
		b->Bhp->nodes++;
		b->filepos[0] = b->Bhp->root;
		b->keypos[0] = 0;
		Bgetnode(b, b->Bhp->root, 0);
	}

	lnp = 0;
	rnp = 0;
	while(B_NUMKEYS(0) == B_MAXKEYS) {
		for(i = 0; i < 5; i++)
			np[i] = B_NODE(0, i);
		for(i = 4; i >= b->keypos[0]; i--)
			np[i + 1] = np[i];
		if(lnp)
			np[b->keypos[0]] = lnp;
		if(rnp)
			np[b->keypos[0] + 1] = rnp;

		for(i = b->keypos[0]; i < B_MAXKEYS; i++) {
			memcpy(b->Bnp[1]->key[i], key, b->Bhp->keysize);
			memcpy(key, b->Bnp[0]->key[i], b->Bhp->keysize);
			memcpy(b->Bnp[0]->key[i], b->Bnp[1]->key[i], b->Bhp->keysize);
		}

		memcpy(b->Bnp[1]->key[0], b->Bnp[0]->key[3], b->Bhp->keysize);
		memcpy(b->Bnp[1]->key[1], key, b->Bhp->keysize);
		memset(b->Bnp[1]->key[2], 0, b->Bhp->keysize);
		memset(b->Bnp[1]->key[3], 0, b->Bhp->keysize);
		B_NODE(1, 0) = np[3];
		B_NODE(1, 1) = np[4];
		B_NODE(1, 2) = np[5];
		B_NODE(1, 3) = 0;
		B_NODE(1, 4) = 0;
		B_NUMKEYS(1) = 2;
		b->filepos[1] = Bfree(b);
		rnp = b->filepos[1];
		Bputnode(b, b->filepos[1], 1);	//	write right subtree
		b->Bhp->nodes++;

		//	keep midkey
		memcpy(key, b->Bnp[0]->key[2], b->Bhp->keysize);

		B_NODE(0, 0) = np[0];
		B_NODE(0, 1) = np[1];
		B_NODE(0, 2) = np[2];
		B_NODE(0, 3) = 0;
		B_NODE(0, 4) = 0;
		B_NUMKEYS(0) = 2;
		memset(b->Bnp[0]->key[2], 0, b->Bhp->keysize);
		memset(b->Bnp[0]->key[3], 0, b->Bhp->keysize);
		lnp = b->filepos[0];
		Bputnode(b, b->filepos[0], 0);	//	write left subtree

		if(b->Bsp) {				//	pop stack for parent node
			b->filepos[0] = b->Bnodes[--b->Bsp];
			b->keypos[0] = b->Bkeys[b->Bsp];
			Bgetnode(b, b->filepos[0], 0);
		}
		else {					//	root node splits; create new one
			B_NODE(0, 0) = lnp;
			B_NODE(0, 1) = rnp;
			for(i = 2; i < 5; i++)
				B_NODE(0, i) = '\0';
			B_NUMKEYS(0) = 1;
			memcpy(b->Bnp[0]->key[0], key, b->Bhp->keysize);
			for(i = 1; i < 4; i++)
				memset(b->Bnp[0]->key[i], 0, b->Bhp->keysize);
			b->filepos[0] = Bfree(b);
			Bputnode(b, b->filepos[0], 0);
			b->Bhp->root = b->filepos[0];
			b->Bhp->nodes++;
			b->Bhp->keys++;
			Bputheader(b);
			b->Bstatus = B_SUCCESSFUL;
			b->filepos[0] = 0;
			return;
		}
	}

	for(i = 3; i >= b->keypos[0]; i--)
		B_NODE(0, i + 1) = B_NODE(0, i);
	B_NODE(0, b->keypos[0]) = lnp;
	B_NODE(0, b->keypos[0] + 1) = rnp;
	for(i = 2; i >= b->keypos[0]; i--)
		memcpy(b->Bnp[0]->key[i + 1], b->Bnp[0]->key[i], b->Bhp->keysize);
	memcpy(b->Bnp[0]->key[b->keypos[0]], key, b->Bhp->keysize);
	B_NUMKEYS(0)++;
	Bputnode(b, b->filepos[0], 0);
	b->Bhp->keys++;
	b->Bsp = 0;
	b->filepos[0] = 0;
	b->Bstatus = B_SUCCESSFUL;
}

/******************************\
 *  Delete a key from B-Tree  *
\******************************/
void Bdelete(struct B *b, char *key)
{
	unsigned long fp;
	unsigned char kp, flag, lr, s1, s2;
	int i, keys;

	if(b->filepos[0] == 0) {
		b->Bstatus = B_DELETE_ERROR;
		return;
	}

	b->Bhp->keys--;

	//	return next key
	Bnext(b, key);
	memcpy(b->Bnp[1]->key[0], key, b->Bhp->keysize);
	Bprev(b, key);
	memcpy(key, b->Bnp[1]->key[0], b->Bhp->keysize);

	fp = b->filepos[0];
	kp = b->keypos[0];
	b->filepos[1] = B_NODE(0, kp + 1);
	b->keypos[1] = kp + 1;
	while(b->filepos[1]) {	//	get leftmost key from right subtree
		if(b->Bsp > B_MAXLEVELS) {
			b->Bstatus = B_STACK_TOO_DEEP;
			return;
		}
		b->Bnodes[b->Bsp] = fp;
		b->Bkeys[b->Bsp++] = b->keypos[1];
		fp = b->filepos[1];
		kp = 0;
		Bgetnode(b, fp, 1);
		b->filepos[1] = B_NODE(1, 0);
		b->keypos[1] = kp;
	}

	if(fp == b->Bhp->root) {	//	tree is only 1-level deep
		for(i = b->keypos[0] + 1; i < B_NUMKEYS(0); i++)
			memcpy(b->Bnp[0]->key[i - 1], b->Bnp[0]->key[i], b->Bhp->keysize);
		for(i = b->keypos[0] + 1; i <= B_NUMKEYS(0); i++)
			B_NODE(0, i - 1) = B_NODE(0, i);
		B_NODE(0, B_NUMKEYS(0)) = 0;
		B_NUMKEYS(0)--;
		memset(b->Bnp[0]->key[B_NUMKEYS(0)], 0, b->Bhp->keysize);
		if(B_NUMKEYS(0) == 0) {
			fp = b->Bhp->free;
			b->Bhp->free = b->Bhp->root;
			B_NODE(0, 0) = fp;
			b->Bhp->root = 0;
			b->Bhp->nodes--;
		}
		Bputnode(b, b->filepos[0], 0);
		Bputheader(b);
		if(strlen(key))
			Bget(b, key);
		else
			b->Bstatus = B_END_OF_FILE;
		return;
	}

	//	copy leftmost key from right subtree into deleted key spot
	Bgetnode(b, fp, 1);
	memcpy(b->Bnp[0]->key[b->keypos[0]], b->Bnp[1]->key[kp], b->Bhp->keysize);
	Bputnode(b, b->filepos[0], 0);
	//	remove leftmost key from right subtree
	for(i = kp + 1; i < B_NUMKEYS(1); i++)
		memcpy(b->Bnp[1]->key[i - 1], b->Bnp[1]->key[i], b->Bhp->keysize);
	for(i = kp + 1; i <= B_NUMKEYS(1); i++)
		B_NODE(1, i - 1) = B_NODE(1, i);
	B_NODE(1, B_NUMKEYS(1)) = 0;
	B_NUMKEYS(1)--;
	memset(b->Bnp[1]->key[B_NUMKEYS(1)], 0, b->Bhp->keysize);
	Bputnode(b, fp, 1);

	Bgetnode(b, fp, 0);				//	get leaf node
	flag = (B_NUMKEYS(0) < 2);

	while(flag) {
		b->filepos[1] = b->Bnodes[--b->Bsp];
		b->keypos[1] = b->Bkeys[b->Bsp];
		Bgetnode(b, b->filepos[1], 1);	//	get parent node
		//	determine with which sibling to contract
		if(b->keypos[1] == 0) {
			lr = 'R';				//	node to the right
			s1 = 0;
			s2 = 2;
			b->filepos[2] = B_NODE(1, b->keypos[1] + 1);
		}
		else {
			lr = 'L';				//	node to the left
			s1 = 2;
			s2 = 0;
			b->keypos[1]--;
			b->filepos[2] = B_NODE(1, b->keypos[1]);
		}
		Bgetnode(b, b->filepos[2], 2);	//	get sibling node
		keys = B_NUMKEYS(0) + 1 + B_NUMKEYS(2);
		if(keys <= B_MAXKEYS) {			//	contract the node with sibling, if there is room

			//	move parent key into the child node
			memcpy(b->Bnp[s1]->key[B_NUMKEYS(s1)++], b->Bnp[1]->key[b->keypos[1]], b->Bhp->keysize);
			for(i = b->keypos[1] + 1; i < B_NUMKEYS(1); i++)
				memcpy(b->Bnp[1]->key[i - 1], b->Bnp[1]->key[i], b->Bhp->keysize);
			for(i = b->keypos[1] + 1; i <= B_NUMKEYS(1); i++)
				B_NODE(1, i - 1) = B_NODE(1, i);
			B_NODE(1, b->keypos[1]) = b->filepos[s1];
			B_NODE(1, B_NUMKEYS(1)) = 0;
			B_NUMKEYS(1)--;
			memset(b->Bnp[1]->key[B_NUMKEYS(1)], 0, b->Bhp->keysize);

			//	move sibling key(s) into contracted node
			for(i = 0; i < B_NUMKEYS(s2); i++)
				memcpy(b->Bnp[s1]->key[B_NUMKEYS(s1) + i], b->Bnp[s2]->key[i], b->Bhp->keysize);
			for(i = 0; i <= B_NUMKEYS(s2); i++)
				B_NODE(s1, B_NUMKEYS(s1) + i) = B_NODE(s2, i);
			B_NUMKEYS(s1) = keys;
			Bputnode(b, b->filepos[s1], s1);

			//	is the parent node now empty and was it the root node?
			if(B_NUMKEYS(1) == 0 && b->filepos[1] == b->Bhp->root) {
				b->Bhp->root = b->filepos[s1];
				B_NODE(1, 0) = b->Bhp->free;
				b->Bhp->free = b->filepos[1];
				b->Bhp->nodes--;
			}

			Bputnode(b, b->filepos[1], 1);	//	write parent node
			flag = (b->Bsp > 0 && B_NUMKEYS(1) < 2);
			fp = b->filepos[s2];
			Bgetnode(b, 0, s2);				//	get null node
			B_NODE(s2, 0) = b->Bhp->free;
			b->Bhp->free = fp;
			Bputnode(b, fp, s2);			//	write null node
			b->Bhp->nodes--;
			Bgetnode(b, b->filepos[1], 0);	//	get leaf node
		}
		else {
			if(lr == 'L') {				//	thus, rotate right operation
				memcpy(b->Bnp[0]->key[1], b->Bnp[0]->key[0], b->Bhp->keysize);
				B_NODE(0, 2)=B_NODE(0, 1);
				B_NODE(0, 1)=B_NODE(0, 0);
				B_NODE(0, 0)=B_NODE(2, B_NUMKEYS(2));
				memcpy(b->Bnp[0]->key[0], b->Bnp[1]->key[b->keypos[1]], b->Bhp->keysize);
				B_NUMKEYS(0) = 2;
				B_NODE(2, B_NUMKEYS(2)) = 0;
				B_NUMKEYS(2)--;
				memcpy(b->Bnp[1]->key[b->keypos[1]], b->Bnp[2]->key[B_NUMKEYS(2)], b->Bhp->keysize);
				memset(b->Bnp[2]->key[B_NUMKEYS(2)], 0, b->Bhp->keysize);
			}
			else {					//  thus, rotate left operation
				memcpy(b->Bnp[0]->key[1], b->Bnp[1]->key[b->keypos[1]], b->Bhp->keysize);
				B_NODE(0, 2) = B_NODE(2, 0);
				B_NUMKEYS(0) = 2;
				memcpy(b->Bnp[1]->key[b->keypos[1]], b->Bnp[2]->key[0], b->Bhp->keysize);
				for(i = 1; i < B_NUMKEYS(2); i++)
					memcpy(b->Bnp[2]->key[i - 1], b->Bnp[2]->key[i], b->Bhp->keysize);
				memset(b->Bnp[2]->key[B_NUMKEYS(2) - 1], 0, b->Bhp->keysize);
				for(i = 1; i <= B_NUMKEYS(2); i++)
					B_NODE(2, i - 1) = B_NODE(2, i);
				B_NODE(2, B_NUMKEYS(2)) = 0;
				B_NUMKEYS(2)--;
			}
			Bputnode(b, b->filepos[0], 0);	//	write leaf node
			Bputnode(b, b->filepos[1], 1);	//	write parent node
			Bputnode(b, b->filepos[2], 2);	//	write sibling node
			flag = 0;
		}
	}
	Bputheader(b);
	if(strlen(key))
		Bget(b, key);
	else
		b->Bstatus = B_END_OF_FILE;
}

/*****************************\
 *  Update record in B-Tree  *
\*****************************/
void Bupdate(struct B *b, char *key)
{
	if(b->filepos[0] == 0 || strncasecmp(key, b->Bnp[0]->key[b->keypos[0]], b->Bhp->keysize) != 0) {
		b->Bstatus = B_UPDATE_ERROR;
		return;
	}
	b->Bstatus = B_SUCCESSFUL;
	memcpy(b->Bnp[0]->key[b->keypos[0]], key, b->Bhp->keysize);
	Bputnode(b, b->filepos[0], 0);
}
