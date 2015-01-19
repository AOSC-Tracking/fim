/* $LastChangedDate$ */
/*
 Image.cpp : Image manipulation and display

 (c) 2007-2014 Michele Martone

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

//#include "Image.h"
#include "fim.h"

#if FIM_WANT_EXIFTOOL
#include "ExifTool.h"
#endif /* FIM_WANT_EXIFTOOL */

#define FIM_WANT_BACKGROUND_LOAD 0

#if FIM_WANT_BACKGROUND_LOAD
#include <thread>
#endif /* FIM_WANT_BACKGROUND_LOAD */

#if FIM_WANT_PIC_CMTS
	std::ostream& operator<<(std::ostream &os, const ImgDscs & id)
	{
		return id.print(os);
	}
#endif /* FIM_WANT_PIC_CMTS */

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
	fim_coo_t Image::original_width(void)const
	{
		fim_coo_t ow;
		assert(fimg_);
		if(orientation_%2)
		       	ow = fimg_->i.height;
		else
			ow = fimg_->i.width;
		return ow;
	}

	fim_coo_t Image::original_height(void)const
	{
		fim_coo_t ow;
		assert(fimg_);
		if(orientation_%2)
		       	ow = fimg_->i.width;
		else
			ow = fimg_->i.height;
		return ow;
	}

	int Image::width(void)const
	{
		assert(img_);
		return img_->i.width;
	}

	int Image::height(void)const
	{
		assert(img_);
		return img_->i.height;
	}

	Image::Image(const fim_char_t *fname, FILE*fd):
		scale_(0.0),
		ascale_(0.0),
		newscale_(0.0),
		angle_(0.0),
		newangle_(0.0),
		page_(0),
                img_     (NULL),
                fimg_    (NULL),
		orientation_(FIM_NO_ROT),
                invalid_(false),
		no_file_(true),
		fs_(0), ms_(0),
#ifdef FIM_NAMESPACES
		Namespace(&cc,FIM_SYM_NAMESPACE_IMAGE_CHAR),
#endif /* FIM_NAMESPACES */
		fis_(fim::string(fname)==fim::string(FIM_STDIN_IMAGE_NAME)?FIM_E_STDIN:FIM_E_FILE),
                fname_     (FIM_CNS_DEFAULT_IFNAME)

	{
		/*
		 *	an image object is created from an image filename
		 */
		reset();	// pointers blank
		if( !load(fname,fd,getGlobalIntVariable(FIM_VID_PAGE)) || check_invalid() || (!fimg_) ) 
		{
			// FIXME: sometimes load() intentionally skips a file. an appropriate message shall be printed out
			cout << "warning : invalid loading "<<fname<<" ! \n";
			if( getGlobalIntVariable(FIM_VID_DISPLAY_STATUS_BAR)||getGlobalIntVariable(FIM_VID_DISPLAY_BUSY))
				cc.set_status_bar( fim::string("error while loading \"")+ fim::string(fname)+ fim::string("\"") , "*");
			invalid_ = true;
			throw FimException();
		}
		else
		{

#if FIM_WANT_PIC_CMTS
			/* Picture commentary. user-set overrides the file's own. */
			struct ida_extra* ie=load_find_extra(&(img_->i),EXTRA_COMMENT);

			if(ie)
				setVariable(FIM_VID_COMMENT,(fim_char_t*)(ie->data));

			if(fname)
			{
				fim_fn_t key(fim_basename_of(fname));
				if(cc.id_.find(key) != cc.id_.end() )
					setVariable(FIM_VID_COMMENT,(cc.id_[key]).c_str());
			}
#endif /* FIM_WANT_PIC_CMTS */

#if FIM_WANT_EXIFTOOL
if(fname && getGlobalIntVariable(FIM_VID_EXIFTOOL) != 0)
{
	fim_int ue = getGlobalIntVariable(FIM_VID_EXIFTOOL);
	/* FIXME: one shall execute this code in a separate thread */
	/* std::cout << "will try exiftool on : " << fname << "\n"; */
	fim::string etc;
	ExifTool *et = new ExifTool();
    	TagInfo *info = et->ImageInfo(fname,NULL,2);

	if (info)
       	{
        	for (TagInfo *i=info; i; i=i->next)
	       	{
			etc+=i->name;
			etc+=" = ";
			etc+=i->value;
			etc+=";";
			etc+="\n";
			//std::cout << "reading " << i->name << "...\n";
        	}
        	delete info;
    	}
       	else if (et->LastComplete() <= 0)
       	{
		std::cerr << "Error executing exiftool!" << std::endl;
    	}
    	char *err = et->GetError();
    	if (err) std::cerr << err;
    	delete et;      // delete our ExifTool object
	//std::cout << "setting: " << etc << "\n",
	if(ue == 1)
		setVariable(FIM_VID_COMMENT,getVariable(FIM_VID_COMMENT)+(etc.c_str()));
	if(ue == 2)
		setVariable(FIM_VID_EXIFTOOL_COMMENT,etc.c_str());
}
#endif /* FIM_WANT_EXIFTOOL */
		}
	}

	void Image::reset(void)
	{
		/*
		 * pointers are blanked and values set to default 
		 * */
                scale_   = 1.0;
                newscale_= 1.0;
                ascale_  = 1.0;
                angle_   = 0.0;
		setVariable(FIM_VID_SCALE  ,scale_*100);
		setVariable(FIM_VID_ASCALE ,ascale_);
		setVariable(FIM_VID_ANGLE  ,angle_);
		no_file_ =true;	//reloading allowed
                invalid_ =false;
                fimg_    = NULL;
                img_     = NULL;
                orientation_=FIM_NO_ROT;
		setVariable(FIM_VID_ORIENTATION ,FIM_NO_ROT);
	}
	
	bool Image::reload(void)
	{
		/*
			reloads the file (no hope for streams, therefore)
			FIXME : still unused
		*/
		bool b=false;
		FILE *fd=fim_fopen(fname_.c_str(),"r");
		if(!fd)
			return b;
		b=load(fname_.c_str(),fd,page_);
		fclose(fd);// FIXME : the fd could already be closed !
		return b;
	}
	
