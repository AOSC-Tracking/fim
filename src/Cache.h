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

	cachels_t 	imageCache;
	rcachels_t	reverseCache;
	lru_t		lru;

	/*	the count of cached images	*/
	int cached_elements()const;

	/*	whether we should free some cache ..	*/
	bool need_free()const;

	/**/
	int mark_used(const char *fname);
	public:
	Cache();

	/*	erases the image from the cache	*/
	int erase(fim::Image* oi);

	/*	returns an image from the cache or loads it from disk	*/
	Image *getImage(const char *fname);
	
	/*	the caller declares this image as free	*/
	int free(fim::Image* oi);

	/**/
	int prefetch(const char *fname);

	Image* get_lru();
	int free_some_lru();
};
}

#endif

