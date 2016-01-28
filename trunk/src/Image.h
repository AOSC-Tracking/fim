/* $LastChangedDate$ */
/*
 Image.h : Image class headers

 (c) 2007-2016 Michele Martone

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
#if FIM_WANT_PIC_CMTS
/* FIXME: temporarily here */
#include <iostream>
#include <fstream>
#include <istream>
#include <ios>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#endif /* FIM_WANT_PIC_CMTS */

namespace fim
{
/*
 *	A general rule in programming the Image member functions is that
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
 */

#ifdef FIM_NAMESPACES
class Image:public Namespace
#else /* FIM_NAMESPACES */
class Image
#endif /* FIM_NAMESPACES */
{
	public:

	Image(const fim_char_t *fname, FILE *fd=FIM_NULL, fim_page_t page = 0);
	Image(const fim_char_t *fname, Foo& foo, FILE *fd=FIM_NULL);
	~Image(void);

	bool prev_page(int j=+1);
	bool next_page(int j=+1);
	int n_pages(void)const;
	bool is_multipage(void)const;
	int is_mirrored(void)const;
	int is_flipped(void)const;
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
	enum { FIM_ROT_L=3,FIM_ROT_R=1,FIM_ROT_U=2 };
        struct ida_image *img_     ;     /* local (eventually) copy images */
	bool reload(void);
	private://TMP
#if FIM_WANT_MIPMAPS
	fim_mipmap_t mm_;
#endif /* FIM_WANT_MIPMAPS */
	struct ida_image *fimg_    ;     /* master image */

	/* image member functions */
	bool load(const fim_char_t *fname, FILE *fd, int want_page);
	public:
	void should_redraw(enum fim_redraw_t sr = FIM_REDRAW_NECESSARY) { redraw_ = sr; }  /* for Viewport after drawing */

	protected:
	fim_redraw_t redraw_;
	enum { FIM_NO_ROT=0,FIM_ROT_ROUND=4 };
	enum { FIM_ROT_L_C='L',FIM_ROT_R_C='R',FIM_ROT_U_C='U' };
	enum { FIM_I_ROT_L=0, FIM_I_ROT_R=1}; /* internal */
	fim_pgor_t              orientation_;	// orthogonal rotation

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
	void reset_scale_flags(void);
	virtual size_t byte_size(void)const;

	bool can_reload(void)const;
	bool update(void);

	fim::string getInfo(void);
	fim::string getInfoCustom(const fim_char_t * ifsp)const;
	Image(const Image& image); // yes, a private constructor (was)
#if FIM_WANT_BDI
	Image(void);
#endif	/* FIM_WANT_BDI */
	fim_err_t rescale( fim_scale_t ns=0.0 );
	fim_err_t rotate( fim_scale_t angle_=1.0 );

	const fim_char_t* getName(void)const;
	cache_key_t getKey(void)const;

	/* viewport member functions */

	void reduce( fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	void magnify(fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	
	fim_pgor_t getOrientation(void)const;

	fim_err_t setscale(fim_scale_t ns);
	/* viewport member functions ? */
	fim_err_t scale_increment(fim_scale_t ds);
	fim_err_t scale_multiply (fim_scale_t sm);
	fim_scale_t ascale()const{ return (ascale_>0.0?ascale_:1.0); }
	bool negate (void);/* let's read e-books by consuming less power :) */
	bool desaturate (void);
	bool gray_negate(void);

	bool check_invalid(void);
	bool check_valid(void);
	bool valid(void)const{return !invalid_;}

	int width(void)const;
	fim_coo_t original_width(void)const;
	int height(void)const;
	fim_coo_t original_height(void)const;
	bool goto_page(fim_page_t j);

	Image * getClone(void);
//	void resize(int nw, int nh);
	fim_int c_page(void)const;
#if FIM_WANT_MIPMAPS
	void mm_free(void);
	void mm_make(void);
	bool has_mm(void)const;
#endif /* FIM_WANT_MIPMAPS */
	bool cacheable(void)const;
	void desc_update();
	fim_bool_t need_redraw(void)const{ return (redraw_ != FIM_REDRAW_UNNECESSARY); }
};
}
#if FIM_WANT_PIC_LVDN
	class VNamespace: public Namespace { size_t byte_size(void)const{return 0; /* FIXME */}; };
#endif /* FIM_WANT_PIC_LVDN */

#if FIM_WANT_PIC_CMTS
/* FIXME: temporarily here */
typedef std::string fim_fn_t; /* file name */
typedef std::string fim_ds_t; /* file description */

class ImgDscs: public std::map<fim_fn_t,fim_ds_t>
{
	public:
#if FIM_WANT_PIC_LVDN
	typedef std::map<std::string,VNamespace> vd_t;
	vd_t vd_;
#endif /* FIM_WANT_PIC_LVDN */
	template<class T> struct ImgDscsCmp:public std::binary_function<typename T::value_type, typename T::mapped_type, bool>
	{
		public:
		bool operator() (const typename T::value_type &vo, const typename T::mapped_type mo) const
		{
			return vo.second == mo;
		}
	};
	ImgDscs::iterator li_;
	ImgDscs::iterator fo(const key_type & sk, const ImgDscs::iterator & li)
	{
		return std::find_if(li,end(),std::bind2nd(ImgDscsCmp<ImgDscs>(),sk));
	}
	ImgDscs::const_iterator fo(const key_type & sk, const ImgDscs::const_iterator & li)const
	{
		return std::find_if(li,end(),std::bind2nd(ImgDscsCmp<ImgDscs>(),sk));
	}
public:
	ImgDscs::const_iterator fo(const key_type & sk)const
	{
		return std::find_if(begin(),end(),std::bind2nd(ImgDscsCmp<ImgDscs>(),sk));
	}
private:
	ImgDscs::iterator fi(const key_type & sk)
	{
		ImgDscs::iterator li;

		if(li_ == end())
			li_ = begin();
		li = li_ = fo(sk,li_);
		if(li_ == end())
			li_ = begin();
		else
		++li_;
		return li;
	}
public:
	void reset(void)
	{
		li_=begin();
	}
	ImgDscs(void)
	{
		reset();
	}
	void fetch(const fim_fn_t &dfn, const fim_char_t sc)
	{
		/* dfn: descriptions file name */
		/* sc: separator char */
		std::ifstream mfs (dfn.c_str(),std::ios::app);
		std::string ln;
#if FIM_WANT_PIC_RCMT
		std::string ld; // last description
#endif /* FIM_WANT_PIC_RCMT */
#if FIM_WANT_PIC_LVDN
		VNamespace ns;
#endif /* FIM_WANT_PIC_LVDN */

		while( std::getline(mfs,ln))
		{
			std::stringstream  ls(ln);
			key_type fn;
			const fim_char_t nl = '\n';
			fim_fn_t ds;

#if FIM_WANT_PIC_LVDN
			if( ls.peek() == FIM_SYM_PIC_CMT_CHAR )
			{
				if( std::getline(ls,fn,nl) )
				{
					/* ignoring it: it's a comment line */
					size_t vn = fn.find_first_of("!fim:",1);
					if( vn != std::string::npos && fn[vn+=5] )
					{
						size_t es = fn.find_first_of("=",vn);
						if( es != std::string::npos )
						{
							std::string varname = fn.substr(vn,es-vn);
							++es;
							if( fn[es] )
							{
								std::string varval = fn.substr(es);
								ns.setVariable(varname,Var(varval));
							}
							else
							{
								std::string varname = fn.substr(vn,es-1-vn);
								ns.unsetVariable(varname);
							}
						}
					}
				}
			}
			else
#endif /* FIM_WANT_PIC_LVDN */
			if(std::getline(ls,fn,sc))
			{
				bool nec = (std::getline(ls,ds,nl)); // non empty commentary
				const bool aoec = true; // (propagate i:variables) also on empty commentary

				if(nec || aoec)
				{
					const bool want_basename = true; /*  */
#if FIM_WANT_PIC_RCMT
					{
						size_t csi = ds.find_first_of("#!fim:",0);
						size_t csil = 6;

						if( csi != 0 )
							ld = ds; // cache new description
						else
						{
							// use last (cached) description
							char sc = ds[csil];

							switch(sc)
							{
								case('='): // #!fim:=
									ds = ld;
								break;
								case('+'): // #!fim:+
									ds = ld + ds.substr(csil+1);
								break;
								case('^'): // #!fim:^
									ds = ds.substr(csil+1) + ld;
								break;
								case('s'): // #!fim:s/from/to '/' not allowed in from or to
								{
									fim::string es = ds.substr(csil);
									size_t m = ((es).re_match("s/[^/]+/[^/]+"));
									if(m)
									{
										size_t n = es.find("/",2);
										std::string fs = es.substr(2,n-2), ts = es.substr(n+1);
										fim::string fds = ld;
										fds.substitute(fs.c_str(),ts.c_str());
										ds = fds.c_str();
									}
								}
								break;
							}
						}
					}
#endif /* FIM_WANT_PIC_RCMT */
					if(! want_basename )
					{
						(*this)[fn]=ds;
#if FIM_WANT_PIC_LVDN
						vd_[std::string(fn)]=ns;
#endif /* FIM_WANT_PIC_LVDN */
					}
					else
					{
						(*this)[fim_basename_of(fn.c_str())]=ds;
#if FIM_WANT_PIC_LVDN
						vd_[fim_basename_of(fn.c_str())]=ns;
#endif /* FIM_WANT_PIC_LVDN */
					}
				}
			}
		}
		reset();
	}
	key_type fk(const mapped_type & sk) 
	{
		mapped_type v;
		if ( fo(sk,li_) != end() )
		{
			v = fi(sk)->first;
		}
		else
		{
			reset();
			if ( fo(sk,li_) != end() )
				v = fi(sk)->first;
		}
		return v;
	}
	size_t byte_size(void)const
	{
		size_t bs = size() + sizeof(*this);
		for( const_iterator it = begin();it != end(); ++it )
			bs += it->first.size() + sizeof(it->first),
			bs += it->second.size() + sizeof(it->second);
		return bs;
	}
	std::ostream& print(std::ostream &os)const
	{
		os << (size()) << " entries in " << byte_size() << " bytes";
		return os;
	}
};
	std::ostream& operator<<(std::ostream &os, const ImgDscs & id);
#endif /* FIM_WANT_PIC_CMTS */
#endif /* FIM_IMAGE_H */
