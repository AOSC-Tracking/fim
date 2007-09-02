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
#define FB_ACTIVE    0
#define FB_REL_REQ   1
#define FB_INACTIVE  2
#define FB_ACQ_REQ   3

/* info about videomode - yes I know, quick & dirty... */
extern struct fb_fix_screeninfo   fb_fix;
extern struct fb_var_screeninfo   fb_var;
extern unsigned char             *fb_mem;
extern int			  fb_mem_offset;
extern int                        fb_switch_state;

/* init + cleanup */
int fb_probe(void);
int  fb_init(char *device, char *mode, int vt);
void fb_cleanup(void);
void fb_catch_exit_signals(void);
void fb_memset(void *addr, int c, size_t len);

/* console switching */
int  fb_switch_init(void);
void fb_switch_release(void);
void fb_switch_acquire(void);
void fb_setcolor(int c);
