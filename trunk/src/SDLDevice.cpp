/* $LastChangedDate$ */
/*
 SDLDevice.cpp : sdllib device Fim driver file

 (c) 2008-2011 Michele Martone
 based on code (c) 1998-2006 Gerd Knorr <kraxel@bytesex.org>

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

/*
 * NOTES : The SDL support is INCOMPLETE:
 *
 *  - largely inefficient, please do not be surprised
 *  - input problems when coupled with readline
 */
#include "fim.h"

#ifdef FIM_WITH_LIBSDL

#include "SDLDevice.h"

#define FIM_SDL_ICONPATH ""

namespace fim
{
	extern CommandConsole cc;
}

#define FIM_SDL_MINWIDTH 2
#define FIM_SDL_MINHEIGHT 2

#define FIM_SDL_ALLOW_QUIT 1
#define FIM_SDL_WANT_KEYREPEAT 1
#define FIM_SDL_WANT_RESIZE 1
#define FIM_SDL_DEBUG 1
#undef FIM_SDL_DEBUG

#ifdef FIM_SDL_DEBUG
#define FIM_SDL_INPUT_DEBUG(C,MSG)  \
/* i miss sooo much printf() :'( */ \
std::cout << (size_t)getmilliseconds() << " : "<<MSG<<" : "; \
std::cout.setf ( std::ios::hex, std::ios::basefield ); \
std::cout.setf ( std::ios::showbase ); \
std::cout << *(int*)(C) <<"\n"; \
std::cout.unsetf ( std::ios::showbase ); \
std::cout.unsetf ( std::ios::hex );
#else
#define FIM_SDL_INPUT_DEBUG(C,MSG) {}
#endif

	/* WARNING : TEMPORARY, FOR DEVELOPEMENT PURPOSES */


fim_err_t SDLDevice::parse_optstring(const fim_char_t *os)
{
	bool want_windowed=want_windowed_;
	bool want_mouse_display=want_mouse_display_;
	bool want_resize=want_resize_;
	fim_coo_t current_w=current_w_;
	fim_coo_t current_h=current_h_;

		if(os)
		{
			while(*os&&!isdigit(*os))
			{
				bool tv=true;
				if(isupper(*os))
					tv=false;
				switch(tolower(*os)){
					case 'w': want_windowed=tv; break;
					case 'm': want_mouse_display=tv; break;
					case 'r': want_resize=tv; break;
					default: std::cerr<<"unrecognized specifier character \""<<*os<<"\"\n";goto err;
				}
				++os;
			}
		if(*os)
		if(2==sscanf(os,"%d:%d",&current_w,&current_h))

		{
		//	std::cout << w << " : "<< h<<"\n";
			current_w=FIM_MAX(current_w,0);
			current_h=FIM_MAX(current_h,0);
			if(!allowed_resolution(current_w,current_h))
				goto err;
		}
		else
		{
			current_w=current_h=0;
			std::cerr << "user specification of resolution (\""<<os<<"\") wrong: it shall be in \"width:height\" format! \n";
			// TODO: a better invaling string message needed here
		}
		}
		// commit
		want_windowed_=want_windowed;
		want_mouse_display_=want_mouse_display;
#if FIM_SDL_WANT_RESIZE 
		want_resize_=want_resize;
#else
		want_resize_=false;
#endif
		current_w_=current_w;
		current_h_=current_h;
		return FIM_ERR_NO_ERROR;
err:
		return FIM_ERR_GENERIC;
}

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	SDLDevice::SDLDevice(MiniConsole & mc_, fim::string opts):DisplayDevice(mc_),
#else
	SDLDevice::SDLDevice(
			fim::string opts
			):DisplayDevice(),
