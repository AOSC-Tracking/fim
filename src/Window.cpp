/* $Id$ */
/*
 Window.cpp : Fim's own windowing system

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

/*
 *	This code is still experimental and programmed in great hurry.
 *	FIXME
 */


#ifdef FIM_WINDOWS
#include "fim.h"

namespace fim
{
        fim::string Window::cmd(const std::vector<fim::string> &args)
        {
		unsigned int i=0;
		while(i<args.size())
                {
			string cmd=args[i];
			if(cmd == "split" || cmd == "hsplit")
			{
				hsplit();
				return "\n";
			}
			else if(cmd == "vsplit")
			{
				vsplit();
				return "\n";
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
				enlarge(10);
				return "\n";
			}
			else if(cmd == "venlarge")
			{
				/*
				 * NOTE : this is not yet RECURSIVE !
				 * */
				venlarge(10);
				return "\n";
			}
			else if(cmd == "henlarge")
			{
				henlarge(10);
				return "\n";
			}
			else if(cmd == "up"   ) { move_focus(Up   ); }
			else if(cmd == "down" ) { move_focus(Down ); }
			else if(cmd == "left" ) { move_focus(Left ); }
			else if(cmd == "right") { move_focus(Right); }
			else if(cmd == "close")
			{
				close();
			}
			/*
			else if(cmd == "test")
			{
				fim::cout << "test ok!!\n";
				fb_clear_rect(10, 610, 100*4,100);
				return "test ok!!!\n";
			}*/
			++i;
                }
                return "";
        }

	Window::Window(const Rect& corners_):corners(corners_),focus(0),first(NULL),second(NULL),amroot(false)
	{
		/*
		 * FIXME
		 * should launch an exception here in the case!
		 */
		viewport=NULL;
		viewport=new Viewport(this);
//		if(!viewport)cc.quit(); // WARNING
	}

	Window::Window(const Window & root):corners(root.corners),focus(root.focus),first(root.first),second(root.second),amroot(false)
	{
		/*
		 * FIXME
		 * should launch an exception here in the case!
		 */
		viewport=NULL;
		viewport=new Viewport(this);
//		if(!viewport)cc.quit(); // WARNING
	}

	bool Window::issplit()const
	{
		return ( first && second ) ;
	}

	bool Window::isleaf()const
	{
		return ( ! first && ! second ) ;
	}

	bool Window::isvalid()const
	{	
		return !(( first && ! second ) || ( ! first && second ) );
	}

	bool Window::ishsplit()const
	{
		return ( issplit() && focused().corners.x==shadowed().corners.x ) ;
	}
	
	bool Window::isvsplit()const
	{
		return ( issplit() && focused().corners.y==shadowed().corners.y ) ;
	}
	
	const Window & Window::c_focused()const
	{
		if(isleaf())return (Window&)(*this);	// should launch an exception here!

		if(focus==0)return first->c_focused();
		else return second->c_focused();
	}

	Window & Window::focused()const
	{
		if(isleaf())return (Window&)(*this);	// should launch an exception here!

		if(focus==0)return *first;
		else return *second;
	}

/*	const Window & Window::c_focused()const
	{
		if(isleaf())return *this;	// should launch an exception here!

		if(focus==0)return *first;
		else return *second;
	}*/

	Window & Window::upper()
	{
		if(!ishsplit())return *this;	// should launch an exception here!
		return *first;
	}

	Window & Window::lower()
	{
		if(!ishsplit())return *this;	// should launch an exception here!
		return *second;
	}

	Window & Window::left()
	{
		if(!isvsplit())return *this;	// should launch an exception here!
		return *first;
	}

	Window & Window::right()
	{
		if(!isvsplit())return *this;	// should launch an exception here!
		return *second;
	}
/*
	const Window & Window::c_shadowed()const
	{
		if(isleaf())return *this;	// should launch an exception here!

		if(focus!=0)return *first;
		else return *second;
	}
*/
	Window & Window::shadowed()const
	{
		if(isleaf())return (Window&)(*this);	// should launch an exception here!

		if(focus!=0)return *first;
		else return *second;
	}

	const Window & Window::c_shadowed()const
	{
		if(isleaf())return (Window&)(*this);	// should launch an exception here!

		if(focus!=0)return first->c_shadowed();
		else return second->c_shadowed();
	}

	void Window::setroot()
	{
		amroot=true;
	}

	void Window::split(){hsplit();}

#if 0
	void Window::print_focused()
	{
		if(isleaf())
		{
			std::cout << "F:" ;
			corners.print();
		}
		else focused().print_focused();
	}

