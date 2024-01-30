/*
 SDLDevice.h : GTK device Fim driver implementation file

 (c) 2023-2024 Michele Martone

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
// - in command line mode, clicking on menus may input command there
// - full screen mode to hide scrolldown menu
// - update and sync widgets with vars' value
// - clean up FIM's finalization with GTK's finalization
// - radio buttons group update
// - shall menu commands be allowed while in the console mode, or not? (FIM_GTK_ALLOW_MENU_IN_CONSOLE)
// - auto sync of menus: variables, commands, aliases... (see FIM_GTK_WITH_VARS_SYNC)
// - needs menu with i:variables
// - commands should not be executed in cb_cc_exec, but only queued, actually (resetting last_pressed_key_ only a dirty fixup)
// - ...

#include <map>
#include <tuple>
#include "fim.h"

#ifdef FIM_WITH_LIBGTK

#include "DisplayDevice.h"
#include "GTKDevice.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkcairo.h>

#define FIM_GTK_DEBUG 1
#undef FIM_GTK_DEBUG /* comment this to activate debug output */

#define FIM_GTK_WITH_RENDERED_STATUSBAR 1 /* 1 to render, 0 to use statusbar_ */
#define FIM_GTK_WITH_MENUBAR 1
#define FIM_GTK_WITH_RELATIVE_LIMIT_MENUS 1
#define FIM_GTK_WITH_SYSTEM_INVOCATION 0
#define FIM_GTK_WITH_MENU_EDITING_DIALOG 0
#define FIM_GTK_ALLOW_MENU_IN_CONSOLE 1
#define FIM_GTK_WITH_SHORT_MENUSPEC 1
#define FIM_GTK_WITH_VARS_SYNC 0

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
#define FIM_GTK_RESPONSE_ACCEPT_REC_FG (-GTK_RESPONSE_ACCEPT+0)
#define FIM_GTK_RESPONSE_ACCEPT_REC_BG (-GTK_RESPONSE_ACCEPT+1) /* yet unused */

#define FIM_GTK_TP char
#define FIM_GTK_LCI (((FIM_GTK_TP*)czptr_)+(asv_.size()-1))
#define FIM_GTK_P2I(P) ((FIM_GTK_TP*)(P)-((FIM_GTK_TP*)czptr_))
#define FIM_GTK_P2S(IDXP) asv_[FIM_GTK_P2I(IDXP)].c_str()
#define FIM_GTK_ASVA(CMD) asv_.push_back(CMD)

#define FIM_GTK_IS_TOGGLE(X) (X==strstr(X, "toggle")) && X[6] && !isalnum(X[6]) && (X[6]!=' ') && X[6]==X[7] // toggle__ toggle:: ...

namespace fim
{
	extern fim_int fim_fmf_; /* FIXME */
	extern CommandConsole cc;
}

#define interactive_mode_ 0

	// here temporarily
	GtkWindow *window_{};
	GtkWidget *grid_{};
#if FIM_GTK_WITH_MENUBAR
	GtkWidget *menubar_{};
	std::map<std::string,GtkMenuItem*> menu_items_; // TODO: broken in GTK4: https://docs.gtk.org/gtk4/class.PopoverMenu.html
#if FIM_GTK_WITH_VARS_SYNC
	std::map<std::string,GtkWidget*> var_menu_items_;
#endif /* FIM_GTK_WITH_VARS_SYNC */
	std::map<std::string,std::set<GtkWidget*>> check_menu_items_; // TODO: may bring problems when actualizing
	std::map<std::string,std::set<GtkWidget*>> radio_menu_items_; // TODO: may bring problems when actualizing
	std::map<GSList*,std::set<GtkWidget*>> group_widgets_; // TODO: may bring problems when actualizing
	std::map<std::string,GtkMenu*> menus_;

	GtkWidget *lmitMenu_{}; // TODO: FIXME: temporary
	std::vector<GtkWidget *> lms3Miv{4}; // TODO: FIXME: temporary
	int hist_n_{}; // TODO: FIXME: temporary
	std::map<std::string,std::string> cmd_to_seq_; // TODO: FIXME: temporary
	std::map<std::string,void(*)(void)> cmd_funcs_; // TODO: FIXME: temporary
	std::map<std::string,std::string> help_; // TODO: FIXME: temporary
	CommandConsole::bindings_t bindings_;
	std::string aliases_;
	std::string commands_;
	std::string variables_;
	std::map<std::string,fim_key_t> sym_keys_; // TODO: FIXME: temporary
	GtkWidget *varsMenu_{}; /* a special menu with all commands, autogenerated via 'FimMenuVariables' */ // TODO: FIXME
	GtkWidget *aliaMenu_{}; /* a special menu with all commands, autogenerated via 'FimMenuAliases' */ // TODO: FIXME
	GtkWidget *keysMenu_{}; /* a special menu with all commands, autogenerated via 'FimMenuKeyBindings' */ // TODO: FIXME
#endif /* FIM_GTK_WITH_MENUBAR */
#if !FIM_GTK_WITH_RENDERED_STATUSBAR
	GtkWidget *statusbar_{};
#endif
	GtkWidget *drawingarea_{};
	guint context_id{};
	GtkWidget *cmdline_entry_{};
	GtkListStore *store_{};
	GtkTreeIter iter_{};
	GtkAccelGroup *accel_group_{};
	const std::string wtitle_ {"GTK-FIM -- no status yet"};
	bool control_pressed_{false};
	bool alt_pressed_{false};
	fim_key_t last_pressed_key_{0};
	GdkPixbuf * pixbuf{};
	fim_coo_t rowstride_{};
	fim_coo_t nw_{}, nh_{};
	int full_screen_{};
	int show_menubar_{1};

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
			control_pressed_ = false;
		if (event->type == GDK_KEY_PRESS)
			control_pressed_ = true;
	}

	if ( ! (event->state & GDK_MOD1_MASK ) )
			alt_pressed_ = false; // e.g. when coming back from Alt-Tab

	if (event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R)
	{
		if (event->type == GDK_KEY_RELEASE)
			alt_pressed_ = false;
		if (event->type == GDK_KEY_PRESS)
			alt_pressed_ = true;
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
		if (handled == FALSE && ((last_pressed_key_ && last_pressed_key_ < 0x100) || cc.inConsole()))
			handled = TRUE;
		// lots of garbage codes can still go in
	}
	if (alt_pressed_ && !cc.inConsole()) // in console aka readline mode Alt is OK
		handled = FALSE, // in interactive mode we pass Alt to the window system, to handle menus
		last_pressed_key_ = 0;
	FIM_GTK_DBG_COUT << " handled=" << handled << " last_pressed_key_=" << ((int)last_pressed_key_) << " alt_pressed_=" << alt_pressed_ << " inConsole()=" << cc.inConsole() << std::endl;
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
		FIM_GTK_DBG_COUT << gtk_widget_get_allocated_height((GtkWidget*)window_) << " " << gtk_widget_get_allocated_height(drawingarea) << "\n"; // FIXME: display_resize is messy
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

