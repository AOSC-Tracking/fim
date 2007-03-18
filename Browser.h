/* $Id$ */
#ifndef BROWSER_FBVI_H
#define BROWSER_FBVI_H
#include "fim.h"
namespace fim
{
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

	Image *image;
	int current_n()const{ return cp?cp-1:cp; }
	const fim::string pop();
	public:
	int empty_file_list()const{ return flist.size()==0;}
	int current_image()const{ return cp; }
	int current_images()const{ return n(); }
	Browser();
	~Browser() { }
	fim::string current()const{ return cp?flist[current_n()]:nofile; }
	fim::string regexp_goto(const std::vector<fim::string> &args);
	fim::string goto_image(const std::vector<fim::string> &args);
	fim::string goto_image(int n);
	fim::string pan_up(const std::vector<fim::string> &args);
	fim::string pan_down(const std::vector<fim::string> &args);
	fim::string scale_increment(const std::vector<fim::string> &args);
	fim::string scale_multiply(const std::vector<fim::string> &args);
	fim::string auto_scale(const std::vector<fim::string> &args);
	fim::string auto_width_scale(const std::vector<fim::string> &args);
	fim::string scale(const std::vector<fim::string> &args);
	fim::string auto_height_scale(const std::vector<fim::string> &args);
	fim::string pan_right(const std::vector<fim::string> &args);
	fim::string pan_left(const std::vector<fim::string> &args);
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
	fim::string push(const std::vector<fim::string> &args)
	{
		for(unsigned int i=0;i<args.size();++i)
			push(args[i]);
		return "";
	}
	fim::string n(const std::vector<fim::string> &args){return n();}
	fim::string get(const std::vector<fim::string> &args){return get();}
	fim::string _sort(const std::vector<fim::string> &args){return _sort();}
	fim::string next(const std::vector<fim::string> &args){return next(args.size()>0?((int)args[0]):1);}
	fim::string prev(int n=1);
	fim::string prev(const std::vector<fim::string> &args){return prev(args.size()>0?((int)args[0]):1);}
	fim::string remove(const std::vector<fim::string> &args);
	fim::string info(const std::vector<fim::string> &args);
	fim::string scrolldown(const std::vector<fim::string> &args);
	fim::string scrollforward(const std::vector<fim::string> &args);
	fim::string info();
	std::ostream& print(std::ostream &os)const;
	void redisplay();
	fim::string redisplay(const std::vector<fim::string> &args);


	fim::string load(const std::vector<fim::string> &args);
	fim::string pop(const std::vector<fim::string> &args);
	const fim::string pop_current();
	fim::string pop_current(const std::vector<fim::string> &args){return pop_current();}
	bool present(const fim::string nf);
	bool push(const fim::string nf);

	fim::string display()
	{
		return display(std::vector<fim::string>());
	}
	private:
	fim::string reload();
	fim::string list()const;

	int n_files()const;
	const fim::string n()const;
	fim::string get()const;
	fim::string _sort();
	fim::string next(int n);
	fim::string do_next(int n);
	public:
};
}

#endif