#if FIM_WANT_BACKGROUND_LOAD
void fim_background_load()
{
	std::cout << "background loading\n";
}
#endif /* FIM_WANT_BACKGROUND_LOAD */

	bool Image::load(const fim_char_t *fname, FILE* fd, int want_page)
	{
		/*
		 *	an image is loaded and initializes this image.
		 *	returns false if the image does not load
		 */
		if(fname==NULL && fname_==FIM_CNS_EMPTY_STRING)
			return false;//no loading = no state change
		this->free();
		fname_=fname;
		if( getGlobalIntVariable(FIM_VID_DISPLAY_STATUS_BAR)||getGlobalIntVariable(FIM_VID_DISPLAY_BUSY))
		{
			if( getGlobalIntVariable(FIM_VID_WANT_PREFETCH) == 1)
				cc.set_status_bar("please wait while prefetching...", "*");
			else
				cc.set_status_bar("please wait while reloading...", "*");
		}

#if FIM_WANT_BACKGROUND_LOAD
		/* this would be a hypothetical starting point for a background running loader */
		std::thread t(&fim_background_load);
		std::cout << "foreground running\n";
		t.join(); 
		std::cout << "loaded!\n";
#endif /* FIM_WANT_BACKGROUND_LOAD */
		fimg_ = FbiStuff::read_image(fname,fd,want_page,this);
#if 0
		if(fimg_)
		{
			// fim_free(fimg_->data);
			/* Such dimensions break SDL */
    			fimg_->i.width = 100*1000*1000;
	       		fimg_->i.height = 1;
    			fimg_->data = fim_pm_alloc(fimg_->i.width, fimg_->i.height);
			for(int i=0;i<fimg_->i.width*fimg_->i.height;++i)
    				fimg_->data[i*3+0]=i%256,
    				fimg_->data[i*3+1]=i%256,
    				fimg_->data[i*3+2]=i%256;
		}
#endif

#if FIM_WANT_EXPERIMENTAL_MIPMAPS
    		if(fimg_)
		if(getGlobalIntVariable(FIM_VID_WANT_MIPMAPS)>0)
			FbiStuff::fim_mipmaps_compute(fimg_,&mm_);
#endif /* FIM_WANT_EXPERIMENTAL_MIPMAPS */

    		if(strcmp(FIM_STDIN_IMAGE_NAME,fname)==0)
		{
			no_file_=true;	//no file is associated to this image (to prevent reloading)
			fis_ = FIM_E_STDIN; // yes, it seems redundant but it is necessary
		}
		else 
		{
#if FIM_WANT_KEEP_FILESIZE
			struct stat stat_s;
			if(-1!=stat(fname,&stat_s))
			{
				fs_=stat_s.st_size;
			}
#endif /* FIM_WANT_KEEP_FILESIZE */
			no_file_=false;	//reloading allowed
		}

		img_=fimg_;	/* no scaling : one copy only */
		should_redraw();

		if(! img_)
		{
			cout<<"warning : image loading error!\n"   ;
			invalid_=true;
			return false;
		}
		else
		       	page_=want_page;
		//cout<<"loaded page "<< want_page<<" to "<<((int*)this)<<"\n";

#ifdef FIM_NAMESPACES
		setVariable(FIM_VID_HEIGHT ,(fim_int)fimg_->i.height);
		setVariable(FIM_VID_WIDTH ,(fim_int)fimg_->i.width );
		setVariable(FIM_VID_SHEIGHT,(fim_int) img_->i.height);
		setVariable(FIM_VID_SWIDTH,(fim_int) img_->i.width );
		if(cc.displaydevice_)
			setVariable(FIM_VID_FIM_BPP ,(fim_int) cc.displaydevice_->get_bpp());
		setVariable(FIM_VID_SCALE  ,newscale_*100);
		setVariable(FIM_VID_ASCALE,ascale_);
		setVariable(FIM_VID_ANGLE , angle_);
		setVariable(FIM_VID_NEGATED , 0);
		setVariable(FIM_VID_DESATURATED, 0);
		setVariable(FIM_VID_FILENAME,fname_.c_str());
#endif /* FIM_NAMESPACES */

		setGlobalVariable(FIM_VID_HEIGHT ,(int)fimg_->i.height);
		setGlobalVariable(FIM_VID_WIDTH  ,(int)fimg_->i.width );
		setGlobalVariable(FIM_VID_SHEIGHT,(int) img_->i.height);
		setGlobalVariable(FIM_VID_SWIDTH ,(int) img_->i.width );
		if(cc.displaydevice_)
			setGlobalVariable(FIM_VID_FIM_BPP ,(int) cc.displaydevice_->get_bpp());
		//setGlobalVariable(FIM_VID_SCALE  ,newscale_*100);
		//setGlobalVariable(FIM_VID_ASCALE ,ascale_);
	
		if( getGlobalIntVariable(FIM_VID_DISPLAY_STATUS_BAR)||getGlobalIntVariable(FIM_VID_DISPLAY_BUSY))
			cc.browser_.display_status(cc.browser_.current().c_str(), NULL); /* FIXME: an ugly way to force the proper status display */
		return true;
	}

	Image::~Image(void)
	{
		/*
		 * buffers are freed
		 * */
#ifdef FIM_CACHE_DEBUG
		std::cout << "freeing Image " << this << "\n";
#endif /* FIM_CACHE_DEBUG */
		this->free();
	}

        bool Image::tiny(void)const
	{
		/*
		 * image width or height is <= 1
		 * */
		if(!img_)
			return true;
	       	return ( img_->i.width<=1 || img_->i.height<=1 )?true:false;
	}

	fim_err_t Image::scale_multiply(fim_scale_t sm)
	{
		/*
		 * current scale_ is multiplied by a factor
		 * */
		if(scale_*sm>0.0)
			newscale_=scale_*sm;rescale();
		return FIM_ERR_NO_ERROR;
	}

	fim_err_t Image::scale_increment(fim_scale_t ds)
	{
		/*
		 * current scale_ is multiplied by a factor
		 * */
		if(scale_+ds>0.0)
			newscale_=scale_+ds;rescale();
		return FIM_ERR_NO_ERROR;
	}

	fim_err_t Image::setscale(fim_scale_t ns)
	{
		/*
		 * a new scale_ is set
		 * */
		newscale_=ns;rescale();
		return FIM_ERR_NO_ERROR;
	}

        bool Image::check_valid(void)
	{
		/*
		 * well,why not ?
		 * */
		return ! check_invalid();
	}

        bool Image::check_invalid(void)
        {
                /*
		 * the image is declared invalid if the image structures are not loaded.
                 */

		//ACHTUNG! 
		if(!img_)
			img_ = fimg_;
                if(!img_)
                        invalid_ = true;
		else
			invalid_ = false;
                return invalid_;
        }

        void Image::free(void)
        {
		/*
		 * the image descriptors are freed if necessary and pointers blanked
		 * */
                if(fimg_!=img_ && img_ )
		       	FbiStuff::free_image(img_ );
                if(fimg_     )
		       	FbiStuff::free_image(fimg_);
                reset();
        }

