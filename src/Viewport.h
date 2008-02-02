/* $Id$ */
/*
 Viewport.h : Viewport class headers

 (c) 2007-2008 Michele Martone

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


#ifndef VIEWPORT_FBI_H
#define VIEWPORT_FBI_H

#include "fim.h"

namespace fim
{
	/*
	 * A viewport displays one single image, so it contains the information
	 * relative to the way it is displayed.
	 *
	 * FIXME:
	 * 20070909 A Viewport object does NOT own an image, so it should be deallocated elsewhere!!
	 * */
#ifdef FIM_NAMESPACES
class Viewport:public Namespace
#else
class Viewport
#endif
{
	protected:
	int		steps,top,left ;	/* viewport variables */
	FramebufferDevice	&framebufferdevice;

	Image  *image;	// !! 
#ifdef FIM_WINDOWS
	Window *window;
#endif

	public:
        void reset();
#ifdef FIM_WINDOWS
        void reassignWindow(Window *w);
#endif

	Viewport( //yes, horrible
#ifdef FIM_WINDOWS
			Window *window
#endif
			);

	Viewport(const Viewport &v);
	~Viewport(){}

	int valid()const;


	/* viewport methods */
	void pan_up   (int s=0);
	void pan_down (int s=0);
	void pan_right(int s=0);
	void pan_left (int s=0);
	int onBottom();
	int onRight();
	int onLeft();
	int onTop();

	int xorigin();
	int yorigin();
	protected:

//	int redraw;	// there is already an external one!
	/* viewport methods */

	public:
	int viewport_width();
	int viewport_height();
	/* viewport methods */
	void top_align();
	void bottom_align();

	/* viewport methods */
	bool display();
	bool redisplay();
	void null_display();

	void auto_width_scale();
	void auto_height_scale();

        void setImage(fim::Image* ni);
	void scale_fix_top_left();
        const Image* c_getImage()const;
        Image* getImage()const;

	void auto_scale();

	void free();
        bool check_invalid();
        bool check_valid();
	int valid();
	bool scrollforward();
};
}
#endif
