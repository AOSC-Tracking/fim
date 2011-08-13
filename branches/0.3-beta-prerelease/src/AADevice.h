/* $LastChangedDate: 2011-06-21 12:32:15 +0200 (Tue, 21 Jun 2011) $ */
/*
 AADevice.h : aalib device Fim driver header file

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
#ifndef FIM_AADEVICE_H
#define FIM_AADEVICE_H
#ifdef FIM_WITH_AALIB

#include "DisplayDevice.h"
#include <aalib.h>

/*
 * Debugging only!
 * */
#define FIM_AALIB_DRIVER_DEBUG 0

class AADevice:public DisplayDevice 
{
	private:
	aa_context *ascii_context_;
	//struct aa_renderparams *ascii_rndparms;//we rely on aa_defrenderparams
	struct aa_hardware_params ascii_hwparms_;
	struct aa_savedata ascii_save_;
	fim_char_t name_[2];	/* FIXME */
	public:
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	AADevice(MiniConsole & mc_):DisplayDevice(mc_){}
#else
	AADevice():DisplayDevice(){}
#endif
	virtual ~AADevice();

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
	int initialize(sym_keys_t &sym_keys);
	void finalize();

	int get_chars_per_line();
	int get_chars_per_column();
	int txt_width();
	int txt_height();
	int width();
	int height();
	fim_err_t status_line(const fim_char_t *msg);
	void status_screen(int desc,int draw_output){}
	fim_bool_t handle_console_switch(){return false;}
	int clear_rect_(
		void* dst,
		int oroff,int ocoff,
		int orows,int ocols,
		int ocskip);

	fim_err_t clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1, fim_coo_t y2);
	fim_err_t fs_puts(struct fs_font *f, fim_coo_t x, fim_coo_t y, const fim_char_t *str);
	void flush();
	fim_err_t init_console();
	fim_bpp_t get_bpp(){return 1; /* :) */ };
	int get_input(fim_key_t * c, bool want_poll=false);
	fim_err_t resize(fim_coo_t w, fim_coo_t h);
};


#endif
#endif
