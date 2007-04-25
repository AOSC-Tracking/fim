/* $Id$ */
/*
 Image.h : Image class headers

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
	struct ida_image *fimg    ;
	struct ida_image *simg    ;
	struct ida_image *img     ;
	float            scale    ;
	float            ascale    ;
	float            newscale ;
	float            newascale ;
	int              c, editable , once ;
//	int              need_read ;
	int              neworientation;
	int              orientation;
	int              rotation;
	int              i, arg, key;
	int    new_image,invalid;		//the first time the image is loaded it is set to 1
	//int redraw;
	int only_first_rescale;		//TEMPORARY
	
	int steps,top,len,left ;
	char             *line, *info, *desc;
	char *fname;

	public:
	char* getInfo();
	void auto_scale();
	//void auto_scale_();
	int valid(){return invalid?0:1;}////////!!!!
	Image(const char *fname_);
	~Image();

	void pan_up();
	void pan_down();
	void pan_right();
	void pan_left();
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
	int scale_increment(double ds){if(scale+ds>0.0)newscale=scale+ds;rescale();return 0;}
	int scale_multiply(double  sm){if(scale*sm>0.0)newscale=scale*sm;rescale();return 0;}
	private:
	void reset();
	void scale_fix_top_left();
	int tiny(){if(!img)return 1; return ( img->i.width<=1 || img->i.height<=1 );}
	int rescale();
	void load(const char *fname_);
	void free_mem();
};
}
#endif
