/* $Id$ */
/*
 Browser.h : Image browser header file

 (c) 2007 Michele Martone

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
#ifndef BROWSER_FBVI_H
#define BROWSER_FBVI_H
#include "fim.h"
namespace fim
{
/*
 * The Browser class oversees image browsing.
 */
class Browser
{
	/*
	 * The file browser holds the names of files in the slideshow.
	 */
	std::vector<fim::string> flist;

	/*
	 * It has a dummy empty filename
	 */
	const fim::string nofile;

	/*
	 * And it keeps a numerical index of the current file, too.
	 *
	 * cp is zero only when there are no files in the list.
	 * the current file index is in current_n()
	 */
	int cp;

#ifndef FIM_WINDOWS
	Viewport *only_viewport;
#endif

	Image *image()const;
	const Image *c_image()const;

	Viewport& viewport()const;

	int current_n()const;
	int current_n(int ccp)const;
//	const fim::string pop(fim::string popped="");
	const fim::string pop();
	fim::string get_next_filename(int n);
	
	fim::string last_regexp;
	fim::Cache cache;
	int current_image()const;
	int current_images()const{ return n(); }
	public:
	int empty_file_list()const;

	Browser();
	~Browser() { }
	fim::string current()const;
	fim::string regexp_goto(const std::vector<fim::string> &args);
	fim::string prefetch(const std::vector<fim::string> &args);
	fim::string regexp_goto_next(const std::vector<fim::string> &args);
	fim::string goto_image(const std::vector<fim::string> &args);
	fim::string goto_image(int n);
	fim::string top_align(const std::vector<fim::string> &args);
	fim::string bottom_align(const std::vector<fim::string> &args);
	fim::string pan_ne(const std::vector<fim::string> &args);
	fim::string pan_nw(const std::vector<fim::string> &args);
	fim::string pan_sw(const std::vector<fim::string> &args);
	fim::string pan_se(const std::vector<fim::string> &args);
	fim::string pan_up(const std::vector<fim::string> &args);
	fim::string pan_down(const std::vector<fim::string> &args);
	fim::string pan_right(const std::vector<fim::string> &args);
	fim::string pan_left(const std::vector<fim::string> &args);
	fim::string scrolldown(const std::vector<fim::string> &args);
	fim::string scrollforward(const std::vector<fim::string> &args);
	fim::string scale_increment(const std::vector<fim::string> &args);
	fim::string scale_multiply(const std::vector<fim::string> &args);
	fim::string auto_scale(const std::vector<fim::string> &args);
	fim::string auto_width_scale(const std::vector<fim::string> &args);
	fim::string scale(const std::vector<fim::string> &args);
	fim::string auto_height_scale(const std::vector<fim::string> &args);
	fim::string reduce(const std::vector<fim::string> &args);
	fim::string magnify(const std::vector<fim::string> &args);
	fim::string scale_factor_increase(const std::vector<fim::string> &args);
	fim::string scale_factor_decrease(const std::vector<fim::string> &args);
	fim::string scale_factor_grow(const std::vector<fim::string> &args);
	fim::string scale_factor_shrink(const std::vector<fim::string> &args);
	fim::string display(const std::vector<fim::string> &args);
	fim::string display_status(const char *l,const char*r);

	fim::string reload(const std::vector<fim::string> &args);
	fim::string list(const std::vector<fim::string> &args){return list();}
	fim::string push(const std::vector<fim::string> &args);

	fim::string n(const std::vector<fim::string> &args){return n();}
	fim::string _sort(const std::vector<fim::string> &args){return _sort();}
	fim::string next(const std::vector<fim::string> &args);

	fim::string prev(int n=1);
	fim::string prev(const std::vector<fim::string> &args);
	fim::string remove(const std::vector<fim::string> &args);
	fim::string info(const std::vector<fim::string> &args);
	fim::string info();
	std::ostream& print(std::ostream &os)const;
	void redisplay();
	fim::string redisplay(const std::vector<fim::string> &args);


	fim::string load(const std::vector<fim::string> &args);
	fim::string pop(const std::vector<fim::string> &args);
	const fim::string pop_current();
	fim::string pop_current(const std::vector<fim::string> &args);
	fim::string no_image(const std::vector<fim::string> &args);
	bool present(const fim::string nf);
	bool push(const fim::string nf);

	fim::string display();
	private:
	fim::string loadCurrentImage();
	fim::string reload();
	fim::string list()const;

	int n_files()const;
	const fim::string n()const;
	fim::string _sort();
	fim::string next(int n);
	fim::string do_next(int n);

	void free_current_image();
	int load_error_handle(fim::string c);
	public:
};
}

#endif
