/* $LastChangedDate$ */
/*
 Window.cpp : Fim's own windowing system

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

/*
 *	This code is still experimental and programmed in great hurry.
 *	FIXME : there are bugs.
 */


#include "fim.h"

#ifdef FIM_WINDOWS

namespace fim
{
        fim::string Window::fcmd_cmd(const std::vector<fim::string> &args)
        {
		unsigned int i=0;
		fim_err_t rc=0;/*return code*/
#if FIM_DISABLE_WINDOW_SPLITTING
		return "Warning: window splitting is temporarily disabled. It shall fixed in a soon-to-come version.\n";
#endif
#ifdef FIM_AUTOCMDS
		fim::string c=getGlobalIntVariable(FIM_VID_FILENAME);
		// note that an autocommand on a transient object is lethal
		if(amroot_)autocmd_exec(FIM_ACM_PREWINDOW,c);
#endif
		try
		{
		while(i<args.size())
                {
			string cmd=args[i];
			const fim_coo_t es=FIM_CNS_WENLARGE_STEPS_DEFAULT;
			if(cmd == "split" || cmd == "hsplit")
			{
				hsplit();
			}
			else if(cmd == "vsplit")
			{
				vsplit();
			}
/*			else if(cmd == "draw")
			{
				draw();
				return "\n";
			}*/
			else if(cmd == "normalize")
			{
				normalize();
				return "\n";
			}
			else if(cmd == "enlarge")
			{
				/*
				 * NOTE : this is not yet RECURSIVE !
				 * */
				enlarge(es);
				return "\n";
			}
			else if(cmd == "venlarge")
			{
				/*
				 * NOTE : this is not yet RECURSIVE !
				 * */
				venlarge(es);
				return "\n";
			}
			else if(cmd == "henlarge")
			{
				henlarge(es);
				return "\n";
			}
			else if(cmd == "up"   ) { move_focus(Up   ); }
			else if(cmd == "down" ) { move_focus(Down ); }
			else if(cmd == "left" ) { move_focus(Left ); }
			else if(cmd == "right") { move_focus(Right); }
			else if(cmd == "close") { close(); }
			else if(cmd == "swap") { swap(); }
			else rc=-1;

			/*
			 * FIXME
			 * */
			if(rc!=0) return "window : bad command\n";

			/*
			else if(cmd == "test")
			{
				fim::cout << "test ok!!\n";
				fb_clear_rect(10, 610, 100*4,100);
				return "test ok!!!\n";
			}*/
			++i;
                }
                }
		catch(FimException e)
		{
			cout << "please note some problems occurred in the window subsystem\n";
		}
#ifdef FIM_AUTOCMDS
		// note that an autocommand on a transient object is lethal
		if(amroot_)autocmd_exec(FIM_ACM_POSTWINDOW,c);
#endif
                return FIM_CNS_EMPTY_RESULT;
        }

	Window::Window(CommandConsole &c,const Rect& corners, Viewport* vp):corners_(corners),focus_(false),first_(NULL),second_(NULL),amroot_(false)
#ifdef FIM_NAMESPACES
	,Namespace(FIM_SYM_NAMESPACE_WINDOW_CHAR)
#endif
	,viewport_(NULL),
	commandConsole_(c)
	{
		/*
		 *  A new leave Window is created with a specified geometry.
		 *  An exception is launched upon memory errors.
		 */
		focus_=false;
		if(vp)
		{
			viewport_=new Viewport(*vp );
			if(viewport_)viewport_->reassignWindow(this);

		}
		else
			viewport_=new Viewport( commandConsole_,  this );

		if( viewport_ == NULL ) throw FIM_E_NO_MEM;
	}

//#ifdef FIM_UNDEFINED
	Window::Window(const Window & root):corners_(root.corners_),focus_(root.focus_),first_(root.first_),second_(root.second_),amroot_(false), viewport_(NULL),commandConsole_(root.commandConsole_)
#ifdef FIM_NAMESPACES
			,Namespace(FIM_SYM_NAMESPACE_WINDOW_CHAR)
#endif
	{
		/*
		 *  A new leave Window is created with a specified geometry.
		 *  An exception is launched upon memory errors.
		 *
		 *  Note : this method is useless, and should be kept private :D
		 */
		viewport_=new Viewport( commandConsole_, this );

		if( viewport_ == NULL ) throw FIM_E_NO_MEM;
	}
//#endif

