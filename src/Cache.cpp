/* $LastChangedDate$ */
/*
 Cache.cpp : Cache manager source file

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
#include "fim.h"
/*	#include <malloc.h>	*/
#ifdef HAVE_SYS_TIME_H
	#include <sys/time.h>
#else /* HAVE_SYS_TIME_H */
	#include <time.h>
#endif /* HAVE_SYS_TIME_H */

#define FIM_CACHE_MIPMAP 1
#define FIM_CACHE_INSPECT 0
#if FIM_CACHE_INSPECT
#define FIM_PR(X) printf("CACHE:%c:%20s:%s",X,__func__,getReport(FIM_CR_CD).c_str());
#else /* FIM_CACHE_INSPECT */
#define FIM_PR(X) 
#endif /* FIM_CACHE_INSPECT */

//#define FIM_CACHE_DEBUG 0

#if 0
#define FIM_LOUD_CACHE_STUFF FIM_PR(-10); FIM_LINE_COUT
#else
#define FIM_LOUD_CACHE_STUFF 
#endif
#define FIM_VCBS(VI) ( sizeof(VI) + VI.size() * ( sizeof(vcachels_t::mapped_type) + sizeof(vcachels_t::key_type) ) )
/* TODO: maybe fim_basename_of is excessive ?  */
	extern CommandConsole cc;
namespace fim
{
	static fim_time_t fim_time(void) FIM_NOEXCEPT /* stand-alone function */
	{
#ifdef HAVE_SYS_TIME_H
		struct timeval tv;
		FIM_CONSTEXPR fim_time_t prec = 1000; /* fraction of second precision */
		gettimeofday(&tv, FIM_NULL);
		return tv.tv_sec * prec + tv.tv_usec / ( 1000000 / prec );
#else /* HAVE_SYS_TIME_H */
		return time(FIM_NULL);
#endif /* HAVE_SYS_TIME_H */
	}

	fim_time_t Cache::get_reltime(void)const
	{
		return fim_time()-time0_;
	}

	Cache::Cache(void)
		:Namespace(&cc)
		,time0_(fim_time())
	{
		FIM_LOUD_CACHE_STUFF;
	}

	int Cache::cached_elements(void)const
	{
		/*	the count of cached images	*/
		FIM_LOUD_CACHE_STUFF;
		return imageCache_.size();
	}

	ImagePtr Cache::get_lru( bool unused )const
	{
		/* get the least recently used element.
		 * if unused is true, only an unused image will be returned, _if any_*/
		lru_t::const_iterator lrui;
		fim_time_t m_time(get_reltime()), l_time=0;
		ImagePtr  l_img(FIM_NULL);
		cachels_t::const_iterator ci;
		FIM_LOUD_CACHE_STUFF;

		if ( cached_elements() < 1 )
			goto ret;

		for( ci=imageCache_.begin();ci!=imageCache_.end();++ci)
		if( ci->second /* <- so we can call this function in some intermediate states .. */
			 && (l_time=last_used(ci->first)) < m_time  &&  (  (! unused) || (used_image(ci->first)<=0)  ) )
		{
			l_img  = ci->second;
			m_time = l_time;
		}
ret:
		return l_img;
	}

	int Cache::free_some_lru(void)
	{
		/*
		 * trigger deletion (and memory free) of cached elements
		 * (a sort of garbage collector)
		 */
		FIM_LOUD_CACHE_STUFF;
		FIM_PR(' ');
		if ( cached_elements() < 1 )
			return 0;
		return erase( get_lru(true)  );
	}

	int Cache::erase_clone(fim::ImagePtr oi)
	{
		/*	erases the image clone from the cache	*/
		FIM_LOUD_CACHE_STUFF;
		FIM_PR(' ');
#ifdef FIM_CACHE_DEBUG
		std::cout << "erasing clone " << fim_basename_of(oi->getName()) << "\n";
#endif /* FIM_CACHE_DEBUG */
		//cloneUsageCounter_.erase(oi);
#if FIM_IMG_NAKED_PTRS
		delete oi;
#else /* FIM_IMG_NAKED_PTRS */
		// TBD
#endif /* FIM_IMG_NAKED_PTRS */
		clone_pool_.erase(oi);
		return 0;
	}

