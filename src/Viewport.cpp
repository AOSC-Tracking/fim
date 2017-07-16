/* $LastChangedDate$ */
/*
 Viewport.cpp : Viewport class implementation

 (c) 2007-2017 Michele Martone

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
#include <cmath>	// ceilf
#define FIM_WANT_VIEWPORT_TRANSFORM 1
namespace fim
{
#if FIM_WANT_BDI
	static Image fim_dummy_img;
#endif	/* FIM_WANT_BDI */

	Viewport::Viewport(
			CommandConsole& c
#ifdef FIM_WINDOWS
			,const Rect & corners
#endif /* FIM_WINDOWS */
			)
			:
#ifdef FIM_NAMESPACES
			Namespace(&c,FIM_SYM_NAMESPACE_VIEWPORT_CHAR),
#endif /* FIM_NAMESPACES */
			psteps_(false),
			displaydevice_(c.displaydevice_)	/* could be FIM_NULL */
#ifdef FIM_WINDOWS
			,corners_(corners)
#endif /* FIM_WINDOWS */
			,image_(FIM_NULL)
			,commandConsole(c)
	{
		if(!displaydevice_)
			throw FIM_E_TRAGIC;
		reset();
	}

	Viewport::Viewport(const Viewport& rhs)
		:
#ifdef FIM_NAMESPACES
		Namespace(rhs),