	bool Window::issplit()const
	{
		/*
		 * return whether this window is split in some way
		 * */
		return ( first_ && second_ ) ;
	}

	bool Window::isleaf()const
	{
		/*
		 * +----------+
		 * |____|_____|
		 * |+--+|     |
		 * ||L ||     |
		 * |+--+|     |
		 * +----------+
		 *   NON LEAF
		 * +----------+
		 *
		 * +----------+
		 * |          |
		 * |  LEAF    |
		 * |          |
		 * |          |
		 * +----------+
		 */
		return ( ! first_ && ! second_ ) ;
	}

	bool Window::isvalid()const
	{	
		/*
		 * return whether this window is split right, if it is
		 * */
		return !(( first_ && ! second_ ) || ( ! first_ && second_ ) );
	}

	bool Window::ishsplit()const
	{
		/*
		 * +----------+
		 * |          |
		 * |__________|
		 * |          |
		 * |          |
		 * +----------+
		 */
		return ( issplit() && focused().corners_.x==shadowed().corners_.x ) ;
	}
	
	bool Window::isvsplit()const
	{
		/*
		 * +----------+
		 * |    |     |
		 * |    |     |
		 * |    |     |
		 * |    |     |
		 * +----------+
		 */
		return ( issplit() && focused().corners_.y==shadowed().corners_.y ) ;
	}
	
	const Window & Window::c_focused()const
	{
		/*
		 * return a const reference to the focused window
		 * throws an exception in case the window is not split!
		 * */
		if(isleaf())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;

		if(focus_==false)return first_->c_focused();
		else return second_->c_focused();
	}

	Window & Window::focused()const
	{
		/*
		 * return a reference to the focused window
		 * throws an exception in case the window is not split!
		 * */
		if(isleaf())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;

		if(focus_==false)return *first_;
		else return *second_;
	}

	Window & Window::upper()
	{
		/*
		 * return a reference to the upper window
		 * throws an exception in case the window is not split!
		 * */
		if(!ishsplit())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;
		return *first_;
	}

	Window & Window::lower()
	{
		/*
		 * return a reference to the lower window
		 * throws an exception in case the window is not split!
		 * */
		if(!ishsplit())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;
		return *second_;
	}

	Window & Window::left()
	{
		/*
		 * return a reference to the left window
		 * throws an exception in case the window is not split!
		 * */
		if(!isvsplit())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;
		return *first_;
	}

	Window & Window::right()
	{
		/*
		 * return a reference to the right window
		 * throws an exception in case the window is not split!
		 * */
		if(!isvsplit())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;
		return *second_;
	}

	Window & Window::shadowed()const
	{
		/*
		 * return a const reference to the right window
		 * throws an exception in case the window is not split!
		 * */		
		if(isleaf())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;

		if(focus_!=false)return *first_;
		else return *second_;
	}

	const Window & Window::c_shadowed()const
	{
		/*
		 * return a const reference to the shadowed window
		 * throws an exception in case the window is not split!
		 * */		
		if(isleaf())/* temporarily, for security reasons */throw FIM_E_WINDOW_ERROR;

		if(focus_!=false)return first_->c_shadowed();
		else return second_->c_shadowed();
	}

	void Window::setroot()
	{
		/*
		 * FIXME
		 * */
		amroot_=true;
	}

	void Window::split()
	{
		/*
		 * an alias for hsplit()
		 * */
		hsplit();
	}

#if 0
	void Window::print_focused()
	{
		if(isleaf())
		{
			std::cout << "F:" ;
			corners_.print();
		}
		else focused().print_focused();
	}

	void Window::print()
	{
		if(amroot_)std::cout<<"--\n";
		if(amroot_)print_focused();
		if(isleaf())std::cout<<"L:";
		corners_.print();
		if(!isleaf())first_ ->print();
		if(!isleaf())second_->print();
	}
#endif
	
