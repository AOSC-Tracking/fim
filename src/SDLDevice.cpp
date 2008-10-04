/* $Id$ */
/*
 SDLDevice.cpp : sdllib device Fim driver file

 (c) 2008 Michele Martone

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

#ifdef FIM_WITH_LIBSDL

#include "SDLDevice.h"

#define min(x,y) ((x)<(y)?(x):(y))


	/* WARNING : TEMPORARY, FOR DEVELOPEMENT PURPOSES */
#define BPP 4
#define DEPTH 32

	SDLDevice::SDLDevice()
	{
		keypress = 0;
		h=0;
	}

	int SDLDevice::clear_rect_(
		void* dst,	// destination array 
		int oroff,int ocoff,	// row  and column  offset of the first output pixel
		int orows,int ocols,	// rows and columns drawable in the output buffer
		int ocskip		// output columns to skip for each line
	)
	{
		/* output screen variables */
		int 
			oi,// output image row index
			oj;// output image columns index

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



	int  SDLDevice::display(
		//struct ida_image *img, // source image structure
		void *ida_image_img, // source image structure
		//void* rgb,// source rgb array
		int iroff,int icoff, // row and column offset of the first input pixel
		int irows,int icols,// rows and columns in the input image
		int icskip,	// input columns to skip for each line
		int oroff,int ocoff,// row and column offset of the first output pixel
		int orows,int ocols,// rows and columns to draw in output buffer
		int ocskip,// output columns to skip for each line
		int flags// some flags
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
	
		orows  = min( orows, height());
		ocols  = min( ocols,  width()); 
		ocskip = width(); 	//FIXME maybe this is not enough and should be commented or rewritten!

		if( orows  > height() ) return -9 -99*100;
		if( ocols  >  width() ) return -10-99*100;
		if( ocskip <  width() ) return -11-99*100;
		if( icskip<icols ) return -6-5*100;

		ocskip = width();// output columns to skip for each line

		if(icols<ocols) { ocoff+=(ocols-icols-1)/2; ocols-=(ocols-icols-1)/2; }
		if(irows<orows) { oroff+=(orows-irows-1)/2; orows-=(orows-irows-1)/2; }


		int h=1;
		int x, y, ytimesw;

		if(SDL_MUSTLOCK(screen))
		{
			if(SDL_LockSurface(screen) < 0) return -1;
		}

		int idr,idc,lor,loc;

		idr = iroff-oroff;
		idc = icoff-ocoff;

		lor = (min(orows-1,irows-1-iroff+oroff));
		loc = (min(ocols-1,icols-1-icoff+ocoff));

		int ii,ij;
		int oi,oj;
		int mirror=flags&FIM_FLAG_MIRROR, flip=flags&FIM_FLAG_FLIP;//STILL UNUSED : FIXME
		unsigned char * srcp;

		// FIXME : temporary
//		clear_rect(  ocoff, ocoff+ocols, oroff, oroff+orows); 
//		clear_rect(  0, ocols, 0, orows); 
		clear_rect(  0, width(), 0, height()); 

		if(!mirror && !flip)
		for(oi=oroff;FIM_LIKELY(oi<lor);++oi)
		for(oj=ocoff;FIM_LIKELY(oj<loc);++oj)
		{
			ytimesw = (oi)*screen->pitch/BPP;

			ii    = oi + idr;
			ij    = oj + idc;
			srcp  = ((unsigned char*)rgb)+(3*(ii*icskip+ij));

			setpixel(screen, oj, ytimesw, (int)srcp[0], (int)srcp[1], (int)srcp[2]);
		}
		else
		for(oi=oroff;FIM_LIKELY(oi<lor);++oi)
		for(oj=ocoff;FIM_LIKELY(oj<loc);++oj)
		{
			/* UNFINISHED : FIX ME */
			ii    = oi + idr;
			ij    = oj + idc;
			
			if(mirror)ij=((icols-1)-ij);
			if( flip )ii=((irows-1)-ii);
			srcp  = ((unsigned char*)rgb)+(3*(ii*icskip+ij));

			setpixel(screen, oj, ytimesw, (int)srcp[0], (int)srcp[1], (int)srcp[2]);
		}

		if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

		SDL_Flip(screen);

		return 0;
	}

	int SDLDevice::initialize()
	{
		/*
		 *
		 * */
		if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
		if (!(screen = SDL_SetVideoMode(width(), height(), DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE)))
		{
			std::cout << "width()  " << width ()<< " .. \n";
			std::cout << "height() " << height()<< " .. \n";
			std::cout << "problems initializing sdl .. \n";
			SDL_Quit();
			return 1;
		}

		return 0;
	}

	void SDLDevice::finalize() { SDL_Quit(); }
	int SDLDevice::get_chars_per_line(){return 10 ;}
	int SDLDevice::width() { return 1024 ;}
	int SDLDevice::height(){ return 768 ;}

	void SDLDevice::setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
	{
		/*
		 * this function is taken straight from the sdl documentation: there are smarter ways to do this.
		 * */
		Uint32 *pixmem32;
		Uint32 colour;

		colour = SDL_MapRGB( screen->format, r, g, b );

		pixmem32 = (Uint32*) screen->pixels  + y + x;
		*pixmem32 = colour;
	}

	int SDLDevice::get_input(int * c )
	{
		int keypress=0;

//		while(SDL_PollEvent(&event))

		if(SDL_PollEvent(&event))
		{
			*c=event.key.keysym.sym;
			return 1;

			switch (event.type)
			{
				case SDL_QUIT:
				keypress = 1;
				break;
				case SDL_KEYDOWN:
				keypress = 1;
				if(event.key.keysym.sym == SDLK_UP )
				keypress = 2;
				break;
			}
		}
		return keypress;
	}


	int SDLDevice::clear_rect(int x1, int x2, int y1,int y2)
	{
		int x,y,ytimesw;

		if(SDL_MUSTLOCK(screen))
		{
			if(SDL_LockSurface(screen) < 0) return -1;
		}

		for(y=y1;y<y2;++y)
		{
			ytimesw = (y)*screen->pitch/BPP;
			for(x=x1;x<x2;++x)
				setpixel(screen, x, ytimesw,0,0,0);

		}
			
		if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

		SDL_Flip(screen);
	}

#endif

