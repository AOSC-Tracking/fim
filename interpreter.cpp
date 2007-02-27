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
 */
std::ostream & operator<<(std::ostream &os,const nodeType &p)
{
	os<< "type " << p.type << "\n";
	return os;
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
			if(cc.getVariableType(p->scon.s)=='f')return (int)cc.getFloatVariable(p->scon.s);
			else return (int)cc.getIntVariable(p->scon.s);
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
		case WHILE: while(ex(p->opr.op[0]) && cc.catchInteractiveCommand()==0) {ex(p->opr.op[1]);} return 0;
		case IF: if (ex(p->opr.op[0])) ex(p->opr.op[1]);
		else if (p->opr.nops > 2) ex(p->opr.op[2]); return 0;
		case PRINT: if(p->opr.op[0]->type!=stringCon)
			    {
				//cout << "getting f " << cc.getFloatVariable(p->scon.s) << "\n";
				//cout << "getting i " << cc.getIntVariable  (p->scon.s) << "\n";
		   	        cout<<ex(p->opr.op[0])<<"\n";//printf("result : %d\n", ex(p->opr.op[0]));
		            }
			    else 
		            {
			    	cout<<p->opr.op[0]->scon.s<<"\n";//printf("result : %s\n", p->opr.op[0]->scon.s);
			    }
			    return 0;
//		case PRINT: printf("result : %d\n", ex(p->opr.op[0])); return 0;
		case ';': ex(p->opr.op[0]); return ex(p->opr.op[1]);
		case 'r': 
		  if( p->opr.nops == 2 )
		  //if( p->opr.nops ==2 && (p->opr.op[0])->type=='x')
	          {
			int times=ex(p->opr.op[1]);
			if(times<0)return -1;
			for (int i=0;i<times && cc.catchInteractiveCommand()==0;++i)
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
//			  std::cout << "args..\n";
                          np=(np->opr.op[1]); //the right subtree first node
			  while( np &&    np->opr.nops >=1 && np->opr.oper=='a')
//			  while(  ((np=(np->opr.op[1])) != NULL ) && np->opr.nops >=1 && np->opr.oper=='a')
		  	  {
				  /*
				   * we descend the right subtree  (the subtree of arguments)
				   * (thus we waste the benefit of the multi argument operator!)
				   */
//				  std::cout << *np << "\n";
//				  std::cout << "args '"<<  np->opr.op[0]->scon.s<<  "'\n";
//				  std::cout << "arg..\n";
//				  if((dp->type)==typeOpr || (dp->type)==intCon) args.push_back(ex(dp));
//				  if((dp->type)==stringCon) args.push_back(dp->scon.s);//}
				  dp=np->opr.op[0];	//we descend 1 step in the left subtree (under arg)
				  dp=dp->opr.op[0];
                          	  if(np->opr.nops < 2) 
				  {
					  np=NULL;
					  //std::cout << dp->opr.oper<< " bom..\n";
//					  cout << "nops==1  ";
			          }
				  else
				  {
					np=(np->opr.op[1]);
//				  	if((dp->type)==stringCon) args.push_back(dp->scon.s);//}
			          }
                   		  if( ((dp->opr.op[0])) && (dp->type)==stringCon)//|| (dp->type)==intCon) 
				  {	
					  args.push_back(dp->scon.s);
//					  cout <<  dp->scon.s<< " is arg!!.  ";
				  }
				  else;
/*				  if( ((dp->opr.op[0])) && (dp->type)==intCon)//|| (dp->type)==intCon) 
				  {
					  args.push_back(dp->con.value);
					  //std::cout <<   " addrgh!!.\n";
				  }
			          else
				  if((dp->type)==typeOpr && ((dp->opr.op[0])) )//|| (dp->type)==intCon) 
				  {
					  args.push_back(ex(dp));
//				          std::cout << dp->con.value<<   " argh!!.\n";
			          }*/
//			          else std::cout << " uff.\n";
//				  cout << "cycled\n";
				  assert(dp);
			  }
//			   std::cout << ".\n";
//			   cout << "gathered " << (int)(0+args.size())<< " parameters\n";
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
				//cout << "setting " << fValue << "\n"; 	//this works
				cc.setVariable(s,fValue);
//				if(cc.setVariable(s,fValue)!=fValue)exit(0);
				return (int)fValue;
			}//'i'
			else
			{
				iValue=ex(p->opr.op[1]);
				cc.setVariable(s,iValue);
				return iValue;
			}
	case UMINUS: return -ex(p->opr.op[0]); //unary minus
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
