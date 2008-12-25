/* $Id$ */
/*
 Image.cpp : Image manipulation and display

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
	extern FramebufferDevice ffd;
/*
 *	There is a general rule here:
 *	 Public functions should be safe when called in 
 *	 any internal state from the outside.
 *	 Private ones are stricter.
 * 
 */
	int Image::original_width()
	{
		//WARNING : assumes the image is valid
		if(orientation%2) return fimg->i.height;
		return fimg->i.width;
	}

	int Image::original_height()
	{
		//WARNING : assumes the image is valid
		if(orientation%2) return fimg->i.width;
		return fimg->i.height;
	}

	int Image::width()
	{
		//WARNING : assumes the image is valid
		return img->i.width;
	}

	int Image::height()
	{
		//WARNING : assumes the image is valid
		return img->i.height;
	}

	Image::Image(const char *fname_, FILE*fd):
		scale(0.0),
		ascale(0.0),
		newscale(0.0),
		angle(0.0),
		newangle(0.0),
		page(0),
                img     (NULL),
                fimg    (NULL),
		framebufferdevice(fim::ffd),
		orientation(0),
                invalid(0),
		no_file(true),
                fname     ("")

	{
		/*
		 *	an image object is created from an image filename
		 */
		reset();	// pointers blank
		if( !load(fname_,fd,0) || check_invalid() || (!fimg) ) 
		{
			cout << "warning : invalid loading ! \n";
			if( getGlobalIntVariable("_display_status_bar")||getGlobalIntVariable("_display_busy"))
				cc.set_status_bar( fim::string("error while loading \"")+ fim::string(fname_)+ fim::string("\"") , "*");
		}
		else
		{
		}
	}

	void Image::reset()
	{
		/*
		 * pointers are blanked and values set to default 
		 * */
                scale    = 1.0;
                newscale = 1.0;
                ascale   = 1.0;
                angle   = 0.0;
		setVariable("scale"  ,scale*100);
		setVariable("ascale" ,ascale);
		setVariable("angle" ,angle);
		no_file=true;	//reloading allowed

                invalid=0;
                fimg    = NULL;
                img     = NULL;
                orientation=0;
		setVariable("orientation" ,0);
	}
	
	bool Image::load(const char *fname_, FILE* fd, int want_page)
	{
		/*
		 *	an image is loaded and initializes this image.
		 *	returns false if the image does not load
		 */
		if(fname_==NULL && fname==""){return false;}//no loading = no state change
		this->free();
		fname=fname_;
		if( getGlobalIntVariable("_display_status_bar")||getGlobalIntVariable("_display_busy"))
			cc.set_status_bar("please wait while reloading...", "*");

		fimg = FbiStuff::read_image((char*)fname_,fd,want_page);
    		if(strcmp("/dev/stdin",fname_)==0)
		{
			no_file=true;	//no file is associated to this image (to prevent reloading)
		}
		else 
			no_file=false;	//reloading allowed

		img=fimg;	/* no scaling : one copy only */
	        {if(cc.displaydevice){should_redraw();}};

		if(! img)
		{
			cout<<"warning : image loading error!\n"   ;invalid=1;return false;
		}
		else page=want_page;


#ifdef FIM_NAMESPACES
		setVariable("height" ,(int)fimg->i.height);
		setVariable("width"  ,(int)fimg->i.width );
		setVariable("sheight",(int) img->i.height);
		setVariable("swidth" ,(int) img->i.width );
		setVariable("_fim_bpp" ,(int) framebufferdevice.fb_var.bits_per_pixel );
		setVariable("scale"  ,newscale*100);
		setVariable("ascale" ,ascale);
		setVariable("angle" , angle);
#endif

		setGlobalVariable("height" ,(int)fimg->i.height);
		setGlobalVariable("width"  ,(int)fimg->i.width );
		setGlobalVariable("sheight",(int) img->i.height);
		setGlobalVariable("swidth" ,(int) img->i.width );
		setGlobalVariable("_fim_bpp" ,(int) framebufferdevice.fb_var.bits_per_pixel );
		//setGlobalVariable("scale"  ,newscale*100);
		//setGlobalVariable("ascale" ,ascale);
		return true;
	}

	Image::~Image()
	{
		/*
		 * buffers are freed
		 * */
#ifdef FIM_CACHE_DEBUG
		std::cout << "freeing Image " << this << "\n";
#endif
		this->free();
	}

        int Image::tiny()const
	{
		/*
		 * image width or height is <= 1
		 * */
		if(!img)return 1; return ( img->i.width<=1 || img->i.height<=1 );
	}

	int Image::scale_multiply(double  sm)
	{
		/*
		 * current scale is multiplied by a factor
		 * */
		if(scale*sm>0.0)newscale=scale*sm;rescale();return 0;
	}

	int Image::scale_increment(double ds)
	{
		/*
		 * current scale is multiplied by a factor
		 * */
		if(scale+ds>0.0)newscale=scale+ds;rescale();return 0;
	}

	int Image::setscale(double ns)
	{
		/*
		 * a new scale is set
		 * */
		newscale=ns;rescale();
		return 0;
	}

        bool Image::check_valid()
	{
		/*
		 * well,why not ?
		 * */
		return ! check_invalid();
	}

        bool Image::check_invalid()
        {
                /*
		 * the image is declared invalid if the image structures are not loaded.
                 */

		//ACHTUNG! 
		if(!img ){img=fimg;}
                if(!img)
                {
                        invalid=1;
                        return true;
                }
		invalid=0;
                return false;
        }

        void Image::free()
        {
		/*
		 * the image descriptors are freed if necessary and pointers blanked
		 * */
                if(fimg!=img && img ) FbiStuff::free_image(img );
                if(fimg     ) FbiStuff::free_image(fimg);
                reset();
        }

