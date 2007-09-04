/* $Id$ */
/*
 Image.h : Image manipulation and display

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

#include "Image.h"

/*
 * TODO :
 *	Windowing related problems:
 *
 * 	Implement a mechanism such that each Image instance owns
 *	one only copy of the original image, and zero or more rescaled versions,
 *	for display use only.
 * 	Once freed, an image could free all of its buffers, depending on the caching policy.
 *
 * 	When windowing will be implemented, note that redisplay will be also affected
 * 	after window geometry change. Update mechanisms are needed..
 */
namespace fim
{
/*
 *	There is a general rule here:
 *	 Public functions should be safe when called in 
 *	 any internal state from the outside.
 *	 Private ones are stricter.
 * 
 */
	int Image::width()
	{
		return fimg->i.width;
	}

	int Image::height()
	{
		return fimg->i.height;
	}


	int Image::rescale()
	{
		/*
		 *	This code is bugful, when called from the constructor, on in a pre-user phase.
		 * 	20070401 hmm  i think it is ok now
		 */
		if( check_invalid() ) return - 1;

		if(tiny() && newscale<scale){newscale=scale;return 0;}

		if(g_fim_no_framebuffer)return 0;

		if(cc.noFrameBuffer())return 0;
		//FIX UPPER MEMORY CONSUMPTION LIMIT...
//		if(newscale > maxscale ) newscale = maxscale;
		neworientation=((cc.getIntVariable("orientation")%4)+4)%4;
		if(newscale == scale && newascale == ascale && neworientation == orientation){return 0;/*no need to rescale*/}
		orientation=neworientation; // fix this
		scale_fix_top_left();
//		status(linebuffer, NULL);
		cc.setVariable("scale",newscale*100);

//		if(simg){free_image(simg);simg=NULL;}
		if(fimg)
		{
			/*
			 * In case of memory allocation failure, we would
			 * like to recover the current image  :) . 
			 *
			 * Here it would be nice to add some sort of memory manager 
			 * keeping score of copies and ... too complicated ...
			 */
			struct ida_image *backup_img=simg;
			if(cc.getIntVariable("_display_status_bar")||cc.getIntVariable("_display_busy"))
				set_status_bar("please wait while rescaling...", getInfo());
			simg  = scale_image(fimg,scale,cc.getFloatVariable("ascale"));
			if( simg && orientation!=0 && orientation != 2)simg  = rotate_image(simg,orientation==1?0:1);
			if( simg && orientation== 2)simg  = flip_image(simg);
			if(!simg)
			{
				simg=backup_img;
				if(cc.getIntVariable("_display_busy"))
					set_status_bar( "rescaling failed (insufficient memory!)", getInfo());
				sleep(1);	//just to give a glimpse..
			}
			else      free_image(backup_img);
			img = simg;
			redraw=1;
			cc.setVariable("height",(int)fimg->i.height);
			cc.setVariable("sheight",(int)img->i.height);
			cc.setVariable("width",(int)fimg->i.width);
			cc.setVariable("swidth",(int)img->i.width);
		}
		else redraw=0;
		return 0;
	}

	Image::Image(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		reset();
		load(fname_);
		if( check_invalid() || (!fimg) ) 
		{
			cout << "invalid loading ! \n";
		}
		else
		{
			/*
			 *	this is a 'patch' to do a first setting of top,left,etc
			 *	variables prior to first visualization without displaying..
			 */
			cc.setVariable("filename",fname_);
			only_first_rescale=1;
			//this->display();
			//if(cc.isInScript()==0)this->auto_scale();
		}
	}

	void Image::reset()
	{
		new_image=1;
		redraw=1;
		scale    = 1.0;
		newscale = 1.0;
		ascale   = 1.0;
		newascale= 1.0;
		steps = cc.getIntVariable("steps");
		if(steps<1)steps = 50;
		top = 0;
		left = 0;
		fimg    = NULL;
		simg    = NULL;
		img     = NULL;
		invalid=0;
		orientation=0;
		neworientation=0;
	}
	
	void Image::load(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		if(fname_==NULL){invalid=1;return;}//DANGER
		free_mem();
		fname=fname_;
		if( cc.getIntVariable("_display_status_bar")||cc.getIntVariable("_display_busy"))set_status_bar("please wait while reloading...", "*");

		if(g_fim_no_framebuffer)
			fimg=NULL;
		else
			fimg = read_image((char*)fname_);

		img=fimg;
//		assert(img);
	        redraw=1;
		if(! img){cout<<"warning : image loading error!\n";invalid=1;return;}
		if(!fimg){cout<<"warning : image allocation error!\n";invalid=1;return;}
	}

	void Image::free_mem()
	{
		if(fimg) free_image(fimg);
		if(simg) free_image(simg);
		reset();
	}

	Image::~Image()
	{
		free_mem();
	}

}
