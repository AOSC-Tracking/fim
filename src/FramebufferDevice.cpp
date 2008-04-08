/* $Id$ */
/*
 FramebufferDevice.cpp : Linux Framebuffer functions from fbi, adapted for fim

 (c) 2008 Michele Martone
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
/*
 * This file comes from fbi, and will undergo severe reorganization.
 * */

#include "FbiStuffFbtools.h"
#include "FramebufferDevice.h"
#include <sys/user.h>	//  for PAGE_MASK (sometimes it is needed to include it here explicitly)

namespace fim
{
#define DITHER_LEVEL 8

typedef unsigned long vector[DITHER_LEVEL];
typedef vector  matrix[DITHER_LEVEL];



//#if DITHER_LEVEL == 8 ||  DITHER_LEVEL == 4
//static int matrix   DM ;
//#endif
#if DITHER_LEVEL == 8
#define DITHER_MASK 7
static matrix   DM =
{
    {0, 32, 8, 40, 2, 34, 10, 42},
    {48, 16, 56, 24, 50, 18, 58, 26},
    {12, 44, 4, 36, 14, 46, 6, 38},
    {60, 28, 52, 20, 62, 30, 54, 22},
    {3, 35, 11, 43, 1, 33, 9, 41},
    {51, 19, 59, 27, 49, 17, 57, 25},
    {15, 47, 7, 39, 13, 45, 5, 37},
    {63, 31, 55, 23, 61, 29, 53, 21}
};

#endif

#if DITHER_LEVEL == 4
#define DITHER_MASK 3
static matrix   DM =
{
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}
};

#endif



extern fim::CommandConsole cc;

void _fb_switch_signal(int signal)
{
	extern FramebufferDevice ffd;
	ffd.fb_switch_signal(signal);
}

int FramebufferDevice::fs_puts(struct fs_font *f, unsigned int x, unsigned int y, unsigned char *str)
{
    unsigned char *pos,*start;
    int i,c,j,w;

    pos  = fb_mem+fb_mem_offset;
    pos += fb_fix.line_length * y;
    for (i = 0; str[i] != '\0'; i++) {
	c = str[i];
	if (NULL == f->eindex[c])
	    continue;
	/* clear with bg color */
	start = pos + x*fs_bpp + f->fontHeader.max_bounds.descent * fb_fix.line_length;
	w = (f->eindex[c]->width+1)*fs_bpp;
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (j = 0; j < f->height; j++) {
/////	    memset_combine(start,0x20,w);
	    memset(start,0,w);
	    start += fb_fix.line_length;
	}
#else
	//sometimes we can gather multiple calls..
	if(fb_fix.line_length==w)
	{
		//contiguous case
		memset(start,0,w*f->height);
	    	start += fb_fix.line_length*f->height;
	}
	else
	for (j = 0; j < f->height; j++) {
	    memset(start,0,w);
	    start += fb_fix.line_length;
	}
#endif
	/* draw char */
	start = pos + x*fs_bpp + fb_fix.line_length * (f->height-f->eindex[c]->ascent);
	fontserver.fs_render_fb(start,fb_fix.line_length,f->eindex[c],f->gindex[c]);
	x += f->eindex[c]->width;
	if (x > fb_var.xres - f->width)
	    return -1;
    }
    return x;
}

	int FramebufferDevice::framebuffer_init()
	{
		int rc=0;
		//initialization of the framebuffer text
		FontServer::fb_text_init1(fontname,&f);
		fd = fb_init(fbdev, fbmode, vt);
		if(fd==-1)
			fd = fb_init(fbdev, fbmode, vt,0xbabebabe==0xbabebabe);//maybe we are under screen..
		if(fd==-1)
			if(fd==-1)exit(1);
			//return -1;//this is a TEMPORARY and DEAF,DUMB, AND BLIND bug noted by iam
		//setting signals to handle in the right ways signals
		fb_catch_exit_signals();
		fb_switch_init();
		/*
		 * C-z is inhibited now (for framebuffer's screen safety!)
		 */
		signal(SIGTSTP,SIG_IGN);
		//signal(SIGSEGV,cleanup_and_exit);
		//set text color to white ?
		
		// textual console reformatting
		mc.setRows ((fb_var.yres/fb_font_height())/2);
		mc.reformat(fb_var.xres/fb_font_width());

		//initialization of the framebuffer device handlers
		if(rc=fb_text_init2())return rc;
	
			switch (fb_var.bits_per_pixel) {
		case 8:
			svga_dither_palette(8, 8, 4);
			dither = TRUE;
			init_dither(8, 8, 4, 2);
			break;
		case 15:
	    	case 16:
	        	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR)
	        	    linear_palette(5);
			if (fb_var.green.length == 5) {
			    lut_init(15);
			} else {
			    lut_init(16);
			}
			break;
		case 24:
	        	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR)
	      	      linear_palette(8);
			break;
		case 32:
	        	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR)
	          	  linear_palette(8);
			lut_init(24);
			break;
		default:
			fprintf(stderr, "Oops: %i bit/pixel ???\n",
				fb_var.bits_per_pixel);
			std::exit(1);
	    	}
	    	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR ||
			fb_var.bits_per_pixel == 8)
		{
			if (-1 == ioctl(fd,FBIOPUTCMAP,&cmap)) {
		    		perror("ioctl FBIOPUTCMAP");
			    std::exit(1);
			}
		}
		return 0;
	}

void FramebufferDevice::dev_init(void)
{
    struct stat dummy;

    if (NULL != devices)
	return;
    if (0 == stat("/dev/.devfsd",&dummy))
	devices = &devs_devfs;
    else
	devices = &devs_default;

}


void FramebufferDevice::console_switch(int is_busy)
{
	//FIX ME
	switch (fb_switch_state) {
	case FB_REL_REQ:
		fb_switch_release();
	case FB_INACTIVE:
		visible = 0;///////
	break;
	case FB_ACQ_REQ:
		fb_switch_acquire();
	case FB_ACTIVE:
		//when stepping in console..
		visible = 1;	///////////
		ioctl(fd,FBIOPAN_DISPLAY,&fb_var);
		redraw = 1;
		cc.setVariable("fresh",1);	//!!
	/*
	 * thanks to the next line, the image is redrawn each time 
	 * the console is switched! 
	 */
		cc.redisplay();
		/*
		 * PROBLEMS : image tearing (also in actual fbi..)
		 */
		//fb_clear_screen();
	//if (is_busy) status("busy, please wait ...", NULL);		
	break;
	default:
	break;
    	}
	switch_last = fb_switch_state;
	return;
}