#if FIM_GTK_WITH_MENUBAR
static std::vector<std::string> menuspecs_;
static std::vector<std::string> asv_; // actionable signals' vector with indices persistent after e.g. resize's following menu adds, where location of strings not guaranteed to be stable, e.g. because of SSO
static const void * czptr_ {nullptr};

int verbose_ = 0;
int verbose_specs_ = 0;

typedef std::pair<fim_key_t,GdkModifierType> km_t;

static km_t str_to_km(const char *s)
{
	const GdkModifierType no_kmod {( GdkModifierType )0}; /* no constant for 0 otherwise in Gdk3 */
	GdkModifierType mmask {no_kmod};

	if ( strlen(s) >= 3 && s[0] == 'C' && s[1] == '-' )
		mmask = GDK_CONTROL_MASK,
		s += 2;

	if ( sym_keys_.find(s) != sym_keys_.end() )
		return km_t {sym_keys_[s],mmask};
	else
		return km_t {'\0',mmask};
}

static const char *sym_strstr(const char *haystack, const char *needle)
{
	const int nl = strlen(needle);
	const char * s = strstr(haystack, needle);

	if ( s && ( !s[nl] || !isalpha(s[nl]) ) )
		return s;
	else
		return NULL;
}

static std::string do_get_item_help(const char* item)
{
#if FIM_GTK_WITH_VARS_SYNC
	const auto ghc = 'l';
#else /* FIM_GTK_WITH_VARS_SYNC */
	const auto ghc = 'L';
#endif /* FIM_GTK_WITH_VARS_SYNC */
	const auto ih = cc.get_help(item, ghc);
	return ih;
}

static std::string xtrcttkn(const char* cmd, const char sep=' ')
{
	// extract token from longer C string into string
	if (sep)
	{
		const auto sc = strchr(cmd, sep);
		std::string cmds;
		cmds = std::string(cmd, sc ? sc : cmd+strlen(cmd));
		return cmds;
	}
	else
	{
		const auto sc = strstr(cmd, "  ");
		std::string cmds;
		cmds = std::string(cmd, sc ? sc : cmd + strlen(cmd));
		return cmds;
	}
}

static void do_force_console()
{
	cc.execute("set", {"_display_console", "0" }, true);
	cc.execute("toggleVerbosity", {}, true);
}

static void do_print_item_help(GtkWidget *, const int * idxp)
{
	const auto item = FIM_GTK_P2S(idxp);
	cc.execute("echo", { do_get_item_help(xtrcttkn(item).c_str()) } ); // TODO: FIXME: this goes better to status, as long as in interactive mode
	do_force_console();	
}

static gboolean cb_open_file( GtkMenuItem*, GtkFileChooserAction);


using toggle_choices_t = std::tuple<std::string,std::string,std::string>;

static toggle_choices_t get_toggle_choices(const std::string & cmd)
{
	const auto sep = cmd.substr(6,2); // XX in toggleXX
	const auto s1p = cmd.find(sep);
	const auto wid = std::string(cmd,0, s1p);
	const auto s2p = cmd.find(sep, s1p+2);
	const auto vid = std::string(cmd, s1p+2, s2p-s1p-2);
	const auto s3p = cmd.find(sep, s2p+2);
	const auto vv1 = std::string(cmd, s2p+2, s3p-s2p-2);
	const auto s4p = cmd.find(sep, s3p+2);
	const auto vv2 = std::string(cmd, s3p+2, s4p-s3p-2);

	return {vid,vv1,vv2};
}

static void sync_toggle_menu(const std::string cmd)
{
	const toggle_choices_t tct = get_toggle_choices(cmd);
	const std::string & vid = std::get<0>(tct);
	const std::string & val1 = std::get<1>(tct);
	const std::string & val2 = std::get<2>(tct);

	if ( check_menu_items_.find(cmd) != check_menu_items_.end() )
		for ( const auto & cmi : check_menu_items_[cmd] )
		{
			const auto val = cc.getStringVariable(vid);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(cmi), (val == val1 ? TRUE : FALSE));
		}
	return;		
}

static std::string get_tooltip_from_toggle_spec(const std::string cmd)
{
	const toggle_choices_t tct = get_toggle_choices(cmd);
	const std::string & vid = std::get<0>(tct);
	const std::string & val1 = std::get<1>(tct);
	const std::string & val2 = std::get<2>(tct);

	return vid + "=" + val1 + "\n" + vid + "=" + val2;
}

static bool is_valid_toggle_spec(const std::string cmd)
{
	const toggle_choices_t tct = get_toggle_choices(cmd);
	const std::string & val1 = std::get<1>(tct);
	const std::string & val2 = std::get<2>(tct);

	return val1 != val2;		
}

static void sync_toggle_menus()
{
	for ( const auto & cmi : check_menu_items_ )
		sync_toggle_menu(cmi.first);
}

static void sync_vars_menus()
{
#if FIM_GTK_WITH_VARS_SYNC
	for ( const auto & cmi : var_menu_items_)
	{
		const auto tooltip = do_get_item_help(1+(const char*)strrchr(cmi.first.c_str(),'/'));
		gtk_widget_set_tooltip_text(cmi.second, tooltip.c_str() );
		gtk_widget_show(cmi.second);
	}
#endif /* FIM_GTK_WITH_VARS_SYNC */
}

static void sync_radio_menu(const std::string cmd)
{
	const auto var = xtrcttkn(cmd.c_str(),'=');
	const auto opt = cmd.c_str() + var.size()+1;

	if ( radio_menu_items_.find(cmd) != radio_menu_items_.end() )
	{
		for ( const auto & cmi : radio_menu_items_[cmd] )
		{
			const auto val = cc.getStringVariable(var);
			if (val == opt)
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(cmi),TRUE);
		}
	}
}

static void sync_radio_menus()
{
	for ( const auto & cmi : radio_menu_items_ )
		sync_radio_menu(cmi.first);
}

