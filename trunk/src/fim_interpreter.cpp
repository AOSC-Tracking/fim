/* $LastChangedDate$ */
/*
 interpreter.cpp : Fim language interpreter

 (c) 2007-2017 Michele Martone

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
#include "fim_interpreter.h"
#include <cstdarg>	/* va_list, va_arg, va_start, va_end */

/* TODO: need to rename the functions in here */

#if 0
#define DBG(X) std::cout<<__LINE__<<":"<<X;
#else
#define DBG(X) 
#endif

#if FIM_INDEPENDENT_NAMESPACE
#define FIM_INTERPRETER_OBSOLETE 0 /* candidates for removal */
#define FIM_NO_BREAK /* fim::cc.catchLoopBreakingCommand(0)==0 */ 1
#define FIM_OPRND(P,N) ((P)->opr.op[(N)])
#define FIM_FACC(O)  (O)->fid.f
#define FIM_SACC(O)  (O)->scon.s
#define FIM_IACC(O)  (O)->con.value
#define FIM_NOPS(O)  (O)->opr.nops
#define FIM_FOPRND(P,N) FIM_FACC(FIM_OPRND(P,N))
#define FIM_IOPRND(P,N) FIM_IACC(FIM_OPRND(P,N))
#define FIM_OPRNDO(P) (P)->opr.oper
#define FIM_OPRNDT(P) (P)->type
#define FIM_OPRNDH(P) (P)->typeHint
#define FIM_GV(V) Var((fim_int)1) /* fim::cc.getVariable(V) */
#define FIM_SV(I,V) /*fim::cc.setVariable(I,V)*/
#define FIM_GVT(V) /*fim::cc.getVariableType(V)*/FIM_SYM_TYPE_FLOAT
#define FIM_EC(CMD,ARGS) /* fim::cc.execute(CMD,ARGS) */ "result" /*Var((fim_int)1)*/ /* FIXME: shall return Arg or Var or Val */
typedef nodeType * NodeType;
#else /* FIM_INDEPENDENT_NAMESPACE */
#define FIM_INTERPRETER_OBSOLETE 0 /* candidates for removal */
#define FIM_NO_BREAK fim::cc.catchLoopBreakingCommand(0)==0
#define FIM_OPRND(P,N) ((P)->opr.op[(N)])
#define FIM_FACC(O)  (O)->fid.f
#define FIM_SACC(O)  (O)->scon.s
#define FIM_IACC(O)  (O)->con.value
#define FIM_NOPS(O)  (O)->opr.nops
#define FIM_FOPRND(P,N) FIM_FACC(FIM_OPRND(P,N))
#define FIM_IOPRND(P,N) FIM_IACC(FIM_OPRND(P,N))
#define FIM_OPRNDO(P) (P)->opr.oper
#define FIM_OPRNDT(P) (P)->type
#define FIM_OPRNDH(P) (P)->typeHint
#define FIM_GV(V) fim::cc.getVariable(V)
#define FIM_SV(I,V) fim::cc.setVariable(I,V)
#define FIM_GVT(V) fim::cc.getVariableType(V)
#define FIM_EC(CMD,ARGS) fim::cc.execute(CMD,ARGS)
typedef nodeType * NodeType;
#endif /* FIM_INDEPENDENT_NAMESPACE */

namespace fim
{
	extern CommandConsole cc;
}

/*
 *	Interpreter of fim's script language.
 *	Invoked by the flex and bison files.
 *	This code will be fully cleaned when the Fim language will settle.
 */
std::ostream& operator<<(std::ostream& os, const nodeType& p)
{
	os<< "type " << p.type << FIM_SYM_ENDL;
	return os;
}

Var ex(NodeType p); /* interpreter execution */

