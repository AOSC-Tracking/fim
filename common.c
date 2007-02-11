#include "fim.h"

void trec(char *str,const char *f,const char*t)
{
	/*	this function translates escaped characters at index i in 
	 *	f into the characters at index i in f.
	 *
	 *	order is not important for the final effect.
	 * 
	 *	this function could be optimized.
	 */
	if(!str || !f || !t || strlen(f)-strlen(t))return;
	char*_p=str;
	const char *fp;
	const char *tp;
	while(*_p && _p[1])//redundant ?
	{
		fp=f;
		tp=t;
		while(*fp)
		{
			if( *_p == '\\' && *(_p+1) == *fp )
			{
				*_p = *tp;//translation	
				++_p;  //new focus
				char*pp;
				pp=_p+1;
				while(*pp){pp[-1]=*pp;++pp;}
				pp[-1]='\0';
				//if(*_p=='\\')++_p;//we want a single pass
				if(*_p)++_p;//we want a single pass
				if(!*_p)return;
				continue;
			}
			++fp;++tp;
		}
		++_p;
	} 
}


