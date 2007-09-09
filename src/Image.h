/* $Id$ */
/*
 Image.h : Image class headers

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
#ifndef IMAGE_FBVI_H
#define IMAGE_FBVI_H
#include "Viewport.h"
#include "fim.h"
namespace fim
{
/*
 *	A general rule in programming the Image methods is that
 *	of keeping them minimal.
 *	Combining them is matter of the invoking function.
 *	So, there is NO internal triggering  here to call ANY 
 *	display function.
 *
 *
 * 	WARNING : 
 *
 *	This class is evolving towards something
 *	reflecting the content of an image file and 
 *	some rescaled images caches.
 */
class Image:public Viewport
{

	//int redraw;	// there is already an external one!

	public:

	Image(const char *fname_);
	~Image();

	bool revertToLoaded();
	private:
	void reset();

	virtual int rescale();

	/* image methods */
	bool load(const char *fname_);

	/* virtual stuff */
	inline int width();
	inline int height();
};
}
#endif
