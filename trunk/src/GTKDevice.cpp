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
#include "fim.h"

#ifdef FIM_WITH_LIBGTK

#include "DisplayDevice.h"
#include "GTKDevice.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkcairo.h>

#define FIM_GTK_DEBUG 1
#undef FIM_GTK_DEBUG

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
	GtkWidget *statusbar_{};
	GtkWidget *drawingarea_{};
	guint context_id{};
	GtkWidget *cmdline_entry_{};
	GtkAccelGroup *accel_group_{};
	const std::string wtitle_ {"GTK-FIM -- no status yet"};
	bool control_pressed_{false};
	fim_key_t last_pressed_key_{0};
	GdkPixbuf * pixbuf{};

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"

void alloc_pixbuf(int nw, int nh)
{
	const auto alpha = FALSE; /* in gmacros.h */

	if(pixbuf)
		g_object_unref(pixbuf);

	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, alpha, 8 /*get_bpp()*/, nw, nh);
	memset(gdk_pixbuf_get_pixels(pixbuf), 0x0, 3*nw*nh); // black
}

static gboolean cb_key_pressed(GtkWidget *window__unused, GdkEventKey* event)
{
	gboolean handled = TRUE;
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
	}
	else
		handled = FALSE;
	return handled;
}

static gboolean cb_do_draw(GtkWidget *drawingarea, cairo_t * cr)
{
	// invoked by gtk_test_widget_wait_for_draw
	const fim_coo_t nw = gtk_widget_get_allocated_width(drawingarea),
			nh = gtk_widget_get_allocated_height(drawingarea);

	if (!pixbuf || gdk_pixbuf_get_width(pixbuf) != gtk_widget_get_allocated_width(drawingarea) 
	            || gdk_pixbuf_get_height(pixbuf) != gtk_widget_get_allocated_height(drawingarea) )
		alloc_pixbuf(nw,nh);

	gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
	cairo_paint (cr);
	return FALSE;
}

fim_err_t GTKDevice::initialize(fim::sym_keys_t&)
{
	gtk_init(NULL, NULL);
	window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window_), FIM_DEFAULT_WINDOW_WIDTH, FIM_DEFAULT_WINDOW_HEIGHT);
	set_wm_caption(wtitle_.c_str());
	gtk_widget_add_events(GTK_WIDGET(window_), GDK_BUTTON_PRESS_MASK);
	grid_ = gtk_grid_new();
	menubar_ = gtk_menu_bar_new();
	statusbar_ = gtk_statusbar_new();
	gtk_widget_set_vexpand(GTK_WIDGET(statusbar_), FALSE);
	gtk_widget_set_hexpand(GTK_WIDGET(statusbar_), TRUE);
	gtk_widget_set_valign(GTK_WIDGET(statusbar_), GTK_ALIGN_END);
	gtk_widget_set_margin_top(GTK_WIDGET(statusbar_), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(statusbar_), 0);
	gtk_widget_set_margin_start(GTK_WIDGET(statusbar_), 0);
	gtk_widget_set_margin_end(GTK_WIDGET(statusbar_), 0);
	drawingarea_ = gtk_drawing_area_new();
	gtk_widget_set_vexpand(GTK_WIDGET(drawingarea_), TRUE);
	gtk_widget_set_hexpand(GTK_WIDGET(drawingarea_), TRUE);
	context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar_), "context");
	gtk_statusbar_push (GTK_STATUSBAR(statusbar_), context_id, "Waiting for you to do something...");
	gtk_grid_attach(GTK_GRID(grid_), GTK_WIDGET(drawingarea_), 0, 1, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(grid_), GTK_WIDGET(statusbar_), GTK_WIDGET(drawingarea_), GTK_POS_BOTTOM, 1, 1);
	cmdline_entry_ = gtk_entry_new();
	gtk_widget_set_vexpand(GTK_WIDGET(cmdline_entry_), FALSE);
	gtk_widget_set_hexpand(GTK_WIDGET(cmdline_entry_), TRUE);
	gtk_widget_set_valign(GTK_WIDGET(cmdline_entry_), GTK_ALIGN_END);
	gtk_widget_set_margin_top(GTK_WIDGET(cmdline_entry_), 2); // to equalize heights of statusbar_ and cmdline_entry_
	gtk_widget_set_margin_bottom(GTK_WIDGET(cmdline_entry_), 3);
	gtk_widget_set_margin_start(GTK_WIDGET(cmdline_entry_), 0);
	gtk_widget_set_margin_end(GTK_WIDGET(cmdline_entry_), 0);
	gtk_entry_set_has_frame(GTK_ENTRY(cmdline_entry_), FALSE);
	gtk_grid_attach_next_to(GTK_GRID(grid_), GTK_WIDGET(cmdline_entry_), GTK_WIDGET(statusbar_), GTK_POS_BOTTOM, 1, 1);
	gtk_container_add(GTK_CONTAINER(window_), grid_);

	g_signal_connect(G_OBJECT(window_), "key-press-event", G_CALLBACK(cb_key_pressed), NULL);
	g_signal_connect(G_OBJECT(window_), "key-release-event", G_CALLBACK(cb_key_pressed), NULL);
	g_signal_connect(G_OBJECT(drawingarea_), "draw", G_CALLBACK(cb_do_draw), NULL);

	accel_group_ = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window_), accel_group_);
	gtk_widget_show_all(GTK_WIDGET(window_));
	gtk_widget_hide(cmdline_entry_);

	gtk_test_widget_wait_for_draw(GTK_WIDGET(window_)); // to get proper window size
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
	return gtk_widget_get_allocated_height((GtkWidget*)drawingarea_);
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
	gtk_statusbar_push(GTK_STATUSBAR(statusbar_), context_id, msg);
	return FIM_ERR_NO_ERROR;
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

	FIM_GTK_DBG_COUT << "DISPLAY\n";

	gint Bpp_{get_bpp()} ;

	if (!pixbuf || gdk_pixbuf_get_width(pixbuf) != gtk_widget_get_allocated_width(drawingarea_) 
	            || gdk_pixbuf_get_height(pixbuf) != gtk_widget_get_allocated_height(drawingarea_) )
		alloc_pixbuf(ww_,wh_);

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
	memset(gdk_pixbuf_get_pixels(pixbuf),255,3*width()*height()); // white background, temporary (FIXME)

	for(oi=oroff;FIM_LIKELY(oi<lor);++oi)
	for(oj=ocoff;FIM_LIKELY(oj<loc);++oj)
	{
		ii    = oi + idr;
		ij    = oj + idc;
		
		if(mirror)ij=((icols-1)-ij);
		if( flip )ii=((irows-1)-ii);
		srcp  = ((fim_byte_t*)rgb)+(3*(ii*icskip+ij));

		gdk_pixbuf_get_pixels(pixbuf)[3*(oi*ocskip+oj)+0]=srcp[2];
		gdk_pixbuf_get_pixels(pixbuf)[3*(oi*ocskip+oj)+1]=srcp[1];
		gdk_pixbuf_get_pixels(pixbuf)[3*(oi*ocskip+oj)+2]=srcp[0];
	}

	gtk_widget_queue_draw(GTK_WIDGET(window_));
	return FIM_ERR_NO_ERROR;
}
#endif /* FIM_WITH_LIBGTK */
#pragma GCC pop_options