static void cb_cc_exec(GtkWidget *wdgt, const void * idxp)
{
#if !FIM_GTK_ALLOW_MENU_IN_CONSOLE
	if ( cc.inConsole() )
	{
		FIM_GTK_DBG_COUT << "Warning! inConsole()=" << cc.inConsole() << std::endl;
		return;
	}
	assert ( ! cc.inConsole() );
#endif /* FIM_GTK_ALLOW_MENU_IN_CONSOLE */
	auto cmd = FIM_GTK_P2S(idxp);

	FIM_GTK_DBG_COUT << "cmd:" << cmd << "\n";

	last_pressed_key_ = 0; // dirty overkill fixup for the case it was keypress-triggered case

	if( 0 == strncmp(cmd, "open", 4) )
	{
		if( 0 == strncmp(cmd+4, "_dir", 4) ) // open_dir
			cb_open_file(NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
		else
			cb_open_file(NULL, GTK_FILE_CHOOSER_ACTION_OPEN);
		return;
	}

#if FIM_GTK_WITH_SYSTEM_INVOCATION
	if( 0 == strncmp(cmd, "man_fim", 7) )
	{
		if( 0 == strncmp(cmd+7, "rc", 2) || !cmd[7] ) // man_fimrc || man_fim
			cmd_funcs_[cmd]();
		return;
	}
#endif /* FIM_GTK_WITH_SYSTEM_INVOCATION */

	{
		if ( FIM_GTK_IS_TOGGLE(cmd) )
		{
			const toggle_choices_t tct = get_toggle_choices(xtrcttkn(cmd));
			const std::string vid = std::get<0>(tct);
			const std::string val1 = std::get<1>(tct);
			const std::string val2 = std::get<2>(tct);

			if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(wdgt) ) )
				cc.execute("set", {vid, val1}, true);
			else
				cc.execute("set", {vid, val2}, true);
			cc.execute("set", {"i:fresh", "1" }, true); // TODO: find better solution
		}
		else
		{
			if( regexp_match(cmd, "  .*  ") || regexp_match(cmd, "=") )
			//if( regexp_match(cmd, "  .*  ") || regexp_match(cmd, "^[a-zA-Z][a-zA-Z0-9_]*=.*$") )
			{
				const auto actn = xtrcttkn(cmd);
				if( regexp_match(actn.c_str(), "=") )
				{
					const auto var = xtrcttkn(cmd,'=');
					const auto val = actn.c_str() + var.size()+1;
					cc.execute("set", {var, val}, true);
				}
				else
				{
					std::cerr << "ERROR: wrong assignment spec : [" << actn << "] in radio specification:\n" << cmd << "\n";
				}
				cc.execute("set", {"i:fresh", "1" }, true); // TODO: find better solution
			}
			else
			{
				const std::string cmds = xtrcttkn(cmd, '\0');
				if( cmds.find(" ") != cmds.npos )
				{
					cc.execute("eval", {cmds}, true); // ok, dirty trick
					if(verbose_) std::cout << "EXECUTE: eval " << cmds << std::endl;
				}
				else
				{
					cc.execute(cmds, {}, true);
					if(verbose_) std::cout << "EXECUTE: " << cmds << std::endl;
				}
			}
		}
	}
}

void do_print_var_val(GtkWidget *, const void* idxp)
{
	const auto var = FIM_GTK_P2S(idxp);
	const auto vv = cc.getStringVariable(var) + "\n";
	cc.execute("echo", { std::string(var) + " is: " + std::string(vv) + "\n"} ); // TODO: FIXME: this goes better to status, as long as in interactive mode
	do_force_console();	
}

void do_rebuild_help_aliases_menu(GtkWidget *aliaMi, const bool help_or_cmd, const char * const s)
{
	if(aliaMi) // workaround around warnings in gtk_menu_item_set_submenu
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(aliaMi), NULL);
	aliaMenu_ = gtk_menu_new();
	g_object_ref_sink(aliaMenu_);
	if(aliaMi && aliaMenu_) // workaround around warnings in gtk_menu_item_set_submenu
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(aliaMi), aliaMenu_),
		gtk_widget_set_tooltip_text(aliaMi, verbose_specs_?s:"" );

	for (auto a0 = 0UL, a1 = aliases_.find(' '); a0 < a1 && a1!=std::string::npos ; a0 = a1+1, a1 = aliases_.find(' ',a1+1) )
	{
		const auto alias = aliases_.substr(a0,a1-a0);
		GtkWidget * const aliMi = gtk_menu_item_new_with_label(alias.c_str());
		gtk_widget_set_tooltip_text(aliMi, alias.c_str() );
		{
			FIM_GTK_ASVA(alias);
			if ( help_or_cmd )
				gtk_widget_set_tooltip_text(aliMi, (do_get_item_help(alias.c_str()) /*+ " ... and click to see help..."*/).c_str() ),
				g_signal_connect( G_OBJECT(aliMi), "activate", G_CALLBACK( do_print_item_help ), FIM_GTK_LCI ); // TODO; need specific help mechanism..
			else
				gtk_widget_set_tooltip_text(aliMi, (do_get_item_help(alias.c_str()) /*+ " ... and click to see definition..."*/).c_str() ),
				g_signal_connect( G_OBJECT(aliMi), "activate", G_CALLBACK( cb_cc_exec ), FIM_GTK_LCI );
		}
		gtk_menu_shell_append(GTK_MENU_SHELL(aliaMenu_), aliMi);
		gtk_widget_show(aliMi);
	}
	gtk_widget_show(aliaMenu_);
}

void do_rebuild_help_variables_menu(GtkWidget *varsMi, const bool help_or_cmd, const char * const s)
{
	if(varsMi)
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(varsMi), NULL);
	varsMenu_ = gtk_menu_new();
	g_object_ref_sink(varsMenu_);
	if(varsMi)
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(varsMi), varsMenu_),
		gtk_widget_set_tooltip_text(varsMi, verbose_specs_?s:"" );

	for (auto v0 = 0UL, v1 = variables_.find(' '); v0 < v1 && v1!=std::string::npos ; v0 = v1+1, v1 = variables_.find(' ',v1+1) )
	{
		const auto var = variables_.substr(v0,v1-v0);
		void * vp = (void*) (variables_.c_str()+v0);
		FIM_GTK_ASVA(var);
		if(verbose_) std::cout << "VAR: " << var << std::endl;
		GtkWidget * const varMi = gtk_menu_item_new_with_label(var.c_str());
#if FIM_GTK_WITH_VARS_SYNC
		var_menu_items_[s+var] = varMi;
#endif /* FIM_GTK_WITH_VARS_SYNC */
		// perhaps still need check if variable proper..
		if (help_or_cmd)
			gtk_widget_set_tooltip_text(varMi, (do_get_item_help(var.c_str()) /*+ " ... and click to see help..."*/).c_str() ),
			g_signal_connect( G_OBJECT(varMi), "activate", G_CALLBACK( do_print_item_help ), FIM_GTK_LCI );
		else
			gtk_widget_set_tooltip_text(varMi, (do_get_item_help(var.c_str()) /*+ " ... and click to see value ..."*/).c_str() ),
			g_signal_connect( G_OBJECT(varMi), "activate", G_CALLBACK( do_print_var_val ), FIM_GTK_LCI );
		gtk_menu_shell_append(GTK_MENU_SHELL(varsMenu_), varMi);
		gtk_widget_show(varMi);
	}
	gtk_widget_show(varsMenu_);
}