void FramebufferDevice::svga_display_image_new(struct ida_image *img, int xoff, int yoff,unsigned int bx,unsigned int bw,unsigned int by,unsigned int bh,int mirror,int flip)
{
/*	bx is the box's x origin
 *	by is the box's y origin
 *	bw is the box's width
 *	bh is the box's heigth
 * */

	/*
	 * WARNING : SHOULD ASSeRT BX+BW < FB_VAR.XReS ..!!
	 * */
    unsigned int     dwidth  = MIN(img->i.width,  bw);
    unsigned int     dheight = MIN(img->i.height, bh);
    unsigned int     data, video, bank, offset, bytes, y;
    int yo=(bh-dheight)/2;
    int xo=(bw-dwidth )/2;
    int cxo=bw-dwidth-xo;
    int cyo=bh-yo;

    if (!visible)/*COMMENT THIS IF svga_display_image IS NOT IN A CYCLE*/
	return;
    /*fb_clear_screen();//EXPERIMENTAL
    if(xoff&&yoff)fb_clear_rect(0,xoff,0,yoff);*/

    bytes = FB_BPP;

    /* offset for image data (image > screen, select visible area) */
    offset = (yoff * img->i.width + xoff) * 3;
    
    /* offset for video memory (image < screen, center image) */
    video = 0, bank = 0;
    video += FB_BPP * (bx);
    if (img->i.width < bw)
    {	    
	    video += FB_BPP * (xo);
    }
    
    video += fb_fix.line_length * (by);
    if (img->i.height < bh )
    {	   
	    video += fb_fix.line_length * (yo);
    }

    if (dheight < bh ) 
    {	    
    	/* clear by lines */
#ifdef FIM_FASTER_CLEARLINES
	if(bw==fb_var.xres && bx==0)
	{
		/*
		 * +------------------------------+
		 * | whole screen line clear join |
		 * +------------------------------+
		 */
		// wide screen clear
		{ clear_line(FB_BPP, by, bw*(bh), FB_MEM(bx,by)); }
		
		//top and bottom lines clear : maybe better
		//{ clear_line(FB_BPP, by, bw*(yo), FB_MEM(bx,by)); }
		//{ clear_line(FB_BPP, by+yo, bw*(dheight), FB_MEM(bx,by+yo)); }
		//{ clear_line(FB_BPP, by+dheight+yo, bw*(bh-yo-dheight), FB_MEM(bx,by+yo+dheight)); }
	}
	else
#endif
	{
	    	for ( y = by; y < by+yo;++y) { clear_line(FB_BPP, y, bw, FB_MEM(bx,y)); }
		for ( y = by+dheight+yo; y < by+bh;++y) { clear_line(FB_BPP, y, bw, FB_MEM(bx,y)); }
	}
    }

    if (dwidth < bw )
    {	    
#ifdef FIM_FASTER_CLEARLINES
    	    if(bw==fb_var.xres && bx==0)
	    {
	    	if (dheight >= bh ) 
			clear_line(FB_BPP, by, bw*(bh), FB_MEM(bx,by));
	    }
	    else
#endif
    	    for ( y = by; y < by+bh;++y)
	    {
		    clear_line(FB_BPP, y, xo, FB_MEM(bx,y));
		    clear_line(FB_BPP, y, cxo,FB_MEM(bx+xo+dwidth,y));
	    }
    }
    /*for ( y = 0; y < fb_var.yres;y+=100)fb_line(0, fb_var.xres, y, y);*/

    /* go ! */
    /*flip patch*/
#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif
    int fb_fix_line_length=FB_MEM_LINE_OFFSET;
    if(flip) {	fb_fix_line_length*=-1; video += (min(img->i.height,dheight)-1)*(fb_fix.line_length);}
    /*flip patch*/
    for (data = 0, y = by;
	 data < img->i.width * img->i.height * 3
	     && data / img->i.width / 3 < dheight;
	 data += img->i.width * 3, video += fb_fix_line_length)
    {
	convert_line(fb_var.bits_per_pixel, y++, dwidth,
		     fb_mem+video, img->data + data + offset,mirror);/*<- mirror patch*/
    }
}

int FramebufferDevice::fb_init(char *device, char *mode, int vt, int try_boz_patch)
{
    char   fbdev[16];
    struct vt_stat vts;

    dev_init();
    tty = 0;
    if (vt != 0)
	fb_setvt(vt);

#ifdef FIM_BOZ_PATCH
    if(!try_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_GETSTATE, &vts)) {
	fprintf(stderr,"ioctl VT_GETSTATE: %s (not a linux console?)\n",
		strerror(errno));
	return -1;
//	exit(1);
    }
    
    if (NULL == device) {
	device = getenv("FRAMEBUFFER");
	if (NULL == device) {
	    struct fb_con2fbmap c2m;
	    if (-1 == (fb = open(devices->fb0,O_RDWR /* O_WRONLY */,0))) {
		fprintf(stderr,"open %s: %s\n",devices->fb0,strerror(errno));
		exit(1);
	    }
	    c2m.console = vts.v_active;
#ifdef FIM_BOZ_PATCH
    if(!try_boz_patch){
#endif
	    if (-1 == ioctl(fb, FBIOGET_CON2FBMAP, &c2m)) {
		perror("ioctl FBIOGET_CON2FBMAP");
		exit(1);
	    }
	    close(fb);
/*	    fprintf(stderr,"map: vt%02d => fb%d\n",
		    c2m.console,c2m.framebuffer);*/
	    sprintf(fbdev,devices->fbnr,c2m.framebuffer);
	    device = fbdev;
#ifdef FIM_BOZ_PATCH
    	    }
    else
	    device = "/dev/fb0";
#endif
	}
    }

    /* get current settings (which we have to restore) */
    if (-1 == (fb = open(device,O_RDWR /* O_WRONLY */))) {
	fprintf(stderr,"open %s: %s\n",device,strerror(errno));
	exit(1);
    }
    if (-1 == ioctl(fb,FBIOGET_VSCREENINFO,&fb_ovar)) {
	perror("ioctl FBIOGET_VSCREENINFO");
	exit(1);
    }
    if (-1 == ioctl(fb,FBIOGET_FSCREENINFO,&fb_fix)) {
	perror("ioctl FBIOGET_FSCREENINFO");
	exit(1);
    }
    if (fb_ovar.bits_per_pixel == 8 ||
	fb_fix.visual == FB_VISUAL_DIRECTCOLOR) {
	if (-1 == ioctl(fb,FBIOGETCMAP,&ocmap)) {
	    perror("ioctl FBIOGETCMAP");
	    exit(1);
	}
    }
