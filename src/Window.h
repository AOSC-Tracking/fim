/* $Id$ */
/*
 Window.h : Fim's own windowing system header file

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

#ifndef WINDOW_H
#define WINDOW_H

#ifdef FIM_WINDOWS

#include <vector>
#include <iostream>
#include "fim.h"


#ifndef NULL
	#define NULL 0
#endif

/*
 *	This code is still experimental.
 */

namespace fim
{

/*
 
  The window class should model the behaviour of a binary splitting window
 in a portable manner.
  It should not be tied to a particular window system or graphical environment,
 but it should mimic the behaviour of Vim's windowing system.

 (x,y) : upper left point in
 +--------------+
 |              |
 |              |
 |              |
 +--------------+
 |              |
 +--------------+
                 (x+w,y+h) : lower right point out

*/
class Rect
{
	public:
	int x,y,w,h;	// units, not pixels
	void print()
	{
		std::cout << x <<" " << y  << " "<< w << " " << h  << "\n";
	}

	Rect(int x,int y,int w,int h):
	x(x), y(y), w(w), h(h)
	/* redundant, but not evil */
	{
	}

	Rect(const Rect& rect): x(rect.x), y(rect.y), w(rect.w), h(rect.h)
	{
	}

	public:

	enum Splitmode{ Left,Right,Upper,Lower};

	Rect hsplit(Splitmode s){return split(s);}
	Rect vsplit(Splitmode s){return split(s);}
	Rect split(Splitmode s)
	{
		/*
		 * the default split halves
		 * */
		switch(s)
		{
		case Left:
			return Rect(x,y,w/2,h);
		case Right:
			return Rect(x+w/2,y,w-w/2,h);
		case Upper:
			return Rect(x,y,w,h/2);
		case Lower:
			return Rect(x,y+h/2,w,h-h/2);
		break;
		}
		return Rect(x,y,w,h);
	}

	/* todo : to unsigned integer ! */
	int vlgrow(int units=1)   { h+=units; return 0; } 
	int vlshrink(int units=1) { h-=units; return 0; }
	int vugrow(int units=1)   { y-=units; h+=units ; return 0; } 
	int vushrink(int units=1) { y+=units; h-=units ; return 0; }

	int hlgrow(int units=1)   { x-=units; w+=units ; return 0; } 
	int hrshrink(int units=1) { w-=units; return 0; }
	int hrgrow(int units=1)   { w+=units; return 0; } 
	int hlshrink(int units=1) { x+=units; w-=units ; return 0; }
	bool operator==(const Rect&rect)const
	{
		return x==rect.x &&
		y==rect.y &&
		w==rect.w &&
		h==rect.h;
	}
};


class Window
{
	private:

	enum Spacings{ hspacing=0, vspacing=0};

	/* FIXME : temporary : this will be a Viewport ! */
	public:
	Image *getImage()const;
	private:

/*
 * should we model some status bar here ?
 *
 * note that status could also be located elsewhere.. it is 
 * not in the corners box coordinate system.
 *
 * */
	Rect corners;//,status,canvas;
	int focus;	// if 0 left/up ; otherwise right/lower
	Window *first,*second;
	bool amroot;
	
	public:
	enum Moves{Up,Down,Left,Right,NoMove};

	void print();
	void print_focused();
	Window(const Rect& corners);

	void split();
	void hsplit();
	void vsplit();
	void close();
	void balance();
	int chfocus();
	Moves move_focus(Moves move);
	Moves reverseMove(Moves move);
	int normalize();
	int enlarge(int units);
	int henlarge(int units);
	int venlarge(int units);

	bool can_vgrow(const Window & window, int howmuch);
	bool can_hgrow(const Window & window, int howmuch);

	int heigth()const;
	int width()const;
	int xorigin()const;
	int yorigin()const;

	void setroot();

	private:
	Window(const Window & root);
	bool isleaf()const;
	bool isvalid()const;
	bool issplit()const;
	bool ishsplit()const;
	bool isvsplit()const;
	int hnormalize(int x, int w);
	int vnormalize(int y, int h);
	int count_hdivs()const;
	int count_vdivs()const;

	int vlgrow(int units);
	int vugrow(int units);
	int vushrink(int units);
	int vlshrink(int units);

	int hlgrow(int units);
	int hrgrow(int units);
	int hlshrink(int units);
	int hrshrink(int units);

	Window & focused()const;
	Window & shadowed()const;

	Window & upper();
	Window & lower();
	Window & left();
	Window & right();

	int setwidth(int w);
	int setheigth(int h);
	int setxorigin(int x);
	int setyorigin(int y);

	bool operator==(const Window&window)const;
	public:

	Viewport *viewport;
	Viewport & current_viewport()const;

	const Window & c_focused()const;
	const Window & c_shadowed()const;
        fim::string cmd(const std::vector<fim::string> &args);
	void draw()const;
};

}
#endif
#endif