	void Window::hsplit()
	{
		/*
		 * splits the window with a horizontal separator
		 * */
		if(   ! isvalid() ) return;

		/*
		 * we should check if there is still room to split ...
		 * */
		if(isleaf())
		{
			first_  = new Window( commandConsole_, this->corners_.hsplit(Rect::Upper),viewport_);
			second_ = new Window( commandConsole_, this->corners_.hsplit(Rect::Lower),viewport_);
			if(viewport_ && first_ && second_)
			{
#define FIM_COOL_WINDOWS_SPLITTING 0
#if     FIM_COOL_WINDOWS_SPLITTING
				first_ ->current_viewport().pan_up  ( second_->current_viewport().viewport_height() );
#endif
				delete viewport_;
				viewport_ = NULL;
			}
		}
		else focused().hsplit();
	}

	void Window::vsplit()
	{
		/*
		 * splits the window with a vertical separator
		 * */
		if(   !isvalid() ) return;

		/*
		 * we should check if there is still room to split ...
		 * */
		if(isleaf())
		{
			first_  = new Window( commandConsole_, this->corners_.vsplit(Rect::Left ),viewport_);
			second_ = new Window( commandConsole_, this->corners_.vsplit(Rect::Right),viewport_);
			if(viewport_ && first_ && second_)
			{
#if     FIM_COOL_WINDOWS_SPLITTING
				second_->current_viewport().pan_right( first_->current_viewport().viewport_width() );
#endif
				delete viewport_;
				viewport_ = NULL;
			}
		}
		else focused().vsplit();
	}

	bool Window::swap()
	{
		/*
		 * swap window content
		 *
		 * FIXME : unfinished
		 *
		 * +-----+-----+   +-----+-----+
		 * |     |     |   |     |     |
		 * | S   | F   |-->| F   | S   |
		 * |     |     |   |     |     |
		 * |b.jpg|a.jpg|   |a.jpg|b.jpg|
		 * +-----+-----+   +-----+-----+
		 */
		if(   !isvalid() ) return false;

		if(isleaf())
		{
			// no problem
			return false;
		}
		else if(focused().isleaf())
		{
			Viewport *vf,*vs;
			vf = focused().viewport_;
			vs = shadowed().viewport_;
			// WARNING : dangerous
			if(vf && vs)
			{
				vf ->reassignWindow(&(shadowed()));
				vs ->reassignWindow(&( focused()));
				focused().viewport_  = vs;
				shadowed().viewport_ = vf;
			}
			else
			{
				// an error should be spawned
				// FIXME
				return false;
			}
		}
		else return focused().swap();
		return true;
	}


	bool Window::close()
	{
		/*
		 * closing a leaf window implies its rejoining with the parent one
		 *
		 * FIXME : unfinished
		 *
		 * +----+-----+   +----------+
		 * |    |     |   |          |
		 * |    |     |-->|          |
		 * |    |     |   |          |
		 * |    |     |   |          |
		 * +----+-----+   +----------+
		 */
		if(   !isvalid() ) return false;

		if(isleaf())
		{
			// no problem
			return false;
		}
		else if(focused().isleaf())
		{
			/*if(ishsplit())
			this->corners_=Rect(focused().corners_.x,focused().corners_.y,shadowed().corners_.w,focused().corners_.h+shadowed().corners_.h);
			else if(isvsplit())
			this->corners_=Rect(focused().corners_.x,focused().corners_.y,shadowed().corners_.w+focused().corners_.w,shadowed().corners_.h);
			else ;//error
			*/
			/*
			 * some inheritance operations needed here!
			 */

			// WARNING : dangerous
			if(viewport_)
			{
				cout << "viewport_ should be NULL!\n";
				// an error should be spawned
			}
			if( ( viewport_ = focused().viewport_ ) )
			{
				viewport_ ->reassignWindow(this);
				focused().viewport_=NULL;
			}
			else
			{
				// error action
				return false;
			}
			delete first_;  first_  = NULL;
			delete second_; second_ = NULL;
		}
		else return focused().close();
//		print();
		return true;
	}

	void Window::balance()
	{
		/*
		 * FIXME
		 * +---+-------+   +-----+-----+
		 * |---+-------+   |     |     |
		 * |           |-->|     |     |
		 * |           |   +-----+-----+
		 * |           |   |     |     |
		 * |           |   |     |     |
		 * +-----------+   +-----+-----+
		 */
	}