	bool Cache::need_free(void)const
	{
		/*	whether we should free some cache ..	*/
		/*	temporary : we free elements for more than some cached images	*/
		/*
		struct mallinfo mi = mallinfo();
		cout << "allocated : " <<  mi.uordblks << "\n";
		if( mi.uordblks > getIntGlobalVariable(FIM_VID_MAX_CACHED_MEMORY) )
			return true;

		these are not the values we want ..
		*/
		int mci = getGlobalIntVariable(FIM_VID_MAX_CACHED_IMAGES);
		int mcm = getGlobalIntVariable(FIM_VID_MAX_CACHED_MEMORY); /* getIntGlobalVariable */
		size_t smcm = mcm > 0 ? mcm : 0;

	       	if( smcm > 0 && img_byte_size()/FIM_CNS_CSU > smcm )
			goto rt;

		if(mci==-1)
			goto rf;

		/* return ( cached_elements() > ( ( mci>0)?mci:-1 ) ); */
		if(mci > 0 && cached_elements() > mci)
			goto rt;
rf:
		return false;
rt:
		return true;
	}

	int Cache::used_image(cache_key_t key)const
	{
		FIM_LOUD_CACHE_STUFF;
		return ( usageCounter_.find(key)!=usageCounter_.end() ) ?  (*(usageCounter_.find(key))).second : 0;
	}

	bool Cache::is_in_clone_cache(fim::ImagePtr oi)const
	{
		FIM_LOUD_CACHE_STUFF;
		if(!oi)
			return false;
		return ( clone_pool_.find(oi)!=clone_pool_.end() )	
			&&
			((*clone_pool_.find(oi)) == oi );
	}

	bool Cache::is_in_cache(cache_key_t key)const
	{
		FIM_LOUD_CACHE_STUFF;
		return ( imageCache_.find(key)!=imageCache_.end() )
			&&
			((*(imageCache_.find(key))).second!=FIM_NULL) ;
	}

	int Cache::prefetch(cache_key_t key)
	{
		int retval = 0;
		FIM_PR('*');

		FIM_LOUD_CACHE_STUFF;
		if(is_in_cache(key))
		{
			FIM_PR('c');
			goto ret;
		}
#if 0
	  	if(need_free())
			free_some_lru();
		if(need_free())
		{
			FIM_PR('f');
			goto ret; /* skip prefetch if cache is full */
		}
#endif
		if(key.first == FIM_STDIN_IMAGE_NAME)
		{
			FIM_PR('s');
			goto ret;// just a fix in the case the browser is still lame
		}
#ifdef FIM_CACHE_DEBUG
		std::cout << "prefetch request for "<< key.first << " \n";
#endif /* FIM_CACHE_DEBUG */

    		if( regexp_match(key.first.c_str(),FIM_CNS_ARCHIVE_RE,1) )
		{
			/* FIXME: This is a hack. One shall determine unprefetchability othwerwise. */
			FIM_PR('j');
			goto ret;
		}

		if(!loadNewImage(key,0,true))
		{
			retval = -1;
#ifdef FIM_CACHE_DEBUG
			std::cout << "loading failed\n";
#endif /* FIM_CACHE_DEBUG */
			goto ret;
		}
		else
		{
			FIM_PR('l');
		}
		setGlobalVariable(FIM_VID_CACHED_IMAGES,(fim_int)cached_elements());
		setGlobalVariable(FIM_VID_CACHE_STATUS,getReport().c_str());
ret:
		FIM_PR('.');
		return retval;
	}

