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
		//FIXME
		return fimg->i.width;
	}

	int Image::original_height()
	{
		//FIXME
		return fimg->i.height;
	}

	int Image::width()
	{
		//FIXME
		return img->i.width;
		return (img?img:fimg)->i.width;
	}

	int Image::height()
	{
		//FIXME
		return img->i.height;
		return (img?img:fimg)->i.height;
	}

	Image::Image(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		img=fimg=NULL; //this is important
		reset();
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
//			only_first_rescale=1; // WARNING !!
			//this->display();
			//if(cc.isInScript()==0)this->auto_scale();
		}
	}

	void Image::reset()
	{
		cout << "ReSeT!\n";
                scale    = 1.0;
                newscale = 1.0;
                ascale   = 1.0;
                newascale= 1.0;

                fimg    = NULL; // !
                invalid=0;
                img     = NULL;
                orientation=0;
                neworientation=0;

	}
	
	bool Image::load(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		if(fname_==NULL){invalid=1;return false;}//DANGER
		this->free();
		fname=fname_;
		if( cc.getIntVariable("_display_status_bar")||cc.getIntVariable("_display_busy"))set_status_bar("please wait while reloading...", "*");

		if(g_fim_no_framebuffer)
			fimg=NULL;
		else
			fimg = read_image((char*)fname_);

		img=fimg;
	        redraw=1;
		if(! img){cout<<"warning : image loading error!\n"   ;invalid=1;return false;}
//		if(!fimg){cout<<"warning : image allocation error!\n";invalid=1;return false;}
		return true;
	}

	Image::~Image()
	{
		this->free();
	}

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
//		reset();
		return  true;
		
	}

        int Image::tiny()const
	{
		if(!img)return 1; return ( img->i.width<=1 || img->i.height<=1 );
	}

	int Image::scale_multiply(double  sm)
	{
		if(scale*sm>0.0)newscale=scale*sm;rescale();return 0;
	}

	int Image::scale_increment(double ds)
	{
		if(scale+ds>0.0)newscale=scale+ds;rescale();return 0;
	}

	int Image::setscale(double ns)
	{
		newscale=ns;rescale();
		return 0;
	}


        bool Image::check_valid()
	{
		return ! check_invalid();
	}

        bool Image::check_invalid()
        {
                /*
                 *      WARNING ! was:

                        if(!img){return;}
                        if(!fimg){invalid=1;return;}
                */

		//ACHTUNG! 
		if(!img ){img=fimg;}
                if(!img)
                {
                        invalid=1;
                        return true;
                }
                return false;
        }

        void Image::free()
        {
                if(fimg!=img) free_image(img );
                if(fimg     ) free_image(fimg);
                reset();
        }

	int Image::rescale( float ns )
	{
		if(ns>0.0)newscale=ns;//patch
		/*
		 *	This code is bugful, when called from the constructor, on in a pre-user phase.
		 * 	20070401 hmm  i think it is ok now
		 */
		if( check_invalid() ) return - 1;
		if(tiny() && newscale<scale){newscale=scale;return 0;}

		if(g_fim_no_framebuffer)return 0;

		if(cc.noFrameBuffer())return 0;
		//FIX UPPER MEMORY CONSUMPTION LIMIT...
//		if(newscale > maxscale ) newscale = maxscale;
		neworientation=((cc.getIntVariable("orientation")%4)+4)%4;
		if(newscale == scale && newascale == ascale && neworientation == orientation){return 0;/*no need to rescale*/}
		orientation=neworientation; // fix this
//		scale_fix_top_left();
//		status(linebuffer, NULL);

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
			//FIXME !!!
/*				cout << " scale ; " << scale << "\n";
				cout << "ascale ; " <<ascale << "\n";
				cout << "w:"<<width()<<"\n";
				cout << "h:"<<height()<<"\n";*/

//			newscale=1.0;ascale=1.0;
//
			img  = scale_image(fimg,scale=newscale,cc.getFloatVariable("ascale"));
			if( img && orientation!=0 && orientation != 2)img  = rotate_image(img,orientation==1?0:1);
			if( img && orientation== 2)img  = flip_image(img);
			if(!img)
			{
				img=backup_img;
				if(cc.getIntVariable("_display_busy"))
					set_status_bar( "rescaling failed (insufficient memory!)", getInfo());
				sleep(1);	//just to give a glimpse..
			}
			else if(img!=fimg) free_image(backup_img);
			redraw=1;
 	                new_image=1; // for centering
			cc.setVariable("height",(int)fimg->i.height);
			cc.setVariable("sheight",(int)img->i.height);
			cc.setVariable("width",(int)fimg->i.width);
			cc.setVariable("swidth",(int)img->i.width);
		}
		else redraw=0;
		return 0;
	}


	void Image::reduce(float factor)
	{
//		factor = 2.0;scale=1.0;ascale=1.0;
		newscale = scale / factor;
		rescale();
	}

	void Image::magnify(float factor)
	{
//		factor = 2.0;scale=1.0;ascale=1.0;
		newscale = scale * factor;
		rescale();
	}

	char* Image::getInfo()
	{
		// ATENCION!
		if(fimg)return make_info(fimg,scale);return NULL;
	}

	void Image::resize(int nw, int nh)
	{
		//fixme
		if(check_invalid())return;
		
	}

	Image::Image(const Image& image)
	{
		// FIXME
		img=fimg=NULL; //this is important
		reset();
		memcpy(this,&image,sizeof(Image)); // very evil
		img  = fbi_image_clone(image.img );
		fimg = fbi_image_clone(image.fimg);
	}

	Image * Image::getClone()
	{
		return new Image(*this);
	}

}