#ifdef FIM_BOZ_PATCH
    if(!try_boz_patch)
#endif
    if (-1 == ioctl(tty,KDGETMODE, &kd_mode)) {
	perror("ioctl KDGETMODE");
	exit(1);
    }
#ifdef FIM_BOZ_PATCH
    if(!try_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_GETMODE, &vt_omode)) {
	perror("ioctl VT_GETMODE");
	exit(1);
    }
    tcgetattr(tty, &term);
    
    /* switch mode */
    fb_setmode(mode);
    
    /* checks & initialisation */
    if (-1 == ioctl(fb,FBIOGET_FSCREENINFO,&fb_fix)) {
	perror("ioctl FBIOGET_FSCREENINFO");
	exit(1);
    }
    if (fb_fix.type != FB_TYPE_PACKED_PIXELS) {
	fprintf(stderr,"can handle only packed pixel frame buffers\n");
	goto err;
    }
#if 0
    switch (fb_var.bits_per_pixel) {
    case 8:
	white = 255; black = 0; bpp = 1;
	break;
    case 15:
    case 16:
	if (fb_var.green.length == 6)
	    white = 0xffff;
	else
	    white = 0x7fff;
	black = 0; bpp = 2;
	break;
    case 24:
	white = 0xffffff; black = 0; bpp = fb_var.bits_per_pixel/8;
	break;
    case 32:
	white = 0xffffff; black = 0; bpp = fb_var.bits_per_pixel/8;
	fb_setpixels = fb_setpixels4;
	break;
    default:
	fprintf(stderr, "Oops: %i bit/pixel ???\n",
		fb_var.bits_per_pixel);
	goto err;
    }
#endif
    fb_mem_offset = (unsigned long)(fb_fix.smem_start) & (~PAGE_MASK);
    fb_mem = (unsigned char*) mmap(NULL,fb_fix.smem_len+fb_mem_offset,
		  PROT_READ|PROT_WRITE,MAP_SHARED,fb,0);
    if (-1L == (long)fb_mem) {
	perror("mmap");
	goto err;
    }
    /* move viewport to upper left corner */
    if (fb_var.xoffset != 0 || fb_var.yoffset != 0) {
	fb_var.xoffset = 0;
	fb_var.yoffset = 0;
	if (-1 == ioctl(fb,FBIOPAN_DISPLAY,&fb_var)) {
	    perror("ioctl FBIOPAN_DISPLAY");
	    goto err;
	}
    }
#ifdef FIM_BOZ_PATCH
    if(!try_boz_patch)
#endif
    if (-1 == ioctl(tty,KDSETMODE, KD_GRAPHICS)) {
	perror("ioctl KDSETMODE");
	goto err;
    }
    fb_activate_current(tty);

    /* cls */
    fb_memset(fb_mem+fb_mem_offset,0,fb_fix.smem_len);

#ifdef FIM_BOZ_PATCH
    with_boz_patch=try_boz_patch;
#endif
    return fb;

 err:
    fb_cleanup();
    exit(1);
}

void FramebufferDevice::fb_memset (void *addr, int c, size_t len)
{
#if 1 /* defined(__powerpc__) */
    unsigned int i, *p;
    
    i = (c & 0xff) << 8;
    i |= i << 16;
    len >>= 2;
#ifdef FIM_IS_SLOWER_THAN_FBI
    for (p = addr; len--; p++)
	*p = i;
#else
    memset(addr, i, len );
#endif
#else
    memset(addr, c, len);
#endif
}

void FramebufferDevice::fb_setvt(int vtno)
{
    struct vt_stat vts;
    char vtname[12];
    
    if (vtno < 0) {
	if (-1 == ioctl(tty,VT_OPENQRY, &vtno) || vtno == -1) {
	    perror("ioctl VT_OPENQRY");
	    exit(1);
	}
    }

    vtno &= 0xff;
    sprintf(vtname, devices->ttynr, vtno);
    chown(vtname, getuid(), getgid());
    if (-1 == access(vtname, R_OK | W_OK)) {
	fprintf(stderr,"access %s: %s\n",vtname,strerror(errno));
	exit(1);
    }
    switch (fork()) {
    case 0:
	break;
    case -1:
	perror("fork");
	exit(1);
    default:
	exit(0);
    }
    close(tty);
    close(0);
    close(1);
    close(2);
    setsid();
    open(vtname,O_RDWR);
    dup(0);
    dup(0);

#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_GETSTATE, &vts)) {
	perror("ioctl VT_GETSTATE");
	exit(1);
    }
    orig_vt_no = vts.v_active;
    if (-1 == ioctl(tty,VT_ACTIVATE, vtno)) {
	perror("ioctl VT_ACTIVATE");
	exit(1);
    }
    if (-1 == ioctl(tty,VT_WAITACTIVE, vtno)) {
	perror("ioctl VT_WAITACTIVE");
	exit(1);
    }
}

