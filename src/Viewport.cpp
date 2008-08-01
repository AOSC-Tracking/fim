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
	extern CommandConsole cc;

	Viewport::Viewport(
			CommandConsole &c
#ifdef FIM_WINDOWS
			,Window *window_
#endif
			)
			:steps(0)
			,top(0)
			,left(0)
			,displaydevice(cc.displaydevice)
			,image(NULL)
#ifdef FIM_WINDOWS
			,window(window_)
#endif
			,commandConsole(c)
	{
		// WARNING : this constructor will be filled soon
		reset();
	}

	Viewport::Viewport(const Viewport &v)
		:
		steps(v.steps)
		,top(v.top)
		,left(v.left)
		,displaydevice(cc.displaydevice)
		,image(NULL)
#ifdef FIM_WINDOWS
		,window(v.window)
#endif
		,commandConsole(v.commandConsole)
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
		        displaydevice->redraw=1;
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
		        displaydevice->redraw=1;
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
		        displaydevice->redraw=1;
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
	        	displaydevice->redraw=1;
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
		return displaydevice->fb_var.xres;
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
		return displaydevice->fb_var.yres;
#endif
	}

	void Viewport::bottom_align()
	{
		if(this->onBottom())return;
		if( check_valid() )top = image->height() - this->viewport_height();
	        displaydevice->redraw=1;
	}

	void Viewport::top_align()
	{
		if(this->onTop())return;
		top=0;
	        displaydevice->redraw=1;
	}

	bool Viewport::redisplay()
	{
		/*
		 * we 'force' redraw.
		 * display() has still the last word :P
		 * */
	        displaydevice->redraw=1;
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
		if( displaydevice->redraw==0 )return;
#ifdef FIM_WINDOWS
		/* FIXME : note that fbi's clear_rect() is a buggy function and thus the fs_bpp multiplication need ! */
		if(displaydevice == &(fim::ffd))
		{
			/*
			 * temporary exception until the fim::ffd.fs_bpp need is fixed for FrameBufferDevice
			 * */
			fim::ffd.fb_clear_rect(
				xorigin(),
				xorigin()+viewport_width()*fim::ffd.fs_bpp,
				yorigin(),
				yorigin()+viewport_height()
				);
		}
		else
		{
			displaydevice->clear_rect(
				xorigin(),
				xorigin()+viewport_width(),
				yorigin(),
				yorigin()+viewport_height()
				);
		}
#else
		/* FIXME */
		displaydevice->clear_rect( 0, viewport_width()*displaydevice->fs_bpp, 0, viewport_height());
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
		if((displaydevice->redraw==0) )return false;
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
		((getGlobalIntVariable("autoflip")== 1)|(image->getIntVariable("flipped")== 1)|(getIntVariable("flipped")== 1)&&
		!((getGlobalIntVariable("autoflip")==-1)|(image->getIntVariable("flipped")==-1)|(getIntVariable("flipped")==-1)));
		int mirror   =
		(((getGlobalIntVariable("automirror")== 1)|(image->getIntVariable("mirrored")== 1)|(getIntVariable("mirrored")== 1))&&
		!((getGlobalIntVariable("automirror")==-1)|(image->getIntVariable("mirrored")==-1)|(getIntVariable("mirrored")==-1)));

		image->update();
    
		if (getGlobalIntVariable("i:new") && displaydevice->redraw)
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
//		if (displaydevice->redraw  ) 
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
		
		if(displaydevice->redraw)
		{
			displaydevice->redraw=0;
			/*
			 * there should be more work to use double buffering (if possible!?)
			 * and avoid image tearing!
			 */
#ifdef FIM_WINDOWS
			if(commandConsole.displaydevice )
			displaydevice->display(
					image->img,
					top,
					left,
					image->height(),
					image->width(),
					image->width(),
					yorigin(),
					xorigin(),
					viewport_height(),
					viewport_width(),
					viewport_width(),
					(mirror?FIM_FLAG_MIRROR:0)|(flip?FIM_FLAG_FLIP:0)/*flags : FIXME*/
					);
#else
			displaydevice->display(
					image->img,
					top,
					left,
					displaydevice->fb_var.yres,
					displaydevice->fb_var.xres,
					displaydevice->fb_var.xres,
					0,
					0,
					displaydevice->fb_var.yres,
					displaydevice->fb_var.xres,
					displaydevice->fb_var.xres,
					(mirror?FIM_FLAG_MIRROR:0)|(flip?FIM_FLAG_FLIP:0)/*flags : FIXME*/
					);
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
			xs = (float)this->viewport_width()  / (image->original_width()*(image->ascale>0.0?image->ascale:1.0));
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
                displaydevice->redraw=1;
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

		newscale = (float)this->viewport_width() / (image->original_width()*(image->ascale>0.0?image->ascale:1.0));

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

