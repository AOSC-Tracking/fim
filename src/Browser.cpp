/* $LastChangedDate$ */
/*
 Browser.cpp : Fim image browser

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

#define firstorzero(x) (x.size()?((int)(x[0])):0)
#define firstforzero(x) (x.size()?((float)(x[0])):0.0)

#include <dirent.h>
#include <sys/types.h>	/* POSIX Standard: 2.6 Primitive System Data Types (e.g.: ssize_t) */
#include "fim.h"

namespace fim
{
	int Browser::current_n()const{ return current_n(cp_); }
	int Browser::current_n(int ccp)const{ return ccp?ccp-1:ccp; }

	fim::string Browser::list()const
	{
		/*
		 * returns a string with the info about the files in list
		 */
		fim::string fileslist;
		for(size_t i=0;i<flist_.size();++i)fileslist+=flist_[i]+fim::string(" ");
		return fileslist;
	}

	fim::string Browser::fcmd_list(const args_t &args)
	{
		/*
		 */
		if(args.size()<1)
		{
			return list();
			goto nop;
		}
		else
		{
			if(args[0]=="random_shuffle")
				return _random_shuffle();
			if(args[0]=="sort")
				return _sort();
			if(args[0]=="reverse")
				return _reverse();
			if(args[0]=="pop")
			{
				/*
				 * deletes the last image from the files list
				 * FIX ME :
				 * filename matching based remove..
		 		*/
				pop();
				return this->n();
			}
			if(args[0]=="remove")
			{
				args_t argsc(args);
				argsc.erase(argsc.begin());
				return do_remove(argsc);
			}
			if(args[0]=="push")
			{
				args_t argsc(args);
				argsc.erase(argsc.begin());
				return do_push(argsc);
			}
			if(args[0]=="filesnum")
			{
				return n();
			}
#if FIM_WANT_FILENAME_MARK_AND_DUMP
			if(args[0]=="mark")
			{ cc.markCurrentFile(); goto nop; } 
			if(args[0]=="unmark")
			{ cc.unmarkCurrentFile(); goto nop; } 
#else
			if(args[0]=="mark")
				return "sorry, mark functionality was opted out.";
			if(args[0]=="unmark")
				return "sorry, mark functionality was opted out.";
#endif
			return FIM_CMD_HELP_LIST;
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	std::ostream& Browser::print(std::ostream &os)const
	{
		/*
		 * accessory method
		 */
		for(size_t i=0;i<flist_.size();++i)
			os << flist_[i] << "\n";
		return os;
	}


	fim::string Browser::fcmd_redisplay(const args_t &args)
	{
		/*
		 * for now redisplaying is optionless
		 */
		redisplay();
		return FIM_CNS_EMPTY_RESULT;
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
			autocmd_exec(FIM_ACM_PREREDISPLAY,c);
#endif
			if(c_image())
			{
				/*
				 * FIXME : philosophically, this is wrong.
				 * viewport().redisplay(); <- and this should be good
				 * :)
				 * */
				if( commandConsole_.redisplay() )
					this->display_status(current().c_str(), NULL);
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTREDISPLAY,c);
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

		if(default_image_) delete default_image_;
		default_image_=stdin_image;
	}
#endif

	Browser::Browser(CommandConsole &cc):nofile_(FIM_CNS_EMPTY_STRING),commandConsole_(cc)
#ifdef FIM_NAMESPACES
		,Namespace(FIM_SYM_NAMESPACE_BROWSER_CHAR)
#endif
	{	
		/*
		 * we initialize to no file the current file name
		 */
		cp_=0;	//and to file index 0 (no file)
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
		if(flist_.size()<=0)return nofile_;
		assert(cp_);
		flist_.erase(flist_.begin()+current_n());
		setGlobalVariable(FIM_VID_FILELISTLEN,current_images());
		return s;
	}

	const fim::string Browser::pop(fim::string filename)
	{	
		/*
		 * pops the last image filename from the filenames list
		 * ( note that it doesn't refresh the image in any way ! )
		 */
		fim::string s;
		if(flist_.size()<=0)return nofile_;
		assert(cp_);
		if(filename==FIM_CNS_EMPTY_STRING)
		{
			if(current_n()==(int)flist_.size())cp_--;
			s=flist_[flist_.size()-1];
			flist_.erase(flist_.begin()+current_n());
		}
		else
		{
			// FIXME: shall use a search method/function
			for(size_t i=0;i<flist_.size();++i)
				if(flist_[i]==filename)
					flist_.erase(flist_.begin()+i);
		}
		setGlobalVariable(FIM_VID_FILELISTLEN,current_images());
		return s;
	}

	fim::string Browser::pan(const args_t &args)
	{
		/*
		 * pans the image left
		 * FIXME: unfinished
		 */
		if(args.size()<1 || (!args[0].c_str()))goto nop;
		if(c_image())
		{
			fim::string c=current();
			fim_char_t f=tolower(*args[0].c_str());
			if(f)
			{
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PREPAN,c);
#endif
			if(c_image() && viewport())
				viewport()->pan(args);
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTPAN,c);
#endif
			}
		}
		else prev();
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scale_multiply(const args_t &args)
	{
		/*
		 * scales the image by a user specified factor
		 */
		fim_scale_t multiscale;
		if(args.size()==0)goto nop;
		multiscale=fim_atof(args[0].c_str());
		if(multiscale==0.0)goto nop;
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PRESCALE,c);
#endif
			if(c_image())image()->scale_multiply(multiscale);
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTSCALE,c);
#endif
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}
	
	fim::string Browser::fcmd_scale_increment(const args_t &args)
	{
		/*
		 * increments the scale positively
		 */
		fim_scale_t deltascale;
		if(args.size()==0)goto nop;
		deltascale=fim_atof(args[0].c_str());
		if(deltascale==0.0)goto nop;
		if(strstr(args[0].c_str(),"%"))deltascale*=.01;
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PRESCALE,c);
#endif
			if(c_image())image()->scale_increment(deltascale);
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTSCALE,c);
#endif
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scale(const args_t &args)
	{
		/*
		 * scales the image to a certain scale factor
		 * FIXME: no user error checking -- poor error reporting for the user
		 * TODO: wxh / w:h syntax needed
		 */
		fim_scale_t newscale;
		fim_char_t fc=FIM_SYM_CHAR_NUL;
		const fim_char_t*ss=NULL;
		if(args.size()<1 || !(ss=args[0].c_str()))goto nop;
		fc=tolower(*ss);
		if(isalpha(fc))
		{
			if( !(fc=='w'||fc=='h'||fc=='a'))
				goto nop;
		}
		else
		{
			newscale=fim_atof(ss);
			if(newscale==FIM_CNS_SCALEFACTOR_ZERO)
			{
				goto nop;
			}
			if(strstr(ss,"%"))
				newscale*=.01;
		}
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PRESCALE,c);
#endif
			if(c_image())
			switch(fc)
			{
				case('w'):
				if(viewport())viewport()->auto_width_scale();
				break;
				case('h'):
				if(viewport())viewport()->auto_height_scale();
				break;
				case('a'):
				if(viewport())viewport()->auto_scale();
				break;
				default:
				image()->setscale(newscale);
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTSCALE,c);
#endif
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}
	
	fim::string Browser::fcmd_negate(const args_t &args)
	{
		/*
		 */
		if(!image() )
			goto nop;

		if(image() && image()->negate())
			goto nop;
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::display_status(const char *l,const char *r)
	{
		/*
		 * displays the left text message and a right bracketed one
		 */
		if(getGlobalIntVariable(FIM_VID_DISPLAY_STATUS))
			commandConsole_.set_status_bar((const char*)l, image()?(image()->getInfo().c_str()):"*");
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_display(const args_t &args)
	{
		/*
		 * displays the current image, (if already loaded), on screen
		 */
		fim::string c=current();
		if(c_image())
		{
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PREDISPLAY,c);
#endif
			/*
			 * the following is a trick to override redisplaying..
			 */
			if(image() && getGlobalIntVariable(FIM_VID_OVERRIDE_DISPLAY)==0)
			//	if(c_image())
			{
				//fb_clear_screen();
				//viewport().display();
				/*
				 * we redraw the whole screen and thus all of the windows
				 * */
				if( commandConsole_.display() )
					this->display_status(current().c_str(), NULL);
//				FIXME:
//				if(commandConsole_.window)commandConsole_.window->recursive_display();
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTDISPLAY,c);
#endif
		}
		else{ cout << "no image to display, sorry!";
		commandConsole_.set_status_bar("no image loaded.", "*");}
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_no_image(const args_t &args)
	{
		/*
		 * sets no image as the current one
		 * */
		free_current_image();
		return FIM_CNS_EMPTY_RESULT;
	}

	int Browser::load_error_handle(fim::string c)
	{
		/*
		 * assume there was a load attempt : check and take some action in case of error
		 *
		 * FIXME : this behaviour is BUGGY, because recursion will be killed off 
		 *         by the autocommand loop prevention mechanism. (this is not true, as 20090215)
		 * */
		static int lehsof=0;	/* './fim FILE NONFILE' and hitting 'prev' will make this necessary  */

		if(lehsof)return 0; /* this prevents infinite recursion */
		if(/*image() &&*/ viewport() && ! (viewport()->check_valid()))
		{
			free_current_image();
			++lehsof;
#ifdef FIM_REMOVE_FAILED
				//pop(c);	//removes the currently specified file from the list. (pop doesn't work in this way)
				args_t args;
				args.push_back(c.c_str());
				do_remove(args);	// remove is an experimental function
#ifdef FIM_AUTOSKIP_FAILED
				if(n_files())
				{
					//next(1);
					reload(); /* this is effective, at least partially */
				}
#endif
#endif
			--lehsof;
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
			return fcmd_reload(args_t());
		return FIM_CNS_EMPTY_RESULT;
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
	#ifdef FIM_CACHE_DEBUG
		if(viewport())std::cout << "browser::loadCurrentImage(\"" << current().c_str() << "\")\n";
	#endif
		if(viewport())viewport()->setImage( cache_.useCachedImage(cache_key_t(current(),(current()==FIM_STDIN_IMAGE_NAME)?FIM_E_STDIN:FIM_E_FILE)) );// FIXME
#else
		// warning : in this cases exception handling is missing
	#ifdef FIM_READ_STDIN_IMAGE
		if(current()!=FIM_STDIN_IMAGE_NAME)
		{
			if(viewport())viewport()->setImage( new Image(current().c_str()) );
		}
		else
		{
			if( viewport() && default_image_ )
			{
				// a one time only image (new, experimental)
				viewport()->setImage(default_image_->getClone());
				//default_image_=NULL;
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
		return FIM_CNS_EMPTY_RESULT;
	}

	void Browser::free_current_image()
	{
		/*
		 * FIXME
		 * only cleans up the internal data structures
		 * */
		if(viewport())viewport()->free();
		setGlobalVariable(FIM_VID_CACHE_STATUS,cache_.getReport().c_str());
	}

	fim::string Browser::fcmd_prefetch(const args_t &args)
	{
		/*
		 * fetches in the cache_ the next image..
		 *
		 * FIX ME : enrich this behaviour
		 * */
#ifdef FIM_BUGGED_CACHE
		return " prefetching disabled";
#endif

#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PREPREFETCH,current());
#endif
		if( args.size() > 0 )return FIM_CNS_EMPTY_RESULT;

		setGlobalVariable(FIM_VID_WANT_PREFETCH,0);
		if(cache_.prefetch(cache_key_t(get_next_filename( 1).c_str(),FIM_E_FILE)))// we prefetch 1 file forward
#ifdef FIM_AUTOSKIP_FAILED
			pop(get_next_filename( 1));/* if the filename doesn't match a loadable image, we remove it */
#else
			{}	/* beware that this could be dangerous and trigger loops */
#endif
		if(cache_.prefetch(cache_key_t(get_next_filename(-1).c_str(),FIM_E_FILE)))// we prefetch 1 file backward
#ifdef FIM_AUTOSKIP_FAILED
			pop(get_next_filename(-1));/* if the filename doesn't match a loadable image, we remove it */
#else
			{}	/* beware that this could be dangerous and trigger loops */
#endif
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTPREFETCH,current());
#endif
		setGlobalVariable(FIM_VID_WANT_PREFETCH,1);
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_reload(const args_t &args)
	{
		/*
		 * deletes the structures associated to the present image
		 * and then
		 * tries to load a new one from the current filename
		 */
		fim::string c=current();
		//for(size_t i=0;i<args.size();++i) push(args[i]);
		if(empty_file_list())return "sorry, no image to reload\n";
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_PRERELOAD,c);
#endif
		free_current_image();
		loadCurrentImage();
		//if(image())image()->reload();

//		while( n_files() && viewport() && ! (viewport()->check_valid() ) && load_error_handle(c) );
		load_error_handle(c);
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_POSTRELOAD,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_load(const args_t &args)
	{
		/*
		 * loads the current file, if not already loaded
		 */
		fim::string c=current();
		//for(size_t i=0;i<args.size();++i) push(args[i]);
		if(image() && ( image()->getName() == current()) )
		{
			return "image already loaded\n";		//warning
		}
		if(empty_file_list())return "sorry, no image to load\n";	//warning
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_PRELOAD,c);
#endif
		commandConsole_.set_status_bar("please wait while loading...", "*");

		loadCurrentImage();

		load_error_handle(c);
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_POSTLOAD,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	bool Browser::present(const fim::string nf)
	{
		/* 
		 * returns whether the file nf is in the files list
		 */
		for(size_t i=0;i<flist_.size();++i)
			if(flist_[i]==nf)return true;
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
		f+=FIM_CNS_DIRSEP_STRING;
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
#endif

	bool Browser::push(fim::string nf)
	{	
		/*
		 * FIX ME:
		 * are we sure we want no repetition!????
		 * */
		if(nf!=FIM_STDIN_IMAGE_NAME)
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
		//if(  S_ISCHR(stat_s.st_mode))return FIM_CNS_EMPTY_RESULT;
		/*	if it is a block device , return */
		//if(  S_ISBLK(stat_s.st_mode))return FIM_CNS_EMPTY_RESULT;
		/*	if it is a directory , return */
		//if(  S_ISDIR(stat_s.st_mode))return FIM_CNS_EMPTY_RESULT;
#ifdef FIM_READ_DIRS
		if(  S_ISDIR(stat_s.st_mode))return push_dir(nf);
#endif
		/*	we want a regular file .. */
		if(
			! S_ISREG(stat_s.st_mode) 
#define FIM_READ_BLK_DEVICES 1
#ifdef FIM_READ_BLK_DEVICES
			&& ! S_ISBLK(stat_s.st_mode)  // NEW
#endif
		)
		{
			/*
			 * i am not fully sure this is effective
			 * */
			nf+=" is not a regular file!";
			commandConsole_.set_status_bar(nf.c_str(), "*");
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
		flist_.push_back(nf);

		setGlobalVariable(FIM_VID_FILELISTLEN,current_images());
		if(cp_==0)++cp_;
		return false;
	}
	
	int Browser::n_files()const
	{
		/*
		 * the number of files in the filenames list
		 */
		return flist_.size();
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
		std::sort(flist_.begin(),flist_.end());
		return n_files()?(flist_[current_n()]):nofile_;
	}

	fim::string Browser::_random_shuffle()
	{
		/*
		 *	sorts the image filenames list
		 *	TODO: it would be cool to support a user supplied seed value
		 */
		std::random_shuffle(flist_.begin(),flist_.end());
		return n_files()?(flist_[current_n()]):nofile_;
	}

	fim::string Browser::_reverse()
	{
		/*
		 *	sorts the image filenames list
		 */
		std::reverse(flist_.begin(),flist_.end());
		return n_files()?(flist_[current_n()]):nofile_;
	}

	fim::string Browser::fcmd_regexp_goto_next(const args_t &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		args_t arg;
		arg.push_back(last_regexp);
		return fcmd_regexp_goto(arg);
	}

	fim::string Browser::fcmd_regexp_goto(const args_t &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		size_t i,j,c=current_n(),s=flist_.size();
		if( args.size() < 1 || s < 1 )goto nop;
		for(j=0;j<s;++j)
		{
			last_regexp=args[0];
			i=(j+c+1)%s;
			if(commandConsole_.regexp_match(flist_[i].c_str(),args[0].c_str()))
			{	
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				autocmd_exec(FIM_ACM_PREGOTO,c);
#endif
				goto_image(i+1);
#ifdef FIM_AUTOCMDS
				autocmd_exec(FIM_ACM_POSTGOTO,c);
				if(!commandConsole_.inConsole())
					commandConsole_.set_status_bar((current()+fim::string(" matches \"")+args[0]+fim::string("\"")).c_str(),NULL);
				goto nop;
#endif
			}
		}
		cout << "sorry, no filename matches \""<<args[0]<<"\"\n";
		if(!commandConsole_.inConsole())
			commandConsole_.set_status_bar((fim::string("sorry, no filename matches \"")+
						args[0]+
						fim::string("\"")).c_str(),NULL);
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::goto_image(int n)
	{
		/*
		 *	FIX ME
		 */
		int N=flist_.size();
		if(!N)return FIM_CNS_EMPTY_RESULT;

		if( N==1 && c_image() && c_image()->is_multipage())
		{
			image()->goto_page(n);
			return N;
		}

		cp_=n;
		if(cp_<0)cp_=(cp_%N)+N+1;//+1 added lately
		if(cp_>N) cp_=1+(n%N);
		if(!cp_)++cp_;
		setGlobalVariable(FIM_VID_FILEINDEX,current_image());
		setGlobalVariable(FIM_VID_FILENAME, current().c_str());
		fim::string result = n_files()?(flist_[current_n()]):nofile_;
		return result;
	}

	fim::string Browser::next(int n=1)
	{
		/*
		 * jumps n images forward in the image list
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_PRENEXT,c);
#endif
		fim::string result=do_next(n);
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_POSTNEXT,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::prev(int n)
	{
		/*
		 * jumps n images backwards in the image list
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_PREPREV,c);
#endif
		fim::string result=do_next(-n);
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_POSTPREV,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_prev(const args_t &args)
	{
		return prev(args.size()>0?((int)args[0]):1);
	}

	fim::string Browser::get_next_filename(int n)const
	{
		/*
		 * returns to the next image in the list, the mechanism
		 * p.s.: n<>0
		 */
		int ccp=cp_;
		int N=flist_.size();
		if(!N)return FIM_CNS_EMPTY_RESULT;
		ccp+=n;
		ccp%=N;
		ccp+=N;
		ccp%=N;
		if(!ccp)ccp=N;
		return flist_[current_n(ccp)];
	}

	fim::string Browser::do_next(int n)
	{
		/*
		 * jumps to the next n'th image in the list.
		 * p.s.: n<>0
		 */
		int N=flist_.size();
		if(!N)
			goto nop;
		else
		{
		cp_+=n;
		cp_%=N;
		cp_+=N;
		cp_%=N;
		if(!cp_)cp_=N;
		setGlobalVariable(FIM_VID_FILEINDEX,current_image());
		setGlobalVariable(FIM_VID_FILENAME, current().c_str());
		//fim::string result = n_files()?(flist_[current_n()]):nofile_;
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}
	
	fim::string Browser::fcmd_goto_image(const args_t &args)
	{
		/*
		 *	FIX ME
		 *	there should be a way to have an interactive goto
		 */
		const fim_char_t*errmsg=FIM_CNS_EMPTY_STRING;
		int cn=0,g=0;
		if(n_files()==0){errmsg="no image to go to!";goto err;}
		cn=g=current_n()+1;
		if(args.size()<1){errmsg="please specify a file to view ( a number or ^ or $ ) \n";goto err;}
		else
		{
			const fim_char_t*s=args[0].c_str();
			fim_char_t c=FIM_SYM_CHAR_NUL;
			int sl=0;
			bool pcnt=false;
			if(!s)goto ret;
			sl=strlen(s);
			if(sl<1)goto ret;
			c=*s;
			pcnt=(s[sl-1]=='%');
			if(isdigit(c)  || c=='-' || c=='+')g=atoi(s);
			else if(c=='^' || c=='f')g=1;
			else if(c=='$' || c=='l')g=n();
			else cout << " please specify a number or ^ or $\n";
			if(pcnt)g=(g*n_files())/100;//FIXME: gross errors may occur here
			if(c=='+' || c=='-')g=cn+g; // FIXME: what if g g<1 ? pity :)
			//if(g!=-1)
			{	
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				autocmd_exec(FIM_ACM_PREGOTO,c);
#endif
				goto_image(g);
#ifdef FIM_AUTOCMDS
				autocmd_exec(FIM_ACM_POSTGOTO,c);
#endif
			}
		}
ret:
		return FIM_CNS_EMPTY_RESULT;
err:
		return errmsg;
	}

	fim::string Browser::do_remove(const args_t &args)
	{
		/*
		 *	ONLY if the image filename exists and matches EXACTLY,
		 *
		 *	FIXME : dangerous!
		 */
		if(flist_.size()<1)return "the files list is empty\n";
		args_t rlist=args;	//the remove list
		if(rlist.size()>0)
		{
			/*
			 * the list is unsorted. it may contain duplicates
			 * if this software will have success, we will have indices here :)
			 * sort(rlist.begin(),rlist.end());...
			 */
			for(size_t r=0;r<rlist.size();++r)
			for(size_t i=0;i<flist_.size();++i)
			if(flist_[i]==rlist[r])
			{
//				std::cout << "removing" << flist_[i]<<"\n";
				flist_.erase(flist_.begin()+i);
			}
			cp_=cp_%(flist_.size()+1);// new
			setGlobalVariable(FIM_VID_FILELISTLEN,current_images());
			goto nop;
		}
		else
		{
			/*
			 * removes the current file from the list
			 */
/*			if(cp_-1==current_n())--cp_;
			flist_.erase(flist_.begin()+current_n());
			if(cp_==0 && n_files()) ++cp_;
			return FIM_CNS_EMPTY_RESULT;*/
			return pop_current();
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scrollforward(const args_t &args)
	{
		/*
		 * scrolls the image as it were a book :)
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PREPAN,c);
#endif
		if(c_image() && viewport())
		{
			if(viewport()->onRight() && viewport()->onBottom())
				next();
			else
			if(viewport()->onRight())
			{
				viewport()->pan("down",FIM_CNS_SCROLL_DEFAULT);
				while(!(viewport()->onLeft()))viewport()->pan("left",FIM_CNS_SCROLL_DEFAULT);
			}
			else viewport()->pan("right",FIM_CNS_SCROLL_DEFAULT);
		}
		else next(1);
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTPAN,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scrolldown(const args_t &args)
	{
		/*
		 * scrolls the image down 
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PREPAN,c);
#endif
		if(c_image() && viewport())
		{
			if(viewport()->onBottom()) next();
			else
				viewport()->pan("down",FIM_CNS_SCROLL_DEFAULT);
		}
		else next(1);
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTPAN,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_info(const args_t &args)
	{
		/*
		 *	short information in status-line format
		 */
#if 0
		string fl;
		for(size_t r=0;r<flist_.size();++r)
		{
			fl+=flist_[r];
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
		return fcmd_info(args_t(0));
	}

	fim::string Browser::fcmd_scale_factor_grow(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		fim_scale_t sfm=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_MULTIPLIER);if(sfm<=FIM_CNS_SCALEFACTOR_ONE)sfm=FIM_CNS_SCALEFACTOR_MULTIPLIER;
		setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)*sfm);
		setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)*sfm);
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scale_factor_shrink(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		fim_scale_t sfm=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_MULTIPLIER);if(sfm<=FIM_CNS_SCALEFACTOR_ONE)sfm=FIM_CNS_SCALEFACTOR_MULTIPLIER;
		setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)/sfm);
		setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)/sfm);
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scale_factor_increase(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		fim_scale_t sfd=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_DELTA);if(sfd<=FIM_CNS_SCALEFACTOR_ZERO)sfd=FIM_CNS_SCALEFACTOR_DELTA ;
		setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)+sfd);
		setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)+sfd);
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_scale_factor_decrease(const args_t &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		fim_scale_t sfd=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_DELTA);if(sfd<=FIM_CNS_SCALEFACTOR_ZERO)sfd=FIM_CNS_SCALEFACTOR_DELTA ;
		setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)-sfd);
		setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)-sfd);
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_rotate(const args_t &args)
	{
		/*
		 * rotates the displayed image a specified amount of degrees
		 */ 
		fim_angle_t angle;
		if(args.size()==0)angle=FIM_CNS_ANGLE_ONE;
		else angle=fim_atof(args[0].c_str());
		if(angle==FIM_CNS_ANGLE_ZERO)return FIM_CNS_EMPTY_RESULT;

		if(c_image())
		{
			//angle = (double)getGlobalFloatVariable(FIM_VID_ANGLE);
			fim::string c=current();
#ifdef FIM_AUTOCMDS
//			autocmd_exec(FIM_ACM_PREROTATE,c);//FIXME
			autocmd_exec(FIM_ACM_PRESCALE,c); //FIXME
#endif
			if(c_image())
			{
				if(angle)
					{if(image())image()->rotate(angle);}
				else	
					{if(image())image()->rotate();}
			}
#ifdef FIM_AUTOCMDS
//			autocmd_exec(FIM_ACM_POSTROTATE,c);//FIXME
			autocmd_exec(FIM_ACM_POSTSCALE,c); //FIXME
#endif
		}
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_magnify(const args_t &args)
	{
		/*
		 * magnifies the displayed image
		 */ 
		if(c_image())
		{
			fim_scale_t factor;
			factor = firstforzero(args);
			if(!factor) factor = (fim_scale_t)getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR);
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PRESCALE,c);
#endif
			if(c_image())
			{
				if(factor)
					{
						if(image())image()->magnify(factor);
						if(viewport())viewport()->scale_position_magnify(factor);
					}
				else	
					{
						if(image())image()->magnify();
						if(viewport())viewport()->scale_position_magnify();
					}
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTSCALE,c);
#endif
		}
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_reduce(const args_t &args)
	{
		/*
		 * reduces the displayed image size
		 */ 
		if(c_image())
		{
			fim_scale_t factor;
			factor = firstforzero(args);
			if(!factor) factor = (fim_scale_t)getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR);
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PRESCALE,c);
#endif
			if(c_image())
			{
				if(factor)
					{
						if(image())image()->reduce(factor);
						if(viewport())viewport()->scale_position_reduce(factor);
					}
				else	
					{
						if(image())image()->reduce();
						if(viewport())viewport()->scale_position_reduce();
					}
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTSCALE,c);
#endif
		}
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_align(const args_t &args)
	{
		/*
		 * aligns to top/bottom the displayed image
		 * TODO: incomplete
		 */ 
		if(args.size()<1)goto err;
		if(!args[0].c_str() || !args[0].re_match("^(bottom|top)"))goto err;
		if(c_image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_PREPAN,c);
#endif
			if(c_image() && viewport())
			{
				if(args[0].re_match("top"))
					viewport()->top_align();
				if(args[0].re_match("bottom"))
					viewport()->bottom_align();
			}
#ifdef FIM_AUTOCMDS
			autocmd_exec(FIM_ACM_POSTPAN,c);
#endif
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
err:
		return FIM_CMD_HELP_ALIGN;
	}

	const Image *Browser::c_image()const
	{
		/*
		 *	a const pointer to the currently loaded image
		 */
		if( commandConsole_.current_viewport() )
			return commandConsole_.current_viewport()->c_getImage();
		else
			return NULL;
	}

	Image *Browser::image()const
	{
		/*
		 *	the image loaded in the current viewport is returned
		 */
		if( commandConsole_.current_viewport() )
			return commandConsole_.current_viewport()->getImage();
		else
			return NULL;
	}

	Viewport* Browser::viewport()const
	{
		/* 
		 * A valid pointer will be returned 
		 * whenever the image is loaded !
		 *
		 * NULL is returned in case no viewport is loaded.
		 * */
		return (commandConsole_.current_viewport());
	}

	fim::string Browser::current()const
	{
		/*
		 * dilemma : should the current() filename and next() operations
		 * be relative to viewport's own current's ?
		 * */
		if(empty_file_list())return nofile_; // FIXME: patch!
	       	return cp_?flist_[current_n()]:nofile_;
	}

	int Browser::empty_file_list()const
	{
		/*
		 *	is the filename list empty ?
		 */
		return flist_.size()==0;
	}

	fim::string Browser::display()
	{
		/*
		 *	display the current image
		 */
		return fcmd_display(args_t());
	}

	fim::string Browser::pop_current(const args_t &args)
	{
		/*
		 *	pops the last image filename off the image list
		 */
		return pop_current();
	}

	fim::string Browser::do_push(const args_t &args)
	{
		/*
		 *	pushes a new image filename on the back of the image list
		 */
		for(size_t i=0;i<args.size();++i)
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
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_next_page(const args_t &args)
	{
		/*
		 * jumps one page forward in the current multipage image
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_PRENEXT,c);
#endif
		if(c_image())
		{
			if(image())image()->next_page();
		}
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_POSTNEXT,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_prev_page(const args_t &args)
	{
		/*
		 * jumps one page backward in the current multipage image
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_PRENEXT,c);
#endif
		if(c_image())
		{
			if(image())image()->prev_page();
		}
#ifdef FIM_AUTOCMDS
		autocmd_exec(FIM_ACM_POSTNEXT,c);
#endif
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::fcmd_next_picture(const args_t &args)
	{
		/*
		 * FIX ME
		 * */
		if(c_image() && c_image()->have_nextpage())
			return fcmd_next_page(args);
		else
			return next(args.size()>0?((int)args[0]):1);
	}

	fim::string Browser::fcmd_prev_picture(const args_t &args)
	{
		/*
		 * FIX ME
		 * */
		if(c_image() && c_image()->have_prevpage())
			return fcmd_prev_page(args);
		else
			return prev(args.size()>0?((int)args[0]):1);
	}

	fim::string Browser::fcmd_next(const args_t &args)
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
		return cp_;
	}
}

