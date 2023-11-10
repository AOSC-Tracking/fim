/*
 SDLDevice.h : GTK device Fim driver implementation file

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
// - need to cache more vars and eliminate redundancy of gdk_pixbuf_get_pixels()
// - move vars to (anyway singleton) class
// - ...

#include "fim.h"

#ifdef FIM_WITH_LIBGTK

#include "DisplayDevice.h"
#include "GTKDevice.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkcairo.h>

#define FIM_GTK_DEBUG 1
#undef FIM_GTK_DEBUG

#define FIM_GTK_WITH_RENDERED_STATUSBAR 1 /* 1 to render, 0 to use statusbar_ */

#ifdef FIM_GTK_DEBUG
#define FIM_GTK_DBG_COUT std::cout << "GTK:" << __FILE__ ":" << __LINE__ << ":" << __func__ << "() "
#define FIM_GTK_INPUT_DEBUG(C,MSG)  \
{ \
/* i miss sooo much printf() :'( */ \
FIM_GTK_DBG_COUT << (size_t)getmilliseconds() << " : "<<MSG<<" : "; \
std::cout.setf ( std::ios::hex, std::ios::basefield ); \
std::cout.setf ( std::ios::showbase ); \
std::cout << *(fim_key_t*)(C) <<"\n"; \
std::cout.unsetf ( std::ios::showbase ); \
std::cout.unsetf ( std::ios::hex ); \
}
#else /* FIM_GTK_DEBUG */
#define FIM_GTK_DBG_COUT if(0) std::cout 
#define FIM_GTK_INPUT_DEBUG(C,MSG) {}
#endif /* FIM_GTK_DEBUG */
#define FIM_WANT_POSITION_DISPLAYED FIM_WANT_MOUSE_PAN && 0

#define FIM_GTK_KEYSYM_TO_RL(X) ((X) | (1<<31)) // after FIM_SDL_KEYSYM_TO_RL
#define FIM_GTK_KEYSYM_TO_RL_CTRL(X) 1+((X)-'a') // after FIM_SDL_KEYSYM_TO_RL_CTRL

namespace fim
{
	extern fim_int fim_fmf_; /* FIXME */
	extern CommandConsole cc;
}

#define interactive_mode_ 0

	// here temporarily
	GtkWindow *window_{};
	GtkWidget *grid_{};
	GtkWidget *menubar_{};
#if !FIM_GTK_WITH_RENDERED_STATUSBAR
	GtkWidget *statusbar_{};
#endif
	GtkWidget *drawingarea_{};
	guint context_id{};
	GtkWidget *cmdline_entry_{};
	GtkAccelGroup *accel_group_{};
	const std::string wtitle_ {"GTK-FIM -- no status yet"};
	bool control_pressed_{false};
	fim_key_t last_pressed_key_{0};
	GdkPixbuf * pixbuf{};
	fim_coo_t rowstride_{};
	fim_coo_t nw_{}, nh_{};
	int full_screen_{};

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"

static void toggle_fullscreen(const bool full_screen)
{
	if (full_screen)
		gtk_window_fullscreen (GTK_WINDOW(window_));
	else
		gtk_window_unfullscreen (GTK_WINDOW(window_));
}

static gboolean cb_window_state_event(GtkWidget *window__unused, GdkEventKey* event)
{
	// minimize/maximize
	// TODO: need implementation
	return FALSE;
}

static fim_bpp_t _get_bpp(void) {
	// FIXME: is displaced
	return 8;
}

fim_bpp_t GTKDevice::get_bpp(void)const
{
	return _get_bpp();
}

