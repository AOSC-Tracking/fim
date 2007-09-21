/* $Id$ */
/*
 Cache.h : Cache manager header file

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
#ifndef CACHE_FBVI_H
#define CACHE_FBVI_H
#include "fim.h"
namespace fim
{
class Cache
{	
	typedef std::map<fim::Image*,time_t> 	   lru_t;	//filename - last usage time
	typedef std::map<fim::string,fim::Image*>  cachels_t;	//filename - image
	typedef std::map<fim::Image*,fim::string>  rcachels_t;	//image - filename
	typedef std::map<fim::string,int >        ccachels_t;	//filename - counter
	typedef std::map<fim::string,std::vector<fim::Image*> > cloned_cachels_t;	//filename - cloned images??

	cachels_t 	imageCache;
	rcachels_t	reverseCache;
	lru_t		lru;
	ccachels_t	usageCounter;
	cloned_cachels_t cloneCache;
	std::set< fim::Image* > clone_pool;
//	clone_counter_t cloneCounter;

	/*	the count of cached images	*/
	int cached_elements()const;

	/*	whether we should free some cache ..	*/
	bool need_free()const;

	/**/
	int mark_used(const char *fname);

	bool is_in_cache(fim::Image* oi);
	bool is_in_clone_cache(fim::Image* oi);

	bool cacheNewImage( fim::Image* ni );
	Image * loadNewImage(const char *fname);

	/*	returns an image from the cache or loads it from disk marking it as used in the LRU (internal) */
	Image *getImage(const char *fname);

	/*	the caller declares this image as free	*/
//	int free(fim::Image* oi);

	/*	erases the image from the cache	*/
	int erase(fim::Image* oi);

	/*	erases the image clone from the cache	*/
	int erase_clone(fim::Image* oi);

	/* get the lru element. if unused is true, only an unused image will be returned, _if any_*/
	Image* get_lru( bool unused = false );

	int free_some_lru();

	bool free_all();
	
	/*	returns whether a file is already cached */
	bool haveImage(const char *fname);

	bool used_image(const char* fname);
	public:
	Cache();

	/*	free() and counter update */
	bool freeCachedImage(Image *image);

	/*	getImage() and counter update */
	Image * useCachedImage(const char *fname);

	/**/
	int prefetch(const char *fname);

};
}

#endif

