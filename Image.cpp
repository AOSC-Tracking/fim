#include "Image.h"

namespace fim
{
/*
 *	There is a general rule here:
 *	 Public functions should be safe when called in 
 *	 any internal state from the outside.
 *	 Private ones are stricter.
 * 
 */
	void Image::auto_scale()
	{
		float xs,ys;
//		if(!img){img=fimg;}
		if(!img){return;}
		if(!fimg){invalid=1;return;}
#ifndef FIM_NOFB
		xs = (float)fb_var.xres / fimg->i.width;
		ys = (float)fb_var.yres / fimg->i.height;
#else
		xs=ys=10.0f;
#endif
		newscale = (xs < ys) ? xs : ys;
//		cout << "autoscale  xs:"  << xs << ",ys:" << ys<< "\n";
		rescale();
	}

	void Image::auto_height_scale()
	{
		if(!img){img=fimg;}
		if(!img){return;}
		if(!fimg){invalid=1;return;}
//		if(cc.isInScript())return;
#ifndef FIM_NOFB
		newscale = (float)fb_var.yres / fimg->i.height;
#endif
		rescale();
	}

	void Image::auto_width_scale()
	{
//		if(!img){img=fimg;}
		if(!img){return;}
		if(!fimg){invalid=1;return;}
//		if(cc.isInScript())return;
#ifndef FIM_NOFB
		newscale = (float)fb_var.xres / fimg->i.width;
#endif
		rescale();
	}

	int Image::rescale()
	{
		/*
		 *	THIS CODE IS BUGFUL, WHEN CALLED FROM THE CONSTRUCTOR...
		 *	...OR IN THE PRE-USER PHASE..
		 */
		 //	so look at this code as a patch:
		 //
		//TODO : MAX MEMORY INSTEAD OF MAX SCALE!!
		float maxscale=10.0;
		float minscale= 0.1;
	    	if(!img)invalid=1;
	    	if(!img)return -1;
	    	if(invalid)return -1;//IN CASE OF MEMORY PROBLEMS
#ifndef FIM_NOFB
		if(cc.noFrameBuffer())return 0;
		//FIX UPPER MEMORY CONSUMPTION LIMIT...
#define MAXMEM 100*1000*1000.0
		maxscale = MAXMEM /(float)( fimg->i.width * fimg->i.height * 3 );
#undef MAXMEM 
		//if (newscale < minscale ) newscale = minscale;
		if (newscale > maxscale ) newscale = maxscale;
//		cout << "scale  :"  << scale << "-> " << newscale<< "\n";
		scale_fix_top_left();
//		sprintf(linebuffer,"scaling (%.0f%%) %s ...", scale*100, fname);
//		status(linebuffer, NULL);
		if(simg)free_image(simg);
		if(fimg)
		{
			simg = scale_image(fimg,scale);
			img = simg;
		}
#endif
		redraw=1;
	}

	void Image::redisplay()
	{
		display();
	}

	void Image::display()
	{
		/*
		 *	FIX ME
		 */
		/*
		 *	the display function draws the image in the frame buffer
		 *	memory.
		 *	no scaling occurs, only some alignment.
		 */
		if(redraw==0 || cc.noFrameBuffer())return;

		if(!img)invalid=1;//IN CASE OF MEMORY PROBLEMS
		if(invalid)return;//IN CASE OF MEMORY PROBLEMS
		
		int autotop=cc.getIntVariable("autotop");

#ifndef FIM_NOFB
		if (new_image && redraw)
		{
			if(autotop && img->i.height>=fb_var.yres) //THIS SHOULD BECOME AN AUTOCMD..
		  	{
			    top=autotop>0?0:img->i.height-fb_var.yres;
			}
			/* start with centered image, if larger than screen */
			if (img->i.width > fb_var.xres )
				left = (img->i.width - fb_var.xres) / 2;
//			if (img->i.height > fb_var.yres && !textreading)
			if (img->i.height > fb_var.yres &&  autotop==0)
				top = (img->i.height - fb_var.yres) / 2;
			new_image = 0;
		}
		else
		//if (redraw  ) 
		{
	    		if (img->i.height <= fb_var.yres)
	    		{
				top = 0;
	    		}
			else 
			{
				if (top < 0)
					top = 0;
				if (top + fb_var.yres > img->i.height)
		    			top = img->i.height - fb_var.yres;
	    		}
			if (img->i.width <= fb_var.xres)
			{
				left = 0;
	    		}
			else
			{
				if (left < 0)
				    left = 0;
				if (left + fb_var.xres > img->i.width)
			    		left = img->i.width - fb_var.xres;
		    	}
		}
		if(only_first_rescale){only_first_rescale=0;return;}
		
		while(fb_switch_state!=0)//FB_ACTIVE
		{
	    		fb_switch_state=3;//FB_ACQ_REQ;
			console_switch(1);
		}
		if(redraw)
		{
			redraw=0;
			/*
			 * there should be more work to use double buffering
			 * and avoid image tearing!
			 */
			fb_clear_screen();
			svga_display_image(img, left, top);
		}
		while(fb_switch_state!=2)//fb_inactive
		{
	  		//  	fb_switch_state=0;
			//    console_switch(1);
			return;
		}
/*	    while(switch_last != fb_switch_state)
            {
		    console_switch(0);
	    }*/
//	    if (switch_last != fb_switch_state) { console_switch(0);}
//	    status(desc, info);
	

//	    	status_screen(desc, ( char*)info);
//    		cout << "displaying\n";
  //  		cout << desc << "\n";
#endif
	}

