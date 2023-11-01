/*
 SDLDevice.h : GTK device Fim driver header file

 (c) 2023-2023 Michele Martone

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

// TODO: this is work in progress, don't use it.
#ifndef FIM_GTKDEVICE_H
#define FIM_GTKDEVICE_H
#ifdef FIM_WITH_LIBGTK

class GTKDevice : public DisplayDevice {
	public:
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	GTKDevice(MiniConsole& mc_, fim::string opts): DisplayDevice (mc_) {}
#else /* FIM_WANT_NO_OUTPUT_CONSOLE */
	GTKDevice(fim::string opts):DisplayDevice() {}
#endif /* FIM_WANT_NO_OUTPUT_CONSOLE */

	fim_err_t initialize(fim::sym_keys_t&);
	void finalize() {}
	fim_err_t display(const void*, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_flags_t){return 0;}
	fim_coo_t get_chars_per_line() const {return 0;}
	fim_coo_t get_chars_per_column() const {return 0;}
	fim_coo_t width() const {return 0;}
	fim_bpp_t get_bpp() const {return 0;}
	fim_coo_t height() const {return 0;}
	fim_coo_t status_line_height() const {return 0;}
	fim_err_t status_line(const fim_char_t*) {return 0;}
	fim_bool_t handle_console_switch() {return 0;}
	fim_err_t clear_rect(fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t) {return 0;}
	fim_err_t fill_rect(fim_coo_t, fim_coo_t, fim_coo_t, fim_coo_t, fim_color_t) {return 0;}
	fim_err_t fs_puts(fim::fs_font*, fim_coo_t, fim_coo_t, const fim_char_t*) {return 0;}
};
#endif /* FIM_WITH_LIBGTK */
#endif /* FIM_GTKDEVICE_H */
