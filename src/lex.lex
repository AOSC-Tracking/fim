%{
#include <math.h>
#include <stdio.h>

#include <stdlib.h>
#include "lex.h"
#include "yacc.tab.hpp"
#include "common.h"
void yyerror(char *);
#ifdef YY_PROTO
//this branch is taken by flex 2.5.4
//int yywrap YY_PROTO((void)){return 1;}
#else
//this branch is taken by flex 2.5.33
int yywrap (){return 1;}
#endif

int pipedesc[2];
/*#define YY_INPUT(buf,result,max_size) \
{ \
	int r=read(pipedesc[0],buf,1); \
	printf("letti in input : %d\n",r); \
	result = (buf[0]==EOF||r<1)?YY_NULL:1; \
	return; \
}*/

#define YY_INPUT(buf,result,max_size) \
{ \
	int r=read(pipedesc[0],buf,1); \
	if(0)printf("letti in input : %d(%c)\n",r,*buf); \
	result = (buf[0]==EOF||r<1)?EOB_ACT_END_OF_FILE:EOB_ACT_CONTINUE_SCAN; \
	result = (buf[0]==EOF||r<1)?0:1; \
	if(result<=0) {close(pipedesc[0]);close(pipedesc[1]);} \
	if(r==0)number_to_move == YY_MORE_ADJ; \
}


//allocate and strcpy
#define astrcpy(dst,src) \
{ \
	if((src)==NULL)yyerror("null pointer given!\n"); \
	if(((dst)=(char*)calloc(1+strlen(src),1))==NULL) \
		yyerror("out of memory\n"); \
	strcpy((dst),(src)); \
}

//quoted allocate and strcpy
#define qastrcpy(dst,src) \
{ \
	if((src)==NULL)yyerror("null pointer given!\n"); \
	(src+1)[strlen(src+1)-1]='\0'; \
	astrcpy(dst,src+1) \
}

//to lower
#define tl(src) \
{ \
	if((src)==NULL)yyerror("null pointer given!\n"); \
	{char*s=src;while(*s){*s=tolower(*s);++s;}} \
}


%}


DIGIT    [0-9]
NUMBER [0-9]+
_ID       [a-z][a-z0-9]*
UC  [A-Z]
LC  [a-z]
Q   \'
DQ  \"
C   UC|LC
CD  C|DIGIT
__ID  [a-zA-Z]CD*
ID  [a-z_A-Z][a-z_A-Z0-9]*
ARG [a-z_A-Z0-9]+
HH  [a-f]|[A-F]|[0-9]
__SYMBOL   [%:;,.\-\\$|!/(){}_]
SYMBOL [-()<>=+*\/;{}.,`:$\\^%#]
EXCLMARK !
STRINGC	 {SYMBOL}|{DIGIT}|{UC}|{LC}|[ _]
STRINGC_Q  {STRINGC}|\"
STRINGC_DQ {STRINGC}|\'


%%
"|" return SYSTEM;
"!"  return NOT;
">=" return GE;
"<=" return LE;
"==" return EQ;
"!=" return NE;
"&&" return AND;
"||" return OR;
"while" return WHILE;
"if" return IF;
"else" return ELSE;
	/*"print" return PRINT;*/
"do" return DO;


\'((\\\')|[^\'])*\' {
	trec(yytext+1,"n\\\'","\n\\\'");
	qastrcpy(yylval.sValue,yytext);;
	return STRING;
	}

\"((\\\")|[^\"])*\" {
	trec(yytext+1,"n\\\"","\n\\\"");
	qastrcpy(yylval.sValue,yytext);;
	return STRING;
	}

{ID}	{
	astrcpy(yylval.sValue,yytext);
	//tl(yylval.sValue);
	// tolower breaks aliases, but it would be useful on  keywords, above..
	return IDENTIFIER;
	}

[0-9]+	{
	yylval.iValue = atoi(yytext);
	return INTEGER;
	}

"$"	{
	yylval.iValue = -1;
	return INTEGER;
	}

"^"	{
	yylval.iValue = 0;
	return INTEGER;
	}

{DIGIT}+"."{DIGIT}* {
	//yylval.fValue = atof(yytext);
	yylval.fValue = atof(yytext);
	return FLOAT;
	}

{SYMBOL} {
	return *yytext;
	}

^"/".+  {  
	astrcpy(yylval.sValue,yytext+1);;
	return REGEXP;
	}

[ \t]+ { /* we ignore whitespace */ ; }



. printf("Unknown character :'%s'\n",yytext);yyerror("Unknown character");
	/*THERE SHOULD GO LEX ERRORS..*/

\#.*$  { /*  ... a comment encountered... */ ; }

%%

