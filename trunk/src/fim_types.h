/* $LastChangedDate$ */
/*
 fim_types.h : Basic Fim type declarations

 (c) 2011-2011 Michele Martone

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

/* This file will grow, as definitions will be moved there from fim.h */

#ifndef FIM_TYPES_FIM_H
#define FIM_TYPES_FIM_H

namespace fim
{
	typedef int fim_pan_t;		/* a type for pixel offsets (neg/pos)  */
	typedef int fim_off_t;		/* a type for pixel offsets (positive)  */
	typedef float fim_scale_t;	/* a type for image scaling */
	typedef float fim_angle_t;	/* a type for angles */
	typedef float fim_float_t;	/* a type for floats */
	typedef int   fim_page_t;	/* a type for multipage document pages */
	typedef int   fim_pgor_t;	/* a type for page orientation */
	typedef bool   fim_bool_t;	/* a type for bolean expressions */
	typedef int fim_coo_t;		/* a type for coordinates */
	typedef int fim_cc_t;		/* a type for console control */
	typedef int fim_flags_t;	/* a type for display flags */
	typedef int fim_bpp_t;		/* a type for bits Per Pixel */
	typedef int fim_key_t;		/* a type for keycodes */
	typedef int fim_err_t;		/* a type for errors */
	typedef int fim_perr_t;		/* a type for program errors */
	typedef int fim_status_t;	/* a type for fim's status */
	typedef int fim_cycles_t;	/* a type for fim's cycles */
	typedef int fim_cmd_type_t;	/* a type for fim's command types */
	typedef int fim_var_t;		/* a type for fim's variable types */
	typedef int fim_int;		/* a type for fim's internal integer type (TODO: shall extend its use!) */

	typedef int fim_ts_t;		/* a type for time, in seconds */
	typedef int fim_tms_t;		/* a type for time, in milliseconds */
	typedef double fim_fms_t;		/* a type for time, in milliseconds, floating point */
	typedef unsigned long fim_tus_t;	/* a type for time, in microseconds */
	typedef char fim_char_t;	/* a type for chars */
	typedef unsigned char fim_byte_t;	/* a type for bytes */
	typedef size_t fim_size_t;	/* a type for file size */
}

/* we wait for variadic macros support in standard C++ */
#define FIM_FPRINTF fprintf

#endif