#endif /* FIM_NAMESPACES */
		psteps_(rhs.psteps_)
		,displaydevice_(rhs.displaydevice_)
		,corners_(rhs.corners_)
		,image_(FIM_NULL)
		,commandConsole(rhs.commandConsole)
	{
		steps_ = rhs.steps_;
		hsteps_ = rhs.hsteps_;
		vsteps_ = rhs.vsteps_;
		top_ = rhs.top_;
		left_ = rhs.left_;
		panned_ = rhs.panned_;
		try
		{
	#ifdef FIM_CACHE_DEBUG
			if(rhs.image_)
				std::cout << "Viewport:Viewport():maybe will cache \"" <<rhs.image_->getName() << "\" from "<<rhs.image_<<FIM_CNS_NEWLINE ;
			else
				std::cout << "no image_ to cache..\n";
	#endif /* FIM_CACHE_DEBUG */
			if(rhs.image_ && !rhs.image_->check_invalid())
			{
				ViewportState viewportState;
				setImage( commandConsole.browser_.cache_.useCachedImage(rhs.image_->getKey(),&viewportState) );
				this->ViewportState::operator=(viewportState);
			}
		}
		catch(FimException e)
		{
			image_=FIM_NULL;
			std::cerr << "fatal error" << __FILE__ << ":" << __LINE__ << FIM_CNS_NEWLINE;
		}
	}

	fim_bool_t Viewport::pan_up(fim_pan_t s)
	{
		fim_bool_t rv = true;
		panned_ |= 0x1;
		if(s<0)
			rv = pan_down(-s);
		else
		{
			if(this->onTop())
				return false;
			s=(s==0)?steps_:s;
			top_ -= s;
		}
		return rv;
	}

	fim_bool_t Viewport::pan_down(fim_pan_t s)
	{
		fim_bool_t rv = true;
		panned_ |= 0x1;
		if(s<0)
			rv = pan_up(-s);
		else
		{
			if(this->onBottom())
				return false;
			s=(s==0)?steps_:s;
			top_ += s;
		}
		return rv;
	}

	fim_bool_t Viewport::pan_right(fim_pan_t s)
	{
		fim_bool_t rv = true;
		panned_ |= 0x2;
		if(s<0)
			rv = pan_left(-s);
		else
		{
			if(onRight())
				return false;
			s=(s==0)?steps_:s;
			left_+=s;
		}
		return rv;
	}

	fim_bool_t Viewport::pan_left(fim_pan_t s)
	{
		fim_bool_t rv = true;
		panned_ |= 0x2;
		if(s<0)
			rv = pan_right(-s);
		else
		{
			if(onLeft())
				return false;
			s=(s==0)?steps_:s;
			left_-=s;
		}
		return rv;
	}

	bool Viewport::onBottom(fim_coo_t approx_fraction)const
	{
		if( check_invalid() )
			return false;
		fim_coo_t tol = approx_fraction > 1 ? viewport_height() / approx_fraction : 0;
		return (top_ + viewport_height() + tol >= image_->height());
	}

	bool Viewport::onRight(fim_coo_t approx_fraction)const
	{
		if( check_invalid() )
			return false;
		fim_coo_t tol = approx_fraction > 1 ? viewport_width() / approx_fraction : 0;
		return (left_ + viewport_width() + tol >= image_->width());
	}

	bool Viewport::onLeft(fim_coo_t approx_fraction)const
	{
		if( check_invalid() )
			return false;
		fim_coo_t tol = approx_fraction > 1 ? viewport_width() / approx_fraction : 0;
		return (left_ <= tol );
	}

	bool Viewport::onTop(fim_coo_t approx_fraction)const
	{
		if( check_invalid() )
			return false;
		fim_coo_t tol = approx_fraction > 1 ? viewport_height() / approx_fraction : 0;
		return (top_ <= tol );
	}

	fim_coo_t Viewport::viewport_width(void)const
	{
#ifdef FIM_WINDOWS
		return corners_.width();
#else
		return displaydevice_->width();
#endif /* FIM_WINDOWS */
	}

	fim_coo_t Viewport::viewport_height(void)const
	{
		fim_coo_t fh = displaydevice_->status_line_height();
		if(fh)
			fh*=(getGlobalIntVariable(FIM_VID_DISPLAY_STATUS)==1?1:0);
#ifdef FIM_WINDOWS
		return corners_.height()-fh;
#else
		return displaydevice_->height()-fh;
#endif /* FIM_WINDOWS */
	}

	bool Viewport::redisplay(void)
	{
		/*
		 * we 'force' redraw.
		 * display() has still the last word :P
		 * */
		should_redraw();
		return display();
	}

	fim_coo_t Viewport::xorigin(void)
	{
		// horizontal origin coordinate (upper)
#ifdef FIM_WINDOWS
		return corners_.xorigin();
#else
		return 0;
#endif /* FIM_WINDOWS */
	}

	fim_coo_t Viewport::yorigin(void)
	{
		// vertical origin coordinate (upper)
#ifdef FIM_WINDOWS
		return corners_.yorigin();
#else /* FIM_WINDOWS */
		return 0;
#endif /* FIM_WINDOWS */
	}

	void Viewport::null_display(void)
	{
		/*
		 * for recovery purposes. FIXME
		 * */
		if(! need_redraw())
			return;
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
#else /* FIM_WINDOWS */
		/* FIXME */
		displaydevice_->clear_rect( 0, (viewport_width()-1)*displaydevice_->get_bpp(), 0, (viewport_height()-1));
#endif /* FIM_WINDOWS */
	}

	void Viewport::fs_multiline_puts(const char *str, fim_int doclear)
	{
		int fh=displaydevice_->f_ ? displaydevice_->f_->sheight():1; // FIXME : this is not clean
		int fw=displaydevice_->f_ ? displaydevice_->f_->swidth():1; // FIXME : this is not clean
		int sl = strlen(str), rw = viewport_width() / fw, wh = viewport_height();
		int cpl = displaydevice_->get_chars_per_line();

		if(doclear && cpl)
		{
			int lc = FIM_INT_FRAC(sl,cpl); /* lines count */
			displaydevice_->clear_rect(0, viewport_width()-1, 0, FIM_MIN(fh*lc,wh-1));
		}

		for( int li = 0 ; sl > rw * li ; ++li )
			if((li+1)*fh<wh) /* FIXME: maybe this check shall better reside in fs_puts() ? */
			displaydevice_->fs_puts(displaydevice_->f_, 0, fh*li, str+rw*li);
	}

	bool Viewport::shall_negate(void)const
	{
		return ((getGlobalIntVariable(FIM_VID_AUTONEGATE)== 1)&&(image_->getIntVariable(FIM_VID_NEGATED)==0));
	}

	bool Viewport::shall_desaturate(void)const
	{
		return ((getGlobalIntVariable(FIM_VID_AUTODESATURATE)== 1)&&(image_->getIntVariable(FIM_VID_DESATURATED)==0));
	}

	bool Viewport::shall_mirror(void)const
	{
		return 
		(((getGlobalIntVariable(FIM_VID_AUTOMIRROR)== 1)|(image_->getIntVariable(FIM_VID_MIRRORED)== 1)/*|(getIntVariable(FIM_VID_MIRRORED)== 1)*/)&&
		!((getGlobalIntVariable(FIM_VID_AUTOMIRROR)==-1)|(image_->getIntVariable(FIM_VID_MIRRORED)==-1)/*|(getIntVariable(FIM_VID_MIRRORED)==-1)*/));
	}

	bool Viewport::shall_autotop(void)const
	{
		return /*getGlobalIntVariable(FIM_VID_AUTOTOP)   |*/ image_->getIntVariable(FIM_VID_AUTOTOP) /* | getIntVariable(FIM_VID_AUTOTOP)*/;
	}

	bool Viewport::shall_flip(void)const
	{
		//return getGlobalIntVariable(FIM_VID_AUTOFLIP)  | image_->getIntVariable(FIM_VID_FLIPPED) | getIntVariable(FIM_VID_FLIPPED);
		return 
		((getGlobalIntVariable(FIM_VID_AUTOFLIP)== 1)|(image_->getIntVariable(FIM_VID_FLIPPED)== 1)/*|(getIntVariable(FIM_VID_FLIPPED)== 1)*/&&
		!((getGlobalIntVariable(FIM_VID_AUTOFLIP)==-1)|(image_->getIntVariable(FIM_VID_FLIPPED)==-1)/*|(getIntVariable(FIM_VID_FLIPPED)==-1)*/));
	}

	bool Viewport::display(void)
	{
		/*
		 *	the display function draws the image in the framebuffer
		 *	memory.
		 *	no scaling occurs, only some alignment.
		 *
		 *	returns true when some drawing occurred.
		 */
		if(! need_redraw())
			return false;
		if( check_invalid() )
			null_display();//  NEW
		if( check_invalid() )
			return false;

		fim_int autotop = shall_autotop();

		image_->update_meta();

		if(shall_negate())
			image_->negate();
		if(shall_desaturate())
			image_->desaturate();

		if ( ( autotop || getGlobalIntVariable("i:" FIM_VID_WANT_AUTOCENTER)==1 ) && need_redraw() )
		{
			if(autotop && image_->height()>=this->viewport_height())
		  	{
			    top_=autotop>0?0:image_->height()-this->viewport_height();
			    panned_ |= 0x1; // FIXME: shall do the same for l/r and introduce constants.
			}
			/* start with centered image, if larger than screen */
			if (image_->width()  > this->viewport_width() )
				left_ = (image_->width() - this->viewport_width()) / 2;
			if (image_->height() > this->viewport_height() &&  autotop==0)
				top_ = (image_->height() - this->viewport_height()) / 2;
			image_->set_auto_props(0, 0);
		}
		{
			/*
			 * Old fbi code snippets.
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
		
		if( need_redraw())
		{
			bool mirror = shall_mirror();
			bool flip = shall_flip();
			fim_flags_t flags = (mirror?FIM_FLAG_MIRROR:0)|(flip?FIM_FLAG_FLIP:0);

			should_redraw(FIM_REDRAW_UNNECESSARY);
			image_->should_redraw(FIM_REDRAW_UNNECESSARY);

#if FIM_WANT_VIEWPORT_TRANSFORM
			this->transform(mirror, flip);
#endif /* FIM_WANT_VIEWPORT_TRANSFORM */
#ifdef FIM_WINDOWS
			if(displaydevice_ )
			displaydevice_->display(
					image_->get_ida_image(),
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
					flags
					);
#else
			displaydevice_->display(
					image_->get_ida_image(),
					top_,
					left_,
					//displaydevice_->height(), displaydevice_->width(), displaydevice_->width(),
					viewport_height(), viewport_width(), viewport_width(),
					0,
					0,
					// displaydevice_->height(), displaydevice_->width(), displaydevice_->width(),
					viewport_height(), viewport_width(), viewport_width(),
					flags
					);
#endif					
#if FIM_WANT_VIEWPORT_TRANSFORM
			this->transform(mirror, flip);
#endif /* FIM_WANT_VIEWPORT_TRANSFORM */
#if FIM_WANT_PIC_CMTS
			/* FIXME: temporary; move to fs_puts_multiline() */
			if(image_)
			if(fim_int wcoi = getGlobalIntVariable(FIM_VID_COMMENT_OI))
			{
#if 0
				int fh=displaydevice_->f_ ? displaydevice_->f_->sheight():1; // FIXME : this is not clean
				int fw=displaydevice_->f_ ? displaydevice_->f_->swidth():1; // FIXME : this is not clean
				int sl = strlen(image_->getStringVariable(FIM_VID_COMMENT)), rw = viewport_width() / fw, wh = viewport_height();
				for( int li = 0 ; sl > rw * li ; ++li )
					if((li+1)*fh<wh) /* FIXME: maybe this check shall better reside in fs_puts() ? */
					displaydevice_->fs_puts(displaydevice_->f_, 0, fh*li, cmnts+rw*li);
#else
				if(commandConsole.isSetVar(FIM_VID_COMMENT_OI_FMT))
					this->fs_multiline_puts(image_->getInfoCustom(getGlobalStringVariable(FIM_VID_COMMENT_OI_FMT)),wcoi-1);
				else
					this->fs_multiline_puts(""/*image_->getStringVariable(FIM_VID_COMMENT)*/,wcoi-1);
#endif
			}
#endif /* FIM_WANT_PIC_CMTS */

#define FIM_WANT_BROWSER_PROGRESS_BAR 0 /* new */
#if FIM_WANT_BROWSER_PROGRESS_BAR 
			fim_float_t bp = commandConsole.browser_.file_progress() * 100.0;
			const fim_pan_t bw = 1; // bar width
			const fim_pan_t vw = viewport_width();
			const fim_pan_t vh = viewport_height();
			const fim_coo_t xc = vw; // x coordinate
			fim_color_t bc = FIM_CNS_WHITE;
			bc = FIM_CNS_WHITE;
			if(xc>bw) displaydevice_->fill_rect(xc-bw, xc, 0, FIM_FLT_PCNT_OF_100(bp,vh-1), bc);
			// if(xc>bw) displaydevice_->fill_rect(xc-bw, xc, FIM_FLT_PCNT_OF_100(bp,vh-1),vh, bc);
			// displaydevice_->fill_rect(0, 1, 0, FIM_FLT_PCNT_OF_100(bp,displaydevice_->height()-1), bc);
			// displaydevice_->fill_rect(0, FIM_FLT_PCNT_OF_100(bp,displaydevice_->width()-1), 0, 1, bc);
#endif /* FIM_WANT_BROWSER_PROGRESS_BAR */
			return true;
		}
		return false;
	}

	void Viewport::auto_scale(void)
	{
		fim_scale_t xs,ys;

		if( check_invalid() )
			return;
		else
			xs = viewport_xscale(),
			ys = viewport_yscale();

		image_->do_scale_rotate(FIM_MIN(xs,ys));
	}

	void Viewport::auto_scale_if_bigger(void)
	{
		if( check_invalid() )
			return;
		else
		{
			if((this->viewport_width()<(image_->original_width()*image_->ascale()))
			||(this->viewport_height() < image_->original_height()))
				auto_scale();
		}
	}

        Image* Viewport::getImage(void)const
	{
		/* returns the image pointer, regardless its use!  */
#if FIM_WANT_BDI
		if(!image_)
			return &fim_dummy_img;
		else
#endif	/* FIM_WANT_BDI */
#if FIM_IMG_NAKED_PTRS
			return image_;
#else /* FIM_IMG_NAKED_PTRS */
			return image_.get();
#endif /* FIM_IMG_NAKED_PTRS */
	}

        const Image* Viewport::c_getImage(void)const
	{
		/* returns the image pointer, regardless its use!  */
		return getImage();
	}

        void Viewport::setImage(fim::ImagePtr ni)
	{
#ifdef FIM_CACHE_DEBUG
		std::cout << "setting image \""<<ni->getName()<<"\" in viewport: "<< ni << "\n\n";
#endif /* FIM_CACHE_DEBUG */

		if(ni)
			free_image();
		reset();
		image_ = ni;
	}

        void Viewport::steps_reset(void)
	{
#ifdef FIM_WINDOWS
		steps_ = getGlobalIntVariable(FIM_VID_STEPS);
		if(steps_<FIM_CNS_STEPS_MIN)
			steps_ = FIM_CNS_STEPS_DEFAULT_N;
		else
			psteps_=(getGlobalStringVariable(FIM_VID_STEPS).re_match("%$"));

		hsteps_ = getGlobalIntVariable(FIM_VID_HSTEPS);
		if(hsteps_<FIM_CNS_STEPS_MIN)
			hsteps_ = steps_;
		else psteps_=(getGlobalStringVariable(FIM_VID_HSTEPS).re_match("%$"));

		vsteps_ = getGlobalIntVariable(FIM_VID_VSTEPS);
		if(vsteps_<FIM_CNS_STEPS_MIN)
			vsteps_ = steps_;
		else psteps_=(getGlobalStringVariable(FIM_VID_VSTEPS).re_match("%$"));
#else  /* FIM_WINDOWS */
		hsteps_ = vsteps_ = steps_ = FIM_CNS_STEPS_DEFAULT_N;
		psteps_ = FIM_CNS_STEPS_DEFAULT_P;
#endif /* FIM_WINDOWS */
	}

        void Viewport::reset(void)
        {
		/*
		 * resets some image flags and should reset the image position in the viewport
		 *
		 * FIXME
		 * */
		if(image_)
			image_->reset_state(),
			image_->set_auto_props(1, 0);
		should_redraw();
                top_  = 0;
                left_ = 0;
        	steps_reset();
        }

	void Viewport::auto_height_scale(void)
	{
		/*
		 * scales the image in a way to fit in the viewport height
		 * */
		fim_scale_t newscale;
		if( check_invalid() )
			return;

		newscale = FIM_INT_SCALE_FRAC(this->viewport_height(),static_cast<fim_scale_t>(image_->original_height()));

		image_->do_scale_rotate(newscale);
	}

	void Viewport::auto_width_scale(void)
	{
		/*
		 * scales the image in a way to fit in the viewport width
		 * */
		if( check_invalid() )
			return;
		image_->do_scale_rotate(viewport_xscale());
	}

	void Viewport::free_image(void)
	{
		if(image_)
			commandConsole.browser_.cache_.freeCachedImage(image_,this);
		image_ = FIM_NULL;
	}

        bool Viewport::check_valid(void)const
	{
		return ! check_invalid();
	}

        bool Viewport::check_invalid(void)const
	{
		if(!image_)
			return true;
		else
			return image_->check_invalid();
	}

	void Viewport::scale_position_magnify(fim_scale_t factor)
	{
		/*
		 * scale image positioning variables by adjusting by a multiplying factor
		 * */
		if(factor<=0.0)
			return;
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
		if(factor<=0.0)
			return;
		left_ = (fim_off_t)ceilf(FIM_INT_SCALE_FRAC(left_,factor));
		top_  = (fim_off_t)ceilf(FIM_INT_SCALE_FRAC(top_ ,factor));
		//if(!panned_  /* && we_want_centering */ )
			this->recenter();
	}

	void Viewport::recenter_horizontally(void)
	{
		if(image_)
			left_ = (image_->width() - this->viewport_width()) / 2;
	}

	void Viewport::recenter_vertically(void)
	{
		if(image_)
			top_ = (image_->height() - this->viewport_height()) / 2;
	}

	void Viewport::recenter(void)
	{
		if(!(panned_ & 0x02))
			recenter_horizontally();
		if(!(panned_ & 0x01))
			recenter_vertically();
	}

	Viewport::~Viewport(void)
	{
		free_image();
	}

	fim::string Viewport::pan(const fim_char_t*a1, const fim_char_t*a2)
	{
		// FIXME: a quick hack
#if FIM_USE_CXX11
		if(a1 || a2)
			return pan({ (a1 ? a1 : a2 ) });
		else
			return pan({});
#else /* FIM_USE_CXX11 */
		args_t args;
		if(a1)
			args.push_back(a1);
		if(a2)
			args.push_back(a2);
		return pan(args);
#endif /* FIM_USE_CXX11 */
	}

	bool Viewport::place(const fim_pan_t px, const fim_pan_t py)
	{
		/* FIXME: find a nicer name. */
		/* FIXME: shall merge this in an internal pan function. */
		fim_pan_t _px = px, _py = py;

#if FIM_WANT_VIEWPORT_TRANSFORM
		if(image_ && image_->is_flipped()) /* FIXME: this is only i: ... */
			_py = 100 - _py;
		if(image_ && image_->is_mirrored()) /* FIXME: this is only i: ... */
			_px = 100 - _px;
#endif /* FIM_WANT_VIEWPORT_TRANSFORM */

		if(image_)
		{
			fim_pan_t ih = image_->height();
			fim_pan_t iw = image_->width();
			fim_pan_t vh = this->viewport_height();
			fim_pan_t vw = this->viewport_width();
			fim_off_t top = top_, left = left_;

			if(ih>vh)
				top = FIM_INT_PCNT_SAFE(_py,ih-vh);
			if(iw>vw)
				left = FIM_INT_PCNT_SAFE(_px,iw-vw);

			if( top != top_ || left != left_ )
			{
				top_ = top;
			       	left_ = left;
				should_redraw();
			}
		}
		return true;
	}

	fim::string Viewport::pan(const args_t& args)
	{
		fim::string result = FIM_CNS_EMPTY_RESULT;
		/* FIXME: unfinished */
		fim_pan_t hs=0,vs=0;
		fim_bool_t ps=false;
		fim_char_t f=FIM_SYM_CHAR_NUL,s=FIM_SYM_CHAR_NUL;
		const fim_char_t*const fs=args[0];
		const fim_char_t*ss=FIM_NULL;
		fim_bool_t prv = true;

		if(args.size()<1 || (!fs))
			goto nop;
		f=tolower(*fs);
		if(!fs[0])
			goto nop;
		s=tolower(fs[1]);
        	steps_reset();
		// FIXME: write me
		if(args.size()>=2)
		{
			ps=((ss=args[1]) && *ss && ((ss[strlen(ss)-1])=='%'));
			vs=hs=(int)(args[1]);
		}
		else
		{
			ps = psteps_;
			vs = vsteps_;
			hs = hsteps_;
		}
		if(ps)
		{
			// FIXME: new, brittle
			vs = FIM_INT_PCNT(viewport_height(),vs);
			hs = FIM_INT_PCNT(viewport_width(), hs);
		}

		//std::cout << vs << " " << hs << " " << ps << FIM_CNS_NEWLINE;
		
#if FIM_WANT_VIEWPORT_TRANSFORM
		if(image_ && image_->is_flipped()) /* FIXME: this is only i: ... */
			vs=-vs;
		if(image_ && image_->is_mirrored()) /* FIXME: this is only i: ... */
			hs=-hs;
#endif /* FIM_WANT_VIEWPORT_TRANSFORM */

		switch(f)
		{
			case('u'):
				prv=pan_up(vs);
			break;
			case('d'):
				prv=pan_down(vs);
			break;
			case('r'):
				prv=pan_right(hs);
			break;
			case('l'):
				prv=pan_left(hs);
			break;
			case('n'):
			case('s'):
			if(f=='n')
		       		prv=pan_up(vs);
			if(f=='s')
			       	prv=pan_down(vs);
			switch(s)
			{
				case('e'):
					prv=pan_left(hs);
				break;
				case('w'):
					prv=pan_right(hs);
				break;
				default:
					goto err;
			}
			break;
			default:
			goto err;
		}
		should_redraw();
nop:
		result = "maybe";
		if(prv==false)
			result = "no"; // TODO: this is ugly
		return result;
err:
		return result;
	}

	size_t Viewport::byte_size(void)const
	{
		size_t bs = 0;
		bs += sizeof(*this);
		return bs;
	}

	int Viewport::snprintf_centering_info(char *str, size_t size)const
	{
		int vum,vlm;
		int hum,hlm;
		float va,ha;
		char vc='c',hc='c';
		int res=0;
		const char*cs=FIM_NULL;
		const char*es="";
		const float bt=99.0;
		const float ct=1.0;

		vum = top_;
	        vlm = image_->height() - viewport_height() - vum;
		hum = left_;
	       	hlm = image_->width() - viewport_width() - hum;
		vum = FIM_MAX(vum,0);
		vlm = FIM_MAX(vlm,0);
		hum = FIM_MAX(hum,0);
		hlm = FIM_MAX(hlm,0);

		if(vum<1 && vlm<1)
		{
			va=0.0;
			// res = snprintf(str+res, size-res, "-");
		}
		else
		{
			va=((float)(vum))/((float)(vum+vlm));
			va-=0.5;
			va*=200.0;
			if(va<0.0)
			{
				va*=-1.0,vc='^';
				if(va>=bt)
					cs="top";
			}
			else
			{
				vc='v';
				if(va>=bt)
					cs="bot";
			}
			if(va<ct)
				cs="";
			if(cs)
				res += snprintf(str+res, size-res, "%s%s",es,cs);
			else
				res += snprintf(str+res, size-res, "%s%2.0f%%%c",es,va,vc);
		}
	
		if(cs)
			es=" ",cs=FIM_NULL;

		if(hum<1 && hlm<1)
			ha=0.0;
		else
		{
			ha=((float)(hum))/((float)(hum+hlm));
		       	ha-=0.5;
			ha*=200.0;
			if(ha<0.0)
			{
				ha*=-1.0,hc='<';
				if(ha>=bt)
					cs="left";
			}
			else
			{
				hc='>';
				if(ha>=bt)
					cs="right";
			}
			if(ha<ct)
				cs="";
			if(cs)
				res += snprintf(str+res, size-res, "%s%s",es,cs);
			else
				res += snprintf(str+res, size-res, "%s%2.0f%%%c",es,ha,hc);
		}
		return res;
	}

	void Viewport::align(const char c)
	{
		switch( c )
		{
		case 'b':
		{
			if(this->onBottom())
				goto ret;
			if( check_valid() )
				top_ = image_->height() - this->viewport_height();
		}
		break;
		case 't':
		{
			if(this->onTop())
				goto ret;
			top_=0;
		}
		break;
		case 'l':
			left_=0;
		break;
		case 'r':
	       		left_ = image_->width() - viewport_width();
		break;
		case 'c':
			//this->recenter();
			this->recenter_vertically(); this->recenter_horizontally();
		break;
		default:
		goto ret;
		}
		should_redraw();
ret:
		return;
	}

	Viewport& Viewport::operator= (const Viewport&rhs){return *this;/* a disabled assignment */}

	void Viewport::transform(bool mirror, bool flip)
	{
		if(mirror)
		{ if( image_ && image_->width() > viewport_width() ) left_ = image_->width() - viewport_width() - left_; }
		if(flip)
		{ if( image_ && image_->height() > viewport_height() ) top_ = image_->height() - viewport_height() - top_; }
	}
	
	fim_bool_t Viewport::need_redraw(void)const
	{
		return ( ( redraw_ != FIM_REDRAW_UNNECESSARY ) || ( image_ && image_->need_redraw() ) || ( displaydevice_ && displaydevice_->need_redraw() ) );
       	}
	void Viewport::should_redraw(enum fim_redraw_t sr)
       	{
		redraw_ = sr;
       	} 

	fim_scale_t Viewport::viewport_xscale(void)const
	{
		assert(image_);
		return FIM_INT_SCALE_FRAC(this->viewport_width(),image_->original_width()*image_->ascale());
	}

	fim_scale_t Viewport::viewport_yscale(void)const
	{
		assert(image_);
		return FIM_INT_SCALE_FRAC(this->viewport_height(),static_cast<fim_scale_t>(image_->original_height()));
	}
}

