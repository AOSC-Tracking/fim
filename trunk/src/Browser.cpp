/* $LastChangedDate$ */
/*
 Browser.cpp : Fim image browser

 (c) 2007-2017 Michele Martone

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
#endif /* HAVE_LIBGEN_H */

#define FIM_READ_BLK_DEVICES 1

#define FIM_BROWSER_INSPECT 0
#if FIM_BROWSER_INSPECT
#define FIM_PR(X) printf("BROWSER:%c:%20s: f:%d/%d p:%d/%d %s\n",X,__func__,getGlobalIntVariable(FIM_VID_FILEINDEX),getGlobalIntVariable(FIM_VID_FILELISTLEN),getGlobalIntVariable(FIM_VID_PAGE),/*(image()?image()->getIntVariable(FIM_VID_PAGES):-1)*/-1,current().c_str());
#else /* FIM_BROWSER_INSPECT */
#define FIM_PR(X) 
#endif /* FIM_BROWSER_INSPECT */

#if FIM_USE_CXX11
#include <utility>	/* std::swap */
#else /* FIM_USE_CXX11 */
#include <algorithm>	/* std::swap */
#endif /* FIM_USE_CXX11 */

#if FIM_WANT_FLIST_STAT 
#include <sstream>	// std::istringstream 
#endif /* FIM_WANT_FLIST_STAT */

#if 0
#if HAVE_WORDEXP_H
#include <wordexp.h>
#else /* HAVE_WORDEXP_H */
#endif /* HAVE_WORDEXP_H */

#if HAVE_GLOB_H
#include <glob.h>
#else /* HAVE_GLOB_H */
#endif /* HAVE_GLOB_H */

#if HAVE_FNMATCH_H
#include <fnmatch.h>
#else /* HAVE_FNMATCH_H */
#endif /* HAVE_FNMATCH_H */
#endif /* 0 */

#if HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */
#if HAVE_TIME_H && ( _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED )
#define FIM_USE_GETDATE
#endif

#ifdef FIM_USE_GETDATE
/* TODO: shall use it like:
	struct tm*tmp;
	tmp=getdate("2009/12/28");
	if(tmp)
		min_mtime_str=*tmp;
*/
#endif /* FIM_USE_GETDATE */

#define FIM_CNS_ENOUGH_FILES_TO_WARN 1000
#define FIM_WITHIN(MIN,VAL,MAX) ((MIN)<=(VAL) && (VAL)<=(MAX))
#define FIM_CNS_Ki 1000
#define FIM_CNS_MANY_DUMPED_TOKENS 1000000
#define FIM_CNS_A_FEW_DUMPED_TOKENS 20

namespace fim
{
	int Browser::current_n(void)const
	{
	       	return flist_.cf();
	}

	fim_cxr Browser::fcmd_list(const args_t& args)
	{
		fim::string result = FIM_CNS_EMPTY_RESULT;

		FIM_PR('*');
		if(args.size()<1)
		{
			/* returns a string with the info about the files in list (const op) */
			fim::string fileslist;

			for(size_t i=0;i<flist_.size();++i)
				fileslist += fim::string(flist_[i]) + fim::string(" ");
			result = fileslist;
			goto ret;
		}
		else
		{
			/* read-only commands, that do not depend on FIM_VID_LOADING_IN_BACKGROUND */
			if(args[0]=="filesnum")
			{
				result = n_files();
			}
#if FIM_WANT_FILENAME_MARK_AND_DUMP
			else if(args[0]=="mark" || args[0]=="unmark")
			{
				bool domark = (args[0]=="mark");
#if FIM_WANT_PIC_LBFL
				if(args.size() > 1)
					result = do_filter_cmd(args_t(args.begin()+1,args.end()),true,domark ? Mark : Unmark);
				else
#endif /* FIM_WANT_PIC_LBFL */
			       		cc.markCurrentFile(domark); 
				goto ret;
		       	} 
			else if(args[0]=="marked")
			{
                                std::string mfl = cc.marked_files_list();
		                if( mfl != FIM_CNS_EMPTY_STRING )
                                {
                                        result += "The following files have been marked by the user :\n";
                                        result += mfl;
                                }
                                else
                                        result += "No files have been marked by the user.\n";
				goto ret;
		       	} 
			else if(args[0]=="dumpmarked")
			{
				std::cout << cc.marked_files_list();
				goto ret;
		    } 
			else if(args[0]=="unmarkall")
			{
				cc.marked_files_clear();
				goto ret;
		    } 
#else /* FIM_WANT_FILENAME_MARK_AND_DUMP */
			else if(args[0]=="mark")
				result = FIM_EMSG_NOMARKUNMARK;
			else if(args[0]=="marked")
				result = FIM_EMSG_NOMARKUNMARK;
			else if(args[0]=="unmark")
				result = FIM_EMSG_NOMARKUNMARK;
			else if(args[0]=="dumpmarked")
				result = FIM_EMSG_NOMARKUNMARK;
			else if(args[0]=="unmarkall")
				result = FIM_EMSG_NOMARKUNMARK;
#endif /* FIM_WANT_FILENAME_MARK_AND_DUMP */
#if FIM_WANT_BACKGROUND_LOAD
			else
			if(cc.getIntVariable(FIM_VID_LOADING_IN_BACKGROUND)!=0)
			{
				result = "File list temporarily locked; check out the " FIM_VID_LOADING_IN_BACKGROUND " variable later.";
				goto ret;
			}
#endif /* FIM_WANT_BACKGROUND_LOAD */
			/* commands, that do depend on FIM_VID_LOADING_IN_BACKGROUND */
			if(args[0]=="clear")
				result = _clear_list();
			else if(args[0]=="random_shuffle")
				result = _random_shuffle();
			else if(args[0]=="sort")
				result = _sort();
			else if(args[0]=="sort_basename")
				result = _sort(FIM_SYM_SORT_BN);
#if FIM_WANT_SORT_BY_STAT_INFO
			else if(args[0]=="sort_mtime")
				result = _sort(FIM_SYM_SORT_MD);
			else if(args[0]=="sort_fsize")
				result = _sort(FIM_SYM_SORT_SZ);
#endif /* FIM_WANT_SORT_BY_STAT_INFO */
			else if(args[0]=="reverse")
				result = _reverse();
			else if(args[0]=="swap")
				result = _swap();
			else if(args[0]=="pop")
				/* deletes the last image from the files list.  someday may add filename matching based remove..  */
				pop(),
				result = this->n_files();
			else if(args[0]=="remove")
				result = do_filter(args_t(args.begin()+1,args.end())/*,FullFileNameMatch,false,Delete*/);
			else if(args[0]=="push")
				result = do_push(args_t(args.begin()+1,args.end()));
#ifdef FIM_READ_DIRS
			else if(args[0]=="pushdir")
			{
				if(args.size()>=2)
					push_dir(args[1]);
				else
					push_dir(".");
				result = FIM_CNS_EMPTY_RESULT;
			}
			else if(args[0]=="pushdirr")
			{
#ifdef FIM_RECURSIVE_DIRS
				if(args.size()>=2)
					push_dir(args[1],true);
				else
					push_dir(".",true);
				result = FIM_CNS_EMPTY_RESULT;
#else /* FIM_RECURSIVE_DIRS */
				result = "Please recompile with +FIM_RECURSIVE_DIRS to activate pushdirr.";
#endif /* FIM_RECURSIVE_DIRS */
			}
#endif /* FIM_READ_DIRS */
			else result = FIM_CMD_HELP_LIST;
		}
ret:
		FIM_PR('.');
		return result;
	}

	std::ostream& Browser::print(std::ostream& os)const
	{
#if FIM_USE_CXX11
		for(auto le : flist_)
			os << le << FIM_CNS_NEWLINE;
#else /* FIM_USE_CXX11 */
		for(size_t i=0; i<flist_.size(); ++i)
			os << flist_[i] << FIM_CNS_NEWLINE;
#endif /* FIM_USE_CXX11 */
		return os;
	}

	fim_cxr Browser::fcmd_redisplay(const args_t& args)
	{
		/* ...shall merge with fcmd_display() */
		redisplay();
		return FIM_CNS_EMPTY_RESULT;
	}

	void Browser::redisplay(void)
	{
		/*
		 * Given the current() file, display it again like the first time.
		 * This behaviour is different from reloading.
		 */
		fim::string c=current();
		FIM_PR('*');

		if(c_image())
		{
			FIM_AUTOCMD_EXEC(FIM_ACM_PREREDISPLAY,c);
			if(c_image())
			{
				/*
				 * FIXME : this is conceptually wrong.
				 * should be:
				 * viewport().redisplay();
				 */
				viewport()->recenter();
				if( commandConsole_.redisplay() )
					this->display_status(current().c_str());
			}
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTREDISPLAY,c);
		}
		FIM_PR('.');
	}

#ifdef FIM_READ_STDIN_IMAGE
	void Browser::set_default_image(ImagePtr stdin_image)
	{
		/*
		 * this is used mainly to set image files read from pipe or stdin
		 * */
		FIM_PR('*');
#if FIM_USE_CXX11
		if( !stdin_image || stdin_image->check_invalid() )
			goto ret;
		default_image_ = std::move(stdin_image);
#else /* FIM_USE_CXX11 */
		if( !stdin_image || stdin_image->check_invalid() )
			goto ret;
		if( default_image_ )
		       	delete default_image_;
		default_image_ = stdin_image;
#endif /* FIM_USE_CXX11 */
ret:
		FIM_PR('.');
		return;
	}
#endif /* FIM_READ_STDIN_IMAGE */

	Browser::Browser(CommandConsole& cc):
#ifdef FIM_NAMESPACES
		Namespace(&cc,FIM_SYM_NAMESPACE_BROWSER_CHAR),
#endif /* FIM_NAMESPACES */
#if FIM_WANT_PIC_LBFL
		flist_(tlist_),
#endif /* FIM_WANT_PIC_LBFL */
#if FIM_WANT_BACKGROUND_LOAD
		pcache_(cache_),