	ImagePtr Cache::loadNewImage(cache_key_t key, fim_page_t page, fim_bool_t delnc)
	{
		ImagePtr ni = FIM_NULL;
		FIM_PR('*');

		FIM_LOUD_CACHE_STUFF;
		/*	load attempt as alternative approach	*/
		try
		{
		if( ( ni = ImagePtr( new Image(key.first.c_str(), FIM_NULL, page) ) ) )
		{
#ifdef FIM_CACHE_DEBUG
			std::cout << "loadNewImage("<<key.first.c_str()<<")\n";
#endif /* FIM_CACHE_DEBUG */
			if( ni->cacheable() )
				cacheNewImage( ni );
			else
				if (delnc) // delete non cacheable
				{
#if FIM_IMG_NAKED_PTRS
					delete ni;
#else /* FIM_IMG_NAKED_PTRS */
					// TBD
#endif /* FIM_IMG_NAKED_PTRS */
					ni = FIM_NULL;
				}
		}
		}
		catch(FimException e)
		{
			FIM_PR('E');
			ni = FIM_NULL; /* not a big problem */
//			if( e != FIM_E_NO_IMAGE )throw FIM_E_TRAGIC;  /* hope this never occurs :P */
		}
		FIM_PR(' ');
		return ni;
	}
	
	ImagePtr Cache::getCachedImage(cache_key_t key)
	{
		/* returns an image if already in cache. */
		ImagePtr ni = FIM_NULL;
		FIM_LOUD_CACHE_STUFF;
		FIM_PR(' ');
	
		if( ( ni = this->imageCache_[key]) )
			this->lru_touch(key);
		return ni;
	}

	bool Cache::cacheNewImage( fim::ImagePtr ni )
	{
		FIM_LOUD_CACHE_STUFF;
		FIM_PR(' ');
#ifdef FIM_CACHE_DEBUG
		std::cout << "going to cache: "<< ni << "\n";
#endif /* FIM_CACHE_DEBUG */
		this->imageCache_[ni->getKey()]=ni;
		lru_touch( ni->getKey() );
		usageCounter_[ ni->getKey()]=0; // we don't assume any usage yet
		setGlobalVariable(FIM_VID_CACHED_IMAGES,(fim_int)cached_elements());
		return true;
	}
	
	int Cache::erase(fim::ImagePtr oi)
	{
		/*	erases the image from the cache	*/
		int retval=-1;
		cache_key_t key;
		FIM_PR(' ');

		FIM_LOUD_CACHE_STUFF;
		if(!oi)
			goto ret;

		if( is_in_cache(key = oi->getKey()) )
		{
			usageCounter_[key]=0;
			/* NOTE : the user should call usageCounter_.erase(key) after this ! */
#ifdef FIM_CACHE_DEBUG
			std::cout << "will erase  "<< oi << " " <<  fim_basename_of(oi->getName()) << " time:"<< lru_[oi->getKey()] << "\n";
			std::cout << "erasing original " << fim_basename_of(oi->getName()) << "\n";
#endif /* FIM_CACHE_DEBUG */
			lru_.erase(key);
			imageCache_.erase(key);
			usageCounter_.erase(key);
#if FIM_IMG_NAKED_PTRS
			delete oi;
#else /* FIM_IMG_NAKED_PTRS */
			// TBD
#endif /* FIM_IMG_NAKED_PTRS */
			setGlobalVariable(FIM_VID_CACHED_IMAGES,(fim_int)cached_elements());
			retval = 0;
		}
ret:
		return retval;
	}

	fim_time_t Cache::last_used(cache_key_t key)const
	{
		fim_time_t retval=0;

		FIM_LOUD_CACHE_STUFF;
		if(imageCache_.find(key)==imageCache_.end())
			goto ret;
		if(lru_.find(key)==lru_.end())
			goto ret;
		retval = lru_.find(key)->second;
ret:
		return retval;
	}

	int Cache::lru_touch(cache_key_t key)
	{
		/*
		 * if the specified file is cached, in this way it is marked as used, too.
		 * the usage count is not affected, 
		 * */
		FIM_LOUD_CACHE_STUFF;
		FIM_PR(' ');
//		std::cout << lru_[key] << " -> ";
		lru_[key]= get_reltime();
//		std::cout << lru_[key] << "\n";
		return 0;
	}