#endif
	current_w_(0), current_h_(0),
	opts_(opts),
	want_windowed_(false),
	want_mouse_display_(false),
	want_resize_(false),
	Bpp_(NULL),
	bpp_(NULL),
	screen_(NULL),
	vi_(NULL)
	{
		FontServer::fb_text_init1(fontname_,&f_);	// FIXME : move this outta here
		keypress_ = 0;
		h_=0;
#if FIM_WANT_SDL_OPTIONS_STRING 
		const char*os=opts_.c_str();
		parse_optstring(os);
#endif
		bzero(&bvi_,sizeof(bvi_));
		//current_w_=current_h_=0;
	}

	int SDLDevice::clear_rect_(
		void* dst,	// destination array 
		int oroff,int ocoff,	// row  and column  offset of the first output pixel
		int orows,int ocols,	// rows and columns drawable in the output buffer
		int ocskip		// output columns to skip for each line
	)
	{
		/* output screen variables */
//		int 
//			oi,// output image row index
//			oj;// output image columns index

		int lor,loc;
    		
		if( oroff <0 ) return -8;
		if( ocoff <0 ) return -9;
		if( orows <=0 ) return -10;
		if( ocols <=0 ) return -11;
		if( ocskip<0 ) return -12;

		if( oroff>orows ) return -8-10*100;
		if( ocoff>ocols ) return -9-11*100;

		if( ocskip<ocols ) return -12-11*100;

		/*
		 * orows and ocols is the total number of rows and columns in the output window.
		 * no more than orows-oroff rows and ocols-ocoff columns will be rendered, however
		 * */

		lor = orows-1;
		loc = ocols-1;
		
		return  -1;
	}

	fim_err_t SDLDevice::display(
		//struct ida_image *img, // source image structure
		void *ida_image_img, // source image structure
		//void* rgb,// source rgb array
		fim_coo_t iroff,fim_coo_t icoff, // row and column offset of the first input pixel
		fim_coo_t irows,fim_coo_t icols,// rows and columns in the input image
		fim_coo_t icskip,	// input columns to skip for each line
		fim_coo_t oroff,fim_coo_t ocoff,// row and column offset of the first output pixel
		fim_coo_t orows,fim_coo_t ocols,// rows and columns to draw in output buffer
		fim_coo_t ocskip,// output columns to skip for each line
		fim_flags_t flags// some flags
	)
	{
		
		/*
		 * TODO : generalize this routine and put in common.cpp
		 * */
		/*
		 * FIXME : centering mechanisms missing here; an intermediate function
		 * shareable with FramebufferDevice would be nice, if implemented in AADevice.
		 * */
		//was : void
		unsigned char* rgb = ida_image_img?((struct ida_image*)ida_image_img)->data:NULL;// source rgb array
		if ( !rgb ) return -1;
	
		if( iroff <0 ) return -2;
		if( icoff <0 ) return -3;
		if( irows <=0 ) return -4;
		if( icols <=0 ) return -5;
		if( icskip<0 ) return -6;
		if( oroff <0 ) return -7;
		if( ocoff <0 ) return -8;
		if( orows <=0 ) return -9;
		if( ocols <=0 ) return -10;
		if( ocskip<0 ) return -11;
		if( flags <0 ) return -12;

		if( iroff>irows ) return -2-3*100 ;
		if( icoff>icols ) return -3-5*100;
//		if( oroff>orows ) return -7-9*100;//EXP
//		if( ocoff>ocols ) return -8-10*100;//EXP
		if( oroff>height() ) return -7-9*100;//EXP
		if( ocoff>width()) return -8-10*100;//EXP

		if( icskip<icols ) return -6-5*100;
		if( ocskip<ocols ) return -11-10*100;
	
		orows  = FIM_MIN( orows, height());
		ocols  = FIM_MIN( ocols,  width()); 
		ocskip = width(); 	//FIXME maybe this is not enough and should be commented or rewritten!

		if( orows  > height() ) return -9 -99*100;
		if( ocols  >  width() ) return -10-99*100;
		if( ocskip <  width() ) return -11-99*100;
		if( icskip<icols ) return -6-5*100;

		ocskip = width();// output columns to skip for each line

		if(icols<ocols) { ocoff+=(ocols-icols-1)/2; ocols-=(ocols-icols-1)/2; }
		if(irows<orows) { oroff+=(orows-irows-1)/2; orows-=(orows-irows-1)/2; }


//		int h_=1;
//		int x, y;
		int ytimesw;

		if(SDL_MUSTLOCK(screen_))
		{
			if(SDL_LockSurface(screen_) < 0) return -1;
		}

		int idr,idc,lor,loc;

		idr = iroff-oroff;
		idc = icoff-ocoff;

		lor = (FIM_MIN(orows-1,irows-1-iroff+oroff));
		loc = (FIM_MIN(ocols-1,icols-1-icoff+ocoff));

		int ii,ij;
		int oi,oj;
		int mirror=flags&FIM_FLAG_MIRROR, flip=flags&FIM_FLAG_FLIP;//STILL UNUSED : FIXME
		unsigned char * srcp;

		// FIXME : temporary
//		clear_rect(  ocoff, ocoff+ocols, oroff, oroff+orows); 
//		clear_rect(  0, ocols, 0, orows); 
		clear_rect(  0, width()-1, 0, height()-1); 

		if(!mirror && !flip)
		for(oi=oroff;FIM_LIKELY(oi<lor);++oi)
		for(oj=ocoff;FIM_LIKELY(oj<loc);++oj)
		{
			ytimesw = (oi)*screen_->pitch/Bpp_;

			ii    = oi + idr;
			ij    = oj + idc;
			srcp  = ((unsigned char*)rgb)+(3*(ii*icskip+ij));

			setpixel(screen_, oj, ytimesw, (int)srcp[0], (int)srcp[1], (int)srcp[2]);
		}
		else
		for(oi=oroff;FIM_LIKELY(oi<lor);++oi)
		for(oj=ocoff;FIM_LIKELY(oj<loc);++oj)
		{

			ytimesw = (oi)*screen_->pitch/Bpp_;
			/* UNFINISHED : FIX ME */
			ii    = oi + idr;
			ij    = oj + idc;
			
			if(mirror)ij=((icols-1)-ij);
			if( flip )ii=((irows-1)-ii);
			srcp  = ((unsigned char*)rgb)+(3*(ii*icskip+ij));

			setpixel(screen_, oj, ytimesw, (int)srcp[0], (int)srcp[1], (int)srcp[2]);
		}

		if(SDL_MUSTLOCK(screen_)) SDL_UnlockSurface(screen_);

		SDL_Flip(screen_);

		return FIM_ERR_NO_ERROR;
	}

	bool SDLDevice::sdl_window_update()
	{
		vi_ = SDL_GetVideoInfo();
		if(!vi_)
			return false;
		current_w_=vi_->current_w;
		current_h_=vi_->current_h;
		bpp_      =vi_->vfmt->BitsPerPixel;
		Bpp_      =vi_->vfmt->BytesPerPixel;
		return true;
	}

	fim_err_t SDLDevice::initialize(sym_keys_t &sym_keys)
	{
		/*
		 *
		 * */
		//int want_width=0, want_height=0, want_bpp=0;
		int want_width=current_w_, want_height=current_h_, want_bpp=0;
		int want_flags=SDL_FULLSCREEN|SDL_HWSURFACE;
		int delay=0,interval=0;
		//want_flags|=SDL_NOFRAME;
		//std::cout << want_width << " : "<< want_height<<"\n";
#if 0
		//want_windowed_=true;
		want_height=480;
		want_width=480;
#endif
#if FIM_SDL_WANT_RESIZE 
		if(want_resize_)
			want_flags|=SDL_RESIZABLE;
#endif
		if(want_windowed_)
			want_flags&=~SDL_FULLSCREEN;
		//want_flags|=SDL_DOUBLEBUF;
		
		if(!allowed_resolution(want_width,want_height))
		{
			std::cout << "requested window size ("<<want_width<<":"<<want_height<<") smaller than the smallest allowed.\n";
			return FIM_ERR_GENERIC;
		}

		if (SDL_Init(SDL_INIT_VIDEO) < 0 )
		{
			std::cout << "problems initializing SDL (SDL_Init)\n";
			return 1;
		}
		{
			const SDL_VideoInfo*bvip=SDL_GetVideoInfo();
			if(bvip)bvi_=*bvip;
		}
		fim_perror(NULL);
		
		if(FIM_SDL_WANT_KEYREPEAT)
		{
		//	std::cout<<"interval:"<<interval<<"\n"; std::cout<<"delay :"<<delay <<"\n";
			delay=SDL_DEFAULT_REPEAT_DELAY;
			interval=SDL_DEFAULT_REPEAT_INTERVAL;
			if(SDL_EnableKeyRepeat(delay,interval)<0)
			{
			}
			else
			{
				SDL_GetKeyRepeat(&delay,&interval);
		//		std::cout<<"interval:"<<interval<<"\n"; std::cout<<"delay :"<<delay <<"\n";
			}
			fim_perror(NULL);
		}

		if(resize(want_width,want_height))
		{
			std::cout << "problems initializing SDL (SDL_SetVideoMode)\n";
			SDL_Quit();
			return FIM_ERR_GENERIC;
		}
		fim_perror(NULL);

		/* Enable Unicode translation ( for a more flexible input handling ) */
	        SDL_EnableUNICODE( 1 );
		SDL_WM_SetCaption(FIM_CNS_FIM_APPTITLE,FIM_SDL_ICONPATH);
		fim_perror(NULL);

		sym_keys["PageUp" ]=SDLK_PAGEUP;
		sym_keys["PageDown" ]=SDLK_PAGEDOWN;
		sym_keys["Left" ]=SDLK_LEFT;
		sym_keys["Right"]=SDLK_RIGHT;
		sym_keys["Up"   ]=SDLK_UP;
		sym_keys["Down" ]=SDLK_DOWN;
		sym_keys["Space"]=SDLK_SPACE;
		sym_keys["End"  ]=SDLK_END;
		sym_keys["Home" ]=SDLK_HOME;
		sym_keys["F1" ]=SDLK_F1;
		sym_keys["F2" ]=SDLK_F2;
		sym_keys["F3" ]=SDLK_F3;
		sym_keys["F4" ]=SDLK_F4;
		sym_keys["F5" ]=SDLK_F5;
		sym_keys["F6" ]=SDLK_F6;
		sym_keys["F7" ]=SDLK_F7;
		sym_keys["F8" ]=SDLK_F8;
		sym_keys["F9" ]=SDLK_F9;
		sym_keys["F10"]=SDLK_F10;
		sym_keys["F11"]=SDLK_F11;
		sym_keys["F12"]=SDLK_F12;
		
		SDL_ShowCursor(want_mouse_display_?1:0);
		fim_perror(NULL);

		// textual console reformatting
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
		mc_.setGlobalVariable(FIM_VID_CONSOLE_ROWS,height()/(2*f_->height));
		mc_.reformat(    width() /    f_->width   );
#endif
		return FIM_ERR_NO_ERROR;
	}

	void SDLDevice::finalize()
	{
		finalized_=true;
		SDL_Quit();
	}

	int SDLDevice::get_chars_per_column()
	{
		return height() / f_->height;
	}

	int SDLDevice::get_chars_per_line()
	{
		return width() / f_->width;
	}

	int SDLDevice::width()
	{
		return current_w_;
	}

	int SDLDevice::height()
	{
		return current_h_;
	}

	inline void SDLDevice::setpixel(SDL_Surface *screen_, int x, int y, Uint8 r, Uint8 g, Uint8 b)
	{
		/*
		 * this function is taken straight from the sdl documentation: there are smarter ways to do this.
		 * */

		switch (bpp_)
		{
		case  8:
		{
			Uint8 *pixmem8;
			Uint8 colour;
			colour = SDL_MapRGB( screen_->format, b, g, r );
			pixmem8 = (Uint8*)((char*)( screen_->pixels)  + (y + x)*Bpp_);
			*pixmem8 = colour;
		}
		break;
		case 15:
		case 16:
		{
			Uint16 *pixmem16;
			Uint16 colour;
			colour = SDL_MapRGB( screen_->format, b, g, r );
			pixmem16 = (Uint16*)((char*)( screen_->pixels)  + (y + x)*Bpp_);
			*pixmem16 = colour;
		}
		break;
		case 24:
		{
			Uint32 *pixmem32;
			Uint32 colour;
			colour = SDL_MapRGB( screen_->format, b, g, r );
			pixmem32 = (Uint32*)((char*)( screen_->pixels)  + (y + x)*Bpp_);
			*pixmem32 = colour;
		}
		break;
		case 32:
		{
			Uint32 *pixmem32;
			Uint32 colour;
			colour = SDL_MapRGBA( screen_->format, b, g, r, 255 );
			pixmem32 = (Uint32*)((char*)( screen_->pixels)  + (y + x)*Bpp_);
			*pixmem32 = colour;
		}
		break;
		default:
		{
			/* 1,2,4 modes unsupported for NOW */
		}
		}

	}

	static int get_input_inner(fim_key_t * c, SDL_Event*eventp, int *keypressp, bool want_poll)
	{
//		int keypress_=0;
		bool ctrl_on=0;
		bool alt_on=0;
		bool shift_on=0;
		int ret=0;
		SDL_Event event=*eventp;
		*c = 0x0;	/* blank */

//		while(SDL_PollEvent(&event))
		if(want_poll)
			ret=SDL_PollEvent(&event);
		else
			ret=SDL_WaitEvent(&event);
		if(ret)
		{
			if(event.type==SDL_KEYUP)
				if(!SDL_PollEvent(&event))
					goto done;

			switch (event.type)
			{
#if FIM_SDL_WANT_RESIZE 
				case SDL_VIDEORESIZE:
						cc.resize(event.resize.w,event.resize.h);
				break;
#endif
				case SDL_QUIT:
#if FIM_SDL_ALLOW_QUIT
				*c=cc.find_keycode_for_bound_cmd(FIM_FLT_QUIT);
				return 1;
				//cc.quit();
#endif	
				*keypressp = 1;
				
				break;
				case SDL_KEYDOWN:

				if(event.key.keysym.mod == KMOD_RCTRL || event.key.keysym.mod == KMOD_LCTRL ) ctrl_on=true;
				if(event.key.keysym.mod == KMOD_RALT  || event.key.keysym.mod == KMOD_LALT  )  alt_on=true;
				if(event.key.keysym.mod == KMOD_RSHIFT  || event.key.keysym.mod == KMOD_LSHIFT  )  shift_on=true;

			//	std::cout << "sym : " << (int)event.key.keysym.sym << "\n" ;
			//	std::cout << "uni : " << (int)event.key.keysym.unicode<< "\n" ;
			//	if(shift_on)std::cout << "shift_on\n";

				if( event.key.keysym.unicode == 0x0 )
				{
					/* arrows and stuff */
					FIM_SDL_INPUT_DEBUG(c,"no unicode");
					if(event.key.keysym.sym<256)
					{
						FIM_SDL_INPUT_DEBUG(c,"uhm");
						*c=event.key.keysym.sym;
						return 1;
					}
					else
					if(event.key.keysym.sym>=SDLK_F1 && event.key.keysym.sym<=SDLK_F12)
					{
						FIM_SDL_INPUT_DEBUG(c,"FXX?");
						*c=event.key.keysym.sym;
						return 1;
					}
					else
					if(
						event.key.keysym.sym!=SDLK_LSHIFT
					&&	event.key.keysym.sym!=SDLK_RSHIFT
					&&	event.key.keysym.sym!=SDLK_LALT
					&&	event.key.keysym.sym!=SDLK_RALT
					&&	event.key.keysym.sym!=SDLK_LCTRL
					&&	event.key.keysym.sym!=SDLK_RCTRL
					)
					{
						/* arrows.. .. */
						*c=event.key.keysym.sym;
						FIM_SDL_INPUT_DEBUG(c,"arrow");
						return 1;
					}
					else
					{
						FIM_SDL_INPUT_DEBUG(c,"shift");
						/* we ignore lone shift or alt .. */
						return 0;
					}
				}

				if(alt_on)
				{
					*c=(unsigned char)event.key.keysym.unicode;
					*c|=(1<<31);	/* FIXME : a dirty trick */
					return 1;
				}

				if( 	event.key.keysym.unicode < 0x80)
				{
					/* 
					 * SDL documentation 1.2.12 about unicode:
					 * It is useful to note that unicode values < 0x80 translate directly
					 * a characters ASCII value.
					 * */
			//		if(event.key.keysym.mod == KMOD_RCTRL || event.key.keysym.mod == KMOD_LCTRL ) std::cout << "ctrl ! \n";
					*c=(unsigned char)event.key.keysym.unicode;

					if(ctrl_on)
					{
						// if(*c-1+'a'=='c')std::exit(-1);//works 
						if(*c-1+'a'<='z')
						{
							//std::cout << "with control : " << *c+'a'-1 << "\n";
						}
						else
						{
							/* other chars */
							*c-='a';
							*c+= 1 ;
						}
					}
					if(*c)	/* !iscntrl(c) */
					{
						/* the usual chars */
						FIM_SDL_INPUT_DEBUG(c,"keysym");
						return 1;
					}
					else	/*  iscntrl(c) */
					{
						FIM_SDL_INPUT_DEBUG(c,"iscntrl");
						return 0;
					}
					/*
					 * p.s.: note that we get 0 in some cases (e.g.: KMOD_RSHIFT, ...).
					 * */
				}
				else
				{
					FIM_SDL_INPUT_DEBUG(c,"wchar");
					cout << "sorry, no support for wide chars in fim\n";
					/*  no support for wide chars in fim */
					return 0;
				}
				FIM_SDL_INPUT_DEBUG(c,"unknown");
				return 0;

				break;
#if FIM_WANT_SDL_PROOF_OF_CONCEPT_MOUSE_SUPPORT
				//case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
				//case SDL_MOUSEBUTTONUP:
				{
					int x,y;
					Uint8 ms=SDL_GetMouseState(&x,&y);
#if 0
					cout << "mouse clicked at "<<x<<" "<<y<<" : "<< ((x>this->width()/2)?'r':'l') <<"; state: "<<ms<<"\n";
					if(ms&SDL_BUTTON_RMASK) cout << "rmask\n";
					if(ms&SDL_BUTTON_LMASK) cout << "lmask\n";
					if(ms&SDL_BUTTON_MMASK) cout << "mmask\n";
					if(ms&SDL_BUTTON_X1MASK) cout << "x1mask\n";
					if(ms&SDL_BUTTON_X2MASK) cout << "x2mask\n";
#endif
					if(!cc.inConsole())
					{
						if(ms&SDL_BUTTON_LMASK) { *c='n'; return 1; }
						if(ms&SDL_BUTTON_RMASK) { *c='b'; return 1; }
						if(ms&SDL_BUTTON_MMASK) { *c='q'; return 1; }
					}
				}
				break;
#endif
				case SDL_KEYUP:
				return 0;
				default:
				FIM_SDL_INPUT_DEBUG(c,"default-unknown");
			}
			return 0;
		}
done:
		FIM_SDL_INPUT_DEBUG(c,"no key");
		return 0;
	}

	int SDLDevice::get_input(fim_key_t * c, bool want_poll)
	{
		return get_input_inner(c,&event_,&keypress_,want_poll);
	}

	int SDLDevice::fill_rect(int x1, int x2, int y1,int y2, int color)
	{
		int y;
		/*
		 * This could be optimized
		 * */
		for(y=y1;y<y2;++y)
		{
			memset(((char*)(screen_->pixels)) + y*screen_->pitch + x1*Bpp_,color, (x2-x1)* Bpp_);
		}
		return 0;
	}

	fim_err_t SDLDevice::clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2)
	{
		int y;
		/*
		 * This could be optimized
		 * */
		for(y=y1;y<=y2;++y)
		{
			bzero(((char*)(screen_->pixels)) + y*screen_->pitch + x1*Bpp_, (x2-x1+1)* Bpp_);
		}
		return 0;
	}

