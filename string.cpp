#include "fim.h"

namespace fim
{
	/*
	 *	FIX ME : 
	 *	this string should be dynamic as soon as possible,
	 *	without breaking the rest of the program.
	 */

	std::ostream& operator<<(std::ostream &os,const string& s)
	{
		return s.print(os);
	}

	std::ostream& operator<<(std::ostream &os, const std::vector<fim::string> & v)
	{
		std::cout<<"{";
		for(int i=0;i<v.size();++i)
			std::cout<<v[i]<<",";
		std::cout<<"}";
		return os;
	}

	std::ostream& operator<<(std::ostream &os, const Browser & b)
	{
		return b.print(os);
	}

	string::string(){ this->reset(); }
	string::string(const string& s){strcpy(this->s,s.s);}
	string::string(const char *str){strncpy(s,str,TOKSIZE-1);s[TOKSIZE-1]='\0';/*??*/}
	string::string(const int &i){sprintf(s,"%d",i);}//this works if TOKSIZE is enough
	const char*string::c_str()const{return s;}
	bool string::operator==(const string& s)const{return strcmp(this->s,s.s)==0;}
	bool string::operator==(const char *  s)const{return strcmp(this->s,  s)==0;}
	bool string::operator!=(const string& s)const{return strcmp(this->s,s.s)!=0;}
	bool string::operator<=(const string& s)const{return strcmp(this->s,s.s)<=0;}
	bool string::operator>=(const string& s)const{return strcmp(this->s,s.s)>=0;}
	bool string::operator <(const string& s)const{return strcmp(this->s,s.s) <0;}
	bool string::operator >(const string& s)const{return strcmp(this->s,s.s) >0;}

	string& string::operator =(const string& s)
	{
		*(this->s)='\0';
		int len=strlen(this->s),slen=strlen(s.s),flen=TOKSIZE-1-slen-len;
		strncat(this->s+len,s.s,flen);
		return *this;
	} 
	string& string::operator+=(const string& s)
	{
		int len=strlen(this->s),slen=strlen(s.s),flen=TOKSIZE-1-slen-len;
		strncat(this->s+len,s.s,flen);
		return *this;
	} 
	string& string::operator+(const string& s)const
	{
		return string(*this)+=s;
	} 
	int  string::length()const { return strlen(s); }
	int  string::size()const { return length(); }
	int  string::find(const string&str)const{return find(str.s);}
	int  string::assign(const string&str){strcpy(s,str.s);}
	int  string::find(const char*ss)const{const char*p=strstr(s,ss);if(!p)return -1;return p-s;}
 	std::ostream& string::print(std::ostream &os)const { return os<<s; }

};
