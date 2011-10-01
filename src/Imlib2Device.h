/* $LastChangedDate$ */
/*
 Imlib2Device.h : Imlib2 device Fim driver header file

 (c) 2011-2011 Michele Martone

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
#ifndef FIM_IMLIB2DEVICE_H
#define FIM_IMLIB2DEVICE_H
#ifdef FIM_WITH_LIBIMLIB2

#include "DisplayDevice.h"
#include <X11/Xlib.h>
#include <Imlib2.h>

class Imlib2Device:public DisplayDevice 
{
	private:

	public:

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	Imlib2Device(MiniConsole & mc_,
#else
	Imlib2Device(
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

	fim_err_t initialize(sym_keys_t &sym_keys);
	void finalize() ;

	int get_chars_per_line() ;
	int get_chars_per_column();
	fim_coo_t width();
	fim_coo_t height();
	fim_err_t status_line(const fim_char_t *msg);
	fim_bool_t handle_console_switch(){return false;}
	fim_err_t clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2);
	fim_sys_int get_input(fim_key_t * c, bool want_poll=false);
	virtual fim_key_t catchInteractiveCommand(fim_ts_t seconds)const;
	void fs_render_fb(fim_coo_t x, fim_coo_t y, FSXCharInfo *charInfo, fim_byte_t *data);
	fim_err_t fs_puts(struct fs_font *f_, fim_coo_t x, fim_coo_t y, const fim_char_t *str);
	void flush();
	void lock();
	void unlock();
	fim_bpp_t get_bpp(){return 0; };
	private:
	fim_err_t clear_rect_( void* dst, fim_coo_t oroff,fim_coo_t ocoff,fim_coo_t  orows,fim_coo_t ocols,fim_coo_t  ocskip);
	/* TEMPORARY */
	void status_screen_(int desc,int draw_output){ return ; }
	fim_err_t fill_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2, fim_color_t color);
	fim_coo_t txt_width() ;
	fim_coo_t txt_height() ;
	virtual fim_err_t resize(fim_coo_t w, fim_coo_t h);
	private:
	bool allowed_resolution(fim_coo_t w, fim_coo_t h);
	virtual fim_err_t reinit(const fim_char_t *rs);
	fim_err_t parse_optstring(const fim_char_t *os);
	virtual fim_err_t set_wm_caption(const fim_char_t *msg);
	fim_err_t reset_wm_caption();
};


#endif
#endif