// if the image rescaling mechanism is suspected of bugs, this will inhibit its use.
#define FIM_BUGGED_RESCALE 0

	fim_err_t Image::rescale( fim_scale_t ns )
	{
		/*
		 * effective image rescaling
		 * TODO: should rather be called "apply"
		 * */
		fim_pgor_t neworientation;
		fim_angle_t	gascale;
		fim_scale_t	newascale;
		fim_angle_t	gangle;

#if FIM_BUGGED_RESCALE
		goto ret;
#endif /* FIM_BUGGED_RESCALE */
		if(ns>0.0)
			newscale_=ns;//patch

		if( check_invalid() )
			goto err;
		if(tiny() && newscale_<scale_)
		{
			newscale_=scale_;
			goto ret;
		}

		neworientation=getOrientation();
		gascale=getGlobalFloatVariable(FIM_VID_ASCALE);
		newascale=getFloatVariable(FIM_VID_ASCALE);
		newascale=(newascale>0.0 && newascale!=1.0)?newascale:((gascale>0.0 && gascale!=1.0)?gascale:1.0);
		
		//float newascale=getFloatVariable(FIM_VID_ASCALE); if(newascale<=0.0) newascale=1.0;
		/*
		 * The global angle_ variable value will override the local if not 0 and the local unset
		 * */
		gangle  =getGlobalFloatVariable(FIM_VID_ANGLE),
			newangle_=getFloatVariable(FIM_VID_ANGLE);
		newangle_=angle_?newangle_:((gangle!=0.0)?gangle:newangle_);

		if(	newscale_ == scale_
			&& newascale == ascale_
			&& neworientation == orientation_
			//&& newangle_ == angle_
			&& ( !newangle_  && !angle_ )
		)
		{
			goto ret;/*no need to rescale*/
		}
		orientation_=((neworientation%FIM_ROT_ROUND)+FIM_ROT_ROUND)%FIM_ROT_ROUND; // fix this

		setGlobalVariable(FIM_VID_SCALE,newscale_*100);
		if(fimg_)
		{
			/*
			 * In case of memory allocation failure, we would
			 * like to recover the current image  :) . 
			 *
			 * Here it would be nice to add some sort of memory manager 
			 * keeping score of copies and ... too complicated ...
			 */
			struct ida_image *backup_img=img_;

			if(getGlobalIntVariable(FIM_VID_DISPLAY_STATUS_BAR)||getGlobalIntVariable(FIM_VID_DISPLAY_BUSY))
				cc.set_status_bar("please wait while rescaling...", "*");

#define FIM_PROGRESSIVE_RESCALING 0
#if FIM_PROGRESSIVE_RESCALING
			/*
			 * progressive rescaling is computationally convenient in when newscale_<scale_
			 * at the cost of a progressively worsening image quality (especially when newscale_~scale_)
			 * and a sequence ----+ will suddenly 'clear' out the image quality, so it is not a desirable
			 * option ...
			 * */
			if( 
				//( newscale_>scale_ && scale_ > 1.0) ||
				( newscale_<scale_ && scale_ < 1.0) )
				img_ = scale_image( img_,newscale_/scale_,newascale);
			else
				img_ = scale_image(fimg_,newscale_,newascale);
#else
			img_ = FbiStuff::scale_image(fimg_,newscale_,newascale
#if FIM_WANT_EXPERIMENTAL_MIPMAPS
					,(getGlobalIntVariable(FIM_VID_WANT_MIPMAPS)>1)?(&mm_):NULL
#endif /* FIM_WANT_EXPERIMENTAL_MIPMAPS */
					);
#endif /* FIM_PROGRESSIVE_RESCALING */
			/* orientation_ can be 0,1,2,3 */
			if( img_ && orientation_!=FIM_ROT_0Q && orientation_ != FIM_ROT_2Q)
			{
				// we make a backup.. who knows!
				// FIXME: should use a faster and memory-smarter method : in-place
				struct ida_image *rb=img_;
				rb  = FbiStuff::rotate_image90(rb,orientation_==FIM_ROT_1Q?FIM_ROT_0Q:FIM_ROT_1Q);
				if(rb)
				{
					FbiStuff::free_image(img_);
					img_=rb;
				}
			}
			if( img_ && orientation_!=FIM_ROT_0Q && orientation_ == FIM_ROT_2Q)
			{	
				// we make a backup.. who knows!
				struct ida_image *rbb=NULL,*rb=NULL;
				// FIXME: should use a faster and memory-smarter method : in-place
				rb  = FbiStuff::rotate_image90(img_,0);
				if(rb)
					rbb  = FbiStuff::rotate_image90(rb,0);
				if(rbb)
				{
					FbiStuff::free_image(img_);
					FbiStuff::free_image(rb);
					img_=rbb;
				}
				else
				{
					if(rbb)
						FbiStuff::free_image(rbb);
					if(rb )
						FbiStuff::free_image(rb);
				}
			}

			/* we rotate only in case there is the need to do so */
			if( img_ && ( angle_ != newangle_ || newangle_) )
			{	
				// we make a backup.. who knows!
				struct ida_image *rbb=NULL,*rb=NULL;
				rb  = FbiStuff::rotate_image(img_,newangle_);
				if(rb)
					rbb  = FbiStuff::rotate_image(rb,0);
				if(rbb)
				{
					FbiStuff::free_image(img_);
					FbiStuff::free_image(rb);
					img_=rbb;
				}
				else
				{
					if(rbb)
						FbiStuff::free_image(rbb);
					if(rb )
						FbiStuff::free_image(rb);
				}
			}

			if(!img_)
			{
				img_=backup_img;
				if(getGlobalIntVariable(FIM_VID_DISPLAY_BUSY))
					cc.set_status_bar( "rescaling failed (insufficient memory?!)", getInfo().c_str());
				sleep(1);	//just to give a glimpse..
			}
			else 
			{
				/* reallocation succeeded */
				if( backup_img && backup_img!=fimg_ )
				       	FbiStuff::free_image(backup_img);
				scale_=newscale_;
				ascale_=newascale;
				angle_ =newangle_;
	        		should_redraw();
			}

			/*
			 * it is important to set these values after rotation, too!
			 * */
			setVariable(FIM_VID_HEIGHT ,(fim_int)fimg_->i.height);
			setVariable(FIM_VID_WIDTH  ,(fim_int)fimg_->i.width );
			setVariable(FIM_VID_SHEIGHT,(fim_int) img_->i.height);
			setVariable(FIM_VID_SWIDTH ,(fim_int) img_->i.width );
			setVariable(FIM_VID_ASCALE , ascale_ );
			//setGlobalVariable(FIM_VID_ANGLE  ,  angle_ );
		}
		else
		       	should_redraw(0);
		orientation_=neworientation;
ret:
		return FIM_ERR_NO_ERROR;
err:
		return FIM_ERR_GENERIC;
	}

	void Image::reduce(fim_scale_t factor)
	{
		/*
		 * scale_ is adjusted by a dividing factor
		 * */
		newscale_ = scale_ / factor;
		rescale();
	}

	void Image::magnify(fim_scale_t factor)
	{
		/*
		 * scale_ is adjusted by a multiplying factor
		 * */
		newscale_ = scale_ * factor;
		rescale();
	}

	/*
	void Image::resize(int nw, int nh)
	{
		//fixme
		if(check_invalid())
			return;
	}*/

	Image::Image(const Image& image):
		scale_(image.scale_),
		ascale_(image.ascale_),
		newscale_(image.newscale_),
		angle_(image.angle_),
		newangle_(image.newangle_),
		page_(page_),//FIXME
                img_     (NULL),
                fimg_    (NULL),
		orientation_(image.orientation_),
                //invalid_(0),
                invalid_(image.invalid_),
		no_file_(true),
		fis_(image.fis_),
		fs_(0), ms_(0),
                fname_     (image.fname_)
	{
		/*
		 * builds a clone of this image.
		 * it should be completely independent from this object.
		 * */
		reset();
		img_  = fbi_image_clone(image.img_ );
		fimg_ = fbi_image_clone(image.fimg_);

		/* an exception is launched immediately */
		if(!img_ || !fimg_)
#if 0
			///* temporarily, for security reasons :  throw FIM_E_NO_IMAGE*/;
		{
			std::cerr << "fatal error : " << __FILE__ << ":" << __LINE__ << " ( are you sure you gave an image file in standard input, uh ?)\n";
			throw FimException();
			std::exit(*(int*)NULL);// FIXME
		}
#else
			invalid_=true;
#endif
	}

	Image * Image::getClone(void)
	{
		/*
		 * returns a clone of this image.
		 * it should be an object completely independent from this.
		 * */
		return new Image(*this);
	}