void do_rebuild_help_commands_menu(GtkWidget *cmdsMi, const bool help_or_press, const char * const s)
{
	if(cmdsMi) // workaround around warnings in gtk_menu_item_set_submenu
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(cmdsMi), NULL);
	GtkWidget *cmdsMenu{}; /* special menu with all commands, autogenerated via 'FimMenuCommandsHelp' */
	cmdsMenu = gtk_menu_new();
	g_object_ref_sink(cmdsMenu);
	if(cmdsMi && cmdsMenu) // workaround around warnings in gtk_menu_item_set_submenu
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(cmdsMi), cmdsMenu),
		gtk_widget_set_tooltip_text(cmdsMi, verbose_specs_?s:"" );


	for (auto c0 = 0UL, c1 = commands_.find(' '); c0 < c1 && c1!=std::string::npos ; c0 = c1+1, c1 = commands_.find(' ',c1+1) )
	{
		const auto cmd = commands_.substr(c0,c1-c0);
		void * cp = (void*) (commands_.c_str()+c0);
		// TODO: what about cmd_to_seq_?
		const auto & csi = cmd_to_seq_.find(cmd);
		GtkWidget * const cmdMi = gtk_menu_item_new_with_label(cmd.c_str());
		gtk_widget_set_tooltip_text(cmdMi, (cmd + ": " + do_get_item_help(cmd.c_str())).c_str() ); // help msg

		if ( csi != cmd_to_seq_.end() )
		{
			const auto & cs = *csi;
			const auto & sc = std::string("sequence");// *seq_to_cmd_.find(cs.second);
			//const std::string & seq = sc.first;
			const km_t km = str_to_km(sc.c_str()); // FIXME: stub for now -- need to recover binding

			if ( km.first )
			{
				const fim_key_t key = km.first;
				const GdkModifierType mmask = km.second;
				gtk_widget_add_accelerator(cmdMi, "activate", accel_group_, key, mmask, GTK_ACCEL_VISIBLE);
			}
		}

		if ( true )
		//if ( cmd_funcs_.find(cmd) != cmd_funcs_.end() )
		{
			FIM_GTK_ASVA(cmd);
			if ( help_or_press )
				g_signal_connect(G_OBJECT(cmdMi), "activate", G_CALLBACK( do_print_item_help ), FIM_GTK_LCI );
			else
				g_signal_connect(G_OBJECT(cmdMi), "activate", G_CALLBACK( cb_cc_exec ), FIM_GTK_LCI );
		}
		else
			if(verbose_) std::cout << "Warning: no command found for " << cmd << std::endl;

		gtk_menu_shell_append(GTK_MENU_SHELL(cmdsMenu), cmdMi);
		gtk_widget_show(cmdMi);
	}
	gtk_widget_show(cmdsMenu);
}

void do_rebuild_help_bindings_menu(GtkWidget *keysMi, const bool help_or_press, const char * const s)
{
	if(keysMi) // workaround around warnings in gtk_menu_item_set_submenu
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(keysMi), NULL);
	keysMenu_ = gtk_menu_new();
	g_object_ref_sink(keysMenu_);
	if(keysMi && keysMenu_) // workaround around warnings in gtk_menu_item_set_submenu
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(keysMi), keysMenu_),
		gtk_widget_set_tooltip_text(keysMi, verbose_specs_?s:"" );

	for (const auto & bc : bindings_)
	{
		const auto ks = cc.find_key_syms(bc.first);
		GtkWidget * const keyMi = gtk_menu_item_new_with_label((bc.second).c_str());
		gtk_widget_set_tooltip_text(keyMi, (bc.second + " ...").c_str() );
		const km_t km = str_to_km(ks.c_str()); // FIXME: need update

		//if ( km.first )
		if (true)
		{
			//const fim_key_t key = km.first;
			const fim_key_t key = bc.first;
			const GdkModifierType mmask = km.second;

			gtk_widget_add_accelerator(keyMi, "activate", accel_group_, key, mmask, GTK_ACCEL_VISIBLE);
		}

		if ( true )
		//if ( cmd_funcs_.find(bc.second) != cmd_funcs_.end() )
		{
			FIM_GTK_ASVA(bc.second);
			if ( help_or_press )
				g_signal_connect(G_OBJECT(keyMi), "activate", G_CALLBACK( do_print_item_help ), FIM_GTK_LCI );
			else
				g_signal_connect(G_OBJECT(keyMi), "activate", G_CALLBACK( cb_cc_exec ), FIM_GTK_LCI );
		}

		gtk_menu_shell_append(GTK_MENU_SHELL(keysMenu_), keyMi);
		gtk_widget_show(keyMi);
	}
	gtk_widget_show(keysMenu_);
} /* do_rebuild_help_bindings_menu */

void cb_do_limit(GtkWidget * ,GtkWidget * w)
{
	const int idx = w-(lms3Miv[0]);
	if(verbose_) std::cout << "limit using index " << idx + 1 << "\n";
}

#if !FIM_GTK_WITH_RELATIVE_LIMIT_MENUS 
static void get_limit_menu_strings(void)
{
	std::istringstream mfs (cc.browser_.limit_to_variables(2, true).c_str(),std::ios::app);
	std::string ln;

	while( std::getline(mfs,ln) )
		menuspecs_.emplace_back(std::move(ln));
}
#endif /* FIM_GTK_WITH_RELATIVE_LIMIT_MENUS */

void rebuild_limit_menu(GtkWidget*lmitMi)
{
	// FIXME: TODO: replace this stub
	const size_t N = lms3Miv.size();
	// Note: not clear how to free memory of lmitMenu_.
	// Considered gtk_container_remove, gtk_menu_shell_deactivate, gtk_menu_detach, GTK_WIDGET_DESTROY, g_object_unref, without much success.
	// Reason is warnings and leak persists.
	// The current solution seem to leak very modestly.
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lmitMi), NULL);
#if 0   /* FIXME: broken: causes lots of warnings when pressing 'a', therefore it's temporarily deactivated. */
	lmitMenu_ = gtk_menu_new();
	g_object_ref_sink(lmitMenu_);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lmitMi), lmitMenu_);

	// const size_t M = 3000; // fast forward to exacerbate any memory leak
	const size_t M = 1;
	for (size_t j = 0; j < M; ++j)
	for (size_t i = 0; i < N; ++i)
	{
		if (lms3Miv[i])
		{
			//GTK_WIDGET_DESTROY(GTK_WIDGET(lms3Miv[i])); // seemingly bad
			//g_free:(GTK_WIDGET(lms3Miv[i])); // ?
			gtk_widget_remove_accelerator(lms3Miv[i], accel_group_, GDK_KEY_1 + i, GDK_CONTROL_MASK);
			//gtk_container_remove(GTK_CONTAINER(lmitMenu_), lms3Miv[i]);

			// https://bugzilla.redhat.com/show_bug.cgi?id=1970254 suggests to use the two of:
			g_object_unref (lms3Miv[i]);
			/// g_object_unref(lms3Miv[i]); //  g_object_unref: assertion 'G_IS_OBJECT (object)' failed // changing order seems no solution
			// ...but leak still there.
		}
		std::string lbl = "Limit Sample " + std::to_string(hist_n_) +
			" (_" + std::to_string(i+1) + ")"
			;
		hist_n_++;
		lms3Miv[i] = gtk_menu_item_new_with_mnemonic(lbl.c_str());
		gtk_menu_shell_append(GTK_MENU_SHELL(lmitMenu_), lms3Miv[i]);
		g_signal_connect(G_OBJECT(lms3Miv[i]), "activate", G_CALLBACK(cb_do_limit), (lms3Miv[0]+i));
		gtk_widget_set_tooltip_text(lms3Miv[i], "limit file list to ...");
		gtk_widget_show(lms3Miv[i]);
		gtk_widget_add_accelerator(lms3Miv[i], "activate", accel_group_, GDK_KEY_1 + i, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	}
	gtk_widget_show(lmitMenu_);
#endif
}

