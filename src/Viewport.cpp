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
	Viewport::Viewport(
#ifdef FIM_WINDOWS
			Window *window_):window(window_
#endif
			)
	{
		// WARNING : this constructor will be filled soon
		image=NULL;
		reset();
	}

	Viewport::Viewport(const Viewport &v)
#ifdef FIM_WINDOWS
		:window(v.window)
#endif
	{
		// WARNING
//		image=new Image(v.getImage());
		image=NULL;
		reset();
		memcpy(this,&v,sizeof(Viewport));
//		image->img  = fbi_image_clone(this->img );
//		image->fimg = fbi_image_clone(this->fimg);
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
		return (top + viewport_height() >= image->height());
	}

	int Viewport::onRight()
	{
		if(g_fim_no_framebuffer || check_invalid() )return 0;
		return (left + viewport_width() >= image->width());
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
		//return cc.viewport_width();
		//FIXME !
		return window->width();
#else
		return fb_var.xres;
#endif
	}

	int Viewport::viewport_height()
	{
#ifdef FIM_WINDOWS
//		return cc.viewport_height();
		//FIXME !
		return window->height();
#else
		return fb_var.yres;
#endif
	}

	void Viewport::bottom_align()
	{
		if(this->onBottom())return;
		if( check_valid() )top = image->height() - this->viewport_height();
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

	int Viewport::xorigin()
	{
		//FIXME !
#ifdef FIM_WINDOWS
		return window->xorigin();
#else
		return 0;
#endif
	}

	int Viewport::yorigin()
	{
		//FIXME !
#ifdef FIM_WINDOWS
		return window->yorigin();
#else
		return 0;
#endif
	}

	void Viewport::null_display()
	{
		if( g_fim_no_framebuffer  )return;
		if(redraw==0 )return;
#ifdef FIM_WINDOWS
		fb_clear_rect(
				xorigin(),
				xorigin()+viewport_width()*fs_bpp,
				yorigin(),
				yorigin()+viewport_height()
				);
#else
		fb_clear_rect( 0, viewport_width()*fs_bpp, 0, viewport_height());
#endif
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
		if((redraw==0) )return;
		if( check_invalid() ) null_display();//  NEW
		if( check_invalid() ) return;
		int autotop=cc.getIntVariable("autotop");
		int flip=cc.getIntVariable("autoflip");
		int mirror=cc.getIntVariable("automirror");
    
		if(g_fim_no_framebuffer)return;
		if (image->new_image && redraw)
		{
			if(autotop && image->height()>=this->viewport_height()) //THIS SHOULD BECOME AN AUTOCMD..
		  	{
			    top=autotop>0?0:image->height()-this->viewport_height();
			}
			/* start with centered image, if larger than screen */
			if (image->width()  > this->viewport_width() )
				left = (image->width() - this->viewport_width()) / 2;
			if (image->height() > this->viewport_height() &&  autotop==0)
				top = (image->height() - this->viewport_height()) / 2;
			image->new_image = 0;
		}
// uncommenting the next 2 lines will reinsert a bug
//		else
//		if (redraw  ) 
		{
			/*	
			 *	20070911
			 *	this code is essential in order to protect from bad left and top values.
			 * */
			/*
			 * This code should be studied in detail..
			 * as it is is straight from fbi.
			 */
	    		if (image->height() <= this->viewport_height())
	    		{
				top = 0;
	    		}
			else 
			{
				if (top < 0)
					top = 0;
				if (top + this->viewport_height() > image->height())
		    			top = image->height() - this->viewport_height();
	    		}
			if (image->width() <= this->viewport_width())
			{
				left = 0;
	    		}
			else
			{
				if (left < 0)
				    left = 0;
				if (left + this->viewport_width() > image->width())
			    		left = image->width() - this->viewport_width();
		    	}
		}
		
		if(redraw)
		{
			redraw=0;
			/*
			 * there should be more work to use double buffering (if possible!?)
			 * and avoid image tearing!
			 */
			//fb_clear_screen();
#ifdef FIM_WINDOWS
			svga_display_image_new(image->img, left, top,
					xorigin(),
					viewport_width(),
					yorigin(),
					viewport_height(),
					mirror, flip);
#else
			svga_display_image(image->img, left, top, mirror, flip);
#endif					
/*			cout 
				<<  top << " "
				<< left << " "
				<< "\n";*/
		}
	}

	void Viewport::auto_scale()
	{
		float xs,ys;
		if( check_invalid() ) return;

		if(g_fim_no_framebuffer)xs=ys=1.0f;
		else
		{
			xs = (float)this->viewport_width()  / image->original_width();
			ys = (float)this->viewport_height() / image->original_height();
		}

		image->rescale( (xs < ys) ? xs : ys );
	}

	int Viewport::valid()
	{
		//fixme
		return check_valid();
//		return invalid?0:1;
	}

	Viewport::Viewport* clone()
	{
		/*
		 * FIXME : this is essential to implement properly window splitting mechanisms!!
		 **/
		return NULL;
//		return new Viewport();
	}

        const Image* Viewport::c_getImage()const
	{
		/*
		 * returns the image pointer, regardless its use! 
		 * */
		return image;
	}

        Image* Viewport::getImage()const
	{
		/*
		 * returns the image pointer, regardless its use! 
		 * */
		return image;
	}

        void Viewport::setImage(fim::Image* ni)
	{
		/* 
		 * the image could be NULL
		 * this image is not tightly bound!
		 *
		 * FIXME
		 */

		image = NULL;
		reset();
		image = ni;
	}

        void Viewport::reset()
        {
		/*
		 * resets some image flags and should reset the image position in the viewport
		 *
		 * FIXME
		 * */
		if(image)
		{
			image->reset();
                	image->new_image=1;
		}
                redraw=1;
                top  = 0;
                left = 0;

#ifdef FIM_WINDOWS
		steps = cc.getIntVariable("steps");
		if(steps<1)steps = 50;
#else 
		// WARNING : FIXME, TEMPORARY
		steps = 50;
#endif
        }

	void Viewport::auto_height_scale()
	{
		/*
		 * scales the image in a way to fit in the viewport height
		 * */
		float newscale;
		if( check_invalid() || g_fim_no_framebuffer ) return;

		newscale = (float)this->viewport_height() / image->original_height();

		image->rescale(newscale);
	}

	void Viewport::auto_width_scale()
	{
		/*
		 * scales the image in a way to fit in the viewport width
		 * */
		float newscale;
		if( check_invalid() || g_fim_no_framebuffer ) return;

		newscale = (float)this->viewport_width() / image->original_width();

		image->rescale(newscale);
	}

	void Viewport::free()
	{
		//WARNING : THIS SHOULD FREE IMAGES !!
		//see browser first
		//FIXME
	}

        bool Viewport::check_valid()
	{
		/*
		 * yes :)
		 * */
		return ! check_invalid();
	}

        bool Viewport::check_invalid()
	{
		/*
		 * this should not happen! (and probably doesn't happen :) )
		 *
		 * FIXME
		 * */
		if(!image)return true;
		if( image)return image->check_invalid();
		return true;
	}

#ifdef FIM_WINDOWS
        void Viewport::reassign(Window *w)
	{
		window = w;
	}
#endif
}

