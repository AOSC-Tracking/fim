/* $LastChangedDate$ */
/*
 DisplayDevice.h : virtual device Fim driver header file

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
#ifndef FIM_DISPLAY_DEVICE_H
#define FIM_DISPLAY_DEVICE_H

#include "DebugConsole.h"

class DisplayDevice
{
	protected:
	fim_bool_t finalized_;
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
#ifndef FIM_KEEP_BROKEN_CONSOLE
	public:
	MiniConsole & mc_;
#endif
#endif
	/*
	 * The generalization of a Fim output device.
	 */
	public:
	struct fs_font *f_;
	const fim_char_t* fontname_;
	fim_bool_t debug_;// really, only for making happy fbdev
	public:
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	DisplayDevice(MiniConsole &mc_);
#else
	DisplayDevice();
#endif
	virtual fim_err_t initialize(sym_keys_t &sym_keys)=0;
	virtual void  finalize()=0;

	virtual fim_err_t display(
		void *ida_image_img, // source image structure
		fim_coo_t iroff,fim_coo_t icoff, // row and column offset of the first input pixel
		fim_coo_t irows,fim_coo_t icols,// rows and columns in the input image
		fim_coo_t icskip,	// input columns to skip for each line
		fim_coo_t oroff,fim_coo_t ocoff,// row and column offset of the first output pixel
		fim_coo_t orows,fim_coo_t ocols,// rows and columns to draw in output buffer
		fim_coo_t ocskip,// output columns to skip for each line
		fim_flags_t flags// some flags
		)=0;

	virtual ~DisplayDevice();

	virtual void flush(){};
	virtual void lock(){}
	virtual void unlock(){}
	virtual int get_chars_per_line()=0;
	virtual int get_chars_per_column()=0;
	virtual int width()=0;
	virtual fim_bpp_t get_bpp()=0;
	virtual int height()=0;
	virtual fim_err_t status_line(const fim_char_t *msg)=0;
	fim_err_t console_control(fim_cc_t code);
	virtual fim_bool_t handle_console_switch()=0;
	virtual fim_err_t clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2)=0;
	virtual int get_input(fim_key_t * c);
	virtual fim_key_t catchInteractiveCommand(fim_ts_t seconds)const;
	virtual fim_err_t init_console();
	virtual void switch_if_needed(){}// really, only for making happy fbdev
	virtual void cleanup(){}// really, only for making happy fbdev

	int redraw_;
	virtual fim_err_t fs_puts(struct fs_font *f, fim_coo_t x, fim_coo_t y, const fim_char_t *str)=0;
	void fb_status_screen_new(const fim_char_t *msg, fim_bool_t draw, fim_flags_t flags);//experimental
	void quickbench();
	private:
	virtual void console_switch(fim_bool_t is_busy){}// really, only for making happy fbdev
	public:
	virtual fim_err_t resize(fim_coo_t w, fim_coo_t h){}
};

#endif