void cb_on_open_response (GtkDialog *dialog, int response)
{
	if (response == GTK_RESPONSE_ACCEPT || response == FIM_GTK_RESPONSE_ACCEPT_REC_FG /* || response == FIM_GTK_RESPONSE_ACCEPT_REC_BG */) {
		std::string msg ("opening ");
		char * const fn = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)); // FIXME: need gtk_file_chooser_add_filter ...
		msg += fn;
		// std::cout << msg << "\n";
		cc.push(fn,(response == GTK_RESPONSE_ACCEPT)?0:FIM_FLAG_PUSH_REC/*|((response == FIM_GTK_RESPONSE_ACCEPT_REC_FG )?0:FIM_FLAG_PUSH_BACKGROUND)*/); // FIXME: preliminary
		g_free (fn);
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
	dialog = NULL;
	//GTK_WIDGET_DESTROY(GTK_WIDGET(dialog));
}

static gboolean cb_open_file( GtkMenuItem*, GtkFileChooserAction action)
{
	GtkWidget *dialog = 
		(action==GTK_FILE_CHOOSER_ACTION_OPEN) ?
			gtk_file_chooser_dialog_new("Open file", GTK_WINDOW(window_), GTK_FILE_CHOOSER_ACTION_OPEN, "cancel", GTK_RESPONSE_CANCEL, "select file", GTK_RESPONSE_ACCEPT, NULL)
		:
			gtk_file_chooser_dialog_new("Open dir", GTK_WINDOW(window_), GTK_FILE_CHOOSER_ACTION_OPEN, "cancel", GTK_RESPONSE_CANCEL, "select directory", GTK_RESPONSE_ACCEPT, "select directory recursively (slow)", FIM_GTK_RESPONSE_ACCEPT_REC_FG/*, "select directory recursively background", FIM_GTK_RESPONSE_ACCEPT_REC_BG*/, NULL);
	gtk_file_chooser_set_action ((GtkFileChooser*)dialog, action);
	gtk_widget_show(dialog);
	g_signal_connect (dialog, "response", G_CALLBACK (cb_on_open_response), NULL);
	return FALSE;
}

static bool add_to_menubar(const char * const s)
{
	// add one menu item to the menu
	// FIXME: return false on error and react consequently
	const char *st = "  "; // separator token
	const char *e;
	const char *b = s;
	std::string add;
	std::string top;
	std::string lbl;
	std::set<std::string> rcmds {}; // repeated radio commands
#if FIM_GTK_WITH_SHORT_MENUSPEC
	char lst [3] = {st[0], st[1], '\0'}; // local separator token // TODO: single char is enough
#endif

	if(verbose_) std::cout << s << std::endl;

	int tc = 0; // token count
	const char * p = b;
	GSList *menuGr {};
	std::set<GtkWidget*> radioset {};

#if !FIM_GTK_WITH_SHORT_MENUSPEC
	while ( ( p = strstr(p + 2, st) ) )
		++tc;
#endif

	while ( ( e = strchr(b, '/') ) && e[-1] != ' ' )
	{
		const char * l = strstr(b, st);
		char new_submenu = 'n';

		if ( l && l + 2 < e )
			lbl = std::string(b,0,l-b);
		else
			lbl = std::string(b,0,e-b);

		top = add;
		add += '/';
		if(verbose_) std::cout << "LBL: " << add << " : " << lbl << "\n";
		add += lbl;

		if ( menus_.find(add) == menus_.end() )
			new_submenu = 'y';

		if ( l && l + 2 < e && l + 2 == strstr( l + 2, "FimMenu" ) )
		{
			new_submenu = 's'; // special
			l += 2;
			if(verbose_) std::cout << "SPECIAL\n";
		}

		if(verbose_) std::cout << "SMN " << add << " : " << ' ' << " : " << new_submenu << std::endl;
		if ( new_submenu != 'n' )
			menu_items_[add] = (GtkMenuItem*) gtk_menu_item_new_with_mnemonic(lbl.c_str());
		if ( new_submenu == 's' )
		{
			// TODO: versions of these with extras or without
			if ( l == sym_strstr( l, "FimMenuAliases" ) )
				do_rebuild_help_aliases_menu((GtkWidget*)menu_items_[add],false,s);
			else
			if ( l == strstr( l, "FimMenuAliasesHelp" ) )
				do_rebuild_help_aliases_menu((GtkWidget*)menu_items_[add],true,s);
			else
			if ( l == sym_strstr( l, "FimMenuKeyBindings" ) )
				do_rebuild_help_bindings_menu((GtkWidget*)menu_items_[add],false,s);
			else
			if ( l == strstr( l, "FimMenuKeyBindingsHelp" ) )
				do_rebuild_help_bindings_menu((GtkWidget*)menu_items_[add],true,s);
			else
			if ( l == sym_strstr( l, "FimMenuCommands" ) )
				do_rebuild_help_commands_menu((GtkWidget*)menu_items_[add],false,s);
			else
			if ( l == strstr( l, "FimMenuCommandsHelp" ) )
				do_rebuild_help_commands_menu((GtkWidget*)menu_items_[add],true,s);
			else
			if ( l == sym_strstr( l, "FimMenuVariables" ) )
				do_rebuild_help_variables_menu((GtkWidget*)menu_items_[add],false,s);
			else
			if ( l == strstr( l, "FimMenuVariablesHelp" ) )
				do_rebuild_help_variables_menu((GtkWidget*)menu_items_[add],true,s);
			else
			if ( l == strstr( l, "FimMenuLimit" ) )
			{
#if FIM_GTK_WITH_RELATIVE_LIMIT_MENUS
				std::istringstream mfs (cc.browser_.limit_to_variables(2, true).c_str(),std::ios::app);
				std::string ln;

				while( std::getline(mfs,ln) )
				{
					const std::string nme = (std::string(s,0,l-2-s)+"/"+ln);
					add_to_menubar(nme.c_str());
					if(verbose_) std::cout << "menu: " << nme << "\n";
				}
				goto ok;
#else /* FIM_GTK_WITH_RELATIVE_LIMIT_MENUS */
				rebuild_limit_menu((GtkWidget*)menu_items_[add]);
#endif /* FIM_GTK_WITH_RELATIVE_LIMIT_MENUS */
			}
			else
			{
				std::cerr << "ERROR: bogus special menu spec " << l << " in:\n" << s << "\n";
				goto oops;
			}
			if(verbose_) std::cout << "SPECIAL\n";
		}
		if ( new_submenu != 'n' )
			menus_[add] = (GtkMenu*) gtk_menu_new();
		if ( new_submenu == 'y' )
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_items_[add]), (GtkWidget*)menus_[add]);
		if ( new_submenu != 'n' )
		{
			if ( b == s )
				gtk_menu_shell_append(GTK_MENU_SHELL(menubar_), (GtkWidget*)menu_items_[add]);
			else
				gtk_menu_shell_append(GTK_MENU_SHELL(menus_[top]), (GtkWidget*)menu_items_[add]);
		}
		b = e + 1;
	}

	if (add.empty())
	{
		std::cerr << "ERROR: / is a missing requisite in menu specification:\n" << s << "\n";
		goto oops;
	}
	top = add;

