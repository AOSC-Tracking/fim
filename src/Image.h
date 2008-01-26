/* $Id$ */
/*
 Image.h : Image class headers

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
#include "fim.h"

#ifndef IMAGE_FBI_H
#define IMAGE_FBI_H

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

#define FIM_SCALE_FACTOR 1.322

#ifdef FIM_NAMESPACES
class Image:public Namespace
#else
class Image
#endif
{


	friend class Viewport;		/* don't panic, we are wise people ;) */
	public:

	Image(const char *fname_);
	~Image();

	private:
	float            scale    ;	/* viewport variables */
	float            ascale   ;
	float            newscale ;

	/* virtual stuff */
        struct ida_image *img     ;     /* local (eventually) copy images */
	struct ida_image *fimg    ;     /* master image */

	/* image methods */
	bool load(const char *fname_);

	protected:
	int              orientation;	//aka rotation

	int    invalid;		//the first time the image is loaded it is set to 1

	string  fname;		/* viewport variable, too */

        void free();
	void reset();


        int tiny()const;
	public:
	bool update();

	fim::string getInfo();
	Image(const Image& image); // yes, a private constructor (was)

	int rescale( float ns=0.0 );

	const char* getName(){return fname.c_str();}

	/* viewport methods */

	void reduce( float factor=FIM_SCALE_FACTOR );
	void magnify(float factor=FIM_SCALE_FACTOR );
	
	int getOrientation();

	int setscale(double ns);
	/* viewport methods ? */
	int scale_increment(double ds);
	int scale_multiply (double sm);

	bool check_invalid();
	bool check_valid();

	int width();
	int original_width();
	int height();
	int original_height();

	Image * getClone();
//	void resize(int nw, int nh);
};
}
#endif
