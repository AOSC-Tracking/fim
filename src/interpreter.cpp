/* $Id$ */
/*
 interpreter.cpp : Fim language interpreter

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

#include <cstdio>
#include <map>
#include "lex.h"
//#include "y.tab.h"
#include "yacc.tab.hpp"
#include "fim.h"
#include "common.h"

/*
 *	This code implements the interpreter of the script language
 *	It is triggered by the flex and bison files.
 *
 *	Questo codice e' uno scorfano a vedersi!
 */
std::ostream & operator<<(std::ostream &os,const nodeType &p)
{
	os<< "type " << p.type << "\n";
	return os;
}

int ex(nodeType *p);

/*
 *	this function evaluates a single 'arg' entry
 */
fim::string cvar(nodeType *p)
{
	nodeType *np=p;
  	fim::string arg;
/*	if(np->opr.oper=='a' )
	{
		if(np->type == stringCon )
		{
			return (np->scon.s);
		}
	}*/
	int i;
	if(p->type == typeOpr && p->opr.oper=='.' )
	for(i=0;i<p->opr.nops;++i)
	{
		np=(p->opr.op[i]);
		arg+=cvar(np);
	}
	else
	if(p->type == typeOpr && p->opr.oper=='a' )
	for(i=0;i<p->opr.nops;++i)
	{
		//warning : only 1 should be allowed here..
		np=(p->opr.op[i]);
//		if(np->type == stringCon )arg=(np->scon.s);
//		if(np->type == typeOpr   )
//		{
			arg=cvar(np);
//		}
//		if(np->type == typeOpr   ){cout<<"A";arg=ex(np);}
//		if(np->type == intCon )arg=ex(np);
	}
	else
	if(p->type == stringCon )arg=(p->scon.s);
	else
	if(p->type == vId )
	{
		arg=cc.getStringVariable(p->scon.s);
	}
	else if(p->type == intCon )arg=ex(p);
	else
	{
		arg=ex(p);
	}
	return arg;
}

/*
 *	this function evaluates a whole chain of arg entries
 */
std::vector<fim::string> var(nodeType *p)
{
	nodeType *np=p;
  	std::vector<fim::string> args;
	int i;
	if(p->type == typeOpr && np->opr.oper=='a' )
	for(i=0;i<p->opr.nops;++i)
	{
		np=(p->opr.op[i]);
		if(np->type == stringCon )
		{
			args.push_back(np->scon.s);
		}
		else
		if(np->type == typeOpr && np->opr.oper=='a' )
		{
		  	std::vector<fim::string> vargs=var(np);
			for(unsigned int j=0;j<vargs.size();++j) args.push_back(vargs[j]);
		}
		else
		args.push_back(cvar(np));
	}
	else
	if(p->type == typeOpr && np->opr.oper=='.' )
	for(i=0;i<p->opr.nops;++i)
	{
		np=(p->opr.op[i]);
		if(np->type == typeOpr && np->opr.oper=='.' )
		{
		  	std::vector<fim::string> vargs=var(np);
			for(unsigned int j=0;j<vargs.size();++j) args.push_back(vargs[j]);
		}
	}
	else
	{
		args.push_back(cvar(np));
	}
	return args;
}

