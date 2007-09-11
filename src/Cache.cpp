/* $Id$ */
/*
 Cache.cpp : Cache manager source file

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
#include "fim.h"
/*	#include <malloc.h>	*/
namespace fim
{
	Cache::Cache()
	{
		/*	FIXME : potential flaw ?	*/
		lru.erase(lru.begin(),lru.end());
	}

	int Cache::cached_elements()const
	{
		int count=0;
		cachels_t::const_iterator ci;
		for( ci=imageCache.begin();ci!=imageCache.end();++ci)++count;
		return count;
	}

	Image* Cache::get_lru( bool unused )
	{
		lru_t::const_iterator lrui;

		/* warning : syscall ! */
		time_t m_time;
		m_time = time(NULL);
		Image*  l_img=NULL;
		if ( cached_elements() < 1 ) return NULL;
		cachels_t::const_iterator ci;
		for( ci=imageCache.begin();ci!=imageCache.end();++ci)
		if( lru[ci->second] < m_time  && ( ! unused || usageCounter[ci->first]<=0  ) )
		{
			m_time = lru[ci->second];
			l_img  = ci->second;
		}
		return l_img;
	}

	int Cache::free_some_lru()
	{
		/*
		 * this triggering deletion (and memory freeying) of cached elements
		 * (yes, it is a sort of garbage collector, with its pros and cons)
		 */
		if ( cached_elements() < 1 ) return 0;
		return erase( get_lru()  );
	}

	int Cache::erase(fim::Image* oi)
	{
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!oi)
		{
			return -1;
		}

		if(is_in_cache(oi))
		{
			delete imageCache[reverseCache[oi]];
			lru.erase(oi);
			imageCache.erase(reverseCache[oi]);
			reverseCache.erase(oi);
			cc.setVariable("_cached_images",cached_elements());
			return 0;
		}
		return -1;
	}

	bool Cache::need_free()const
	{
		/*	temporary : we free elements for more than some cached images	*/

		/*
		struct mallinfo mi = mallinfo();
		cout << "allocated : " <<  mi.uordblks << "\n";
		if( mi.uordblks > cc.getIntVariable("_max_cached_memory") )return true;

		these are not the values we want ..
		*/

		int mci = cc.getIntVariable("_max_cached_images");
		if(mci==-1)return false;
		return ( cached_elements() > ( ( mci>0)?mci:-1 ) );
	}

	bool Cache::is_in_cache(fim::Image* oi)
	{
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!oi)return -1;
		return reverseCache[oi]!="";
	}

	int Cache::free(fim::Image* oi)
	{
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!oi)return -1;

		if(!is_in_cache(oi))
		{
			/* if the image is not already one of ours, it 
			 * is probably a cloned one, and the caller 
			 * didn't know this.
			 *
			 * in this case we keep it in the cache, 
			 * so it could be useful in the future.
			 * */
			if( oi->revertToLoaded() )//removes internal scale caches
				cacheImage( oi ); //FIXME : validity should be checked ..
		}

		/*
		 * fixme : we should explicitly mark for deletion someday.. 
		 * */

		//if(need_free())return erase(oi);
		/*	careful here !!	*/
		//if(need_free())free_some_lru();
		else return 0;	/* no free needed */
	}

	int Cache::prefetch(const char *fname)
	{
		if(need_free())free_some_lru();
		return getImage(fname)?0:-1;
	}

	bool Cache::haveImage(const char *fname)
	{
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!fname)return NULL;

		return ( this->imageCache[fim::string(fname)] != NULL );
	}

	bool Cache::freeCachedImage(Image *image)
	{
		if(image)
			return freeCachedImage(image->getName());
	}

	bool Cache::freeCachedImage(const char *fname)
	{
		/*
		 * declare this image as unused and decrease a relative counter.
		 * then delete image.
		 * a useImage action will do the converse operation.
		 *
		 * if the image was a clone, it is deleted!
		 * */
		int i;
		if(!haveImage(fname)) return true;	// we couldn't care less
		if( usageCounter[fname] <= 0 ) return false;//some problem!
		usageCounter[fname]--;
		if( usageCounter[fname] >= 0 && (i=cloneCache[fname].size())>0 )
		{
			delete cloneCache[fname][i-1];
			cloneCache[fname].pop_back();
		}
		return ( free( this->imageCache[fim::string(fname)] ) == 0 );
	}

	Image * Cache::useCachedImage(const char *fname)
	{
		/*
		 * declare this image as used an increase a relative counter.
		 * a freeImage action will do the converse operation (and delete).
		 * */
		Image * image=NULL;
		if(!haveImage(fname))
		{
			image = getImage(fname);
			if(haveImage(fname)) usageCounter[fname]=0;
			else return NULL; // lassimm' perd'
		}
		usageCounter[fname]++;
		image=getImage(fname);// in this way we update the LRU cache :)
		if( usageCounter[fname] >= 1 && image )image=image->getClone(); // EVIL !!
		return image;	
	}

	Image * Cache::getImage(const char *fname)
	{
		Image *ni = NULL;
	
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!fname)return ni;

		/*	cache lookup first	*/
		this->cached_elements();
		if(ni = this->imageCache[fim::string(fname)])
		{
			this->mark_used(fname);
			return ni;
		}
		
		/*	load attempt as alternative approach	*/
		if( ni = new Image(fname) )
		{	
			cacheImage( ni );
			mark_used( fname );
		}
		return ni;
	}

	bool Cache::cacheImage( fim::Image* ni )
	{
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!ni)return ni;

		this->imageCache[fim::string( ni->getName() )]=ni;
		this->reverseCache[ni]=fim::string( ni->getName() );
		cc.setVariable("_cached_images",cached_elements());
		mark_used( ni->getName() );
	}
	
	/*		*/
	int Cache::mark_used(const char *fname)
	{
		if(!fname) return -1;
		if(!imageCache[fim::string(fname)])return -1;
		if(fim::string(fname)=="")return -1;
		lru[imageCache[fim::string(fname)]]=time(NULL);
		return 0;
	}
}

