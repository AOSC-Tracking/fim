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
	int Browser::current_n()const{ return cp?cp-1:cp; }
	const fim::string Browser::pop();
	public:
	int Browser::current_image()const{ return cp; }
	int Browser::current_images()const{ return n(); }
	Browser::Browser();
	Browser::~Browser() { }
	fim::string Browser::current()const{ return cp?flist[current_n()]:nofile; }
	fim::string Browser::goto_image(const std::vector<fim::string> &args);
	fim::string Browser::goto_image(int n);
	fim::string Browser::pan_up(const std::vector<fim::string> &args);
	fim::string Browser::pan_down(const std::vector<fim::string> &args);
	fim::string Browser::auto_scale(const std::vector<fim::string> &args);
	fim::string Browser::auto_width_scale(const std::vector<fim::string> &args);
	fim::string Browser::auto_height_scale(const std::vector<fim::string> &args);
	fim::string Browser::pan_right(const std::vector<fim::string> &args);
	fim::string Browser::pan_left(const std::vector<fim::string> &args);
	fim::string Browser::reduce(const std::vector<fim::string> &args);
	fim::string Browser::magnify(const std::vector<fim::string> &args);
	fim::string Browser::scale_factor_increase(const std::vector<fim::string> &args);
	fim::string Browser::scale_factor_decrease(const std::vector<fim::string> &args);
	fim::string Browser::scale_factor_grow(const std::vector<fim::string> &args);
	fim::string Browser::scale_factor_shrink(const std::vector<fim::string> &args);
	fim::string Browser::display(const std::vector<fim::string> &args);
	fim::string Browser::display_status(const char *l,const char*r);

	fim::string Browser::reload(const std::vector<fim::string> &args);
	fim::string Browser::list(const std::vector<fim::string> &args){return list();}
	fim::string Browser::push(const std::vector<fim::string> &args)
	{
		for(int i=0;i<args.size();++i)
			push(args[i]);
		return "";
	}
	fim::string Browser::n(const std::vector<fim::string> &args){return n();}
	fim::string Browser::get(const std::vector<fim::string> &args){return get();}
	fim::string Browser::_sort(const std::vector<fim::string> &args){return _sort();}
	fim::string Browser::next(const std::vector<fim::string> &args){return next(args.size()>0?((int)args[0]):1);}
	fim::string Browser::prev(int n=1);
	fim::string Browser::prev(const std::vector<fim::string> &args){return prev(args.size()>0?((int)args[0]):1);}
	fim::string Browser::remove(const std::vector<fim::string> &args);
	fim::string Browser::info(const std::vector<fim::string> &args);
	fim::string Browser::scrolldown(const std::vector<fim::string> &args);
	fim::string Browser::info();
	std::ostream& Browser::print(std::ostream &os)const;
	void Browser::redisplay();
	fim::string Browser::redisplay(const std::vector<fim::string> &args);


	fim::string Browser::load(const std::vector<fim::string> &args);
	fim::string Browser::pop(const std::vector<fim::string> &args);
	bool Browser::present(const fim::string nf);
	bool Browser::push(const fim::string nf);

	fim::string Browser::display()
	{
		return display(std::vector<fim::string>());
	}
	private:
	fim::string Browser::reload();
	fim::string Browser::list()const;

	int Browser::n_files()const;
	const fim::string Browser::n()const;
	fim::string Browser::get()const;
	fim::string Browser::_sort();
	fim::string Browser::next(int n);
	fim::string Browser::do_next(int n);
	public:
};
};

#endif
