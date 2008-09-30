/* $Id$ */
/*
 Browser.cpp : Fim image browser

 (c) 2007-2008 Michele Martone

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

#define firstorzero(x) (x.size()?((int)(x[0])):0)

#include <dirent.h>
#include <sys/types.h>
#include "fim.h"

namespace fim
{
	int Browser::current_n()const{ return current_n(cp); }
	int Browser::current_n(int ccp)const{ return ccp?ccp-1:ccp; }

	fim::string Browser::list()const
	{
		/*
		 * returns a string with the info about the files in list
		 */
		fim::string fileslist;
		for(unsigned int i=0;i<flist.size();++i)fileslist+=flist[i]+fim::string(" ");
		cout << fileslist.c_str();
		return fileslist;
	}

	std::ostream& Browser::print(std::ostream &os)const
	{
		/*
		 * accessory method
		 */
		for(unsigned int i=0;i<flist.size();++i)
			os << flist[i] << "\n";
		return os;
	}


	fim::string Browser::redisplay(const args_t &args)
	{
		/*
		 * for now redisplaying is optionless
		 */
		redisplay();
		return "";
	}

	void Browser::redisplay()
	{
		/*
		 * Given a current() file, we display it again like 
		 * the first time it should be displayed.
		 * So, this behaviour is different from reloading..
		 */
		fim::string c=current();
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreRedisplay",c);
#endif
			if(c_image())
			{
				/*
				 * FIXME : philosophically, this is wrong.
				 * viewport().redisplay(); <- and this should be good
				 * :)
				 * */
				if( commandConsole.redisplay() )
					this->display_status(current().c_str(), NULL);
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostRedisplay",c);
#endif
		}
	}

#ifdef FIM_READ_STDIN_IMAGE
	void Browser::set_default_image(Image *stdin_image)
	{
		/*
		 * this is used mainly to set image files read from pipe or stdin
		 * */
		if(!stdin_image || stdin_image->check_invalid())return;

		if(default_image) delete default_image;
		default_image=stdin_image;
	}
