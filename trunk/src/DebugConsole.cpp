/* $LastChangedDate$ */
/*
 DebugConsole.cpp : Fim virtual console display.

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

#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
namespace fim
{

		#define min(x,y) ((x)<(y)?(x):(y))
		int MiniConsole::line_length(int li)
		{
			if(li<cline_)
			{
				return li<cline_?(line_[li+1]-line_[li]):(ccol_);
			}
			else
			if(li<0)return 0;
			// in the case li==cline_, ccol_==bp_-buffer_ will do the job:
			return ccol_;
		}

		int MiniConsole::do_dump(int f_, int l)const
		{
			/*
			 *
			 * if f_ <= l and f_>= 0 
			 * lines from min(f_,cline_) to min(l,cline_) will be dumped
			 *
			 * if f_<0 and l>=0 and f_+l<0 and -f_<=cline_,
			 * lines from cline_+f_ to cline_-l will be dumped
			 *
			 * FIXME : this function returns often with -1 !
			 * */
			int i;
			const int maxcols = cc_.displaydevice_->get_chars_per_line();
	
			if(f_<0 && l>=0 && f_+l<0 && -f_<=cline_) { f_=cline_+f_; l=cline_-l; }
			else
				if(f_<=l && f_>=0){f_=f_>cline_?cline_:f_;l=l>cline_?cline_:l;}
			else
				return -1;// unsupported combination
			
			if(maxcols<1)
			{
#if FIM_WANT_OVERLY_VERBOSE_DUMB_CONSOLE
				if(maxcols==0)
				{
					// a fixup for the dumb console
					int n=1+line_[l]-line_[f_];
					char*buf=NULL;
					if(n>0 && (buf=(char*)fim_malloc(n))!=NULL)
					{
						strncpy(buf,line_[f_],n);
						buf[n-1]='\0';
						cc_.displaydevice_->fs_puts(cc_.displaydevice_->f_,0,0,buf);
						fim_free(buf);
					}
				}
				else
#endif
					return -1;
			}

			char *buf = (char*) fim_malloc(maxcols+1); // ! we work on a stack, don't we ?! Fortran teaches us something here.
			if(!buf)return -1;

			if(*bp_){fim_free(buf);return -1;}//if *bp_ then we could print garbage so we exit before damage is done

			int fh=cc_.displaydevice_->f_ ? cc_.displaydevice_->f_->height:1; // FIXME : this is not clean
			l-=f_; l%=(rows_+1); l+=f_;

			/* FIXME : the following line_ is redundant in fb, but not in SDL 
			 * moreover, it seems useless in AA (could be a bug)
			 * */
			if(fh*(l-f_+1)>=cc_.displaydevice_->height())
				goto done;
			cc_.displaydevice_->clear_rect(0, cc_.displaydevice_->width()-1, 0 ,fh*(l-f_+1) );

			// fs_puts alone won't draw on screen, but in the back plane, so unlock/flip will be necessary
			cc_.displaydevice_->lock();

	    		for(i=f_  ;i<=l   ;++i)
			{
				int t = (i<cline_?(line_[i+1]-line_[i]):(ccol_))%(min(maxcols,lwidth_)+1);
				if( t<0 ){fim_free(buf);return -1;} // hmmm
				strncpy(buf,line_[i],t);
				while(buf[t-1]=='\n' && t>0)--t;
				buf[maxcols]='\0';
				while(t<maxcols){buf[t++]=' ';}
				/*
				 * Since the user is free to set any value > 0 for the line_ width,
				 * we truncate the line_ for the interval exceeding the screen width.
				 * */
				buf[ maxcols ]='\0';
				cc_.displaydevice_->fs_puts(cc_.displaydevice_->f_, 0, fh*(i-f_), (const fim_char_t*)buf);
			}

			/* FIXME : THE FOLLOWING IS A FIXUP FOR AA!  */
	    		for(i=0  ;i<scroll_ ;++i)
			{
				int t = (min(maxcols,lwidth_)+1);
				fim_memset(buf,' ',t);
				buf[t-1]='\0';
				cc_.displaydevice_->fs_puts(cc_.displaydevice_->f_, 0, fh*((l-f_+1)+i), (const fim_char_t*)buf);
			}
			cc_.displaydevice_->unlock();

			cc_.displaydevice_->flush();