static Var cvar(NodeType p)
{
	/* evaluate a single 'arg' entry */
	NodeType np=p;
  	fim::string arg;
	int i;

	if(FIM_OPRNDT(p) == typeOpr && FIM_OPRNDO(p)==FIM_SYM_STRING_CONCAT)
	{
		DBG(".:"<<FIM_SYM_ENDL);
		for(i=0;i<FIM_NOPS(p);++i)
		{
			np=(FIM_OPRND(p,i));
			arg+=(string)(cvar(np).getString());
		}
		goto ret;
	}
#if FIM_INTERPRETER_OBSOLETE
	else
	if(FIM_OPRNDT(p) == typeOpr && FIM_OPRNDO(p)=='a' )
	{
		DBG("a:"<<FIM_SYM_ENDL);
		for(i=0;i<FIM_NOPS(p);++i)
		{
			//warning : only 1 should be allowed here..
			np=(FIM_OPRND(p,i));
			arg=cvar(np).getString();
		}
	}
#endif /* FIM_INTERPRETER_OBSOLETE */
	else
	if(FIM_OPRNDT(p) == stringCon )
	{
		DBG("stringCon:"<<FIM_SYM_ENDL);
		arg=(FIM_SACC(p));
		return arg;
	}
	else
	if(FIM_OPRNDT(p) == vId )
	{	
		DBG("cvId:"<<FIM_SYM_ENDL);
		return FIM_GV(FIM_SACC(p));
	}
	else if(FIM_OPRNDT(p) == intCon )
	{
		/* FIXME : int cast is due to some sick inner conversion */
		DBG("cvar:intCon:"<<FIM_IACC(p)<<FIM_SYM_ENDL);
		return Var((fim_int)(FIM_IACC(p)));
	}
	else if(FIM_OPRNDT(p) == floatCon)
	{
		DBG("cvar:floatCon:"<<FIM_FACC(p)<<FIM_SYM_ENDL);
		return FIM_FACC(p);
	}
	else
	{
		DBG("nest:\n");
		return ex(p);
	}
ret:
	return arg;
}