	void Window::print()
	{
		if(amroot)std::cout<<"--\n";
		if(amroot)print_focused();
		if(isleaf())std::cout<<"L:";
		corners.print();
		if(!isleaf())first ->print();
		if(!isleaf())second->print();
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
			first  = new Window(this->corners.hsplit(Rect::Upper));
			second = new Window(this->corners.hsplit(Rect::Lower));
			if(viewport)
			{
				delete viewport;
				viewport = NULL;
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
			first  = new Window(this->corners.vsplit(Rect::Left ));
			second = new Window(this->corners.vsplit(Rect::Right));
			if(viewport)
			{
				delete viewport;
				viewport = NULL;
			}
		}
		else focused().vsplit();
	}

	void Window::close()
	{
		/*
		 * closing a leaf window implies its rejoining with the parent one
		 *
		 * FIXME : unfinished
		 * */
		if(   !isvalid() ) return;

		if(isleaf())
		{
			// no problem
		}
		else if(focused().isleaf())
		{
			/*if(ishsplit())
			this->corners=Rect(focused().corners.x,focused().corners.y,shadowed().corners.w,focused().corners.h+shadowed().corners.h);
			else if(isvsplit())
			this->corners=Rect(focused().corners.x,focused().corners.y,shadowed().corners.w+focused().corners.w,shadowed().corners.h);
			else ;//error
			*/
			std::cout << "closing..\n";
			/*
			 * some inheritance operations needed here!
			 */

			// WARNING : dangerous
			if(viewport)
			{
				cout << "viewport should be NULL!\n";
				// an error should be spawned
			}
			if( viewport = focused().viewport )
			{
				viewport ->reassign(this);
				focused().viewport=NULL;
			}
			else
			{
				// error action
			}
			delete first;  first  = NULL;
			delete second; second = NULL;
		}
		else focused().close();
//		print();
	}

	void Window::balance()
	{
		/*
		 * FIXME
		 */
	}

	Window::Moves Window::reverseMove(Moves move)
	{
		/*
		 * returns the complementary window move
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
		 * shifts the focus from a window to another, 
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

	int Window::chfocus()
	{
		/*
		 * this makes sense if issplit()
		 * */
		return focus = ~focus;
	}

	int Window::height()const
	{
		return corners.h ;
	}

	int Window::setwidth(int w)
	{
		//!
		return corners.w=w;
	}

	int Window::setheight(int h)
	{
		//!
		return corners.h=h;
	}

	int Window::width()const
	{
		return corners.w ;
	}

	int Window::setxorigin(int x)
	{
		//!
		return corners.x=x ;
	}

	int Window::setyorigin(int y)
	{
		//!
		return corners.y=y ;
	}

	int Window::xorigin()const
	{
		return corners.x ;
	}

	int Window::yorigin()const
	{
		return corners.y ;
	}

	bool Window::can_vgrow(const Window & window, int howmuch)
	{
	//!!
		return window.height() + howmuch + vspacing  < height();
	}

	bool Window::can_hgrow(const Window & window, int howmuch)
	{
	//!!
		return window.width() + howmuch + hspacing   < width();
	}


	bool Window::operator==(const Window&window)const
	{
		return corners==window.corners;
	}

	int Window::count_hdivs()const
	{
	//!
		return (isleaf()|| !ishsplit())?1: first->count_hdivs()+ second->count_hdivs();
	}

	int Window::count_vdivs()const
	{
		return (isleaf()|| !isvsplit())?1: first->count_vdivs()+ second->count_vdivs();
	}

	int Window::normalize()
	{
	//!!
		return
//		(hnormalize(xorigin(), width())!= -1);
		(hnormalize(xorigin(), width() )!= -1) &&
		(vnormalize(yorigin(), height())!= -1);
	}

	int Window::vnormalize(int y, int h)
	{
		/*
		 * equalizes the horizontal divisions heights
		 * */
		/*
		 * FIXME
		 */
		if(isleaf())
		{
			setyorigin(y);
			setheight(h);
			return 0;
		}
		else
		{
			int fhdivs,shdivs,hdivs,upd;
			fhdivs=first ->count_hdivs();
			shdivs=second->count_hdivs();
			hdivs=count_hdivs();
			upd=h/hdivs;
			if(hdivs>h)return -1;// no space left
			//...
			setyorigin(y);
			setheight(h);

			if(ishsplit())
			{
				first-> vnormalize(y,upd*fhdivs);
				second->vnormalize(y+upd*fhdivs,h-upd*fhdivs);
			}
			else
			{
				first-> vnormalize(y,h);
				second->vnormalize(y,h);
			}
			return 0;
		}
	}

