/* $LastChangedDate$ */
/*
 interpreter.cpp : Fim language interpreter

 (c) 2007-2016 Michele Martone

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <cstdio>
#include <map>
#include "lex.h"
#include "yacc.tab.hpp"
#include "fim.h"
#include "common.h"

#if 0
#define DBG(X) std::cout<<__LINE__<<":"<<X;
#else
#define DBG(X) 
#endif

#define FIM_INTERPRETER_OBSOLETE 0 /* candidates for removal */
#define FIM_NO_BREAK fim::cc.catchLoopBreakingCommand(0)==0
#define FIM_OPRND(P,N) ((P)->opr.op[(N)])
#define FIM_FACC(O,N)  (O)->fid.f
#define FIM_IACC(O)  (O)->con.value
#define FIM_NOPS(O)  (O)->opr.nops
#define FIM_FOPRND(P,N) FIM_FACC(FIM_OPRND(P,N))
#define FIM_IOPRND(P,N) FIM_IACC(FIM_OPRND(P,N))

namespace fim
{
	extern CommandConsole cc;
}

/*
 *	Interpreter of fim's script language.
 *	Invoked by the flex and bison files.
 *	This code will be fully cleaned when the Fim language will settle.
 */
std::ostream & operator<<(std::ostream &os,const nodeType &p)
{
	os<< "type " << p.type << FIM_SYM_ENDL;
	return os;
}

Var ex(nodeType *p); /* interpreter execution */

static Var cvar(nodeType *p)
{
	/* evaluate a single 'arg' entry */
	nodeType *np=p;
  	fim::string arg;
	int i;

	if(p->type == typeOpr && p->opr.oper==FIM_SYM_STRING_CONCAT)
	{
		DBG(".:"<<FIM_SYM_ENDL);
		for(i=0;i<FIM_NOPS(p);++i)
		{
			np=(p->opr.op[i]);
			arg+=(string)(cvar(np).getString());
		}
		goto ret;
	}
#if FIM_INTERPRETER_OBSOLETE
	else
	if(p->type == typeOpr && p->opr.oper=='a' )
	{
		DBG("a:"<<FIM_SYM_ENDL);
		for(i=0;i<p->opr.nops;++i)
		{
			//warning : only 1 should be allowed here..
			np=(p->opr.op[i]);
			arg=cvar(np).getString();
		}
	}
#endif /* FIM_INTERPRETER_OBSOLETE */
	else
	if(p->type == stringCon )
	{
		DBG("stringCon:"<<FIM_SYM_ENDL);
		arg=(p->scon.s);
		return arg;
	}
	else
	if(p->type == vId )
	{	
		DBG("cvId:"<<FIM_SYM_ENDL);
		return fim::cc.getVariable(p->scon.s);
	}
	else if(p->type == intCon )
	{
		/* FIXME : int cast is due to some sick inner conversion */
		DBG("cvar:intCon:"<<FIM_IACC(p)<<FIM_SYM_ENDL);
		return Var((fim_int)(FIM_IACC(p)));
	}
	else if(p->type == floatCon)
	{
		DBG("cvar:floatCon:"<<p->fid.f<<FIM_SYM_ENDL);
		return p->fid.f;
	}
	else
	{
		DBG("nest:\n");
		return ex(p);
	}
ret:
	return arg;
}

static std::vector<fim::string> var(nodeType *p)
{
	/* evaluate a whole chain of arg entries */
	nodeType *np=p;
  	std::vector<fim::string> args;
	int i;

	if(p->type == typeOpr && np->opr.oper=='a' )
	for(i=0;i<p->opr.nops;++i)
	{
		DBG("'a'args:"<<i<<"/"<<p->opr.nops<<":\n");
		np=(p->opr.op[i]);
		if(np->type == stringCon )
		{
			args.push_back(np->scon.s);
		}
		else
		if(np->type == typeOpr && np->opr.oper=='a' )
		{
		  	std::vector<fim::string> vargs=var(np);
			for(size_t j=0;j<vargs.size();++j) args.push_back(vargs[j]);
		}
		else
		{
			DBG("CVARB\n");
			args.push_back(cvar(np).getString());
			DBG("CVARA\n");
		}
	}
#if FIM_INTERPRETER_OBSOLETE
	else
	if(p->type == typeOpr && np->opr.oper==FIM_SYM_STRING_CONCAT)
	for(i=0;i<p->opr.nops;++i)
	{
		DBG("'i'args:"<<i<<"/"<<p->opr.nops<<":\n");
		np=(p->opr.op[i]);
		if(np->type == typeOpr && np->opr.oper==FIM_SYM_STRING_CONCAT)
		{
		  	std::vector<fim::string> vargs=var(np);
			for(size_t j=0;j<vargs.size();++j) args.push_back(vargs[j]);
		}
	}
	else
	{
		DBG("~:\n");
		args.push_back(cvar(np).getString());
	}
#endif /* FIM_INTERPRETER_OBSOLETE */
	DBG("?:\n");
	return args;
}

