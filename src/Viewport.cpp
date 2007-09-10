/* $Id$ */
/*
 Viewport.cpp : Viewport class implementation

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

#include "Viewport.h"
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
	Viewport::Viewport():Image(NULL)
	{
		// WARNING : this constructor will be filled soon
		image=NULL;
		reset();
	}

	Viewport::Viewport(const Viewport &v):Image(NULL)
	{
		// WARNING
		image=NULL;
		reset();
		memcpy(this,&v,sizeof(Viewport));
		this->img  = fbi_image_clone(this->img );
		this->fimg = fbi_image_clone(this->fimg);
		// WARNING : these could be NULL's : fixme
		// WARNING : THIS INSTANTIATION OF Viewport NEEDS AD HOC DESTRUCTOR!
	}

	void Viewport::pan_up(int s)
	{
		if(s<0)pan_down(-s);
		else
		{
			if(this->onTop())return;
			s=(s==0)?steps:s;
			top -= s;
			redraw=1;
		}
	}

	void Viewport::pan_down(int s)
	{
		if(s<0)pan_up(-s);
		else
		{
			if(this->onBottom())return;
			s=(s==0)?steps:s;
			top += s;
			redraw=1;
		}
	}

	void Viewport::pan_right(int s)
	{
		if(s<0)pan_left(s);
		else
		{
			if(onRight())return;
			s=(s==0)?steps:s;
			left+=s;
			redraw=1;
		}
	}

	void Viewport::pan_left(int s)
	{
		if(s<0)pan_right(s);
		else
		{
			if(onLeft())return;
			s=(s==0)?steps:s;
			left-=s;
			redraw=1;
		}
	}

	int Viewport::onBottom()
	{
		if(g_fim_no_framebuffer || check_invalid() )return 0;
		return (top + viewport_height() >= img->i.height);
	}

	int Viewport::onRight()
	{
		if(g_fim_no_framebuffer || check_invalid() )return 0;
		return (left + viewport_width() >= img->i.width);
	}

	int Viewport::onLeft()
	{
		if(g_fim_no_framebuffer || check_invalid() )return 0;
		return (left <= 0 );
	}

	int Viewport::onTop()
	{
		if(g_fim_no_framebuffer || check_invalid() )return 0;
		return (top <= 0 );
	}

	int Viewport::viewport_width()
	{
#ifdef FIM_WINDOWS
		return cc.viewport_width();
#else
		return fb_var.xres;
#endif
	}

	int Viewport::viewport_height()
	{
#ifdef FIM_WINDOWS
		return cc.viewport_height();
#else
		return fb_var.yres;
#endif
	}

	void Viewport::bottom_align()
	{
		if(this->onBottom())return;
		if( check_valid() )top = img->i.height - this->viewport_height();
		redraw=1;
	}

	void Viewport::top_align()
	{
		if(this->onTop())return;
		top=0;
		redraw=1;
	}



	void Viewport::redisplay()
	{
	    	redraw=1;
		display();
	}

	void Viewport::display()
	{
		/*
		 *	FIX ME
		 *
		 *
		 *	the display function draws the image in the frame buffer
		 *	memory.
		 *	no scaling occurs, only some alignment.
		 */
		if(redraw==0 || cc.noFrameBuffer())return;

		cout << "predis\n";
		if(img)cout << "postdis!!!\n";
		if( check_invalid() ) return;
		if(img)cout << "postdis!!!\n";
		cout << "postdis\n";
		int autotop=cc.getIntVariable("autotop");
		int flip=cc.getIntVariable("autoflip");
		int mirror=cc.getIntVariable("automirror");
    
		if(g_fim_no_framebuffer)return;

		if (new_image && redraw)
		{
			if(autotop && img->i.height>=this->viewport_height()) //THIS SHOULD BECOME AN AUTOCMD..
		  	{
			    top=autotop>0?0:img->i.height-this->viewport_height();
			}
			/* start with centered image, if larger than screen */
			if (img->i.width > this->viewport_width() )
				left = (img->i.width - this->viewport_width()) / 2;
			if (img->i.height > this->viewport_height() &&  autotop==0)
				top = (img->i.height - this->viewport_height()) / 2;
			new_image = 0;
		}
		else
		//if (redraw  ) 
		{
			/*
			 * This code should be studied in detail..
			 * as it is is straight from fbi.
			 */
	    		if (img->i.height <= this->viewport_height())
	    		{
				top = 0;
	    		}
			else 
			{
				if (top < 0)
					top = 0;
				if (top + this->viewport_height() > img->i.height)
		    			top = img->i.height - this->viewport_height();
	    		}
			if (img->i.width <= this->viewport_width())
			{
				left = 0;
	    		}
			else
			{
				if (left < 0)
				    left = 0;
				if (left + this->viewport_width() > img->i.width)
			    		left = img->i.width - this->viewport_width();
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
#ifdef FIM_WINDOWS
			svga_display_image_new(img, left, top,
					cc.viewport_xorigin(),
					cc.viewport_width(),
					cc.viewport_yorigin(),
					cc.viewport_height(),
					mirror, flip);
#else
			svga_display_image(img, left, top, mirror, flip);
#endif					
		}
	}

	void Viewport::auto_scale()
	{
		float xs,ys;
		if( check_invalid() ) return;

		if(g_fim_no_framebuffer)xs=ys=1.0f;
		else
		{
			xs = (float)this->viewport_width()  / fimg->i.width;
			ys = (float)this->viewport_height() / fimg->i.height;
		}

		newscale = (xs < ys) ? xs : ys;
		rescale();
	}

	int Viewport::valid()const
	{
		return invalid?0:1;
	}

	Viewport::Viewport* clone()
	{
		/*
		 * FIXME : this is essential to implement properly window splitting mechanisms!!
		 **/
		return NULL;
//		return new Viewport();
	}

        Image* Viewport::getImage()const
	{
		return (Image*)this; // !!
		//return image;
	}

        void Viewport::setImage(fim::Image* ni)
	{
		if(!ni)return;
		if(ni)image = ni;
		if(ni->img )img=ni->img;
		if(ni->fimg)fimg=ni->fimg;
		//WARNING !!
	}

        void Viewport::reset()
        {
		((Image*)this)->reset(); /* i love vtable */
                new_image=1;
                redraw=1;
                scale    = 1.0;
                newscale = 1.0;
                ascale   = 1.0;
                newascale= 1.0;
                top = 0;
                left = 0;
                fimg    = NULL;
                img     = NULL;
                invalid=0;
                orientation=0;
                neworientation=0;
        }

	void Viewport::auto_height_scale()
	{
		if( check_invalid() ) return;

		if(g_fim_no_framebuffer=0)newscale = (float)this->viewport_width() / this->width();
		newscale = (float)this->viewport_height() / this->height();

		rescale();
	}

	void Viewport::auto_width_scale()
	{
		if( check_invalid() ) return;

		if(g_fim_no_framebuffer=0)newscale = (float)this->viewport_width() / width();

		rescale();
	}

	void Viewport::scale_fix_top_left()
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
		cx = (float)(left + this->viewport_width() /2) / (img->i.width  * old);
		cy = (float)(top  + this->viewport_height()/2) / (img->i.height * old);
		width  = (int)(img->i.width  * fnew);
		height = (int)(img->i.height * fnew);
		left   = (int)(cx * width  - this->viewport_width() /2);
		top    = (int)(cy * height - this->viewport_height()/2);
		//the cast was added by me...
		scale = newscale;
	}

}