	bool Cache::freeCachedImage(ImagePtr image, const ViewportState *vsp)
	{
		/*
		 * Shall rename to free().
		 *
		 * If the supplied image is cached as a master image of a clone, it is freed and deregistered.
		 * If not, no action is performed.
		 * */
		// WARNING : FIXME : DANGER !!
		FIM_LOUD_CACHE_STUFF;
		FIM_PR('*');

		if( !image )
			goto err;

		if(vsp)
			viewportInfo_[image->getKey()] = *vsp;
		if( is_in_clone_cache(image) )
		{
			usageCounter_[image->getKey()]--;
			FIM_PR('c');
			erase_clone(image);	// we _always_ immediately delete clones
			setGlobalVariable(FIM_VID_CACHE_STATUS,getReport().c_str());
			goto ret;
		}
		else
		if( is_in_cache(image->getKey()) )
		{
			FIM_PR('-');
			lru_touch( image->getKey() ); // we have been using it until now
			usageCounter_[image->getKey()]--;
#if FIM_WANT_MIPMAPS
			if( getGlobalStringVariable(FIM_VID_CACHE_CONTROL).find('M') == 0 )
				image->mm_free();
#endif /* FIM_WANT_MIPMAPS */
			if(
				(usageCounter_[image->getKey()])==0 && 
				image->getKey().second!=FIM_E_STDIN 
				)
			{
				int minci = getGlobalIntVariable(FIM_VID_MIN_CACHED_IMAGES);

				if ( minci < 1 )
					minci = 4;
#if 0
				if( need_free() && image->getKey().second!=FIM_E_STDIN )
				{
					cache_key_t key = image->getKey();
					this->erase( image );
					usageCounter_.erase(key);
				}
#else
				/* doing it here is dangerous : */
				if( need_free() && cached_elements() > minci )
				{
					ImagePtr lrui = get_lru(true);
					FIM_PR('o');
					if( lrui ) 
					{
						cache_key_t key = lrui->getKey();
						if( FIM_VCBS(viewportInfo_) > FIM_CNS_VICSZ )
							viewportInfo_.erase(key);
						if(( key.second != FIM_E_STDIN ))
						{	
							this->erase( lrui );
						}
					}
					// missing usageCounter_.erase()..
				}
#endif
			}
			setGlobalVariable(FIM_VID_CACHE_STATUS,getReport().c_str());
			goto ret;
		}
err:
		FIM_PR('.');
		return false;
ret:
		FIM_PR('.');
		return true;
	}

	ImagePtr Cache::useCachedImage(cache_key_t key, ViewportState *vsp, fim_page_t page)
	{
		/*
		 * Shall rename to get().
		 *
		 * The caller invokes this member function to obtain an Image object pointer.
		 * If the object is cached and it already used, a clone is built and returned.
		 *
		 * If we have an unused master copy, we return that.
		 *
		 * Then declare this image as used and increase a relative counter.
		 *
		 * A freeImage action will do the converse operation (and delete).
		 * If the image is not already cached, it is loaded, if possible.
		 *
		 * So, if there is no such image, FIM_NULL is returned
		 * */
		ImagePtr image = FIM_NULL;

		FIM_LOUD_CACHE_STUFF;
		FIM_PR('*');

#ifdef FIM_CACHE_DEBUG
		std::cout << "  useCachedImage(\""<<fim_basename_of(key.first.c_str())<<" of type "<< ( key.second == FIM_E_FILE ? " file ": " stdin ")<<"\")\n";
#endif /* FIM_CACHE_DEBUG */
		if(!is_in_cache(key)) 
		{
#ifdef FIM_CACHE_DEBUG
			std::cout << "not in the cache: "<< key.first << " \n";
#endif /* FIM_CACHE_DEBUG */
			image = loadNewImage(key,page,false);
			if(!image)
				goto ret; // bad luck!
			if(!image->cacheable())
				goto ret; // we keep it but don't cache it
			usageCounter_[key]=1;
		}
		else // is_in_cache(key)
		{
			image = getCachedImage(key);// in this way we update the LRU cache :)
			if(!image)
			{
#ifdef FIM_CACHE_DEBUG
				cout << "critical internal cache error!\n";
#endif /* FIM_CACHE_DEBUG */
				goto done;
			}
			if( used_image( key ) )
			{
				// if the image was already used, cloning occurs
				try
				{
#ifdef FIM_CACHE_DEBUG
					ImagePtr oi=image;
#endif /* FIM_CACHE_DEBUG */
					image = ImagePtr ( new Image(*image) ); // cloning
#ifdef FIM_CACHE_DEBUG
					std::cout << "  cloned image: \"" <<fim_basename_of(image->getName())<< "\" "<< image << " from \""<<fim_basename_of(oi->getName()) <<"\" " << oi << "\n";
#endif /* FIM_CACHE_DEBUG */
				}
				catch(FimException e)
				{
					FIM_PR('E');
					/* we will survive :P */
					image = FIM_NULL; /* we make sure no taint remains */
//					if( e != FIM_E_NO_IMAGE )throw FIM_E_TRAGIC;  /* hope this never occurs :P */
				}
				if(!image)
					goto ret; //means that cloning failed.
				clone_pool_.insert(image); // we have a clone
				//cloneUsageCounter_[image]=1;
			}
#if FIM_WANT_MIPMAPS
			if( getGlobalStringVariable(FIM_VID_CACHE_CONTROL).find('M') == 0 )
				image->mm_make();
#endif /* FIM_WANT_MIPMAPS */
			lru_touch( key );
			// if loading and eventual cloning succeeded, we count the image as used of course
			usageCounter_[key]++;
		}
done:
		setGlobalVariable(FIM_VID_CACHE_STATUS,getReport().c_str());
ret:
		if(vsp && image)
		{
			*vsp = viewportInfo_[image->getKey()];
			/* *vsp = viewportInfo_[key]; */
		}
		FIM_PR('.');
		return image;
	}

