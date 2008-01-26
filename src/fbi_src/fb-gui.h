/*
     (c) 2007 Michele Martone
     (c) 1998-2006 Gerd Knorr <kraxel@bytesex.org>

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
#ifndef FIM_NO_FBI
extern int visible;

void fb_clear_mem(void);
void fb_clear_screen(void);

void fb_text_init1(char *font);
void fb_text_init2(void);
int  fb_font_width(void);
void fb_status_line(unsigned char *msg);
void fb_edit_line(unsigned char *str, int pos);
void fb_text_box(int x, int y, char *lines[], unsigned int count);
struct fs_font * fb_font_get_current_font(void);
int  fb_font_width(void);
int  fb_font_height(void);
#endif

