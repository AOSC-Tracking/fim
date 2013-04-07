/* $LastChangedDate$ */
/*
 Viewport.cpp : Viewport class implementation

 (c) 2007-2013 Michele Martone

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
#include <math.h>	// ceilf
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
			,FimWindow *window
#endif
			)
			:steps_(0)
			,hsteps_(0)
			,vsteps_(0)
			,psteps_(false)
			,top_(0)
			,left_(0)
			,panned_(0x0)
			,displaydevice_(c.displaydevice_)	/* could be NULL */
			,image_(NULL)
#ifdef FIM_WINDOWS
			,window_(window)
#endif
#ifdef FIM_NAMESPACES
			,Namespace(FIM_SYM_NAMESPACE_VIEWPORT_CHAR)
#endif
			,commandConsole(c)
	{
		// WARNING : this constructor will be filled soon
		if(!displaydevice_)
			throw FIM_E_TRAGIC;
		reset();
	}

	Viewport::Viewport(const Viewport &v)
		:
		steps_(v.steps_)
		,hsteps_(v.hsteps_)
		,vsteps_(v.vsteps_)
		,psteps_(v.psteps_)
		,top_(v.top_)
		,left_(v.left_)
		,panned_(v.panned_)
		,displaydevice_(v.displaydevice_)
		,image_(NULL)
#ifdef FIM_WINDOWS
		,window_(v.window_)
#endif
#ifdef FIM_NAMESPACES
		,Namespace(FIM_SYM_NAMESPACE_VIEWPORT_CHAR)