#if FIM_GTK_WITH_SHORT_MENUSPEC
	if ( b[0] && b[1] && b[2] && b[0] == b[1] && !isalnum(b[0]) )
	{
		lst[0] = lst[1] = b[0];
		if(verbose_) std::cout << "SEP: [" << lst[0] << lst[1] << "]\n";
		e += 2, b += 2;
	}

	p=b;
	if (*p)
	while ( ( p = strstr(p + 2, lst) ))
		++tc;
#endif
repeat:

#if FIM_GTK_WITH_SHORT_MENUSPEC
	if ( ( e = strstr(b, lst) ) )
#else
	if ( ( e = strstr(b, st) ) )
#endif
	{
		// b = e + 2;
		if(verbose_) std::cout << "tc=" << tc << " e-b=" << e-b << " b: " << b << "  " << "\n";
		lbl = std::string(b,0,e-b);
		add = top;
		add += '/';
		if(verbose_) std::cout << "ADD: " << add << "\n";
		if(verbose_) std::cout << "LBL: " << lbl << "\n";
		add += lbl;
		if ( ! *e )
		{
			// TODO: eliminate this branch
			std::cout << "TERMINAL" << b << "\n";
			if ( menu_items_.find(add) == menu_items_.end() )
				menu_items_[add] = (GtkMenuItem*) gtk_menu_item_new_with_mnemonic(lbl.c_str());
			gtk_menu_shell_append(GTK_MENU_SHELL(menus_[top]), (GtkWidget*)menu_items_[add]);
		}
		b = e + 2;
	}

#if FIM_GTK_WITH_SHORT_MENUSPEC
	if ( ( e = strstr(b, lst) ) || ( e = b + strlen(b) ) )
#else
	if ( ( e = strstr(b, st) ) || ( e = b + strlen(b) ) )
#endif
	{
		const auto cmd = std::string(b,0,e-b);
		const void* cp = b;
		std::string tooltip;

		if ( verbose_specs_ )
			tooltip += s,
			tooltip += "\n";

		if ( rcmds.find(cmd) != rcmds.end() )
		{
			std::cerr << "ERROR: repeated radio command : [" << cmd << "] in menu specification:\n" << s << "\n";
			goto oops;
		}

		if( cmd.size() && !regexp_match(cmd.c_str(), "^([a-zA-Z_][a-z0-9A-Z_ '=~]*" "|toggle...*" "|(i:)?[a-zA-Z_][a-zA-Z_]*=.*" ")$") )
		{
			std::cerr << "ERROR: bad command : [" << cmd << "] in menu specification:\n" << s << "\n";
			goto oops;
		}

		if ( menu_items_.find(add) == menu_items_.end() )
		{
			if(verbose_) std::cout << "COMMAND@" << e-b << ":tc=" << tc << ":[" << cmd << "]\n";
#if FIM_GTK_WITH_SHORT_MENUSPEC
			if ( FIM_GTK_IS_TOGGLE(b) && (tc < 4) )
#else
			if ( FIM_GTK_IS_TOGGLE(b) )
#endif
			{
				if ( ! is_valid_toggle_spec(cmd) )
				{
					std::cerr << "ERROR: invalid toggle spec : [" << cmd << "] in menu specification:\n" << s << "\n";
					goto oops;
				}
				menu_items_[add] = (GtkMenuItem*) gtk_check_menu_item_new_with_mnemonic(lbl.c_str()),
				check_menu_items_[xtrcttkn(cmd.c_str())].insert((GtkWidget*) menu_items_[add]), // or vid?
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_items_[add]), FALSE);
				tooltip += get_tooltip_from_toggle_spec(cmd);
			}
			else
			{
				if (tc > 2)
				{
					// group is lbl
					if(verbose_) std::cout << "RADIO " << add << " -> " << cmd << "\n";
					menu_items_[add] = (GtkMenuItem*) gtk_radio_menu_item_new_with_mnemonic(menuGr, lbl.c_str()),
					radio_menu_items_[cmd].insert((GtkWidget*) menu_items_[add]), // TODO: need a different container, e.g. 
					menuGr = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_items_[add])),
					radioset.insert((GtkWidget*) menu_items_[add]);
					rcmds.insert(cmd);
				}
				else
				{
					menu_items_[add] = (GtkMenuItem*) gtk_menu_item_new_with_mnemonic(lbl.c_str());
				}
			}
		}

		if (! gtk_widget_get_parent((GtkWidget*)menu_items_[add]))
			gtk_menu_shell_append(GTK_MENU_SHELL(menus_[top]), (GtkWidget*)menu_items_[add]);

		if(verbose_) std::cout << "CMD: " << top << " + " << lbl << " -> " << cmd << "\n";
		if (cmd.size())
		{
			FIM_GTK_ASVA(cmd);
#if 0
			if ( b == strstr(b, "toggle") )
				g_signal_connect(G_OBJECT(menu_items_[add]), "toggled", G_CALLBACK(cmd_funcs_[cmd]), NULL);
			else
				g_signal_connect(G_OBJECT(menu_items_[add]), "activate", G_CALLBACK(cmd_funcs_[cmd]), NULL);
#endif
			if ( true )
			{
				g_signal_connect(G_OBJECT(menu_items_[add]), "activate", G_CALLBACK( cb_cc_exec ), FIM_GTK_LCI ); // cp centered on command, b is more
				if (!tooltip.size() && cc.aliasRecall(cmd).size())
					tooltip = cmd + " -> " + cc.aliasRecall(cmd);
				gtk_widget_set_tooltip_text((GtkWidget*)menu_items_[add], (tooltip.size() ? tooltip.c_str() : cmd.c_str()) );
			}
			else
			{
				if(verbose_) std::cout << "Warning: no command found for " << cmd << " so cannot give action to menu" << std::endl;
			}
		}

		if ( ! *e )
			;
		else
		{
#if FIM_GTK_WITH_SHORT_MENUSPEC
			const char * ss = strstr(b = e + 2, lst); // FIXME
#else
			const char * ss = strstr(b = e + 2, st); // FIXME
#endif
			//const std::string sym = ss ? std::string(b, 0, ss - b) : std::string(b); // FIXME
			auto key = cc.find_key_for_bound_cmd(cmd);

			if (key.size())
			{
				km_t km = str_to_km(key.c_str());
				km.first = key[0]; // FIXME, need to separate key (or ignore it, or change format)
				if( km.first )
					gtk_widget_add_accelerator( (GtkWidget*)menu_items_[add], "activate", accel_group_, km.first, km.second, GTK_ACCEL_VISIBLE);
			}
#if FIM_GTK_WITH_SHORT_MENUSPEC
			e = strstr(b, lst); // e points after symbol
#else
			e = strstr(b, st); // e points after symbol
#endif
		}
	}

	if ( e && /* (tc + 2) / 3 > 1 && */ *e ) // && ! menuGr )
	{
#if FIM_GTK_WITH_SHORT_MENUSPEC
		assert ( strstr(b, lst) );
#else
		assert ( strstr(b, st) );
#endif
		b = e + 2;
		if(verbose_) std::cout << "AGAIN: " << b << " " << "\n";
#if FIM_GTK_WITH_SHORT_MENUSPEC
		e = strstr(b, lst);
#else
		e = strstr(b, st);
#endif
		if(verbose_) std::cout << "GROUPING: " << s << "\n";
		if (e)
			goto repeat;
	}

	if (menuGr)
		group_widgets_[menuGr] = radioset;