void alloc_pixbuf(int nw, int nh)
{
	const auto alpha = FALSE; /* in gmacros.h */

	nw_ = nw;
	nh_ = nh;
	if(pixbuf)
		g_object_unref(pixbuf);
	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, alpha, _get_bpp(), nw, nh);
	assert ( gdk_pixbuf_get_n_channels (pixbuf) == 3 );
	rowstride_ = gdk_pixbuf_get_rowstride (pixbuf);
	FIM_GTK_DBG_COUT << " nw:" << nw << " nh:" << nh << " pitch/3:" << rowstride_/GTKDevice::Bpp_ << "\n";
	memset (gdk_pixbuf_get_pixels(pixbuf), 0x0, rowstride_*nh); // black
}

static gboolean cb_window_event(GtkWidget *window__unused, GdkEventKey* event)
{
	FIM_GTK_DBG_COUT << " \n";

	// related to alloc_pixbuf

	cc.display_resize(gtk_widget_get_allocated_width((GtkWidget*)drawingarea_),gtk_widget_get_allocated_height((GtkWidget*)drawingarea_));
	return FALSE;
}

static gboolean cb_key_pressed(GtkWidget *window__unused, GdkEventKey* event)
{
	gboolean handled = FALSE;
	last_pressed_key_ = 0;

	if (event->keyval == GDK_KEY_Control_L)
	{
		if (event->type == GDK_KEY_RELEASE)
			control_pressed_ = true;
		if (event->type == GDK_KEY_PRESS)
			control_pressed_ = false;
	}

	if ((event->type == GDK_KEY_PRESS) /*&& (event->state & GDK_CONTROL_MASK)*/)
	{
		FIM_GTK_INPUT_DEBUG(&last_pressed_key_,"");
		std::string kst;

		if ( event->state & GDK_CONTROL_MASK )
			kst += "C-";
		last_pressed_key_ = event->keyval;

		if (last_pressed_key_ < 0x100) // isalpha & co don't require this
		if (isalpha(last_pressed_key_)) // if (last_pressed_key_ >= 'a') && (last_pressed_key_ <= 'z')
		if (isalnum(last_pressed_key_))
		if (isprint(last_pressed_key_))
		{
			if ( event->state & GDK_CONTROL_MASK )
				last_pressed_key_ = FIM_GTK_KEYSYM_TO_RL_CTRL(last_pressed_key_),
				handled = TRUE;
			if ( event->state & GDK_MOD1_MASK )
				last_pressed_key_ = FIM_GTK_KEYSYM_TO_RL(last_pressed_key_),
				handled = TRUE;
		}
		if (        last_pressed_key_ == GDK_KEY_Shift_L
			 || last_pressed_key_ == GDK_KEY_Shift_R
			 || last_pressed_key_ == GDK_KEY_Alt_L
			 || last_pressed_key_ == GDK_KEY_Alt_R
			 || last_pressed_key_ == GDK_KEY_Control_L
			 || last_pressed_key_ == GDK_KEY_Control_R
			)
			last_pressed_key_ = 0;
		if (last_pressed_key_ && handled == FALSE && last_pressed_key_ < 0x100)
			handled = TRUE;
		FIM_GTK_DBG_COUT << " handled=" << handled << "  last_pressed_key_=" << ((int)last_pressed_key_) << std::endl;
		// lots of garbage codes can still go in
	}
	return handled;
}

static gboolean cb_do_draw(GtkWidget *drawingarea, cairo_t * cr)
{
	// invoked by gtk_test_widget_wait_for_draw
	const fim_coo_t nw = gtk_widget_get_allocated_width(drawingarea),
			nh = gtk_widget_get_allocated_height(drawingarea);

	const bool nrsz = ( nw_ != nw || nh_ != nh );

	FIM_GTK_DBG_COUT << " " << nw_ << " " << nh_ << " " << (nrsz?'-':'=') <<  "-> " << nw << " " << nh << "  " << rowstride_/GTKDevice::Bpp_ << "\n";

	if ( !pixbuf || nrsz )
	{
		alloc_pixbuf(nw, nh);
		cc.display_resize(nw, nh);
	}

	gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
	cairo_paint (cr);
	return TRUE;
}

