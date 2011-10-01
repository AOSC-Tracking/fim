/* $LastChangedDate$ */
/*
 Imlib2.cpp : Imlib2 device Fim driver file

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

/*
 * NOTES : The Imlib2 support is INCOMPLETE
 */
#include "fim.h"

#ifdef FIM_WITH_LIBIMLIB2

#include "Imlib2Device.h"
#define FIM_IL2_PRINTF(X) printf("%s",X)
namespace fim
{
	extern CommandConsole cc;
}

fim_err_t Imlib2Device::parse_optstring(const fim_char_t *os)
{
		return FIM_ERR_NO_ERROR;
err:
		return FIM_ERR_GENERIC;
}

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	Imlib2Device::Imlib2Device(MiniConsole & mc_, fim::string opts):DisplayDevice(mc_)
#else
	Imlib2Device::Imlib2Device(
			fim::string opts
			):DisplayDevice()
#endif
	{
	}

	fim_err_t Imlib2Device::clear_rect_(
		void* dst,	// destination array 
		fim_coo_t oroff,fim_coo_t ocoff,	// row  and column  offset of the first output pixel
		fim_coo_t orows,fim_coo_t ocols,	// rows and columns drawable in the output buffer
		fim_coo_t ocskip		// output columns to skip for each line
	)
	{
		return  FIM_ERR_GENERIC;
	}

	fim_err_t Imlib2Device::display(
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
		return FIM_ERR_NO_ERROR;
	}

	fim_err_t Imlib2Device::initialize(sym_keys_t &sym_keys)
	{
		return FIM_ERR_NO_ERROR;
err:
		return FIM_ERR_GENERIC;
	}

	void Imlib2Device::finalize()
	{
		finalized_=true;
	}

	int Imlib2Device::get_chars_per_column()
	{
		return height() / f_->height;
	}

	int Imlib2Device::get_chars_per_line()
	{
		return width() / f_->width;
	}

	fim_coo_t Imlib2Device::width()
	{
		return 0;
	}

	fim_coo_t Imlib2Device::height()
	{
		return 0;
	}

	fim_sys_int Imlib2Device::get_input(fim_key_t * c, bool want_poll)
	{
		return 0;
	}

	fim_err_t Imlib2Device::fill_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2, fim_color_t color)
	{
		return FIM_ERR_NO_ERROR;
	}

	fim_err_t Imlib2Device::clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2)
	{
		return FIM_ERR_NO_ERROR;
	}

void Imlib2Device::fs_render_fb(fim_coo_t x_, fim_coo_t y, FSXCharInfo *charInfo, fim_byte_t *data)
{

}

fim_err_t Imlib2Device::fs_puts(struct fs_font *f_, fim_coo_t x, fim_coo_t y, const fim_char_t *str)
{
	return FIM_ERR_NO_ERROR;
err:
	return FIM_ERR_GENERIC;
}

	fim_err_t Imlib2Device::status_line(const fim_char_t *msg)
	{
done:
		return FIM_ERR_NO_ERROR;
	}

	fim_key_t Imlib2Device::catchInteractiveCommand(fim_ts_t seconds)const
	{
		return -1;
	}

	void Imlib2Device::flush()
	{
	}

	void Imlib2Device::lock()
	{
	}

	void Imlib2Device::unlock()
	{
	}

	bool Imlib2Device::allowed_resolution(fim_coo_t w, fim_coo_t h)
	{
		return true;
	}

	fim_err_t Imlib2Device::resize(fim_coo_t w, fim_coo_t h)
	{
		return FIM_ERR_NO_ERROR;
	}

	fim_err_t Imlib2Device::reinit(const fim_char_t *rs)
	{
		return FIM_ERR_GENERIC;
	}

	fim_err_t Imlib2Device::set_wm_caption(const fim_char_t *msg)
	{
		fim_err_t rc=FIM_ERR_NO_ERROR;
err:
		return rc;
	}
	
	fim_err_t Imlib2Device::reset_wm_caption()
	{
		return FIM_ERR_NO_ERROR;
	}
#endif
