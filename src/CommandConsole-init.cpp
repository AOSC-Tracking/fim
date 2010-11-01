/* $Id$ */
/*
 CommandConsole-init.cpp : Fim console initialization

 (c) 2010-2010 Michele Martone

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
	fim_err_t CommandConsole::init(string device)
	{
		/*
		 * TODO : move most of this stuff to the constructor, some day.
		 */

		/* new : prevents atof, sprintf and such conversion mismatches! */
		setlocale(LC_ALL,"C");	/* portable (among Linux hosts) : should use dots for numerical radix separator */
		//setlocale(LC_NUMERIC,"en_US"); /* lame  */
		//setlocale(LC_ALL,""); /* just lame */

		displaydevice=NULL;	/* TODO : is this really necessary ? */
		int xres=0,yres=0;

#ifndef FIM_WITH_NO_FRAMEBUFFER
		if( device=="fb" )
		{
			extern char *default_fbdev,*default_fbmode;
			extern int default_vt;
			extern float default_fbgamma;
			FramebufferDevice * ffdp=NULL;

			displaydevice=new FramebufferDevice(mc);
			if(!displaydevice || ((FramebufferDevice*)displaydevice)->framebuffer_init()){cleanup();return -1;}
			ffdp=((FramebufferDevice*)displaydevice);
			setVariable(FIM_VID_DEVICE_DRIVER, "fbdev");
			if(default_fbdev)ffdp->set_fbdev(default_fbdev);
			if(default_fbmode)ffdp->set_fbmode(default_fbmode);
			if(default_vt!=-1)ffdp->set_default_vt(default_vt);
			if(default_fbgamma!=-1.0)ffdp->set_default_fbgamma(default_fbgamma);
		}
#endif	//#ifndef FIM_WITH_NO_FRAMEBUFFER


		#ifdef FIM_WITH_LIBSDL
		if(device=="sdl")
		{
			DisplayDevice *sdld=NULL;
			sdld=new SDLDevice(mc); if(sdld && sdld->initialize(key_bindings)!=0){delete sdld ; sdld=NULL;}
			if(sdld && displaydevice==NULL)
			{
				displaydevice=sdld;
				setVariable(FIM_VID_DEVICE_DRIVER, "sdl");
			}
		}
		#endif

		#ifdef FIM_WITH_CACALIB
		if(device=="caca")
		{
			DisplayDevice *cacad=NULL;
			cacad=new CACADevice(mc); if(cacad && cacad->initialize(key_bindings)!=0){delete cacad ; cacad=NULL;}
			if(cacad && displaydevice==NULL)
			{
				displaydevice=cacad;
				setVariable(FIM_VID_DEVICE_DRIVER, "cacalib");
			}
		}
		#endif

		#ifdef FIM_WITH_AALIB
		if(device=="aa")
		{
		aad=new AADevice(mc);

		if(aad && aad->initialize(key_bindings)!=0){delete aad ; aad=NULL;}
		if(aad && displaydevice==NULL)
		{
			displaydevice=aad;
			setVariable(FIM_VID_DEVICE_DRIVER, "aalib");

#if 1
			/*
			 * FIXME
			 *
			 * seems like the keymaps get shifted when running under screen
			 * weird, isn't it ?
			 * Regard this as a weird patch.
			 * */
			const char * term = fim_getenv("TERM");
			if(term && string(term).re_match("screen"))
			{
				key_bindings["Left"]-=3072;
				key_bindings["Right"]-=3072;
				key_bindings["Up"]-=3072;
				key_bindings["Down"]-=3072;
			}
#endif
		}
		}
		#endif
		tty_raw();// this inhibits unwanted key printout (raw mode), and saves the current tty state

		if( displaydevice==NULL)
		{
			displaydevice=&dummydisplaydevice;
			setVariable(FIM_VID_DEVICE_DRIVER, "dummy");
		}

		xres=displaydevice->width(),yres=displaydevice->height();

		// textual console reformatting (should go to displaydevice some day)
		displaydevice->init_console();
	