static void keys_setup(fim::sym_keys_t&sym_keys)
{
	// populate GDK key code -> FIM key code
	std::map<fim_key_t,std::string> key_syms;

	if (!key_syms.size())
	{
		key_syms[GDK_KEY_Page_Up] = FIM_KBD_PAGEUP;
		key_syms[GDK_KEY_Page_Down] = FIM_KBD_PAGEDOWN;
		key_syms[GDK_KEY_Left] = FIM_KBD_LEFT;
		key_syms[GDK_KEY_Right] = FIM_KBD_RIGHT;
		key_syms[GDK_KEY_Up] = FIM_KBD_UP;
		key_syms[GDK_KEY_Down] = FIM_KBD_DOWN;
		key_syms[GDK_KEY_space] = FIM_KBD_SPACE;
		key_syms[GDK_KEY_End] = FIM_KBD_END;
		key_syms[GDK_KEY_Home] = FIM_KBD_HOME;
		key_syms[GDK_KEY_BackSpace] = FIM_KBD_BACKSPACE;
		key_syms[GDK_KEY_Tab] = FIM_KBD_TAB;
		key_syms[GDK_KEY_Return] = FIM_KBD_ENTER;
		key_syms[GDK_KEY_Pause] = FIM_KBD_PAUSE;
		key_syms[GDK_KEY_Insert] = FIM_KBD_INS;
		key_syms[GDK_KEY_Delete] = FIM_KBD_DEL;
		key_syms[GDK_KEY_Menu] = FIM_KBD_MENU;
		key_syms[GDK_KEY_Control_L] = "LeftControl";
		key_syms[GDK_KEY_Control_R] = "RightControl";
		key_syms[GDK_KEY_Escape] = "Esc";
		key_syms[GDK_KEY_colon] = FIM_KBD_COLON;
		key_syms[GDK_KEY_plus] = FIM_KBD_PLUS;
		key_syms[GDK_KEY_minus] = FIM_KBD_MINUS;
		// key_syms[GDK_KEY_function] = "Fn";
		// see /usr/include/gtk-3.0/gdk/gdkkeysyms.h
		// TODO: there is redundancy around here.
		for (fim_key_t k = '!'; k <= '~' ; ++ k )
			key_syms[k] = k;
		for (fim_key_t k = GDK_KEY_F1; k <= GDK_KEY_F12; ++ k )
			key_syms[k] = "F" + std::to_string(1+k-GDK_KEY_F1);
		for (const auto & ks: key_syms)
			sym_keys[ks.second] = ks.first;
		cc.key_syms_update();
	}
}

fim_err_t GTKDevice::initialize(fim::sym_keys_t&sym_keys)
{
	gtk_init(NULL, NULL);
	window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window_), FIM_DEFAULT_WINDOW_WIDTH, FIM_DEFAULT_WINDOW_HEIGHT);
	set_wm_caption(wtitle_.c_str());
	gtk_widget_add_events(GTK_WIDGET(window_), GDK_BUTTON_PRESS_MASK);
	grid_ = gtk_grid_new();
	menubar_ = gtk_menu_bar_new();
#if !FIM_GTK_WITH_RENDERED_STATUSBAR
	statusbar_ = gtk_statusbar_new();
	gtk_widget_set_vexpand(GTK_WIDGET(statusbar_), FALSE);
	gtk_widget_set_hexpand(GTK_WIDGET(statusbar_), TRUE);
	gtk_widget_set_valign(GTK_WIDGET(statusbar_), GTK_ALIGN_END);
	gtk_widget_set_margin_top(GTK_WIDGET(statusbar_), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(statusbar_), 0);
	gtk_widget_set_margin_start(GTK_WIDGET(statusbar_), 0);
	gtk_widget_set_margin_end(GTK_WIDGET(statusbar_), 0);