ok:
	return true;
oops:
	return false;
} /* add_to_menubar */

#if FIM_GTK_WITH_MENU_EDITING_DIALOG
static gboolean cb_menu_dialog(GtkMenuItem*)
{
	GtkWidget *entry;
	GtkEntryCompletion *completion;
	GtkListStore *store;
	completion = gtk_entry_completion_new();
	GtkWidget *dialog = gtk_dialog_new_with_buttons("Add menu dialog", GTK_WINDOW(window_),
		GTK_DIALOG_MODAL,
		//GTK_DIALOG_DESTROY_WITH_PARENT,
		"Ok", GTK_RESPONSE_OK,
		NULL );
	entry = gtk_entry_new();
	gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
	gtk_widget_set_halign(GTK_WIDGET(entry), GTK_ALIGN_START);
	gtk_widget_set_margin_top(GTK_WIDGET(entry), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(entry), 0);
	gtk_entry_get_activates_default(GTK_ENTRY(entry));
	gtk_entry_set_has_frame(GTK_ENTRY(entry), FALSE);
	gtk_entry_set_width_chars(GTK_ENTRY(entry), 80);
	store = gtk_list_store_new (1, G_TYPE_STRING);
	for (size_t i = 0; i < menuspecs_.size() ; i++)
	{
		gtk_list_store_append (store, &iter_);
		gtk_list_store_set (store, &iter_, 0, menuspecs_[i], -1);
	}
	gtk_entry_set_completion (GTK_ENTRY (entry), completion);
	gtk_entry_completion_set_model (completion, GTK_TREE_MODEL (store));
	gtk_entry_completion_set_text_column (completion, 0);
	gtk_entry_completion_set_minimum_key_length(completion, 0);
	gtk_dialog_add_action_widget (GTK_DIALOG(dialog), entry, 0);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	gtk_entry_set_text(GTK_ENTRY(entry), menuspecs_.size()?menuspecs_[0].c_str():"...");
	gtk_widget_show_all(entry);
	gtk_widget_show_all(dialog);
	const auto result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result != GTK_RESPONSE_OK && result != 0)
		std::cout << "Warning: wrong dialog termination : " << result << "\n";
	else
	{
		// TODO: FIXME: this is not yet ready
		const std::string menubarspec_ = gtk_entry_get_text(GTK_ENTRY(entry));
		std::cout << "Using menu spec : " << menubarspec_ << " after getting code " << result << "\n";
		if (menubarspec_.size())
			add_to_menubar(menubarspec_.c_str()),
			gtk_widget_show_all(menubar_);
	}
	gtk_widget_destroy (dialog);
	//GTK_WIDGET_DESTROY(GTK_WIDGET(dialog));
	return TRUE;
}
#endif /* FIM_GTK_WITH_MENU_EDITING_DIALOG */

static void do_init_cmd_funcs(void)
{
	// bind actual functionality to commands
	FIM_GTK_DBG_COUT << "\n";
#if FIM_GTK_WITH_MENU_EDITING_DIALOG
	cmd_funcs_["menu_dialog"] = [](){ cb_menu_dialog(NULL); };
#endif /* FIM_GTK_WITH_MENU_EDITING_DIALOG */
	// cmd_funcs_["rebuild_limit_menu"] = [](){ };

#if FIM_GTK_WITH_SYSTEM_INVOCATION
	if ( system("which xterm") )
	{
		cmd_funcs_["man_fim"] = [](){ system("xterm -e man fim"); };
		cmd_funcs_["man_fimrc"] = [](){ system("xterm -e man fimrc"); };
	}
	if ( system("which konsole") )
	{
		cmd_funcs_["man_fim"] = [](){ system("konsole -e man fim"); };
		cmd_funcs_["man_fimrc"] = [](){ system("konsole -e man fimrc"); };
	}
#endif /* FIM_GTK_WITH_SYSTEM_INVOCATION */
	// TODO: something to reinit autocompletion
}

void do_rebuild_help_menus(void)
{
	// build/rebuild menus from scratch (not showing them)
	if (menubar_)
		gtk_container_remove (GTK_CONTAINER(grid_), menubar_);

	asv_ = {};
	// menuspecs_ = {}; // this must be reset elsewhere
	menu_items_ = {};
#if FIM_GTK_WITH_VARS_SYNC
	var_menu_items_ = {};
#endif /* FIM_GTK_WITH_VARS_SYNC */
	check_menu_items_ = {};
	radio_menu_items_ = {};
	group_widgets_ = {};
	menus_ = {};

	menubar_ = gtk_menu_bar_new();
	gtk_widget_set_vexpand(GTK_WIDGET(menubar_), FALSE);
	gtk_widget_set_hexpand(GTK_WIDGET(menubar_), FALSE);
	gtk_widget_set_margin_top(GTK_WIDGET(menubar_), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(menubar_), 0);
	gtk_grid_attach_next_to (GTK_GRID(grid_), GTK_WIDGET(menubar_), GTK_WIDGET(drawingarea_), GTK_POS_TOP, 1, 1);

	bindings_ = cc.get_bindings(); // note: this invalidates widgets
	aliases_ = cc.get_aliases_list(); // note: this invalidates widgets
	commands_ = cc.get_commands_list(); // note: this invalidates widgets
	variables_ = cc.get_variables_list(); // note: this invalidates widgets

#if !FIM_GTK_WITH_RELATIVE_LIMIT_MENUS 
	if ( menuspecs_.size() )
		get_limit_menu_strings(); // only add when menu already exists and this does not end up being first one (which is likely when images are loaded actually)
#endif /* FIM_GTK_WITH_RELATIVE_LIMIT_MENUS */
	for (const auto & menu_spec: menuspecs_)
		add_to_menubar(menu_spec.c_str());
//	gtk_widget_show_all (GTK_WIDGET(menubar_));
//	gtk_widget_show_all (GTK_WIDGET(window_));
//	gtk_widget_hide (cmdline_entry_);
} /* do_rebuild_help_menus */
#endif /* FIM_GTK_WITH_MENUBAR */