#ifdef FIM_WINDOWS
	
		/* true pixels if we are in framebuffer mode */
		/* fake pixels if we are in text (er.. less than!) mode */
		if( xres<=0 || yres<=0 ) return -1;

		try
		{
			window = new Window( *this, Rect(0,0,xres,yres) );

			if(window)window->setroot();
		}
		catch(FimException e)
		{
			if( e == FIM_E_NO_MEM || true ) quit(FIM_E_NO_MEM);
		}

		/*
		 * TODO: exceptions should be launched here in case ...
		 * */
		addCommand(new Command(fim::string("window" ),fim::string("manipulates the window system windows"), window,&Window::cmd));
#else
		try
		{
			viewport = new Viewport(*this);
		}
		catch(FimException e)
		{
			if( e == FIM_E_NO_MEM || true ) quit(FIM_E_NO_MEM);
		}
#endif
		setVariable(FIM_VID_SCREEN_WIDTH, xres);
		setVariable(FIM_VID_SCREEN_HEIGHT,yres);

		/*
		 *	Here the program loads initialization scripts :
		 * 	the default configuration file, and user invoked scripts.
		 */
//		executeFile("/etc/fim.conf");	//GLOBAL DEFAULT CONFIGURATION FILE
//		executeFile("/etc/fimrc");	//GLOBAL DEFAULT CONFIGURATION FILE

#ifndef FIM_NOFIMRC
  #ifndef FIM_NOSCRIPTING
		char rcfile[FIM_PATH_MAX];
		const char *e = fim_getenv("HOME");

		/* default, hard-coded configuration first */
		if(getIntVariable(FIM_VID_LOAD_DEFAULT_ETC_FIMRC)==1 )
		{
			if(is_file("/etc/fimrc"))
				if(-1==executeFile("/etc/fimrc"));
		}
		
		/* default, hard-coded configuration first */
		if(getIntVariable(FIM_VID_NO_DEFAULT_CONFIGURATION)==0 )
		{
    #ifdef FIM_DEFAULT_CONFIGURATION
			/* so the user could inspect what goes in the default configuration */
			setVariable(FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS,FIM_DEFAULT_CONFIG_FILE_CONTENTS);

			execute(FIM_DEFAULT_CONFIG_FILE_CONTENTS,0,1);
    #endif		
		}

		if(e && strlen(e)<FIM_PATH_MAX-8)//strlen("/.fimrc")+2
		{
			strcpy(rcfile,e);
			strcat(rcfile,"/.fimrc");
			if(getIntVariable(FIM_VID_NO_RC_FILE)==0 )
			{
				if(
					(!is_file(rcfile) || -1==executeFile(rcfile))
				&&	(!is_file("/etc/fimrc") || -1==executeFile("/etc/fimrc"))
				  )
  #endif
#endif
				{
					/*
					 if no configuration file is present, or fails loading,
					 we use the default configuration (raccomended !)  !	*/
  #ifdef FIM_DEFAULT_CONFIGURATION
					execute(FIM_DEFAULT_CONFIG_FILE_CONTENTS,0,1);
  #endif		
				}
#ifndef FIM_NOFIMRC
  #ifndef FIM_NOSCRIPTING
			}

		}
  #endif		
#endif		
#ifndef FIM_NOSCRIPTING
		for(size_t i=0;i<scripts.size();++i) executeFile(scripts[i].c_str());
#endif		
#ifdef FIM_AUTOCMDS
		if(postInitCommand!=fim::string(""))
			autocmd_add("PreExecutionCycle","",postInitCommand.c_str());
		if(postExecutionCommand!=fim::string(""))
			autocmd_add("PostExecutionCycle","",postExecutionCommand.c_str());
#endif
		/*
		 *	FIXME : A TRADITIONAL /etc/fimrc LOADING WOULDN'T BE BAD..
		 * */
#ifdef FIM_USE_READLINE
		rl::initialize_readline( displaydevice==NULL );
		load_history();
#endif
		if(getIntVariable(FIM_VID_SANITY_CHECK)==1 )
		{
			/* experimental */
			displaydevice->quickbench();
			quit(return_code);
			exit(return_code);
		}
		return 0;
	}

	void CommandConsole::dumpDefaultFimrc()const
	{
#ifdef FIM_DEFAULT_CONFIGURATION
		std::cout << FIM_DEFAULT_CONFIG_FILE_CONTENTS << "\n";
#endif
	}
}