#endif
	drawingarea_ = gtk_drawing_area_new();
	gtk_widget_set_vexpand(GTK_WIDGET(drawingarea_), TRUE);
	gtk_widget_set_hexpand(GTK_WIDGET(drawingarea_), TRUE);
#if !FIM_GTK_WITH_RENDERED_STATUSBAR
	context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar_), "context");
	gtk_statusbar_push (GTK_STATUSBAR(statusbar_), context_id, "Waiting for you to do something...");
	gtk_grid_attach(GTK_GRID(grid_), GTK_WIDGET(drawingarea_), 0, 1, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(grid_), GTK_WIDGET(statusbar_), GTK_WIDGET(drawingarea_), GTK_POS_BOTTOM, 1, 1);
#else
	gtk_grid_attach(GTK_GRID(grid_), GTK_WIDGET(drawingarea_), 0, 1, 1, 1);
#endif
	cmdline_entry_ = gtk_entry_new();
	// TODO: GTK-specific autocompletion
	gtk_widget_set_vexpand(GTK_WIDGET(cmdline_entry_), FALSE);
	gtk_widget_set_hexpand(GTK_WIDGET(cmdline_entry_), TRUE);
	gtk_widget_set_valign(GTK_WIDGET(cmdline_entry_), GTK_ALIGN_END);
	gtk_widget_set_margin_top(GTK_WIDGET(cmdline_entry_), 2); // to equalize heights of statusbar_ and cmdline_entry_
	gtk_widget_set_margin_bottom(GTK_WIDGET(cmdline_entry_), 3);
	gtk_widget_set_margin_start(GTK_WIDGET(cmdline_entry_), 0);
	gtk_widget_set_margin_end(GTK_WIDGET(cmdline_entry_), 0);
	gtk_entry_set_has_frame(GTK_ENTRY(cmdline_entry_), FALSE);
#if !FIM_GTK_WITH_RENDERED_STATUSBAR
	gtk_grid_attach_next_to(GTK_GRID(grid_), GTK_WIDGET(cmdline_entry_), GTK_WIDGET(statusbar_), GTK_POS_BOTTOM, 1, 1);
#endif
	g_signal_connect(G_OBJECT(window_), "destroy", G_CALLBACK(
			[]()
			{
				g_object_unref(pixbuf);
				pixbuf = NULL;
				gtk_container_remove (GTK_CONTAINER(grid_), drawingarea_);
				drawingarea_ = NULL;
				last_pressed_key_ = cc.find_keycode_for_bound_cmd(FIM_FLT_QUIT);
			}
		), NULL);
	gtk_container_add(GTK_CONTAINER(window_), grid_);

	g_signal_connect(G_OBJECT(window_), "key-press-event", G_CALLBACK(cb_key_pressed), NULL);
	g_signal_connect(G_OBJECT(window_), "key-release-event", G_CALLBACK(cb_key_pressed), NULL);
	g_signal_connect(G_OBJECT(window_), "window-state-event", G_CALLBACK(cb_window_state_event), NULL);
	g_signal_connect(G_OBJECT(window_), "configure-event", G_CALLBACK(cb_window_event), NULL);
	g_signal_connect(G_OBJECT(drawingarea_), "draw", G_CALLBACK(cb_do_draw), NULL);
	// TODO: handle events in drawingarea_ and cmdline_entry_

	accel_group_ = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window_), accel_group_);
	keys_setup(sym_keys);
	// TODO: need to populate menus and if needed, rebuild them
	gtk_widget_show_all(GTK_WIDGET(window_));
	gtk_widget_hide(cmdline_entry_);

	gtk_test_widget_wait_for_draw(GTK_WIDGET(window_)); // to get proper window size
	FIM_GTK_DBG_COUT << "\n";
	return FIM_ERR_NO_ERROR;
}