int FramebufferDevice::fb_setmode(char *name)
{
    FILE *fp;
    char line[80],label[32],value[16];
    int  geometry=0, timings=0;
    
    /* load current values */
    if (-1 == ioctl(fb,FBIOGET_VSCREENINFO,&fb_var)) {
	perror("ioctl FBIOGET_VSCREENINFO");
	exit(1);
    }
    
    if (NULL == name)
	return -1;
    if (NULL == (fp = fopen("/etc/fb.modes","r")))
	return -1;
    while (NULL != fgets(line,79,fp)) {
	if (1 == sscanf(line, "mode \"%31[^\"]\"",label) &&
	    0 == strcmp(label,name)) {
	    /* fill in new values */
	    fb_var.sync  = 0;
	    fb_var.vmode = 0;
	    while (NULL != fgets(line,79,fp) &&
		   NULL == strstr(line,"endmode")) {
		if (5 == sscanf(line," geometry %d %d %d %d %d",
				&fb_var.xres,&fb_var.yres,
				&fb_var.xres_virtual,&fb_var.yres_virtual,
				&fb_var.bits_per_pixel))
		    geometry = 1;
		if (7 == sscanf(line," timings %d %d %d %d %d %d %d",
				&fb_var.pixclock,
				&fb_var.left_margin,  &fb_var.right_margin,
				&fb_var.upper_margin, &fb_var.lower_margin,
				&fb_var.hsync_len,    &fb_var.vsync_len))
		    timings = 1;
		if (1 == sscanf(line, " hsync %15s",value) &&
		    0 == strcasecmp(value,"high"))
		    fb_var.sync |= FB_SYNC_HOR_HIGH_ACT;
		if (1 == sscanf(line, " vsync %15s",value) &&
		    0 == strcasecmp(value,"high"))
		    fb_var.sync |= FB_SYNC_VERT_HIGH_ACT;
		if (1 == sscanf(line, " csync %15s",value) &&
		    0 == strcasecmp(value,"high"))
		    fb_var.sync |= FB_SYNC_COMP_HIGH_ACT;
		if (1 == sscanf(line, " extsync %15s",value) &&
		    0 == strcasecmp(value,"true"))
		    fb_var.sync |= FB_SYNC_EXT;
		if (1 == sscanf(line, " laced %15s",value) &&
		    0 == strcasecmp(value,"true"))
		    fb_var.vmode |= FB_VMODE_INTERLACED;
		if (1 == sscanf(line, " double %15s",value) &&
		    0 == strcasecmp(value,"true"))
		    fb_var.vmode |= FB_VMODE_DOUBLE;
	    }
	    /* ok ? */
	    if (!geometry || !timings)
		return -1;
	    /* set */
	    fb_var.xoffset = 0;
	    fb_var.yoffset = 0;
	    if (-1 == ioctl(fb,FBIOPUT_VSCREENINFO,&fb_var))
		perror("ioctl FBIOPUT_VSCREENINFO");
	    /* look what we have now ... */
	    if (-1 == ioctl(fb,FBIOGET_VSCREENINFO,&fb_var)) {
		perror("ioctl FBIOGET_VSCREENINFO");
		exit(1);
	    }
	    return 0;
	}
    }
    return -1;
}

int FramebufferDevice::fb_activate_current(int tty)
{
/* Hmm. radeonfb needs this. matroxfb doesn't. */
    struct vt_stat vts;
    
#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_GETSTATE, &vts)) {
	perror("ioctl VT_GETSTATE");
	return -1;
    }
#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_ACTIVATE, vts.v_active)) {
	perror("ioctl VT_ACTIVATE");
	return -1;
    }
#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_WAITACTIVE, vts.v_active)) {
	perror("ioctl VT_WAITACTIVE");
	return -1;
    }
    return 0;
}

void FramebufferDevice::fb_status_line(unsigned char *msg)
{
    int y;
    
    if (!visible)
	return;
    y = fb_var.yres - f->height - ys;
    fb_memset(fb_mem + fb_fix.line_length * y, 0,
	      fb_fix.line_length * (f->height+ys));
    fb_line(0, fb_var.xres, y, y);
    fs_puts(f, 0, y+ys, msg);
}

void FramebufferDevice::fb_edit_line(unsigned char *str, int pos)
{
    int x,y;
    
    if (!visible)
	return;
    y = fb_var.yres - f->height - ys;
    x = pos * f->width;
    fb_memset(fb_mem + fb_fix.line_length * y, 0,
	      fb_fix.line_length * (f->height+ys));
    fb_line(0, fb_var.xres, y, y);
    fs_puts(f, 0, y+ys, str);
    fb_line(x, x + f->width, fb_var.yres-1, fb_var.yres-1);
    fb_line(x, x + f->width, fb_var.yres-2, fb_var.yres-2);
}

void FramebufferDevice::fb_text_box(int x, int y, char *lines[], unsigned int count)
{
    unsigned int i,len,max, x1, x2, y1, y2;

    if (!visible)
	return;

    max = 0;
    for (i = 0; i < count; i++) {
	len = strlen(lines[i]);
	if (max < len)
	    max = len;
    }
    x1 = x;
    x2 = x + max * f->width;
    y1 = y;
    y2 = y + count * f->height;

    x += xs; x2 += 2*xs;
    y += ys; y2 += 2*ys;
    
    fb_clear_rect(x1, x2, y1, y2);
    fb_rect(x1, x2, y1, y2);
    for (i = 0; i < count; i++) {
	fs_puts(f,x,y,(unsigned char*)lines[i]);
	y += f->height;
    }
}

void FramebufferDevice::fb_line(int x1, int x2, int y1,int y2)
{
/*static void fb_line(int x1, int x2, int y1,int y2)*/
    int x,y,h;
    float inc;

    if (x2 < x1)
	h = x2, x2 = x1, x1 = h;
    if (y2 < y1)
	h = y2, y2 = y1, y1 = h;
    if (x2 - x1 < y2 - y1) {
	inc = (float)(x2-x1)/(float)(y2-y1);
	for (y = y1; y <= y2; y++) {
	    x = x1 + (int)(inc * (float)(y - y1));
	    fb_setpixel(x,y,fs_white);
	}
    } else {
	inc = (float)(y2-y1)/(float)(x2-x1);
	for (x = x1; x <= x2; x++) {
	    y = y1 + (int)(inc * (float)(x - x1));
	    fb_setpixel(x,y,fs_white);
	}
    }
}