	Window::Moves Window::reverseMove(Moves move)
	{
		/*
		 * returns the complementary window move
		 *
		 * ( > )^-1 = <
		 * ( < )^-1 = >
		 * ( ^ )^-1 = v
		 * ( v )^-1 = ^
		 * */
		if(move==Left )return Right;
		if(move==Right)return Left;
		if(move==Up   )return Down;
		if(move==Down )return Up;
		return move;
	}

	Window::Moves Window::move_focus(Moves move)
	{
		/*
		 * shifts the focus_ from a window to another, 
		 * unfortunately not always adjacent (a better algorithm would is needed for this)
		 *
		 * maybe more abstractions is needed here..
		 * */
		Moves m;
		if( isleaf() || move==NoMove )return NoMove;
		else
		if( isvsplit() )
		{
			if(  move != Left  &&  move != Right )
				return focused().move_focus(move);

			if( focused().isleaf() )
			{
				if( ( right() == focused() && move == Left ) || ( left() == focused() && move == Right ) )
				{
					chfocus();
					return move;
				}
				else
				{
					return NoMove;
				}
			}
			else
			{
				if((m=focused().move_focus(move)) == NoMove)
				{
					chfocus();
					focused().move_focus(reverseMove(move));
				}
				return move;
			}
		}
		else
		if( ishsplit() )
		{
			if(  move != Up  &&  move != Down )
				return focused().move_focus(move);

			if( focused().isleaf() )
			{
				if( ( upper() == focused() && move == Down ) || ( lower() == focused() && move == Up ) )
				{
					chfocus();
					return move;
				}
				else
				{
					return NoMove;
				}
			}
			else
			{
				if((m=focused().move_focus(move)) == NoMove)
				{
					chfocus();
					focused().move_focus(reverseMove(move));
				}
				return move;
			}
		}
		return move;
	}

	bool Window::chfocus()
	{
		/*
		 * this makes sense if issplit().
		 *
		 * swaps the focus_ only.
		 *
		 * +----+----+   +----+----+
		 * |    |    |   |    |    |
		 * |    |    |   |    |    |
		 * | F  | S  |-->| S  | F  |
		 * |    |    |   |    |    |
		 * +----+----+   +----+----+
		 */
		return focus_ = !focus_;
	}

	fim_coo_t Window::height()const
	{
		/*
		 * +---+ +
		 * |   | |
		 * +---+ +
		 */
		return corners_.h ;
	}

	fim_coo_t Window::setwidth(fim_coo_t w)
	{
		/*
		 * +---+
		 * +---+
		 * |   |
		 * +---+
		 */
		return corners_.w=w;
	}

	fim_coo_t Window::setheight(fim_coo_t h)
	{
		/*
		 * +---+ +
		 * |   | |
		 * +---+ +
		 */
		return corners_.h=h;
	}

	fim_coo_t Window::width()const
	{
		/*
		 * +---+
		 * +---+
		 * |   |
		 * +---+
		 */
		return corners_.w ;
	}

	fim_coo_t Window::setxorigin(fim_coo_t x)
	{
		/*
		 * o---+
		 * |   |
		 * +---+
		 */
		return corners_.x=x ;
	}

	fim_coo_t Window::setyorigin(fim_coo_t y)
	{
		/*
		 * o---+
		 * |   |
		 * +---+
		 */
		return corners_.y=y ;
	}

	fim_coo_t Window::xorigin()const
	{
		/*
		 * o---+
		 * |   |
		 * +---+
		 */
		return corners_.x ;
	}

	fim_coo_t Window::yorigin()const
	{
		/*
		 * o---+
		 * |   |
		 * +---+
		 */
		return corners_.y ;
	}

	bool Window::can_vgrow(const Window & window, fim_coo_t howmuch)
	{
		/*
		 * Assuming that the argument window is a contained one, 
		 * can this window grow the specified amount and assure the
		 * minimum spacing is respected ?
		 *
		 * +--------+
		 * +-^-+this|
		 * | ? |    |
		 * +-v-+    |
		 * +--------+
		 */
		return window.height() + howmuch + vspacing  < height();
	}

