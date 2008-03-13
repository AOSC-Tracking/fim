/* $Id$ */
/*
 DebugConsole.h : Fim virtual console display.

 (c) 2008 Michele Martone

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
#ifndef FIM_CONSOLE
#define FIM_CONSOLE


#include <vector>
#include "fim.h"

namespace fim
{
	class FimConsole
	{
		public:
		//virtual void add(string s)=0;
		virtual int do_dump(int line, char *s)=0;
	};

	class MiniConsole:public FimConsole
	{
		char *buffer;	// the raw console buffer
		char **line;	// the (displayed) line pointers array

		char *bp;	// pointer to the top of the buffer

		int  bsize;	// the buffer size
		int  lsize;	// the lines array size

		int  ccol;	// the currently pointed column
		int  cline;	// the line on the top of the buffer
		
		int  lwidth;
		int  rows;

		public:

		MiniConsole();
		int dump(int amount);
		int dump(int f, int l);
		int dump();
		int do_dump(int line,char *s);

		int grow_lines(int glines);
		int grow_buffer(int gbuffer);
		int grow();
		int grow(int glines, int gbuffer);
		int reformat(int newlsize);
		void add(const char* cs);
	};
}

#endif