static fim_sys_int get_input_inner(fim_key_t * c, GdkEventKey*eventk, fim_sys_int *keypressp, bool want_poll)
{
	if ( last_pressed_key_ )
	{
		FIM_GTK_DBG_COUT << "PRESSED " << last_pressed_key_ << " \n";
	}

	gtk_main_iteration();

	if ( last_pressed_key_ )
	{
		* c = last_pressed_key_;
		last_pressed_key_ = 0; // to emit once is enough
		return 1;
	}
	return 0;
}

fim_coo_t GTKDevice::width() const
{
	// FIXME: temporary
	return gtk_widget_get_allocated_width((GtkWidget*)drawingarea_);
}

fim_coo_t GTKDevice::height() const
{
	// FIXME: temporary
	fim_coo_t h = 0;
	if(drawingarea_)
		h = gtk_widget_get_allocated_height((GtkWidget*)drawingarea_);
	return h;
}

fim_sys_int GTKDevice::get_input(fim_key_t * c, bool want_poll)
{
	int keypress_ = 0;
	GdkEventKey event_;
	const fim_sys_int iv = get_input_inner(c,&event_,&keypress_,want_poll);
	return iv;
}

fim_key_t GTKDevice::catchInteractiveCommand(fim_ts_t seconds)const
{
	/*
	 * after SDLDevice::catchInteractiveCommand()
	 * */
	fim_key_t c=0;
	fim_sys_int lkeypress=0;
	fim_tms_t sms=10,ms=seconds*1000;// sms: sleep ms
	FIM_GTK_INPUT_DEBUG(&c,"");

	if ( gtk_events_pending() )
	{
		gtk_main_iteration();
		goto done;
	}
	else
	do
	{
		if(ms>0)
			usleep((useconds_t)(sms*1000)),ms-=sms;
		// we read input twice: it seems we have a number of "spurious" inputs. 
	}
	while(ms>0);
	return -1;
done:
	usleep((useconds_t)(sms*1000)),ms-=sms;
	return c;
}

fim_key_t GTKDevice::set_wm_caption(const fim_char_t *msg)
{
	const fim_err_t rc = FIM_ERR_NO_ERROR;
	gtk_window_set_title(window_, msg);
	return rc;
}

fim_coo_t GTKDevice::get_chars_per_line() const
{
	return width() / f_->swidth();
}

fim_coo_t GTKDevice::get_chars_per_column(void) const
{
	return height() / f_->sheight();
}

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
GTKDevice::GTKDevice(MiniConsole& mc_, fim::string opts): DisplayDevice (mc_)
#else /* FIM_WANT_NO_OUTPUT_CONSOLE */
GTKDevice::GTKDevice(fim::string opts):DisplayDevice()
#endif /* FIM_WANT_NO_OUTPUT_CONSOLE */
{
	FontServer::fb_text_init1(fontname_,&f_);
}

fim_err_t GTKDevice::status_line(const fim_char_t *msg)
{
		// FIXME: need to unify and cleanup together with SDL's and other versions
#if FIM_GTK_WITH_RENDERED_STATUSBAR
		fim_err_t errval = FIM_ERR_NO_ERROR;
		fim_coo_t y,ys=3;// FIXME
		if(get_chars_per_column()<1)
			goto done;
		y = height() - f_->sheight() - ys;
		if(y<0 )
			goto done;
		clear_rect(0, width()-1, y+1,y+f_->sheight()+ys-1);
		fs_puts(f_, 0, y+ys, msg);
		fill_rect(0,width()-1, y, y, FIM_CNS_WHITE);
done:
		gtk_widget_queue_draw(GTK_WIDGET(drawingarea_));
		return errval;
#else
	gtk_statusbar_push(GTK_STATUSBAR(statusbar_), context_id, msg);
	FIM_GTK_DBG_COUT << ":" << msg << "\n";
	return FIM_ERR_NO_ERROR;
#endif
}

