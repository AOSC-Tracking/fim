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

#ifndef IMAGE_FBI_H
#define IMAGE_FBI_H

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
 *
 *	Note that the way our code stores image data is device-dependent.
 *	Therefore the need for a framebufferdevice reference in Image.
 *
 *	TODO : separate Image in a way multiple viewports could use the same image.
 *	TODO : rename framebufferdevice.redraw -> this.need_redraw
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

	Image(const char *fname_, FILE *fd=NULL);
	Image(const char *fname_, Foo& foo, FILE *fd=NULL);
	~Image();

	bool prev_page();
	bool next_page();

	private:
	Image& operator= (const Image &i){return *this;/* a nilpotent assignation */}
	float            scale    ;	/* viewport variables */
	float            ascale   ;
	float            newscale ;
	float            angle	  ;
	float            newangle ;
	int		 page ;

	/* virtual stuff */
	public://TMP
        struct ida_image *img     ;     /* local (eventually) copy images */
	private://TMP
	struct ida_image *fimg    ;     /* master image */

	/* image methods */
	bool load(const char *fname_, FILE *fd, int want_page);
	void should_redraw()const;

	protected:
	FramebufferDevice	&framebufferdevice;
	int              orientation;	//aka rotation

	int    invalid;		//the first time the image is loaded it is set to 1
	int	no_file;	//no file is associated to this image (used for reading from /dev/stdin at most once.)

	string  fname;		/* viewport variable, too */

        void free();
	void reset();


        int tiny()const;
	public:
	bool can_reload()const{return !no_file;}
	bool update();

	fim::string getInfo();
	Image(const Image& image); // yes, a private constructor (was)

	int rescale( float ns=0.0 );
	int rotate( float angle=1.0 );

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
