/* $Id$ */
/*
 Browser.cpp : Fim image browser

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

#define firstorzero(x) (x.size()?((int)(x[0])):0)

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


	fim::string Browser::redisplay(const std::vector<fim::string> &args)
	{
		redisplay();
		return "";
	}

	void Browser::redisplay()
	{
		return; // FIXMe
		/*
		 * Given a current() file, we display it again like 
		 * the first time it should be displayed.
		 * So, this behaviour is different from reloading..
		 */
		/*
		 *	THIS METHOD TRIGGERS A BUG.
		 * 
		 */
		fim::string c=current();
		if(image())
		{
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreRedisplay",c);
#endif
			if(image())
			{
				viewport().redisplay();	//THE BUG IS NOT HERE
				this->display_status(info().c_str(), NULL);//THE BUG IS NOT HERE
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostRedisplay",c);
#endif
		}
		else
		{
			cout << "no image object in memory, sorry\n";
#ifdef FIM_REMOVE_FAILED
			if(current()!=""){pop_current();	//removes the current file from the list.
#ifdef FIM_AUTOSKIP_FAILED
			next(0);reload();}
#endif
#endif
		}
	}

	Browser::Browser():nofile("")
	{	
		/*
		 * we initialize to no file
		 */
#ifndef FIM_WINDOWS
		loaded_image=NULL;
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
		cc.setVariable("filelistlen",current_images());
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
		flist.pop_back();
		cc.setVariable("filelistlen",current_images());
		return s;
	}

#ifdef FIM_ALWAYS_UNDEFINED
	fim::string Browser::pan_up(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image up
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_up(firstorzero(args));
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		else prev();
		return "";
	}
	
	fim::string Browser::pan_down(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image down
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_down(firstorzero(args));
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		else next(1);
		return "";
	}

	fim::string Browser::pan_ne(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image ne
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_up();
			if(image())viewport().pan_right();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		//else prev();
		return "";
	}

	fim::string Browser::pan_nw(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image nw
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_up();
			if(image())viewport().pan_left();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		else prev();
		return "";
	}

	fim::string Browser::pan_sw(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image sw
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_down();
			if(image())viewport().pan_left();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		//else next(0);
		return "";
	}

	fim::string Browser::pan_se(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image se
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_down();
			if(image())viewport().pan_right();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		else next(0);
		return "";
	}

	fim::string Browser::scale_multiply(const std::vector<fim::string> &args)
	{
		/*
		 * scales the image by a user specified factor
		 */
		double multiscale;
		if(args.size()==0)return "";
		multiscale=atof(args[0].c_str());
		if(multiscale==0.0)return "";
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())image()->scale_multiply(multiscale);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::scale_increment(const std::vector<fim::string> &args)
	{
		/*
		 * increments the scale additively
		 */
		double deltascale;
		if(args.size()==0)return "";
		deltascale=atof(args[0].c_str());
		if(deltascale==0.0)return "";
		if(strstr(args[0].c_str(),"%"))deltascale*=.01;
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())image()->scale_increment(deltascale);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::scale(const std::vector<fim::string> &args)
	{
		/*
		 * scales the image to a certain factor
		 */
		double newscale;
		if(args.size()==0)return "";
		newscale=atof(args[0].c_str());
		if(newscale==0.0)return "";
		if(strstr(args[0].c_str(),"%"))newscale*=.01;
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())image()->setscale(newscale);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::auto_height_scale(const std::vector<fim::string> &args)
	{
		/*
		 * scale this image to fit in the screen in the vertical dimension
		 */
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())viewport().auto_height_scale();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::auto_width_scale(const std::vector<fim::string> &args)
	{
		/*
		 * scale this image to fit in the screen in the horizontal dimension
		 */
		if(image())
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())viewport().auto_width_scale();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::auto_scale(const std::vector<fim::string> &args)
	{
		/*
		 * auto scale the image accordingly to the *default* settings !
		 */
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())viewport().auto_scale();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::pan_right(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image right
		 */
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_right(firstorzero(args));
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		else next(1);
		return "";
	}

	fim::string Browser::pan_left(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image left
		 */
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())viewport().pan_left(firstorzero(args));
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		else prev();
		return "";
	}
