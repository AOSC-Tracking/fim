/* $LastChangedDate: 2011-08-11 23:28:41 +0200 (Thu, 11 Aug 2011) $ */
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

#define firstorval(x,v)  ((x.size()>0)?((int)(x[0])):(v))
#define firstorzero(x)    firstorval((x),(0))
#define firstorone(x)     firstorval((x),(1))
#define firstforzero(x)   (x.size()>0?((float)(x[0])):0.0)
#define FIM_HORRIBLE_CACHE_INVALIDATING_HACK 1

#include <dirent.h>
#include <sys/types.h>	/* POSIX Standard: 2.6 Primitive System Data Types (e.g.: ssize_t) */
#include "fim.h"
#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

namespace fim
{
	//int Browser::current_n()const{ return current_n(cf_); }
	int Browser::current_n()const{ return cf_; }
	//int Browser::current_n(int ccp)const{ return ccp?ccp-1:ccp; }

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
			if(args[0]=="clear")
				return _clear_list();
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
				return this->n_files();
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
#ifdef FIM_READ_DIRS
			if(args[0]=="pushdir")
			{
				if(args.size()>=2)
					push_dir(args[1]);
				else
					push_dir(".");
				return  "";
			}
#endif
			if(args[0]=="filesnum")
			{
				return n_files();
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
		cf_=0;	//and to file index 0 (no file)
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
		assert(cf_);
		flist_.erase(flist_.begin()+current_n());
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
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
		assert(cf_);
		if(filename==FIM_CNS_EMPTY_STRING)
		{
			if(current_n()==(int)flist_.size())cf_--;
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
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
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

	fim::string Browser::fcmd_scale(const args_t &args)
	{
		/*
		 * scales the image to a certain scale factor
		 * FIXME: no user error checking -- poor error reporting for the user
		 * TODO: wxh / w:h syntax needed
		 */
		fim_scale_t newscale=FIM_CNS_SCALEFACTOR_ZERO;
		fim_char_t fc=FIM_SYM_CHAR_NUL;
		const fim_char_t*ss=NULL;
		int sl=0;
		bool pcsc=false;
		if(args.size()<1 || !(ss=args[0].c_str()))goto nop;
		fc=tolower(*ss);
		sl=strlen(ss);
		if(isalpha(fc))
		{
			if( !(fc=='w'||fc=='h'||fc=='a'||fc=='b'))
				goto nop;
		}
		else
		{
			if(sl==1 && (fc=='+'||fc=='-'))
			{
				if(fc=='+')
					newscale=(fim_scale_t)getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR);
				if(fc=='-')
					newscale=(fim_scale_t)getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR);
				goto comeon;
			}
			if(sl>=2 && (fc=='+'||fc=='-'))
			{
				fim_char_t sc=ss[1];
				if(fc=='+' && sc=='+')
				{
					fim_scale_t sfd=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_DELTA);
					if(sfd<=FIM_CNS_SCALEFACTOR_ZERO)sfd=FIM_CNS_SCALEFACTOR_DELTA ;
					setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)+sfd);
					setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)+sfd);
					goto nop;
				}
				if(fc=='+' && sc=='-')
				{
					fim_scale_t sfd=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_DELTA);
					if(sfd<=FIM_CNS_SCALEFACTOR_ZERO)sfd=FIM_CNS_SCALEFACTOR_DELTA ;
					setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)-sfd);
					setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)-sfd);
					goto nop;
				}
				if(fc=='+' && sc=='*')
				{
					fim_scale_t sfm=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_MULTIPLIER);
					if(sfm<=FIM_CNS_SCALEFACTOR_ONE)sfm=FIM_CNS_SCALEFACTOR_MULTIPLIER;
					setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)*sfm);
					setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)*sfm);
					goto nop;
				}
				if(fc=='+' && sc=='/')
				{
					fim_scale_t sfm=getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_MULTIPLIER);
					if(sfm<=FIM_CNS_SCALEFACTOR_ONE)sfm=FIM_CNS_SCALEFACTOR_MULTIPLIER;
					setGlobalVariable(FIM_VID_REDUCE_FACTOR,getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR)/sfm);
					setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR)/sfm);
					goto nop;
				}
				if(fc=='+' || fc=='-')
				{
					newscale=fim_atof(ss+1);
					pcsc=(strstr(ss,"%")!=NULL);
					if(pcsc)newscale*=.01;
					if(!newscale)goto nop;
#if 1
					if(fc=='+')newscale=1.0+newscale;
					if(fc=='-')newscale=1.0-newscale;
					fc=FIM_SYM_CHAR_NUL;
#endif
					goto comeon;
				}
				goto nop;
			}
			if(sl)
			{
				newscale=fim_atof(ss);
				pcsc=(strstr(ss,"%")!=NULL);
				if(pcsc)newscale*=.01;
				if(newscale==FIM_CNS_SCALEFACTOR_ZERO) goto nop;
				pcsc=false;
				goto comeon;
			}
			goto nop;
		}