void SDLDevice::fs_render_fb(int x_, int y, FSXCharInfo *charInfo, unsigned char *data)
{

/* 
 * These preprocessor macros should serve *only* for font handling purposes.
 * */
#define BIT_ORDER       BitmapFormatBitOrderMSB
#ifdef BYTE_ORDER
#undef BYTE_ORDER
#endif
#define BYTE_ORDER      BitmapFormatByteOrderMSB
#define SCANLINE_UNIT   BitmapFormatScanlineUnit8
#define SCANLINE_PAD    BitmapFormatScanlinePad8
#define EXTENTS         BitmapFormatImageRectMin

#define SCANLINE_PAD_BYTES 1
#define GLWIDTHBYTESPADDED(bits, nBytes)                                    \
        ((nBytes) == 1 ? (((bits)  +  7) >> 3)          /* pad to 1 byte  */\
        :(nBytes) == 2 ? ((((bits) + 15) >> 3) & ~1)    /* pad to 2 bytes */\
        :(nBytes) == 4 ? ((((bits) + 31) >> 3) & ~3)    /* pad to 4 bytes */\
        :(nBytes) == 8 ? ((((bits) + 63) >> 3) & ~7)    /* pad to 8 bytes */\
        : 0)

	int row,bit,bpr,x;

	bpr = GLWIDTHBYTESPADDED((charInfo->right - charInfo->left), SCANLINE_PAD_BYTES);
	for (row = 0; row < (charInfo->ascent + charInfo->descent); row++)
	{
		for (x = 0, bit = 0; bit < (charInfo->right - charInfo->left); bit++) 
		{
			if (data[bit>>3] & fs_masktab[bit&7])
			{	// WARNING !
				setpixel(screen_,x_+x,(y+row)*screen_->pitch/Bpp_,(Uint8)0xff,(Uint8)0xff,(Uint8)0xff);
			}
			x += Bpp_/Bpp_;/* FIXME */
		}
		data += bpr;
	}

#undef BIT_ORDER
#undef BYTE_ORDER
#undef SCANLINE_UNIT
#undef SCANLINE_PAD
#undef EXTENTS
#undef SCANLINE_PAD_BYTES
#undef GLWIDTHBYTESPADDED
}