	Image::Image(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		reset();
		load(fname_);
		if(invalid || (!img) || (!fimg))
		{
			invalid=1;
			cout << "invalid loading ! \n";
		}
		else
		{
			/*
			 *	this is a 'patch' to do a first setting of top,left,etc
			 *	variables prior to first visualization without displaying..
			 */
			only_first_rescale=1;
			this->display();
			//if(cc.isInScript()==0)this->auto_scale();
			//this->auto_scale();
			//this->magnify();
			//this->rescale();
		}
	}

	void Image::reset()
	{
		new_image=1;
		redraw=1;
		scale    = 1;
		newscale = 1;
		editable = 0; once = 0;
		steps = 50;
		len =0;
		top = 0;
		left = 0;
		pos= 0;
		fimg    = NULL;
		simg    = NULL;
		img     = NULL;
		fname = NULL;
		desc = NULL;
		info = NULL;
		invalid=0;//!!!!!!!!!!!
	}
	
	void Image::scale_fix_top_left()
	{
		/*
		 * this function operates on the image currently in memory
		 */
#ifndef FIM_NOFB
		float old=scale;float fnew=newscale;
		unsigned int width, height;
		float cx,cy;
		cx = (float)(left + fb_var.xres/2) / (img->i.width  * old);
		cy = (float)(top  + fb_var.yres/2) / (img->i.height * old);
		width  = (int)(img->i.width  * fnew);
		height = (int)(img->i.height * fnew);
		left   = (int)(cx * width  - fb_var.xres/2);
		top    = (int)(cy * height - fb_var.yres/2);
		//the cast was added by me...
		scale = newscale;
#endif
	}

/*
void Image::scale_fix_top_left()
{
	unsigned int width, height;
	float cx,cy;
	cx = (float)(left + fb_var.xres/2) / (img->i.width  * scale);
	cy = (float)(top  + fb_var.yres/2) / (img->i.height * scale);
	width  = (int)(img->i.width  * newscale);
	height = (int)(img->i.height * newscale);

	left   = (int)(cx * width ) - fb_var.xres/2;
	top    = (int)(cy * height) - fb_var.yres/2;
	scale = newscale;
}*/

	void Image::load(const char *fname_)
	{
		/*
		 *	FIX ME
		 */
		if(fname_==NULL){invalid=1;return;}
//		cout << "..loading "<< fname_ << "\n";
		free_mem();
		fname = dupstr(fname_);
		assert(fname);
#ifndef FIM_NOFB
		fimg = read_image(fname);
#else
		fimg=NULL;
#endif
//		desc = make_desc(&fimg->i,fname);
//	    	info = make_info(fimg,scale);
//		desc = info = "foo";
		desc = info = "";
//		cout << fname << "\n";//return;
		img=fimg;
//		assert(img);
	        redraw=1;
		if(! img){cout<<"warning : image loading error!\n";invalid=1;}
		if(!fimg){cout<<"warning : image allocation error!\n";invalid=1;}
	}

	void Image::free_mem()
	{
		if(fimg) free_image(fimg);
		if(simg) free_image(simg);
		if(fname) free(fname);
		reset();
	}

	void Image::reduce(float factor)
	{
		newscale = scale / factor;
		rescale();
	}

	void Image::magnify(float factor)
	{
		newscale = scale * factor;
		rescale();
	}

	void Image::pan_up()
	{
	    top -= steps;
	    redraw=1;
	}

	void Image::pan_down()
	{
	    top += steps;
	    redraw=1;
	}

	void Image::pan_right()
	{
	    if (pos < len) pos+=steps;
	    left+=steps;
	    redraw=1;
	}

	void Image::pan_left()
	{
	    if (pos > 0) pos-=steps;
	    left-=steps;
	    redraw=1;
	}

	int Image::onBottom()
	{
#ifndef FIM_NOFB
		/**
		 **	PERCHE NON FUNZIONA?
		 **/
		if(img && invalid==0)
//		return ( top+img->i.height<fb_var.yres);
		return (top + fb_var.yres >= img->i.height);
		else return 0;
#else
		return 0;
#endif
	}
	char* Image::getInfo()
	{
		if(fimg)return make_info(fimg,scale);return NULL;
	}
};
