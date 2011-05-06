/* $Id$ */
/*
 Viewport.h : Viewport class headers

 (c) 2007-2011 Michele Martone

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


#ifndef FIM_VIEWPORT_H
#define FIM_VIEWPORT_H

#include "fim.h"
#include "DisplayDevice.h"

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
	fim_off_t		steps_,top_,left_,panned_ ;	/* viewport variables */
        DisplayDevice* displaydevice_;


	Image  *image_;	// !! 
#ifdef FIM_WINDOWS
	Window *window_;
#endif

	CommandConsole &commandConsole;
	public:
        void reset();
#ifdef FIM_WINDOWS
        void reassignWindow(Window *w);
#endif

	Viewport( //yes, horrible
			CommandConsole &c
#ifdef FIM_WINDOWS
			,Window *window
#endif
			);

	Viewport(const Viewport &v);
	~Viewport();
	private:
	Viewport& operator= (const Viewport&v){return *this;/* a nilpotent assignation */}
	void should_redraw()const;
	public:

#if 0
	int valid()const;
#endif

	/* viewport methods */
	void pan_up   (fim_pan_t s=0);
	void pan_down (fim_pan_t s=0);
	void pan_right(fim_pan_t s=0);
	void pan_left (fim_pan_t s=0);
	bool onBottom();
	bool onRight();
	bool onLeft();
	bool onTop();

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
        bool check_invalid()const;
        bool check_valid()const;
#if 0
	int valid();
#endif
	bool scrollforward();
	void scale_position_magnify(fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	void scale_position_reduce(fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	void recenter_horizontally();
	void recenter_vertically();
	void recenter();
};
}
#endif