fim::string Image::getInfoCustom(const fim_char_t * ifsp)const
{
	static fim_char_t linebuffer[FIM_STATUSLINE_BUF_SIZE];
	fim_char_t pagesinfobuffer[FIM_STATUSLINE_BUF_SIZE];
	fim_char_t imagemode[3],*imp;
	int n=getGlobalIntVariable(FIM_VID_FILEINDEX);
#if FIM_WANT_CUSTOM_INFO_STATUS_BAR
	fim::string ifs;
#endif /* FIM_WANT_CUSTOM_INFO_STATUS_BAR */
	imp=imagemode;

	//if(getGlobalIntVariable(FIM_VID_AUTOFLIP))*(imp++)='F';
	//if(getGlobalIntVariable(FIM_VID_AUTOMIRROR))*(imp++)='M';

	// should flip ? should mirror ?
	int flip   =
	(((getGlobalIntVariable(FIM_VID_AUTOFLIP)== 1)|(getGlobalIntVariable("v:" FIM_VID_FLIPPED)== 1)|(getIntVariable(FIM_VID_FLIPPED)== 1))&&
	!((getGlobalIntVariable(FIM_VID_AUTOFLIP)==-1)|(getGlobalIntVariable("v:" FIM_VID_FLIPPED)==-1)|(getIntVariable(FIM_VID_FLIPPED)==-1)));
	int mirror   =
	(((getGlobalIntVariable(FIM_VID_AUTOMIRROR)== 1)|(getGlobalIntVariable("v:" FIM_VID_MIRRORED)== 1)|(getIntVariable(FIM_VID_MIRRORED)== 1))&&
	!((getGlobalIntVariable(FIM_VID_AUTOMIRROR)==-1)|(getGlobalIntVariable("v:" FIM_VID_MIRRORED)==-1)|(getIntVariable(FIM_VID_MIRRORED)==-1)));

	if(flip  )*(imp++)=FIM_SYM_FLIPCHAR;
	if(mirror)*(imp++)=FIM_SYM_MIRRCHAR;
	*imp='\0';

	if(fimg_ && fimg_->i.npages>1)
		snprintf(pagesinfobuffer,sizeof(pagesinfobuffer)," [%d/%d]",page_+1,fimg_->i.npages);
	else
		*pagesinfobuffer='\0';
		
/* #if FIM_WANT_DISPLAY_MEMSIZE */
	// ms_ = byte_size();
	size_t ms = fimg_ ? ( fimg_->i.height*fimg_->i.width*3 ) : 0;
/* #endif */ /* FIM_WANT_DISPLAY_MEMSIZE */


#if FIM_WANT_CUSTOM_INFO_STATUS_BAR
	//if((ifs=getGlobalStringVariable(FIM_VID_INFO_FMT_STR))!="" && ifs.c_str() != NULL)
	{
		static fim_char_t clb[FIM_STATUSLINE_BUF_SIZE];
		//char*ifsp=(char*)ifs.c_str(); // FIXME
		const char*fp=ifsp;
		const char*sp=ifsp;

		clb[0]=FIM_SYM_CHAR_NUL;

		while(*sp && *sp!='%')
		{
			++sp;
		}
		goto sbum;
		while(*sp=='%' && isprint(sp[1]))
		{
			++sp;
			switch(*sp)
			{
				// "%p %wx%h %i/%l %F %M"
				case('p'):
					snprintf(clb+strlen(clb), sizeof(clb), "%.0f",scale_*100);
				break;
				case('w'):
					snprintf(clb+strlen(clb), sizeof(clb), "%d",this->width());
				break;
				case('h'):
					snprintf(clb+strlen(clb), sizeof(clb), "%d",this->height());
				break;
				case('i'):
					/* browser property. TODO: move outta here */
					snprintf(clb+strlen(clb), sizeof(clb), "%d",n?n:1);
				break;
#if 1
				case('k'):
				{
					const char * cmnts = getStringVariable(FIM_VID_COMMENT).c_str();
					if(cmnts && *cmnts)
						snprintf(clb+strlen(clb), sizeof(clb), "[%s] ",cmnts); /* FIXME: need sanitization */
				}/* FIXME: this shamelessly breaks all max lenght assumptions ! */
#endif
				break;
				case('l'):
					/* browser property. TODO: move outta here */
					snprintf(clb+strlen(clb), sizeof(clb), "%d",(getGlobalIntVariable(FIM_VID_FILELISTLEN)));
				break;
				case('L'):
					snprintf(clb+strlen(clb), sizeof(clb), "%s",imagemode);
				break;
				case('P'):
					snprintf(clb+strlen(clb), sizeof(clb), "%s",pagesinfobuffer);
				break;
				case('F'):
					fim_snprintf_XB(clb+strlen(clb), sizeof(clb),fs_);
				break;
				case('M'):
					fim_snprintf_XB(clb+strlen(clb), sizeof(clb),ms);
				break;
				case('n'):
					snprintf(clb+strlen(clb), sizeof(clb), "%s",getStringVariable(FIM_VID_FILENAME).c_str());
				break;
				case('N'):
					snprintf(clb+strlen(clb), sizeof(clb), "%s",fim_basename_of(getStringVariable(FIM_VID_FILENAME).c_str()));
				break;
				case('T'):
					/* console property. TODO: move outta here */
					fim_snprintf_XB(clb+strlen(clb), sizeof(clb),cc.byte_size());
				break;
				case('m'):
					fim_snprintf_XB(clb+strlen(clb), sizeof(clb),mm_.byte_size());
				break;
				case('C'):
					/* cache property. TODO: move outta here */
					fim_snprintf_XB(clb+strlen(clb), sizeof(clb),cc.browser_.cache_.byte_size());
				break;
				case('c'):
					/* viewport property. TODO: move outta here */
					cc.current_viewport()->snprintf_centering_info(clb+strlen(clb), sizeof(clb));
				break;
				case('v'):
					snprintf(clb+strlen(clb), sizeof(clb), "%s",FIM_CNS_FIM_APPTITLE);
				break;
				case('%'):
					snprintf(clb+strlen(clb), sizeof(clb), "%c",'%');
				break;
#if FIM_EXPERIMEMTAL_VAR_EXPANDOS 
				case('?'): /* "%?forward_comment?_filename?back_comment?" */
				if(strlen(sp+1)>=3)
				{
					char *fcp = NULL, *vip = NULL, *bcp = NULL;
					if( 3 == sscanf(sp,"?%a[^?%]?%a[A-Z_a-z]?%a[^?%]?",&fcp,&vip,&bcp) )
					if(fcp && bcp && vip)
					{
						if(*vip && isSetVar(vip))
							snprintf(clb+strlen(clb), sizeof(clb), "%s%s%s",fcp,getStringVariable(vip).c_str(),bcp);
						sp += strlen(fcp)+strlen(vip)+strlen(bcp)+3;
					}
					if(fcp)std::free(fcp);
					if(bcp)std::free(bcp);
					if(vip)std::free(vip);
				}
				break;
#endif /* FIM_EXPERIMEMTAL_VAR_EXPANDOS */
				// default:
				/* rejecting char; may display an error message here */
			}
			++sp;
			fp=sp;
sbum:
			while(*sp!='%' && sp[0])
				++sp;
			snprintf(clb+strlen(clb), FIM_MIN(sp-fp+1,sizeof(clb)), "%s",fp);
		}
		//std::cout << "Custom format string chosen: "<< ifsp << ", resulting in: "<< clb <<"\n";
		snprintf(linebuffer, sizeof(linebuffer),"%s",clb);
		goto labeldone;
	}
#endif /* FIM_WANT_CUSTOM_INFO_STATUS_BAR */
labeldone:
	return fim::string(linebuffer);
}

