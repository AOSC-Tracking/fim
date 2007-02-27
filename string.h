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
	void reset(){*s='\0';}
	string();
	string(const string& s);
	string(const char *str);
	string(const int &i);
	const char*c_str()const;
	bool operator==(const string& s)const;
	bool operator==(const char *  s)const;
	bool operator!=(const string& s)const;
	bool operator<=(const string& s)const;
	bool operator>=(const string& s)const;
	bool operator <(const string& s)const;
	bool operator >(const string& s)const;

	string& operator =(const string& s);
	string& operator+=(const string& s);
	string& operator+(const string& s)const;
	int  length()const;
	static int  max(){return TOKSIZE-1;}
	int  size()const;
	int  find(const string&str)const;
	int  assign(const string&str);
	int  find(const char*ss)const;
 	std::ostream& print(std::ostream &os)const;
//	int operator=(int &i,const string& s){i=-1;return i;}
	operator int()const{return atoi(s);}
	operator float()const{return atof(s);}
	};

}


#endif
