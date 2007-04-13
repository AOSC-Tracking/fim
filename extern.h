/* $Id$ */
/*
 extern.h : Variables and includes for gluing Fim to Fbi and the lexer/parser subsystem

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef EXTERN_H
#define EXTERN_H
#include "common.h"
//#include "src/fbtools.h"	//for FB_ costants..
#define min(x,y) ((x)<(y)?(x):(y))
extern rl_hook_func_t *rl_event_hook;
extern rl_hook_func_t *rl_pre_input_hook;

/*
 *	This file documents the dependency from fbi.
 */

extern "C"{
int  fb_font_width(void);
int  fb_font_height(void);


extern float fbgamma;
extern void fb_clear_rect(int x1, int x2, int y1,int y2);
	
extern int switch_last, fb_switch_state;
extern int redraw,visible;
extern unsigned int statusline_cursor;

extern void fb_status_line(unsigned char *msg);
//extern void console_switch(int is_busy);
void console_switch(int is_busy);



extern void fb_switch_release();
extern void fb_switch_acquire();



int fs_puts(struct fs_font *f, unsigned int x, unsigned int y, unsigned char *str);
struct fs_font * fb_font_get_current_font(void);
void fb_clear_screen(void);
//void scale_fix_top_left(float old, float, struct ida_image *img);
void free_image(struct ida_image *img);
struct ida_image* read_image(char *filename);
char *make_info(struct ida_image *img, float scale);
char *make_desc(struct ida_image_info *img, char *filename);
extern void svga_display_image(struct ida_image *img, int xoff, int yoff,int,int);//mirror and flip patch
//extern void status(const char *desc,const char *info);
//extern void status_screen(const char *desc, char *info);
extern void fb_setcolor(int c);
//static char *fontname;
extern char                       *fbdev;
extern char                       *fbmode;
extern int                        fd, debug;

extern    int              vt;
extern void fb_catch_exit_signals(void);
extern int fb_init(char *device, char *mode, int vt);
extern int fb_switch_init();
void fb_text_init2(void);
int main_(int argc, char *argv[]);
void fb_text_init1(char *font);

extern struct fb_var_screeninfo   fb_var;
extern void svga_dither_palette(int r, int g, int b);

//int             dither = FALSE, pcd_res = 3, steps = 50;
extern int             dither, pcd_res, steps;
extern void            init_dither(int, int, int, int);
extern struct fb_fix_screeninfo   fb_fix;
void linear_palette(int bit);
void lut_init(int depth);
extern unsigned short red[256],  green[256],  blue[256];
extern struct fb_cmap cmap;

extern     int              c, editable, once;
extern     int              need_read, need_refresh;
extern     int              i, arg, key;

extern     char             *line, *info, *desc;
extern     char             linebuffer[128];

//void cleanup_and_exit(int code);
void show_error(unsigned char *msg);
extern void fb_clear_mem(void);

extern struct ida_image* scale_image(struct ida_image *src, float scale);
void free_image(struct ida_image *img);
struct ida_image* read_image(char *filename);
struct flist* flist_next(struct flist *f, int eof, int loop);
struct flist* flist_prev(struct flist *f);
struct flist* flist_goto(int dest);
void flist_renumber(void);
void fb_cleanup(void);
//static struct flist  *fcurrent;
//static int           fcount;
extern int svga_show(struct ida_image *img, int timeout, char *desc, char *info, int *nr);
}
class fim::CommandConsole;
extern fim::CommandConsole cc;
#endif