comeon:
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
				case('b'):
				if(viewport())viewport()->auto_scale_if_bigger();
				break;
				case('-'):
				{
				if(newscale)
					{
						if(image())image()->reduce(newscale);
						if(viewport())viewport()->scale_position_reduce(newscale);
					}
				else	
					{
						if(image())image()->reduce();
						if(viewport())viewport()->scale_position_reduce();
					}
				}
				break;
				case('+'):
				{
				if(newscale)
					{
						if(image())image()->magnify(newscale);
						if(viewport())viewport()->scale_position_magnify(newscale);
					}
				else	
					{
						if(image())image()->magnify();
						if(viewport())viewport()->scale_position_magnify();
					}

				}
				break;
				default:
				if(pcsc)
					image()->scale_multiply(newscale);
				else
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
		if(getGlobalIntVariable(FIM_VID_DISPLAY_STATUS)==1)
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
			if(args.size()>0 && args[0] == "reinit")
			{
				string arg=args.size()>1?args[1]:"";
				commandConsole_.display_reinit(arg.c_str());
			}
			if(image() && (getGlobalIntVariable(FIM_VID_OVERRIDE_DISPLAY)!=1))
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
ddone:
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
#if FIM_HORRIBLE_CACHE_INVALIDATING_HACK
		if(args.size()>0)
		{
			int mci=getGlobalIntVariable(FIM_VID_MAX_CACHED_IMAGES);
			setGlobalVariable(FIM_VID_MAX_CACHED_IMAGES,0);
			free_current_image();
			setGlobalVariable(FIM_VID_MAX_CACHED_IMAGES,mci);
		}
#else
		free_current_image();
#endif
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

	fim_int Browser::find_file_index(const fim::string nf)
	{
		/* 
		 * returns whether the file nf is in the files list
		 */
		for(fim_int i=0;i<flist_.size();++i)
			if(flist_[i]==nf)return i;
		return -1;
	}

	bool Browser::present(const fim::string nf)
	{
		/* 
		 * returns whether the file nf is in the files list
		 */
		fim_int i=find_file_index(nf);
		if(i>=0)
			return true;
		else
			return false;
	}

