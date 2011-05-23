/* $LastChangedDate$ */
/*
 SDLDevice.h : sdllib device Fim driver header file

 (c) 2008-2011 Michele Martone

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
#ifndef FIM_SDLDEVICE_H
#define FIM_SDLDEVICE_H
#ifdef FIM_WITH_LIBSDL

#include "DisplayDevice.h"
#include <SDL.h>

class SDLDevice:public DisplayDevice 
{
	private:

	SDL_Surface *screen_;
	SDL_Event event_;
	const SDL_VideoInfo* vi_;

	int keypress_ ;
	int h_;

	int current_w_;
	int current_h_;
	int Bpp_,bpp_;
	fim::string opts_;
	bool want_windowed_;
	bool want_mouse_display_;

	public:

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	SDLDevice(MiniConsole & mc_,
#else
	SDLDevice(
#endif
			fim::string opts
		);

	virtual fim_err_t  display(
		void *ida_image_img, // source image structure (struct ida_image *)(but we refuse to include header files here!)
		//void* rgb,// destination gray array and source rgb array
		fim_coo_t iroff,fim_coo_t icoff, // row and column offset of the first input pixel
		fim_coo_t irows,fim_coo_t icols,// rows and columns in the input image
		fim_coo_t icskip,	// input columns to skip for each line
		fim_coo_t oroff,fim_coo_t ocoff,// row and column offset of the first output pixel
		fim_coo_t orows,fim_coo_t ocols,// rows and columns to draw in output buffer
		fim_coo_t ocskip,// output columns to skip for each line
		fim_flags_t flags// some flags
		);

	int initialize(key_bindings_t &key_bindings);
	void finalize() ;

	int get_chars_per_line() ;
	int get_chars_per_column();
	int width();
	int height();
	fim_err_t status_line(const fim_char_t *msg);
	fim_bool_t handle_console_switch(){return false;}
	fim_err_t clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2);
	int get_input(fim_key_t * c);
	virtual fim_key_t catchInteractiveCommand(fim_ts_t seconds)const;
	void fs_render_fb(int x, int y, FSXCharInfo *charInfo, unsigned char *data);
	fim_err_t fs_puts(struct fs_font *f_, fim_coo_t x, fim_coo_t y, const fim_char_t *str);
	void flush();
	void lock();
	void unlock();
	fim_bpp_t get_bpp(){return bpp_; };
	bool sdl_window_update();
	private:
	int clear_rect_( void* dst, int oroff,int ocoff, int orows,int ocols, int ocskip);
	/* TEMPORARY */
	inline void setpixel(SDL_Surface *screen_, int x, int y, Uint8 r, Uint8 g, Uint8 b);
	void status_screen_(int desc,int draw_output){ return ; }
	int fill_rect(int x1, int x2, int y1,int y2, int color);
	int txt_width() ;
	int txt_height() ;
};


#endif
#endif