fim_err_t GTKDevice::initialize(fim::sym_keys_t&sym_keys)
{
	FIM_GTK_DBG_COUT << "\n";
	do_init_cmd_funcs();
	gtk_init(NULL, NULL);
	window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window_), FIM_DEFAULT_WINDOW_WIDTH, FIM_DEFAULT_WINDOW_HEIGHT);
	set_wm_caption(wtitle_.c_str());
	gtk_widget_add_events(GTK_WIDGET(window_), GDK_BUTTON_PRESS_MASK);
	grid_ = gtk_grid_new();
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
	reinit((opts_+"f").c_str());
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
	sync_toggle_menus();
	sync_vars_menus();
	sync_radio_menus();

	if ( last_pressed_key_ )
	{
		if (want_poll) // hack: only set if in readline mode
			switch (last_pressed_key_)
			{
				case(GDK_KEY_Up):   last_pressed_key_=0x111; break;
				case(GDK_KEY_Down): last_pressed_key_=0x112; break;
				case(GDK_KEY_Right):last_pressed_key_=0x113; break;
				case(GDK_KEY_Left): last_pressed_key_=0x114; break;
			}
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
	FIM_GTK_DBG_COUT << " iv=" << iv << " *c=" << *c << " want_poll=" << want_poll << "\n";
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
		c = last_pressed_key_;
		gtk_main_iteration();
		FIM_GTK_INPUT_DEBUG(&c,"");
		if (c) // if any input there
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

fim_err_t GTKDevice::menu_ctl(const char action, const fim_char_t *menuspec)
{
	fim_err_t rc = FIM_ERR_NO_ERROR;

	switch (action)
	{
		case 'A':
		case 'a':
		{
			std::string menuspecstr(menuspec);
			if ( add_to_menubar(menuspecstr.c_str()) )
			{
				menuspecs_.emplace_back(std::move(menuspecstr));
				gtk_widget_show_all(menubar_);
				rc = FIM_ERR_NO_ERROR;
			}
			else
				rc = FIM_ERR_BAD_PARAMS;
		}
		break;
		case 'D':
		case 'd':
			// FIXME: TODO: continue here
			rc = FIM_ERR_UNSUPPORTED;
		break;
		case 'V':
			verbose_ = 0;
		break;
		case 'v':
			verbose_ = 1;
		break;
		default:
		rc = FIM_ERR_GENERIC;
	} 
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
GTKDevice::GTKDevice(MiniConsole& mc_, fim::string opts): DisplayDevice (mc_),
#else /* FIM_WANT_NO_OUTPUT_CONSOLE */
GTKDevice::GTKDevice(fim::string opts):DisplayDevice(),
#endif /* FIM_WANT_NO_OUTPUT_CONSOLE */
	opts_(opts)
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

	fim_err_t GTKDevice::resize(fim_coo_t w, fim_coo_t h)
	{
		// FIXME: this mechanism is messy, because gtk_widget_get_allocated_height(drawingarea_) != gtk_widget_get_allocated_height(window_)
		if (w != gtk_widget_get_allocated_width((GtkWidget*)drawingarea_) || h != gtk_widget_get_allocated_height((GtkWidget*)drawingarea_))
			gtk_window_resize (window_, w, h);
		return FIM_ERR_NO_ERROR;
	}
	
	fim_err_t GTKDevice::reinit(const fim_char_t *rs)
	{
		FIM_GTK_DBG_COUT << ":" << rs << "\n";

		if((!verbose_) && cc.getVariable(FIM_VID_DBG_COMMANDS).find("mm") >= 0)
		{
			verbose_ = 1;
			std::cout << FIM_CNS_DBG_CMDS_PFX << "setting verbose menus build\n";
		}
		if((!verbose_specs_) && cc.getVariable(FIM_VID_DBG_COMMANDS).find('m') >= 0)
		{
			verbose_specs_ = 1;
			std::cout << FIM_CNS_DBG_CMDS_PFX << "setting verbose menu tooltip\n";
		}

		if( rs && *rs )
		{
			fim_coo_t current_w = width();
			fim_coo_t current_h = height();
			auto os = rs;
			while ( *os && ! isdigit(*os) )
				++os;
			if ( *os && isdigit(*os) )
			{
				if(const int si = sscanf(os,"%d:%d",&current_w,&current_h))
				{
					if ( si == 1)
						current_h = current_w;
					if ( strrchr(os,'%') && !strrchr(os,'%')[1] )
						current_w = FIM_MIN(current_w, 100),
						current_h = FIM_MIN(current_h, 100);
					if (current_w && current_h)
						gtk_window_resize (window_, current_w, current_h); // notice actual resize is postponed
				}
				else
				{
					current_w = current_h = 0;
					std::cerr << "user specification of resolution (\""<<os<<"\") wrong: it shall be in \"width:height\" format! \n";
				}
			}
		}

		if (strchr(rs, 'W'))
			full_screen_=1;
		else
			full_screen_=0;
		if (strchr(rs, 'w') || strchr(rs, 'W'))
			toggle_fullscreen(full_screen_);

		if (strchr(rs, *FIM_CNS_GTK_MNRBCHR_STR))
			do_rebuild_help_menus();

		if (strchr(rs, 'b')) // TODO: FIXME: barely documented
			show_menubar_=0;
		else
			show_menubar_=1;

		if (strchr(rs, 'b') || strchr(rs, 'B') || strchr(rs, *FIM_CNS_GTK_MNRBCHR_STR))
		{
			if ( show_menubar_ )
				gtk_widget_show_all (menubar_);
			else
				gtk_widget_hide (menubar_);
		}

		if ( cc.display_resize(nw_,nh_) == FIM_ERR_NO_ERROR ) // notice nw_ and nh_ still at old value
		{
			// if reinit called from constructor it returns error
			// opts_ = rs;
			return FIM_ERR_NO_ERROR;
		}

		return FIM_ERR_GENERIC;
	}

#endif /* FIM_WITH_LIBGTK */
#pragma GCC pop_options