	int Window::hnormalize(int x, int w)
	{
		/*
		 * equalizes the vertical divisions widths
		 * */
		/*
		 * FIXME
		 */
		if(isleaf())
		{
			setxorigin(x);
			setwidth(w);
			return 0;
		}
		else
		{
			int fvdivs,svdivs,vdivs,upd;
			fvdivs=first ->count_vdivs();
			svdivs=second->count_vdivs();
			vdivs=count_vdivs();
			upd=w/vdivs;
			if(vdivs>w)return -1;// no space left
			//...
			setxorigin(x);
			setwidth(w);

			if(isvsplit())
			{
				first-> hnormalize(x,upd*fvdivs);
				second->hnormalize(x+upd*fvdivs,w-upd*fvdivs);
			}
			else
			{
				first-> hnormalize(x,w);
				second->hnormalize(x,w);
			}
			return 0;
		}
	}

#define FIM_BUGGED_ENLARGE 1
	int Window::venlarge(int units=1)
	{
#if FIM_BUGGED_ENLARGE
			/*
			 * +----------+
			 * |    |     |
			 * |   >|     |
			 * |    |     |
			 * |    |     |
			 * +----------+
			 */
			if( isleaf() )return 0;

			if(ishsplit())
			{
				upper().hrgrow(units);
			}
			focused() .venlarge(units); //evil
			if(ishsplit())shadowed().hlshrink(units);
			if(ishsplit())shadowed().normalize();
#endif
			return 0;
	}

	int Window::henlarge(int units=1)
	{
#if FIM_BUGGED_ENLARGE
			/*
			 * +----------+
			 * |          |
			 * |__________|
			 * |     ^    |
			 * |          |
			 * +----------+
			 */
			if( isleaf() )return 0;

			if(ishsplit())
			{
				if(focused()==upper()) focused().vlgrow(units);
				if(focused()==lower()) focused().vugrow(units);
			}
			focused().henlarge(units); //evil
			if(ishsplit())
			{
				if(focused()==upper()) shadowed().vushrink(units);
				if(focused()==lower()) shadowed().vlshrink(units);

				shadowed().normalize(); 
			}
#endif
			return 0;
	}

	int Window::enlarge(int units=1)
	{
		/*
		 * FIXME
		 */
#if FIM_BUGGED_ENLARGE
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
			return 0;
#endif
			return 0;
	}


	int Window::vlgrow(int units=1)   {  return corners.vlgrow(  units); } 
	int Window::vlshrink(int units=1) {  return corners.vlshrink(units); }
	int Window::vugrow(int units=1)   {  return corners.vugrow(  units); } 
	int Window::vushrink(int units=1) {  return corners.vushrink(units); }

	int Window::hlgrow(int units=1)   {  return corners.hlgrow(  units); } 
	int Window::hlshrink(int units=1) {  return corners.hlshrink(units); }
	int Window::hrgrow(int units=1)   {  return corners.hrgrow(  units); } 
	int Window::hrshrink(int units=1) {  return corners.hrshrink(units); }

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
			fb_clear_rect(corners.x+OFF, corners.x+(corners.w-OFF)*K, (corners.y+OFF),(corners.y+(corners.h-OFF)));
		}
		else
		{
			focused().draw();
			shadowed().draw();
		}
	}
#endif

	// WARNING : SHOULD BE SURE VIEWPORT IS CORRECTLY INITIALIZED
	void Window::recursive_redisplay()const
	{
		/*
		 * whole, deep, window redisplay
		 * */
		if(isleaf())
		{
			if(viewport)viewport->redisplay();
		}
		else
		{
			focused().recursive_redisplay();
			shadowed().recursive_redisplay();
		}
	}

	// WARNING : SHOULD BE SURE VIEWPORT IS CORRECTLY INITIALIZED
	void Window::recursive_display()const
	{
		/*
		 * whole, deep, window display
		 * */
		if(isleaf())
		{
			if(viewport)viewport->display();
		}
		else
		{
			focused().recursive_display();
			shadowed().recursive_display();
		}
	}


	Viewport & Window::current_viewport()const
	{
		/*
		 *	FIX ME
		 */
		if(!isleaf()) return focused().current_viewport();

		// isleaf
		if(!viewport)
		{
			cout << "WINDOW : NO VIeWPORT !\n";
//			cc.quit();
		}
		return *viewport;
//		return (Viewport &)(viewport);
	}	

	Image *Window::getImage()const
	{
		return current_viewport().getImage();
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

