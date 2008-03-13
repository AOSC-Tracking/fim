/* $Id$ */
/*
 DebugConsole.cpp : Fim virtual console display.

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


namespace fim
{
/*
 *	20080313	Note that there were problems (BUGS) when adding long lines (ex.: lwidth=128).
 *	in interactive mode, typing
 *
 *	10i\t
 *	used to trigger a crash. This was fixed, but some more bugs could still reside here.
 *
 *	This class is still UNFINISHED.
 * */

//#define DEBUG 1

extern fim::FramebufferDevice ffd;
extern fim::CommandConsole cc;
		#define min(x,y) ((x)<(y)?(x):(y))
		int MiniConsole::dump(int f, int l)
		{
			/*
			 *
			 * if f <= l and f>= 0 
			 * lines from min(f,cline) to min(l,cline) will be dumped
			 *
			 * if f<0 and l>=0 and f+l<0 and -f<=cline,
			 * lines from cline+f to cline-l will be dumped
			 *
			 * */
			int i;
			char *s;
	
			if(f<0 && l>=0 && f+l<0 && -f<=cline) { f=cline+f; l=cline-l; }
			else
				if(f<=l && f>=0){f=f>cline?cline:f;l=l>cline?cline:l;}
			else
				return -1;// unsupported combination
			
			
//			for(i=0  ;i<R ;++i) do_dump(i-f,s);//from 0 to l-f
//			for(i=f ;i<=l ;++i) do_dump(i  ,s);//from 0 to l-f
		
			//if(!cc.drawOutput())return 0;//CONVENTION!
			// FIX ME
			char buf[200];

			if(*bp)return -1;//if *bp then we could print garbage so we exit before damage is done

			/*
			 * resetting of f an l
			 * */

		/*
		 * WE HAVE THE NEED TO KNOW THE CURRENT LINE LENGTH..
		 * */
//			static unsigned char blanks[200];
//			static int blanked=0;
//			if(!blanked){unsigned char *k=blanks+200;while(k-->blanks){*k=' ';}k[199]='\0';}

		//	f=0; l=cline;
		//	if(l>cline)l=cline;	// we crop to the lines that will fit in the console
		//	if(l+1-f >rows)f=l-rows+1;	// we crop to the lines that will fit in the console
#ifndef DEBUG
	    		int y = 1*ffd.fb_font_height();
//	    		for(i=0  ;i<rows ;++i)ffd.fs_puts(ffd.fb_font_get_current_font(), 0, y*(i),blanks);
#endif
	    		for(i=f  ;i<=l   ;++i)
			{
//				char bb[100];
//				sprintf(bb,"%d",i);
				//bzero(buf,sizeof(buf));
				//strncpy(buf,line[i],min(lwidth,strlen(line[i])));
				//strncpy(buf,line[i],(line[i]+ccol)-bp);
				int t = i<cline?(line[i+1]-line[i]):(lwidth);
				//while(buf[t]=='\n' && t>0)--t;
				while(buf[t]=='\n' )--t;
				//int t = i<cline?(line[i+1]-line[i]):(strchr(line[i],'\n')-line[i]);
				strncpy(buf,line[i],t);
				
#if 1
				//yes, both
				buf[t]='\0';
				buf[lwidth]='\0';
#else
				char *k=buf+t;
				buf[lwidth]='\0';
				while(*k){*k=' ';++k;}
#endif				
				sanitize_string_from_nongraph_except_newline(buf,0);
#ifndef DEBUG
			//	ffd.fs_puts(ffd.fb_font_get_current_font(), 0, y*(i-f), (unsigned char*)bb);
				ffd.fs_puts(ffd.fb_font_get_current_font(), 0, y*(i-f), (unsigned char*)buf);
	//			ffd.fs_puts(ffd.fb_font_get_current_font(), t, y*(i-f), (unsigned char*)(blanks+sizeof(blanks)-1-(lwidth-t)));
#else
				printf("%s\n",buf);
#endif
			}
			return 0;
		#undef min
		}

		void MiniConsole::add(const char* cs)
		{
			//const char *cs=fs.c_str();
			char *s,*b;
			int nc;
			int nl,ol=cline;

	//		int nls=cline; //TEMPORARY

			if(!cs)return;
			nc=strlen(cs);
			if(!nc)return;
			nl=lines_count(cs,lwidth);
			// we count exactly the number of new entries needed in the arrays we have
			if((s=strchr(cs,'\n'))!=NULL && s!=cs)nl+=(ccol+(s-cs-1))/lwidth;// single line with \n or multiline
			else nl+=(strlen(cs)+ccol)/lwidth;	// single line, with no terminators

			/*
			 * we check for room (please note that nl >= the effective new lines introduced , while
			 * nc amounts to the exact extra room needed )
			 * */
			if(nc+1+(bp-buffer)>bsize || nl+1+cline>lsize)return;//no room : realloc needed ; 1 is for secur1ty

			// we copy the whole new string in our buffer
			strcpy(bp,cs);
			//sanitize_string_from_nongraph_except_newline(bp,0);
			s=bp-ccol;// we will work in our buffer space now on
			b=s;
//			std::cout << "<>:"<<bp<<" - " << ccol << " = " << s << "\n";
			while(*s && (s=(char*)next_row(s,lwidth))!=NULL && *s)
			{
				line[++cline]=s;// we keep track of each new line
				ccol=0;
				bp=s;
			}// !s || !*s
			if(!*s && s-b==lwidth){line[++cline]=(bp=s);}// we keep track of the last line too
			
	//		nls=cline-nls;
	//		char nlb[100];sprintf(nlb,"new %d lines, nl=%d!",nls,nl);
	//		cc.set_status_bar(nlb,NULL);
			if(ol==cline)
			{
				ccol=strlen(line[cline]);	// we update the current (right after last) column
				bp+=strlen(bp);	// the buffer points to the current column
			}
			else
			{
				ccol=strlen(bp);	// we update the current (right after last) column
				bp+=ccol;	// the buffer points to the current column
			}
//			std::cout << "lbp: "<< ccol<<"\n";
		}

		MiniConsole::MiniConsole()
		{
			int BS=1024;
			bsize =BS*128;
			lsize =BS*8;

			lwidth=128;
			rows=24;

			cline =0;
			ccol=0;
			buffer=(char*) calloc(bsize,sizeof(char ));
			line  =(char**)calloc(lsize,sizeof(char*));
			bp=buffer;
			if(!buffer || !line)
			{
				bsize=0;
				lsize=0;
				if(buffer)free(buffer);
				if(line  )free(line);
			}
			else
			{
			
				line[cline]=buffer;
			}
		}

		int MiniConsole::dump(int amount)
		{
			if(amount > 0)
			{
				// dumps the last amount of lines
				amount=amount>cline?cline:amount;
				dump(cline-amount+1,cline);
			}else
			if(amount ==0)
			{
				// dumps all of the lines
				dump(0,cline);
			}else
			if(amount < 0)
			{
				// dumps the first amount of lines
				if(-amount>=cline)cline+=amount;
				dump(0,cline);
			}
			return 0;
		}

		int MiniConsole::grow_lines(int glines)
		{
			/*
			 * grow of a specified amount of lines the lines array
			 *
			 * see the doc for grow() to get more
			 * */
			/* TEST ME AND FINISH ME */
			if(glines< 0)return -1;
			if(glines==0)return  0;
			char **p;int i;
			p=line;
			line=(char**)realloc(line,bsize+glines*sizeof(char*));
			if(!line){line=p;return -1;/* no change */}
			lsize+=glines;
			return 0;
		}

		int MiniConsole::grow_buffer(int gbuffer)
		{
			/*
			 * grow of a specified amount of lines the buffer array
			 *
			 * see the doc for grow() to get more
			 * */
			/* TEST ME AND FINISH ME */
			if(gbuffer< 0)return -1;
			if(gbuffer==0)return  0;
			char *p;int i,d;
			p=buffer;
			buffer=(char*)realloc(buffer,bsize+gbuffer*sizeof(char));
			if(!buffer){buffer=p;return -1;/* no change */}
			if((d=(p-buffer))!=0)// in the case a shift is needed
			{
				for(i=0;i<cline;++i)line[i]+=d;
				bp+=d;
			}
			bsize+=gbuffer;
			return 0;
		}

		int MiniConsole::grow()
		{
			return grow(128,1024);
		}

		int MiniConsole::grow(int glines, int gbuffer)
		{
			/*
			 * grow of a specified amount of lines or bytes the 
			 * current line and text buffers; i.e.: make room
			 * to support glines more lines and gbuffer more characters.
			 *
			 * grow values can be negative. in this case, the current 
			 * buffers will be shrunk of the specified amounts.
			 *
			 * consistency will be preserved by deleting a certain amount
			 * of lines: the older ones.
			 *
			 * a zero amount for a value imply the line or buffer arrays
			 * to remain untouched.
			 * */
			/* FINISH ME AND TEST ME */
			int gb,gl;
			gb=grow_buffer(gbuffer);
			gl=grow_lines(gbuffer);
			return !( gb==0 && 0==gl );// 0 if both 0
		}


		int MiniConsole::reformat(int newlsize)
		{
			/*
			 * This method reformats the whole buffer array; that is, it recomputes
			 * line information for it, thus updating the whole line array contents.
			 * It may fail, in the case a new line width (smaller) is desired, because
			 * more line information would be needed.
			 *
			 * If the new lines are longer than before, then it could not fail.
			 * Upon a successful execution, the width is updated.
			 *
			 * WRITE ME
			 * */
			/* FINISH ME AND TEST ME */
			if(newlsize==lsize)return 0;//are we sure?
			if(newlsize> lsize)
			{
				// no risk
				return -1;
			}
			else
			{
				// risk
				return -1;
			}
			return -1;
		}

		int MiniConsole::do_dump(int line,char *s)
		{
			/*
			 * this method dumps on the output device at line 'line' the string s.
			 * */
			return -1;
			//y = 1*fb_font_height();
			//for(i=0  ;i<R ;++i) fs_puts(fb_font_get_current_font(), 0, y*(i), (unsigned char*)columns[i]);
		}
		int MiniConsole::dump()
		{
			dump((cline-rows+1)>=0?(cline-rows+1):0,cline);
			return 0;
		}

}