void FramebufferDevice::fb_rect(int x1, int x2, int y1,int y2)
{
    fb_line(x1, x2, y1, y1);
    fb_line(x1, x2, y2, y2);
    fb_line(x1, x1, y1, y2);
    fb_line(x2, x2, y1, y2);
}

void FramebufferDevice::fb_setpixel(int x, int y, unsigned int color)
{
    unsigned char *ptr;

    ptr  = fb_mem;
    ptr += y * fb_fix.line_length;
    ptr += x * fs_bpp;
    fs_setpixel(ptr, color);
}

void FramebufferDevice::fb_clear_rect(int x1, int x2, int y1,int y2)
{
    unsigned char *ptr;
    int y,h;

    if (!visible)
	return;

    if (x2 < x1)
	h = x2, x2 = x1, x1 = h;
    if (y2 < y1)
	h = y2, y2 = y1, y1 = h;
    ptr  = fb_mem;
    ptr += y1 * fb_fix.line_length;
    ptr += x1 * fs_bpp;

    for (y = y1; y <= y2; y++) {
	fb_memset(ptr, 0, (x2 - x1 + 1) * fs_bpp);
	ptr += fb_fix.line_length;
    }
}

void FramebufferDevice::fb_clear_mem(void)
{
    if (visible)
	fb_memset(fb_mem,0,fb_fix.smem_len);
}



void FramebufferDevice::fb_cleanup(void)
{
    /* restore console */
    if (-1 == ioctl(fb,FBIOPUT_VSCREENINFO,&fb_ovar))
	perror("ioctl FBIOPUT_VSCREENINFO");
    if (-1 == ioctl(fb,FBIOGET_FSCREENINFO,&fb_fix))
	perror("ioctl FBIOGET_FSCREENINFO");
    if (fb_ovar.bits_per_pixel == 8 ||
	fb_fix.visual == FB_VISUAL_DIRECTCOLOR) {
	if (-1 == ioctl(fb,FBIOPUTCMAP,&ocmap))
	    perror("ioctl FBIOPUTCMAP");
    }
    close(fb);

#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,KDSETMODE, kd_mode))
	perror("ioctl KDSETMODE");
#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_SETMODE, &vt_omode))
	perror("ioctl VT_SETMODE");
    if (orig_vt_no && -1 == ioctl(tty, VT_ACTIVATE, orig_vt_no))
	perror("ioctl VT_ACTIVATE");
    if (orig_vt_no && -1 == ioctl(tty, VT_WAITACTIVE, orig_vt_no))
	perror("ioctl VT_WAITACTIVE");
    tcsetattr(tty, TCSANOW, &term);
    close(tty);
}


unsigned char * FramebufferDevice::convert_line(int bpp, int line, int owidth, char unsigned *dest, char unsigned *buffer, int mirror)/*dez's mirror patch*/
{
    unsigned char  *ptr  = (unsigned char *)dest;
    unsigned short *ptr2 = (unsigned short*)dest;
    unsigned long  *ptr4 = (unsigned long *)dest;
    int x;
    int xm;/*mirror patch*/

    switch (fb_var.bits_per_pixel) {
    case 8:
	dither_line(buffer, ptr, line, owidth, mirror);
	ptr += owidth;
	return ptr;
    case 15:
    case 16:
#ifdef FIM_IS_SLOWER_THAN_FBI
	/*swapped RGB patch*/
	for (x = 0; x < owidth; x++) {
            xm=mirror?owidth-1-x:x;
	    ptr2[xm] = lut_red[buffer[x*3]] |
		lut_green[buffer[x*3+1]] |
		lut_blue[buffer[x*3+2]];
	}
#else
	if(!mirror)
	for (x = 0; x < owidth; x++) {
	    ptr2[x] = lut_red[buffer[x*3+2]] |
		lut_green[buffer[x*3+1]] |
		lut_blue[buffer[x*3]];
	}
	else
	for (x = 0,xm=owidth; x < owidth; x++) {
            xm--;
	    ptr2[xm] = lut_red[buffer[x*3+2]] |
		lut_green[buffer[x*3+1]] |
		lut_blue[buffer[x*3]];
	}
#endif
	ptr2 += owidth;
	return (unsigned char*)ptr2;
    case 24:
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (x = 0; x < owidth; x++) {
            xm=mirror?owidth-1-x:x;
	    ptr[3*xm+2] = buffer[3*x+0];
	    ptr[3*xm+1] = buffer[3*x+1];
	    ptr[3*xm+0] = buffer[3*x+2];
	}
#else
	/*swapped RGB patch*/
	if(!mirror)
	{
		/*
		 * this code could be faster if using processor specific routines..
		 * ... or maybe even not ?
		 */
		//owidth*=3;
#if 0
		for (x = 0; x < owidth; x+=3)
		{
	            ptr[x+2] = buffer[x+0];
		    ptr[x+1] = buffer[x+1];
		    ptr[x+0] = buffer[x+2];
		}
#else
		/*
		 * this is far worse than the preceding !
		 */
		memcpy(ptr,buffer,owidth*3);
		//register char t;
		//register i=x;
		/*since RGB and GBR swap already done, this is not necessary*/
		/*for (i = 0; i < owidth; i+=3)
		{
	            t=ptr[i];
	            ptr[i]=ptr[i+2];
	            ptr[i+2]=t;
		}*/
#endif
		//owidth/=3;
	}else
/*this is still slow ... FIXME*/
#if 0
	for (x = 0; x < owidth; x++) {
	    x*=3;
            xm=3*owidth-x-3;
	    ptr[xm+2] = buffer[x+0];
	    ptr[xm+1] = buffer[x+1];
	    ptr[xm+0] = buffer[x+2];
	    x/=3;
	}
#else
	for (x = 0; x < owidth; x++) {
	    x*=3;
            xm=3*owidth-x-3;
	    ptr[xm+2] = buffer[x+2];
	    ptr[xm+1] = buffer[x+1];
	    ptr[xm+0] = buffer[x+0];
	    x/=3;
	}
#endif
#endif
	ptr += owidth * 3;
	return ptr;
    case 32:
#ifndef FIM_IS_SLOWER_THAN_FBI
	/*swapped RGB patch*/
	for (x = 0; x < owidth; x++) {
            xm=mirror?owidth-1-x:x;
	    ptr4[xm] = lut_red[buffer[x*3+2]] |
		lut_green[buffer[x*3+1]] |
		lut_blue[buffer[x*3]];
	}
#else
	for (x = 0; x < owidth; x++) {
            xm=mirror?owidth-1-x:x;
	    ptr4[xm] = lut_red[buffer[x*3]] |
		lut_green[buffer[x*3+1]] |
		lut_blue[buffer[x*3+2]];
	}
#endif
	ptr4 += owidth;
	return (unsigned char*)ptr4;
    default:
	/* keep compiler happy */
	return NULL;
    }
}

