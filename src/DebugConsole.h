/* $LastChangedDate$ */
/*
 DebugConsole.h : Fim virtual console display.

 (c) 2008-2011 Michele Martone

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
#ifndef FIM_CONSOLE_H
#define FIM_CONSOLE_H


#include <vector>
#include "fim.h"
#include "Var.h"
#include "Namespace.h"

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
namespace fim
{
	class FimConsole
	{
		public:
		//virtual void add(string s)=0;
	//	virtual int do_dump(int f, int l)const;
	};

	class MiniConsole
#ifdef FIM_NAMESPACES
	:public Namespace
#endif
	{
		char *buffer_;	// the raw console buffer
		char **line_;	// the (displayed) line_ pointers array

		char *bp_;	// pointer to the top of the buffer

		int  bsize_;	// the buffer size
		int  lsize_;	// the lines array size

		int  ccol_;	// the currently pointed column
		int  cline_;	// the line_ on the top of the buffer
		
		int  lwidth_;
		int  rows_;
		int  scroll_;

		public:
		CommandConsole & cc_;	// temporarily

		MiniConsole(CommandConsole & cc,int lw=48, int r=12);
		virtual ~MiniConsole(){}
		int dump();	// non const due to user variables reaction
		int grow();
		fim_err_t setRows(int nr);
		fim_err_t add(const char * cso);
		fim_err_t add(const unsigned char* cso){return add((const char*)cso);}
		int reformat(int newlsize);
		int do_dump(int amount)const;
		int clear();
		int scroll_down();
		int scroll_up();

		private:
		MiniConsole& operator= (const MiniConsole&mc){return *this;/* a nilpotent assignment */}
		MiniConsole(const MiniConsole &mc) :
			buffer_(NULL),
			line_(NULL),
			bp_(NULL),
			bsize_(0),
			lsize_(0),
			ccol_(0),
			cline_(0),
			lwidth_(0),
			rows_(0),
			scroll_(0),
			cc_(mc.cc_)
			{/* this constructor should not be used */}

		int line_length(int li);
		int do_dump(int f, int l)const;
		int do_dump()const;

		int grow_lines(int glines);
		int grow_buffer(int gbuffer);
		int grow(int glines, int gbuffer);
	};
}
#endif

#endif

