/* $Id$ */
/*
 fim_stream.cpp : Textual output facility

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


#include "fim.h"
#include "fim_stream.h"

namespace fim
{
		fim_stream& fim_stream::operator<<(const  char* s)
		{
			if(g_fim_no_framebuffer==0)
			{
				if(s)status_screen(s,"");
			}
			else
				if(s)printf("%s",s);

			return *this;
		}
}