#endif /* FIM_WANT_BACKGROUND_LOAD */
		nofile_(FIM_CNS_EMPTY_STRING),commandConsole_(cc)
	{	
	}

	const fim::string Browser::pop_current(void)
	{
		/*
		 * pops the current image filename from the filenames list
		 * ( note that it doesn't refresh the image in any way ! )
		 *
		 * WARNING : SAME AS ERASE !
		 */
		flist_.pop_current();
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
		return nofile_;
	}

	const fim::string Browser::pop(fim::string filename)
	{	
		/*
		 * pops the last image filename from the filenames list
		 * ( note that it doesn't refresh the image in any way ! )
		 */
		fim::string s = nofile_;

		if( flist_.size() <= 1 )
			goto ret;

		s = flist_.pop(filename);

		setGlobalVariable(FIM_VID_FILEINDEX,current_image());
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
ret:
		return s;
	}

	fim::string Browser::pan(const args_t& args)
	{
		FIM_PR('*');

		if( args.size() < 1 || (!args[0].c_str()) )
			goto nop;

		if( c_image() )
		{
			fim::string c = current();
			fim_char_t f = tolower(*args[0].c_str());
			if( f )
			{
				FIM_AUTOCMD_EXEC(FIM_ACM_PREPAN,c);
				if(c_image() && viewport())
					viewport()->pan(args);
				FIM_AUTOCMD_EXEC(FIM_ACM_POSTPAN,c);
			}
		}
		else
			prev();
nop:
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_cxr Browser::fcmd_scale(const args_t& args)
	{
		/*
		 * scales the image to a certain scale factor
		 * FIXME: no user error checking -- poor error reporting for the user
		 * TODO: wxh / w:h syntax needed
		 * FIXME: this shall belong to viewport
		 */
		fim_scale_t newscale = FIM_CNS_SCALEFACTOR_ZERO;
		fim_char_t fc = FIM_SYM_CHAR_NUL;
		const fim_char_t*ss = FIM_NULL;
		int sl = 0;
		bool pcsc = false;
		bool aes = false; // approximate exponential scaling
		FIM_PR('*');

		if( args.size() < 1 || !(ss=args[0].c_str() ))
			goto nop;
		fc = tolower(*ss);
		sl = strlen(ss);

#if FIM_WANT_APPROXIMATE_EXPONENTIAL_SCALING
		{
			if( fc == '<' )
			{
				newscale = 0.5;
				fc='+';
				aes = true;
				goto comeon;
			}
			if( fc == '>' )
			{
				newscale = 2;
				fc='+';
				aes = true;
				goto comeon;
			}
		}
#endif /* FIM_WANT_APPROXIMATE_EXPONENTIAL_SCALING */

		if( isalpha(fc) )
		{
			if( !( fc == 'w' || fc == 'h' || fc == 'a' || fc == 'b' ) )
				goto nop;
		}
		else
		{
			if( sl == 1 && ( fc =='+' || fc == '-' ) )
			{
				if( fc == '+' )
					newscale=(fim_scale_t)getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR);
				if( fc == '-' )
					newscale=(fim_scale_t)getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR);
				goto comeon;
			}
			if( sl >= 2 && ( fc == '+' || fc == '-' ) )
			{
				fim_char_t sc =ss[1];

				if( fc == '+' )
				{
					fim_scale_t vmf = getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR);
					fim_scale_t vrf = getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR);
					fim_scale_t sfd = getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_DELTA);
					fim_scale_t sfm = getGlobalFloatVariable(FIM_VID_SCALE_FACTOR_MULTIPLIER);
					if( sfd <= FIM_CNS_SCALEFACTOR_ZERO )
						sfd = FIM_CNS_SCALEFACTOR_DELTA;
					if( sfm <= FIM_CNS_SCALEFACTOR_ONE )
					       	sfm = FIM_CNS_SCALEFACTOR_MULTIPLIER;

					switch(sc)
					{
						case('+'):
						{
							vrf += sfd;
							vmf += sfd;
						}
						break;
						case('-'):
						{
							vrf -= sfd;
							vmf -= sfd;
						}
						break;
						case('*'):
						{
							vrf *= sfm;
							vmf *= sfm;
						}
						break;
						case('/'):
						{
							vrf /= sfm;
							vmf /= sfm;
						}
						break;
						default:
						goto noplus;
					}

					setGlobalVariable(FIM_VID_REDUCE_FACTOR, vrf);
					setGlobalVariable(FIM_VID_MAGNIFY_FACTOR,vmf);
					goto nop;
				}
noplus:
				if( fc == '+' || fc == '-')
				{
					newscale = fim_atof(ss+1);
					pcsc = (strstr(ss,"%") != FIM_NULL );
					if(pcsc)
						newscale *= .01;
					if( !newscale )
						goto nop;
#if 1
					if( fc == '+' )
						newscale = 1.0 + newscale;
					if( fc == '-' )
						newscale = 1.0 - newscale;
					fc = FIM_SYM_CHAR_NUL;
#endif
					goto comeon;
				}
				goto nop;
			}
			if( sl )
			{
				if(fc=='*')
				{
					++ss;
					if(!*ss)
						goto nop; /* a '*' alone. may assign a special meaning to this... */
				}
				newscale = fim_atof(ss);
				if(fc=='*')
				{
					fc = '+';
					goto comeon;
				}
				pcsc = (strstr(ss,"%") != FIM_NULL );
				if(pcsc)
					newscale *= .01;
				if( newscale == FIM_CNS_SCALEFACTOR_ZERO )
				       	goto nop;
				pcsc = false;
				goto comeon;
			}
			goto nop;
		}
comeon:
#if FIM_WANT_BDI
		if(1)
#else	/* FIM_WANT_BDI */
		if(c_image())
#endif	/* FIM_WANT_BDI */
		{
			fim_fn_t c = current();
			FIM_AUTOCMD_EXEC(FIM_ACM_PRESCALE,c);
			if(image())
				image()->update();/* rotation update */ /* FIXME: shall separate scaling from orientation */
			if( c_image() )
			switch( fc )
			{
				case('w'):
				if(viewport())
					viewport()->auto_width_scale();
				break;
				case('h'):
				if(viewport())
				viewport()->auto_height_scale();
				break;
				case('a'):
				if(viewport())
					viewport()->auto_scale();
				break;
				case('b'):
				if(viewport())
					viewport()->auto_scale_if_bigger();
				break;
				case('-'):
				{
					if( newscale )
					{
						if(image())
							image()->reduce(newscale);
						if(viewport())
							viewport()->scale_position_reduce(newscale);
					}
					else	
					{
						if(image())
							image()->reduce();
						if(viewport())
							viewport()->scale_position_reduce();
					}
				}
				break;
				case('+'):
				{
					if( newscale )
					{
						if(image())
							image()->magnify(newscale,aes);
						if(viewport())
							viewport()->scale_position_magnify(newscale);
					}
					else	
					{
						if(image())
							image()->magnify();
						if(viewport())
							viewport()->scale_position_magnify();
					}
				}
				break;
				default:
				if( pcsc )
					image()->scale_multiply(newscale);
				else
					image()->setscale(newscale);
			}
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTSCALE,c);
		}
nop:
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}
	
	fim_cxr Browser::fcmd_color(const args_t& args)
	{
		/*
		 */
		if( !image() )
			goto nop;

		if(args.size()>0)
		{
			bool daltonize=false;
			enum fim_cvd_t cvd=FIM_CVD_NO;

			if( args[0] == "desaturate" )
				if( image()->desaturate() )
					goto nop;
			if( args[0] == "negate" )
				if( image()->negate() )
					goto nop;
			if( args[0] == "identity" )
				if( image()->identity() )
					goto nop;

			if( args.size()>1 && ( args[1] == "daltonize" || args[1] == "D" ) )
				daltonize=true;
			if( args[0] == "c" || args[0] == "colorblind"  ||
			    args[0] == "d" || args[0] == "deuteranopia" )
				cvd = FIM_CVD_DEUTERANOPIA;
			if( args[0] == "p" || args[0] == "protanopia" )
				cvd = FIM_CVD_PROTANOPIA;
			if( args[0] == "t" || args[0] == "tritanopia" )
				cvd = FIM_CVD_TRITANOPIA  ;

			if(cvd!=FIM_CVD_NO)
				if( image()->colorblind(cvd,daltonize) )
					goto nop;
			/* TODO: help printout here ? */
		}
nop:
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::do_filter_cmd(const args_t args, bool negative, enum FilterAction faction)
	{
		fim::string result = FIM_CNS_EMPTY_RESULT;

		/* Interprets a mark/limiting pattern. */
		if(args.size()>0)
		{
#if FIM_WANT_LIMIT_DUPBN
			if( args[0] == "~=" )
			{
				// result = result + "Limiting to duplicate files\n";
				result = do_filter(args,DupFileNameMatch,negative,faction);
			}
			else
			if( args[0] == "~^" )
			{
				// result = result + "Limiting to first occurrences of files\n";
				result = do_filter(args,FirstFileNameMatch,negative,faction);
			}
			else
			if( args[0] == "~$" )
			{
				// result = result + "Limiting to last occurrences of files\n";
				result = do_filter(args,LastFileNameMatch,negative,faction);
			}
			else
			if( args[0] == "~!" )
			{
				// result = result + "Limiting to unique files\n";
				result = do_filter(args,UniqFileNameMatch,negative,faction);
			}
			else
			if( args[0] == "~i" && args.size()>1 )
			{
				// result = result + "Limiting according to list interval\n";
				result = do_filter(args,ListIdxMatch,negative,faction);
			}
			else
#if FIM_WANT_FLIST_STAT 
			if( args[0] == "~d" )
			{
				// result = result + "Limiting according to time interval\n";
				result = do_filter(args,TimeMatch,negative,faction);
			}
			else
			if( args[0] == "~z" )
			{
				// result = result + "Limiting according to file size\n";
				result = do_filter(args,SizeMatch,negative,faction);
			}
			else
#else /* FIM_WANT_FLIST_STAT */
			/* FIXME: need info here */
#endif /* FIM_WANT_FLIST_STAT */
#endif /* FIM_WANT_LIMIT_DUPBN */
#if FIM_WANT_FILENAME_MARK_AND_DUMP
			if( args[0] == "!" )
			{
				// result = result + "Limiting to marked files\n";
				result = do_filter(args,MarkedMatch,!negative,faction); /* this is remove on filenames; need remove on comments */
			}
			else
#endif /* FIM_WANT_FILENAME_MARK_AND_DUMP */
				result = do_filter(args,VarMatch,!negative,faction); /* this is remove on filenames; need remove on comments */
		}
		return result;
	}

#if FIM_WANT_PIC_LBFL
	fim_cxr Browser::fcmd_limit(const args_t& args)
	{
		fim::string result = FIM_CNS_EMPTY_RESULT;
		int aoc = fim_args_opt_count(args,'-');
		size_t max_vars = FIM_CNS_A_FEW_DUMPED_TOKENS, max_vals = FIM_CNS_A_FEW_DUMPED_TOKENS;

		if( args.size()-aoc > 0 )
		{
			fim_int lbl = n_files(); // length before limiting
			enum FilterAction faction = Delete;
			flist_t clist; // current list copy

#if FIM_WANT_PIC_LVDN
			if( args.size()-aoc == 1 && aoc == 1 && 
				( fim_args_opt_have(args,"-list") || fim_args_opt_have(args,"-listall" ) ) )
			{
				if( fim_args_opt_have(args,"-listall"))
					max_vars=FIM_CNS_MANY_DUMPED_TOKENS;
				result = cc.id_.get_values_list_for(args[1],max_vars) + string("\n");
				goto nop;
			}
#endif /* FIM_WANT_PIC_LVDN */

			if(!limited_)
				limited_ = true,
				tlist_ = flist_; // a copy

			if(                  fim_args_opt_have(args,"-merge"))
			       	clist = flist_; // current list
			else
			if(                  fim_args_opt_have(args,"-subtract"))
			       	clist = flist_; // current list

			if(tlist_.size() && !fim_args_opt_have(args,"-further"))
			       	flist_ = tlist_; // limit on total list

			result = do_filter_cmd(args_t(args.begin()+aoc,args.end()),false,faction);

			if(clist.size())
			{
				clist._sort(FIM_SYM_SORT_FN);
				if(fim_args_opt_have(args,"-merge"))
					flist_._set_union(clist);
				else
				if(fim_args_opt_have(args,"-subtract"))
					flist_._set_difference_from(clist);
			}

			if(n_files() != lbl)
				reload();
		}
		else
		{
#if FIM_WANT_PIC_LVDN
			if( args.size()-aoc == 0 && aoc == 1 && 
				( fim_args_opt_have(args,"-list") || fim_args_opt_have(args,"-listall" ) ) )
			{
				if( fim_args_opt_have(args,"-listall"))
					max_vals=FIM_CNS_MANY_DUMPED_TOKENS;
				result = cc.id_.get_variables_id_list(max_vals) + string("\n");
				goto nop;
			}
#endif /* FIM_WANT_PIC_LVDN */

			if(limited_)
			{
				// result = "Restoring the original browsable files list.";
			       	commandConsole_.set_status_bar("restoring files list", "*");
				flist_ = tlist_; // original list
				limited_ = false;
			}
		}
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files()); /* TODO: need a specific 'refresh' function */
nop:
		return result;
	}
