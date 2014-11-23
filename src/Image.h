/* $LastChangedDate$ */
/*
 Image.h : Image class headers

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

#ifndef FIM_IMAGE_H
#define FIM_IMAGE_H

#include "FbiStuff.h"
#include "fim.h"

namespace fim
{
/*
 *	A general rule in programming the Image methods is that
 *	of keeping them minimal.
 *	Combining them is matter of the invoking function.
 *	So, there is NO internal triggering  here to call ANY 
 *	display function.
 *
 *
 * 	WARNING : 
 *
 *	This class is evolving towards something
 *	reflecting the content of an image file and 
 *	some rescaled images caches.
 *
 *	Note that the way our code stores image data is device-dependent.
 *	Therefore the need for a framebufferdevice reference in Image.
 *
 *	TODO : separate Image in a way multiple viewports could use the same image.
 *	TODO : rename framebufferdevice.redraw -> this.need_redraw
 */

#ifdef FIM_NAMESPACES
class Image:public Namespace
#else /* FIM_NAMESPACES */
class Image
#endif /* FIM_NAMESPACES */
{

	friend class Viewport;		/* don't panic, we are wise people ;) */

	public:

	Image(const fim_char_t *fname, FILE *fd=NULL);
	Image(const fim_char_t *fname, Foo& foo, FILE *fd=NULL);
	~Image(void);

	bool prev_page(int j=+1);
	bool next_page(int j=+1);
	int n_pages(void)const;
	bool is_multipage(void)const;
	bool have_nextpage(int j=1)const;
	bool have_prevpage(int j=1)const;

	private:
	Image& operator= (const Image &i){return *this;/* a nilpotent assignation */}
	fim_scale_t            scale_;	/* viewport variables */
	fim_scale_t            ascale_;
	fim_scale_t            newscale_;
	fim_scale_t            angle_;
	fim_scale_t            newangle_;
	fim_page_t		 page_;

	/* virtual stuff */
	public://TMP
        struct ida_image *img_     ;     /* local (eventually) copy images */
	bool reload(void);
	private://TMP
#if FIM_WANT_EXPERIMENTAL_MIPMAPS
	fim_mipmap_t mm_;
#endif /* FIM_WANT_EXPERIMENTAL_MIPMAPS */
	struct ida_image *fimg_    ;     /* master image */

	/* image methods */
	bool load(const fim_char_t *fname, FILE *fd, int want_page);
	void should_redraw(int should=1)const;

	protected:
	enum { FIM_NO_ROT=0,FIM_ROT_0Q=0,FIM_ROT_1Q=1,FIM_ROT_2Q=2,FIM_ROT_3Q=3,FIM_ROT_4Q=4,FIM_ROT_ROUND=4 };
	fim_pgor_t              orientation_;	//aka rotation

	fim_bool_t invalid_;		//the first time the image is loaded it is set to 1
	fim_bool_t no_file_;	//no file is associated to this image (used for reading from /dev/stdin at most once.)
	fim_image_source_t fis_;

	string  fname_;		/* viewport variable, too */
	size_t fs_;		/* file size */
	size_t ms_;		/* memory size */

        void free(void);
	void reset(void);

        bool tiny(void)const;
	public:
	virtual size_t byte_size(void)const;

	bool can_reload(void)const;
	bool update(void);

	fim::string getInfo(void);
	Image(const Image& image); // yes, a private constructor (was)
#if FIM_WANT_BDI
	Image(void);
#endif	/* FIM_WANT_BDI */
	fim_err_t rescale( fim_scale_t ns=0.0 );
	fim_err_t rotate( fim_scale_t angle_=1.0 );

	const fim_char_t* getName(void)const;
	cache_key_t getKey(void)const;

	/* viewport methods */

	void reduce( fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	void magnify(fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	
	fim_pgor_t getOrientation(void)const;

	fim_err_t setscale(fim_scale_t ns);
	/* viewport methods ? */
	fim_err_t scale_increment(fim_scale_t ds);
	fim_err_t scale_multiply (fim_scale_t sm);
	bool negate (void);/* let's read e-books by consuming less power :) */
	bool desaturate (void);
	bool gray_negate(void);

	bool check_invalid(void);
	bool check_valid(void);

	int width(void)const;
	fim_coo_t original_width(void)const;
	int height(void)const;
	fim_coo_t original_height(void)const;
	bool goto_page(fim_page_t j);

	Image * getClone(void);
//	void resize(int nw, int nh);
	int c_page(void)const;
};
}
#endif /* FIM_IMAGE_H */
