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

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
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

	accel_group_ = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window_), accel_group_);
	gtk_widget_show_all(GTK_WIDGET(window_));
	gtk_widget_hide(cmdline_entry_);

	gtk_main();

	return 0;
}

#endif /* FIM_WITH_LIBGTK */
#pragma GCC pop_options