/*dez's*/
/*unsigned char * FramebufferDevice::clear_lines(int bpp, int lines, int owidth, char unsigned *dest)
{

}*/

unsigned char * FramebufferDevice::clear_line(int bpp, int line, int owidth, char unsigned *dest)
{
    unsigned char  *ptr  = (unsigned char*)dest;
    unsigned short *ptr2 = (unsigned short*)dest;
    unsigned long  *ptr4 = (unsigned long*)dest;
    unsigned ZERO_BYTE=0x00;
    int x;

    switch (fb_var.bits_per_pixel) {
    case 8:
	bzero(ptr, owidth);
	ptr += owidth;
	return ptr;
    case 15:
    case 16:
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (x = 0; x < owidth; x++) {
	    ptr2[x] = 0x0;
	}
#else
	memset(ptr,ZERO_BYTE,2*owidth);
#endif
	ptr2 += owidth;
	return (unsigned char*)ptr2;
    case 24:
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (x = 0; x < owidth; x++) {
	    ptr[3*x+2] = 0x0;
	    ptr[3*x+1] = 0x0;
	    ptr[3*x+0] = 0x0;
	}
#else
	memset(ptr,ZERO_BYTE,3*owidth);
#endif
	ptr += owidth * 3;
	return ptr;
    case 32:
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (x = 0; x < owidth; x++) {
	    ptr4[x] = 0x0;
	}
#else
	memset(ptr,ZERO_BYTE,4*owidth);
#endif
	ptr4 += owidth;
	return (unsigned char*)ptr4;
    default:
	/* keep compiler happy */
	return NULL;
    }
}

void FramebufferDevice::init_dither(int shades_r, int shades_g, int shades_b, int shades_gray)
{
    int             i, j;
    unsigned char   low_shade, high_shade;
    unsigned short  index;
    float           red_colors_per_shade;
    float           green_colors_per_shade;
    float           blue_colors_per_shade;
    float           gray_colors_per_shade;

    red_mult = shades_g * shades_b;
    green_mult = shades_b;

    red_colors_per_shade = 256.0 / (shades_r - 1);
    green_colors_per_shade = 256.0 / (shades_g - 1);
    blue_colors_per_shade = 256.0 / (shades_b - 1);
    gray_colors_per_shade = 256.0 / (shades_gray - 1);

    /* this avoids a shift when checking these values */
    for (i = 0; i < DITHER_LEVEL; i++)
	for (j = 0; j < DITHER_LEVEL; j++)
	    DM[i][j] *= 0x10000;

    /*  setup arrays containing three bytes of information for red, green, & blue  */
    /*  the arrays contain :
     *    1st byte:    low end shade value
     *    2nd byte:    high end shade value
     *    3rd & 4th bytes:    ordered dither matrix index
     */

    for (i = 0; i < 256; i++) {

	/*  setup the red information  */
	{
	    low_shade = (unsigned char) (i / red_colors_per_shade);
	    high_shade = low_shade + 1;

	    index = (unsigned short)
		(((i - low_shade * red_colors_per_shade) / red_colors_per_shade) *
		 (DITHER_LEVEL * DITHER_LEVEL + 1));

	    low_shade *= red_mult;
	    high_shade *= red_mult;

	    red_dither[i] = (index << 16) + (high_shade << 8) + (low_shade);
	}

	/*  setup the green information  */
	{
	    low_shade = (unsigned char) (i / green_colors_per_shade);
	    high_shade = low_shade + 1;

	    index = (unsigned short)
		(((i - low_shade * green_colors_per_shade) / green_colors_per_shade) *
		 (DITHER_LEVEL * DITHER_LEVEL + 1));

	    low_shade *= green_mult;
	    high_shade *= green_mult;

	    green_dither[i] = (index << 16) + (high_shade << 8) + (low_shade);
	}

	/*  setup the blue information  */
	{
	    low_shade = (unsigned char) (i / blue_colors_per_shade);
	    high_shade = low_shade + 1;

	    index = (unsigned short)
		(((i - low_shade * blue_colors_per_shade) / blue_colors_per_shade) *
		 (DITHER_LEVEL * DITHER_LEVEL + 1));

	    blue_dither[i] = (index << 16) + (high_shade << 8) + (low_shade);
	}

	/*  setup the gray information  */
	{
	    low_shade = (unsigned char) (i / gray_colors_per_shade);
	    high_shade = low_shade + 1;

	    index = (unsigned short)
		(((i - low_shade * gray_colors_per_shade) / gray_colors_per_shade) *
		 (DITHER_LEVEL * DITHER_LEVEL + 1));

	    gray_dither[i] = (index << 16) + (high_shade << 8) + (low_shade);
	}
    }
}