#endif /* FIM_WANT_PIC_LBFL */

	fim::string Browser::display_status(const fim_char_t *l)
	{
		fim_bool_t wcs = cc.isSetVar(FIM_VID_WANT_CAPTION_STATUS);
		FIM_PR('*');

		if( getGlobalIntVariable(FIM_VID_DISPLAY_STATUS) == 1 )
		{
			fim::string dss ;
			const fim_char_t *dssp=FIM_NULL;

			if( cc.isSetVar(FIM_VID_DISPLAY_STATUS_FMT) )
			{
				dss = c_image()->getInfoCustom(cc.getStringVariable(FIM_VID_DISPLAY_STATUS_FMT).c_str());
			}
			dssp=dss.c_str();
			commandConsole_.set_status_bar( (dssp && *dssp ) ? dssp : l, image()?(image()->getInfo().c_str()):"*");
		}
		else
		{
			if(wcs)
				wcs = cc.set_wm_caption(FIM_NULL);
		}
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_cxr Browser::fcmd_display(const args_t& args)
	{
		/*
		 * displays the current image, (if already loaded), on screen
		 */
		fim::string c = current();
		FIM_PR('*');

		if( c_image() )
		{
			FIM_AUTOCMD_EXEC(FIM_ACM_PREDISPLAY,c);
			/* FIXME: need a "help" request answer. */
			/*
			 * the following is a trick to override redisplaying..
			 */
			if( args.size()>0 && args[0] == "reinit" )
			{
				string arg = args.size()>1?args[1]:"";
				commandConsole_.display_reinit(arg.c_str());
			}
			if( args.size()>0 && args[0] == "resize" )
			{
				fim_coo_t nww = c_image()->width();
				fim_coo_t nwh = c_image()->height();
				fim_bool_t wsl = (getGlobalIntVariable(FIM_VID_DISPLAY_BUSY)) ?  true : false;;

#if 0
				if( args.size() == 2 && args[1] == "original" )
					nww = c_image()->original_width(),
					nwh = c_image()->original_height() + fh;
#endif
				if( args.size()>2 )
					nww = args[1],
					nwh = args[2],
					wsl = false;
				commandConsole_.resize(nww,nwh,wsl);
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
					this->display_status(current().c_str());
//				FIXME:
//				if(commandConsole_.window)commandConsole_.window->recursive_display();
			}
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTDISPLAY,c);
		}
		else
		{
		       	cout << "no image to display, sorry!";
			commandConsole_.set_status_bar("no image loaded.", "*");
		}
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

#if FIM_WANT_FAT_BROWSER
	fim_cxr Browser::fcmd_no_image(const args_t& args)
	{
		/* sets no image as the current one */
		FIM_PR('*');
		free_current_image();
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}
#endif /* FIM_WANT_FAT_BROWSER */

	int Browser::load_error_handle(fim::string c)
	{
		/*
		 * assume there was a load attempt : check and take some action in case of error
		 *
		 * FIXME : this behaviour is BUGGY, because recursion will be killed off 
		 *         by the autocommand loop prevention mechanism. (this is not true, as 20090215)
		 * */
		static int lehsof = 0;	/* './fim FILE NONFILE' and hitting 'prev' will make this necessary  */
		int retval = 0;
		FIM_PR('*');

		if( lehsof )
			goto ret; /* this prevents infinite recursion */
		if( /*image() &&*/ viewport() && ! (viewport()->check_valid()) )
		{
			free_current_image();
			++ lehsof;
#ifdef FIM_REMOVE_FAILED
				//pop(c);	//removes the currently specified file from the list. (pop doesn't work in this way)
#if FIM_USE_CXX11
				do_filter({c});
#else /* FIM_USE_CXX11 */
				args_t args;
				args.push_back(c.c_str());
				do_filter(args);
#endif /* FIM_USE_CXX11 */
#ifdef FIM_AUTOSKIP_FAILED
				if(n_files())
				{
					//next(1);
					reload(); /* this is effective, at least partially */
				}
#endif /* FIM_AUTOSKIP_FAILED */
#endif /* FIM_REMOVE_FAILED */
			--lehsof;
			retval = 1;
		}
ret:
		FIM_PR('.');
		return retval;
	}

	fim::string Browser::reload(void)
	{
		/*
		 * FIXME
		 *
		 * reload the current filename
		 * */
		if( n_files() )
#if FIM_USE_CXX11
			return fcmd_reload({});
#else /* FIM_USE_CXX11 */
			return fcmd_reload( args_t() );
#endif /* FIM_USE_CXX11 */
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_err_t Browser::loadCurrentImage(void)
	{
		/*
		 * FIXME
		 *
		 * an attempt to load the current image
		 * */
		fim_err_t errval = FIM_ERR_NO_ERROR;

		FIM_PR('*');
		try
		{
#ifndef FIM_BUGGED_CACHE
	#ifdef FIM_CACHE_DEBUG
		if( viewport() ) std::cout << "browser::loadCurrentImage(\"" << current().c_str() << "\")\n";
	#endif /* FIM_CACHE_DEBUG */
		if( viewport()
			&& !( current()!=FIM_STDIN_IMAGE_NAME && !is_file(current()) ) /* FIXME: this is an unelegant fix to prevent crashes on non-existent files. One shall better fix this by a good exception mechanism for Image::Image() and a clean approach w.r.t. e.g. free_current_image() */
		)
		{
			ViewportState viewportState;
			FIM_PR('0');
			viewport()->setImage(
#if FIM_WANT_BACKGROUND_LOAD
					pcache_.
#else /* FIM_WANT_BACKGROUND_LOAD */
					 cache_.
#endif /* FIM_WANT_BACKGROUND_LOAD */
					useCachedImage(cache_key_t(current(),(current()==FIM_STDIN_IMAGE_NAME)?FIM_E_STDIN:FIM_E_FILE),&viewportState,getGlobalIntVariable(FIM_VID_PAGE)) );// FIXME
			viewport()->setState(viewportState);
		}
#else /* FIM_BUGGED_CACHE */
		// warning : in this cases exception handling is missing
	#ifdef FIM_READ_STDIN_IMAGE
		if( current() != FIM_STDIN_IMAGE_NAME )
		{
			FIM_PR('1');
			if(viewport())
				viewport()->setImage( new Image(current().c_str()) );
		}
		else
		{
			FIM_PR('2');
			if( viewport() && default_image_ )
			{
				// a one time only image (new, experimental)
				viewport()->setImage(default_image_->getClone());
				//default_image_=FIM_NULL;
			}
		}
	#else
		FIM_PR('3');
		if( viewport() )
			viewport()->setImage( new Image(current().c_str()) );
	#endif /* FIM_READ_STDIN_IMAGE */
#endif /* FIM_BUGGED_CACHE */
		}
		catch(FimException e)
		{
			FIM_PR('E');
			if(viewport())
				viewport()->setImage( FIM_NULL );
//		commented temporarily for safety reasons
//			if( e != FIM_E_NO_IMAGE )throw FIM_E_TRAGIC;  /* hope this never occurs :P */
		}
		FIM_PR('.');
		return errval;
	}

	void Browser::free_current_image(void)
	{
		/*
		 * FIXME
		 * only cleans up the internal data structures
		 * */
		FIM_PR('*');
		if( viewport() )
			viewport()->free();
		setGlobalVariable(FIM_VID_CACHE_STATUS,cache_.getReport().c_str());
		FIM_PR('.');
	}

	fim_cxr Browser::fcmd_prefetch(const args_t& args)
	{
#ifdef FIM_BUGGED_CACHE
		return " prefetching disabled";
#endif /* FIM_BUGGED_CACHE */
		fim_int wp=1;
		FIM_PR('*');

			FIM_AUTOCMD_EXEC(FIM_ACM_PREPREFETCH,current());
		if( args.size() > 0 )
			goto ret;

		wp = getGlobalIntVariable(FIM_VID_WANT_PREFETCH);
		setGlobalVariable(FIM_VID_WANT_PREFETCH,(fim_int)0);
#if FIM_WANT_BACKGROUND_LOAD
		if(wp == 2)
		{
			pcache_.asyncPrefetch(get_next_filename(1));
			pcache_.asyncPrefetch(get_next_filename(-1));
			goto apf;
		}
#endif /* FIM_WANT_BACKGROUND_LOAD */
		if(cache_.prefetch(cache_key_t(get_next_filename( 1).c_str(),FIM_E_FILE)))// we prefetch 1 file forward
#ifdef FIM_AUTOSKIP_FAILED
			pop(get_next_filename( 1));/* if the filename doesn't match a loadable image, we remove it */
#else /* FIM_AUTOSKIP_FAILED */
			{}	/* beware that this could be dangerous and trigger loops */
#endif /* FIM_AUTOSKIP_FAILED */
		if(cache_.prefetch(cache_key_t(get_next_filename(-1).c_str(),FIM_E_FILE)))// we prefetch 1 file backward
#ifdef FIM_AUTOSKIP_FAILED
			pop(get_next_filename(-1));/* if the filename doesn't match a loadable image, we remove it */
#else /* FIM_AUTOSKIP_FAILED */
			{}	/* beware that this could be dangerous and trigger loops */
#endif /* FIM_AUTOSKIP_FAILED */
#if FIM_WANT_BACKGROUND_LOAD
apf:		/* after prefetch */
#endif /* FIM_WANT_BACKGROUND_LOAD */
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTPREFETCH,current());
		setGlobalVariable(FIM_VID_WANT_PREFETCH,(fim_int)wp);
ret:
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_cxr Browser::fcmd_reload(const args_t& args)
	{
		/*
		 * deletes the structures associated to the present image
		 * and then
		 * tries to load a new one from the current filename
		 */
		fim::string c = current();
		fim::string result;

		FIM_PR('*');
		//for(size_t i=0;i<args.size();++i) push(args[i]);
		if( empty_file_list() )
		{ result = "sorry, no image to reload\n"; goto ret; }
		FIM_AUTOCMD_EXEC(FIM_ACM_PRERELOAD,c);
#if FIM_HORRIBLE_CACHE_INVALIDATING_HACK
		if( args.size() > 0 )
		{
			fim_int mci = getGlobalIntVariable(FIM_VID_MAX_CACHED_IMAGES);
			setGlobalVariable(FIM_VID_MAX_CACHED_IMAGES,(fim_int)0);
			free_current_image();
			setGlobalVariable(FIM_VID_MAX_CACHED_IMAGES,mci);
		}
#else /* FIM_HORRIBLE_CACHE_INVALIDATING_HACK */
		free_current_image();
#endif /* FIM_HORRIBLE_CACHE_INVALIDATING_HACK */
		loadCurrentImage();
		//if(image())image()->reload();

//		while( n_files() && viewport() && ! (viewport()->check_valid() ) && load_error_handle(c) );
		load_error_handle(c);
		FIM_AUTOCMD_EXEC(FIM_ACM_POSTRELOAD,c);
		result = FIM_CNS_EMPTY_RESULT;
ret:
		FIM_PR('.');
		return result;
	}

	fim_cxr Browser::fcmd_load(const args_t& args)
	{
		/*
		 * loads the current file, if not already loaded
		 */
		fim::string c = current();
		fim::string result = FIM_CNS_EMPTY_RESULT;
		FIM_PR('*');

		//for(size_t i=0;i<args.size();++i) push(args[i]);
		if( image() && ( image()->getName() == current()) )
		{
			result = "image already loaded\n";		//warning
			goto ret;
		}
		if( empty_file_list() )
		{
			result = "sorry, no image to load\n";	//warning
			goto ret;
		}
		FIM_AUTOCMD_EXEC(FIM_ACM_PRELOAD,c);
		commandConsole_.set_status_bar("please wait while loading...", "*");

		loadCurrentImage();

		load_error_handle(c);
		FIM_AUTOCMD_EXEC(FIM_ACM_POSTLOAD,c);
ret:
		FIM_PR('.');
		return result;
	}

	fim_int Browser::find_file_index(const fim::string nf)const
	{
		/* 
		 * returns whether the file nf is in the files list
		 */
		fim_int fi = -1;
#if 1
		for(flist_t::const_iterator fit=flist_.begin();fit!=flist_.end();++fit)
			if( string(*fit) == nf )
			{
				fi = fit - flist_.begin();
				goto ret;
			}
#else
		for(fim_size_t i=0;i<flist_.size();++i)
			if( flist_[i] == nf )
			{
				fi = (fim_int)i;
				goto ret;
			}
#endif
ret:
		return fi;
	}

	bool Browser::present(const fim::string nf)const
	{
		/* 
		 * returns whether the file nf is in the files list
		 */
		fim_int i = find_file_index(nf);
		bool ip = false;

		if( i >= 0 )
			ip = true;
		return ip;
	}

#ifdef FIM_READ_DIRS
	bool Browser::push_dir(fim::string nf, fim_flags_t pf, const fim_int * show_must_go_on)
	{
		// TODO: may introduce some more variable to control recursive push 	
		DIR *dir = FIM_NULL;
		struct dirent *de = FIM_NULL;
		fim::string f;
		bool retval = false;
		FIM_PR('*');

		if( show_must_go_on && !*show_must_go_on )
			goto rret;

		if(cc.getIntVariable(FIM_VID_PRELOAD_CHECKS)!=1)
			goto nostat;
		/*	we want a dir .. */
#ifdef HAVE_LIBGEN_H
		if( !is_dir( nf.c_str() ) )
			nf = fim_dirname(nf);
#else /* HAVE_LIBGEN_H */
		if( !is_dir( nf ))
			goto ret;
#endif /* HAVE_LIBGEN_H */

nostat:
#if FIM_WANT_PROGRESS_RECURSIVE_LOADING
		if( pf & FIM_FLAG_PUSH_BACKGROUND )
		{
			static int maxsize=0;
			const char * FIM_CLEARTERM_STRING = "\x1B\x4D"; /* man terminfo */
			std::cout << FIM_CLEARTERM_STRING << "scanning: " << flist_.size()<< nf << std::endl;
			maxsize = FIM_MAX(nf.size(),maxsize);
		}
#endif

		if ( ! ( dir = opendir(nf.c_str() ) ))
			goto ret;

		f += nf;
		f += FIM_CNS_DIRSEP_STRING;
		//are we sure -1 is not paranoid ?
		while( ( de = readdir(dir) ) != FIM_NULL )
		{
			if( de->d_name[0] == '.' &&  de->d_name[1] == '.' && !de->d_name[2] )
				continue;
			if( de->d_name[0] == '.' && !de->d_name[1] )
				continue;
#if FIM_RECURSIVE_HIDDEN_DIRS_SKIP_CHECK
			/*
			 * We follow the convention of ignoring hidden files.
			 * */
			if( (!(pf & FIM_FLAG_PUSH_HIDDEN)) && de->d_name[0] == '.' )
				continue;
#endif /* FIM_RECURSIVE_HIDDEN_DIRS_SKIP_CHECK */
			
			/*
			 * Warning : this is dangerous, as following circular links may cause memory exhaustion.
			 * */
			if( is_dir( f + fim::string(de->d_name)) )
			{
#ifdef FIM_RECURSIVE_DIRS
				if( pf & FIM_FLAG_PUSH_REC )
					retval |= push_dir( f + fim::string(de->d_name), pf, show_must_go_on);
				else
#endif /* FIM_RECURSIVE_DIRS */
					continue;
			}
			else 
			{
				fim::string re = getGlobalStringVariable(FIM_VID_PUSHDIR_RE);
				fim_fn_t fn = f + fim::string( de->d_name );

				if( re == FIM_CNS_EMPTY_STRING )
					re = FIM_CNS_PUSHDIR_RE;
				if( fn.re_match(re.c_str()) )
					retval |= push( f + fim::string(de->d_name) );
				//std::cout << re << " " << f + fim::string(de->d_name) << "!\n";
			}
#if FIM_WANT_BACKGROUND_LOAD
			if( ( pf & FIM_FLAG_PUSH_ONE ) && retval )
				goto ret;
#endif /* FIM_WANT_BACKGROUND_LOAD */
		}
ret:
		//retval = ( closedir(dir) == 0 );
		closedir(dir);
rret:
		FIM_PR('.');
		return retval;
	}
#endif /* FIM_READ_DIRS */

	bool Browser::push(fim::string nf, fim_flags_t pf, const fim_int * show_must_go_on)
	{
		bool pec = false; // push error code
#if 0
		int eec = 0; // expansion error code
#if HAVE_WORDEXP_H
	{
		wordexp_t p;
		eec = wordexp(nf.c_str(),&p,0);
		if( eec == 0 )
		{
			for(size_t g=0;g<p.we_wordc;++g)
				pec |= push_noglob(p.we_wordv[g],pf,show_must_go_on);
			wordfree(&p);
			goto ret;
		}
	}
#endif /* HAVE_WORDEXP_H */

#if HAVE_GLOB_H
	{
               	glob_t pglob;
               	pglob.gl_offs = 0;
       		eec = glob(nf.c_str(),GLOB_NOSORT| GLOB_NOMAGIC | GLOB_TILDE, FIM_NULL, &pglob);
		if( eec == 0 )
		{
			for(size_t g=0;g<pglob.gl_pathc;++g)
				pec |= push_noglob(pglob.gl_pathv[g],pf,show_must_go_on);
			globfree(&pglob);
			goto ret;
		}
	}
#endif /* HAVE_GLOB_H */
#endif /* 0 */
		pec = push_noglob(nf.c_str(),pf,show_must_go_on);
// ret:
		return pec;
	}

	bool Browser::push_noglob(fim::string nf, fim_flags_t pf, const fim_int * show_must_go_on)
	{	
		/*
		 * FIX ME:
		 * are we sure we want no repetition!????
		 * */
		bool retval = false;
		bool lib = false; /* load in background */

#if FIM_WANT_BACKGROUND_LOAD
		if( pf & FIM_FLAG_PUSH_BACKGROUND )
			lib = true;
#endif /* FIM_WANT_BACKGROUND_LOAD */
		FIM_PR('*');

		if( nf == FIM_STDIN_IMAGE_NAME )
			goto isfile;

		if(cc.getIntVariable(FIM_VID_PRELOAD_CHECKS)!=1)
		{
			size_t sl = strlen(nf.c_str());

			if(sl < 1)
				goto ret;

			if( nf[sl-1] == FIM_CNS_SLASH_CHAR )
			{
				goto isdir;
			}
			else
			{
				goto isfile;
			}
		}

		{
#ifdef FIM_CHECK_FILE_EXISTENCE
			/*
			 * skip adding the filename in the list if
			 * it is not existent or it is a directory...
			 */
			struct stat stat_s;

			/*	if the file doesn't exist, return */
			if( -1 == stat(nf.c_str(),&stat_s) )
			{
#if 0
				if( errno != EOVERFLOW) /* this may happen with a readable file...  */
#endif
				{
					/* fim_perror("!"); */
					goto ret;
				}
			}
			/*	if it is a character device , return */
			//if(  S_ISCHR(stat_s.st_mode))return FIM_CNS_EMPTY_RESULT;
			/*	if it is a block device , return */
			//if(  S_ISBLK(stat_s.st_mode))return FIM_CNS_EMPTY_RESULT;
			/*	if it is a directory , return */
			//if(  S_ISDIR(stat_s.st_mode))return FIM_CNS_EMPTY_RESULT;
#ifdef FIM_READ_DIRS
			{
				fim_int ppd = getGlobalIntVariable(FIM_VID_PUSH_PUSHES_DIRS);
				if( ppd >= 1 && S_ISDIR(stat_s.st_mode))
				{
#if FIM_RECURSIVE_HIDDEN_DIRS_SKIP_CHECK
					if( ppd > 1 )
						pf |= FIM_FLAG_PUSH_HIDDEN;
#endif /* FIM_RECURSIVE_HIDDEN_DIRS_SKIP_CHECK */
					goto isdir;
				}
			}
#endif /* FIM_READ_DIRS */
			/*	we want a regular file .. */
			if(
				! S_ISREG(stat_s.st_mode) 
#ifdef FIM_READ_BLK_DEVICES
				&& ! S_ISBLK(stat_s.st_mode)  // NEW
#endif /* FIM_READ_BLK_DEVICES */
			)
			{
				/*
				 * i am not fully sure this is effective
				 * */
				if(!lib)
					nf += " is not a regular file!",
					commandConsole_.set_status_bar(nf.c_str(), "*");
				goto ret;
			}
#endif /* FIM_CHECK_FILE_EXISTENCE */
		}
isfile:
#ifdef FIM_CHECK_DUPLICATES
		if( ( ! ( pf & FIM_FLAG_PUSH_ALLOW_DUPS ) ) && present(nf) )
		{
			//there could be an option to have duplicates...
			//std::cout << "no duplicates allowed..\n";
			goto ret;
		}
#endif /* FIM_CHECK_DUPLICATES */
		flist_.push_back(nf);
		//std::cout << "pushing " << nf << FIM_CNS_NEWLINE;
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
		retval = true;
		goto ret;
#ifdef FIM_READ_DIRS
isdir:
		retval = push_dir(nf,pf,show_must_go_on);
#endif /* FIM_READ_DIRS */
ret:
		FIM_PR('.');
		return retval;
	}
	
	fim_int Browser::n_files(void)const
	{
		/*
		 * the number of files in the filenames list
		 */
		return flist_.size();
	}

	void Browser::_unique(void)
	{
		// this only makes sense if flist_ is sorted.
		flist_._unique();
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
	}

	fim::string Browser::_sort(const fim_char_t sc)
	{
		/*
		 *	sorts the image filenames list
		 */
		flist_._sort(sc);
		return current();
	}

	fim::string Browser::_random_shuffle(bool dts)
	{
		/*
		 *	sorts the image filenames list
		 *	if dts==true, do time() based seeding
		 *	TODO: it would be cool to support a user supplied seed value
		 */
		if( dts )
			std::srand(time(FIM_NULL));	/* FIXME: AFAIK, effect of srand() on random_shuffle is not mandated by any standard. */
		std::random_shuffle(flist_.begin(),flist_.end());
		return current();
	}

	fim::string Browser::_clear_list(void)
	{
		/*
		 */
		flist_.erase(flist_.begin(),flist_.end());
		return 0;
	}

	fim::string Browser::_reverse(void)
	{
		/*
		 *	sorts the image filenames list
		 */
		std::reverse(flist_.begin(),flist_.end());
		return current();
	}

	fim::string Browser::_swap(void)
	{
		/*
		 *	swap current with first
		 */
		if( n_files() > 1 && current_n() )
			std::swap(flist_[0],flist_[current_n()]);
		return current();
	}

	fim::string Browser::regexp_goto(const args_t& args, fim_int src_dir)
	{
		/*
		 * goes to the next filename-matching file
		 * TODO: this member function shall only find the index and return it !
		 */
		size_t i,j,c = current_n(),s = flist_.size();
		const char *rso = FIM_NULL;
		int rsic = 1; /* ignore case */
		int rsbn = 1; /* base name */
		FIM_PR('*');

		if( cc.isSetVar(FIM_VID_RE_SEARCH_OPTS) )
		       rso = strdupa ( cc.getStringVariable(FIM_VID_RE_SEARCH_OPTS).c_str());
		if(!rso)
			rso = "bi";

		if ( rso && strchr(rso,'i') )
			rsic = 1;
		else
			if ( rso && strchr(rso,'I') )
				rsic = 0;
		if ( rso && strchr(rso,'b') )
			rsbn = 1;
		else
			if ( rso && strchr(rso,'f') )
				rsbn = 0;

		if( args.size() < 1 || s < 1 )
			goto nop;

		last_regexp_ = args[0];
		last_src_dir_ = src_dir;

		for(j=0;j<s;++j)
		{
			const fim_char_t *fstm = FIM_NULL;
			bool hm = false;

			i = ((src_dir<0?(s-j):j)+c+src_dir)%s;

			if(!(fstm = flist_[i].c_str()))
				continue;

			if(rsbn==1)
				fstm = fim_basename_of(fstm);

			hm = (commandConsole_.regexp_match(fstm,args[0].c_str(),rsic));
#if FIM_WANT_PIC_CMTS
			/* If filename does not match, we look for match on description. */
			if(!hm)
			{
				/* FIXME: in the long run need a uniform decision on whether comments are to be accessed by full path or basename. */
				const fim_char_t * bfstm = fim_basename_of(fstm);
				if(cc.id_.find(fim_fn_t(bfstm)) != cc.id_.end() )
					bfstm = (cc.id_[fim_fn_t(bfstm)]).c_str();
				hm = (commandConsole_.regexp_match(bfstm,args[0].c_str(),rsic));
			}
#endif /* FIM_WANT_PIC_CMTS */

			if(hm)
			{	
				fim::string c = current();
				FIM_AUTOCMD_EXEC(FIM_ACM_PREGOTO,c);
				goto_image(i);
#ifdef FIM_AUTOCMDS
				FIM_AUTOCMD_EXEC(FIM_ACM_POSTGOTO,c);
				if(!commandConsole_.inConsole())
					commandConsole_.set_status_bar((current()+fim::string(" matches \"")+args[0]+fim::string("\"")).c_str(),FIM_NULL);
				goto nop;
#endif /* FIM_AUTOCMDS */
			}
		}
		cout << "sorry, no filename matches \""<<args[0]<<"\"\n";
		if(!commandConsole_.inConsole())
			commandConsole_.set_status_bar((fim::string("sorry, no filename matches \"")+
						args[0]+
						fim::string("\"")).c_str(),FIM_NULL);
nop:
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim::string Browser::goto_image(fim_int n, bool isfg)
	{
		/*
		 *	FIX ME: ultimately, all file transitions should pass by here.
		 */
		fim::string result = FIM_CNS_EMPTY_RESULT;
#if FIM_USE_CXX11
		auto N = flist_.size();
#else /* FIM_USE_CXX11 */
		fim_int N = flist_.size();
#endif /* FIM_USE_CXX11 */
		FIM_PR('*');

		if( !N )
			goto ret;

		if( !isfg )
#if FIM_WANT_BDI
		if( N==1 &&              c_image()->is_multipage())
#else	/* FIM_WANT_BDI */
		if( N==1 && c_image() && c_image()->is_multipage())
#endif	/* FIM_WANT_BDI */
		{
			//if(1)std::cout<<"goto page "<<n<<FIM_CNS_NEWLINE;
			FIM_PR(' ');
			image()->goto_page(n);
			result = N;
			goto ret;
		}
#if FIM_WANT_GOTOLAST
		if(getGlobalIntVariable(FIM_VID_LASTFILEINDEX) != current_image())
			setGlobalVariable(FIM_VID_LASTFILEINDEX, current_image());
		flist_.set_cf(FIM_MOD(n,N));
#if FIM_WANT_LASTGOTODIRECTION
		if( ( n_files() + current_image() - getGlobalIntVariable(FIM_VID_LASTFILEINDEX) ) % n_files() > n_files() / 2 )
			setGlobalVariable(FIM_VID_LASTGOTODIRECTION,"-1");
		else
			setGlobalVariable(FIM_VID_LASTGOTODIRECTION,"+1");
#endif /* FIM_WANT_LASTGOTODIRECTION */
#endif /* FIM_WANT_GOTOLAST */
		FIM_PR(' ');
		setGlobalVariable(FIM_VID_PAGE ,(fim_int)0);
		setGlobalVariable(FIM_VID_FILEINDEX,current_image());
		//setGlobalVariable(FIM_VID_FILEINDEX,cf_);
		setGlobalVariable(FIM_VID_FILENAME, current().c_str());
		FIM_PR(' ');
		//loadCurrentImage();
		result = n_files()?(flist_[current_n()]):nofile_;
ret:
		FIM_PR('.');
		return result;
	}

	fim::string Browser::get_next_filename(int n)const
	{
		/*
		 * returns to the next image in the list, the mechanism
		 * p.s.: n<>0
		 */
		if( !flist_.size() )
			return FIM_CNS_EMPTY_RESULT;
		return flist_[FIM_MOD(flist_.cf() + n,flist_.size())];
	}

	fim::string Browser::next(int n)
	{
		fim::string gs = "+";

		gs += fim::string(n);
		return goto_image_internal(gs.c_str(),FIM_X_NULL);  
	}

	fim::string Browser::prev(int n)
	{
		fim::string gs = "-";
		
		gs += fim::string(n);
		return goto_image_internal(gs.c_str(),FIM_X_NULL);  
	}
	
	fim_cxr Browser::fcmd_goto_image(const args_t& args)
	{
		if( args.size() > 0 )
			return goto_image_internal(args[0].c_str(),FIM_X_NULL);
		else
			return goto_image_internal(FIM_NULL,FIM_X_NULL);
	}

	fim::string Browser::goto_image_internal(const fim_char_t *s,fim_xflags_t xflags)
	{
		/*
		 */
		const fim_char_t*errmsg = FIM_CNS_EMPTY_STRING;
		const int cf = flist_.cf(),cp =getGlobalIntVariable(FIM_VID_PAGE),pc = FIM_MAX(1,n_pages()),fc = n_files();
		fim_int gv = 0,nf = cf,mv = 0,np = cp;
		FIM_PR('*');

		if( n_files() == 0 || !s )
		{
			errmsg = FIM_CMD_HELP_GOTO;
			goto err;
		}
		if(!s)
		{
			errmsg = FIM_CMD_HELP_GOTO;
			goto err;
		}
		else
		{
			fim_char_t c = FIM_SYM_CHAR_NUL;
			fim_char_t l = FIM_SYM_CHAR_NUL;
			int sl = 0,li = 0;
			bool pcnt = false;
			bool isre = false;
			bool ispg = false;
			bool isfg = false;
			bool isrj = false;

			if( !s )
				goto ret;
			sl = strlen(s);
			if( sl < 1 )
				goto ret;
			c = *s;
			//for(li=sl-2;li<sl;++li) { l=tolower(s[li]); pcnt=(l=='%'); ispg=(l=='p'); }
			l = tolower(s[li=sl-1]);
			pcnt = (l=='%'); 
			ispg = (l=='p');
			isfg = (l=='f');
			isre = ((sl>=2) && ('/'==s[sl-1]) && (((sl>=3) && (c=='+' || c=='-') && s[1]=='/') ||( c=='/')));
			isrj = (c=='+' || c=='-');

#if FIM_WANT_VAR_GOTO
			if( c == '-' || c == '+' && s[1] && ( isalpha(s[1]) || s[1] == '_' ) )
			{
				const char * sp = s+2;
				int neg=(c=='-'?-1:1);
				bool ner=false; // non empty requirement

				while(*sp && ( isalpha(*sp) || *sp == '_' || isdigit(*sp) ) )
					++sp;
				if(*sp == '+' )
					ner = true;

				std::string varname(s+1,sp);
				std::string varval = cc.id_.vd_[fim_basename_of(current())].getStringVariable(varname);

				for(size_t fi=0;fi<fc;++fi)
				{
					size_t idx=(cf+neg*(1+fi))%fc;
					std::string nvarval = cc.id_.vd_[fim_basename_of(flist_[idx])].getStringVariable(varname);
					if(nvarval != varval && ! ( ner == true && !nvarval.size() ) )
					{
						//std::cout<<"["<<varname<<"]="<<varval<<" -> "<<nvarval<<std::endl;
						//std::cout<<cf<<" -> "<<idx<<std::endl;
						nf=idx;
						goto go_jump;
					}
				}
			}
#endif /* FIM_WANT_VAR_GOTO */
			if( isdigit(c)  || c == '-' || c == '+' )
			{
				gv = fim_atoi(s);
				if(gv == FIM_CNS_LAST)
					gv = -1;
			}
			else
			       	if( c == '^' || c == 'f' )
					gv = 1;
			else
			       	if( c == '$' || c == 'l' )
					gv = -1;// temporarily
			else
			if( c == '?' )
			{
				gv = find_file_index(string(s).substr(1,sl-1));
				//std::cout<<string(s).substr(1,sl-1)<<" "<<gv<<FIM_CNS_NEWLINE;
				if( gv < 0 )
				{
					goto ret;
				}
				nf = gv;
				goto go_jump;
			}
			else
				if( isre )
					{;}
			else
			{
				cout << " please specify a number or ^ or $\n";
			}
			if( li > 0 && ( isfg || pcnt || ispg ))
			{
				l = tolower( s[ li = sl-2 ] );
				if( l == '%' )
					pcnt = true;
				if( l == 'p' )
					ispg = true;
				if( l == 'f' )
					isfg = true;
			}
			if( c=='$' || c == 'l' )
				gv = mv - 1;
			if( (isrj) && (!isfg) && (!ispg) && pc > 1 )
			{
				ispg = true;
				if(( cp == 0 && gv < 0 ) || (cp == pc-1 && gv > 0 ) )
					if( fc > 1 )
						isfg = true, ispg = false;
			}
			if( ispg )
				mv = pc;
			else
				mv = fc;
			if( pcnt )
			{
			       	gv = FIM_INT_PCNT_OF_100(gv,mv);
			}
			if( !mv )
			{
			       	goto ret; 
			}
			if( isfg && ispg )
			{
				// std::cout << "!\n";
				goto err;
			}
			//if((!isre) && (!isrj))nf=gv;
			//if(isrj && gv<0 && cf==1){cf=0;}//TODO: this is a bugfix
			if( (!isrj) && gv > 0 )
				gv = gv - 1;// user input is interpreted as 1-based 
			gv = FIM_MOD(gv,mv);
			/* gv=FIM_MAX(FIM_MIN(gv,mv-1),0); */
			//cout << "at " << cf <<", gv="<<gv <<", mod="<<mv<<FIM_CNS_NEWLINE;
			if( ispg )
			{
				if( isrj )
					{ np = cp + gv;}// FIXME: what if gv gv<1 ? pity :)
				else
					np = gv;
			}
			else
			{
				np = 0; /* first page -- next file's page count is unknown ! */
				if( isrj )
					{nf = cf + gv;}// FIXME: what if gv gv<1 ? pity :)
				else
					nf = gv;
			}
			gv = FIM_MOD(gv,mv);
			nf = FIM_MOD(nf,fc);
			np = FIM_MOD(np,pc);
//go:
			if(0)
			cout << "goto: "
				<<" s:" << s
				<<" cf:" << cf 
				<<" cp:" << cp 
				<< " nf:" << nf 
				<< " np:" << np 
				<< " gv:" << gv 
				<< " mv:" << mv 
				<< " isrj:"<<isrj
				<< " ispg:"<<ispg
				<< " isfg:"<<isfg
				<< " pcnt:"<<pcnt
				<< " max[pf]:"<<mv
				<<FIM_CNS_NEWLINE;
			if(isre)
			{
				args_t argsc;
				fim_int src_dir = 1;

				if( c == '-' )
					src_dir = ((c=='-')?-1:1);
				if( (c == '+' || c == '-' ) && sl == 3 && s[1] == '/' && s[2] == '/' )
					argsc.push_back(last_regexp_);
				else
                                {
				        int sks = (c == '+' || c == '-' ) ? 1 : 0;
					argsc.push_back(string(s).substr(1+sks,sl-2-sks));
                                }
				FIM_PR('.');
				return regexp_goto(argsc,src_dir);
			}
go_jump:
			if( ( nf != cf ) || ( np != cp ) )
			{	
				fim::string c = current();
				if(!(xflags&FIM_X_NOAUTOCMD))
				{ FIM_AUTOCMD_EXEC(FIM_ACM_PREGOTO,c); }
				if( ispg )
					image()->goto_page(np);
				else
					goto_image(nf,isfg?true:false);

				if(!(xflags&FIM_X_NOAUTOCMD))
				{ FIM_AUTOCMD_EXEC(FIM_ACM_POSTGOTO,c); }
			}
		}
ret:
		errmsg = FIM_CNS_EMPTY_RESULT;
err:
		FIM_PR('.');
		return errmsg;
	}

	fim_stat_t fim_get_stat(const fim_fn_t& fn, bool * dopushp);

	fim::string Browser::do_filter(const args_t& args, MatchMode rm, bool negative, enum FilterAction faction)
	{
		/*
		 * TODO: introduce flags for negative (instead of a bool).
		 * TODO: message like 'marked xxx files' ...
		 */
		fim::string result;
		const bool wom = (flist_.size() > FIM_CNS_ENOUGH_FILES_TO_WARN );
		fim_int marked = 0;
		fim_bitset_t lbs(flist_.size()); // limit bitset, used for mark / unmark / delete / etc
		// fim_fms_t dt;

		FIM_PR('*');
		// std::cout << "match mode:" << rm << "  negation:" << negative << "  faction:" << faction << "\n"; // TODO: a debug mode shall enable such printouts.

		if ( args.size() > 1 && rm == ListIdxMatch )
		{
			size_t min_idx=0,max_idx=FIM_MAX_VALUE_FOR_TYPE(size_t);
			std::string ss(args[1]);
			std::istringstream is(ss);

			if(is.peek()!='-') // MIN-
			{
				if(!isdigit(is.peek()))
				{
					result = "Bad MINIDX[-MAXIDX] expression ! MINIDX or MAXIDX should be a number.";
					goto nop;
				}
				is >> min_idx;
				if(tolower(is.peek())=='k')
					is.ignore(1), min_idx *= FIM_CNS_Ki;
				while(is.peek() != -1 && is.peek() != '-')
					is.ignore(1);
				if(is.peek() == -1)
				{
					max_idx=min_idx;
					goto parsed_idx;
				}
			}

			if(is.peek()=='-') // -MAX
			{
				is.ignore(1);
				is >> max_idx;
				if(tolower(is.peek())=='k')
					is.ignore(1), max_idx *= FIM_CNS_Ki;
			}
parsed_idx:
			if(min_idx>max_idx)
				std::swap(min_idx,max_idx);
		
			if(min_idx>=flist_.size() || max_idx < 1)
			{
				if(wom) commandConsole_.set_status_bar("requested index range is wrong (not so many files) :-)", "*");
				goto nop;
			}

			min_idx=FIM_MIN(FIM_MAX(min_idx,1),flist_.size());
			max_idx=FIM_MIN(FIM_MAX(max_idx,1),flist_.size());

			// std::cout << "Limiting index between " << min_idx << " and " << max_idx << " .\n";

			if(min_idx==1 && max_idx>=flist_.size())
			{
				if(wom) commandConsole_.set_status_bar("requested index limit range covers entire list :-)", "*");
				goto nop;
			}

			if(wom) commandConsole_.set_status_bar("limiting to list index range...", "*");

			/* not efficient but conceptually clean in this context */
			for(size_t fi=min_idx;fi<=max_idx;++fi)
				lbs.set(fi-1);
			negative = !negative;
			goto rfrsh;
		}

		if ( rm == TimeMatch || rm == SizeMatch )
		{
#if FIM_WANT_FLIST_STAT 
			off_t min_size=0,max_size=FIM_MAX_VALUE_FOR_TYPE(off_t);
			time_t min_mtime=0,max_mtime=FIM_MAX_VALUE_FOR_TYPE(time_t);
			std::string ss(args[args.size()>1?1:0]); // if 0 we'll ignore this
			std::istringstream is(ss);

			if ( rm == SizeMatch )
			{
				if(args.size()==1)
				{
					fim_stat_t fss = fim_get_stat(current(),FIM_NULL);
					if( ( min_size=max_size=fss.st_size ) == 0 ) 
						goto nop;
					goto parsed_limits;
				}
				
				if(is.peek()!='-') // MIN-
				{
					if(!isdigit(is.peek()))
					{
						result = "Bad MINSIZE[-MAXSIZE] expression ! MINSIZE or MAXSIZE should be a number followed by K or M.";
						goto nop;
					}
					is >> min_size;
					if(tolower(is.peek())=='k')
						is.ignore(1), min_size *= FIM_CNS_K;
					else
					if(tolower(is.peek())=='m')
						is.ignore(1), min_size *= FIM_CNS_M;
					while(is.peek() != -1 && is.peek() != '-')
						is.ignore(1);
					if(is.peek() == -1)
					{
						max_size=min_size;
						goto parsed_limits;
					}
				}

				if(is.peek()=='-') // -MAX
				{
					is.ignore(1);
					is >> max_size;
					if(tolower(is.peek())=='k')
						is.ignore(1), max_size *= FIM_CNS_K;
					else
					if(tolower(is.peek())=='m')
						is.ignore(1), max_size *= FIM_CNS_M;
				}
			}

			if ( rm == TimeMatch )
			{
				if(args.size()==1)
				{
					fim_stat_t fss = fim_get_stat(current(),FIM_NULL);
					if( ( min_mtime=max_mtime=fss.st_mtime ) == 0 ) 
						goto nop;
					min_mtime-=60*60*24;
					max_mtime+=60*60*24;
					goto parsed_limits;
				}
				
				if(strchr(args[1].c_str(),'/'))
				{
					// DD/MM/YYYY format
					min_mtime=0;

					if(isdigit(is.peek()))
					{
						struct tm min_mtime_str;
						fim_bzero(&min_mtime_str,sizeof(min_mtime_str));

						is >> min_mtime_str.tm_mday;
						if(is.peek() != '/')
							goto errpd;
						is.ignore(1);
						is >> min_mtime_str.tm_mon;
						min_mtime_str.tm_mon-=1;
						if(is.peek() != '/')
							goto errpd;
						is.ignore(1);
						is >> min_mtime_str.tm_year;
						min_mtime_str.tm_year-=1900;

						min_mtime=mktime(&min_mtime_str);
					}
					else
						; // -MAX

					if(is.peek() == -1)
						max_mtime = min_mtime;

					if(is.peek() != '-')
						goto errpd;
					is.ignore(1); // eat '-'

					if(!isdigit(is.peek()))
						goto errpd;
					else
					{
						struct tm max_mtime_str;
						fim_bzero(&max_mtime_str,sizeof(max_mtime_str));

						is >> max_mtime_str.tm_mday;
						if(is.peek() != '/')
							goto errpd;
						is.ignore(1);
						is >> max_mtime_str.tm_mon;
						max_mtime_str.tm_mon-=1;
						if(is.peek() != '/')
							goto errpd;
						is.ignore(1);
						is >> max_mtime_str.tm_year;
						max_mtime_str.tm_year-=1900;

						max_mtime=mktime(&max_mtime_str);
					}
					// std::cout << min_mtime << " .. "  << max_mtime << "\n";

					goto parsed_limits;
				}
				// FIXME: this code is still far from clean / perfect.
				if(is.peek()!='-') // MIN-
				{
					if(!isdigit(is.peek()))
					{
						result = "Bad MINTIME[-MAXTIME] expression ! MINTIME or MAXTIME should be a number.";
						goto nop;
					}
					is >> min_mtime;
					while(is.peek() != -1 && is.peek() != '-')
						is.ignore(1);
					if(is.peek() == -1)
					{
						max_mtime=min_mtime;
						goto parsed_limits;
					}
				}

				if(is.peek()=='-') // -MAX
				{
					is.ignore(1);
					is >> max_mtime;
				}
			}
parsed_limits:
			if(min_size>max_size)
				std::swap(min_size,max_size);
			if(min_mtime>max_mtime)
				std::swap(min_mtime,max_mtime);

			// std::cout << "Limiting size between " << min_size << " and " << max_size << " bytes.\n";
			// std::cout << "Limiting time between " << min_mtime << " and " << max_mtime << " .\n";

			if(wom)
				 commandConsole_.set_status_bar("getting stat() info...", "*");
			flist_.get_stat();

			if ( rm == SizeMatch )
			{
				if(wom)
					commandConsole_.set_status_bar("limiting to a size...", "*");
				for(size_t fi=0;fi<flist_.size();++fi)
					if ( ! FIM_WITHIN ( min_size, flist_[fi].stat_.st_size, max_size ) )
						lbs.set(fi);
			}
			if ( rm == TimeMatch )
			{
				if(wom)
					commandConsole_.set_status_bar("limiting to a timespan...", "*");
				for(size_t fi=0;fi<flist_.size();++fi)
					if ( ! FIM_WITHIN ( min_mtime, flist_[fi].stat_.st_mtime, max_mtime ) )
						lbs.set(fi);
			}
#else /* FIM_WANT_FLIST_STAT */
#endif /* FIM_WANT_FLIST_STAT */
			goto rfrsh;
	errpd:
			goto nop;
		}
#if FIM_WANT_LIMIT_DUPBN
		if ( rm == DupFileNameMatch || rm == UniqFileNameMatch || rm == FirstFileNameMatch || rm == LastFileNameMatch)
		{
			flist_t slist = flist_;

			slist._sort(FIM_SYM_SORT_BN);
			if(wom)
			{
				const char * msg = "";
				switch( rm )
				{
					case(  DupFileNameMatch): msg = "limiting to duplicate base filenames..."; break;
					case( UniqFileNameMatch): msg = "limiting to unique base filenames..."; break;
					case(FirstFileNameMatch): msg = "limiting to first base filenames..."; break;
					case( LastFileNameMatch): msg = "limiting to last base filenames..."; break;
				}
			       	commandConsole_.set_status_bar(msg, "*");
			}

			/* as an improvement, might use a std::map<file name type, bit>, rather than this */
			for(size_t i=0;i<slist.size();++i)
			{
				size_t j=i,ii=0,jj=0;

				while(j+1<slist.size() && 0 == 
					strcmp(fim_basename_of(slist[i].c_str()),fim_basename_of(slist[j+1].c_str())) )
					++j;
				// j+1-i same basenames
				if ( rm == DupFileNameMatch )
				{
					if(i==j)
						ii=i,jj=i; // no dups: no results
					else
						ii=i,jj=j+1; // all dups: all results
				}
				if ( rm == UniqFileNameMatch )
				{
					if(i==j)
						ii=i,jj=i+1; // uniq; a result
					else
						ii=i,jj=i; // there are dups: no results
				}
				if ( rm == FirstFileNameMatch)
				{
					ii=i,jj=i+1; // first is result
				}
				if ( rm == LastFileNameMatch)
				{
					ii=j,jj=j+1; // last is result
				}
				for(i=ii;i<jj;++i)
				{
					fim_int fi = find_file_index(slist[i]);
					if(fi>=0)
						lbs.set(fi);
				}
				i=j;
			}
			negative = !negative;
			goto rfrsh;
		}
#endif /* FIM_WANT_LIMIT_DUPBN */
#if FIM_WANT_FILENAME_MARK_AND_DUMP
		if ( rm == MarkedMatch )
		{
			if(wom) commandConsole_.set_status_bar("limiting marked...", "*");
			for(size_t i=0;i<flist_.size();++i)
				if( cc.isMarkedFile(flist_[i]) == negative )
					lbs.set(i);
			goto rfrsh;
		}
#endif /* FIM_WANT_FILENAME_MARK_AND_DUMP */
		if( flist_.size() < 1 )
		{
			result = "the files list is empty\n";
			goto nop;
		}
		if(args.size()>0)
		{
			args_t rlist = args;	//the remove list
			/*
			 * the list is unsorted. it may contain duplicates
			 * if this software will have success, we will have indices here :)
			 * sort(rlist.begin(),rlist.end());...
			 */
			//fim_int lf = flist_.size();
#if FIM_WANT_PIC_LVDN
			if ( rm == VarMatch )
				if(rlist.size() < 2)
					rm = CmtMatch;
#else
			if ( rm == VarMatch )
				rm = FullFileNameMatch;
#endif /* FIM_WANT_PIC_LVDN */
			// if(wom) commandConsole_.set_status_bar("limiting matching...", "*");
			if(wom)
			{
				const char * msg = "";
				switch( rm )
				{
					case(FullFileNameMatch   ): msg = "limiting to full filename match..."; break;
					case(PartialFileNameMatch): msg = "limiting to partial filename match..."; break;
					case(VarMatch            ): msg = string("limiting variable \"") + string(rlist[0]) + string("\" matching value \"") + string(rlist[1]) + string("\" ..."); break;
					case(CmtMatch            ): msg = "limiting to comment match..."; break;
				}
			       	commandConsole_.set_status_bar(msg, "*");
			}

			// dt = - getmilliseconds();
			for(size_t r=0;r<rlist.size();++r)
			for(size_t i=0;i<flist_.size();++i)
			{
				bool match = false;
#if HAVE_FNMATCH_H && 0
				match |= ( rm == FullFileNameMatch && ( ( 0 == fnmatch(rlist[r].c_str(), flist_[i].c_str(),0)  ) != negative ) );
#else /* HAVE_FNMATCH_H */
				match |= ( rm == FullFileNameMatch && ( ( flist_[i] == rlist[r] ) != negative ) );
#endif /* HAVE_FNMATCH_H */
				match |= ( rm == PartialFileNameMatch && ( ( flist_[i].re_match(rlist[r].c_str()) ) != negative ) );
#if FIM_WANT_PIC_LVDN
				match |= ( rm == VarMatch     && ( ( cc.id_.vd_[fim_basename_of(flist_[i].c_str())].getStringVariable(rlist[0]) == rlist[1] ) != negative ) );
				match |= ( rm == CmtMatch     && ( ( string(cc.id_    [fim_basename_of(flist_[i].c_str())]).re_match(rlist[0].c_str()) ) != negative ) );
#endif /* FIM_WANT_PIC_LVDN */
				if( match )
					lbs.set(i); // TODO: further cycles on r are unnecessary after match.
			}
			// dt+ =  getmilliseconds();
			// std::cout << fim::string("    limiting took ") << dt << " ms" << std::endl;

	//		if(lf != flist_.size() )
	//			cout << "limited to " << int(flist_.size()) << " files, excluding " << int(lf - flist_.size()) << " files." <<FIM_CNS_NEWLINE;
			if(negative)
			       	negative = !negative;
			goto rfrsh;
		}
		else
		{
			/* removes the current file from the list */
			result = pop_current();
			goto nop;
		}
rfrsh:
		if (negative)
			lbs.negate();
		// dt = - getmilliseconds();
		if (lbs.any())
		{
			size_t tsize = lbs.size(),cnt=lbs.count();

			if(faction == Delete)
				cnt=tsize-cnt;

			result = "Selected ";
		       	result += string((fim_int)cnt);
			result += " files out of ";
			result += string((fim_int)tsize);
			result += ".\n";

			if(faction == Delete)
			{
				if(cnt < tsize/2) // a very rough but effective first optimization
					flist_ = flist_.copy_from_bitset(lbs);
				else
					flist_.erase_at_bitset(lbs);
			}
			else /* Mark / Unmark */
			{
				for(size_t pos=0;pos<tsize;++pos)
					if(lbs.at(pos))
						marked += cc.markFile(flist_[pos],(faction == Mark),false);
			}
		}
		// dt += getmilliseconds();
		// std::cout << fim::string("bit limiting took ") << dt << " ms" << std::endl;

		//if ((faction != Delete) && marked)
		//	cout << ( faction == Mark ? "  Marked " : "Unmarked "  ) << marked << " files\n";

		setGlobalVariable(FIM_VID_FILEINDEX,current_image());
		setGlobalVariable(FIM_VID_FILELISTLEN,n_files());
		goto nop;
		result = FIM_CNS_EMPTY_RESULT;
nop:
		FIM_PR('.');
		return result;
	}

	fim_cxr Browser::fcmd_scrollforward(const args_t& args)
	{
		/*
		 * scrolls the image as it were a book :)
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c = current();
		FIM_PR('*');

		FIM_AUTOCMD_EXEC(FIM_ACM_PREPAN,c);
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
			else
			       	viewport()->pan("right",FIM_CNS_SCROLL_DEFAULT);
		}
		else
		       	next(1);
		FIM_AUTOCMD_EXEC(FIM_ACM_POSTPAN,c);
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_cxr Browser::fcmd_scrolldown(const args_t& args)
	{
		/*
		 * scrolls the image down 
		 *
		 * FIX ME : move to Viewport
		 */
		fim::string c = current();
		FIM_PR('*');

		FIM_AUTOCMD_EXEC(FIM_ACM_PREPAN,c);
		if(c_image() && viewport())
		{
			if(viewport()->onBottom())
				next();
			else
				viewport()->pan("down",FIM_CNS_SCROLL_DEFAULT);
		}
		else
		       	next(1);
		FIM_AUTOCMD_EXEC(FIM_ACM_POSTPAN,c);
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_cxr Browser::fcmd_info(const args_t& args)
	{
		/*
		 *	short information in status-line format
		 */
#if 0
		string fl;
		for(size_t r=0;r<flist_.size();++r)
		{
			fl+=flist_[r];
			fl+=FIM_CNS_NEWLINE;
		}
		return fl;
#else
		fim::string r = current();
		FIM_PR('*');

		if(image())
			r += image()->getInfo();
		else
			r += " (unloaded)";
		FIM_PR('.');
		return r;
#endif
	}

	fim::string Browser::info(void)
	{
		/*
		 *	short information in status-line format
		 */
		return fcmd_info(args_t(0));
	}

	fim_cxr Browser::fcmd_rotate(const args_t& args)
	{
		/*
		 * rotates the displayed image a specified amount of degrees
		 */ 
		fim_angle_t angle;

		FIM_PR('*');

		if( args.size() == 0 )
			angle = FIM_CNS_ANGLE_ONE;
		else
			angle = fim_atof(args[0].c_str());
		if( angle == FIM_CNS_ANGLE_ZERO)
			goto ret;

		if(c_image())
		{
			//angle = (double)getGlobalFloatVariable(FIM_VID_ANGLE);
			fim::string c = current();
//			FIM_AUTOCMD_EXEC(FIM_ACM_PREROTATE,c);//FIXME
			FIM_AUTOCMD_EXEC(FIM_ACM_PRESCALE,c); //FIXME
			if( c_image() )
			{
				if(angle)
				{
					if(image())
						image()->rotate(angle);
				}
				else	
				{
					if(image())
						image()->rotate();
				}
			}
//			FIM_AUTOCMD_EXEC(FIM_ACM_POSTROTATE,c);//FIXME
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTSCALE,c); //FIXME
		}
ret:
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

#if FIM_WANT_FAT_BROWSER
	fim_cxr Browser::fcmd_magnify(const args_t& args)
	{
		/*
		 * magnifies the displayed image
		 */ 
		FIM_PR('*');
		if(c_image())
		{
			fim_scale_t factor;
			fim::string c = current();
			factor = firstforzero(args);
			if( !factor )
				factor = (fim_scale_t)getGlobalFloatVariable(FIM_VID_MAGNIFY_FACTOR);
			FIM_AUTOCMD_EXEC(FIM_ACM_PRESCALE,c);
			if(c_image())
			{
				if(factor)
				{
					if(image())
						image()->magnify(factor);
					if(viewport())
						viewport()->scale_position_magnify(factor);
				}
				else	
				{
					if(image())
						image()->magnify();
					if(viewport())
						viewport()->scale_position_magnify();
				}
			}
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTSCALE,c);
		}
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_cxr Browser::fcmd_reduce(const args_t& args)
	{
		/*
		 * reduces the displayed image size
		 */ 
		FIM_PR('*');
		if(c_image())
		{
			fim_scale_t factor;
			factor = firstforzero(args);
			if(!factor)
				factor = (fim_scale_t)getGlobalFloatVariable(FIM_VID_REDUCE_FACTOR);
			fim::string c = current();
			FIM_AUTOCMD_EXEC(FIM_ACM_PRESCALE,c);
			if(c_image())
			{
				if(factor)
				{
					if(image())
						image()->reduce(factor);
					if(viewport())
						viewport()->scale_position_reduce(factor);
				}
				else	
				{
					if(image())
						image()->reduce();
					if(viewport())
						viewport()->scale_position_reduce();
				}
			}
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTSCALE,c);
		}
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
	}
#endif /* FIM_WANT_FAT_BROWSER */

	fim_cxr Browser::fcmd_align(const args_t& args)
	{
		/*
		 * aligns to top/bottom the displayed image
		 * TODO: incomplete
		 */ 
		FIM_PR('*');
		if( args.size() < 1 )
			goto err;
		if( !args[0].c_str() || !args[0].re_match("^(bottom|top|left|right|center)") )
			goto err;
		if( c_image() )
		{
			fim::string c = current();
			FIM_AUTOCMD_EXEC(FIM_ACM_PREPAN,c);
			if(c_image() && viewport())
			{
				// FIXME: need a switch/case construct here
				if(args[0].re_match("top"))
					viewport()->align('t');
				if(args[0].re_match("bottom"))
					viewport()->align('b');
				if(args[0].re_match("left"))
					viewport()->align('l');
				if(args[0].re_match("right"))
					viewport()->align('r');
				if(args[0].re_match("center"))
					viewport()->align('c');
			}
			FIM_AUTOCMD_EXEC(FIM_ACM_POSTPAN,c);
		}
		FIM_PR('.');
		return FIM_CNS_EMPTY_RESULT;
err:
		FIM_PR('.');
		return FIM_CMD_HELP_ALIGN;
	}

	ImageCPtr Browser::c_image(void)const
	{
		/*
		 *	a const pointer to the currently loaded image
		 */
		ImageCPtr image = FIM_NULL;

		if( commandConsole_.current_viewport() )
			image = commandConsole_.current_viewport()->c_getImage();
		return image;
	}

	ImagePtr Browser::image(void)const
	{
		/*
		 *	the image loaded in the current viewport is returned
		 */
		ImagePtr image = FIM_NULL;

		if( commandConsole_.current_viewport() )
			image = commandConsole_.current_viewport()->getImage();
		return image;
	}

	Viewport* Browser::viewport(void)const
	{
		/* 
		 * A valid pointer will be returned 
		 * whenever the image is loaded !
		 *
		 * FIM_NULL is returned in case no viewport is loaded.
		 * */
		return (commandConsole_.current_viewport());
	}

	fim_fn_t Browser::current(void)const
	{
		/*
		 * dilemma : should the current() filename and next() operations
		 * be relative to viewport's own current's ?
		 * */
		if( empty_file_list() )
			return nofile_;

	       	return flist_[flist_.cf()];
	}

	int Browser::empty_file_list(void)const
	{
		/*
		 *	is the filename list empty ?
		 */
		return flist_.size() == 0;
	}

	fim::string Browser::display(void)
	{
		/*
		 *	display the current image
		 */
#if FIM_USE_CXX11
		return fcmd_display({});
#else /* FIM_USE_CXX11 */
		return fcmd_display(args_t());
#endif /* FIM_USE_CXX11 */
	}

	fim::string Browser::pop_current(const args_t& args)
	{
		/*
		 *	pops the last image filename off the image list
		 */
		return pop_current();
	}

	fim::string Browser::do_push(const args_t& args)
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
			fim::string ss = args[i];

			ss.substitute(" +$","");
			push(ss);
#else /* FIM_SMART_COMPLETION */
			push(args[i]);
#endif /* FIM_SMART_COMPLETION */
		}
		return FIM_CNS_EMPTY_RESULT;
	}

	fim_int Browser::current_image(void)const
	{
		/* counting from 1 */
		return flist_.cf() + 1;
	}

	fim_int Browser::n_pages(void)const
	{
		fim_int pi = 0;

#if !FIM_WANT_BDI
		if( c_image() )
#endif	/* FIM_WANT_BDI */
			pi = c_image()->n_pages();
		return pi;
	}

	fim_int Browser::c_page(void)const
	{
		fim_int pi = 0;

#if !FIM_WANT_BDI
		if( c_image() )
#endif	/* FIM_WANT_BDI */
			pi = c_image()->c_page();
		return pi;
	}

	size_t Browser::byte_size(void)const
	{
		size_t bs = 0;

		bs += cache_.byte_size();;
		bs += sizeof(*this);
		/* TODO: this is incomplete ... */
		return bs;
	}

	void Browser::mark_from_list(const args_t& argsc)
	{
		/* TODO: one would like to have better matching options */
		if (argsc.size() )
			this->do_filter(argsc,Browser::PartialFileNameMatch,false,Browser::Mark);
	}

	bool Browser::dump_desc(const fim_fn_t nf, fim_char_t sc, const bool want_all_vars, const bool want_append)const
	{
		/* 
		   Dumps descriptions of the current images list.
		   on no comment does not touch the file.
		 */
#if FIM_WANT_PIC_LBFL
		// TODO: if no nf, then standard output
		fim::string cmtfc; // comment file contents

		for(size_t i=0;i<flist_.size();++i)
		{
			fim::string bof = flist_[i].c_str();
#if FIM_WANT_PIC_LVDN
			if( cc.id_.find(bof) == cc.id_.end() )
				bof = fim_basename_of(flist_[i].c_str());
			if(want_all_vars)
				cmtfc += cc.id_.vd_[bof].get_variables_list(true, true);
#endif /* FIM_WANT_PIC_LVDN */
			cmtfc += bof,
			cmtfc += sc,
			cmtfc += cc.id_[bof];
			cmtfc += "\n";
#if FIM_WANT_PIC_LVDN
#if 0
			if(want_all_vars)
				cmtfc += cc.id_.vd_[bof].get_variables_list(false,true),
				cmtfc += "\n";
#else
			if(want_all_vars)
				cmtfc += "#!fim:!=\n"; /* clean up vars for the next */
#endif
#endif /* FIM_WANT_PIC_LVDN */
		}

		if ( write_to_file(nf,cmtfc,want_append) )
			cout << "Successfully wrote to file " << nf << " .\n";
		else
			goto err;
/*
 		// TODO: this will print info about all of them
		cc.id_.print_descs(std::cout,sc);
*/
		return true;
err:
#else /* FIM_WANT_PIC_LBFL */
		/* ... */
#endif /* FIM_WANT_PIC_LBFL */
		return false;
	}

#if FIM_WANT_BENCHMARKS
	fim_int Browser::get_n_qbenchmarks(void)const
	{
		return 2;
	}
	void Browser::quickbench_init(fim_int qbi)
	{
		string msg;
		
		switch(qbi)
		{
			case 0:
			msg="fim browser push check";
			std::cout << msg << " : " << "please be patient\n";
			break;
			case 1:
			msg="fim browser pop check";
			std::cout << msg << " : " << "please be patient\n";
			break;
		}
	}
	void Browser::quickbench_finalize(fim_int qbi)
	{
	}
	void Browser::quickbench(fim_int qbi)
	{
		static fim_int ci = 0;

		switch(qbi)
		{
			case 0:
			push(string(ci)+string(".jpg"));
			ci++;
			break;
			case 1:
			ci--;
			push(string(ci)+string(".jpg"));
			break;
		}
	}
	string Browser::get_bresults_string(fim_int qbi, fim_int qbtimes, fim_fms_t qbttime)const
	{
		string msg=FIM_CNS_EMPTY_STRING;

		switch(qbi)
		{
			case 0:
			msg+="fim browser push check";
			msg+=" : ";
			msg+=string((float)(((fim_fms_t)qbtimes)/((qbttime)*1.e-3)));
			msg+=" push()/s\n";
			break;
			case 1:
			msg+="fim browser pop check";
			msg+=" : ";
			msg+=string((float)(((fim_fms_t)qbtimes)/((qbttime)*1.e-3)));
			msg+=" pop()/s\n";
			break;
		}
		return msg;
	}
#endif /* FIM_WANT_BENCHMARKS */

} /* namespace fim */

