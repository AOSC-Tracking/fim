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
	fim_key_t fim_key_{0};

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"

static gboolean cb_key_pressed(GtkWidget *window__unused, GdkEventKey* event)
{
	gboolean handled = TRUE;
	fim_key_ = 0;
	if (event->keyval == GDK_KEY_Control_L)
	{
		if (event->type == GDK_KEY_RELEASE)
			control_pressed_ = true;
		if (event->type == GDK_KEY_PRESS)
			control_pressed_ = false;
	}
	if ((event->type == GDK_KEY_PRESS) /*&& (event->state & GDK_CONTROL_MASK)*/)
	{
		std::string kst;

		if ( event->state & GDK_CONTROL_MASK )
			kst += "C-";
		fim_key_ = event->keyval;
	}
	else
		handled = FALSE;
	return handled;
}

fim_err_t GTKDevice::initialize(fim::sym_keys_t&)
{
	gtk_init(NULL, NULL);
	window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window_), FIM_DEFAULT_WINDOW_WIDTH, FIM_DEFAULT_WINDOW_HEIGHT);
	gtk_window_set_title(GTK_WINDOW(window_), wtitle_.c_str());
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

	accel_group_ = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window_), accel_group_);
	gtk_widget_show_all(GTK_WIDGET(window_));
	gtk_widget_hide(cmdline_entry_);

	return FIM_ERR_NO_ERROR;
}

static fim_sys_int get_input_inner(fim_key_t * c, GdkEventKey*eventk, fim_sys_int *keypressp, bool want_poll)
{
	if ( fim_key_ )
	{
		FIM_GTK_DBG_COUT << "PRESSED " << fim_key_ << " \n";
	}

	gtk_main_iteration();

	if ( fim_key_ )
	{
		* c = fim_key_;
		return 1;
	}
	return 0;
}

fim_coo_t GTKDevice::width() const
{
	// FIXME: temporary
	return gtk_widget_get_allocated_width((GtkWidget*)window_);
}

fim_coo_t GTKDevice::height() const
{
	// FIXME: temporary
	return gtk_widget_get_allocated_height((GtkWidget*)window_);
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


#endif /* FIM_WITH_LIBGTK */
#pragma GCC pop_options