#endif
		,commandConsole(v.commandConsole)
	{
		// WARNING
		//reset();
		try
		{
#ifndef FIM_BUGGED_CACHE
	#ifdef FIM_CACHE_DEBUG
			if(v.image_) std::cout << "Viewport:Viewport():maybe will cache \"" <<v.image_->getName() << "\" from "<<v.image_<<"\n" ;
			else std::cout << "no image_ to cache..\n";
	#endif
			if(v.image_ && !v.image_->check_invalid()) setImage( commandConsole.browser_.cache_.useCachedImage(v.image_->getKey()) );
#else
			if(v.image_) setImage ( new Image(*v.image_) ) ;
#endif
		}
		catch(FimException e)
		{
			image_=NULL;
			std::cerr << "fatal error" << __FILE__ << ":" << __LINE__ << "\n";
		}
	}

	void Viewport::pan_up(fim_pan_t s)
	{
		panned_ |= 0x1;
		if(s<0)pan_down(-s);
		else
		{
			if(this->onTop())return;
			s=(s==0)?steps_:s;
			top_ -= s;
			should_redraw();
		}
	}

	void Viewport::pan_down(fim_pan_t s)
	{
		panned_ |= 0x1;
		if(s<0)pan_up(-s);
		else
		{
			if(this->onBottom())return;
			s=(s==0)?steps_:s;
			top_ += s;
			should_redraw();
		}
	}

	void Viewport::pan_right(fim_pan_t s)
	{
		panned_ |= 0x2;
		if(s<0)pan_left(s);
		else
		{
			if(onRight())return;
			s=(s==0)?steps_:s;
			left_+=s;
			should_redraw();
		}
	}

	void Viewport::pan_left(fim_pan_t s)
	{
		panned_ |= 0x2;
		if(s<0)pan_right(s);
		else
		{
			if(onLeft())return;
			s=(s==0)?steps_:s;
			left_-=s;
			should_redraw();
		}
	}

	bool Viewport::onBottom()
	{
		if( check_invalid() )return false;
		return (top_ + viewport_height() >= image_->height());
	}

	bool Viewport::onRight()
	{
		if( check_invalid() )return false;
		return (left_ + viewport_width() >= image_->width());
	}

	bool Viewport::onLeft()
	{
		if( check_invalid() )return false;
		return (left_ <= 0 );
	}

	bool Viewport::onTop()
	{
		if( check_invalid() )return false;
		return (top_ <= 0 );
	}

	fim_coo_t Viewport::viewport_width()
	{
		/*
		 * */
#ifdef FIM_WINDOWS
		if(window_)return window_->width();
		else return 0;
#else
		return displaydevice_->width();
#endif
	}

	fim_coo_t Viewport::viewport_height()
	{
		/*
		 * */
#ifdef FIM_WINDOWS
		if(window_)return window_->height();
		else return 0;
#else
		return displaydevice_->height();
#endif
	}

	void Viewport::bottom_align()
	{
		if(this->onBottom())return;
		if( check_valid() )top_ = image_->height() - this->viewport_height();
		should_redraw();
	}

	void Viewport::top_align()
	{
		if(this->onTop())return;
		top_=0;
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

	fim_coo_t Viewport::xorigin()
	{
		// horizontal origin coordinate (upper)
#ifdef FIM_WINDOWS
		return window_->xorigin();
#else
		return 0;
#endif
	}

	fim_coo_t Viewport::yorigin()
	{
		// vertical origin coordinate (upper)
#ifdef FIM_WINDOWS
		return window_->yorigin();
#else
		return 0;
#endif
	}

	void Viewport::null_display()
	{
		/*
		 * for recovery purposes. FIXME
		 * */
		if( displaydevice_->redraw_==FIM_REDRAW_UNNECESSARY )return;
#ifdef FIM_WINDOWS
		/* FIXME : note that fbi's clear_rect() is a buggy function and thus the fs_bpp multiplication need ! */
		{
			displaydevice_->clear_rect(
				xorigin(),
				xorigin()+viewport_width()-1,
				yorigin(),
				yorigin()+viewport_height()-1
				);
		}
#else
		/* FIXME */
		displaydevice_->clear_rect( 0, (viewport_width()-1)*displaydevice_->get_bpp(), 0, (viewport_height()-1));
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
		if((displaydevice_->redraw_==FIM_REDRAW_UNNECESSARY) )return false;
		if( check_invalid() ) null_display();//  NEW
		if( check_invalid() ) return false;
		/*
		 * should flip ? should mirror ?
		 *
		 * global or inner (not i: !) or local (v:) marker
		 * */
		int autotop=getGlobalIntVariable(FIM_VID_AUTOTOP)   | image_->getIntVariable(FIM_VID_AUTOTOP) | getIntVariable(FIM_VID_AUTOTOP);
		//int flip   =getGlobalIntVariable(FIM_VID_AUTOFLIP)  | image_->getIntVariable(FIM_VID_FLIPPED) | getIntVariable(FIM_VID_FLIPPED);
		int flip   =
		((getGlobalIntVariable(FIM_VID_AUTOFLIP)== 1)|(image_->getIntVariable(FIM_VID_FLIPPED)== 1)|(getIntVariable(FIM_VID_FLIPPED)== 1)&&
		!((getGlobalIntVariable(FIM_VID_AUTOFLIP)==-1)|(image_->getIntVariable(FIM_VID_FLIPPED)==-1)|(getIntVariable(FIM_VID_FLIPPED)==-1)));
		int mirror   =
		(((getGlobalIntVariable(FIM_VID_AUTOMIRROR)== 1)|(image_->getIntVariable(FIM_VID_MIRRORED)== 1)|(getIntVariable(FIM_VID_MIRRORED)== 1))&&
		!((getGlobalIntVariable(FIM_VID_AUTOMIRROR)==-1)|(image_->getIntVariable(FIM_VID_MIRRORED)==-1)|(getIntVariable(FIM_VID_MIRRORED)==-1)));
		int negate   =	/* FIXME : temporarily here */
		((getGlobalIntVariable(FIM_VID_AUTONEGATE)== 1)&&(image_->getIntVariable(FIM_VID_NEGATED)==0));
		int desaturate  =	/* FIXME : temporarily here */
		((getGlobalIntVariable(FIM_VID_AUTODESATURATE)== 1)&&(image_->getIntVariable(FIM_VID_DESATURATED)==0));
		image_->update();

		if(negate)
			image_->negate();
		if(desaturate)
			image_->desaturate();

		if (getGlobalIntVariable("i:"FIM_VID_WANT_AUTOCENTER)==1 && displaydevice_->redraw_!=FIM_REDRAW_UNNECESSARY  )
		{
			/*
			 * If this is the first image display, we have
			 * the right to rescale the image.
			 * */
			if(autotop && image_->height()>=this->viewport_height()) //THIS SHOULD BECOME AN AUTOCMD..
		  	{
			    top_=autotop>0?0:image_->height()-this->viewport_height();
			}
			/* start with centered image, if larger than screen */
			if (image_->width()  > this->viewport_width() )
				left_ = (image_->width() - this->viewport_width()) / 2;
			if (image_->height() > this->viewport_height() &&  autotop==0)
				top_ = (image_->height() - this->viewport_height()) / 2;
                       setGlobalVariable("i:"FIM_VID_WANT_AUTOCENTER,0);
		}
// uncommenting the next 2 lines will reintroduce a bug
//		else
//		if (displaydevice_->redraw_!=FIM_REDRAW_UNNECESSARY  ) 
		{
			/*	
			 *	20070911
			 *	this code is essential in order to protect from bad left_ and top_ values.
			 * */
			/*
			 * This code should be studied in detail..
			 * as it is is straight from fbi.
			 */
	    		if (image_->height() <= this->viewport_height())
	    		{
				top_ = 0;
	    		}
			else 
			{
				if (top_ < 0)
					top_ = 0;
				if (top_ + this->viewport_height() > image_->height())
		    			top_ = image_->height() - this->viewport_height();
	    		}
			if (image_->width() <= this->viewport_width())
			{
				left_ = 0;
	    		}
			else
			{
				if (left_ < 0)
				    left_ = 0;
				if (left_ + this->viewport_width() > image_->width())
			    		left_ = image_->width() - this->viewport_width();
		    	}
		}
		
		if(displaydevice_->redraw_!=FIM_REDRAW_UNNECESSARY)
		{
			displaydevice_->redraw_=FIM_REDRAW_UNNECESSARY;
			/*
			 * there should be more work to use double buffering (if possible!?)
			 * and avoid image tearing!
			 */
#ifdef FIM_WINDOWS
			if(commandConsole.displaydevice_ )
			{
			// FIXME : we need a mechanism for keeping the image pointer valid during multiple viewport usage
			//std::cout << "display " << " ( " << yorigin() << "," << xorigin() << " ) ";
			//std::cout << " " << " ( " << viewport_height() << "," << viewport_width() << " )\n";
			displaydevice_->display(
					image_->img_,
					top_,
					left_,
					image_->height(),
					image_->width(),
					image_->width(),
					yorigin(),
					xorigin(),
					viewport_height(),
					viewport_width(),
					viewport_width(),
					(mirror?FIM_FLAG_MIRROR:0)|(flip?FIM_FLAG_FLIP:0)/*flags : FIXME*/
					);}
#else
			displaydevice_->display(
					image_->img_,
					top_,
					left_,
					displaydevice_->height(),
					displaydevice_->width(),
					displaydevice_->width(),
					0,
					0,
					displaydevice_->height(),
					displaydevice_->width(),
					displaydevice_->width(),
					(mirror?FIM_FLAG_MIRROR:0)|(flip?FIM_FLAG_FLIP:0)/*flags : FIXME*/
					);
#endif					
			return true;
		}
		return false;
	}

	void Viewport::auto_scale()
	{
		fim_scale_t xs,ys;
		if( check_invalid() ) return;
		else
		{
			xs = (fim_scale_t)this->viewport_width()  / (fim_scale_t)(image_->original_width()*(image_->ascale_>0.0?image_->ascale_:1.0));
			ys = (fim_scale_t)this->viewport_height() / (fim_scale_t)image_->original_height();
		}

		image_->rescale(FIM_MIN(xs,ys));
	}

	void Viewport::auto_scale_if_bigger()
	{
		if( check_invalid() ) return;
		else
		{
			if((this->viewport_width()<(image_->original_width()*(image_->ascale_>0.0?image_->ascale_:1.0)))
			||(this->viewport_height() < image_->original_height()))
				auto_scale();
		}
	}

#if 0
	int Viewport::valid()
	{
		// int instead of bool
		return check_valid();
	}
#endif

        const Image* Viewport::c_getImage()const
	{
		/*
		 * returns the image pointer, regardless its use! 
		 *
		 * FIXME : this check is heavy.. move it downwards the call tree!
		 * */
		return check_valid() ? image_ : NULL;
	}

        Image* Viewport::getImage()const
	{
		/*
		 * returns the image pointer, regardless its use! 
		 * */
		return image_;
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

		//image_ = NULL;
		if(ni)free();
		reset();
		image_ = ni;
	}

        void Viewport::steps_reset()
	{
#ifdef FIM_WINDOWS
		steps_ = getGlobalIntVariable(FIM_VID_STEPS);
		if(steps_<FIM_CNS_STEPS_MIN)steps_ = FIM_CNS_STEPS_DEFAULT_N;
		else psteps_=(getGlobalStringVariable(FIM_VID_STEPS).re_match("%$"));

		hsteps_ = getGlobalIntVariable(FIM_VID_HSTEPS);
		if(hsteps_<FIM_CNS_STEPS_MIN)hsteps_ = steps_;
		else psteps_=(getGlobalStringVariable(FIM_VID_HSTEPS).re_match("%$"));

		vsteps_ = getGlobalIntVariable(FIM_VID_VSTEPS);
		if(vsteps_<FIM_CNS_STEPS_MIN)vsteps_ = steps_;
		else psteps_=(getGlobalStringVariable(FIM_VID_VSTEPS).re_match("%$"));
#else 
		hsteps_ = vsteps_ = steps_ = FIM_CNS_STEPS_DEFAULT_N;
		psteps_ = FIM_CNS_STEPS_DEFAULT_P;
#endif
	}

        void Viewport::reset()
        {
		/*
		 * resets some image flags and should reset the image position in the viewport
		 *
		 * FIXME
		 * */
		if(image_)
		{
			image_->reset();
			setGlobalVariable("i:"FIM_VID_WANT_AUTOCENTER,1);
		}
		should_redraw();
                top_  = 0;
                left_ = 0;
        	steps_reset();
        }

	void Viewport::auto_height_scale()
	{
		/*
		 * scales the image in a way to fit in the viewport height
		 * */
		fim_scale_t newscale;
		if( check_invalid() ) return;

		newscale = ((fim_scale_t)this->viewport_height()) / (fim_scale_t)image_->original_height();

		image_->rescale(newscale);
	}

	void Viewport::auto_width_scale()
	{
		/*
		 * scales the image in a way to fit in the viewport width
		 * */
		fim_scale_t newscale;
		if( check_invalid() ) return;

		newscale = ((fim_scale_t)this->viewport_width()) / ((fim_scale_t)image_->original_width()*(image_->ascale_>0.0?image_->ascale_:1.0));

		image_->rescale(newscale);
	}

	void Viewport::free()
	{
		/*
		 * frees the currently loaded image, if any
		 */
#ifndef FIM_BUGGED_CACHE
		if(image_)
		{	
			if( !commandConsole.browser_.cache_.freeCachedImage(image_) )
				delete image_;	// do it yourself :P
		}
#else
		// warning : in this cases exception handling is missing
		if(image_)delete image_;
#endif
		image_ = NULL;
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
		if(!image_)
			return true;
		else
			return image_->check_invalid();
	}

#ifdef FIM_WINDOWS
        void Viewport::reassignWindow(FimWindow *w)
	{
		window_ = w;
	}
#endif
	void Viewport::scale_position_magnify(fim_scale_t factor)
	{
		/*
		 * scale image positioning variables by adjusting by a multiplying factor
		 * */
		if(factor<=0.0)return;
		left_ = (fim_off_t)ceilf(((fim_scale_t)left_)*factor);
		top_  = (fim_off_t)ceilf(((fim_scale_t)top_ )*factor);
		/*
		 * should the following be controlled by some optional variable ?
		 * */
		//if(!panned_  /* && we_want_centering */ )
			this->recenter();
	}

	void Viewport::scale_position_reduce(fim_scale_t factor)
	{
		/*
		 * scale image positioning variables by adjusting by a multiplying factor
		 * */
		if(factor<=0.0)return;
		left_ = (fim_off_t)ceilf(((fim_scale_t)left_)/factor);
		top_  = (fim_off_t)ceilf(((fim_scale_t)top_ )/factor);
		//if(!panned_  /* && we_want_centering */ )
			this->recenter();
	}

	void Viewport::recenter_horizontally()
	{
		left_ = (image_->width() - this->viewport_width()) / 2;
	}

	void Viewport::recenter_vertically()
	{
		top_ = (image_->height() - this->viewport_height()) / 2;
	}

	void Viewport::recenter()
	{
		if(!(panned_ & 0x02))recenter_horizontally();
		if(!(panned_ & 0x01))recenter_vertically();
	}

	void Viewport::should_redraw()const
	{
		/* FIXME */
		if(image_)
			image_->should_redraw();
		else
	        	if(displaydevice_)displaydevice_->redraw_=FIM_REDRAW_NECESSARY;
	}

	Viewport::~Viewport()
	{
		// FIXME : we need a revival for free()
		free();
	}

	fim::string Viewport::pan(const fim_char_t*a1, const fim_char_t*a2)
	{
		// FIXME: a quick hack
		args_t args;
		if(a1)args.push_back(a1);
		if(a2)args.push_back(a2);
		return pan(args);
	}

	fim::string Viewport::pan(const args_t &args)
	{
		fim_pan_t hs=0,vs=0;
		fim_bool_t ps=false;
		fim_char_t f=FIM_SYM_CHAR_NUL,s=FIM_SYM_CHAR_NUL;
		const fim_char_t*fs=args[0].c_str();
		const fim_char_t*ss=NULL;
		if(args.size()<1 || (!fs))goto nop;
		f=tolower(*fs);
		if(!fs[0])goto nop;
		s=tolower(fs[1]);
        	steps_reset();
		// FIXME: write me
		if(args.size()>=2)
		{
			ps=((ss=args[1].c_str()) && *ss && ((ss[strlen(ss)-1])=='%'));
			vs=hs=(int)(args[1]);
		}
		else
		{
			ps=psteps_;
			vs=vsteps_;
			hs=hsteps_;
		}
		if(ps)
		{
			// FIXME: new, brittle
			vs=(viewport_height()*vs)/100;
			hs=(viewport_width()*hs)/100;
		}

		//std::cout << vs << " " << hs << " " << ps << "\n";

		switch(f)
		{
			case('u'):
				pan_up(vs);
			break;
			case('d'):
				pan_down(vs);
			break;
			case('r'):
				pan_right(hs);
			break;
			case('l'):
				pan_left(hs);
			break;
			case('n'):
			case('s'):
			if(f=='n') pan_up(vs);
			if(f=='s') pan_down(vs);
			switch(s)
			{
				case('e'):
					pan_left(hs);
				break;
				case('w'):
					pan_right(hs);
				break;
				default:
					goto err;
			}
			break;
			default:
			goto err;
		}
		// ...
nop:
		return FIM_CNS_EMPTY_RESULT;
err:
		return FIM_CNS_EMPTY_RESULT;
	}
}