void inline FramebufferDevice::dither_line(unsigned char *src, unsigned char *dest, int y, int width,int mirror)
{
    register long   a, b;
    long           *ymod, xmod;

    ymod = (long int*) DM[y & DITHER_MASK];
    /*	mirror patch by dez	*/
    register int inc;inc=mirror?-1:1;
    dest=mirror?dest+width-1:dest;
    /*	mirror patch by dez	*/
    while (width--) {
	xmod = width & DITHER_MASK;

#if 0
// 20070923 : seems like there is NO NEED OF PATCHING THIS :)
//#ifndef FIM_IS_SLOWER_THAN_FBI
	/*RGB swapped patch	*/
	b = red_dither[*(src++)];
	if (ymod[xmod] < b)
	    b >>= 8;

	a = green_dither[*(src++)];
	if (ymod[xmod] < a)
	    a >>= 8;
	b += a;

	a = blue_dither[*(src++)];
	if (ymod[xmod] < a)
	    a >>= 8;
	b += a;
#else
	/*original	*/
	b = blue_dither[*(src++)];
	if (ymod[xmod] < b)
	    b >>= 8;

	a = green_dither[*(src++)];
	if (ymod[xmod] < a)
	    a >>= 8;
	b += a;

	a = red_dither[*(src++)];
	if (ymod[xmod] < a)
	    a >>= 8;
	b += a;
#endif

    /*	mirror patch by dez	*/
	*(dest) = b & 0xff;
	dest+=inc;
	/*	*(dest++) = b & 0xff;*/
    }
}
void FramebufferDevice::dither_line_gray(unsigned char *src, unsigned char *dest, int y, int width)
{
    long           *ymod, xmod;
    register long   a;

    ymod = (long int*) DM[y & DITHER_MASK];

    while (width--) {
	xmod = width & DITHER_MASK;

	a = gray_dither[*(src++)];
	if (ymod[xmod] < a)
	    a >>= 8;

	*(dest++) = a & 0xff;
    }
}
void FramebufferDevice::fb_switch_release()
{
    ioctl(tty, VT_RELDISP, 1);
    fb_switch_state = FB_INACTIVE;
    if (debug)
	write(2,"vt: release\n",12);
}
void FramebufferDevice::fb_switch_acquire()
{
    ioctl(tty, VT_RELDISP, VT_ACKACQ);
    fb_switch_state = FB_ACTIVE;
    if (debug)
	write(2,"vt: acquire\n",12);
}
int FramebufferDevice::fb_switch_init()
{
    struct sigaction act,old;

    memset(&act,0,sizeof(act));
    act.sa_handler  = _fb_switch_signal;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1,&act,&old);
    sigaction(SIGUSR2,&act,&old);
#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_GETMODE, &vt_mode)) {
	perror("ioctl VT_GETMODE");
	exit(1);
    }
    vt_mode.mode   = VT_PROCESS;
    vt_mode.waitv  = 0;
    vt_mode.relsig = SIGUSR1;
    vt_mode.acqsig = SIGUSR2;
    
#ifdef FIM_BOZ_PATCH
    if(!with_boz_patch)
#endif
    if (-1 == ioctl(tty,VT_SETMODE, &vt_mode)) {
	perror("ioctl VT_SETMODE");
	exit(1);
    }
    return 0;
}

void FramebufferDevice::fb_switch_signal(int signal)
{
    if (signal == SIGUSR1) {
	/* release */
	fb_switch_state = FB_REL_REQ;
	if (debug)
	    write(2,"vt: SIGUSR1\n",12);
    }
    if (signal == SIGUSR2) {
	/* acquisition */
	fb_switch_state = FB_ACQ_REQ;
	if (debug)
	    write(2,"vt: SIGUSR2\n",12);
    }
}


int FramebufferDevice::fb_text_init2(void)
{
    return fs_init_fb(255);
//    return fontserver.fs_init_fb(255);
}
	int  FramebufferDevice::fb_font_width(void) { return f->width; }
	int  FramebufferDevice::fb_font_height(void) { return f->height; }

int FramebufferDevice::fs_init_fb(int white8)
{
    switch (fb_var.bits_per_pixel) {
    case 8:
	fs_white = white8; fs_black = 0; fs_bpp = 1;
	fs_setpixel = setpixel1;
	break;
    case 15:
    case 16:
	if (fb_var.green.length == 6)
	    fs_white = 0xffff;
	else
	    fs_white = 0x7fff;
	fs_black = 0; fs_bpp = 2;
	fs_setpixel = setpixel2;
	break;
    case 24:
	fs_white = 0xffffff; fs_black = 0; fs_bpp = fb_var.bits_per_pixel/8;
	fs_setpixel = setpixel3;
	break;
    case 32:
	fs_white = 0xffffff; fs_black = 0; fs_bpp = fb_var.bits_per_pixel/8;
	fs_setpixel = setpixel4;
	break;
    default:
	fprintf(stderr, "Oops: %i bit/pixel ???\n",
		fb_var.bits_per_pixel);
	return -1;
    }
    return 0;
}

/*
 *	This function treats the framebuffer screen as a text outout terminal.
 *	So it prints all the contents of its buffer on screen..
 *	if noDraw is set, the screen will be not refreshed.
	 *	NULL,NULL is the clearing combination !!
	//FIX ME
	20070628 now this function adapts to the screen resolution. yet there happens 
	something strange for a number of lines filling more than half of the screen.. 

	//FIX ME : move this functionality to some new class and add ways to scroll and manipulate it

	dez's
 */
