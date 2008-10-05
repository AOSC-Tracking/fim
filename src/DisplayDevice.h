/* $Id$ */
/*
 DisplayDevice.h : virtual device Fim driver header file

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
#ifndef DISPLAY_DEVICE_FIM_H
#define DISPLAY_DEVICE_FIM_H


class DisplayDevice
{
	/*
	 * The generalization of a Fim output device.
	 */
	public:
	virtual int initialize()=0;
	virtual void  finalize()=0;

	virtual int  display(
		void *ida_image_img, // source image structure
		int iroff,int icoff, // row and column offset of the first input pixel
		int irows,int icols,// rows and columns in the input image
		int icskip,	// input columns to skip for each line
		int oroff,int ocoff,// row and column offset of the first output pixel
		int orows,int ocols,// rows and columns to draw in output buffer
		int ocskip,// output columns to skip for each line
		int flags// some flags
		)=0;

	virtual ~DisplayDevice(){}

	virtual int get_chars_per_line()=0;
	virtual int width()=0;
	virtual int height()=0;
	virtual int status_line(unsigned char *msg)=0;
	virtual int console_control(int code)=0;
	virtual int handle_console_switch()=0;
	virtual int clear_rect(int x1, int x2, int y1,int y2)=0;
	virtual int get_input(int * c);
	virtual int catchInteractiveCommand(int seconds)const;

	int redraw;
	private:
};

#endif