	bool Window::can_hgrow(const Window & window, fim_coo_t howmuch)
	{
		/*
		 * Assuming that the argument window is a contained one, 
		 * can this window grow the specified amount and assure the
		 * minimum spacing is respected ?
		 *
		 * +--------+
		 * +---+this|
		 * |<?>|    |
		 * +---+    |
		 * +--------+
		 */		return window.width() + howmuch + hspacing   < width();
	}


	bool Window::operator==(const Window&window)const
	{
		/*
		 * #===#
		 * #   #
		 * #===#
		 */
		return corners_==window.corners_;
	}

	int Window::count_hdivs()const
	{
		/*
		 * how many horizontal divisions ?
		 *
		 * +----------+
		 * |          |
		 * |          | hdivs = 3
		 * +----------+
		 * +----------+
		 * +----------+
		 * */
		return (isleaf()|| !ishsplit())?1: first_->count_hdivs()+ second_->count_hdivs();
	}

	int Window::count_vdivs()const
	{
		/*
		 * how many vertical divisions ?
		 * */
		return (isleaf()|| !isvsplit())?1: first_->count_vdivs()+ second_->count_vdivs();
	}

	bool Window::normalize()
	{
		/*
		 * FIXME vs balance
		 *
		 * +---+-------+   +-----+-----+
		 * |---+-------+   |     |     |
		 * |           |-->|     |     |
		 * |           |   +-----+-----+
		 * |           |   |     |     |
		 * |           |   |     |     |
		 * +-----------+   +-----+-----+
		 */
		return
//		(hnormalize(xorigin(), width() )!= -1);
		(hnormalize(xorigin(), width() )!= -1) &&
		(vnormalize(yorigin(), height())!= -1);
	}

	fim_err_t Window::vnormalize(fim_coo_t y, fim_coo_t h)
	{
		/*
		 * balances the horizontal divisions height
		 *
		 * FIXME
		 *
		 * +---+-------+   +---+-------+
		 * |---+-------+   |   |       |
		 * |           |-->|   |       |
		 * |           |   +---+-------+
		 * |           |   |   |       |
		 * |           |   |   |       |
		 * +-----------+   +---+-------+
		 */
		if(isleaf())
		{
			setyorigin(y);
			setheight(h);
			return FIM_ERR_NO_ERROR;
		}
		else
		{
			int fhdivs,shdivs,hdivs,upd;
			fhdivs=first_ ->count_hdivs();
			shdivs=second_->count_hdivs();
			hdivs=count_hdivs();
			upd=h/hdivs;
			if(hdivs>h)// no space left
				return FIM_ERR_GENERIC;
			//...
			setyorigin(y);
			setheight(h);

			if(ishsplit())
			{
				first_-> vnormalize(y,upd*fhdivs);
				second_->vnormalize(y+upd*fhdivs,h-upd*fhdivs);
			}
			else
			{
				first_-> vnormalize(y,h);
				second_->vnormalize(y,h);
			}
			return FIM_ERR_NO_ERROR;
		}
	}

	fim_err_t Window::hnormalize(fim_coo_t x, fim_coo_t w)
	{
		/*
		 * balances the vertical divisions width
		 *
		 * FIXME
		 *
		 * +---+-------+   +-----+-----+
		 * |   |       |   |     |     |
		 * |   |       |-->|     |     |
		 * |---+-------+   +-----+-----+
		 * |   |       |   |     |     |
		 * |   |       |   |     |     |
		 * +---+-------+   +-----+-----+
		 */
		if(isleaf())
		{
			setxorigin(x);
			setwidth(w);
			return FIM_ERR_NO_ERROR;
		}
		else
		{
			int fvdivs,svdivs,vdivs,upd;
			fvdivs=first_ ->count_vdivs();
			svdivs=second_->count_vdivs();
			vdivs=count_vdivs();
			upd=w/vdivs;
			if(vdivs>w)// no space left
				return FIM_ERR_GENERIC;
			//...
			setxorigin(x);
			setwidth(w);

			if(isvsplit())
			{
				first_-> hnormalize(x,upd*fvdivs);
				second_->hnormalize(x+upd*fvdivs,w-upd*fvdivs);
			}
			else
			{
				first_-> hnormalize(x,w);
				second_->hnormalize(x,w);
			}
			return FIM_ERR_NO_ERROR;
		}
	}

