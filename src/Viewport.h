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
	 * */
class Viewport
{
	protected:
	int		steps,top,left ;	/* viewport variables */

	float            scale    ;	/* viewport variables */
	float            ascale   ;
	float            newscale ;
	float            newascale ;
	int              neworientation;
	int              orientation;
	int              rotation;

	string	fname;	/* viewport variable, too */

	public:
	Viewport();
	Viewport(const Viewport &v);

	int valid()const;

	void scale_fix_top_left();
	int tiny(){if(!img)return 1; return ( img->i.width<=1 || img->i.height<=1 );}

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
	int    invalid;		//the first time the image is loaded it is set to 1
	int    new_image;		//the first time the image is loaded it is set to 1
	int only_first_rescale;		//TEMPORARY

	bool check_invalid();
	bool check_valid();

	struct ida_image *img     ;	/* local (eventually) copy images */
	struct ida_image *fimg    ;	/* master image */
	
	/* viewport methods */
	int viewport_width();
	int viewport_height();

	public:
	/* viewport methods */
	void top_align();
	void bottom_align();
	void reduce(float factor=1.322);	//FIX ME
	void magnify(float factor=1.322);

	virtual int rescale(){};
	/*virtual*/ Viewport* clone();

	/* viewport methods */
	void display();
	void redisplay();
	void auto_width_scale();
	void auto_height_scale();
	int setscale(double ns){newscale=ns;rescale();return 0;}
	virtual int width()=0;
	virtual int height()=0;

	/* viewport methods ? */
	int scale_increment(double ds){if(scale+ds>0.0)newscale=scale+ds;rescale();return 0;}
	int scale_multiply(double  sm){if(scale*sm>0.0)newscale=scale*sm;rescale();return 0;}

	/* viewport methods */
	char* getInfo();
	const char* getName(){return fname.c_str();}
	void auto_scale();

};
}
#endif
