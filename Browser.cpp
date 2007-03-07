/* $Id$ */
#include "fim.h"
namespace fim
{
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

	void Browser::redisplay()
	{
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
		if(image)
		{
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreRedisplay",c);
#endif
		if(image)
			{
				image->redisplay();	//THE BUG IS NOT HERE
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
		image=NULL;
		cp=0;	//and to file index 0 (no file)
	}

	const fim::string Browser::pop_current()
	{	
		/*
		 * pop the last image filename from the filenames list
		 *
		 * WARNING : SAME AS ERASE !
		 */
		fim::string s;
		if(flist.size()<=0)return nofile;
		assert(cp);
		flist.erase(flist.begin()+current_n());
		return s;
	}

	const fim::string Browser::pop()
	{	
		/*
		 * pop the last image filename from the filenames list
		 */
		fim::string s;
		if(flist.size()<=0)return nofile;
		assert(cp);
		if(current_n()==(int)flist.size())cp--;
		s=flist[flist.size()-1];
		flist.pop_back();
		return s;
	}

	fim::string Browser::pan_up(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image up
		 */
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image)image->pan_up();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::pan_down(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image down
		 */
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image)image->pan_down();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	fim::string Browser::scale_multiply(const std::vector<fim::string> &args)
	{
		/*
		 * scale the image 
		 */
		double multiscale;
		if(args.size()==0)return "";
		multiscale=atof(args[0].c_str());
		if(multiscale==0.0)return "";
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)image->scale_multiply(multiscale);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::scale_increment(const std::vector<fim::string> &args)
	{
		/*
		 * scale the image 
		 */
		double deltascale;
		if(args.size()==0)return "";
		deltascale=atof(args[0].c_str());
		if(deltascale==0.0)return "";
		if(strstr(args[0].c_str(),"%"))deltascale*=.01;
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)image->scale_increment(deltascale);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::scale(const std::vector<fim::string> &args)
	{
		/*
		 * scale the image 
		 */
		double newscale;
		if(args.size()==0)return "";
		newscale=atof(args[0].c_str());
		if(newscale==0.0)return "";
		if(strstr(args[0].c_str(),"%"))newscale*=.01;
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)image->setscale(newscale);
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::auto_height_scale(const std::vector<fim::string> &args)
	{
		/*
		 * scale the image to fit in the screen in the vertical dimension
		 */
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)image->auto_height_scale();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}
	
	fim::string Browser::auto_width_scale(const std::vector<fim::string> &args)
	{
		/*
		 * scale the image to fit in the screen in the horizontal dimension
		 */
		if(image)
		{
#ifdef FIM_AUTOCMDS
			fim::string c=current();
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)image->auto_width_scale();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::auto_scale(const std::vector<fim::string> &args)
	{
		/*
		 * auto scale the image
		 */
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)image->auto_scale();
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
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image)image->pan_right();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	fim::string Browser::pan_left(const std::vector<fim::string> &args)
	{
		/*
		 * pan the image left
		 */
		if(image)
		{
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
			if(image)image->pan_left();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostPan",c);
#endif
		}
		return "";
	}

	fim::string Browser::display_status(const char *l,const char *r)
	{
		//FIX ME
		if(cc.getIntVariable("_display_status"))
			status((const char*)l, image?image->getInfo():"*");
		return "";
	}

	fim::string Browser::display(const std::vector<fim::string> &args)
	{
		/*
		 * display the current image, if loaded, on screen
		 */
		fim::string c=current();
		if(image)
		{
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreDisplay",c);
#endif
			/*
			 * the following is a trick to override redisplaying..
			 */
			if(image && cc.getIntVariable("_override_display")==0)
			//	if(image)
			{
				//fb_clear_screen();
				image->display();
				this->display_status(info().c_str(), NULL);
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostDisplay",c);
#endif
		}
		else{ cout << "no image to display, sorry!";
		status("no image loaded.", "*");}
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
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PreReload",c);
#endif
		if(image) delete image;
		image = new Image(current().c_str());
		if(image && ! (image->valid()))
		{
			delete image;image=NULL;
			return fim::string("error reloading the file ")
				+c+fim::string("\n");
		}
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
		if(image) return "";
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PreLoad",c);
#endif
		status("please wait while loading...", "*");
		image = new Image(current().c_str());
		if(image && ! (image->valid()))
		{
			delete image;image=NULL;
#ifdef FIM_REMOVE_FAILED
			if(current()!=""){pop_current();	//removes the current file from the list.
#ifdef FIM_AUTOSKIP_FAILED
			next(0);reload();}
#endif
#endif
			return fim::string("error loading the file ")
				+c+fim::string("\n");
		}
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
		if(-1==stat(nf.c_str(),&stat_s))return "";
		if(S_ISDIR(stat_s.st_mode))return "";
#endif

#ifdef FIM_CKECK_DUPLICATES
		if(present(nf))
		{
			//there could be an option to have duplicates...
			//std::cout << "no duplicates allowed..\n";
			return false;
		}
#endif

		flist.push_back(nf);
		if(cp==0)++cp;
		//cout<<nf<<" ";
		return false;
	}
	
	fim::string Browser::reload()
	{
		return reload(std::vector<fim::string>());
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

	fim::string Browser::regexp_goto(const std::vector<fim::string> &args)
	{
		/*
		 * goes to the next filename-matching file
		 */
		unsigned int i,j,c=current_n(),s=flist.size();
		if( args.size() < 1 || s < 1 )return "";
		for(j=0;j<s;++j)
		{
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
				return "";
#endif
			}
		}
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
//		cc.autocmd_exec("PreNext",current());
#ifdef FIM_AUTOCMDS
		cc.autocmd_exec("PreNext",c);
#endif
		fim::string result=do_next(n);
#ifdef FIM_AUTOCMDS
//		cc.autocmd_exec("PostNext",current());
		cc.autocmd_exec("PostNext",c);
#endif
		return "";
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
			if(cp-1==current_n())--cp;
			flist.erase(flist.begin()+current_n());
			if(cp==0 && n_files()) ++cp;
			return "";
		}
		return "";
	}

	fim::string Browser::scrollforward(const std::vector<fim::string> &args)
	{
		/*
		 * scrolls the image as it were a book :)
		 *
		 * FIX ME
		 */
		fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PrePan",c);
#endif
		if(image)
		{
			if(image->onRight() && image->onBottom())
				next();
			else
			if(image->onRight())
			{
				image->pan_down();
				while(!(image->onLeft()))image->pan_left();
			}
			else image->pan_right();
		}
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
		 * FIX ME
		 */
		if(image)
		{
			if(image->onBottom()) next();
			else pan_down(std::vector<fim::string>());
		}
		return "";
	}

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
		return current();
	//	return list();
	}

	fim::string Browser::info()
	{
		return info(std::vector<fim::string>(0));
	}

	fim::string Browser::redisplay(const std::vector<fim::string> &args)
	{
		redisplay();
		return "";
	}

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
		if(image)
		{
			float factor = (float)cc.getFloatVariable("magnify_factor");
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)
			{
				if(factor) image->magnify(factor);
				else	image->magnify();
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
		if(image)
		{
			float factor = (float)cc.getFloatVariable("reduce_factor");
			fim::string c=current();
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PreScale",c);
#endif
			if(image)
			{
				if(factor) image->reduce(factor);
				else	image->reduce();
			}
#ifdef FIM_AUTOCMDS
			cc.autocmd_exec("PostScale",c);
#endif
		}
		return "";
	}

	fim::string Browser::prev(int n)
	{
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
}
