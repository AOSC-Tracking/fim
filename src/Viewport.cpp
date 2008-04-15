/* $Id$ */
/*
 Viewport.cpp : Viewport class implementation

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
	extern FramebufferDevice ffd;
	Viewport::Viewport(
			CommandConsole &c
#ifdef FIM_WINDOWS
			,Window *window_
#endif
			)
			:framebufferdevice(fim::ffd)
			,commandConsole(c)
			,steps(0)
			,left(0)
			,top(0)
			,image(NULL)
#ifdef FIM_WINDOWS
			,window(window_)
#endif
	{
		// WARNING : this constructor will be filled soon
		reset();
	}

	Viewport::Viewport(const Viewport &v)
		:framebufferdevice(fim::ffd)
		,commandConsole(v.commandConsole)
		,steps(v.steps)
		,left(v.left)
		,top(v.top)
		,image(NULL)
#ifdef FIM_WINDOWS
		,window(v.window)
#endif
	{
		// WARNING
		reset();
		try
		{
#ifndef FIM_BUGGED_CACHE
			if(v.image) image = commandConsole.browser.cache.useCachedImage(v.image->getName());
#else
			if(v.image)image = new Image(*v.image);
#endif
		}
		catch(FimException e)
		{
			image=NULL;
		}
	}

	void Viewport::pan_up(int s)
	{
		if(s<0)pan_down(-s);
		else
		{
			if(this->onTop())return;
			s=(s==0)?steps:s;
			top -= s;
		        framebufferdevice.redraw=1;
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
		        framebufferdevice.redraw=1;
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
		        framebufferdevice.redraw=1;
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
	        	framebufferdevice.redraw=1;
		}
	}

	int Viewport::onBottom()
	{
		if( check_invalid() )return 0;
		return (top + viewport_height() >= image->height());
	}

	int Viewport::onRight()
	{
		if( check_invalid() )return 0;
		return (left + viewport_width() >= image->width());
	}

	int Viewport::onLeft()
	{
		if( check_invalid() )return 0;
		return (left <= 0 );
	}

	int Viewport::onTop()
	{
		if( check_invalid() )return 0;
		return (top <= 0 );
	}

	int Viewport::viewport_width()
	{
		/*
		 * */
#ifdef FIM_WINDOWS
		if(window)return window->width();
		else return 0;
#else
		return framebufferdevice.fb_var.xres;
