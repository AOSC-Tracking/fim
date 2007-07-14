/* $Id$ */
/*
 Cache.cpp : Cache manager source file

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

	int Cache::cached_elements()
	{
		int count=0;
		cachels_t::const_iterator ci;
		for( ci=imageCache.begin();ci!=imageCache.end();++ci)++count;
		return count;
	}

	Image* Cache::get_lru()
	{
		lru_t::const_iterator lrui;

		/* warning : syscall ! */
		time_t m_time;
		m_time = time(NULL);
		Image*  l_img=NULL;
		if ( cached_elements() < 1 ) return NULL;
		cachels_t::const_iterator ci;
		for( ci=imageCache.begin();ci!=imageCache.end();++ci)
		if( lru[ci->second] < m_time )
		{
			m_time = lru[ci->second];
			l_img  = ci->second;
		}
		return l_img;
	}

	int Cache::free_some_lru()
	{
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
		if(reverseCache[oi]!="")
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

	bool Cache::need_free()
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

	int Cache::free(fim::Image* oi)
	{
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!oi)return -1;
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

	Image *Cache::getImage(const char *fname)
	{
		Image *ni = NULL;
	
		/*	acca' nun stimm'a'ppazzia'	*/
		if(!fname)return ni;

		/*	cache lookup first	*/
		cached_elements();
		if(ni = imageCache[fim::string(fname)])
		{
			mark_used(fname);
			return ni;
		}
		
		/*	load attempt as alternative approach	*/
		if( ni = new Image(fname) )
		{	
			imageCache[fim::string(fname)]=ni;
			reverseCache[ni]=fim::string(fname);
			cc.setVariable("_cached_images",cached_elements());
			mark_used(fname);
		}
		return ni;
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