#endif

	fim::string Browser::display_status(const char *l,const char *r)
	{
		//FIX ME
		if(cc.getIntVariable("_display_status"))
			set_status_bar((const char*)l, image()?image()->getInfo():"*");
		return "";
	}

	fim::string Browser::display(const std::vector<fim::string> &args)
	{
		/*
		 * display the current image, if already loaded, on screen
		 */
		fim::string c=current();
		if(image())
		{
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreDisplay",c);
#endif
			/*
			 * the following is a trick to override redisplaying..
			 */
			if(image() && cc.getIntVariable("_override_display")==0)
			//	if(image())
			{
				//fb_clear_screen();
				viewport().display();
				this->display_status(info().c_str(), NULL);
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostDisplay",c);
#endif
		}
		else{ cout << "no image to display, sorry!";
		set_status_bar("no image loaded.", "*");}
		return "";
	}

	fim::string Browser::no_image(const std::vector<fim::string> &args)
	{
		free_current_image();
		return "";
	}

	void Browser::free_current_image()
	{
#ifdef FIM_WINDOWS
		if(image()) cache.free(image());
		if(image()) viewport().free(); //NOTE : cache is wasted in this way.
		// FIXME : here should land support for cache reusing !
		viewport().setImage( NULL );
#else
		if(image())
		{
			cache.free(image());
			loaded_image=NULL;
		}
		loaded_image = cache.getImage(current().c_str());
#endif
	}

	fim::string Browser::load_error_handle(fim::string c)
	{
		if(image() && ! (viewport().check_valid()))
		{
			cout << "BAD SIGN..\n";
			free_current_image();
#ifdef FIM_REMOVE_FAILED
			if(current()!=""){pop_current();	//removes the current file from the list.
#ifdef FIM_AUTOSKIP_FAILED
#ifdef FIM_ALWAYS_UNDEFINED
			next(0);reload();
#endif
#endif
			}
#endif
			fim::string rs("error loading the file ");
			rs+=c;
			rs+="\n";
			return rs;
		}
		return "";
	}

	fim::string Browser::reload()
	{
		if(n_files())
		return reload(std::vector<fim::string>());
		return "";
	}

	fim::string Browser::reload(const std::vector<fim::string> &args)
	{
		/*
		 * deletes the structures associated to the present image
		 * and then
		 * tries to load a new one from the current filename
		 */
		fim::string c=current();
		if(empty_file_list())return "sorry, no image to reload\n";
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PreReload",c);
#endif
		free_current_image();
#ifdef FIM_WINDOWS
		viewport().setImage( cache.getImage(current().c_str()) );
#else
		loaded_image = cache.getImage(current().c_str());
#endif

		if(cc.getIntVariable("_prefetch")) cache.prefetch(get_next_filename(1).c_str());/*this will become an autocommand*/
		cc.setVariable("fileindex",current_image());

		if(image() && ! (viewport().check_valid()))return load_error_handle(c);

#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PostReload",c);
#endif
		return "";
	}

	fim::string Browser::load(const std::vector<fim::string> &args)
	{
		/*
		 * loads the current file, if not already loaded
		 */
		fim::string c=current();
		if(image() && ( image()->getName() == current()) )
		{
			return "image already loaded\n";		//warning
		}
		free_current_image();
		if(empty_file_list())return "sorry, no image to load\n";	//warning
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PreLoad",c);
#endif
		set_status_bar("please wait while loading...", "*");

#ifdef FIM_WINDOWS
		viewport().setImage( cache.getImage(current().c_str()) );
#else
		loaded_image = cache.getImage(current().c_str());
#endif
		if(cc.getIntVariable("_prefetch")) cache.prefetch(get_next_filename(1).c_str());
		cc.setVariable("fileindex",current_image());

		if(image() && ! (viewport().check_valid()))return load_error_handle(c);
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PostLoad",c);
#endif
		return "";
	}

	fim::string Browser::pop(const std::vector<fim::string> &args)
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

	bool Browser::push(const fim::string nf)
	{	
		/*
		 * FIX ME:
		 * no repetition!????
		 * */
		
#ifdef FIM_CHECK_FILE_EXISTENCE
		/*
		 * skip adding the filename in the list if
		 * it is not existent or it is a directory...
		 */
		struct stat stat_s;
		/*	if the file doesn't exist, return */
		if(-1==stat(nf.c_str(),&stat_s))return "";
		/*	if it is a char device , return */
		//if(  S_ISCHR(stat_s.st_mode))return "";
		/*	if it is a block device , return */
		//if(  S_ISBLK(stat_s.st_mode))return "";
		/*	if it is a directory , return */
		//if(  S_ISDIR(stat_s.st_mode))return "";
		/*	..hmm.. paranoia is better :
		 *	we want a regular file .. */
		if(! S_ISREG(stat_s.st_mode))return "";
#endif

#ifdef FIM_CHECK_DUPLICATES
		if(present(nf))
		{
			//there could be an option to have duplicates...
			//std::cout << "no duplicates allowed..\n";
			return false;
		}
#endif
//		cout << "PUSHING : " << info()<<  "\n";
		flist.push_back(nf);
		cc.setVariable("filelistlen",current_images());
		if(cp==0)++cp;
		//cout<<nf<<" ";
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
		return fim::string(n_files());
	}

	fim::string Browser::get()const
	{
		return current();
	}

	fim::string Browser::_sort()
	{
		/*
		 *	FIX ME
		 */
		sort(flist.begin(),flist.end());
		return n_files()?(flist[current_n()]):nofile;
	}

	fim::string Browser::regexp_goto_next(const std::vector<fim::string> &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		std::vector<fim::string> arg;
		arg.push_back(last_regexp);
		return regexp_goto(arg);
	}

	fim::string Browser::prefetch(const std::vector<fim::string> &args)
	{
		/*
		 * fetches in the cache the next image..
		 *
		 * FIX ME : enrich this behaviour
		 */
		if( args.size() > 0 )return "";
		cache.prefetch(get_next_filename(1).c_str());
		return "";
	}

	fim::string Browser::regexp_goto(const std::vector<fim::string> &args)
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
			if(cc.regexp_match(flist[i].c_str(),args[0].c_str()))
			{	
				//cout << flist[i] << " matches!!!\n"; 
				fim::string c=current();
#ifdef FIM_AUTOCMDS
				cc.autocmd_exec("PreGoto",c);
#endif
				goto_image(i+1);
#ifdef FIM_AUTOCMDS
				cc.autocmd_exec("PostGoto",c);
				if(!cc.inConsole())
					set_status_bar((current()+fim::string(" matches \"")+args[0]+fim::string("\"")).c_str(),NULL);
				return "";
#endif
			}
		}
		cout << "sorry, no filename matches \""<<args[0]<<"\"\n";
		if(!cc.inConsole())
			set_status_bar((fim::string("sorry, no filename matches \"")+
						args[0]+
						fim::string("\"")).c_str(),NULL);
		
		return "";
	}

	fim::string Browser::goto_image(int n)
	{
		/*
		 *	FIX ME
		 */
		//if(n==current_n())return "";//Achtung!
		int N=flist.size();
		if(!N)return "";
		cp=n;
		if(cp<0)cp=(cp%N)+N+1;//+1 added lately
		if(cp>N) cp=1+(n%N);
		if(!cp)++cp;
		cc.setVariable("fileindex",current_image());
//		std::cout << "next " << n << "\n";
//		return n_files()?(flist[current_n()]):nofile;
		fim::string result = n_files()?(flist[current_n()]):nofile;
//		reload();
		return result;
	}

	fim::string Browser::next(int n=1)
	{
		/*
		 * jumps to the next image in the list
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PreNext",c);
#endif
		fim::string result=do_next(n);
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PostNext",c);
#endif
		return "";
	}

	fim::string Browser::get_next_filename(int n)
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
		 * jumps to the next image in the list, the mechanism
		 * p.s.: n<>0
		 */
		int N=flist.size();
		if(!N)return "";
		cp+=n;
		cp%=N;
		cp+=N;
		cp%=N;
		if(!cp)cp=N;
		cc.setVariable("fileindex",current_image());
		fim::string result = n_files()?(flist[current_n()]):nofile;
		return "";
	}
	
	fim::string Browser::goto_image(const std::vector<fim::string> &args)
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
				cc.autocmd_exec("PreGoto",c);
