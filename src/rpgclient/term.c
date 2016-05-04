/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * term.c
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

/* term.c -- Routines for managing terminal I/O settings by Alan Cox.
 * From LJ 17 */

/*
#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
*/

#include "all.h"
#include "global.h"

//	This will be used for new terminal settings
//	static struct termios current;

//	This will hold the initial state so that we can restor it later.
static struct termios initial;

//	Restore the termianl settings to those saved when term_init was called.
void term_restore(void)
{
	tcsetattr(0, TCSANOW, &initial);
}

//	Clean up terminal; called on exit.
void term_exit()
{
	term_restore();
}

//	Will be called when contrl-Z is pressed; this correctly handles the terminal.
void term_ctrlz()
{
	logoff=TRUE;
	signal(SIGTSTP, term_ctrlz);
	term_restore();
	kill(getpid(), SIGSTOP);
}

//	Will be called when the application is continued after having been stopped.
void term_cont() 
{
	signal(SIGCONT, term_cont);
	tcsetattr(0, TCSANOW, &current);
}

//	Needs to be called to initialize the terminal.
void term_init(void) 
{
	//	If stdin isn't a terminal this fails.
	//	But then so does tcsetattr(), so it doesn't matter.
	tcgetattr(STDIN_FILENO, &initial);
	//	Save a copy to work with later.
	current = initial;
	//	We _must_ clean up when we exit.
	//	signal(SIGINT, (sighandler_t)term_exit);
	//	signal(SIGQUIT, (sighandler_t)term_exit);
	//	Control-Z must also be handled.
	signal(SIGTSTP, (sighandler_t)term_ctrlz);
	signal(SIGCONT, (sighandler_t)term_cont);	
	//	atexit(term_exit);
}

//	Set character-by-character input mode.
void term_character(void) 
{
	setvbuf(stdin,(char *)NULL,_IONBF,0);
	cfmakeraw(&current);
	//	One or more characters are sufficient to cause a read return.
	current.c_cc[VMIN] = 1;
	//	No timeout; read waits forever until ready.
	current.c_cc[VTIME] = 0;
	//	Line-by-line mode and echo off 
	current.c_iflag &= ~(IGNCR);
	current.c_iflag |= (IGNBRK | INLCR);
	current.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &current);
	fcntl(STDIN_FILENO, O_NONBLOCK);
}

//	Return to line-by-line input mode.
void term_line(void) 
{
	current.c_lflag |= ICANON;
	tcsetattr(0, TCSANOW, &current);
}
