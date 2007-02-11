#ifndef VAR_FBVI_H
#define VAR_FBVI_H

#include "fim.h"
namespace fim
{
class Var
{
	union{float f;
	int i;};
	int type;
	public:
	Var::Var(const Var &v)
	{
		this->type=v.type;
		if(type=='i')this->i=v.i;
		if(type=='f')this->f=v.f;
	}
	Var::Var(const char*s="0",int type_='i')
	{
		type=type_;
		if(type=='i')i=atoi(s);
		else if(type=='f')f=atof(s);
		else i=0;
	}
/*	void Var::operator= (int   i){if(type=='i')this->i=i;}
	void Var::operator= (float f){if(type=='f')this->f=f;}*/
	void Var::operator= (int   i){type='i';this->i=i;}
	void Var::operator= (float f){setFloat(f);}
	float Var::setFloat(float f){type='f';this->f=f;}
	int   Var::setInt(int i){type='i';this->i=i;}
	int Var::getType()const{return type;}
	int Var::getInt()const{return(type=='i')?i:0;}
	float Var::getFloat()const{return(type=='f')?f:0.0f;}
	operator int()const{return getInt();}
	operator float()const{return getFloat();}
	bool Var::operator==(const Var &v)const
	{
		return (type==v.getType()) && (i==v.getInt());
	}
};
};


#endif