	ImagePtr Cache::setAndCacheStdinCachedImage(ImagePtr image)
	{
		/* Cache an image coming from stdin (that is, not reloadable).
		 * */
		cache_key_t key(FIM_STDIN_IMAGE_NAME,FIM_E_STDIN);
		FIM_LOUD_CACHE_STUFF;
		FIM_PR('*');

		if(!image)
			goto ret;
		
		try
		{
#ifdef FIM_CACHE_DEBUG
			ImagePtr oi=image;
#endif /* FIM_CACHE_DEBUG */
			image = ImagePtr ( new Image(*image) ); // cloning
			if(image)
			{
				cacheNewImage( image );
			}
		}
		catch(FimException e)
		{
			FIM_PR('E');
			/* we will survive :P */
			image = FIM_NULL; /* we make sure no taint remains */
//			if( e != FIM_E_NO_IMAGE )throw FIM_E_TRAGIC;  /* hope this never occurs :P */
		}
		if(!image)
			goto ret; //means that cloning failed.
		setGlobalVariable(FIM_VID_CACHE_STATUS,getReport().c_str());
ret:
		return image;	//so, it could be a clone..
	}

	fim::string Cache::getReport(int type)const
	{
		/* TODO: rename to info(). */
		fim::string cache_report;

		if(type == FIM_CR_CN || type == FIM_CR_CD)
		{
			fim_char_t buf[FIM_PRINTFNUM_BUFSIZE];
			int mci = getGlobalIntVariable(FIM_VID_MAX_CACHED_IMAGES);
			int mcm = getGlobalIntVariable(FIM_VID_MAX_CACHED_MEMORY);
			mcm = mcm >= 0 ? mcm*FIM_CNS_CSU:0;
			cache_report  = " ";
			cache_report += "count:";
			cache_report += fim::string(cached_elements());
			cache_report += "/";
			cache_report += fim::string(mci);
			cache_report += " ";
			cache_report += "occupation:";
			fim_snprintf_XB(buf, sizeof(buf), img_byte_size());cache_report += buf;
			cache_report += "/";
			fim_snprintf_XB(buf, sizeof(buf), mcm);cache_report += buf;
			cache_report += " ";
			for(ccachels_t::const_iterator ci=usageCounter_.begin();ci!=usageCounter_.end();++ci)
			if(
			  ( type == FIM_CR_CD && ( imageCache_.find(ci->first) != imageCache_.end() ) ) ||
			  ( type == FIM_CR_CN && ( imageCache_.find(ci->first) != imageCache_.end()  && ci->second) )
			  )
			{
				cache_report+= fim_basename_of((*ci).first.first.c_str());
				cache_report+=":";
				cache_report+=fim::string((*ci).second);
				cache_report+=":";
				fim_snprintf_XB(buf, sizeof(buf), imageCache_.find(ci->first)->second->byte_size());
				cache_report += buf;
				cache_report+="@";
				cache_report+=fim::string((size_t)last_used(ci->first));
				cache_report+=" ";
			}
			cache_report += "\n";
			goto ret;
		}
		cache_report = "cache contents : \n";
		FIM_LOUD_CACHE_STUFF;
#if 0
		cachels_t::const_iterator ci;
		for( ci=imageCache_.begin();ci!=imageCache_.end();++ci)
		{	
			cache_report+=((*ci).first);
			cache_report+=" ";
			cache_report+=fim::string(usageCounter_[((*ci).first)]);
			cache_report+="\n";
		}
#else
		for(ccachels_t::const_iterator ci=usageCounter_.begin();ci!=usageCounter_.end();++ci)
		{	
			cache_report+=((*ci).first.first);
			cache_report+=":";
			cache_report+=fim::string((*ci).first.second);
			cache_report+=" ,usage:";
			cache_report+=fim::string((*ci).second);
			cache_report+="\n";
		}
		cache_report += "clone pool contents : \n";
		for(std::set< fim::ImagePtr >::const_iterator  cpi=clone_pool_.begin();cpi!=clone_pool_.end();++cpi)
		{	
			cache_report+=fim_basename_of((*cpi)->getName());
			cache_report+=" " ; 
#if FIM_IMG_NAKED_PTRS
			cache_report+= string((int*)(*cpi));
#else /* FIM_IMG_NAKED_PTRS */
			cache_report+= string((int*)((*cpi).get()));
#endif /* FIM_IMG_NAKED_PTRS */
			cache_report+=",";
		}
		cache_report+="\n";
#endif
ret:
		return cache_report;
	}

