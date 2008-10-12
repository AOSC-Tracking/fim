/* $Id$ */
/*
 DisplayDevice.cpp : virtual device Fim driver file

 (c) 2008 Michele Martone

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

	DisplayDevice::DisplayDevice():fontname(NULL){}

	int DisplayDevice::get_input(int * c)
	{
		/*
		 * It is sad to place this functionality here, but often the input subsystem 
		 * is tightly bound to the output device.
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
			//r=read(fim_stdin,&c,1); if(c==0x1b){read(0,&c,3);c=(0x1b)+(c<<8);}
			/*
			 * so the next coded shoul circumvent this behaviour!
			 */
			{
				cc.tty_raw();// this, here, inhibits unwanted key printout (raw mode?!)

				fd_set set;
				int fdmax;
				struct timeval  limit;
				int timeout=1,rc,paused=0;
	
			        FD_ZERO(&set);
			        FD_SET(cc.fim_stdin, &set);
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
					return 0;	/* warning : originally a 'continue' in a loop ! */
				
				if (FD_ISSET(cc.fim_stdin,&set))rc = read(cc.fim_stdin, c, 4);
				r=rc;
				*c=int2msbf(*c);
			}
#else	
			/*
			 * this way the console switches the wrong way
			 */
			r=read(fim_stdin,&c,4);	//up to four chars should suffice
#endif


			return r;
	}

	int DisplayDevice::catchInteractiveCommand(int seconds)const
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
		
		int c,r;//char buf[64];
		//r=read(fim_stdin,&c,4);
		r=read(cc.fim_stdin,&c,1); if(r>0&&c==0x1b){read(0,&c,3);c=(0x1b)+(c<<8);}

		//we restore the previous console attributes
		tcsetattr (0, TCSAFLUSH, &sattr);

		if( r<=0 ) return -1;	/*	-1 means 'no character pressed	*/

		return c;		/*	we return the read key		*/
	}