#endif

	Browser::Browser(CommandConsole &cc):nofile(""),commandConsole(cc)
	{	
		/*
		 * we initialize to no file the current file name
		 */
#ifndef FIM_WINDOWS
		only_viewport = new Viewport();
		// DANGER !!
		// EXCEPTIONS NEEDED
#endif
		cp=0;	//and to file index 0 (no file)
	}

	const fim::string Browser::pop_current()
	{	
		/*
		 * pops the current image filename from the filenames list
		 * ( note that it doesn't refresh the image in any way ! )
		 *
		 * WARNING : SAME AS ERASE !
		 */
		fim::string s;
		if(flist.size()<=0)return nofile;
		assert(cp);
		flist.erase(flist.begin()+current_n());
		setGlobalVariable("filelistlen",current_images());
		return s;
	}

	const fim::string Browser::pop()
	{	
		/*
		 * pops the last image filename from the filenames list
		 * ( note that it doesn't refresh the image in any way ! )
		 */
		fim::string s;
		if(flist.size()<=0)return nofile;
		assert(cp);
		if(current_n()==(int)flist.size())cp--;
		s=flist[flist.size()-1];
		flist.erase(flist.begin()+current_n());
		setGlobalVariable("filelistlen",current_images());
		return s;
	}

	fim::string Browser::pan_up(const args_t &args)
	{
		/*
		 * pans the image up
		 * +----------+
		 * |          |
		 * |          |
		 * |   :-)    |
		 * +----------+
		 */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_up(firstorzero(args));
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		else prev();
		return "";
	}
	
	fim::string Browser::pan_down(const args_t &args)
	{
		/*
		 * pans the image down
		 * +----------+
		 * |   :-)    |
		 * |          |
		 * |          |
		 * +----------+
		 */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_down(firstorzero(args));
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		else next(1);
		return "";
	}

	fim::string Browser::pan_ne(const args_t &args)
	{
		/*
		 * pans the image up and right
		 * +----------+
		 * |          |
		 * |          |
		 * | :-)      |
		 * +----------+
		 */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_up();
			if(c_image() && viewport())viewport()->pan_right();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		//else prev();
		return "";
	}

	fim::string Browser::pan_nw(const args_t &args)
	{
		/*
		 * pans the image up and left
		 * +----------+
		 * |          |
		 * |          |
		 * |     :-)  |
		 * +----------+		 
		 * */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_up();
			if(c_image() && viewport())viewport()->pan_left();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		else prev();
		return "";
	}

	fim::string Browser::pan_sw(const args_t &args)
	{
		/*
		 * pans the image down and left
		 * +----------+
		 * |     :-)  |
		 * |          |
		 * |          |
		 * +----------+
		 */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_down();
			if(c_image() && viewport())viewport()->pan_left();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		//else next(0);
		return "";
	}

	fim::string Browser::pan_se(const args_t &args)
	{
		/*
		 * pans the image down and right
		 * +----------+
		 * | :-)      |
		 * |          |
		 * |          |
		 * +----------+
		 */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_down();
			if(c_image() && viewport())viewport()->pan_right();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		else next(0);
		return "";
	}

	fim::string Browser::scale_multiply(const args_t &args)
	{
		/*
		 * scales the image by a user specified factor
		 */
		double multiscale;
		if(args.size()==0)return "";
		multiscale=atof(args[0].c_str());
		if(multiscale==0.0)return "";
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image())image()->scale_multiply(multiscale);
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::scale_increment(const args_t &args)
	{
		/*
		 * increments the scale positively
		 */
		double deltascale;
		if(args.size()==0)return "";
		deltascale=atof(args[0].c_str());
		if(deltascale==0.0)return "";
		if(strstr(args[0].c_str(),"%"))deltascale*=.01;
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image())image()->scale_increment(deltascale);
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::scale(const args_t &args)
	{
		/*
		 * scales the image to a certain scale factor
		 */
		double newscale;
		if(args.size()==0)return "";
		newscale=atof(args[0].c_str());
		if(newscale==0.0)return "";
		if(strstr(args[0].c_str(),"%"))newscale*=.01;
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image())image()->setscale(newscale);
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::auto_height_scale(const args_t &args)
	{
		/*
		 * scales this image to fit in the screen in the vertical dimension
		 */
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image() && viewport())viewport()->auto_height_scale();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::auto_width_scale(const args_t &args)
	{
		/*
		 * scales this image to fit in the screen in the horizontal dimension
		 */
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			autocmd_exec("PreScale",c);
#endif
			if(c_image() && viewport())viewport()->auto_width_scale();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::auto_scale(const args_t &args)
	{
		/*
		 * auto scale the image accordingly to the *default* settings !
		 */
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image() && viewport())viewport()->auto_scale();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::pan_right(const args_t &args)
	{
		/*
		 * pans the image right
		 */
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PrePan",c);
#endif
			if(c_image())viewport()->pan_right(firstorzero(args));
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		else next(1);
		return "";
	}

	fim::string Browser::pan_left(const args_t &args)
	{
		/*
		 * pans the image left
		 */
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())viewport()->pan_left(firstorzero(args));
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		else prev();
		return "";
	}

	fim::string Browser::display_status(const char *l,const char *r)
	{
		/*
		 * displays the left text message and a right bracketed one
		 */
		if(getGlobalIntVariable("_display_status"))
			commandConsole.set_status_bar((const char*)l, image()?(image()->getInfo().c_str()):"*");
		return "";
	}

	fim::string Browser::display(const args_t &args)
	{
		/*
		 * displays the current image, (if already loaded), on screen
		 */
		fim::string c=current();
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreDisplay",c);
#endif
			/*
			 * the following is a trick to override redisplaying..
			 */
			if(image() && getGlobalIntVariable("_override_display")==0)
			//	if(c_image())
			{
				//fb_clear_screen();
				//viewport().display();
				/*
				 * we redraw the whole screen and thus all of the windows
				 * */
				if( commandConsole.display() )
					this->display_status(current().c_str(), NULL);
//				FIXME:
//				if(commandConsole.window)commandConsole.window->recursive_display();
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostDisplay",c);
#endif
		}
		else{ cout << "no image to display, sorry!";
		commandConsole.set_status_bar("no image loaded.", "*");}
		return "";
	}

	fim::string Browser::no_image(const args_t &args)
	{
		/*
		 * sets no image as the current one
		 * */
		free_current_image();
		return "";
	}

	int Browser::load_error_handle(fim::string c)
	{
		/*
		 * assume there was a load attempt : check and take some action in case of error
		 *
		 * FIXME : this behaviour is BUGGY, because recursion will be killed off 
		 *         by the autocommand loop prevention mechanism.
		 * */
		if(image() && viewport() && ! (viewport()->check_valid()))
		{
			free_current_image();
#ifdef FIM_REMOVE_FAILED
			if(current()!="")
			{
				pop_current();	//removes the current file from the list.
#ifdef FIM_AUTOSKIP_FAILED
#ifdef FIM_ALWAYS_UNDEFINED
				if(n_files())next(1);
				reload(); /* this will not be effective ! */
#endif
#endif
			}
#endif
			return 1;
		}
		return 0;
	}

	fim::string Browser::reload()
	{
		/*
		 * FIXME
		 *
		 * reload the current filename
		 * */
		if(n_files())
		return reload(args_t());
		return "";
	}

	fim::string Browser::loadCurrentImage()
	{
		/*
		 * FIXME
		 *
		 * an attempt to load the current image
		 * */
		try
		{
#ifndef FIM_BUGGED_CACHE
		if(viewport())viewport()->setImage( cache.useCachedImage(current().c_str()) );
#else
		// warning : in this cases exception handling is missing
	#ifdef FIM_READ_STDIN_IMAGE
		if(current()!="")
		{
			if(viewport())viewport()->setImage( new Image(current().c_str()) );
		}
		else
		{
			if( viewport() && default_image )
			{
				// a one time only image (new, experimental)
				viewport()->setImage(default_image->getClone());
				//default_image=NULL;
			}
		}
	#else
		if(viewport())viewport()->setImage( new Image(current().c_str()) );
	#endif
#endif
		}
		catch(FimException e)
		{
			if(viewport())viewport()->setImage( NULL );
//		commented temporarily for safety reasons
//			if( e != FIM_E_NO_IMAGE )throw FIM_E_TRAGIC;  /* hope this never occurs :P */
		}
		return "";
	}

	void Browser::free_current_image()
	{
		/*
		 * FIXME
		 * only cleans up the internal data structures
		 * */
		if(viewport())viewport()->free();
		setGlobalVariable("_cache_status",cache.getReport().c_str());
	}

	fim::string Browser::prefetch(const args_t &args)
	{
		/*
		 * fetches in the cache the next image..
		 *
		 * FIX ME : enrich this behaviour
		 * */
#ifdef FIM_BUGGED_CACHE
		return " prefetching disabled";
#endif

		if( args.size() > 0 )return "";
		cache.prefetch(get_next_filename(1).c_str());
		return "";
	}

	bool Browser::can_reload()const
	{
		const Image *i=NULL;
		if( commandConsole.current_viewport() && (i=commandConsole.current_viewport()->c_getImage())!=NULL )
			return i->can_reload();
		return true;
	}

	fim::string Browser::reload(const args_t &args)
	{
		/*
		 * deletes the structures associated to the present image
		 * and then
		 * tries to load a new one from the current filename
		 */
		fim::string c=current();
		if(empty_file_list())return "sorry, no image to reload\n";
#ifdef FIM_AUTOCMDS
		autocmd_exec("PreReload",c);
#endif
		free_current_image();
		if(can_reload())
		{
			loadCurrentImage();
		}

		if(getGlobalIntVariable("_prefetch")) prefetch(args_t());/*this will become an autocommand*/

		while( n_files() && viewport() && ! (viewport()->check_valid() ) && load_error_handle(c) );

#ifdef FIM_AUTOCMDS
		autocmd_exec("PostReload",c);
#endif
		return "";
	}

	fim::string Browser::load(const args_t &args)
	{
		/*
		 * loads the current file, if not already loaded
		 */
		fim::string c=current();
		if(image() && ( image()->getName() == current()) )
		{
			return "image already loaded\n";		//warning
		}
		if(empty_file_list())return "sorry, no image to load\n";	//warning
#ifdef FIM_AUTOCMDS
		autocmd_exec("PreLoad",c);
#endif
		commandConsole.set_status_bar("please wait while loading...", "*");

		loadCurrentImage();

		if(getGlobalIntVariable("_prefetch")) prefetch(args_t());

//		if(image() && ! (viewport().check_valid()))return load_error_handle(c);
#ifdef FIM_AUTOCMDS
		autocmd_exec("PostLoad",c);
#endif
		return "";
	}

	fim::string Browser::pop(const args_t &args)
	{
		/*
		 * deletes the last image from the files list
		 * FIX ME :
		 * filename matching based remove..
		 */
		pop();
		return this->n();
	}
	
	bool Browser::present(const fim::string nf)
	{
		/* 
		 * returns whether the file nf is in the files list
		 */
		for(unsigned int i=0;i<flist.size();++i)
			if(flist[i]==nf)return true;
		return false;
	}

