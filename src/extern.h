/* $Id$ */
/*
 extern.h : Variables and includes for gluing Fim to Fbi and the lexer/parser subsystem

 (c) 2007 Michele Martone

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#ifndef EXTERN_H
#define EXTERN_H
#include "common.h"
//#include "src/fbtools.h"	//for FB_ costants..
#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))
extern rl_hook_func_t *rl_event_hook;
extern rl_hook_func_t *rl_pre_input_hook;

/*
 *	This file documents the dependency from fbi.
 */

extern int g_fim_no_framebuffer;
#endif
