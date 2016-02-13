/* $LastChangedDate: 2015-12-19 12:03:27 +0100 (Sat, 19 Dec 2015) $ */
/*
 interpreter.h : Interpreter-related declarations

 (c) 2016-2016 Michele Martone

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

#ifndef FIM_INTERPRETER_H
#define FIM_INTERPRETER_H

#define FIM_SCON(V) scon(V)
#define FIM_VSCON(V,T) vscon(V,T)
#define FIM_FCON(V) fcon(V)
#define FIM_ICON(V) con(V)
#define FIM_OPR opr
#define FIM_FREENODE freeNode

#endif /* FIM_INTERPRETER_H */
