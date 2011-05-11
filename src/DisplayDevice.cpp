/* $Id$ */
/*
 DisplayDevice.cpp : virtual device Fim driver file

 (c) 2008-2011 Michele Martone

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

#include "fim.h"
#include "DisplayDevice.h"

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	DisplayDevice::DisplayDevice(MiniConsole & mc):fontname_(NULL)
	,mc_(mc)
#else
	DisplayDevice::DisplayDevice():fontname_(NULL)
#endif
	,f_(NULL)
	,debug_(false)
	,redraw_(0)
	,finalized_(false)
	{}

	int DisplayDevice::get_input(fim_key_t * c)
	{
		*c=0;
		/*
		 * It is sad to place this functionality here, but often the input subsystem 
		 * is tightly bound to the output device.
		 * FIXME : before, it accepted unsigned int
		 * */
			int r=0;
#ifdef  FIM_SWITCH_FIXUP
			/*
			 * this way the console switches the right way :
			 * the following code taken live from the original fbi.c
			 */

			/*
			 * patch: the following read blocks the program even when switching console
			 */
			//r=read(fim_stdin_,&c,1); if(c==0x1b){read(0,&c,3);c=(0x1b)+(c<<8);}
			/*
			 * so the next coded shoul circumvent this behaviour!
			 */
			{
				fd_set set;
				int fdmax;
				struct timeval  limit;
				int timeout=1,rc,paused=0;
	
			        FD_ZERO(&set);
			        FD_SET(cc.fim_stdin_, &set);
			        fdmax = 1;
#ifdef FBI_HAVE_LIBLIRC
				/*
				 * expansion code :)
				 */
			        if (-1 != lirc) {
			            FD_SET(lirc,&set);
			            fdmax = lirc+1;
			        }
#endif
			        limit.tv_sec = timeout;
			        limit.tv_usec = 0;
			        rc = select(fdmax, &set, NULL, NULL,
			                    (0 != timeout && !paused) ? &limit : NULL);
				if(handle_console_switch())	/* this may have side effects, though */
				{
					return 0;	/* warning : originally a 'continue' in a loop ! */
				}
				
				if (FD_ISSET(cc.fim_stdin_,&set))rc = read(cc.fim_stdin_, c, 4);
				r=rc;
				*c=int2msbf(*c);
			}
#else	
			/*
			 * this way the console switches the wrong way
			 */
			r=read(fim_stdin_,&c,4);	//up to four chars should suffice
#endif
			//std::cout << (int)*c<<"\n";

			return r;
	}

	fim_key_t DisplayDevice::catchInteractiveCommand(fim_ts_t seconds)const
	{
		/*	
		 *
		 *	THIS DOES NOT WORK, BECAUSE IT IS A BLOCKING READ.
		 *	MAKE THIS READ UNBLOCKING AN UNCOMMENT. <- ?
		 *	
		 *	FIX ME
		 *
		 *	NOTE : this call should 'steal' circa 1/10 of second..
		 */
		fd_set          set;
		FD_SET(0, &set);
		size_t rc=0;

		struct termios tattr;
		struct termios sattr;
		//we set the terminal in raw mode.
		    
	//	fcntl(0,F_GETFL,&saved_fl);
		tcgetattr (0, &sattr);

		//fcntl(0,F_SETFL,O_BLOCK);
		memcpy(&tattr,&sattr,sizeof(struct termios));
		tattr.c_lflag &= ~(ICANON|ECHO);
		tattr.c_cc[VMIN]  = 0;
		tattr.c_cc[VTIME] = 1 * (seconds==0?1:(seconds*10)%256);
		tcsetattr (0, TCSAFLUSH, &tattr);
		
		fim_key_t c,r;//char buf[64];
		//r=read(fim_stdin_,&c,4);
		// FIXME : read(.,.,3) is NOT portable. DANGER
		r=read(cc.fim_stdin_,&c,1); if(r>0&&c==0x1b){rc=read(0,&c,3);c=(0x1b)+(c<<8);/* we should do something with rc now */}

		//we restore the previous console attributes
		tcsetattr (0, TCSAFLUSH, &sattr);

		if( r<=0 ) return -1;	/*	-1 means 'no character pressed	*/

		return c;		/*	we return the read key		*/
	}

#ifndef FIM_KEEP_BROKEN_CONSOLE
void DisplayDevice::fb_status_screen_new(const fim_char_t *msg, fim_bool_t draw, fim_flags_t flags)//experimental
{
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	int r;
	
	if(flags==0x03)
	{
		/* clear screen sequence */
		mc_.clear();
		return;
	}

	if( flags==0x01 ) { mc_.scroll_down(); return; }
	if( flags==0x02 ) { mc_.scroll_up(); return; }

	r=mc_.add(msg);
	if(r==FIM_ERR_BUFFER_FULL)
	{
		r=mc_.grow();
		if(r==FIM_ERR_GENERIC)return;
		r=mc_.add(msg);
		if(r==FIM_ERR_GENERIC)return;
	}

	if(!draw )return;//CONVENTION!
	
	//fb_memset(fb_mem ,0,fb_fix.line_length * (fb_var.yres/2)*(fs_bpp));
	cc.displaydevice_->lock();
#if 1
	// FIXME: this is temporary
	{
		int ls=cc.getIntVariable(FIM_VID_CONSOLE_ROWS);
		int fh=f_?f_->height:1;
		clear_rect(0, width()-1, 0,fh*ls);
	}
#else
	clear_rect(0, width()-1, 0,ls);
#endif
	cc.displaydevice_->unlock();
	mc_.dump();
//	mc_.dump(0,1000000);
#endif
	return;
}
#endif

fim_err_t DisplayDevice::console_control(fim_cc_t arg)//experimental
{
	if(arg==0x01)fb_status_screen_new(NULL,false,arg);
	if(arg==0x02)fb_status_screen_new(NULL,false,arg);
	if(arg==0x03)fb_status_screen_new(NULL,false,arg);
	return FIM_ERR_NO_ERROR;
}

fim_err_t DisplayDevice::init_console()
{
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	if(f_)
	{	
		mc_.setRows ((height()/f_->height)/2);
		mc_.reformat( width() /f_->width    );
	}
	else
	{
		mc_.setRows ( height()   );
		mc_.reformat( width()    );
	}
#endif
	return FIM_ERR_NO_ERROR;
}

DisplayDevice::~DisplayDevice()
{
}

void DisplayDevice::quickbench()
{
	/*
		a quick draw benchmark and sanity check.
		currently performs only the clear function.
	*/
	double tbtime,btime;// ms
	size_t times=1;
	string msg="fim check";

	std::cout << msg << " : " << "please be patient\n";

	times=1;
	tbtime=1000.0,btime=0.0;// ms

	do
	{
		btime=-getmilliseconds();
		clear_rect(0, width()-1, 0,height()/2);
		btime+=getmilliseconds();
		++times;
		tbtime-=btime;
	}
	while(btime>=0.0 && tbtime>0.0 && times>0);
	--times;
	tbtime=1000.0-tbtime;

	std::cout << msg << " : " << ((double)times)/((tbtime)*1.e-3) << " clears/s\n";
}