static args_t var(NodeType p)
{
	/* evaluate a whole chain of arg entries */
	NodeType np=p;
  	args_t args;
	int i;

	if(FIM_OPRNDT(p) == typeOpr && FIM_OPRNDO(np)=='a' )
	for(i=0;i<FIM_NOPS(p);++i)
	{
		DBG("'a'args:"<<i<<"/"<<FIM_NOPS(p)<<":\n");
		np=(FIM_OPRND(p,i));
		if(FIM_OPRNDT(np) == stringCon )
		{
			args.push_back(FIM_SACC(np));
		}
		else
		if(FIM_OPRNDT(np) == typeOpr && FIM_OPRNDO(np)=='a' )
		{
		  	args_t vargs=var(np);
			for(size_t j=0;j<vargs.size();++j)
				args.push_back(vargs[j]);
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
	if(FIM_OPRNDT(p) == typeOpr && FIM_OPRNDO(np)==FIM_SYM_STRING_CONCAT)
	for(i=0;i<FIM_NOPS(p);++i)
	{
		DBG("'i'args:"<<i<<"/"<<FIM_NOPS(p)<<":\n");
		np=(FIM_OPRND(p,i));
		if(FIM_OPRNDT(np) == typeOpr && FIM_OPRNDO(np)==FIM_SYM_STRING_CONCAT)
		{
		  	args_t vargs=var(np);
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
Var ex(NodeType p)
{
  	args_t args;
	fim_int typeHint;

	if (!p)
		goto ret;

	switch(FIM_OPRNDT(p))
	{
		case intCon:
			DBG("intCon:\n");
			return (fim_int)FIM_IACC(p);
	        case floatCon:
			DBG("ex:floatCon:"<<FIM_FACC(p)<<FIM_SYM_ENDL);
			return FIM_FACC(p);
		case vId:
		{
			DBG("vId:\n");
			// eventually may handle already here if FIM_SACC(p) is "random" ...
			if(FIM_GVT(FIM_SACC(p))==FIM_SYM_TYPE_INT)
			{
				DBG("vId:"<<FIM_SACC(p)<<":"<<(fim_int)fim::cc.getIntVariable(FIM_SACC(p))<<FIM_SYM_ENDL);
				return FIM_GV(FIM_SACC(p));
			}

			if(FIM_GVT(FIM_SACC(p))==FIM_SYM_TYPE_FLOAT)
			{
				DBG("'f':\n");
				return FIM_GV(FIM_SACC(p));
			}
			else
			{
				DBG("'s':\n");
				return FIM_GV(FIM_SACC(p));
			}
		}
		case stringCon:
			DBG("'stringCon':\n");
			// a single string token was encountered
			return Var(FIM_SACC(p));
		case typeOpr:	/*	some operator	*/
			DBG("'typeOpr':\n");
		switch(FIM_OPRNDO(p))
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
				else if (FIM_NOPS(p) > 2)
					ex(FIM_OPRND(p,2));
				goto ret;
			case FIM_SYM_SEMICOLON:
				DBG(";:\n"); // cmd;cmd
				ex(FIM_OPRND(p,0));
				return ex(FIM_OPRND(p,1));
			case 'r':
				DBG("r\n");
			if( FIM_NOPS(p) == 2 )
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
						NodeType np=p;	
						np=(FIM_OPRND(np,1)); //the right subtree first node
						while( np &&    FIM_NOPS(np) >=1 )
						if( FIM_OPRNDO(np)=='a' )
						{
					  		args_t na=var(np);
				          	for(fim_size_t i=0;i<na.size();++i)
							{
								//std::cout << "?"<<na[i]<<FIM_SYM_ENDL;
								// FIXME : non sono sicuro che questo serva
#if FIM_USE_CXX11
								args.emplace_back(std::move(na[i]));}
#else /* FIM_USE_CXX11 */
								args.push_back(na[i]);}
#endif /* FIM_USE_CXX11 */
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
	                   		  if( ((FIM_OPRND(dp,0))) && (dFIM_OPRNDT(p))==stringCon)//|| (dFIM_OPRNDT(p))==intCon) 
					  {	
						  //probably dead code
					  }
	                   		  if( ((FIM_OPRND(dp,0))) && (dFIM_OPRNDT(p))==typeOpr)//|| (dFIM_OPRNDT(p))==intCon) 
					  {	
						  //probably dead code
					  }
					  else if( ((dFIM_OPRNDO(p)==FIM_SYM_STRING_CONCAT)))
					  {
				  	//cout <<  "DEAD CODE\n";
						  //probably dead code
				          }
					  else;
					  assert(dp);
#endif
						}
						else if( FIM_OPRNDO(np)==FIM_SYM_STRING_CONCAT )
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
						//std::cout  <<"GULP:"<< FIM_SACC(FIM_OPRND(p,0))<< args[0] <<FIM_SYM_ENDL;
//						if(args.size()>0)
//							std::cout  <<"GULP:"<< (fim_int*)FIM_SACC(FIM_OPRND(p,0))<<" "<<FIM_SACC(FIM_OPRND(p,0))<<" "<<args[0] <<FIM_SYM_ENDL;
//						else
//							std::cout  <<"GULP:"<< args.size() <<FIM_SYM_ENDL;
						if(p)
							if(FIM_OPRND(p,0))
								if(FIM_SACC(FIM_OPRND(p,0)))
									result = FIM_EC(FIM_SACC(FIM_OPRND(p,0)),args);
						/* sometimes there are NULLs  : BAD !!  */
						return fim_atoi(result.c_str());
			  		}
		}
		case 'a':
			// we shouldn't be here, because 'a' (argument) nodes are evaluated elsewhere
			assert(0);
			goto err;
		case '=':
		{
			//assignment of a variable
			fim_char_t *s=FIM_NULL;
			s=FIM_SACC(FIM_OPRND(p,0));
			DBG("SV:"<<s<<FIM_SYM_ENDL)
			typeHint=FIM_OPRNDH(FIM_OPRND(p,0));
#if FIM_INTERPRETER_OBSOLETE
			if(typeHint==FIM_SYM_TYPE_FLOAT)
			{
				float fValue=FIM_FOPRND(p,1);
				DBG("SVf"<<s<<FIM_SYM_ENDL);
				FIM_SV(s,fValue);
				return (fim_int)fValue;
			}
			else if(typeHint=='s')
			{
				DBG("SVs\n");
				if(FIM_OPRNDT(FIM_OPRND(p,1))!=stringCon)
				{
					//this shouldn't happen
				}
				else 
				{
					DBG("SVs:"<<s<<":"<<FIM_SACC(FIM_OPRND(p,0))<<FIM_SYM_ENDL);
					// got a string!
		       		        FIM_SV(s,FIM_SACC(FIM_OPRND(p,0)));
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
				fim_int iValue=ex(FIM_OPRND(p,1)).getInt();
				DBG("SVi:"<<s<<":"<<iValue<<""<<FIM_SYM_ENDL);
				FIM_SV(s,iValue);
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
				FIM_SV(s,v);
			        DBG("SET:"<<s<<":"<<v.getString()<<" '"<<(fim_char_t)v.getType()<<"'\n");
			        DBG("GET:"<<s<<":"<<FIM_GV(s).getString()<<" "<<(fim_char_t)FIM_GV(s).getType()<<FIM_SYM_ENDL);
			       	return v;
			}
			else
			{
#if FIM_INTERPRETER_OBSOLETE
				DBG("SV?\n");
				goto err;
#endif /* FIM_INTERPRETER_OBSOLETE */
			}
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

#define SIZEOF_NODETYPE ((fim_char_t *)&p->con - (fim_char_t *)p)

/*
 * string constant handling
 */
nodeType *scon(fim_char_t*s)
{
	if(s==NULL)yyerror("TOKEN NULL!\n");
	nodeType *p;
	size_t nodeSize;
	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(stringNodeType);
	if ((p =(nodeType*) malloc(nodeSize)) == NULL)
		yyerror(FIM_EMSG_OUT_OF_MEM);
	/* copy information */
	p->type = stringCon; 
	p->scon.s=s;
	return p;
}

nodeType *vscon(fim_char_t*s,int typeHint)
{
	/*
	 * NOTE:
	 *
	 * The following code is much more complicated than it could be.
	 * But I like it in this way :)
	 */
#ifdef FIM_RANDOM
#ifndef FIM_BIG_ENDIAN
#if ((SIZEOF_INT)>=8)
	if( *reinterpret_cast<int*>(s+0) == 0x006d6f646e6172 ) // ..modnar
#else
	/*
	* Warning : this is LSB order, so it is not portable code.
	*/
	if( *reinterpret_cast<int*>(s+0) == 0x646e6172 // dnar
	&& (*reinterpret_cast<int*>(s+4)<<8)== 0x006d6f00    ) // .mo.
#endif
#else
	/* WARNING : UNTESTED */
#if ((SIZEOF_INT)>=8)
	if( *reinterpret_cast<int*>(s+0) == 0x72616e646f6d00 ) // random..
#else
	if( *reinterpret_cast<int*>(s+0) == 0x646e6172 // rand
	&& (*reinterpret_cast<int*>(s+4)<<8)== 0x006f6d00    ) // .om.
#endif
#endif
	return con(fim_rand());
#endif
	nodeType *p=scon(s);
	if(p)
		p->type = vId,
		p->typeHint = typeHint; 
	return p;
}


nodeType *fcon(float fValue)
{
	nodeType *p;
	size_t nodeSize;
	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(fidNodeType);
	if ((p =(nodeType*) malloc(nodeSize)) == NULL)
	yyerror(FIM_EMSG_OUT_OF_MEM);
	/* copy information */
	p->type = floatCon;
	p->fid.f = fValue;
	return p;
}



nodeType *con(fim_int value)
{
	nodeType *p;
	size_t nodeSize;
	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
	if ((p =(nodeType*) malloc(nodeSize)) == NULL)
	yyerror(FIM_EMSG_OUT_OF_MEM);
	/* copy information */
	p->type = intCon;
	p->con.value = value;
	return p;
}

nodeType *opr(int oper, int nops, ...)
{
	va_list ap;
	nodeType *p;
	size_t nodeSize;
	int i;
	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
	(nops - 1) * sizeof(nodeType*);
	if ((p =(nodeType*) malloc(nodeSize)) == NULL)
	yyerror(FIM_EMSG_OUT_OF_MEM);
	/* copy information */
	p->type = typeOpr;
	p->opr.oper = oper;
	p->opr.nops = nops;
	va_start(ap, nops);
	for (i = 0; i < nops; i++)
	p->opr.op[i] = va_arg(ap, nodeType*);
	va_end(ap);
	return p;
}

void freeNode(nodeType *p)
{
	int i;
	if (!p) return;
	if (p->type == stringCon)
		{fim_free(p->scon.s);p->scon.s=NULL;}
	if (p->type == vId)
		{fim_free(p->scon.s);p->scon.s=NULL;}
	if (p->type == typeOpr)
	{
		for (i = 0; i < p->opr.nops; i++)
		freeNode(p->opr.op[i]);
	}
	free (p);
}

void yyerror(const fim_char_t *s)
{
	//fprintf(stdout, "%s \n", s);
	/* omitting std:: causes error on darwin gcc */
	//std::cout << s << "\n";
#if FIM_INDEPENDENT_NAMESPACE
	std::cout << s << FIM_SYM_ENDL;
#else /* FIM_INDEPENDENT_NAMESPACE */
	fim::cout << s << FIM_SYM_ENDL;
#endif /* FIM_INDEPENDENT_NAMESPACE */
}