done:
			fim_free(buf);
			return 0;
		#undef min
		}

		fim_err_t MiniConsole::add(const char* cso_)
		{
			char *s=NULL,*b=NULL;
			int nc;
			int nl,ol=cline_;
			char *cs=NULL;/* using co would mean provoking the compiler */
			char*cso=(char*)cso_;// FIXME

#if FIM_WANT_MILDLY_VERBOSE_DUMB_CONSOLE
			if(cc_.displaydevice_ && 0==cc_.displaydevice_->get_chars_per_column())
			{
				// we display input as soon as it received.
				if(this->getGlobalIntVariable(FIM_VID_DISPLAY_CONSOLE))
					cc_.displaydevice_->fs_puts(cc_.displaydevice_->f_,0,0,(const fim_char_t*)cso);
			}
#endif
			cs=dupstr(cso);

			if(!cs)goto rerr;
			nc=strlen(cs);
			if(!nc)goto rerr;
			nl=lines_count(cs,lwidth_);
			if(lwidth_<1)goto rerr;
			nl=lines_count(cs,lwidth_);
			// we count exactly the number of new entries needed in the arrays we have
			if((s=const_cast<char*>(strchr(cs,'\n')))!=NULL && s!=cs)nl+=(ccol_+(s-cs-1))/lwidth_;// single line_ with \n or multiline
			else nl+=(strlen(cs)+ccol_)/lwidth_;	// single line_, with no terminators

			/*
			 * we check for room (please note that nl >= the effective new lines introduced , while
			 * nc amounts to the exact extra room needed )
			 * */
			if(nc+1+(int)(bp_-buffer_)>bsize_ || nl+1+cline_>lsize_)return FIM_ERR_BUFFER_FULL;//no room : realloc needed ; 1 is for secur1ty
			scroll_=scroll_-nl<0?0:scroll_-nl;

			// we copy the whole new string in our buffer_
			strcpy(bp_,cs);
			fim_free(cs); cs=NULL;
			sanitize_string_from_nongraph_except_newline(bp_,0);
			s=bp_-ccol_;// we will work in our buffer_ space now on
			b=s;
			while(*s && (s=(char*)next_row(s,lwidth_))!=NULL && *s)
			{
				line_[++cline_]=s;// we keep track of each new line_
				ccol_=0;
				bp_=s;
			}// !s || !*s
			if(!*s && s-b==lwidth_){line_[++cline_]=(bp_=s);}// we keep track of the last line_ too
			

			if(ol==cline_)
			{
				ccol_=strlen(line_[cline_]);	// we update the current (right after last) column
				bp_+=strlen(bp_);	// the buffer_ points to the current column
			}
			else
			{
				ccol_=strlen(bp_);	// we update the current (right after last) column
				bp_+=ccol_;	// the buffer_ points to the current column
			}
			return FIM_ERR_NO_ERROR;
rerr:
			fim_free(cs);
			return FIM_ERR_GENERIC;
		}

		fim_err_t MiniConsole::setRows(int nr)
		{
			/*
			 * We update the displayed rows_, if this is physically possible
			 * If nr is negative, no correctness checks will occur.
			 * ( useful if calling this routine with NULL displaydevice.. )
			 * */
			int maxrows;
			if(nr<0)
			{
				rows_=-nr;
				return FIM_ERR_NO_ERROR;
			}
			maxrows = cc_.displaydevice_->get_chars_per_column();
			if(nr>0 && nr<=maxrows)
			{
				rows_=nr;
				return FIM_ERR_NO_ERROR;
			}
			return FIM_ERR_GENERIC;
		}

		MiniConsole::MiniConsole(CommandConsole & cc,int lw, int r)
		:
		cc_(cc),
		buffer_(NULL),
		line_(NULL),
		bp_(NULL),
		bsize_(0),
		lsize_(0),
		ccol_(0),
		cline_(0),
		lwidth_(0),
		rows_(0)
		{
			/*
			 * We initialize the console
			 * */
			int BS=FIM_CONSOLE_BLOCKSIZE;	//block size of 1k

			bsize_ = BS * FIM_CONSOLE_DEF_WIDTH;
			lsize_ = BS *   8;

			lwidth_=lw<=0?FIM_CONSOLE_DEF_WIDTH:lw;
			rows_=r<=0?FIM_CONSOLE_DEF_ROWS:r;

			cline_ =0;
			ccol_  =0;
			buffer_=(char*) fim_calloc(bsize_,sizeof(char ));
			line_  =(char**)fim_calloc(lsize_,sizeof(char*));
			bp_    =buffer_;
			if(!buffer_ || !line_)
			{
				bsize_=0;
				lsize_=0;
				if(buffer_)fim_free(buffer_);
				if(line_  )fim_free(line_);
			}
			else
			{
			
				line_[cline_]=buffer_;
			}
			add(FIM_MSG_CONSOLE_FIRST_LINE_BANNER);
		}

		int MiniConsole::do_dump(int amount)const
		{
			/*
			 * We dump:
			 * 	the last amount of lines if	amount >  0
			 * 	all the lines if		amount == 0
			 * 	the first ones if		amount <  0
			 * */
			if(amount > 0)
			{
				// dumps the last amount of lines
				amount=amount>cline_?cline_:amount;
				do_dump(cline_-amount+1,cline_);
			}
			else
			if(amount ==0)
			{
				// dumps all of the lines
				do_dump(0,cline_);
			}
			else
			if(amount < 0)
			{
				// dumps the first amount of lines
				if(-amount>=cline_)amount+=cline_;
				do_dump(0,-amount);
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
			char **p;
			p=line_;
			line_=(char**)realloc(line_,bsize_+glines*sizeof(char*));
			if(!line_){line_=p;return -1;/* no change */}
			lsize_+=glines;
			return 0;
		}

		int MiniConsole::grow_buffer(int gbuffer)
		{
			/*
			 * grow of a specified amount of lines the buffer_ array
			 *
			 * see the doc for grow() to get more
			 * */
			/* TEST ME AND FINISH ME */
			if(gbuffer< 0)return -1;
			if(gbuffer==0)return  0;
			char *p;int i,d;
			p=buffer_;
			buffer_=(char*)realloc(buffer_,(bsize_+gbuffer)*sizeof(char));
			if(!buffer_){buffer_=p;return -1;/* no change */}
			if((d=(p-buffer_))!=0)// in the case a shift is needed
			{
				for(i=0;i<cline_;++i)line_[i]-=d;
				bp_-=d;
			}
			bsize_+=gbuffer;
			return 0;
		}

		int MiniConsole::grow()
		{
			/*
			 * We grow a specified amount both the line_ count and the line_ buffer_.
			 * */
			return grow(FIM_CONSOLE_BLOCKSIZE,8*FIM_CONSOLE_BLOCKSIZE);
		}

		int MiniConsole::grow(int glines, int gbuffer)
		{
			/*
			 * grow of a specified amount of lines or bytes the 
			 * current line_ and text buffers; i.e.: make room
			 * to support glines more lines and gbuffer more characters.
			 *
			 * grow values can be negative. in this case, the current 
			 * buffers will be shrunk of the specified amounts.
			 *
			 * consistency will be preserved by deleting a certain amount
			 * of lines: the older ones.
			 *
			 * a zero amount for a value imply the line_ or buffer_ arrays
			 * to remain untouched.
			 * */
			/* FINISH ME AND TEST ME */
			int gb,gl;
			gb=grow_buffer(gbuffer);
			gl=grow_lines (glines);
			return !( gb==0 && 0==gl );// 0 if both 0
		}

		int MiniConsole::reformat(int newlwidth)
		{
			/*
			 * This method reformats the whole buffer_ array; that is, it recomputes
			 * line_ information for it, thus updating the whole line_ array contents.
			 * It may fail, in the case a new line_ width (smaller) is desired, because
			 * more line_ information would be needed.
			 *
			 * If the new lines are longer than before, then it could not fail.
			 * Upon a successful execution, the width is updated.
			 * 
			 * */
			int nls;
			if(newlwidth==lwidth_)return 0;//are we sure?
			if(newlwidth< lwidth_)
			{
				// realloc needed
				if ( ( nls=lines_count(buffer_, newlwidth) + 1 ) < lsize_ )
				if ( grow_lines( nls )!=0 )return -1;
			}
			if(newlwidth> lwidth_ || ( lines_count(buffer_, newlwidth) + 1 < lsize_ ) )
			{
				// no realloc, no risk
				fim::string buf=buffer_;
				if(((int)buf.size())==((int)(bp_-buffer_)))
				{
					ccol_=0;cline_=0;lwidth_=newlwidth;*line_=buffer_;bp_=buffer_;
					// the easy way
					add(buf.c_str());// by adding a very big chunk of text, we make sure it gets formatted.
					return 0;
				}
				// if some error happened in buf string initialization, we return -1
				return -1;
			}
			return -1;
		}

		int MiniConsole::dump()
		{
			/*
			 * We dump on screen the textual console contents.
			 * We care about user set variables.
			 * */
			fim_int co=getGlobalIntVariable(FIM_VID_CONSOLE_LINE_OFFSET);
			fim_int lw=getGlobalIntVariable(FIM_VID_CONSOLE_LINE_WIDTH );
			fim_int ls=getGlobalIntVariable(FIM_VID_CONSOLE_ROWS       );
			setGlobalVariable(FIM_VID_CONSOLE_BUFFER_TOTAL,bsize_);
			setGlobalVariable(FIM_VID_CONSOLE_BUFFER_FREE,(int)bsize_-(int)(bp_-buffer_));
			setGlobalVariable(FIM_VID_CONSOLE_BUFFER_USED,(int)(bp_-buffer_));
			setGlobalVariable(FIM_VID_CONSOLE_BUFFER_LINES,cline_);
			// we eventually update internal variables now
			setRows(ls);
			if( lw > 0 && lw!=lwidth_ ) reformat(lw);
			if(co>=0)
			{
				scroll_=scroll_%(rows_+1);
				if(scroll_>0)
					return do_dump((cline_-rows_+1-co)>=0?(cline_-(rows_-scroll_)+1-co):0,cline_-co);
				else
					return do_dump((cline_-rows_+1-co)>=0?(cline_-rows_+1-co):0,cline_-co);
			}
			else
				return do_dump(-co-1,cline_);
			return -1;
		}

		int MiniConsole::do_dump()const
		{
			/*
			 * We dump on screen the textual console contents.
			 * */
			return do_dump((cline_-rows_+1)>=0?(cline_-rows_+1):0,cline_);
		}

		int MiniConsole::clear()
		{
			scroll_=rows_;
			return 0;
		}

		int MiniConsole::scroll_down()
		{
			scroll_=scroll_<1?0:--scroll_;
			return 0;
		}

		int MiniConsole::scroll_up()
		{
			scroll_=scroll_<rows_?++scroll_:scroll_;
			return 0;
		}
}
#endif