#endif
				goto_image(g);
#ifdef FIM_AUTOCMDS
				cc.autocmd_exec("PostGoto",c);
#endif
			}
		}
		return "";
	}

	fim::string Browser::remove(const std::vector<fim::string> &args)
	{
		/*
		 *	ONLY if the image filename exists and matches EXACTLY,
		 */
		if(flist.size()<1)return "the files list is empty\n";
		std::vector<fim::string> rlist=args;	//the remove list
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

#ifdef FIM_ALWAYS_UNDEFINED
	fim::string Browser::scrollforward(const std::vector<fim::string> &args)
	{
		/*
		 * scrolls the image as it were a book :)
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
		if(image())
		{
			if(viewport().onRight() && viewport().onBottom())
				next();
			else
			if(viewport().onRight())
			{
				viewport().pan_down();
				while(!(viewport().onLeft()))viewport().pan_left();
			}
			else viewport().pan_right();
		}
		else next(1);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		return "";
	}

	fim::string Browser::scrolldown(const std::vector<fim::string> &args)
	{
		/*
		 * scrolls the image down 
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
		if(image())
		{
			if(viewport().onBottom()) next();
			else pan_down(std::vector<fim::string>());
		}
		else next(1);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		return "";
	}
#endif

	fim::string Browser::info(const std::vector<fim::string> &args)
	{
		/*
		 *	short information in status-line format
		 */
		//std::cout << 
//		fim_stream out;
//		cout << "[" << current().c_str() << "] ("<<cp<<"/"<<n_files() << ")\n";
//	return fim::string("[")+current()+fim::string("] (")+cp+fim::string("/")+n_files()+fim::string(")\n");

	/*return fim::string(" [")+current()+fim::string("] (")
		+fim::string(cp)+fim::string("/")
		+fim::string(n_files())+fim::string(")\n");*/
#if 0
		string fl;
		for(unsigned int r=0;r<flist.size();++r)
		{
			fl+=flist[r];
			fl+="\n";
		}
		return fl;
#else
		return current();
#endif
	//	return list();
	}

	fim::string Browser::info()
	{
		return info(std::vector<fim::string>(0));
	}

#ifdef FIM_ALWAYS_UNDEFINED
	fim::string Browser::scale_factor_grow(const std::vector<fim::string> &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfm=cc.getFloatVariable("scale_factor_multiplier");if(sfm<=1.0f)sfm=1.1f;
		cc.setVariable("reduce_factor",cc.getFloatVariable("reduce_factor")*sfm);
		cc.setVariable("magnify_factor",cc.getFloatVariable("magnify_factor")*sfm);
		return "";
	}

	fim::string Browser::scale_factor_shrink(const std::vector<fim::string> &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfm=cc.getFloatVariable("scale_factor_multiplier");if(sfm<=1.0f)sfm=1.1f;
		cc.setVariable("reduce_factor",cc.getFloatVariable("reduce_factor")/sfm);
		cc.setVariable("magnify_factor",cc.getFloatVariable("magnify_factor")/sfm);
		return "";
	}

	fim::string Browser::scale_factor_increase(const std::vector<fim::string> &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfd=cc.getFloatVariable("scale_factor_delta");if(sfd<=0.0f)sfd=0.1f;
		cc.setVariable("reduce_factor",cc.getFloatVariable("reduce_factor")+sfd);
		cc.setVariable("magnify_factor",cc.getFloatVariable("magnify_factor")+sfd);
		return "";
	}

	fim::string Browser::scale_factor_decrease(const std::vector<fim::string> &args)
	{
		/*
		 *	ALIAS AND DELETE ME!
		 */
		float sfd=cc.getFloatVariable("scale_factor_delta");if(sfd<=0.0f)sfd=0.1f;
//		cc.setVariable("scale_factor",cc.getFloatVariable("scale_factor")/1.1f);
		cc.setVariable("reduce_factor",cc.getFloatVariable("reduce_factor")-sfd);
		cc.setVariable("magnify_factor",cc.getFloatVariable("magnify_factor")-sfd);
		return "";
	}


	fim::string Browser::magnify(const std::vector<fim::string> &args)
	{
		/*
		 * magnify the displayed image
		 * FIX ME 
		 */ 
		if(image())
		{
			float factor = (float)cc.getFloatVariable("magnify_factor");
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())
			{
				if(factor) image()->magnify(factor);
				else	image()->magnify();
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::reduce(const std::vector<fim::string> &args)
	{
		/*
		 * reduce the displayed image
		 * FIX ME 
		 */ 
		if(image())
		{
			float factor = (float)cc.getFloatVariable("reduce_factor");
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image())
			{
				if(factor) image()->reduce(factor);
				else	image()->reduce();
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
#endif

	fim::string Browser::prev(int n)
	{
		return "";
		/*
		 * make the previous image in the list current
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PrePrev",c);
#endif
		fim::string result=do_next(-n);
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PostPrev",c);
#endif
		return "";
	}

	fim::string Browser::top_align(const std::vector<fim::string> &args)
	{
		/*
		 * align to top the displayed image
		 */ 
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())
			{
				viewport().top_align();
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	fim::string Browser::bottom_align(const std::vector<fim::string> &args)
	{
		/*
		 * align to the bottom the displayed image
		 */ 
		if(image())
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image())
			{
				viewport().bottom_align();
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	Image *Browser::image()const
	{
//		return NULL; // this is safe.. but invalidating
	#ifdef FIM_WINDOWS
//		return cc.current_window().getImage();
		return cc.current_viewport().getImage();
	#else
	       	return loaded_image;
	#endif
	}

	Viewport& Browser::viewport()const
	{
		/* 
		 *		DANGER 
		 * A valid pointer should be returned 
		 * whenever the image is loaded !
		 * */
#ifdef FIM_WINDOWS
		return (cc.current_viewport());
#else
		return (Viewport&)only_viewport;
#endif
	}

	fim::string Browser::current()const
	{
		if(empty_file_list())return nofile; // FIXME: patch!
	       	return cp?flist[current_n()]:nofile;
	}

	int Browser::empty_file_list()const
	{
		return flist.size()==0;
	}
}

