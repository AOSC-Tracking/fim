/* $Id$ */
#ifndef FBVI_STREAM_FBVI_H
#define FBVI_STREAM_FBVI_H
#include "fim.h"
namespace fim
{


	class fim_stream
//	:public std::ostream
	{/*
std::basic_ostream<char, std::char_traits<char> >::basic_ostream(const <anonymous>**)'
		      are: std::basic_ostream<char, std::char_traits<char> >::basic_ostream(const
				         std::basic_ostream<char, std::char_traits<char> >&)
		      /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.6/include/g++-v3/ostream:106: error:
		                     std::basic_ostream<_CharT,
	      _Traits>::basic_ostream(std::basic_streambuf<_CharT, _Traits>*) [with _CharT
		         = char, _Traits = std::char_traits<char>]
*/
//		std::ostream os;
		public:
	//	basic_ostream()
		fim_stream(){}
		fim_stream& operator<<(float f)
		{
			char s[32];sprintf(s,"%f",f);
			*this<<(const char*)s;
			return *this;
		}
		fim_stream& operator<<(int i)
		{
			char s[32];sprintf(s,"%d",i);
			*this<<s;
			return *this;
		}
		fim_stream& operator<<(const unsigned char* s)
		{
			*this<<(const char*)s;
			return *this;
		}
		fim_stream& operator<<(const  char* s)
		{
#ifndef FIM_NOFB
			if(s)status_screen(s,NULL);
#else
			if(s)printf("%s",s);
#endif
			return *this;
		}
		fim_stream& operator<<(const  fim::string&s)
		{
			*this<<(const  char*)(s.c_str());
			return *this;
		}
	};
}
#endif