fim_err_t GTKDevice::display(
	const void *ida_image_img, // source image structure
	fim_coo_t iroff,fim_coo_t icoff, // row and column offset of the first input pixel
	fim_coo_t irows,fim_coo_t icols,// rows and columns in the input image
	fim_coo_t icskip,	// input columns to skip for each line
	fim_coo_t oroff,fim_coo_t ocoff,// row and column offset of the first output pixel
	fim_coo_t orows,fim_coo_t ocols,// rows and columns to draw in output buffer
	fim_coo_t ocskip,// output columns to skip for each line
	fim_flags_t flags// some flags
)
{
	const fim_coo_t ww_ = width();
	const fim_coo_t wh_ = height();

	FIM_GTK_DBG_COUT << "DISPLAY  ocskip:" << ocskip << "  ocols:" << ocols << " rowstride_/Bpp:" << rowstride_/Bpp_ << "  " << ((rowstride_) == (Bpp_*ocskip)) << "\n";

	fim_byte_t* rgb = ida_image_img?((const struct ida_image*)ida_image_img)->data:FIM_NULL;// source rgb array

	if( iroff <0 ) return -2;
	if( icoff <0 ) return -3;
	if( irows <=0 ) return -4;
	if( icols <=0 ) return -5;
	if( icskip<0 ) return -6;
	if( oroff <0 ) return -7;
	if( ocoff <0 ) return -8;
	if( orows <=0 ) return -9;
	if( ocols <=0 ) return -10;
	if( ocskip<0 ) return -11;
	if( flags <0 ) return -12;

	if( iroff>irows ) return -2-3*100 ;
	if( icoff>icols ) return -3-5*100;
//	if( oroff>orows ) return -7-9*100;//EXP
//	if( ocoff>ocols ) return -8-10*100;//EXP
	if( oroff>height() ) return -7-9*100;//EXP
	if( ocoff>width()) return -8-10*100;//EXP

	if( icskip<icols ) return -6-5*100;
	if( ocskip<ocols ) return -11-10*100;
	
	orows  = FIM_MIN( orows, height());
	ocols  = FIM_MIN( ocols,  width()); 
	ocskip = width(); 	//FIXME maybe this is not enough and should be commented or rewritten!

	if( orows  > height() ) return -9 -99*100;
	if( ocols  >  width() ) return -10-99*100;
	if( ocskip <  width() ) return -11-99*100;
	if( icskip<icols ) return -6-5*100;

	ocskip = width();// output columns to skip for each line

	if(icols<ocols) { ocoff+=(ocols-icols-1)/2; ocols-=(ocols-icols-1)/2; }
	if(irows<orows) { oroff+=(orows-irows-1)/2; orows-=(orows-irows-1)/2; }

	fim_coo_t ytimesw;

	fim_coo_t idr,idc,lor,loc;

	idr = iroff-oroff;
	idc = icoff-ocoff;

	lor = oroff+(FIM_MIN(orows,irows-iroff));
	loc = ocoff+(FIM_MIN(ocols,icols-icoff));

	fim_coo_t ii,ij;
	fim_coo_t oi,oj;
	fim_flags_t mirror=flags&FIM_FLAG_MIRROR, flip=flags&FIM_FLAG_FLIP;//STILL UNUSED : FIXME
	fim_byte_t * srcp;

	clear_rect(  0, width()-1, 0, height()-1); 

	for(oi=oroff;FIM_LIKELY(oi<lor);++oi)
	for(oj=ocoff;FIM_LIKELY(oj<loc);++oj)
	{
		ii    = oi + idr;
		ij    = oj + idc;
		
		if(mirror)ij=((icols-1)-ij);
		if( flip )ii=((irows-1)-ii);
		srcp  = ((fim_byte_t*)rgb)+(Bpp_*(ii*icskip+ij));

		gdk_pixbuf_get_pixels(pixbuf)[rowstride_*oi+Bpp_*(oj)+0]=srcp[2];
		gdk_pixbuf_get_pixels(pixbuf)[rowstride_*oi+Bpp_*(oj)+1]=srcp[1];
		gdk_pixbuf_get_pixels(pixbuf)[rowstride_*oi+Bpp_*(oj)+2]=srcp[0];
	}

	gtk_widget_queue_draw(GTK_WIDGET(window_));
	return FIM_ERR_NO_ERROR;
}

	fim_coo_t GTKDevice::status_line_height(void)const
	{
		return f_ ? border_height_ + f_->sheight() : 0;
	}


	fim_err_t GTKDevice::clear_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2) FIM_NOEXCEPT
	{
		fim_coo_t y;
		/*
		 * */
		fim_byte_t* rgb = ((fim_byte_t*)(gdk_pixbuf_get_pixels(pixbuf)));

		for(y=y1;y<=y2;++y)
		{
			fim_bzero(rgb + y*(rowstride_) + x1*Bpp_, (x2-x1+1)* Bpp_);
		}
		return FIM_ERR_NO_ERROR;
	}

