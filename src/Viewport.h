/* $LastChangedDate$ */
/*
 Viewport.h : Viewport class headers

 (c) 2007-2016 Michele Martone

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


#ifndef FIM_VIEWPORT_H
#define FIM_VIEWPORT_H

#include "fim.h"
#include "FimWindow.h" // Rect
#include "DisplayDevice.h"

namespace fim
{
	/*
	 * A viewport displays one single image, so it contains the information
	 * relative to the way it is displayed.
	 * */
#ifdef FIM_NAMESPACES
class Viewport:public Namespace,public ViewportState
#else /* FIM_NAMESPACES */
class Viewport:public ViewportState
#endif /* FIM_NAMESPACES */
{
	protected:
	fim_bool_t	psteps_;
        DisplayDevice* displaydevice_;
#ifdef FIM_WINDOWS
	const Rect&corners_; // TODO: for now, no reassignCorners, but this will have to place old reassignWindow).
#endif /* FIM_WINDOWS */
	Image  *image_;	// !! 
	CommandConsole &commandConsole;
	public:
        void reset(void);
        void steps_reset(void);

	Viewport( //yes, horrible
			CommandConsole &c
			,const Rect &rect
			);

	Viewport(const Viewport &rhs);
	~Viewport(void);
	private:
	Viewport& operator= (const Viewport&rhs);
	fim_redraw_t redraw_;
	public:
	void should_redraw(enum fim_redraw_t sr = FIM_REDRAW_NECESSARY);
	fim_bool_t need_redraw(void)const;

#if 0
	int valid(void)const;
#endif

	/* viewport member functions */
	fim::string pan(const args_t &args);
	fim::string pan(const fim_char_t*a1, const fim_char_t*a2);
	bool place(const fim_pan_t px, const fim_pan_t py);
	void setState(const ViewportState & viewportState);
	void pan_up   (fim_pan_t s=0);
	void pan_down (fim_pan_t s=0);
	void pan_right(fim_pan_t s=0);
	void pan_left (fim_pan_t s=0);
	bool onBottom(void)const;
	bool onRight(void)const;
	bool onLeft(void)const;
	bool onTop(void)const;

	fim_coo_t xorigin(void);
	fim_coo_t yorigin(void);
	protected:

//	int redraw;	// there is already an external one!
	/* viewport member functions */
	public:
	fim_coo_t viewport_width(void)const;
	fim_coo_t viewport_height(void)const;
	/* viewport member functions */
	void align(const char c);

	/* viewport member functions */
	bool display(void);
	bool redisplay(void);
	void null_display(void);

	void auto_width_scale(void);
	void auto_height_scale(void);

        void setImage(fim::Image* ni);
	void scale_fix_top_left(void);
        const Image* c_getImage(void)const;
        Image* getImage(void)const;

	void auto_scale(void);
	void auto_scale_if_bigger(void);

	void free(void);
        bool check_invalid(void)const;
        bool check_valid(void)const;
#if 0
	int valid(void);
#endif
	bool scrollforward(void);
	void scale_position_magnify(fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	void scale_position_reduce(fim_scale_t factor=FIM_CNS_SCALEFACTOR);
	void recenter_horizontally(void);
	void recenter_vertically(void);
	void recenter(void);
	virtual size_t byte_size(void)const;
	int snprintf_centering_info(char *str, size_t size)const;
	void transform(bool mirror, bool flip);
	void fs_ml_puts(const char *str, fim_int doclear);
};
}
#endif /* FIM_VIEWPORT_H */