namespace fim
{
	fim_stat_t fim_get_stat(const fim_fn_t& fn, bool * dopushp)
	{
		bool dopush = false;
#if FIM_WANT_FLIST_STAT
		struct stat stat_s;
		if(-1==stat(fn.c_str(),&stat_s))
			goto ret;
		if( S_ISDIR(stat_s.st_mode))
			goto ret;
		if( stat_s.st_size == 0 )
			goto ret;
#endif /* FIM_WANT_FLIST_STAT */
		dopush = true;
ret:
		if( dopushp )
		       *dopushp = dopush;
		return stat_s; // FIXME: what about failure ?
	}

	void flist_t::get_stat(void)
	{
		// TODO: this is costly, might print a message here.
		for(flist_t::iterator fit=begin();fit!=end();++fit)
			fit->stat_ = fim_get_stat(*fit,FIM_NULL);
	}

	flist_t::flist_t(const args_t& a):cf_(0)
       	{
		/* FIXME: unused for now */
		this->reserve(a.size());
		for(args_t::const_iterator fit=a.begin();fit!=a.end();++fit)
		{
			bool dopush;
			struct stat stat_s = fim_get_stat(*fit,&dopush);
			if(dopush)
				push_back(fim::fle_t(*fit,stat_s));
		}
#if FIM_USE_CXX11
		this->shrink_to_fit();
#else /* FIM_USE_CXX11 */
		this->reserve(this->size());
#endif /* FIM_USE_CXX11 */
	}