	fim_err_t Window::venlarge(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)
	{
#if FIM_BUGGED_ENLARGE
		return FIM_ERR_GENERIC;
#endif
		/*
		 * SEEMS BUGGY:
		 * */
		 // make && src/fim media/* -c 'split;vsplit;6henlarge;wd;7henlarge;wu;4henlarge'
		 // make && src/fim media/* -c 'split;vsplit;window "venlarge";wd; window "venlarge";'
			/*
			 * +----------+
			 * |    |     |
			 * |   >|     |
			 * | F  |  S  |
			 * |    |     |
			 * +----------+
			 */
			if( isleaf() )
			{
				if(viewport_)commandConsole_.displaydevice_->redraw_=FIM_REDRAW_NECESSARY;// no effect
				return FIM_ERR_NO_ERROR;
			}

			if(isvsplit())
			{
				/*
				 * +-+-+
				 * + | +
				 * +-+-+
				 * */
				if(focused()==left()) focused().hrgrow(units);
				if(focused()==right())focused().hlgrow(units);
				focused().normalize();  // i think there is a more elegant way to this but hmm..
				
			}
			focused().venlarge(units); //regardless the split status
			if(isvsplit())
			{
				if(focused()==left())  shadowed().hlshrink(units);
				if(focused()==right()) shadowed().hrshrink(units);
				shadowed().normalize(); 
			}
			return FIM_ERR_NO_ERROR;
	}

	fim_err_t Window::henlarge(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)
	{
		/*
		 * SEEMS BUGGY:
		 * */
		 // make && src/fim media/* -c 'split;vsplit;6henlarge;wd;7henlarge;wu;4henlarge'
#if FIM_BUGGED_ENLARGE
		return FIM_ERR_GENERIC;
#endif
			/*
			 * this operation doesn't change the outer bounds of the called window
			 *
			 * +----------+
			 * |   S      |
			 * |__________|
			 * |     ^    |
			 * |   F      |
			 * +----------+
			 */
			if( isleaf() )
			{
				if(viewport_)commandConsole_.displaydevice_->redraw_=FIM_REDRAW_NECESSARY;// no effect
				return FIM_ERR_NO_ERROR;
			}

			if(ishsplit())
			{
				/*
				 * +---+
				 * +---+
				 * +---+
				 * */
				if(focused()==upper()) focused().vlgrow(units);
				if(focused()==lower()) focused().vugrow(units);
				focused().normalize();  // i think there is a more elegant way to thism but hmm..
				
			}
			focused().henlarge(units); //regardless the split status
			if(ishsplit())
			{
				if(focused()==upper()) shadowed().vushrink(units);
				if(focused()==lower()) shadowed().vlshrink(units);

				shadowed().normalize(); 
			}
			return FIM_ERR_NO_ERROR;
	}

	fim_err_t Window::enlarge(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)
	{
		/*
		 * FIXME : ???
		 */
#if FIM_BUGGED_ENLARGE
			return FIM_ERR_GENERIC;
#endif
		/*
		 * complicato ...
		 */
//			std::cout << "enlarge\n";
			if(ishsplit() && can_vgrow(focused(),units))
			{
				return henlarge(units);
			}else
			if(isvsplit() && can_hgrow(focused(),units))
			{
				return venlarge(units);
			}else
			// isleaf()
			return FIM_ERR_NO_ERROR;
	}


	fim_err_t Window::vlgrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   {  return corners_.vlgrow(  units); } 
	fim_err_t Window::vlshrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) {  return corners_.vlshrink(units); }
	fim_err_t Window::vugrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   {  return corners_.vugrow(  units); } 
	fim_err_t Window::vushrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) {  return corners_.vushrink(units); }

	fim_err_t Window::hlgrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   {  return corners_.hlgrow(  units); } 
	fim_err_t Window::hlshrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) {  return corners_.hlshrink(units); }
	fim_err_t Window::hrgrow(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT)   {  return corners_.hrgrow(  units); } 
	fim_err_t Window::hrshrink(fim_coo_t units=FIM_CNS_WGROW_STEPS_DEFAULT) {  return corners_.hrshrink(units); }

