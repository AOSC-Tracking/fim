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
#include "fim.h"

#ifndef IMAGE_FBVI_H
#define IMAGE_FBVI_H


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
class Image
{


	public:

	Image(const char *fname_);
	~Image();

	float            scale    ;	/* viewport variables */
	float            ascale   ;
	float            newscale ;
	float            newascale ;

	bool revertToLoaded();

	/* virtual stuff */
        struct ida_image *img     ;     /* local (eventually) copy images */
	struct ida_image *fimg    ;     /* master image */


	private:


	/* image methods */
	bool load(const char *fname_);

	protected:
	int              neworientation;
	int              orientation;
	int              rotation;

	int    invalid;		//the first time the image is loaded it is set to 1

	string  fname;  /* viewport variable, too */



	public:
	void reset();
	int rescale( float ns=0.0 );

	const char* getName(){return fname.c_str();}
        int tiny()const;

	/* viewport methods */

	void reduce(float factor=1.322);	//FIX ME
	void magnify(float factor=1.322);


	int setscale(double ns);
	/* viewport methods ? */
	int scale_increment(double ds);
	int scale_multiply (double sm);

	bool check_invalid();
	bool check_valid();

        void free();

	char* getInfo();
//	virtual void scale_fix_top_left(){}

	int width();
	int original_width();
	int height();
	int original_height();

	void resize(int nw, int nh);
};
}
#endif