fim_err_t GTKDevice::fs_puts(struct fs_font *f_, fim_coo_t x, fim_coo_t y, const fim_char_t *str) FIM_NOEXCEPT
{
    fim_sys_int i,c/*,j,w*/;

    for (i = 0; str[i] != '\0'; i++) {
	c = (fim_byte_t)str[i];
	if (FIM_NULL == f_->eindex[c])
	    continue;
	/* clear with bg color */
//	w = (f_->eindex[c]->width+1)*Bpp_;
#if 0
#ifdef FIM_IS_SLOWER_THAN_FBI
	for (j = 0; j < f_->sheight(); j++) {
/////	    memset_combine(start,0x20,w);
	    fim_bzero(start,w);
	    start += fb_fix.line_length;
	}
#else
	//sometimes we can gather multiple calls..
	if(fb_fix.line_length==w)
	{
		//contiguous case
		fim_bzero(start,w*f_->sheight());
	    	start += fb_fix.line_length*f_->sheight();
	}
	else
	for (j = 0; j < f_->sheight(); j++) {
	    fim_bzero(start,w);
	    start += fb_fix.line_length;
	}
#endif
#endif
	/* draw character */
	//fs_render_fb(fb_fix.line_length,f_->eindex[c],f_->gindex[c]);
	fs_render_fb(x,y,f_->eindex[c],f_->gindex[c]);
	x += f_->eindex[c]->swidth();
	/* FIXME : SLOW ! */
	if (((fim_coo_t)x) > width() - f_->swidth())
		goto err;
    }
    // FIXME
	return FIM_ERR_NO_ERROR;
err:
	return FIM_ERR_GENERIC;
}