using namespace fim;
Var ex(nodeType *p)
{
	fim_int iValue;
	float fValue;
	fim_char_t *s=FIM_NULL;
  	std::vector<fim::string> args;
	fim_int typeHint;

	if (!p)
		goto ret;

	switch(p->type)
	{
		case intCon:
			DBG("intCon:\n");
			return (fim_int)FIM_IACC(p);
	        case floatCon:
			DBG("ex:floatCon:"<<p->fid.f<<FIM_SYM_ENDL);
			return p->fid.f;
		case vId:
		{
			DBG("vId:\n");
			// eventually may handle already here if p->scon.s is "random" ...
			if(fim::cc.getVariableType(p->scon.s)==FIM_SYM_TYPE_INT)
			{
				DBG("vId:"<<p->scon.s<<":"<<(fim_int)fim::cc.getIntVariable(p->scon.s)<<FIM_SYM_ENDL);
				return fim::cc.getVariable(p->scon.s);
			}

			if(fim::cc.getVariableType(p->scon.s)==FIM_SYM_TYPE_FLOAT)
			{
				DBG("'f':\n");
				return fim::cc.getVariable(p->scon.s);
			}
			else
			{
				DBG("'s':\n");
				return fim::cc.getVariable(p->scon.s);
			}
		}
		case stringCon:
			DBG("'stringCon':\n");
			// a single string token was encountered
			return Var(p->scon.s);
		case typeOpr:	/*	some operator	*/
			DBG("'typeOpr':\n");
		switch(p->opr.oper)
		{
#if FIM_INTERPRETER_OBSOLETE
			case FIM_SYM_STRING_CONCAT:
				DBG(".:\n");
				return (ex(FIM_OPRND(p,0)) + ex(FIM_OPRND(p,1))); // TODO: the , operator
#endif /* FIM_INTERPRETER_OBSOLETE */
			case WHILE:
				while(ex(FIM_OPRND(p,0)).getInt() && FIM_NO_BREAK )
					ex(FIM_OPRND(p,1));
				goto ret;
			case IF:
				DBG("IF:"<<(ex(FIM_OPRND(p,0)).getInt())<<FIM_SYM_ENDL);
				if (ex(FIM_OPRND(p,0)).getInt())
					ex(FIM_OPRND(p,1));
				else if (p->opr.nops > 2)
					ex(FIM_OPRND(p,2));
				goto ret;
			case FIM_SYM_SEMICOLON:
				DBG(";:\n"); // cmd;cmd
				ex(FIM_OPRND(p,0));
				return ex(FIM_OPRND(p,1));
			case 'r':
				DBG("r\n");
			if( p->opr.nops == 2 )
			{
				fim_int times=ex(FIM_OPRND(p,1)).getInt();
				if(times<0)
					goto err;
				for (fim_int i=0;i<times && FIM_NO_BREAK ;++i)
					ex(FIM_OPRND(p,0));
				goto ret;
			}
			else
				goto err;
			case 'x': 
				DBG("X\n");
			  /*
			   * when encountering an 'x' node, the first (left) subtree should 
			   * contain the string with the identifier of the command to 
			   * execute.
			   */
			  {
			  	if( FIM_NOPS(p) < 1 )
			  	{
					DBG("INTERNAL ERROR\n");
					goto err;
				}
			  if(FIM_NOPS(p)==2)	//int yacc.ypp we specified only 2 ops per x node
		          {
				  nodeType *np=p;	
				  //nodeType *dp;
	                          np=(FIM_OPRND(np,1)); //the right subtree first node
				  while( np &&    FIM_NOPS(np) >=1 )
				  if( np->opr.oper=='a' )
			  	  {
					  std::vector<fim::string> na;
					  na=var(np);
				          for(fim_size_t i=0;i<na.size();++i)
                                          {
						//std::cout << "?"<<na[i]<<FIM_SYM_ENDL;
						args.push_back(na[i]);}// FIXME : non sono sicuro che questo serva
					  	DBG("A:"<<FIM_NOPS(np)<<FIM_SYM_ENDL);
						break;
#if 0
					  return 0;
					  /*
					   * we descend the right subtree  (the subtree of arguments)
					   * (thus we waste the benefit of the multi argument operator!)
					   */
					  dp=FIM_OPRND(np,0);	//we descend 1 step in the left subtree (under arg)
					  dp=FIM_OPRND(dp,0);
	                          	  if(FIM_NOPS(np) < 2) 
					  {
						np=FIM_NULL;
				          }
					  else
					  {
						np=(FIM_OPRND(np,1));
				          }
	                   		  if( ((FIM_OPRND(dp,0))) && (dp->type)==stringCon)//|| (dp->type)==intCon) 
					  {	
						  //probably dead code
					  }
	                   		  if( ((FIM_OPRND(dp,0))) && (dp->type)==typeOpr)//|| (dp->type)==intCon) 
					  {	
						  //probably dead code
					  }
					  else if( ((dp->opr.oper==FIM_SYM_STRING_CONCAT)))
					  {
				  	//cout <<  "DEAD CODE\n";
						  //probably dead code
				          }
					  else;
					  assert(dp);
#endif
				  }
			  	  else if( np->opr.oper==FIM_SYM_STRING_CONCAT )
				  {
					//cout <<  "DEAD CODE\n";
					//probably dead code
				  }
			  }
			  {
				DBG("A.\n");
				/*
				 * single command execution
				 */
				fim::string result;
				//std::cout  <<"GULP:"<< FIM_OPRND(p,0)->scon.s<< args[0] <<FIM_SYM_ENDL;
//				if(args.size()>0)
//					std::cout  <<"GULP:"<< (fim_int*)FIM_OPRND(p,0)->scon.s<<" "<<FIM_OPRND(p,0)->scon.s<<" "<<args[0] <<FIM_SYM_ENDL;
//				else
//					std::cout  <<"GULP:"<< args.size() <<FIM_SYM_ENDL;
				if(p)
				if(FIM_OPRND(p,0))
				if(FIM_OPRND(p,0)->scon.s) result =
				       	fim::cc.execute(FIM_OPRND(p,0)->scon.s,args);
				/* sometimes there are NULLs  : BAD !!  */
				return fim_atoi(result.c_str());
			  }
		}
		case 'a':
			// we shouldn't be here, because 'a' (argument) nodes are evaluated elsewhere
			assert(0);
			goto err;
		case '=':
			//assignment of a variable
			s=FIM_OPRND(p,0)->scon.s;
			DBG("SV:"<<s<<FIM_SYM_ENDL)
			typeHint=FIM_OPRND(p,0)->typeHint;
#if FIM_INTERPRETER_OBSOLETE
			if(typeHint==FIM_SYM_TYPE_FLOAT)
			{
				DBG("SVf"<<s<<FIM_SYM_ENDL);
				fValue=FIM_FOPRND(p,1);
				fim::cc.setVariable(s,fValue);
				return (fim_int)fValue;
			}
			else if(typeHint=='s')
			{
				DBG("SVs\n");
				if(FIM_OPRND(p,1)->type!=stringCon)
				{
					//this shouldn't happen
				}
				else 
				{
					DBG("SVs:"<<s<<":"<<FIM_OPRND(p,0)->scon.s<<FIM_SYM_ENDL);
					// got a string!
		       		        fim::cc.setVariable(s,FIM_OPRND(p,0)->scon.s);
#if 0
					if(0 && 0==strcmp(s,"random"))
			                	return fim_rand();//FIXME
					else
#endif
			                	//return fim::cc.getIntVariable(s);
			                	return fim::cc.getStringVariable(s);
				}
				goto err;
			}//FIM_SYM_TYPE_INT
			else if(typeHint==FIM_SYM_TYPE_INT)
			{
				iValue=ex(FIM_OPRND(p,1)).getInt();
				DBG("SVi:"<<s<<":"<<iValue<<""<<FIM_SYM_ENDL);
				fim::cc.setVariable(s,iValue);
				return iValue;
			}
			else if(typeHint=='v')
			{
				DBG("SVv\n");
				//this shouldn't happen
			}
			else
#endif /* FIM_INTERPRETER_OBSOLETE */
			if(typeHint=='a')
			{
				DBG("SVa\n");
				Var v=cvar(FIM_OPRND(p,1));
				fim::cc.setVariable(s,v);
			        DBG("SET:"<<s<<":"<<v.getString()<<" '"<<(fim_char_t)v.getType()<<"'\n");
			        DBG("GET:"<<s<<":"<<fim::cc.getVariable(s).getString()<<" "<<(fim_char_t)fim::cc.getVariable(s).getType()<<FIM_SYM_ENDL);
			       	return v;
			}
			else
			{
#if FIM_INTERPRETER_OBSOLETE
				DBG("SV?\n");
				goto err;
#endif /* FIM_INTERPRETER_OBSOLETE */
			}
#if FIM_WANT_AVOID_FP_EXCEPTIONS
			case '%': {Var v1=ex(FIM_OPRND(p,0)),v2=ex(FIM_OPRND(p,1)); if(v2.getInt())return v1%v2; else return v2;};
			case '/': {Var v1=ex(FIM_OPRND(p,0)),v2=ex(FIM_OPRND(p,1)); if(v2.getInt())return v1/v2; else return v2;};
#else /* FIM_WANT_AVOID_FP_EXCEPTIONS */
			case '%': return ex(FIM_OPRND(p,0)) % ex(FIM_OPRND(p,1)); // FIXME: may generate an exception
			case '/': return ex(FIM_OPRND(p,0)) / ex(FIM_OPRND(p,1)); // FIXME: may generate an exception
#endif /* FIM_WANT_AVOID_FP_EXCEPTIONS */
			case '+': return ex(FIM_OPRND(p,0)) + ex(FIM_OPRND(p,1));
			case '!': return (fim_int)(((ex(FIM_OPRND(p,0))).getInt())==0?1:0);
			case UMINUS: return Var((fim_int)0) - ex(FIM_OPRND(p,0));
			case '-': 
				DBG("SUB\n");
				if ( 2==FIM_NOPS(p) ) {Var d= ex(FIM_OPRND(p,0)) - ex(FIM_OPRND(p,1));return d;}
				else return Var((fim_int)0) - ex(FIM_OPRND(p,0));
			case '*': return ex(FIM_OPRND(p,0)) * ex(FIM_OPRND(p,1));
			case '<': return ex(FIM_OPRND(p,0)) < ex(FIM_OPRND(p,1));
			case '>': return ex(FIM_OPRND(p,0)) > ex(FIM_OPRND(p,1));
			//comparison operators : evaluation to integer..
			case GE: return ex(FIM_OPRND(p,0)) >= ex(FIM_OPRND(p,1));
			case LE: return ex(FIM_OPRND(p,0)) <= ex(FIM_OPRND(p,1));
			case NE: return ex(FIM_OPRND(p,0)) != ex(FIM_OPRND(p,1));
			case EQ: {DBG("EQ\n");return ex(FIM_OPRND(p,0)) == ex(FIM_OPRND(p,1));}
			case REGEXP_MATCH: return ex(FIM_OPRND(p,0)).re_match(ex(FIM_OPRND(p,1)));
			case AND:return ex(FIM_OPRND(p,0)) && ex(FIM_OPRND(p,1));
			case OR :return ex(FIM_OPRND(p,0)) || ex(FIM_OPRND(p,1));
			case BOR :return ex(FIM_OPRND(p,0)) | ex(FIM_OPRND(p,1));
			case BAND:return ex(FIM_OPRND(p,0)) & ex(FIM_OPRND(p,1));
			default: goto err;
		}
	}
ret:
	return Var((fim_int)0);
err:
	return Var((fim_int)-1);
}
