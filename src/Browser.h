/* $LastChangedDate$ */
/*
 Browser.h : Image browser header file

 (c) 2007-2011 Michele Martone

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
#ifndef FIM_BROWSER_H
#define FIM_BROWSER_H
#include "fim.h"
namespace fim
{
/*
 * A Browser object oversees image browsing.
 */
#ifdef FIM_NAMESPACES
class Browser:public Namespace
#else
class Browser
#endif
{
	private:
	args_t flist_; /* the names of files in the slideshow.  */

	const fim::string nofile_; /* a dummy empty filename */

	/*
	 * cf_ is zero only when there are no files in the list.
	 * the current file index is in current_n()
	 */
	int cf_;

#ifndef FIM_WINDOWS
	/* when compiled with no multiple windowing support, one viewport only will last. */
	Viewport *only_viewport_;
#endif
	CommandConsole &commandConsole_;
	Image *image()const;

#ifdef FIM_READ_STDIN_IMAGE
	Image *default_image_;	// experimental
#endif

	Viewport* viewport()const;

	int current_n()const;
	int current_n(int ccp)const;
	const fim::string pop(fim::string filename=FIM_CNS_EMPTY_STRING);
	fim::string get_next_filename(int n)const;
	
	int current_image()const;
	public:
	fim::string last_regexp_; // was private
	Cache cache_;	// was private
#ifdef FIM_READ_STDIN_IMAGE
	void set_default_image(Image *stdin_image);
#endif
	const Image *c_image()const;	// was private
	int empty_file_list()const;

	Browser(CommandConsole &cc);
	~Browser() { }
	private:
	Browser& operator= (const Browser &b){return *this;/* a nilpotent assignation */}
	Browser(const Browser &b):
		flist_(args_t()),
		nofile_(""),
		cf_(0),
		commandConsole_(cc),
#ifdef FIM_READ_STDIN_IMAGE
		default_image_(NULL),
#endif
		last_regexp_(fim::string()),
		cache_(Cache())
		{}
	public:
	fim::string current()const;
	fim::string regexp_goto(const args_t &args);
	fim::string fcmd_prefetch(const args_t &args);
	fim::string regexp_goto_next(const args_t &args);
	fim::string fcmd_goto_image(const args_t &args);
	fim::string goto_image_internal(const fim_char_t *s, fim_xflags_t xflags);
	fim::string goto_image(int n, bool isfg=false);
	fim::string fcmd_align(const args_t &args);
	fim::string pan(const args_t &args);
	fim::string fcmd_scrolldown(const args_t &args);
	fim::string fcmd_scrollforward(const args_t &args);
	fim::string fcmd_scale(const args_t &args);
	fim::string fcmd_reduce(const args_t &args);
	fim::string fcmd_magnify(const args_t &args);
	fim::string fcmd_rotate(const args_t &args);/* FIXME : UNFINISHED */
	fim::string fcmd_display(const args_t &args);
	fim::string display_status(const fim_char_t *l,const fim_char_t*r);
	fim::string fcmd_negate(const args_t &args);

	fim::string fcmd_reload(const args_t &args);
	fim::string fcmd_list(const args_t &args);
	fim::string do_push(const args_t &args);
	fim::string fcmd_next(const args_t &args);
	fim::string prev(int n=1);
	fim::string fcmd_prev(const args_t &args);
	fim::string do_remove(const args_t &args);
	fim::string fcmd_info(const args_t &args);
	fim::string info();
	std::ostream& print(std::ostream &os)const;
	void redisplay();
	fim::string fcmd_redisplay(const args_t &args);
	fim::string fcmd_load(const args_t &args);
	const fim::string pop_current();
	fim::string pop_current(const args_t &args);
	fim::string fcmd_no_image(const args_t &args);
	bool present(const fim::string nf);
	fim_int find_file_index(const fim::string nf);
#ifdef FIM_READ_DIRS
	bool push_dir(fim::string nf);
#endif
	bool push(fim::string nf);

	fim::string display();
	fim::string _random_shuffle();
	fim::string _clear_list();
	private:
	fim::string loadCurrentImage();
	fim::string reload();
	fim::string list()const;

	int n_files()const;
	int n_pages()const;
	fim::string _sort();
	fim::string _reverse();
	fim::string next(int n=1);

	void free_current_image();
	int load_error_handle(fim::string c);
	public:
	int c_page()const;
};
}

#endif