// if the image rescaling mechanism is suspected of bugs, this will inhibit its use.
#define FIM_BUGGED_RESCALE 0

	int Image::rescale( float ns )
	{
		/*
		 * effective image rescaling
		 * */
#if FIM_BUGGED_RESCALE
		return 0;
#endif
		if(ns>0.0)newscale=ns;//patch

		if( check_invalid() ) return - 1;
		if(tiny() && newscale<scale){newscale=scale;return 0;}

		int neworientation=getOrientation();
		float	gascale=getGlobalFloatVariable("ascale"),
			newascale=getFloatVariable("ascale");
		newascale=(newascale>0.0 && newascale!=1.0)?newascale:((gascale>0.0 && gascale!=1.0)?gascale:1.0);
		
		//float newascale=getFloatVariable("ascale"); if(newascale<=0.0) newascale=1.0;
		/*
		 * The global angle variable value will override the local if not 0 and the local unset
		 * */
		float	gangle  =getGlobalFloatVariable("angle"),
			newangle=getFloatVariable("angle");
		newangle=angle?newangle:((gangle!=0.0)?gangle:newangle);

		if(	newscale == scale
			&& newascale == ascale
			&& neworientation == orientation
			//&& newangle == angle
			&& ( !newangle  && !angle )
		){return 0;/*no need to rescale*/}
		orientation=((neworientation%4)+4)%4; // fix this

		setGlobalVariable("scale",newscale*100);
		if(fimg)
		{
			/*
			 * In case of memory allocation failure, we would
			 * like to recover the current image  :) . 
			 *
			 * Here it would be nice to add some sort of memory manager 
			 * keeping score of copies and ... too complicated ...
			 */
			struct ida_image *backup_img=img;
			if(getGlobalIntVariable("_display_status_bar")||getGlobalIntVariable("_display_busy"))
				cc.set_status_bar("please wait while rescaling...", getInfo().c_str());

#define FIM_PROGRESSIVE_RESCALING 0
#if FIM_PROGRESSIVE_RESCALING
			/*
			 * progressive rescaling is computationally convenient in when newscale<scale
			 * at the cost of a progressively worsening image quality (especially when newscale~scale)
			 * and a sequence ----+ will suddenly 'clear' out the image quality, so it is not a desirable
			 * option ...
			 * */
			if( 
				//( newscale>scale && scale > 1.0) ||
				( newscale<scale && scale < 1.0) )
				img = scale_image( img,newscale/scale,newascale);
			else
				img = scale_image(fimg,newscale,newascale);
#else
			img = FbiStuff::scale_image(fimg,newscale,newascale);
#endif
			/* orientation can be 0,1,2,3 */
			if( img && orientation!=0 && orientation != 2)
			{
				// we make a backup.. who knows!
				// FIXME: should use a faster and memory-smarter method : in-place
				struct ida_image *rb=img;
				rb  = FbiStuff::rotate_image90(rb,orientation==1?0:1);
				if(rb)
				{
					FbiStuff::free_image(img);
					img=rb;
				}
			}
			if( img && orientation!=0 && orientation == 2)
			{	
				// we make a backup.. who knows!
				struct ida_image *rbb=NULL,*rb=NULL;
				// FIXME: should use a faster and memory-smarter method : in-place
				rb  = FbiStuff::rotate_image90(img,0);
				if(rb)rbb  = FbiStuff::rotate_image90(rb,0);
				if(rbb)
				{
					FbiStuff::free_image(img);
					FbiStuff::free_image(rb);
					img=rbb;
				}
				else
				{
					if(rbb)FbiStuff::free_image(rbb);
					if(rb )FbiStuff::free_image(rb);
				}
			}

			/* we rotate only in case there is the need to do so */
			if( img && ( angle != newangle || newangle) )
			{	
				// we make a backup.. who knows!
				struct ida_image *rbb=NULL,*rb=NULL;
				rb  = FbiStuff::rotate_image(img,newangle);
				if(rb)rbb  = FbiStuff::rotate_image(rb,0);
				if(rbb)
				{
					FbiStuff::free_image(img);
					FbiStuff::free_image(rb);
					img=rbb;
				}
				else
				{
					if(rbb)FbiStuff::free_image(rbb);
					if(rb )FbiStuff::free_image(rb);
				}
			}

			if(!img)
			{
				img=backup_img;
				if(getGlobalIntVariable("_display_busy"))
					cc.set_status_bar( "rescaling failed (insufficient memory?!)", getInfo().c_str());
				sleep(1);	//just to give a glimpse..
			}
			else 
			{
				/* reallocation succeeded */
				if( backup_img && backup_img!=fimg ) FbiStuff::free_image(backup_img);
				scale=newscale;
				ascale=newascale;
				angle =newangle;
	        		{if(cc.displaydevice){should_redraw();}};
			}

			/*
			 * it is important to set these values after rotation, too!
			 * */
			setGlobalVariable("height" ,(int)fimg->i.height);
			setGlobalVariable("width"  ,(int)fimg->i.width );
			setGlobalVariable("sheight",(int) img->i.height);
			setGlobalVariable("swidth" ,(int) img->i.width );
			setGlobalVariable("ascale" , ascale );
			//setGlobalVariable("angle"  ,  angle );
		}
		else {if(cc.displaydevice){cc.displaydevice->redraw=0;}};
		orientation=neworientation;
		return 0;
	}

	void Image::reduce(float factor)
	{
		/*
		 * scale is adjusted by a dividing factor
		 * */
		newscale = scale / factor;
		rescale();
	}

	void Image::magnify(float factor)
	{
		/*
		 * scale is adjusted by a multiplying factor
		 * */
		newscale = scale * factor;
		rescale();
	}

	/*
	void Image::resize(int nw, int nh)
	{
		//fixme
		if(check_invalid())return;
	}*/

	Image::Image(const Image& image):
		scale(image.scale),
		ascale(image.ascale),
		newscale(image.newscale),
		angle(image.angle),
		newangle(image.newangle),
		page(0),
                img     (NULL),
                fimg    (NULL),
		framebufferdevice(fim::ffd),
		orientation(image.orientation),
                invalid(0),
		no_file(true),
                fname     (image.fname)
	{
		/*
		 * builds a clone of this image.
		 * it should be completely independent from this object.
		 * */
		reset();
		img  = fbi_image_clone(image.img );
		fimg = fbi_image_clone(image.fimg);

		/* an exception is launched immediately */
		if(!img || !fimg)
			/* temporarily, for security reasons :  throw FIM_E_NO_IMAGE*/;
	}

	Image * Image::getClone()
	{
		/*
		 * returns a clone of this image.
		 * it should be an object completely independent from this.
		 * */
		return new Image(*this);
	}