void GTKDevice::fs_render_fb(fim_coo_t x_, fim_coo_t y, FSXCharInfo *charInfo, fim_byte_t *data) FIM_NOEXCEPT
{
	// FIXME: unify this with other driver's versions
/* 
 * These preprocessor macros should serve *only* for font handling purposes.
 * */
#define BIT_ORDER       BitmapFormatBitOrderMSB
#ifdef BYTE_ORDER
#undef BYTE_ORDER
#endif
#define BYTE_ORDER      BitmapFormatByteOrderMSB
#define SCANLINE_UNIT   BitmapFormatScanlineUnit8
#define SCANLINE_PAD    BitmapFormatScanlinePad8
#define EXTENTS         BitmapFormatImageRectMin

#define SCANLINE_PAD_BYTES 1
#define GLWIDTHBYTESPADDED(bits, nBytes)                                    \
        ((nBytes) == 1 ? (((bits)  +  7) >> 3)          /* pad to 1 byte  */\
        :(nBytes) == 2 ? ((((bits) + 15) >> 3) & ~1)    /* pad to 2 bytes */\
        :(nBytes) == 4 ? ((((bits) + 31) >> 3) & ~3)    /* pad to 4 bytes */\
        :(nBytes) == 8 ? ((((bits) + 63) >> 3) & ~7)    /* pad to 8 bytes */\
        : 0)

	fim_coo_t row,bit,x;
	FIM_CONSTEXPR Uint8 rc = 0xff, gc = 0xff, bc = 0xff;
	// const Uint8 rc = 0x00, gc = 0x00, bc = 0xff;
	const fim_sys_int bpr = GLWIDTHBYTESPADDED((charInfo->right - charInfo->left), SCANLINE_PAD_BYTES);
	const Uint16 incr = rowstride_ / Bpp_;

	fim_byte_t* rgb = ((fim_byte_t*)(gdk_pixbuf_get_pixels(pixbuf)));
	for (row = 0; row < (charInfo->ascent + charInfo->descent); row++)
	{
		for (x = 0, bit = 0; bit < (charInfo->right - charInfo->left); bit++, x++) 
		{
			if (data[bit>>3] & fs_masktab[bit&7])
			{	// WARNING !
#if FIM_FONT_MAGNIFY_FACTOR == 0
				const fim_int fim_fmf = fim::fim_fmf_; 
#endif	/* FIM_FONT_MAGNIFY_FACTOR */
#if FIM_FONT_MAGNIFY_FACTOR <  0
				fim_int fim_fmf = fim::fim_fmf_; 
#endif	/* FIM_FONT_MAGNIFY_FACTOR */
#if FIM_FONT_MAGNIFY_FACTOR == 1
				setpixel(rgb,x_+x,(y+row),rc,gc,bc);
#else	/* FIM_FONT_MAGNIFY_FACTOR */
				for(fim_coo_t mi = 0; mi < fim_fmf; ++mi)
				for(fim_coo_t mj = 0; mj < fim_fmf; ++mj)
					setpixel(rgb,x_+fim_fmf*x+mj,(y+fim_fmf*row+mi),rc,gc,bc);
#endif	/* FIM_FONT_MAGNIFY_FACTOR */
			}
		}
		data += bpr;
	}

#undef BIT_ORDER
#undef BYTE_ORDER
#undef SCANLINE_UNIT
#undef SCANLINE_PAD
#undef EXTENTS
#undef SCANLINE_PAD_BYTES
#undef GLWIDTHBYTESPADDED
}

void GTKDevice::setpixel(fim_byte_t* rgb, fim_coo_t x, fim_coo_t y, fim_byte_t r, fim_byte_t g, fim_byte_t b)
{
	// style modified from other drivers; would need some unity and cleanliness though
	rgb += y * rowstride_ + x * Bpp_;
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

fim_err_t GTKDevice::fill_rect(fim_coo_t x1, fim_coo_t x2, fim_coo_t y1,fim_coo_t y2, fim_color_t color) FIM_NOEXCEPT
{
	fim_coo_t y;
	/*
	 * This could be optimized
	 * */
	fim_byte_t* rgb = ((fim_byte_t*)(gdk_pixbuf_get_pixels(pixbuf)));

	for(y=y1;y<=y2;++y)
	{
		fim_memset(rgb + y*rowstride_ + x1*Bpp_,color, (x2-x1+1)* Bpp_);
	}
	return FIM_ERR_NO_ERROR;
}

	fim_err_t GTKDevice::reinit(const fim_char_t *rs)
	{
		FIM_GTK_DBG_COUT << ":" << rs << "\n";

		if (strchr(rs, 'W'))
			full_screen_=1;
		else
			full_screen_=0;
		toggle_fullscreen(full_screen_);

		if ( cc.display_resize(nw_,nh_) == FIM_ERR_NO_ERROR )
		{
			// opts_ = rs;
			return FIM_ERR_NO_ERROR;
		}
		return FIM_ERR_GENERIC;
	}

#endif /* FIM_WITH_LIBGTK */
#pragma GCC pop_options