	Cache::~Cache(void)
	{
		cachels_t::const_iterator ci;
		FIM_LOUD_CACHE_STUFF;
		for( ci=imageCache_.begin();ci!=imageCache_.end();++ci)
			if(ci->second)
			{
#ifdef FIM_CACHE_DEBUG
				std::cout << "about to free " << (ci->first.first) << "\n";
#endif /* FIM_CACHE_DEBUG */
#if FIM_IMG_NAKED_PTRS
				delete ci->second;
#else /* FIM_IMG_NAKED_PTRS */
				// TBD
#endif /* FIM_IMG_NAKED_PTRS */
			}
		imageCache_.clear(); /* destroy Image objects */
	}

	size_t Cache::img_byte_size(void)const
	{
		size_t bs = 0;
		cachels_t::const_iterator ci;

		FIM_LOUD_CACHE_STUFF;
		for( ci=imageCache_.begin();ci!=imageCache_.end();++ci)
			if(ci->second)
				bs += ci->second->byte_size();
		return bs;
	}

	size_t Cache::byte_size(void)const
	{
		size_t bs = 0;
		cachels_t::const_iterator ci;
		FIM_LOUD_CACHE_STUFF;
		bs += img_byte_size();
		bs += sizeof(*this);
		bs += FIM_VCBS(viewportInfo_);
		/* 
		bs += sizeof(usageCounter_);
		TODO: incomplete ...
		*/
		return bs;
	}

	void Cache::touch(cache_key_t key)
	{
		FIM_PR('*');
		getCachedImage(key);
		FIM_PR('.');
       	}

	void Cache::desc_update(void)
	{
#if FIM_WANT_PIC_CMTS
		cachels_t::const_iterator ci;

		FIM_LOUD_CACHE_STUFF;
		for( ci=imageCache_.begin();ci!=imageCache_.end();++ci)
			if(ci->second)
				ci->second->desc_update();
#endif /* FIM_WANT_PIC_CMTS */
	}
}