void FramebufferDevice::fb_status_screen(const char *msg, int draw)
{	
#ifndef FIM_KEEP_BROKEN_CONSOLE
	return fb_status_screen_new(msg, draw,0);
#endif

	/*	WARNING		*/
	//noDraw=0;
	/*	WARNING		*/
	int y,i,j,l,w;
	// R rows, C columns
	int R=(fb_var.yres/fb_font_height())/2,/* half screen : more seems evil */
	C=(fb_var.xres/fb_font_width());
	static char **columns=NULL;
	static char *columns_data=NULL;
	if(R<1 || C < 1)return;		/* sa finimm'acca', nun ce sta nient'a fa! */
	/* R rows and C columns; the last one for string terminators..
	 */
	if(!columns)columns=(char**)calloc(sizeof(char**)*R,1);
	if(!columns_data)columns_data=(char*)calloc(sizeof(char)*(R*(C+1)),1);
	/* 
	 * seems tricky : we allocate one single buffer and use it as console 
	 * storage and console pointers storage ...
	 *
	 * note that we don't deallocate this area until program termination.
	 * it is because we keep the framebuffer...
	 * */
	if(!columns || !columns_data)return;

	for(i=0;i<R;++i)columns[i]=columns_data+i*(C+1);

	static int cline=0,	//current line		[0..R-1]
		   ccol=0;	//current column	[0..C]
	const char *p=msg,	//p points to the substring not yet printed
	      	    *s=p;	//s advances and updates p

	if(!msg)
	{
		cline=0;
		ccol=0;
		p=NULL;
		/*noDraw=0;*/
	}
	if(msg&&*msg=='\0')return;

	if(p)while(*p)
	{
	    //while there are characters to put on screen, we advance
	    while(*s && *s!='\n')++s;
	    //now s points to an endline or a NUL
	    l=s-p;
	    //l is the number of characters which should go on screen (from *p to s[-1])
	    w=0;
	    while(l>0)	//line processing
	    {
		    //w is the number of writable characters on this line ( w in [0,C-ccol] )
		    w=min(C-ccol,l);
		    //there remains l-=w non '\n' characters yet to process in the first substring
		    l-=w;
		    //we place the characters on the line (not padded,though)
		    strncpy(columns[cline]+ccol,p,w);
		    sanitize_string_from_nongraph(columns[cline]+ccol,w);
		    //the current column index is updated,too
		    ccol+=w;
		    //we blank the rest of the line (SHOULD BE UNNECESSARY)
		    for(i=ccol;i<C;++i)columns[cline][i]=' ';
		    //we terminate the line with a NUL
		    columns[cline][C]='\0';
		    //please note that ccol could still point to the middle of the line
		    //the last writable column index is C
	
#ifdef CERCO_GRANE
		    if(ccol>=C+1)cleanup_and_exit(-1);	//ehm.. who knows
#else
		    if(ccol>=C+1)return;
#endif
		    if(ccol==C)
		    {
			    //So if we are at the end of the line, we prepare 
			    //for a new line
			    ccol=0;
			    cline=(cline+1)%(R);
			    if(cline==0)
			    for(i=0;i<R;++i)
			    {
				    for(j=0;j<C;++j)columns[i][j]=' ';
				    columns[i][C]='\0';
			    }
			    //we clean the new line (SHOULD BE NECESSARY ONLY WITH THE FIRST LINE!)
		    	    for(i=0;i<C;++i)columns[cline][i]=' ';
		    }
	            //we advance in the string for w chars 
	    	    p+=w;	//a temporary assignment
	    }
	    	/*
		 * after the chars in [p,s-1] are consumed, we can continue
		 */
		    while(*s=='\n')
		    {
			    ++s;
			    ccol=0;
			    cline=(cline+1)%(R);
			    if(cline==0)
			    for(i=0;i<R;++i)
			    {
				    for(j=0;j<C;++j)columns[i][j]=' ';
				    columns[i][C]='\0';
			    }
		    }
	    p=s;
	}

	//if(!cc.drawOutput() || noDraw)return;//CONVENTION!
	if(!draw )return;//CONVENTION!

	    y = 1*fb_font_height();
	    for(i=0  ;i<R ;++i) fs_puts(fb_font_get_current_font(), 0, y*(i), (unsigned char*)columns[i]);

	    /*
	     *WARNING : note that columns and columns_data arrays are not freed and should not, as long as they are static.
	     * */
}

void FramebufferDevice::console_control(int arg)//experimental
{
	if(arg==0x01)fb_status_screen_new(NULL,0,arg);//experimental
	if(arg==0x02)fb_status_screen_new(NULL,0,arg);//experimental
	return;
}

#ifndef FIM_KEEP_BROKEN_CONSOLE
void FramebufferDevice::fb_status_screen_new(const char *msg, int draw, int flags)//experimental
{
	//printf("ccd\n");
	int r;
	r=mc.add(msg);
	if(r==-2)
	{
		r=mc.grow();
		if(r==-1)return;
		r=mc.add(msg);
		if(r==-1)return;
	}

	// draw e' quasi sempre 0..
	if(!draw )return;//CONVENTION!
	//printf("ccc\n");
//	mc.dump();
	
	//fb_clear_rect(0, fb_var.xres-1 ,0,fb_var.yres/2);

//	int y = fb_var.yres - f->height - ys;
//	int bpp_=(fb_fix.line_length/fb_var.xres);
	fb_memset(fb_mem ,0,fb_fix.line_length * (fb_var.yres/2)*(fs_bpp));

	mc.dump();
//	mc.dump(0,1000000);
	return;
}
#endif



	FramebufferDevice::FramebufferDevice():
	fontname(NULL),
	visible(1),
	x11_font("10x20"),
	ys( 3),
	xs(10),
	fb_mem_offset(0),
	fb_switch_state(FB_ACTIVE),
	orig_vt_no(0),
	fbdev(NULL),
	fbmode(NULL),
	fbgamma(1.0),
	vt(0),
	dither(FALSE),
	pcd_res(3),
	steps(50),
	debug(0),
	fs_setpixel(NULL),
#ifdef FIM_BOZ_PATCH
	with_boz_patch(0),
#endif
#ifndef FIM_KEEP_BROKEN_CONSOLE
	//mc(48,12),
//	int R=(fb_var.yres/fb_font_height())/2,/* half screen : more seems evil */
//	C=(fb_var.xres/fb_font_width());
#endif
	fontserver(*this)
	{

		cmap.start  =  0;
		cmap.len    =  256;
		cmap.red  =  red;
		cmap.green  =  green;
		cmap.blue  =  blue;
		//! transp!
		devs_default.fb0=   "/dev/fb0";
		devs_default.fbnr=  "/dev/fb%d";
		devs_default.ttynr= "/dev/tty%d";
		devs_devfs.fb0=   "/dev/fb/0";
		devs_devfs.fbnr=  "/dev/fb/%d";
		devs_devfs.ttynr= "/dev/vc/%d";
		ocmap.start = 0;
		ocmap.len   = 256;
		ocmap.red=ored;
		ocmap.green=ogreen;
		ocmap.blue=oblue;
		/*
		 * fbgamma and fontname are fbi - defined variables.
		 * */
		char *line;

	    	if (NULL != (line = getenv("FBGAMMA")))
	        	fbgamma = atof(line);
	    	if (NULL != (line = getenv("FBFONT")))
			fontname = line;
	}

}