#if 0
	void Window::draw()const
	{
		/*
		 * 
		 * */
		if(isleaf())
		{
			// we draw
			int OFF=100,K=4;
			OFF=40;
			fb_clear_rect(corners_.x+OFF, corners_.x+(corners_.w-OFF)*K, (corners_.y+OFF),(corners_.y+(corners_.h-OFF)));
		}
		else
		{
			focused().draw();
			shadowed().draw();
		}
	}
#endif

	// WARNING : SHOULD BE SURE VIEWPORT IS CORRECTLY INITIALIZED
	bool Window::recursive_redisplay()const
	{
		/*
		 * whole, deep, window redisplay
		 * */
		bool re=false;//really redisplayed ? sometimes fim guesses it is not necessary
		try
		{
		if(isleaf())
		{
			if(viewport_)re=viewport_->redisplay();
		}
		else
		{
			re |= focused().recursive_redisplay();
			re |= shadowed().recursive_redisplay();
		}
		}
		catch(FimException e)
		{
			if( e != FIM_E_WINDOW_ERROR) ;// this would be bad..
		}
		return re;
	}

	// WARNING : SHOULD BE SURE VIEWPORT IS CORRECTLY INITIALIZED
	bool Window::recursive_display()const
	{
		/*
		 * whole, deep, window display
		 * */
		bool re=false;//really displayed ? sometimes fim guesses it is not necessary
		try
		{
		if(isleaf())
		{
			if(viewport_)re=viewport_->display();
		}
		else
		{
			re |= focused().recursive_display();
			re |= shadowed().recursive_display();
		}
		}
		catch(FimException e)
		{
			if( e != FIM_E_WINDOW_ERROR) ;// this would be bad..
		}
		return re;
	}

	Viewport * Window::current_viewportp()const
	{
		/*
		 * returns a pointer to the current window's viewport_.
		 *
		 * +#===#+-----+
		 * ||   ||     |
		 * ||   ||     |
		 * ||FVP||     |
		 * ||   ||     |
		 * +#===#+-----+
		 */
		if(!isleaf()) return focused().current_viewportp();

		return viewport_;
	}	

	Viewport & Window::current_viewport()const
	{
		/*
		 * returns a reference to the current window's viewport_.
		 * throws an exception if this window is a leaf.
		 *
		 * +#===#+-----+
		 * ||   ||     |
		 * ||   ||     |
		 * ||FVP||     |
		 * ||   ||     |
		 * +#===#+-----+
		 */
		if(!isleaf()) return focused().current_viewport();

		if(!viewport_)/* temporarily, for security reasons throw FIM_E_TRAGIC*/; // isleaf()

		return *viewport_;
	}	

	const Image *Window::getImage()const
	{
		if( current_viewportp() )
			return current_viewportp()->getImage();
		else
			return NULL;
	}

	Window::~Window()
	{
		if(viewport_) delete viewport_;
		if(first_)delete first_;
		if(second_)delete second_; 
	}

	fim_err_t Window::update(const Rect& corners)
	{
		corners_=corners;
		if(viewport_ && commandConsole_.displaydevice_)commandConsole_.displaydevice_->redraw_=FIM_REDRAW_NECESSARY;// FIXME
	}
}
#if 0
/*
 *	A test main program.
 */
int main()
{
	Window w(Rect(0,0,1024,768));
	w.setroot();
	w.vsplit();
	w.hsplit();
	w.normalize();
	w.print();
	std::cout << "move_focus:\n";
	w.move_focus(Window::Down);
	w.move_focus(Window::Right);
	w.move_focus(Window::Left);
	w.move_focus(Window::Down);
	w.print();
	std::cout << "move_focus:\n";
	w.move_focus(Window::Up);
	w.print();
/*	w.enlarge();
	w.enlarge();
	w.enlarge();
	w.enlarge();
	w.enlarge();
	std::cout << "normalized:\n";

	w.print();
	std::cout << "enlarged:\n";*/
//	w.hnormalize(w.xorigin(),w.width());
	w.close();
	w.close();
	w.close();
}
#endif
#endif