#endif
	}

	int Viewport::viewport_height()
	{
		/*
		 * */
#ifdef FIM_WINDOWS
		if(window)return window->height();
		else return 0;
#else
		return framebufferdevice.fb_var.yres;
#endif
	}

	void Viewport::bottom_align()
	{
		if(this->onBottom())return;
		if( check_valid() )top = image->height() - this->viewport_height();
	        framebufferdevice.redraw=1;
	}

	void Viewport::top_align()
	{
		if(this->onTop())return;
		top=0;
	        framebufferdevice.redraw=1;
	}

	bool Viewport::redisplay()
	{
		/*
		 * we 'force' redraw.
		 * display() has still the last word :P
		 * */
	        framebufferdevice.redraw=1;
		return display();
	}

	int Viewport::xorigin()
	{
		// horizontal origin coordinate (upper)
#ifdef FIM_WINDOWS
		return window->xorigin();
#else
		return 0;
#endif
	}

	int Viewport::yorigin()
	{
		// vertical origin coordinate (upper)
#ifdef FIM_WINDOWS
		return window->yorigin();
#else
		return 0;
#endif
	}

	void Viewport::null_display()
	{
		/*
		 * for recovery purposes. FIXME
		 * */
		if(framebufferdevice.redraw==0 )return;
#ifdef FIM_WINDOWS
		framebufferdevice.fb_clear_rect(
				xorigin(),
				xorigin()+viewport_width()*framebufferdevice.fs_bpp,
				yorigin(),
				yorigin()+viewport_height()
				);
#else
		framebufferdevice.fb_clear_rect( 0, viewport_width()*framebufferdevice.fs_bpp, 0, viewport_height());
#endif
	}

	bool Viewport::display()
	{
		/*
		 *	the display function draws the image in the frame buffer
		 *	memory.
		 *	no scaling occurs, only some alignment.
		 *
		 *	returns true when some drawing occurred.
		 */
		if((framebufferdevice.redraw==0) )return false;
		if( check_invalid() ) null_display();//  NEW
		if( check_invalid() ) return false;
		/*
		 * should flip ? should mirror ?
		 *
		 * global or inner (not i: !) or local (v:) marker
		 * */
		int autotop=getGlobalIntVariable("autotop")   | image->getIntVariable("autotop") | getIntVariable("autotop");
		//int flip   =getGlobalIntVariable("autoflip")  | image->getIntVariable("flipped") | getIntVariable("flipped");
		int flip   =
		((getGlobalIntVariable("autoflip")== 1|image->getIntVariable("flipped")== 1|getIntVariable("flipped")== 1)&&
		!(getGlobalIntVariable("autoflip")==-1|image->getIntVariable("flipped")==-1|getIntVariable("flipped")==-1));
		int mirror   =
		((getGlobalIntVariable("automirror")== 1|image->getIntVariable("mirrored")== 1|getIntVariable("mirrored")== 1)&&
		!(getGlobalIntVariable("automirror")==-1|image->getIntVariable("mirrored")==-1|getIntVariable("mirrored")==-1));

		image->update();
    
		if (getGlobalIntVariable("i:new") && framebufferdevice.redraw)
		{
			/*
			 * If this is the first image display, we have
			 * the right to rescale the image.
			 * */
			if(autotop && image->height()>=this->viewport_height()) //THIS SHOULD BECOME AN AUTOCMD..
		  	{
			    top=autotop>0?0:image->height()-this->viewport_height();
			}
			/* start with centered image, if larger than screen */
			if (image->width()  > this->viewport_width() )
				left = (image->width() - this->viewport_width()) / 2;
			if (image->height() > this->viewport_height() &&  autotop==0)
				top = (image->height() - this->viewport_height()) / 2;
			setGlobalVariable("i:new",0);
		}
// uncommenting the next 2 lines will reintroduce a bug
//		else
//		if (framebufferdevice.redraw  ) 
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
		
		if(framebufferdevice.redraw)
		{
			framebufferdevice.redraw=0;
			/*
			 * there should be more work to use double buffering (if possible!?)
			 * and avoid image tearing!
			 */
#ifdef FIM_WINDOWS
			framebufferdevice.svga_display_image_new(image->img, left, top,
					xorigin(),
					viewport_width(),
					yorigin(),
					viewport_height(),
					mirror, flip);
#else
			framebufferdevice.svga_display_image_new(image->img, left, top,
			0,framebufferdevice.fb_var.xres,
			0,framebufferdevice.fb_var.yres,
			mirror, flip);
#endif					
			return true;
		}
		return false;
	}

	void Viewport::auto_scale()
	{
		float xs,ys;
		if( check_invalid() ) return;
		else
		{
			xs = (float)this->viewport_width()  / image->original_width();
			ys = (float)this->viewport_height() / image->original_height();
		}

		image->rescale( (xs < ys) ? xs : ys );
	}

	int Viewport::valid()
	{
		// int instead of bool
		return check_valid();
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
			setGlobalVariable("new",1);
		}
                framebufferdevice.redraw=1;
                top  = 0;
                left = 0;

#ifdef FIM_WINDOWS
		steps = getGlobalIntVariable("steps");
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
		if( check_invalid() ) return;

		newscale = (float)this->viewport_height() / image->original_height();

		image->rescale(newscale);
	}

	void Viewport::auto_width_scale()
	{
		/*
		 * scales the image in a way to fit in the viewport width
		 * */
		float newscale;
		if( check_invalid() ) return;

		newscale = (float)this->viewport_width() / image->original_width();

		image->rescale(newscale);
	}

	void Viewport::free()
	{
		/*
		 * frees the currently loaded image, if any
		 */
#ifndef FIM_BUGGED_CACHE
		if(image)
		{	
			if( !commandConsole.browser.cache.freeCachedImage(image) )
				delete image;	// do it yourself :P
		}
#else
		// warning : in this cases exception handling is missing
		if(image)delete image;
#endif
		image = NULL;
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
		 * */
		if(!image)return true;
		if( image)return image->check_invalid();
		return true;
	}

#ifdef FIM_WINDOWS
        void Viewport::reassignWindow(Window *w)
	{
		window = w;
	}
#endif
}

