/* $LastChangedDate$ */
/*
 CommandConsole-init.cpp : Fim console initialization

 (c) 2010-2011 Michele Martone

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

/*
 * TODO:
 * 	framebufferdevice -> device
 * 	output methods should be moved in some other, new class
 * */
#include "fim.h"
#ifdef FIM_DEFAULT_CONFIGURATION
#include "conf.h"
#endif
#include <sys/time.h>
#include <errno.h>

#ifdef FIM_USE_READLINE
#include "readline.h"
#endif

namespace fim
{
	fim_err_t CommandConsole::init(fim::string device)
	{
		/*
		 * TODO : move most of this stuff to the constructor, some day.
		 */

		/* new : prevents atof, sprintf and such conversion mismatches! */
		setlocale(LC_ALL,"C");	/* portable (among Linux hosts) : should use dots for numerical radix separator */
		//setlocale(LC_NUMERIC,"en_US"); /* lame  */
		//setlocale(LC_ALL,""); /* just lame */

		displaydevice_=NULL;	/* TODO : is this really necessary ? */
		int xres=0,yres=0;
		bool device_failure=false;

#ifndef FIM_WITH_NO_FRAMEBUFFER
		if( device==FIM_DDN_INN_FB )
		{
			extern char *default_fbdev,*default_fbmode;
			extern int default_vt;
			extern float default_fbgamma;
			FramebufferDevice * ffdp=NULL;

			displaydevice_=new FramebufferDevice(
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
					mc_
#endif
					);
			if(!displaydevice_ || ((FramebufferDevice*)displaydevice_)->framebuffer_init()){cleanup();return -1;}
			ffdp=((FramebufferDevice*)displaydevice_);
			setVariable(FIM_VID_DEVICE_DRIVER,FIM_DDN_VAR_FB);
			if(default_fbdev)ffdp->set_fbdev(default_fbdev);
			if(default_fbmode)ffdp->set_fbmode(default_fbmode);
			if(default_vt!=-1)ffdp->set_default_vt(default_vt);
			if(default_fbgamma!=-1.0)ffdp->set_default_fbgamma(default_fbgamma);
		}
#endif	//#ifndef FIM_WITH_NO_FRAMEBUFFER


		#ifdef FIM_WITH_LIBSDL
		if(device.find(FIM_DDN_INN_SDL)==0)
		{
			DisplayDevice *sdld=NULL;
			fim::string fopts;
#if FIM_WANT_SDL_OPTIONS_STRING 
			int dsl=strlen(FIM_DDN_INN_SDL);
			int ssl=strlen(FIM_SYM_DEVOPTS_SEP_STR);
			std::string opts;

			if(device.find(FIM_DDN_INN_SDL FIM_SYM_DEVOPTS_SEP_STR)==0)
			if(device.length()>dsl+ssl)
			{
				opts=device.substr(dsl+ssl,device.length());
				fopts=opts.c_str();//FIXME
			}
#else
#endif
			sdld=new SDLDevice(
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
					mc_,
#endif
					fopts
					); if(sdld && sdld->initialize(key_bindings_)!=FIM_ERR_NO_ERROR){delete sdld ; sdld=NULL;}
			if(sdld && displaydevice_==NULL)
			{
				displaydevice_=sdld;
				setVariable(FIM_VID_DEVICE_DRIVER,FIM_DDN_VAR_SDL);
			}
			else
			{
				device_failure=true;
			}
		}
		#endif

		#ifdef FIM_WITH_CACALIB
		if(device==FIM_DDN_INN_CACA)
		{
			DisplayDevice *cacad=NULL;
			cacad=new CACADevice(
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
					mc_
#endif
					); if(cacad && cacad->initialize(key_bindings_)!=FIM_ERR_NO_ERROR){delete cacad ; cacad=NULL;}
			if(cacad && displaydevice_==NULL)
			{
				displaydevice_=cacad;
				setVariable(FIM_VID_DEVICE_DRIVER,FIM_DDN_VAR_CACA);
			}
			else
				device_failure=true;
		}
		#endif

		#ifdef FIM_WITH_AALIB
		if(device==FIM_DDN_INN_AA)
		{
		aad_=new AADevice(
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
				mc_
#endif
				);

		if(aad_ && aad_->initialize(key_bindings_)!=FIM_ERR_NO_ERROR){delete aad_ ; aad_=NULL;}
		{
		if(aad_ && displaydevice_==NULL)
		{
			displaydevice_=aad_;
			setVariable(FIM_VID_DEVICE_DRIVER,FIM_DDN_VAR_AA);

#if FIM_WANT_SCREEN_KEY_REMAPPING_PATCH
			/*
			 * FIXME
			 *
			 * seems like the keymaps get shifted when running under screen
			 * weird, isn't it ?
			 * Regard this as a weird patch.
			 * */
			const char * term = fim_getenv(FIM_CNS_TERM_VAR);
			if(term && string(term).re_match("screen"))
			{
				key_bindings_[FIM_KBD_LEFT]-=3072;
				key_bindings_[FIM_KBD_RIGHT]-=3072;
				key_bindings_[FIM_KBD_UP]-=3072;
				key_bindings_[FIM_KBD_DOWN]-=3072;
			}
#endif
		}
		else device_failure=true;
		}
		}
		#endif
		tty_raw();// this inhibits unwanted key printout (raw mode), and saves the current tty state

		if( displaydevice_==NULL)
		{
			displaydevice_=&dummydisplaydevice_;
			setVariable(FIM_VID_DEVICE_DRIVER,FIM_DDN_VAR_DUMB);
			if(device_failure)
				std::cerr << "Failure using the \""<<device<<"\" display device driver string.!\n";
			else
			if(device!=FIM_DDN_VAR_DUMB)
				std::cerr << "Unrecognized display device string \""<<device<<"\" (valid choices are "FIM_DDN_VARS")!\n",
			std::cerr << "Using the default \""<<FIM_DDN_INN_DUMB<<"\" display device instead.\n";
		}

		xres=displaydevice_->width(),yres=displaydevice_->height();

		// textual console reformatting (should go to displaydevice some day)
		displaydevice_->init_console();
		// FIXME: shall check the error result

#ifdef FIM_WINDOWS
		/* true pixels if we are in framebuffer mode */
		/* fake pixels if we are in text (er.. less than!) mode */
		if( xres<=0 || yres<=0 ) return -1;

		try
		{
			window_ = new Window( *this, Rect(0,0,xres,yres) );

			if(window_)window_->setroot();
		}
		catch(FimException e)
		{
			if( e == FIM_E_NO_MEM || true ) quit(FIM_E_NO_MEM);
		}

		/*
		 * TODO: exceptions should be launched here in case ...
		 * */
		addCommand(new Command(fim::string(FIM_FLT_WINDOW),fim::string(FIM_CMD_HELP_WINDOW), window_,&Window::fcmd_cmd));
#else
		try
		{
			viewport_ = new Viewport(*this);
		}
		catch(FimException e)
		{
			if( e == FIM_E_NO_MEM || true ) quit(FIM_E_NO_MEM);
		}
#endif
		setVariable(FIM_VID_SCREEN_WIDTH, xres);
		setVariable(FIM_VID_SCREEN_HEIGHT,yres);

		/* Here the program loads initialization scripts */
#ifdef FIM_AUTOCMDS
	    	autocmd_exec(FIM_ACM_PRECONF,"");
	    	autocmd_exec(FIM_ACM_PREHFIMRC,"");
#endif
#ifndef FIM_NOFIMRC
  #ifndef FIM_WANT_NOSCRIPTING
		char rcfile[FIM_PATH_MAX];
		const char *e = fim_getenv(FIM_CNS_HOME_VAR);

		if(preConfigCommand_!=fim::string(""))
			execute_internal(preConfigCommand_.c_str(),FIM_X_HISTORY);

		if(getIntVariable(FIM_VID_NO_DEFAULT_CONFIGURATION)==0 )
		{
    #ifdef FIM_DEFAULT_CONFIGURATION
			/* so the user could inspect what goes in the default configuration */
			setVariable(FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS,FIM_DEFAULT_CONFIG_FILE_CONTENTS);
			execute_internal(FIM_DEFAULT_CONFIG_FILE_CONTENTS,FIM_X_QUIET);
    #endif		
		}

#ifdef FIM_AUTOCMDS
	    	autocmd_exec(FIM_ACM_POSTHFIMRC,""); 
	    	autocmd_exec(FIM_ACM_PREGFIMRC,""); 
#endif

		if((getIntVariable(FIM_VID_LOAD_DEFAULT_ETC_FIMRC)==1 )
		&& (getStringVariable(FIM_VID_DEFAULT_ETC_FIMRC)!=FIM_CNS_EMPTY_STRING)
				)
		{
			string ef=getStringVariable(FIM_VID_DEFAULT_ETC_FIMRC);
			if(is_file(ef.c_str()))
				if(-1==executeFile(ef.c_str()));
		}
		
#ifdef FIM_AUTOCMDS
		/* execution of command line-set autocommands */
	    	autocmd_exec(FIM_ACM_POSTGFIMRC,""); 
	    	autocmd_exec(FIM_ACM_PREUFIMRC,""); 
#endif		
		{
			#include "grammar.h"
			setVariable(FIM_VID_FIM_DEFAULT_GRAMMAR_FILE_CONTENTS,FIM_DEFAULT_GRAMMAR_FILE_CONTENTS);
		}

		if(e && strlen(e)<FIM_PATH_MAX-8)//strlen("/"FIM_CNS_USR_RC_FILEPATH)+2
		{
			strcpy(rcfile,e);
			strcat(rcfile,"/"FIM_CNS_USR_RC_FILEPATH);
			if(getIntVariable(FIM_VID_NO_RC_FILE)==0 )
			{
				if(
					(!is_file(rcfile) || -1==executeFile(rcfile))
	//			&&	(!is_file(FIM_CNS_SYS_RC_FILEPATH) || -1==executeFile(FIM_CNS_SYS_RC_FILEPATH))
				  )
  #endif
#endif
				{
					/*
					 if no configuration file is present, or fails loading,
					 we use the default configuration (raccomended !)  !	*/
  #ifdef FIM_DEFAULT_CONFIGURATION
					// 20110529 commented the following, as it is a (harmful) duplicate execution 
					//execute_internal(FIM_DEFAULT_CONFIG_FILE_CONTENTS,FIM_X_QUIET);
  #endif		
				}
#ifndef FIM_NOFIMRC
  #ifndef FIM_WANT_NOSCRIPTING
			}
		}
  #endif		
#endif		
#ifdef FIM_AUTOCMDS
	    	autocmd_exec(FIM_ACM_POSTUFIMRC,""); 
	    	autocmd_exec(FIM_ACM_POSTCONF,"");
#endif
#ifndef FIM_WANT_NOSCRIPTING
		for(size_t i=0;i<scripts_.size();++i) executeFile(scripts_[i].c_str());
#endif		
#ifdef FIM_AUTOCMDS
		if(postInitCommand_!=fim::string(""))
			autocmd_add(FIM_ACM_PREEXECUTIONCYCLE,"",postInitCommand_.c_str());
		if(postExecutionCommand_!=fim::string(""))
			autocmd_add(FIM_ACM_POSTEXECUTIONCYCLE,"",postExecutionCommand_.c_str());
#endif
		/*
		 *	FIXME : A TRADITIONAL /etc/fimrc LOADING WOULDN'T BE BAD..
		 * */
#ifdef FIM_USE_READLINE
		rl::initialize_readline( displaydevice_==NULL );
		load_history();
#endif
		if(getIntVariable(FIM_VID_SANITY_CHECK)==1 )
		{
			/* experimental */
			displaydevice_->quickbench();
			quit(return_code_);
			exit(return_code_);
		}
		return FIM_ERR_NO_ERROR;
	}

	void CommandConsole::dumpDefaultFimrc()const
	{
#ifdef FIM_DEFAULT_CONFIGURATION
		std::cout << FIM_DEFAULT_CONFIG_FILE_CONTENTS << "\n";
#endif
	}
}


