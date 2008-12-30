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

	Viewport::Viewport(
			CommandConsole &c
#ifdef FIM_WINDOWS
			,Window *window_
#endif
			)
			:steps(0)
			,top(0)
			,left(0)
			,panned(0x0)
			,displaydevice(c.displaydevice)
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
		,panned(v.panned)
		,displaydevice(v.displaydevice)
		,image(NULL)
#ifdef FIM_WINDOWS
		,window(v.window)
#endif
		,commandConsole(v.commandConsole)
	{
		// WARNING
		//reset();
		try
		{
#ifndef FIM_BUGGED_CACHE
	#ifdef FIM_CACHE_DEBUG
			if(v.image) std::cout << "Viewport:Viewport():maybe will cache \"" <<v.image->getName() << "\" from "<<v.image<<"\n" ;
			else std::cout << "no image to cache..\n";
	#endif
			if(v.image && !v.image->check_invalid()) setImage( commandConsole.browser.cache.useCachedImage(v.image->getKey()) );
#else
			if(v.image) setImage ( new Image(*v.image) ) ;
#endif
		}
		catch(FimException e)
		{
			image=NULL;
			std::cerr << "fatal error" << __FILE__ << ":" << __LINE__ << "\n";
		}
	}

	void Viewport::pan_up(int s)
	{
		panned |= 0x1;
		if(s<0)pan_down(-s);
		else
		{
			if(this->onTop())return;
			s=(s==0)?steps:s;
			top -= s;
			should_redraw();
		}
	}

	void Viewport::pan_down(int s)
	{
		panned |= 0x1;
		if(s<0)pan_up(-s);
		else
		{
			if(this->onBottom())return;
			s=(s==0)?steps:s;
			top += s;
			should_redraw();
		}
	}

	void Viewport::pan_right(int s)
	{
		panned |= 0x2;
		if(s<0)pan_left(s);
		else
		{
			if(onRight())return;
			s=(s==0)?steps:s;
			left+=s;
			should_redraw();
		}
	}

	void Viewport::pan_left(int s)
	{
		panned |= 0x2;
		if(s<0)pan_right(s);
		else
		{
			if(onLeft())return;
			s=(s==0)?steps:s;
			left-=s;
			should_redraw();
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
		should_redraw();
	}

	void Viewport::top_align()
	{
		if(this->onTop())return;
		top=0;
		should_redraw();
	}

	bool Viewport::redisplay()
	{
		/*
		 * we 'force' redraw.
		 * display() has still the last word :P
		 * */
		should_redraw();
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
		{
			displaydevice->clear_rect(
				xorigin(),
				xorigin()+viewport_width()-1,
				yorigin(),
				yorigin()+viewport_height()-1
				);
		}
#else
		/* FIXME */
		displaydevice->clear_rect( 0, (viewport_width()-1)*displaydevice->fs_bpp, 0, (viewport_height()-1));
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

		if (getGlobalIntVariable("i:want_autocenter") && displaydevice->redraw)
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
                       setGlobalVariable("i:want_autocenter",0);
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
			{
			// FIXME : we need a mechanism for keeping the image pointer valid during multiple viewport usage
			//std::cout << "display " << " ( " << yorigin() << "," << xorigin() << " ) ";
			//std::cout << " " << " ( " << viewport_height() << "," << viewport_width() << " )\n";
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
					);}
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
		 *
		 * FIXME : this check is heavy.. move it downwards the call tree!
		 * */
		return check_valid() ? image : NULL;
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
#ifdef FIM_CACHE_DEBUG
		std::cout << "setting image \""<<ni->getName()<<"\" in viewport: "<< ni << "\n\n";
#endif

		//image = NULL;
		if(ni)free();
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
			setGlobalVariable("i:want_autocenter",1);
		}
		should_redraw();
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

        bool Viewport::check_valid()const
	{
		/*
		 * yes :)
		 * */
		return ! check_invalid();
	}

        bool Viewport::check_invalid()const
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
	void Viewport::scale_position_magnify(float factor)
	{
		/*
		 * scale image positioning variables by adjusting by a multiplying factor
		 * */
		if(factor<=0.0)return;
		left *= factor;
		top  *= factor;
		/*
		 * should the following be controlled by some optional variable ?
		 * */
		//if(!panned  /* && we_want_centering */ )
			this->recenter();
	}

	void Viewport::scale_position_reduce(float factor)
	{
		/*
		 * scale image positioning variables by adjusting by a multiplying factor
		 * */
		if(factor<=0.0)return;
		left /= factor;
		top  /= factor;
		//if(!panned  /* && we_want_centering */ )
			this->recenter();
	}

	void Viewport::recenter_horizontally()
	{
		left = (image->width() - this->viewport_width()) / 2;
	}

	void Viewport::recenter_vertically()
	{
		top = (image->height() - this->viewport_height()) / 2;
	}

	void Viewport::recenter()
	{
		if(!(panned & 0x02))recenter_horizontally();
		if(!(panned & 0x01))recenter_vertically();
	}

	void Viewport::should_redraw()const
	{
		/* FIXME */
		if(image)
			image->should_redraw();
		else
	        	displaydevice->redraw=1;
	}

	Viewport::~Viewport()
	{
		// FIXME : we need a revival for free()
		free();
	}
}