#ifdef FIM_READ_DIRS
	bool Browser::push_dir(fim::string nf)
	{	
		int d_n;
		DIR *dir;
		struct dirent *de;
		fim::string f;

		/*	we want a dir .. */
		if( !is_dir( nf ))return false;
		
		if ( ! ( dir = opendir(nf.c_str() ) ))
			return false;

		f+=nf;
		f+="/";
		//are we sure -1 is not paranoid ?
		while( ( de = readdir(dir) ) != NULL )
		{
			if( de->d_name[0] == '.' &&  de->d_name[1] == '.' && !de->d_name[2] ) continue;
			if( de->d_name[0] == '.' && !de->d_name[1] ) continue;
#if 1
			/*
			 * We follow the convention of ignoring hidden files.
			 * */
			if( de->d_name[0] == '.' ) continue;
#endif
			
			/*
			 * Warning : this is dangeous, as following circualr links could cause memory exhaustion.
			 * */
			if(is_dir( f+fim::string(de->d_name)))
#ifdef FIM_RECURSIVE_DIRS
				push_dir(f+fim::string(de->d_name));
#else
				continue;
#endif
			else 
				push(f+fim::string(de->d_name));
		}
		return closedir(dir)!=NULL;
	}

	bool Browser::is_dir(fim::string nf)const
	{
		struct stat stat_s;
		/*	if the directory doesn't exist, return */
		if(-1==stat(nf.c_str(),&stat_s))return false;
		if( ! S_ISDIR(stat_s.st_mode))return false;
		return true;
	}
