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
		return fimg->i.width;
	}

	int Image::original_height()
	{
		//WARNING : assumes the image is valid
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
			/*
			 *	this is a 'patch' to do a first setting of top,left,etc
			 *	variables prior to first visualization without displaying..
			 */
			cc.setVariable("filename",fname_);
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

		if(g_fim_no_framebuffer)
			fimg=NULL;
		else
			fimg = read_image((char*)fname_);

		img=fimg;	/* no scaling : one copy only */
	        redraw=1;

		if(! img){cout<<"warning : image loading error!\n"   ;invalid=1;return false;}

		cc.setVariable("height" ,(int)fimg->i.height);
		cc.setVariable("width"  ,(int)fimg->i.width );
		cc.setVariable("sheight",(int) img->i.height);
		cc.setVariable("swidth" ,(int) img->i.width );
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

	/*
	bool Image::revertToLoaded()
	{
		if(img==fimg) img = NULL;
		if( img )//in the case img is a third copy (should not occur)
		{
			free_image(img);
			img = fimg;
	        	redraw=1;
		}
		if(!fimg) return false; // this is bad, but could occur!
		return  true;
		
	}*/

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

		if(g_fim_no_framebuffer)return 0;

		neworientation=((cc.getIntVariable("orientation")%4)+4)%4;	/* ehm ...  */
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
				set_status_bar("please wait while rescaling...", getInfo());

			img  = scale_image(fimg,scale=newscale,cc.getFloatVariable("ascale"));

			/* orientation can be 0,1,2,3 */
			if( img && orientation!=0 && orientation != 2)img  = rotate_image(img,orientation==1?0:1);
			if( img && orientation== 2)img  = flip_image(img);

			if(!img)
			{
				img=backup_img;
				if(cc.getIntVariable("_display_busy"))
					set_status_bar( "rescaling failed (insufficient memory?!)", getInfo());
				sleep(1);	//just to give a glimpse..
			}
			else 
			{
				if( backup_img && backup_img!=fimg ) free_image(backup_img);
			}


			redraw=1;
 	                new_image=1; // for centering

			cc.setVariable("height" ,(int)fimg->i.height);
			cc.setVariable("width"  ,(int)fimg->i.width );
			cc.setVariable("sheight",(int) img->i.height);
			cc.setVariable("swidth" ,(int) img->i.width );
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

	char* Image::getInfo()
	{
		/*
		 * a short information about the current image is returned
		 *
		 * WARNING:
		 * the returned info, if not NULL, belongs to a statical buffer which LIVES with the image!
		 */
		if(fimg)return make_info();
		return NULL;
	}

	/*
	void Image::resize(int nw, int nh)
	{
		//fixme
		if(check_invalid())return;
	}*/

	Image::Image(const Image& image)
	{
		/*
		 * builds a clone of this image.
		 * it should be completely independent from this object.
		 * */
		// FIXME
		reset();
		memcpy(this,&image,sizeof(Image)); // very evil
		img  = fbi_image_clone(image.img );
		fimg = fbi_image_clone(image.fimg);

		/* an exception is launched immediately */
		if(!img || !fimg)
			throw FIM_E_NO_IMAGE;
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
 *	and plates it in a NUL terminated static buffer.
 */
char *Image::make_info()
{
	//FIX ME !
	static char linebuffer[128];
	char imagemode[3],*imp;
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
	     (cc.getIntVariable("fileindex")),
	     (cc.getIntVariable("filelistlen"))
	     );
	return linebuffer;
}

}
