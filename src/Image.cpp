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
 * 	20070215	idea : separate redraw into redraw_compulsory+need_redraw
 * 			and keep old_ and new_ for wverwevery variable!
 *	20070318	i'am afraid that pan operations hide some big bug..
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
	void Image::auto_scale()
	{
		float xs,ys;
		if(!img){return;}
		if(!fimg){invalid=1;return;}

		if(g_fim_no_framebuffer)xs=ys=1.0f;
		else
		{
			xs = (float)fb_var.xres / fimg->i.width;
			ys = (float)fb_var.yres / fimg->i.height;
		}

		newscale = (xs < ys) ? xs : ys;
		rescale();
	}

	void Image::auto_height_scale()
	{
		if(!img){img=fimg;}
		if(!img){return;}
		if(!fimg){invalid=1;return;}
//		if(cc.isInScript())return;
//
		if(g_fim_no_framebuffer=0)newscale = (float)fb_var.xres / fimg->i.width;
		newscale = (float)fb_var.yres / fimg->i.height;

		rescale();
	}

	void Image::auto_width_scale()
	{
		if(!img){return;}
		if(!fimg){invalid=1;return;}
//		if(cc.isInScript())return;
//
		if(g_fim_no_framebuffer=0)newscale = (float)fb_var.xres / fimg->i.width;

		rescale();
	}

	int Image::rescale()
	{
		/*
		 *	THIS CODE IS BUGFUL, WHEN CALLED FROM THE CONSTRUCTOR...
		 *	...OR IN THE PRE-USER PHASE..
		 * 	20070401 hmm  i think it is ok now
		 */
		 //	so look at this code as a patch:
		 //
	    	if(!img)invalid=1;
	    	if(!img)return -1;
	    	if(invalid)return -1;//IN CASE OF MEMORY PROBLEMS
		if(tiny() && newscale<scale){newscale=scale;return 0;}

		if(g_fim_no_framebuffer)return 0;

		if(cc.noFrameBuffer())return 0;
		//FIX UPPER MEMORY CONSUMPTION LIMIT...
//		if(newscale > maxscale ) newscale = maxscale;
		neworientation=((cc.getIntVariable("orientation")%4)+4)%4;
		if(newscale == scale && newascale == ascale && neworientation == orientation){return 0;/*no need to rescale*/}
		orientation=neworientation; // fix this
		scale_fix_top_left();
//		sprintf(linebuffer,"scaling (%.0f%%) %s ...", scale*100, fname);
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

	void Image::redisplay()
	{
	    	redraw=1;
		display();
	}

	void Image::display()
	{
		/*
		 *	FIX ME
		 */
		/*
		 *	the display function draws the image in the frame buffer
		 *	memory.
		 *	no scaling occurs, only some alignment.
		 */
		if(redraw==0 || cc.noFrameBuffer())return;

		if(!img)invalid=1;
		if(invalid)return;
		
		int autotop=cc.getIntVariable("autotop");
		int flip=cc.getIntVariable("autoflip");
		int mirror=cc.getIntVariable("automirror");
    
		if(g_fim_no_framebuffer)return;

		if (new_image && redraw)
		{
			if(autotop && img->i.height>=fb_var.yres) //THIS SHOULD BECOME AN AUTOCMD..
		  	{
			    top=autotop>0?0:img->i.height-fb_var.yres;
			}
			/* start with centered image, if larger than screen */
			if (img->i.width > fb_var.xres )
				left = (img->i.width - fb_var.xres) / 2;
			if (img->i.height > fb_var.yres &&  autotop==0)
				top = (img->i.height - fb_var.yres) / 2;
			new_image = 0;
		}
		else
		//if (redraw  ) 
		{
			/*
			 * This code should be studied in detail..
			 * as it is is straight from fbi.
			 */
	    		if (img->i.height <= fb_var.yres)
	    		{
				top = 0;
	    		}
			else 
			{
				if (top < 0)
					top = 0;
				if (top + fb_var.yres > img->i.height)
		    			top = img->i.height - fb_var.yres;
	    		}
			if (img->i.width <= fb_var.xres)
			{
				left = 0;
	    		}
			else
			{
				if (left < 0)
				    left = 0;
				if (left + fb_var.xres > img->i.width)
			    		left = img->i.width - fb_var.xres;
		    	}
		}
		if(only_first_rescale){only_first_rescale=0;return;}
		
		if(redraw)
		{
			redraw=0;
			/*
			 * there should be more work to use double buffering (if possible!?)
			 * and avoid image tearing!
			 */
			//fb_clear_screen();
			svga_display_image(img, left, top, mirror, flip);
		}
	}

	Image::Image(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		reset();
		load(fname_);
		if(invalid || (!img) || (!fimg))
		{
			invalid=1;
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
		editable = 0; once = 0;
		steps = cc.getIntVariable("steps");
		if(steps<1)steps = 50;
		len =0;
		top = 0;
		left = 0;
		fimg    = NULL;
		simg    = NULL;
		img     = NULL;
		fname = NULL;
		desc = NULL;
		info = NULL;
		invalid=0;
		orientation=0;
		neworientation=0;
	}
	
	void Image::scale_fix_top_left()
	{
		/*
		 * this function operates on the image currently in memory
		 *
		 * WARNING : IT SEEMS .. USELESS :)
		 */
		if(g_fim_no_framebuffer)return;

		float old=scale;float fnew=newscale;
		unsigned int width, height;
		float cx,cy;
		cx = (float)(left + fb_var.xres/2) / (img->i.width  * old);
		cy = (float)(top  + fb_var.yres/2) / (img->i.height * old);
		width  = (int)(img->i.width  * fnew);
		height = (int)(img->i.height * fnew);
		left   = (int)(cx * width  - fb_var.xres/2);
		top    = (int)(cy * height - fb_var.yres/2);
		//the cast was added by me...
		scale = newscale;
	}

	void Image::load(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		if(fname_==NULL){invalid=1;return;}//DANGER
		free_mem();
		fname = dupstr(fname_);//safe
		assert(fname);
		if( cc.getIntVariable("_display_status_bar")||cc.getIntVariable("_display_busy"))set_status_bar("please wait while reloading...", "*");

		if(g_fim_no_framebuffer)
			fimg=NULL;
		else
			fimg = read_image(fname);

//		desc = make_desc(&fimg->i,fname);
//	    	info = make_info(fimg,scale);
//		desc = info = "foo";
		desc = info = "";
//		cout << fname << "\n";//return;
		img=fimg;
//		assert(img);
	        redraw=1;
		if(! img){cout<<"warning : image loading error!\n";invalid=1;return;}
		if(!fname){cout<<"warning : string loading error!\n";fname=NULL;invalid=1;return;}
		if(!fimg){cout<<"warning : image allocation error!\n";invalid=1;return;}
	}

	void Image::free_mem()
	{
		if(fimg) free_image(fimg);
		if(simg) free_image(simg);
		if(fname) free(fname);
		reset();
	}

	void Image::reduce(float factor)
	{
		newscale = scale / factor;
		rescale();
	}

	void Image::magnify(float factor)
	{
		newscale = scale * factor;
		rescale();
	}

	void Image::bottom_align()
	{
	    if(this->onBottom())return;
	    if(img && invalid==0)top = img->i.height - fb_var.yres;
	    redraw=1;
	}

	void Image::top_align()
	{
	    if(this->onTop())return;
	    top=0;
	    redraw=1;
	}

	void Image::pan_up(int s)
	{
	    if(s<0)pan_down(-s);
	    else{
	    if(this->onTop())return;
	    s=(s==0)?steps:s;
	    top -= s;
	    redraw=1;}
	}

	void Image::pan_down(int s)
	{
	    if(s<0)pan_up(-s);
	    else{
	    if(this->onBottom())return;
	    s=(s==0)?steps:s;
	    top += s;
	    redraw=1;}
	}

	void Image::pan_right(int s)
	{
	    if(s<0)pan_left(s);
	    else{
	    if(onRight())return;
	    s=(s==0)?steps:s;
	    left+=s;
	    redraw=1;}
	}

	void Image::pan_left(int s)
	{
            if(s<0)pan_right(s);
	    else{
	    if(onLeft())return;
	    s=(s==0)?steps:s;
	    left-=s;
	    redraw=1;}
	}

	int Image::onBottom()
	{
		if(g_fim_no_framebuffer)return 0;

		if(img && invalid==0) return (top + fb_var.yres >= img->i.height);
		else return 0;
	}

	int Image::onRight()
	{
		if(g_fim_no_framebuffer)return 0;

		if(img && invalid==0)return (left + fb_var.xres >= img->i.width);
		else return 0;
	}

	int Image::onLeft()
	{
		if(g_fim_no_framebuffer)return 0;

		if(img && invalid==0)return (left <= 0 );
		else return 0;
	}

	int Image::onTop()
	{
		if(g_fim_no_framebuffer)return 0;

		if(img && invalid==0)return (top <= 0 );
		else return 0;
	}
	
	Image::~Image()
	{
		free_mem();
	}

	char* Image::getInfo()
	{
		if(fimg)return make_info(fimg,scale);return NULL;
	}
}