fim_err_t SDLDevice::fs_puts(struct fs_font *f_, fim_coo_t x, fim_coo_t y, const fim_char_t *str)
{
    int i,c/*,j,w*/;

    for (i = 0; str[i] != '\0'; i++) {
	c = (unsigned char)str[i];
	if (NULL == f_->eindex[c])
	    continue;
	/* clear with bg color */
//	w = (f_->eindex[c]->width+1)*Bpp_;
#if 0
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (j = 0; j < f_->height; j++) {
/////	    memset_combine(start,0x20,w);
	    memset(start,0,w);
	    start += fb_fix.line_length;
	}
#else
	//sometimes we can gather multiple calls..
	if(fb_fix.line_length==(unsigned int)w)
	{
		//contiguous case
		memset(start,0,w*f_->height);
	    	start += fb_fix.line_length*f_->height;
	}
	else
	for (j = 0; j < f_->height; j++) {
	    memset(start,0,w);
	    start += fb_fix.line_length;
	}
#endif
#endif
	/* draw char */
	//fs_render_fb(fb_fix.line_length,f_->eindex[c],f_->gindex[c]);
	fs_render_fb(x,y,f_->eindex[c],f_->gindex[c]);
	x += f_->eindex[c]->width;
	/* FIXME : SLOW ! */
	if ((int)x > width() - f_->width)
		goto err;
    }
    // FIXME
//	return x;
	return FIM_ERR_NO_ERROR;
err:
	return FIM_ERR_GENERIC;
}

	fim_err_t SDLDevice::status_line(const fim_char_t *msg)
	{
		if(SDL_MUSTLOCK(screen_))
		{
			if(SDL_LockSurface(screen_) < 0) return FIM_ERR_GENERIC;
		}

		int y;
		int ys=3;// FIXME

		if(get_chars_per_column()<1)
			goto done;
		y = height() - f_->height - ys;
		if(y<0 )
			goto done;
		clear_rect(0, width()-1, y+1,y+f_->height+ys-1);
		fs_puts(f_, 0, y+ys, msg);
		fill_rect(0,width()-1, y, y+1, 0xFF);	// FIXME : NO 1!

		if(SDL_MUSTLOCK(screen_)) SDL_UnlockSurface(screen_);
		SDL_Flip(screen_);
done:
		return FIM_ERR_NO_ERROR;
	}

	fim_key_t SDLDevice::catchInteractiveCommand(fim_ts_t seconds)const
	{
		/*
		 * Whether there is some input in the input queue.
		 * Note that in this way the event_ will be lost.
		 * */
		fim_key_t c=0;
		SDL_Event levent;
		int lkeypress=0;
		fim_tms_t sms=10,ms=seconds*1000;// sms: sleep ms
#if 0
		for(;seconds>0;--seconds)
			sleep(1);
		if(!get_input_inner(&c,&levent,&lkeypress))
		{
			//std::cout << "input:" << c << "\n";
			return c;
		}
		else
			return -1;
#else
		do
		{
			if(ms>0)
				usleep((int)(sms*1000)),ms-=sms;
			// we read input twice: it seems we have a number of "spurious" inputs. 
			if(1==get_input_inner(&c,&levent,&lkeypress,true)) goto done;
		}
		while(ms>0);
		return -1;
done:
		usleep((int)(sms*1000)),ms-=sms;
		return c;
#endif
	}

	void SDLDevice::flush()
	{
	}

	void SDLDevice::lock()
	{
		if(SDL_MUSTLOCK(screen_))
		{
			if(SDL_LockSurface(screen_) < 0) return;
		}
	}

	void SDLDevice::unlock()
	{
		if(SDL_MUSTLOCK(screen_)) SDL_UnlockSurface(screen_);
		SDL_Flip(screen_);
	}

	bool SDLDevice::allowed_resolution(fim_coo_t w, fim_coo_t h)
	{
		if(w==0 || h==0)
			goto ok;
		if(w<FIM_SDL_MINWIDTH || h<FIM_SDL_MINHEIGHT)
			return false;
		if(w<f_->width || h<f_->height)
			return false;
ok:
		return true;
	}

	fim_err_t SDLDevice::resize(fim_coo_t w, fim_coo_t h)
	{
		SDL_Surface *nscreen_=NULL;
		int want_flags=screen_?screen_->flags:SDL_FULLSCREEN|SDL_HWSURFACE;

		if(want_resize_)
			want_flags|=SDL_RESIZABLE;
		else
			want_flags&=~SDL_RESIZABLE;

		if(want_windowed_)
			want_flags&=~SDL_FULLSCREEN;
		else
			want_flags|=SDL_FULLSCREEN;

		if(!allowed_resolution(w,h))
			return FIM_ERR_GENERIC;

		if(w==0 && h==0)
			w=bvi_.current_w, h=bvi_.current_h; // best video mode, as suggested by SDL
		//std::cout << "resizing to " << w << " "<< h << "\n";
		if (NULL==(nscreen_ = SDL_SetVideoMode(w, h, bpp_, want_flags)))
		{
			///std::cout << "resizing to " << w << " "<< h << " FAILED!\n";
			return FIM_ERR_GENERIC;
		}
		//std::cout << "resizing to " << w << " "<< h << " SUCCESS!\n";
		screen_=nscreen_;
		if(!sdl_window_update())
		{
			std::cout << "problems initializing SDL (SDL_GetVideoInfo)\n";
			return FIM_ERR_GENERIC;
		}
		return FIM_ERR_NO_ERROR;
	}

	fim_err_t SDLDevice::reinit(const fim_char_t *rs)
	{
#if FIM_SDL_WANT_RESIZE 
#else
		cout << "reinit not allowed\n";
		goto err;
#endif
		// FIXME: a wrong command string shall be ignored!
		if(parse_optstring(rs)!=FIM_ERR_NO_ERROR)
			goto err;
		return cc.resize(current_w_,current_h_);
	err:
		//std::cerr<<"problems!\n";
		return FIM_ERR_GENERIC;
	}
#endif