/*
 *	Creates a little description of some image,
 *	and places it in a NUL terminated static buffer.
 */
fim::string Image::getInfo(void)
{
	/*
	 * a short information about the current image is returned
	 *
	 * WARNING:
	 * the returned info, if not NULL, belongs to a statical buffer which LIVES with the image!
	 */
	//FIX ME !
	if(!fimg_)
		return FIM_CNS_EMPTY_RESULT;

	static fim_char_t linebuffer[FIM_STATUSLINE_BUF_SIZE];
#if FIM_WANT_CUSTOM_INFO_STATUS_BAR
	fim::string ifs;

	if((ifs=getGlobalStringVariable(FIM_VID_INFO_FMT_STR))!="" && ifs.c_str() != NULL)
	{
		fim::string clb = getInfoCustom(ifs.c_str());
		snprintf(linebuffer, sizeof(linebuffer),"%s",clb.c_str());
		goto labeldone;
	}
	else
#endif /* FIM_WANT_CUSTOM_INFO_STATUS_BAR */
{
	/* FIXME: for cleanup, shall eliminate this branch and introduce a default string. */
	fim_char_t pagesinfobuffer[FIM_STATUSLINE_BUF_SIZE];
	fim_char_t imagemode[3],*imp;
	int n=getGlobalIntVariable(FIM_VID_FILEINDEX);
	imp=imagemode;

	//if(getGlobalIntVariable(FIM_VID_AUTOFLIP))*(imp++)='F';
	//if(getGlobalIntVariable(FIM_VID_AUTOMIRROR))*(imp++)='M';

	// should flip ? should mirror ?
	int flip   =
	(((getGlobalIntVariable(FIM_VID_AUTOFLIP)== 1)|(getGlobalIntVariable("v:" FIM_VID_FLIPPED)== 1)|(getIntVariable(FIM_VID_FLIPPED)== 1))&&
	!((getGlobalIntVariable(FIM_VID_AUTOFLIP)==-1)|(getGlobalIntVariable("v:" FIM_VID_FLIPPED)==-1)|(getIntVariable(FIM_VID_FLIPPED)==-1)));
	int mirror   =
	(((getGlobalIntVariable(FIM_VID_AUTOMIRROR)== 1)|(getGlobalIntVariable("v:" FIM_VID_MIRRORED)== 1)|(getIntVariable(FIM_VID_MIRRORED)== 1))&&
	!((getGlobalIntVariable(FIM_VID_AUTOMIRROR)==-1)|(getGlobalIntVariable("v:" FIM_VID_MIRRORED)==-1)|(getIntVariable(FIM_VID_MIRRORED)==-1)));

	if(flip  )*(imp++)=FIM_SYM_FLIPCHAR;
	if(mirror)*(imp++)=FIM_SYM_MIRRCHAR;
	*imp='\0';

	if(fimg_->i.npages>1)
		snprintf(pagesinfobuffer,sizeof(pagesinfobuffer)," [%d/%d]",page_+1,fimg_->i.npages);
	else
		*pagesinfobuffer='\0';
		
/* #if FIM_WANT_DISPLAY_MEMSIZE */
	// ms_ = byte_size();
	ms_ = fimg_ ? ( fimg_->i.height*fimg_->i.width*3 ) : 0;
/* #endif */ /* FIM_WANT_DISPLAY_MEMSIZE */


	snprintf(linebuffer, sizeof(linebuffer),
	     "[ %s%.0f%% %dx%d%s%s %d/%d ]"
#if FIM_WANT_DISPLAY_FILESIZE
	     " %dkB"
#endif /* FIM_WANT_DISPLAY_FILESIZE */
#if FIM_WANT_DISPLAY_MEMSIZE
	     " %dMB"
#endif /* FIM_WANT_DISPLAY_MEMSIZE */
	     ,
	     /*fcurrent->tag*/ 0 ? "* " : "",
	     scale_*100,
	     this->width(), this->height(),
	     imagemode,
	     pagesinfobuffer,
	     n?n:1, /* ... */
	     (getGlobalIntVariable(FIM_VID_FILELISTLEN))
#if FIM_WANT_DISPLAY_FILESIZE
	     ,fs_/FIM_CNS_K
#endif /* FIM_WANT_DISPLAY_FILESIZE */
#if FIM_WANT_DISPLAY_MEMSIZE
	     ,ms_/FIM_CNS_M
#endif /* FIM_WANT_DISPLAY_MEMSIZE */
	     );
}
labeldone:
	return fim::string(linebuffer);
}

	bool Image::update(void)
	{
		/*
		 * updates the image according to its variables
		 *
		 * FIXME: a temporary method
		 * */
		setVariable(FIM_VID_FRESH,(fim_int)0);
		if(fimg_)
			setVariable(FIM_VID_PAGECOUNT,(fim_int)fimg_->i.npages);

		/*
		 * rotation dispatch
		 * */
                fim_pgor_t neworientation=getOrientation();
		if( neworientation!=orientation_)
		{
			rescale();
			orientation_=neworientation;
			return true;
		}
		return false;
	}

	fim_pgor_t Image::getOrientation(void)const
	{
		/*
		 * warning : this should work more intuitively
		 * */
		return (FIM_MOD(
		(  getIntVariable(FIM_VID_ORIENTATION)
		+getGlobalIntVariable("v:" FIM_VID_ORIENTATION)
		+getGlobalIntVariable(FIM_VID_ORIENTATION)
		) ,4));
	}

	fim_err_t Image::rotate( fim_scale_t angle_ )
	{
		/*
		 * rotates the image the specified amount of degrees
		 * */
		float newangle_=this->angle_+angle_;
		if( check_invalid() )
		       	return FIM_ERR_GENERIC;
		setVariable(FIM_VID_ANGLE,newangle_);
		return rescale();	// FIXME : necessary *only* for image update and display
	}

	void Image::should_redraw(int should)const
	{
		/* FIXME : this is BAD style ! */
	        if(cc.displaydevice_)
		        cc.displaydevice_->redraw_=FIM_REDRAW_NECESSARY;
	}

	bool Image::prev_page(int j)
	{
		string s=fname_;
		if(have_prevpage(j))
			return load(s.c_str(),NULL,page_-j);
		else
			return false;
	} 

	bool Image::goto_page(fim_page_t j)
	{
		string s=fname_;
	//	if( j>0 )--j;
		if( !fimg_ )
			return false;
		if( j<0 )
			j=fimg_->i.npages-1;
		if( j>page_ ? have_nextpage(j-page_) : have_prevpage(page_-j) )
		{
			//if(0)cout<<"about to goto page "<<j<<"\n";
			setGlobalVariable(FIM_VID_PAGE ,(fim_int)j);
			return load(s.c_str(),NULL,j);
			//return true;
		}
		else
			return false;
	} 

	bool Image::next_page(int j)
	{
		string s=fname_;
		if(have_nextpage(j))
			return load(s.c_str(),NULL,page_+j);
		else
			return false;
	} 

	cache_key_t Image::getKey(void)const
	{
		return cache_key_t(fname_.c_str(),fis_);
	}

	bool Image::is_multipage(void)const
	{
		if( fimg_ && ( fimg_->i.npages>1 ) )
			return true;
		return false;
	}

	bool Image::have_nextpage(int j)const
	{
		/* FIXME : missing overflow check */
		return (is_multipage() && page_+j < fimg_->i.npages);
	} 

	bool Image::have_prevpage(int j)const
	{
		/* FIXME : missing overflow check */
		return (is_multipage() && page_-j >= 0);
	}
 
	bool Image::gray_negate(void)
	{
		/* FIXME : NEW, but unused */
		int n;
		int th=1;/* 0 ... 256 * 3 * 3 */

		if(!img_ || !img_->data)
			return false;

		if(!fimg_ || !fimg_->data)
			return false;
	
		for( n=0; n< 3*fimg_->i.width*fimg_->i.height ; n+=3 )
		{
			int r,g,b,s,d;
			r=fimg_->data[n+0];
			g=fimg_->data[n+1];
			b=fimg_->data[n+2];
			s=r+g+b;
			d=( s - 3 * r ) * ( s - 3 * g ) * ( s - 3 * b );
			d=d<0?-d:d;
			if( d < th )
			{
				fimg_->data[n+0]=~fimg_->data[n+0];
				fimg_->data[n+1]=~fimg_->data[n+1];
				fimg_->data[n+2]=~fimg_->data[n+2];
			}
		}

		for( n=0; n< 3*img_->i.width*img_->i.height ; n+=3 )
		{
			int r,g,b,s,d;
			r=img_->data[n+0];
			g=img_->data[n+1];
			b=img_->data[n+2];
			s=r+g+b;
			d=( s - 3 * r ) * ( s - 3 * g ) * ( s - 3 * b );
			d=d<0?-d:d;
			if( d < th )
			{
				img_->data[n+0]=~img_->data[n+0];
				img_->data[n+1]=~img_->data[n+1];
				img_->data[n+2]=~img_->data[n+2];
			}
		}

		setGlobalVariable("i:" FIM_VID_NEGATED,1-getGlobalIntVariable("i:" FIM_VID_NEGATED ));

       		should_redraw();

		return true;
	} 

	bool Image::desaturate(void)
	{
		register int avg;
#if 0
		if(! img_ || ! img_->data)
			return false;
		if(!fimg_ || !fimg_->data)
			return false;
#endif

		if( fimg_ &&  fimg_->data)
		for( fim_byte_t * p = fimg_->data; p < fimg_->data + 3*fimg_->i.width*fimg_->i.height ;p+=3)
		{ avg=p[0]+p[1]+p[2]; p[0]=p[1]=p[2]=(fim_byte_t) (avg/3); }

		if(  img_ &&   img_->data)
		for( fim_byte_t * p = img_->data; p < img_->data + 3*img_->i.width*img_->i.height ;p+=3)
		{ avg=p[0]+p[1]+p[2]; p[0]=p[1]=p[2]=(fim_byte_t) (avg/3); }

		setGlobalVariable("i:" FIM_VID_DESATURATED ,1-getGlobalIntVariable("i:" FIM_VID_DESATURATED ));

       		should_redraw();

		return true;
	}

	bool Image::negate(void)
	{
		/* NEW */

		/* FIXME */
		/*return gray_negate();*/
#if 0
		if(! img_ || ! img_->data)
			return false;
		if(!fimg_ || !fimg_->data)
			return false;
#endif

		if( fimg_ &&  fimg_->data)
		for( fim_byte_t * p = fimg_->data; p < fimg_->data + 3*fimg_->i.width*fimg_->i.height ;++p)
			*p = ~ *p;

		if(  img_ &&   img_->data)
		for( fim_byte_t * p = img_->data; p < img_->data + 3*img_->i.width*img_->i.height ;++p)
			*p = ~ *p;

		setGlobalVariable("i:" FIM_VID_NEGATED ,1-getGlobalIntVariable("i:" FIM_VID_NEGATED ));

       		should_redraw();

		return true;
	}

	int Image::n_pages()const{return (fimg_?fimg_->i.npages:0);}

	size_t Image::byte_size(void)const
	{
		size_t ms = 0;

		if(fimg_)
			ms += fimg_->i.height*fimg_->i.width*3;
		if(fimg_!=img_ && img_)
			ms += img_->i.height* img_->i.width*3;
#if FIM_WANT_EXPERIMENTAL_MIPMAPS
		ms += mm_.byte_size();
#endif /* FIM_WANT_EXPERIMENTAL_MIPMAPS */
		return ms;
	}

#if FIM_WANT_BDI
	Image::Image(void)
	{
		/* although invalid, this image instance should support all operations on it */
		// fim_bzero(this,sizeof(*this));
		reset();
		assert(check_invalid());
	}

	bool Image::can_reload(void)const{return !no_file_;}
	const fim_char_t* Image::getName(void)const{return fname_.c_str();}
	int Image::c_page(void)const{return page_;}
#endif	/* FIM_WANT_BDI */
}

