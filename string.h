#ifndef STRING_FBVI_H
#define STRING_FBVI_H
namespace fim
{
	class string{
	/*
	 * this is a declaration of my love to the STL.. (i wrote this dumb code 
	 * in dumb 10 minutes after spending dumb hours messing with the original
	 * STL string template .. ) ( am i dumb ? ghghhh.. probably .. )
	 *
	 * FIX ME : all of this should be made dynamic, but with the right semantics.
	 */
        static const int TOKSIZE=128*8*4*2;	//max len.NUL included
	char s[TOKSIZE];
	public :
	void string::reset(){*s='\0';}
	string::string();
	string::string(const string& s);
	string::string(const char *str);
	string::string(const int &i);
	const char*string::c_str()const;
	bool operator==(const string& s)const;
	bool operator==(const char *  s)const;
	bool operator!=(const string& s)const;
	bool operator<=(const string& s)const;
	bool operator>=(const string& s)const;
	bool operator <(const string& s)const;
	bool operator >(const string& s)const;

	string& string::operator =(const string& s);
	string& string::operator+=(const string& s);
	string& string::operator+(const string& s)const;
	int  string::length()const;
	static int  string::max(){return TOKSIZE-1;}
	int  string::size()const;
	int  string::find(const string&str)const;
	int  string::assign(const string&str);
	int  string::find(const char*ss)const;
 	std::ostream& string::print(std::ostream &os)const;
//	int operator=(int &i,const string& s){i=-1;return i;}
	operator int()const{return atoi(s);}
	operator float()const{return atof(s);}
	};

};


#endif
