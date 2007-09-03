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
#include "fim.h"
namespace fim
{
/*
 *	A general rule in programming the Image methods is that
 *	of keeping them minimal.
 *	Combining them is matter of the invoking function.
 *	So, there is NO internal triggering  here to call ANY 
 *	display function.
 */
class Image
{
	struct ida_image *fimg    ;	/* master image */

	struct ida_image *simg    ;
	struct ida_image *img     ;	/* local (eventually) copy images */

	float            scale    ;	/* viewport variables */
	float            ascale   ;
	float            newscale ;
	float            newascale ;
	int              neworientation;
	int              orientation;
	int              rotation;

	int		steps,top,left ;	/* viewport variables */
	string	fname;			/* viewport variable, too */

	int    new_image,invalid;		//the first time the image is loaded it is set to 1
	int only_first_rescale;		//TEMPORARY
	//int redraw;	// there is already an external one!

	public:
	/* viewport methods */
	char* getInfo();
	void auto_scale();
	//void auto_scale_();
	int valid()const;
	Image(const char *fname_);
	~Image();

	/* viewport methods */
	void pan_up   (int s=0);
	void pan_down (int s=0);
	void pan_right(int s=0);
	void pan_left (int s=0);
	void top_align();
	void bottom_align();
	void reduce(float factor=1.322);	//FIX ME
	void magnify(float factor=1.322);
	void display();
	void redisplay();
	int onBottom();
	int onRight();
	int onLeft();
	int onTop();
	void auto_width_scale();
	void auto_height_scale();
	int setscale(double ns){newscale=ns;rescale();return 0;}

	/* viewport methods ? */
	int scale_increment(double ds){if(scale+ds>0.0)newscale=scale+ds;rescale();return 0;}
	int scale_multiply(double  sm){if(scale*sm>0.0)newscale=scale*sm;rescale();return 0;}

	private:
	void reset();
	void scale_fix_top_left();
	int tiny(){if(!img)return 1; return ( img->i.width<=1 || img->i.height<=1 );}
	int rescale();
	/* image methods */
	void load(const char *fname_);
	void free_mem();

	/* viewport methods */
	bool check_invalid();
	bool check_valid();
	inline int viewport_width();
	inline int viewport_height();
	inline int width();
	inline int heigth();
};
}
#endif