#endif

	bool Browser::push(fim::string nf)
	{	
		/*
		 * FIX ME:
		 * are we sure we want no repetition!????
		 * */
		if(nf!="")
		{
#ifdef FIM_CHECK_FILE_EXISTENCE
		/*
		 * skip adding the filename in the list if
		 * it is not existent or it is a directory...
		 */
		struct stat stat_s;
		/*	if the file doesn't exist, return */
		if(-1==stat(nf.c_str(),&stat_s))return false;
		/*	if it is a char device , return */
		//if(  S_ISCHR(stat_s.st_mode))return "";
		/*	if it is a block device , return */
		//if(  S_ISBLK(stat_s.st_mode))return "";
		/*	if it is a directory , return */
		//if(  S_ISDIR(stat_s.st_mode))return "";
#ifdef FIM_READ_DIRS
		if(  S_ISDIR(stat_s.st_mode))return push_dir(nf);
#endif
		/*	we want a regular file .. */
		if(! S_ISREG(stat_s.st_mode))
		{
			/*
			 * i am not fully sure this is effective
			 * */
			nf+=" is not a regular file!";
			commandConsole.set_status_bar(nf.c_str(), "*");
			return false;
		}
#endif

		}
#ifdef FIM_CHECK_DUPLICATES
		if(present(nf))
		{
			//there could be an option to have duplicates...
			//std::cout << "no duplicates allowed..\n";
			return false;
		}
#endif
		flist.push_back(nf);
		setGlobalVariable("filelistlen",current_images());
		if(cp==0)++cp;
		return false;
	}
	
	int Browser::n_files()const
	{
		/*
		 * the number of files in the filenames list
		 */
		return flist.size();
	}

	const fim::string Browser::n()const
	{
		/*
		 * the number of files in the filenames list
		 */
		return fim::string(n_files());
	}

	fim::string Browser::_sort()
	{
		/*
		 *	sorts the image filenames list
		 */
		sort(flist.begin(),flist.end());
		return n_files()?(flist[current_n()]):nofile;
	}

	fim::string Browser::regexp_goto_next(const args_t &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		args_t arg;
		arg.push_back(last_regexp);
		return regexp_goto(arg);
	}

	fim::string Browser::regexp_goto(const args_t &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		unsigned int i,j,c=current_n(),s=flist.size();
		if( args.size() < 1 || s < 1 )return "";
		for(j=0;j<s;++j)
		{
			last_regexp=args[0];
			i=(j+c+1)%s;
			if(commandConsole.regexp_match(flist[i].c_str(),args[0].c_str()))
			{	
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				autocmd_exec("PreGoto",c);
#endif
				goto_image(i+1);
#ifdef FIM_AUTOCMDS
				autocmd_exec("PostGoto",c);
				if(!commandConsole.inConsole())
					commandConsole.set_status_bar((current()+fim::string(" matches \"")+args[0]+fim::string("\"")).c_str(),NULL);
				return "";
#endif
			}
		}
		cout << "sorry, no filename matches \""<<args[0]<<"\"\n";
		if(!commandConsole.inConsole())
			commandConsole.set_status_bar((fim::string("sorry, no filename matches \"")+
						args[0]+
						fim::string("\"")).c_str(),NULL);
		
		return "";
	}

	fim::string Browser::goto_image(int n)
	{
		/*
		 *	FIX ME
		 */
		int N=flist.size();
		if(!N)return "";
		cp=n;
		if(cp<0)cp=(cp%N)+N+1;//+1 added lately
		if(cp>N) cp=1+(n%N);
		if(!cp)++cp;
		setGlobalVariable("fileindex",current_image());
		setGlobalVariable("filename", current().c_str());
		fim::string result = n_files()?(flist[current_n()]):nofile;
		return result;
	}

	fim::string Browser::next(int n=1)
	{
		/*
		 * jumps n images forward in the image list
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		autocmd_exec("PreNext",c);
#endif
		fim::string result=do_next(n);
#ifdef FIM_AUTOCMDS
		autocmd_exec("PostNext",c);
#endif
		return "";
	}

	fim::string Browser::prev(int n)
	{
		/*
		 * jumps n images backwards in the image list
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		autocmd_exec("PrePrev",c);
#endif
		fim::string result=do_next(-n);
#ifdef FIM_AUTOCMDS
		autocmd_exec("PostPrev",c);
#endif
		return "";
	}

	fim::string Browser::prev(const args_t &args)
	{
		return prev(args.size()>0?((int)args[0]):1);
	}

	fim::string Browser::get_next_filename(int n)const
	{
		/*
		 * returns to the next image in the list, the mechanism
		 * p.s.: n<>0
		 */
		int ccp=cp;
		int N=flist.size();
		if(!N)return "";
		ccp+=n;
		ccp%=N;
		ccp+=N;
		ccp%=N;
		if(!ccp)ccp=N;
		return flist[current_n(ccp)];
	}


	fim::string Browser::do_next(int n)
	{
		/*
		 * jumps to the next n'th image in the list.
		 * p.s.: n<>0
		 */
		int N=flist.size();
		if(!N)return "";
		cp+=n;
		cp%=N;
		cp+=N;
		cp%=N;
		if(!cp)cp=N;
		setGlobalVariable("fileindex",current_image());
		setGlobalVariable("filename", current().c_str());
		fim::string result = n_files()?(flist[current_n()]):nofile;
		return "";
	}
	
	fim::string Browser::goto_image(const args_t &args)
	{
		/*
		 *	FIX ME
		 *	there should be a way to have an interactive goto
		 */
		if(n_files()==0)return "no image to go to!";
		int g=current_n()+1;
		if(args.size()<1)return "please specify a file to view ( a number or ^ or $ ) \n";
		else
		{
			char c=*(args[0].c_str());
			if(isdigit(c)  || c=='-')g=atoi(args[0].c_str());
			else if(c=='^' || c=='f')g=1;
			else if(c=='$' || c=='l')g=n();
			else cout << " please specify a number or ^ or $\n";
			//if(g!=-1)
			{	
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				autocmd_exec("PreGoto",c);
#endif
				goto_image(g);
#ifdef FIM_AUTOCMDS
				autocmd_exec("PostGoto",c);
#endif
			}
		}
		return "";
	}

	fim::string Browser::remove(const args_t &args)
	{
		/*
		 *	ONLY if the image filename exists and matches EXACTLY,
		 *
		 *	FIXME : dangerous!
		 */
		if(flist.size()<1)return "the files list is empty\n";
		args_t rlist=args;	//the remove list
		if(rlist.size()>0)
		{
			/*
			 * the list is unsorted. it may contain duplicates
			 * if this software will have success, we will have indices here :)
			 * sort(rlist.begin(),rlist.end());...
			 */
			for(unsigned int r=0;r<rlist.size();++r)
			for(unsigned int i=0;i<flist.size();++i)
			if(flist[i]==rlist[r])
			{
//				std::cout << "removing" << flist[i]<<"\n";
				flist.erase(flist.begin()+i);
			}
			return "";
		}
		else
		{
			/*
			 * removes the current file from the list
			 */
/*			if(cp-1==current_n())--cp;
			flist.erase(flist.begin()+current_n());
			if(cp==0 && n_files()) ++cp;
			return "";*/
			return pop_current();
		}
		return "";
	}

	fim::string Browser::scrollforward(const args_t &args)
	{
		/*
		 * scrolls the image as it were a book :)
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PrePan",c);
#endif
		if(c_image() && viewport())
		{
			if(viewport()->onRight() && viewport()->onBottom())
				next();
			else
			if(viewport()->onRight())
			{
				viewport()->pan_down();
				while(!(viewport()->onLeft()))viewport()->pan_left();
			}
			else viewport()->pan_right();
		}
		else next(1);
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		return "";
	}

	fim::string Browser::scrolldown(const args_t &args)
	{
		/*
		 * scrolls the image down 
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PrePan",c);
#endif
		if(c_image() && viewport())
		{
			if(viewport()->onBottom()) next();
			else pan_down(args_t());
		}
		else next(1);
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		return "";
	}

	fim::string Browser::info(const args_t &args)
	{
		/*
		 *	short information in status-line format
		 */
#if 0
		string fl;
		for(unsigned int r=0;r<flist.size();++r)
		{
			fl+=flist[r];
			fl+="\n";
		}
		return fl;
#else
		fim::string r=current();
		if(image())
			r+=image()->getInfo();
		else
			r+=" (unloaded)";
		return r;
#endif
	//	return list();
	}

	fim::string Browser::info()
	{
		/*
		 *	short information in status-line format
		 */
		return info(args_t(0));
	}

	fim::string Browser::scale_factor_grow(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfm=getGlobalFloatVariable("scale_factor_multiplier");if(sfm<=1.0f)sfm=1.1f;
		setGlobalVariable("reduce_factor",getGlobalFloatVariable("reduce_factor")*sfm);
		setGlobalVariable("magnify_factor",getGlobalFloatVariable("magnify_factor")*sfm);
		return "";
	}

	fim::string Browser::scale_factor_shrink(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfm=getGlobalFloatVariable("scale_factor_multiplier");if(sfm<=1.0f)sfm=1.1f;
		setGlobalVariable("reduce_factor",getGlobalFloatVariable("reduce_factor")/sfm);
		setGlobalVariable("magnify_factor",getGlobalFloatVariable("magnify_factor")/sfm);
		return "";
	}

	fim::string Browser::scale_factor_increase(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfd=getGlobalFloatVariable("scale_factor_delta");if(sfd<=0.0f)sfd=0.1f;
		setGlobalVariable("reduce_factor",getGlobalFloatVariable("reduce_factor")+sfd);
		setGlobalVariable("magnify_factor",getGlobalFloatVariable("magnify_factor")+sfd);
		return "";
	}

	fim::string Browser::scale_factor_decrease(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfd=getGlobalFloatVariable("scale_factor_delta");if(sfd<=0.0f)sfd=0.1f;
//		setGlobalVariable("scale_factor",getGlobalFloatVariable("scale_factor")/1.1f);
		setGlobalVariable("reduce_factor",getGlobalFloatVariable("reduce_factor")-sfd);
		setGlobalVariable("magnify_factor",getGlobalFloatVariable("magnify_factor")-sfd);
		return "";
	}


	fim::string Browser::magnify(const args_t &args)
	{
		/*
		 * magnifies the displayed image
		 */ 
		if(c_image())
		{
			float factor = (float)getGlobalFloatVariable("magnify_factor");
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image())
			{
				if(factor)
					{if(image())image()->magnify(factor);}
				else	
					{if(image())image()->magnify();}
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::reduce(const args_t &args)
	{
		/*
		 * reduces the displayed image size
		 */ 
		if(c_image())
		{
			float factor = (float)getGlobalFloatVariable("reduce_factor");
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PreScale",c);
#endif
			if(c_image())
			{
				if(factor) {if(image())image()->reduce(factor);}
				else	{if(image())image()->reduce();}
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::top_align(const args_t &args)
	{
		/*
		 * aligns to top the displayed image
		 */ 
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())
			{
				viewport()->top_align();
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	fim::string Browser::bottom_align(const args_t &args)
	{
		/*
		 * aligns to the bottom the displayed image
		 */ 
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec("PrePan",c);
#endif
			if(c_image() && viewport())
			{
				viewport()->bottom_align();
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	const Image *Browser::c_image()const
	{
		/*
		 *	a const pointer to the currently loaded image
		 */
	#ifdef FIM_WINDOWS
		if( commandConsole.current_viewport() )
			return commandConsole.current_viewport()->c_getImage();
		else
			return NULL;
	#else
		if(!only_viewport)return NULL;
		return only_viewport->c_getImage();
	#endif
	}

	Image *Browser::image()const
	{
		/*
		 *	the image loaded in the current viewport is returned
		 */
	#ifdef FIM_WINDOWS
		if( commandConsole.current_viewport() )
			return commandConsole.current_viewport()->getImage();
		else
			return NULL;
	#else
		if(!only_viewport)return NULL;
		return only_viewport->getImage();
	#endif
	}

	Viewport* Browser::viewport()const
	{
		/* 
		 * A valid pointer will be returned 
		 * whenever the image is loaded !
		 *
		 * NULL is returned in case no viewport is loaded.
		 * */
#ifdef FIM_WINDOWS

		return (commandConsole.current_viewport());
#else
		return only_viewport;
#endif
	}

	fim::string Browser::current()const
	{
		/*
		 * dilemma : should the current() filename and next() operations
		 * be relative to viewport's own current's ?
		 * */
		if(empty_file_list())return nofile; // FIXME: patch!
	       	return cp?flist[current_n()]:nofile;
	}

	int Browser::empty_file_list()const
	{
		/*
		 *	is the filename list empty ?
		 */
		return flist.size()==0;
	}

	fim::string Browser::display()
	{
		/*
		 *	display the current image
		 */
		return display(args_t());
	}

	fim::string Browser::pop_current(const args_t &args)
	{
		/*
		 *	pops the last image filename off the image list
		 */
		return pop_current();
	}

	fim::string Browser::push(const args_t &args)
	{
		/*
		 *	pushes a new image filename on the back of the image list
		 */
		for(unsigned int i=0;i<args.size();++i)
		{
#ifdef FIM_SMART_COMPLETION
			/* due to this patch, filenames could arrive here with some trailing space. 
			 * we trim them here, which is not correct if someone intends to push
			 * a space-trailing filenme.
			 * 
			 * FIXME : regard this as a bug
			 * */
			fim::string ss=args[i];
			ss.substitute(" +$","");
			push(ss);
#else
			push(args[i]);
#endif
		}
		return "";
	}

	fim::string Browser::next(const args_t &args)
	{
		/*
		 * FIX ME
		 * */
		return next(args.size()>0?((int)args[0]):1);
	}

	int Browser::current_image()const
	{
		/*
		 * FIX ME
		 */
		return cp;
	}
}

