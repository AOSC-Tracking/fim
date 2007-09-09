/* $Id$ */
/*
 Viewport.h : Viewport class headers

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

#ifndef VIEWPORT_FBVI_H
#define VIEWPORT_FBVI_H

namespace fim
{
	/*
	 * A viewport displays one single image, so it contains the information
	 * relative to the way it is displayed.
	 *
	 * FIXME:
	 * 20070909 A Viewport object does NOT own an image, so it should be deallocated elsewhere!!
	 * */
class Viewport:public Image
{
	protected:
	int		steps,top,left ;	/* viewport variables */

	Image *image;	// !! 

	public:
        void reset();

	Viewport();
	Viewport(const Viewport &v);

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

	protected:

	/* viewport methods */
	int viewport_width();
	int viewport_height();

	public:
	/* viewport methods */
	void top_align();
	void bottom_align();
	/*virtual*/ Viewport* clone();

	/* viewport methods */
	void display();
	void redisplay();

	void auto_width_scale();
	void auto_height_scale();

        void Viewport::setImage(fim::Image* ni);
	void scale_fix_top_left();
        Image* Viewport::getImage()const;

	void auto_scale();
};
}
#endif
