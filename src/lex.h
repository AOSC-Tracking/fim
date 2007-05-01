/* $Id$ */
/*
 lex.h : Lexer (lex) header file

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
typedef enum { intCon, floatCon, typeOpr, stringCon,cmdId,vId } nodeEnum;
/* constants */
typedef struct {
int value; /* value of constant */
} conNodeType;
/* identifiers */
typedef struct {
char *s; /* subscript to sym array */
} stringNodeType;
typedef struct {
float f; /* subscript to sym array */
} fidNodeType;
/* operators */
typedef struct {
int i; /* subscript to sym array */
} idNodeType;
/* operators */
typedef struct {
int oper; /* operator */
int nops; /* number of operands */
struct nodeTypeTag *op[1]; /* operands (expandable) */
} oprNodeType;
typedef struct nodeTypeTag {
nodeEnum type; /* type of node */
int typeHint; /* WARNING : THIS COULD BE HARMFUL, DUE TO C HACKS.. */
/* union must be last entry in nodeType */
/* because operNodeType may dynamically increase */
union {
conNodeType con; /* constants */
stringNodeType scon; /* string constant */
fidNodeType fid; /* identifiers */
idNodeType id; /* identifiers */
oprNodeType opr; /* operators */
};
} nodeType;
extern int sym[26];

