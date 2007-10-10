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

	Image::Image(const char *fname_)
	{
		/*
		 *	an image object is created from an image filename
		 */
		reset();	// pointers blank
		if( !load(fname_) || check_invalid() || (!fimg) ) 
		{
			cout << "warning : invalid loading ! \n";
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
                newascale= 1.0;
		setVariable("scale"  ,scale*100);
		setVariable("ascale" ,ascale);

                invalid=0;
                fimg    = NULL;
                img     = NULL;
                orientation=0;
                neworientation=0;
	}
	
	bool Image::load(const char *fname_)
	{
		/*
		 *	FIX ME
		 *	an image is loaded and initializes this image.
		 *	returns false if the image does not load
		 */
		if(fname_==NULL){return false;}//DANGER
		this->free();
		fname=fname_;
		if( cc.getIntVariable("_display_status_bar")||cc.getIntVariable("_display_busy"))
			set_status_bar("please wait while reloading...", "*");

		fimg = read_image((char*)fname_);

		img=fimg;	/* no scaling : one copy only */
	        redraw=1;

		if(! img){cout<<"warning : image loading error!\n"   ;invalid=1;return false;}


#ifdef FIM_NAMESPACES
		setVariable("height" ,(int)fimg->i.height);
		setVariable("width"  ,(int)fimg->i.width );
		setVariable("sheight",(int) img->i.height);
		setVariable("swidth" ,(int) img->i.width );
		setVariable("_fim_bpp" ,(int) fb_var.bits_per_pixel );
		setVariable("scale"  ,newscale*100);
		setVariable("ascale" ,ascale);
#endif

		cc.setVariable("height" ,(int)fimg->i.height);
		cc.setVariable("width"  ,(int)fimg->i.width );
		cc.setVariable("sheight",(int) img->i.height);
		cc.setVariable("swidth" ,(int) img->i.width );
		cc.setVariable("_fim_bpp" ,(int) fb_var.bits_per_pixel );
		cc.setVariable("scale"  ,newscale*100);
		cc.setVariable("ascale" ,ascale);
		return true;
	}

	Image::~Image()
	{
		/*
		 * buffers are freed
		 * */
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
                if(fimg!=img && img ) free_image(img );
                if(fimg     ) free_image(fimg);
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

//		neworientation=((cc.getIntVariable("orientation")%4)+4)%4;	/* ehm ...  */
#if 0
#ifdef FIM_NAMESPACES
		if( getIntVariable("v:orientation") == 0)
			neworientation=((   getIntVariable(  "orientation")%4)+4)%4;	/* ehm ...  */
		else
			neworientation=((cc.getIntVariable("v:orientation")%4)+4)%4;	/* ehm ...  */
#else
			neworientation=((cc.getIntVariable("orientation"  )%4)+4)%4;	/* ehm ...  */
#endif
#else
			neworientation=((  getIntVariable("orientation"  )%4)+4)%4;	/* ehm ...  */
#endif

		float newascale=getFloatVariable("ascale"); if(newascale<=0.0) newascale=1.0;
		if(newscale == scale && newascale == ascale && neworientation == orientation){return 0;/*no need to rescale*/}
		orientation=neworientation; // fix this

		cc.setVariable("scale",newscale*100);
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
			if(cc.getIntVariable("_display_status_bar")||cc.getIntVariable("_display_busy"))
				set_status_bar("please wait while rescaling...", getInfo().c_str());

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
			img = scale_image(fimg,newscale,newascale);
#endif
			/* orientation can be 0,1,2,3 */
			if( img && orientation!=0 && orientation != 2)
			{
				// we make a backup.. who knows!
				struct ida_image *rb=img;
				rb  = rotate_image(rb,orientation==1?0:1);
				if(rb)img=rb;
			}
			if( img && orientation!=0 && orientation == 2)
			{	
				// we make a backup.. who knows!
				struct ida_image *rb=img;
				rb  = rotate_image(rb,0);
				if(rb)rb  = rotate_image(img,0);
				if(rb)img=rb;
			}

			if(!img)
			{
				img=backup_img;
				if(cc.getIntVariable("_display_busy"))
					set_status_bar( "rescaling failed (insufficient memory?!)", getInfo().c_str());
				sleep(1);	//just to give a glimpse..
			}
			else 
			{
				/* reallocation succeeded */
				if( backup_img && backup_img!=fimg ) free_image(backup_img);
				scale=newscale;
				ascale=newascale;
				redraw=1;
 		                new_image=1; // for centering
			}


#if 0
			// debugging stuff
			int dc;
			dc=cc.getIntVariable("_dc");
			cc.setVariable("_dc" ,(int)dc+1);
#endif
			/*
			 * it is important to set these values after rotation, too!
			 * */
			cc.setVariable("height" ,(int)fimg->i.height);
			cc.setVariable("width"  ,(int)fimg->i.width );
			cc.setVariable("sheight",(int) img->i.height);
			cc.setVariable("swidth" ,(int) img->i.width );
			cc.setVariable("ascale" , ascale );
		}
		else redraw=0;
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
//		newascale(image.newascale),
		orientation(image.orientation),
		rotation(image.rotation),
		neworientation(image.neworientation),
		new_image(image.new_image)
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
	int n=cc.getIntVariable("fileindex");
	imp=imagemode;
	if(cc.getIntVariable("autoflip"))*(imp++)='F';
	if(cc.getIntVariable("automirror"))*(imp++)='M';
	*imp='\0';
	snprintf(linebuffer, sizeof(linebuffer),
	     "%s%.0f%% %dx%d%s %d/%d",
	     /*fcurrent->tag*/ 0 ? "* " : "",
	     scale*100,
	     this->width(), this->height(),
	     imagemode,
	     n?n:1, /* ... */
	     (cc.getIntVariable("filelistlen"))
	     );
	return fim::string(linebuffer);
}

}
