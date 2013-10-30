/* $LastChangedDate$ */
/*
 FimWindow.h : Fim's own windowing system header file

 (c) 2007-2013 Michele Martone

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

#ifndef FIM_WINDOW_H
#define FIM_WINDOW_H

#include "fim.h"

#ifdef FIM_WINDOWS

#include <vector>
#include <iostream>


#ifndef NULL
	#define NULL 0
#endif /* NULL */
#define FIM_DISABLE_WINDOW_SPLITTING 1

/*
 *	This code is still experimental and programmed in great hurry.
 *	FIXME
 */

namespace fim
{

/*
 * temporarily disabled window enlarge functionality
 * */
#define FIM_BUGGED_ENLARGE 0

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
	fim_coo_t x,y,w,h;	// units, not pixels
	void print()
	{
		std::cout << x <<" " << y  << " "<< w << " " << h  << "\n";
	}

	Rect(fim_coo_t x,fim_coo_t y,fim_coo_t w,fim_coo_t h):
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
	fim_err_t vlgrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   { h+=units; return FIM_ERR_NO_ERROR; } 
	fim_err_t vlshrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) { h-=units; return FIM_ERR_NO_ERROR; }
	fim_err_t vugrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   { y-=units; h+=units ; return FIM_ERR_NO_ERROR; } 
	fim_err_t vushrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) { y+=units; h-=units ; return FIM_ERR_NO_ERROR; }

	fim_err_t hlgrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   { x-=units; w+=units ; return FIM_ERR_NO_ERROR; } 
	fim_err_t hrshrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) { w-=units; return FIM_ERR_NO_ERROR; }
	fim_err_t hrgrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   { w+=units; return FIM_ERR_NO_ERROR; } 
	fim_err_t hlshrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) { x+=units; w-=units ; return FIM_ERR_NO_ERROR; }
	bool operator==(const Rect&rect)const
	{
		return x==rect.x &&
		y==rect.y &&
		w==rect.w &&
		h==rect.h;
	}
};


#ifdef FIM_NAMESPACES
class FimWindow:public Namespace
#else /* FIM_NAMESPACES */
class FimWindow
#endif /* FIM_NAMESPACES */
{
	
	private:

	enum Spacings{ hspacing=0, vspacing=0};
	enum Moves{Up,Down,Left,Right,NoMove};

	/* FIXME : temporary : this will be a Viewport ! */
/*
 * should we model some status bar here ?
 *
 * note that status could also be located elsewhere.. it is 
 * not in the corners box coordinate system.
 *
 * */
	Rect corners_;//,status,canvas;
	bool focus_;	// if 0 left/up ; otherwise right/lower

	FimWindow *first_,*second_;
	bool amroot_;
	
	void split(void);
	void hsplit(void);
	void vsplit(void);
	bool close(void);
	bool swap(void); // new
	void balance(void);
	bool chfocus(void);
	Moves move_focus(Moves move);
	Moves reverseMove(Moves move);
	bool normalize(void);
	fim_err_t enlarge(fim_coo_t units);
	fim_err_t henlarge(fim_coo_t units);
	fim_err_t venlarge(fim_coo_t units);

	bool can_vgrow(const FimWindow & window, fim_coo_t howmuch);
	bool can_hgrow(const FimWindow & window, fim_coo_t howmuch);



	private:
//#ifdef FIM_UNDEFINED
	/*
	 seems useless, who knows ?
	 throws FIM_E_NO_MEM exception
	 it is hust defined to make -Weffc++ happy
	 so keep this private!
	*/
	FimWindow(const FimWindow & root);
//#endif /* FIM_UNDEFINED */
	bool isleaf()const;
	bool isvalid()const;
	bool issplit()const;
	bool ishsplit()const;
	bool isvsplit()const;
	fim_err_t hnormalize(fim_coo_t x, fim_coo_t w);
	fim_err_t vnormalize(fim_coo_t y, fim_coo_t h);
	int count_hdivs()const;
	int count_vdivs()const;

	fim_err_t vlgrow(fim_coo_t units);
	fim_err_t vugrow(fim_coo_t units);
	fim_err_t vushrink(fim_coo_t units);
	fim_err_t vlshrink(fim_coo_t units);

	fim_err_t hlgrow(fim_coo_t units);
	fim_err_t hrgrow(fim_coo_t units);
	fim_err_t hlshrink(fim_coo_t units);
	fim_err_t hrshrink(fim_coo_t units);

	FimWindow & focused()const;
	FimWindow & shadowed()const;

	FimWindow & upper(void);
	FimWindow & lower(void);
	FimWindow & left(void);
	FimWindow & right(void);

	fim_coo_t setwidth(fim_coo_t w);
	fim_coo_t setheight(fim_coo_t h);
	fim_coo_t setxorigin(fim_coo_t x);
	fim_coo_t setyorigin(fim_coo_t y);

	bool operator==(const FimWindow&window)const;

	Viewport *viewport_;

	/*
	 * DANGER : nearly each of these methods launches some exception!
	 * */
	const FimWindow & c_focused()const;
	const FimWindow & c_shadowed()const;

	Viewport & current_viewport()const;
	CommandConsole &commandConsole_;

	FimWindow & operator= (const FimWindow &w){return *this;/* a nilpotent assignation */}

	public:
	void setroot(void);	// only one root window should exist

	/* The only public method launching exceptions is the constructor now.
	 * */
	FimWindow(CommandConsole &c, const Rect& corners, Viewport* vp=NULL); // throws FIM_E_NO_MEM exception
	fim_err_t update(const Rect& corners);

	Viewport * current_viewportp()const;
        fim::string fcmd_cmd(const std::vector<fim::string> &args);
	bool recursive_redisplay()const;	//exception safe
	bool recursive_display()const;		//exception safe

	const Image *getImage()const;		//exception safe

#if 0
	void print(void);
	void print_focused(void);
	void draw()const;
#endif

	fim_coo_t height()const;
	fim_coo_t width()const;
	fim_coo_t xorigin()const;
	fim_coo_t yorigin()const;
	~FimWindow(void);
	virtual size_t byte_size(void)const;
};

}
#endif /* FIM_WINDOWS */
#endif /* FIM_WINDOW_H */