	const fim::string flist_t::pop(const fim::string& filename)
	{
		fim::string s;

		assert(this->cf());
		if( filename == FIM_CNS_EMPTY_STRING )
		{
			this->erase( this->begin() + cf_ );
			if( cf_ >= (int)this->size() && cf_ > 0 )
				cf_--;
			s = (*this)[this->size()-1];
		}
		else
		{
			// FIXME: shall use a search member function/function
			for( size_t i=0; i < this->size(); ++i )
				if( fim::string((*this)[i]) == filename )
					this->erase(this->begin()+i);
                        adj_cf();
		}
		return s;
	}

	//std::string fle_t::operator(){return fn;}
	fle_t::fle_t(void) { }
	fle_t::fle_t(const fim_fn_t& s):fim_fn_t(s){fim_bzero(&stat_,sizeof(stat_));}
	fle_t::fle_t(const fim_fn_t& s, const fim_stat_t & ss):fim_fn_t(s),stat_(ss) { }
	fle_t::operator fim_fn_t(void) const { return fim_fn_t(*this);} 

struct FimBaseNameSorter
{
	bool operator() (const fim_fn_t& lfn, const fim_fn_t& rfn)
	{ 
		const char * ls = lfn.c_str();
		const char * rs = rfn.c_str();
		int scr = 0;

		if(ls && rs)
			scr = (strcmp(fim_basename_of(ls),fim_basename_of(rs)));
		return (scr < 0);
	}
} fimBaseNameSorter;

#if FIM_WANT_SORT_BY_STAT_INFO
#if FIM_WANT_FLIST_STAT 
struct FimSizeSorter
{
	bool operator() (const fim::fle_t & lfn, const fim::fle_t & rfn)
	{ 
		return (lfn.stat_.st_size < rfn.stat_.st_size);
	}
}fimSizeSorter;
struct FimDateSorter
{
	bool operator() (const fim::fle_t & lfn, const fim::fle_t & rfn)
	{ 
		return (lfn.stat_.st_mtime < rfn.stat_.st_mtime);
	}
}fimDateSorter;
#else /* FIM_WANT_FLIST_STAT */
struct FimDateSorter
{
	bool operator() (const fim_fn_t& lfn, const fim_fn_t& rfn)
	{ 
		struct stat lstat_s;
		struct stat rstat_s;
		stat(lfn.c_str(),&lstat_s);
		stat(rfn.c_str(),&rstat_s);
		return (lstat_s.st_mtime < rstat_s.st_mtime);
	}
}fimDateSorter;
#endif /* FIM_WANT_FLIST_STAT */
#endif /* FIM_WANT_SORT_BY_STAT_INFO */

	void flist_t::_sort(const fim_char_t sc)
	{
		/* TODO: can cache sorting status, would make e.g. Browser::do_filter more efficient */

		if(sc==FIM_SYM_SORT_FN)
			std::sort(this->begin(),this->end());
		if(sc==FIM_SYM_SORT_BN)
			std::sort(this->begin(),this->end(),fimBaseNameSorter);
#if FIM_WANT_SORT_BY_STAT_INFO
		if(sc==FIM_SYM_SORT_MD)
		{
			this->get_stat();
			std::sort(this->begin(),this->end(),fimDateSorter);
		}
		if(sc==FIM_SYM_SORT_SZ)
		{
			this->get_stat();
			std::sort(this->begin(),this->end(),fimSizeSorter);
		}
#endif /* FIM_WANT_SORT_BY_STAT_INFO */
	}
} /* namespace fim */
