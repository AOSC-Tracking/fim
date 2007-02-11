#ifndef ARG_FBVI_H
#define ARG_FBVI_H
#include "fim.h"
namespace fim
{
class Arg
{
	public:
	fim::string val;
	Arg::Arg(const fim::string &s):val(s){}
	Arg::Arg(const Arg &arg):val(arg.val){}
	bool operator==(int i){int v=atoi(val.c_str());return v==i;}
	bool operator<=(int i){int v=atoi(val.c_str());return v<=i;}
	bool operator>=(int i){int v=atoi(val.c_str());return v>=i;}
	bool operator <(int i){int v=atoi(val.c_str());return v <i;}
	bool operator >(int i){int v=atoi(val.c_str());return v >i;}
};


};
#endif