#ifdef FIM_READ_DIRS
	bool Browser::push_dir(fim::string nf)
	{
		// TODO: may introduce some more variable to control recursive push 	
		int d_n;
		DIR *dir=NULL;
		struct dirent *de=NULL;
		fim::string f;

		/*	we want a dir .. */
#ifdef HAVE_LIBGEN_H
		if(!is_dir(nf.c_str()))
			nf=fim_dirname(nf);
#else
		if( !is_dir( nf ))return false;
#endif

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
			 * Warning : this is dangerous, as following circular links may cause memory exhaustion.
			 * */
			if(is_dir( f+fim::string(de->d_name)))
#ifdef FIM_RECURSIVE_DIRS
				push_dir(f+fim::string(de->d_name));
#else
				continue;
#endif
			else 
			{
				fim::string re=getGlobalStringVariable(FIM_VID_PUSHDIR_RE);
				fim::string fn=f+fim::string(de->d_name);
				if(re==FIM_CNS_EMPTY_STRING)
					re=FIM_CNS_PUSHDIR_RE;
				if(fn.re_match(re.c_str()))
					push(f+fim::string(de->d_name));
			}
		}
		return (closedir(dir)==0);
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
		if(  S_ISDIR(stat_s.st_mode))
			if(getGlobalIntVariable(FIM_VID_PUSH_PUSHES_DIRS)==1)
				return push_dir(nf);
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

		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
		return false;
	}
	
	int Browser::n_files()const
	{
		/*
		 * the number of files in the filenames list
		 */
		return flist_.size();
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

	fim::string Browser::_clear_list()
	{
		/*
		 */
		flist_.erase(flist_.begin(),flist_.end());
		return 0;
	}

	fim::string Browser::_reverse()
	{
		/*
		 *	sorts the image filenames list
		 */
		std::reverse(flist_.begin(),flist_.end());
		return n_files()?(flist_[current_n()]):nofile_;
	}

	fim::string Browser::regexp_goto_next(const args_t &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		args_t arg;
		arg.push_back(last_regexp_);
		return regexp_goto(arg);
	}

	fim::string Browser::regexp_goto(const args_t &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		size_t i,j,c=current_n(),s=flist_.size();
		if( args.size() < 1 || s < 1 )goto nop;
		for(j=0;j<s;++j)
		{
			last_regexp_=args[0];
			i=(j+c+1)%s;
			if(commandConsole_.regexp_match(flist_[i].c_str(),args[0].c_str()))
			{	
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				autocmd_exec(FIM_ACM_PREGOTO,c);
#endif
				goto_image(i);
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

	fim::string Browser::goto_image(int n, bool isfg)
	{
		/*
		 *	FIX ME
		 */
		int N=flist_.size();
		if(!N)return FIM_CNS_EMPTY_RESULT;

		if(!isfg)
		if( N==1 && c_image() && c_image()->is_multipage())
		{
			if(0)std::cout<<"goto page "<<n<<"\n";
			image()->goto_page(n);
			return N;
		}
		cf_=n;
		cf_=FIM_MOD(cf_,N);
		setGlobalVariable(FIM_VID_PAGE ,(fim_int)0);
		setGlobalVariable(FIM_VID_FILEINDEX,current_image());
		setGlobalVariable(FIM_VID_FILENAME, current().c_str());
		fim::string result = n_files()?(flist_[current_n()]):nofile_;
		return result;
	}

	fim::string Browser::get_next_filename(int n)const
	{
		/*
		 * returns to the next image in the list, the mechanism
		 * p.s.: n<>0
		 */
		int ccp=cf_+n;
		int N=flist_.size();
		if(!N)return FIM_CNS_EMPTY_RESULT;
		ccp=FIM_MOD(ccp,N);
		return flist_[ccp];
	}

	fim::string Browser::next(int n)
	{
		fim::string gs="+";
		gs+=fim::string(n);
		return goto_image_internal(gs.c_str(),FIM_X_NULL);  
	}

	fim::string Browser::prev(int n)
	{
		fim::string gs="-";
		gs+=fim::string(n);
		return goto_image_internal(gs.c_str(),FIM_X_NULL);  
	}
	
	fim::string Browser::fcmd_goto_image(const args_t &args)
	{
		if(args.size()>0)
			return goto_image_internal(args[0].c_str(),FIM_X_NULL);
		else
			return goto_image_internal(NULL,FIM_X_NULL);
	}

	fim::string Browser::goto_image_internal(const fim_char_t *s, fim_xflags_t xflags)
	{
		/*
		 */
		const fim_char_t*errmsg=FIM_CNS_EMPTY_STRING;
		//const int cf=cf_,cp=c_page(),pc=n_pages(),fc=n_files();
		const int cf=cf_,cp=getGlobalIntVariable(FIM_VID_PAGE),pc=FIM_MAX(1,n_pages()),fc=n_files();
		int gv=0,nf=cf,mv=0,np=cp;
		if(n_files()==0 || !s){errmsg=FIM_CMD_HELP_GOTO;goto err;}
		if(!s){errmsg=FIM_CMD_HELP_GOTO;goto err;}
		else
		{
			fim_char_t c=FIM_SYM_CHAR_NUL;
			fim_char_t l=FIM_SYM_CHAR_NUL;
			int sl=0,li=0;
			bool pcnt=false;
			bool isre=false;
			bool ispg=false;
			bool isfg=false;
			bool isrj=false;
			if(!s)goto ret;
			sl=strlen(s);
			if(sl<1)goto ret;
			c=*s;
			//for(li=sl-2;li<sl;++li) { l=tolower(s[li]); pcnt=(l=='%'); ispg=(l=='p'); }
			l=tolower(s[li=sl-1]);
			pcnt=(l=='%'); 
			ispg=(l=='p');
			isfg=(l=='f');
			isre=((sl>=2) && ('/'==s[sl-1]) && (((sl>=3) && (c=='+') && s[1]=='/') ||( c=='/')));
			isrj=(c=='+' || c=='-');
			if(isdigit(c)  || c=='-' || c=='+')gv=atoi(s);
			else if(c=='^' || c=='f')gv=1;
			else if(c=='$' || c=='l')gv=-1;// temporarily
			else if(c=='?')
			{
				gv=find_file_index(string(s).substr(1,sl-1));
				//std::cout<<string(s).substr(1,sl-1)<<" "<<gv<<"\n";
				if(gv<0)
				{
					goto ret;
				}
				nf=gv;
				goto go_jump;
			}
			else if(isre){;}
			else
			{
				cout << " please specify a number or ^ or $\n";
			}
			if(li>0 && ( isfg || pcnt || ispg ))
			{
				l=tolower(s[li=sl-2]);
				if(l=='%')pcnt=true;
				if(l=='p')ispg=true;
				if(l=='f')isfg=true;
			}
			if(c=='$' || c=='l')gv=mv-1;
			if((isrj) && (!isfg) && (!ispg) && pc>1)
			{
				if((cp==0 && gv<0) || (cp==pc-1 && gv>0))
					isfg=true;
				else
					ispg=true;
			}
			if(ispg)
				mv=pc;
			else
				mv=fc;
			if(pcnt)
				gv=FIM_INT_PCNT(gv,mv);
			if(!mv)
			{
			       	goto ret; 
			}
			if(isfg && ispg)
			{
				goto err;
			}
			//if((!isre) && (!isrj))nf=gv;
			//if(isrj && gv<0 && cf==1){cf=0;}//TODO: this is a bugfix
			if((!isrj) && gv>0)
				gv=gv-1;// user input is interpreted as 1-based 
			gv=FIM_MOD(gv,mv);
			//cout << "at " << cf <<", gv="<<gv <<", mod="<<mv<<"\n";
			if(ispg)
			{
			if(isrj)
				{np=cp+gv;}// FIXME: what if gv gv<1 ? pity :)
			else
				np=gv;
			}
			else
			{
			if(isrj)
				{nf=cf+gv;}// FIXME: what if gv gv<1 ? pity :)
			else
				nf=gv;
			}
			gv=FIM_MOD(gv,mv);
			nf=FIM_MOD(nf,fc);
			np=FIM_MOD(np,pc);
	//			nf++;
			if(0)
			cout << "goto: "
				<<" s:" << s
				<<" cf:" << cf 
				<<" cp:" << cp 
				<< " nf:" << nf 
				<< " np:" << np 
				<< " gv:" << gv 
				<< " isrj:"<<isrj
				<< " ispg:"<<ispg
				<< " isfg:"<<isfg
				<< " pcnt:"<<pcnt
				<< " max[pf]:"<<mv
				<<"\n";
			if(isre)
			{
				args_t argsc;
				if(c=='+')
					return regexp_goto_next(argsc);/* no args needed */
				else
				{
					argsc.push_back(string(s).substr(1,sl-2));
					return regexp_goto(argsc);
				}
			}
go_jump:
			if((nf!=cf) || (np!=cp) )
			{	
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				if(!(xflags&FIM_X_NOAUTOCMD))autocmd_exec(FIM_ACM_PREGOTO,c);
#endif
				if(ispg)
					image()->goto_page(np);
				else
					goto_image(nf,isfg?true:false);
#ifdef FIM_AUTOCMDS
				if(!(xflags&FIM_X_NOAUTOCMD))autocmd_exec(FIM_ACM_POSTGOTO,c);
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
			int N=flist_.size();
			if(N<=0)cf_=0;
			else cf_=FIM_MIN(cf_,N-1);
			setGlobalVariable(FIM_VID_FILEINDEX,current_image());
			setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
			goto nop;
		}
		else
		{
			/*
			 * removes the current file from the list
			 */
/*			if(cf_-1==current_n())--cf_;
			flist_.erase(flist_.begin()+current_n());
			if(cf_==0 && n_files()) ++cf_;
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
	       	//return cf_?flist_[current_n()]:nofile_;
	       	return cf_>=0?flist_[cf_]:nofile_;
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

	int Browser::current_image()const
	{
		/* counting from 1 */
		return cf_+1;
	}

	int Browser::n_pages()const
	{
		if(c_image())return c_image()->n_pages(); else return 0;
	};

	int Browser::c_page()const
	{
		if(c_image())return c_image()->c_page(); else return 0;
	};
}