/*
 *	Creates a little description of some image,
 *	and places it in a NUL terminated static buffer.
 */
fim::string Image::getInfo()
{
	/*
	 * a short information about the current image is returned
	 *
	 * WARNING:
	 * the returned info, if not NULL, belongs to a statical buffer which LIVES with the image!
	 */
	//FIX ME !
	if(!fimg)return "";

	static char linebuffer[128];
	char imagemode[3],*imp;
	int n=getGlobalIntVariable("fileindex");
	imp=imagemode;

	//if(getGlobalIntVariable("autoflip"))*(imp++)='F';
	//if(getGlobalIntVariable("automirror"))*(imp++)='M';

	// should flip ? should mirror ?
	int flip   =
	(((getGlobalIntVariable("autoflip")== 1)|(getGlobalIntVariable("v:flipped")== 1)|(getIntVariable("flipped")== 1))&&
	!((getGlobalIntVariable("autoflip")==-1)|(getGlobalIntVariable("v:flipped")==-1)|(getIntVariable("flipped")==-1)));
	int mirror   =
	(((getGlobalIntVariable("automirror")== 1)|(getGlobalIntVariable("v:mirrored")== 1)|(getIntVariable("mirrored")== 1))&&
	!((getGlobalIntVariable("automirror")==-1)|(getGlobalIntVariable("v:mirrored")==-1)|(getIntVariable("mirrored")==-1)));

	if(flip  )*(imp++)='F';
	if(mirror)*(imp++)='M';



	*imp='\0';
	snprintf(linebuffer, sizeof(linebuffer),
	     "%s%.0f%% %dx%d%s %d/%d",
	     /*fcurrent->tag*/ 0 ? "* " : "",
	     scale*100,
	     this->width(), this->height(),
	     imagemode,
	     n?n:1, /* ... */
	     (getGlobalIntVariable("filelistlen"))
	     );
	return fim::string(linebuffer);
}

	bool Image::update()
	{
		/*
		 * updates the image according to its variables
		 *
		 * FIXME: a temporary method
		 * */
		setVariable("fresh",0);

		/*
		 * rotation dispatch
		 * */
                int neworientation=getOrientation();
		if( neworientation!=orientation)
		{
			rescale();
			orientation=neworientation;
			return true;
		}
		return false;
	}

	int Image::getOrientation()
	{
		/*
		 * warning : this should work more intuitively
		 * */
		return ((
		(  getIntVariable("orientation")
		+getGlobalIntVariable("v:orientation")
		+getGlobalIntVariable("orientation")
		)
		%4)+4)%4;
	}

	int Image::rotate( float angle )
	{
		/*
		 * rotates the image the specified amount of degrees
		 * */
		float newangle=this->angle+angle;
		if( check_invalid() ) return -1;
		setVariable("angle",newangle);
		return rescale();	// FIXME : necessary *only* for image update and display
	}

	void Image::should_redraw()const
	{
	        cc.displaydevice->redraw=1;
	}

	bool Image::prev_page()
	{
		string s=fname;
		return load(s.c_str(),NULL,page-1);
	} 

	bool Image::next_page()
	{
		string s=fname;
		return load(s.c_str(),NULL,page+1);
	} 
}