using namespace fim;
extern CommandConsole cc;
int ex(nodeType *p)
{
	int iValue;
	float fValue;
			char *s;

  	std::vector<fim::string> args;
	int typeHint;
	if (!p) return 0;
	switch(p->type)
	{
		case cmdId: assert(0);return -1;//cc.execute(fim::string::string(p->scon.s));return 0;
		case intCon: return p->con.value;
//	case floatCon: return (int)(void*)(p->fid.f);
	        case floatCon: return 0;//FIXME
	//case vId: printf("key : %s\n",p->scon.s); return vars[p->scon.s];
		case vId:
		{
			//printf("key : %s\n",p->scon.s);
			//beware the next conversion..
//			cout<<"f("<<p->scon.s<< ")="<< cc.getFloatVariable(p->scon.s) ;
		//	if(cc.getVariableType(p->scon.s)=='f')return (int)cc.getFloatVariable(p->scon.s);
		//	else return (int)cc.getIntVariable(p->scon.s);
			return (int)cc.getStringVariable(p->scon.s);
/*			if(p->typeHint=='f')
			{
				cout << "vId node f\n";
				return (int)cc.getFloatVariable(p->scon.s);
			}
			else return cc.getIntVariable(p->scon.s) ;*/
		}
		case stringCon: //printf("string(\"%s\")\n",p->scon.s);
		case typeOpr:
		switch(p->opr.oper)
		{
		case WHILE: while(ex(p->opr.op[0]) && (cc.catchLoopBreakingCommand(0)==0)) {ex(p->opr.op[1]);} return 0;
		case IF: if (ex(p->opr.op[0])) ex(p->opr.op[1]);
		else if (p->opr.nops > 2) ex(p->opr.op[2]); return 0;
/*		case PRINT: if(p->opr.op[0]->type!=stringCon)
			    {
				//cout << "getting f " << cc.getFloatVariable(p->scon.s) << "\n";
				//cout << "getting i " << cc.getIntVariable  (p->scon.s) << "\n";
		   	        cout<<ex(p->opr.op[0])<<"\n";//printf("result : %d\n", ex(p->opr.op[0]));
		            }
			    else 
		            {
		                cc.printVariable(p->opr.op[0]->scon.s);
			    	//cout<<p->opr.op[0]->scon.s<<"\n";//printf("result : %s\n", p->opr.op[0]->scon.s);
			    }
			    return 0;
//		case PRINT: printf("result : %d\n", ex(p->opr.op[0])); return 0;*/
		case ';': ex(p->opr.op[0]); return ex(p->opr.op[1]);
		case 'r': 
		  if( p->opr.nops == 2 )
		  //if( p->opr.nops ==2 && (p->opr.op[0])->type=='x')
	          {
			int times=ex(p->opr.op[1]);
			if(times<0)return -1;
			for (int i=0;i<times && cc.catchLoopBreakingCommand(0)==0;++i)
			{
				ex(p->opr.op[0]);
			}
		  	return 0;	
		  }
		  return -1;
		case 'x': 
		  /*
		   * when encountering an 'x' node, the first (left) subtree should 
		   * contain the string with the identifier of the command to 
		   * execute.
		   */
		  {
		  	if( p->opr.nops<1 )
		  {
			  return -1;
		  }
		  if(p->opr.nops==2)	//int yacc.ypp we specified only 2 ops per x node
	          {
			  nodeType *np=p;	
			  nodeType *dp;
                          np=(np->opr.op[1]); //the right subtree first node
//			  while( np &&    np->opr.nops >=1 && np->opr.oper=='a')
			  while( np &&    np->opr.nops >=1 )
			  if( np->opr.oper=='a' )
		  	  {
				  args=var(np);
				  break;
				  return 0;
				  /*
				   * we descend the right subtree  (the subtree of arguments)
				   * (thus we waste the benefit of the multi argument operator!)
				   */
				  dp=np->opr.op[0];	//we descend 1 step in the left subtree (under arg)
				  dp=dp->opr.op[0];
                          	  if(np->opr.nops < 2) 
				  {
					np=NULL;
			          }
				  else
				  {
					np=(np->opr.op[1]);
			          }
                   		  if( ((dp->opr.op[0])) && (dp->type)==stringCon)//|| (dp->type)==intCon) 
				  {	
					  //probably dead code
				  }
                   		  if( ((dp->opr.op[0])) && (dp->type)==typeOpr)//|| (dp->type)==intCon) 
				  {	
					  //probably dead code
				  }
				  else if( ((dp->opr.oper=='.')))
				  {
					  //probably dead code
			          }
				  else;
				  assert(dp);
			  }
		  	  else if( np->opr.oper=='.' )
			  {
				  //probably dead code
			  }
		  }
		  {
			fim::string result(cc.execute(fim::string::string(p->opr.op[0]->scon.s),args));//.c_str());
			cout << result;
			return atoi(result.c_str());
		  }
	}
	case 'a':
		// we shouldn't be here, because 'a' (argument) nodes are evaluated elsewhere
		assert(0);
			  return -1;
	//case '=': return sym[p->opr.op[0]->id.i] = ex(p->opr.op[1]);
	case '=':
		//assignment of a variable
			s=p->opr.op[0]->scon.s;
			typeHint=p->opr.op[0]->typeHint;
			if(typeHint=='f')
			{
				fValue=p->opr.op[1]->fid.f;
				if(fValue==0.0f)exit(0);
				cc.setVariable(s,fValue);
				return (int)fValue;
			}//'i'
			else if(typeHint=='s')
			{
			    if(p->opr.op[1]->type!=stringCon)
			    {
				//this shouldn't happen
		            }
			    else 
		            {
				// got a string!
	       		        cc.setVariable(s,p->opr.op[0]->scon.s);
		                return cc.getIntVariable(s);
			    }
			    return -1;
			}//'i'
			else if(typeHint=='i')
			{
				iValue=ex(p->opr.op[1]);
				cc.setVariable(s,iValue);
				return iValue;
			}
			else if(typeHint=='v')
			{
				//this shouldn't happen
			}
			else if(typeHint=='a')
			{
				fim::string r=cvar(p->opr.op[1]);
				iValue=r;
				cc.setVariable(s,r.c_str());
				return iValue;
			}
	case UMINUS: return -ex(p->opr.op[0]); //unary minus
	case '%': return ex(p->opr.op[0]) % ex(p->opr.op[1]);
	case '+': return ex(p->opr.op[0]) + ex(p->opr.op[1]);
	case '-': return ex(p->opr.op[0]) - ex(p->opr.op[1]);
	case '*': return ex(p->opr.op[0]) * ex(p->opr.op[1]);
	case '/': return ex(p->opr.op[0]) / ex(p->opr.op[1]);
	case '<': return ex(p->opr.op[0]) < ex(p->opr.op[1]);
	case '>': return ex(p->opr.op[0]) > ex(p->opr.op[1]);
	//comparison operators : evaluation to integer..
	case GE: return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
	case LE: return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
	case NE: return ex(p->opr.op[0]) != ex(p->opr.op[1]);
	case EQ: return ex(p->opr.op[0]) == ex(p->opr.op[1]);
	}
	}
	return 0;
}
