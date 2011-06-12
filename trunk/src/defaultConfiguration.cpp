/* $LastChangedDate$ */
/*
 defaultConfiguration.cpp :
  The following code contains key bindings and aliases definitions
  which make up a first usable configuration for FIM.
 
 (c) 2007-2011 Michele Martone

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
#if FIM_DEFAULT_CONFIG
//		alias("q",FIM_FLT_QUIT);
//		bind('f',"files");
		bind('q',FIM_FLT_QUIT);
//		bind('f',FIM_FLT_FILE);
		//bind('n',"next;display;");
		bind('n',FIM_FLT_NEXT);
//		bind('h',FIM_FLT_HELP);
//		bind('x',FIM_FLT_ECHO);
//		bind('e',FIM_FLT_ECHO);
		//bind('p',"prev;display;");
		bind('p',FIM_FLT_PREV);
		bind('P',"pop");
		bind('s',"sort");
		bind('r',FIM_FLT_RELOAD);
		bind('R',FIM_FLT_REDISPLAY);
		bind('d',FIM_FLT_DISPLAY);
		bind('+',FIM_FLT_MAGNIFY);
		bind('a',"auto_scale");
		bind('v',"toggleVerbosity");
		bind('V',"toggleKeyVerbosity");
		bind('A',"A");
		bind('m',FIM_FLT_MARK);
		bind('-',FIM_FLT_REDUCE);
//		bind(4283163,"panup");
		bind('k',"panup");
//		bind(4414235,"panright");
		bind('l',"panright");
//		bind(4348699,"pandown");
		bind('j',"pandown");
//		bind(4479771,"panleft");
		bind('h',"panleft");
		bind(' ',"scrolldown");
//		bind(2117491483,"next");
//		bind(2117425947,"prev");
		bind(29,"scale_factor_increase");
		bind(31,"scale_factor_decrease");
		bind(29,"scale_factor_grow");
		bind(31,"scale_factor_shrink");
		bind('i',FIM_FLT_INFO);
//		bind('P',"f=`files`/2;prev (--$f);\n");
//		bind('N',"f=`files`/2;next (--$f);\n");

/*		alias('A',"'autotop=1-autotop;'");
		alias("toggleVerbosity","verbose=1-verbose;display;");
		alias("toggleKeyVerbosity","_verbose_keys=1-_verbose_keys;display;");*/
	
		setVariable(FIM_VID_CONSOLE_KEY,FIM_SYM_CONSOLE_KEY);
/*		setVariable(FIM_VID_VERBOSE_KEYS,0);*/
		//setVariable(FIM_VID_MAGNIFY_FACTOR,FIM_CNS_SCALEFACTOR);
		//setVariable(FIM_VID_SCALE_FACTOR_MULTIPLIER,FIM_CNS_SCALEFACTOR_MULTIPLIER);
		//setVariable(FIM_VID_SCALE_FACTOR_DELTA,FIM_CNS_SCALEFACTOR_DELTA);
		//setVariable(FIM_VID_REDUCE_FACTOR,FIM_CNS_SCALEFACTOR);
/*		autocmd_add("Next","*","echo 'rendering next image..'");
		autocmd_add("Prev","*","echo 'rendering prev image..'");
		autocmd_add("PostNext","*","reload;");		//BUGFUL!
		autocmd_add("PostPrev","*","reload;");		//BUGFUL!
*/

//		autocmd_add("PreRedisplay","*","echo 'there should go autoscale'");		//NOT BUGFUL
//		autocmd_add("PreRedisplay","*","2magnify;");		//NOT BUGFUL!! WHY??? (8 too is ok)
//		autocmd_add("PreRedisplay","*","auto_scale_;");		//BUGFUL! (contains 1 magnify call..)
//		autocmd_add("PostReload","*","fresh=1;");		//BUGFUL! (contains 1 magnify call..)
	//	autocmd_add("PostRedisplay","*","fresh=1;");		//BUGFUL! (contains 1 magnify call..)
//		autocmd_add("PostDisplay","*","fresh=0;");		//BUGFUL! (contains 1 magnify call..)
//		autocmd_add("PreRedisplay","*","auto_scale;");		//BUGFUL!
//		autocmd_add("PostReload","*","redisplay;");		//BUGFUL!
//		autocmd_add("PostLoad","*","redisplay;");		//BUGFUL!
//		autocmd_add("PostNext","*","fresh=1;");		//BUGFUL! (contains 1 magnify call..)
//		autocmd_add("PostPrev","*","fresh=1;");		//BUGFUL! (contains 1 magnify call..)

//		autocmd_add("PostReload","*","echo 'scaled..';sleep 1;auto_scale;display;");		//BUGFUL!?! SHOULD NOT!
//		autocmd_add("PostReload","*","redisplay;");		//BUGFUL!?
//		autocmd_add("PostLoad"  ,"*","echo 'scaled..';sleep 1;auto_scale;display;");		//BUGFUL!?! SHOULD NOT!
/*		autocmd_add("PostLoad"  ,"*","fresh=1");		//BUGFUL!?! SHOULD NOT!
		autocmd_add("PostReload","*","fresh=1");		//BUGFUL!?! SHOULD NOT!
		autocmd_add("PostLoad"  ,"*","if(autowidth*fresh){auto_width_scale;}");		//BUGFUL!?! SHOULD NOT!
		autocmd_add("PostReload","*","if(autowidth*fresh){auto_width_scale;}");		//BUGFUL!?! SHOULD NOT!
		autocmd_add("PostLoad"  ,"*","fresh=0");		//BUGFUL!?! SHOULD NOT!
		autocmd_add("PostReload","*","fresh=0");		//BUGFUL!?! SHOULD NOT!
//		autocmd_add("PostLoad"  ,"*","redisplay;");		//BUGFUL!?
*/
/*		autocmd_add("PreExecutionCycle","*","reload;");	
		autocmd_add("PreExecutionCycle","*","redisplay;");	*/

//		autocmd_add("PreExecutionCycle","*","reload;auto_scale;display;");	

//		autocmd_add("PreRedisplay","*","auto_width_scale;");//BUGFUL!
//
//		autocmd_add("PostDisplay","*","auto_width_scale;");//BUGFUL!
//		autocmd_add("PostScale","*","display;");
//		autocmd_add("PostInteractiveCommand","*","display;");
//		autocmd_add("PostInteractiveCommand","*","auto_scale;");
//		autocmd_add("PostInteractiveCommand","*","redisplay;");	//necessary? altrimenti si blocca..
//

//		autocmd_add("PreExecutionCycle","*","fresh=1;");
//		autocmd_add("PreExecutionCycle","*","if(autowidth*fresh==1){auto_scale;redisplay;}");
//		autocmd_add("PreExecutionCycle","*","if(autowidth*fresh==0){redisplay;}");

		
//		autocmd_add("PreExecutionCycle","*","auto_scale;display;");
/*		autocmd_add("PreExecutionCycle","*","redisplay;");
		autocmd_add("PreExecutionCycle","*","fresh=0;");*/

/*		autocmd_add("PostInteractiveCommand","*","if(autowidth*fresh==1){auto_width_scale;redisplay;};");//ok ma non e' cio che voglio
		autocmd_add("PostInteractiveCommand","*","if(autowidth*fresh==0){redisplay;};");//ok ma non e' cio che voglio
		autocmd_add("PostInteractiveCommand","*","if(autowidth*fresh==0){redisplay;};");//ok ma non e' cio che voglio
		autocmd_add("PostInteractiveCommand","*","fresh=0;");//ok ma non e' cio che voglio
		*/
//		yyparse("next ( 4 );\n");
//		((browser)->)(*Browser::next)(std::vector)
		//SORTING IS A PROBLEM!
//		for(int i=0;i<commands.size();++i) if(commands[i]) commands[i]->execute(std::vector<Arg>());
//		init();

#endif
/*
 * The following code binds key codes to key symbols.
 * In this way the user can write a configuration file
 * and associate actions to key configurations..
 *
 * Note : certain key bindings (especially Control key with arrows 
 * or other non letter keys ) are known to be non working.
 *
 * But by modifying this file one can experiment!
 */
#if FIM_DEFAULT_KEY_CONFIG
		/*
		 * Standard key configuration
		 */
		char k[4];k[1]='\0';

		/*	97=0x61 to 122=0x7A	*/
		for(char i='a';i<='z';++i,*k=i) key_bindings_[k]=i;

		
		/*	65=0x41 to 90=0x5A (shifted keys)	*/
		for(char i='A';i<='Z';++i,*k=i) key_bindings_[k]=i;
		
		/*	32=0x20 to 65=0x40	*/
		for(char i=' ';i<='@';++i,*k=i) key_bindings_[k]=i;

		/*	91=0x5B to 96=0x60	*/
		for(char i='[';i<'a';++i,*k=i) key_bindings_[k]=i;

		/*	123=0x7B to 127=0x7F (DEL)	*/
		for(int i='{';i<=127;++i,*k=(char)i) key_bindings_[k]=(char)i;
		// A note : DEL is not captured by the above configuration.
		
		/*
		 * Control keys handling
		 */
		k[0]='C'; k[1]='-'; k[3]='\0';
		//problem : no uppercase with control.sorry.
		for(char i='b';i<='j';++i,k[2]=i) key_bindings_[k]=((i+1)-'a');
		for(char i='k';i<='z';++i,k[2]=i) key_bindings_[k]=((i+1)-'a');
		for(char i=' ';i<='?';++i,k[2]=i) key_bindings_[k]=( i+1)-'a' ;
		key_bindings_["C-a"]=1;//to circumvent a mysterious arithmetical bug..
		/*
		 * The following bindings are known to be useless or bugful.
		*/
		/*
		for(char i='[';i<'a';++i,k[2]=i) key_bindings_[k]=(i+1);
		for(int i='{';i<=127;++i,k[2]=(char)i) key_bindings_[k]=(i+1)(char)i;
		*/
		
		/*
		 * The following means that when a decimal 4479771 will be read from
		 * the keyboard, it will be interpreted as a Left pressure.
		 */
		key_bindings_[FIM_KBD_LEFT]=4479771;
		// and so on..
		key_bindings_[FIM_KBD_RIGHT]=4414235;
		key_bindings_[FIM_KBD_UP]=4283163;
		key_bindings_[FIM_KBD_DOWN]=4348699;
		key_bindings_[FIM_KBD_PAGEDOWN]=2117491483;
		key_bindings_[FIM_KBD_PAGEUP]=2117425947;
		key_bindings_[FIM_KBD_ENTER]=10;
		key_bindings_[FIM_KBD_BACKSPACE_]=127;
		key_bindings_[FIM_KBD_BACKSPACE]=127;
		key_bindings_[FIM_KBD_DEL]=2117294875;
		key_bindings_[FIM_KBD_INS]=2117229339;
		key_bindings_[FIM_KBD_HOME]=2117163803;
		key_bindings_[FIM_KBD_END] =2117360411;
		key_bindings_[FIM_KBD_TAB] =9;
		key_bindings_[FIM_KBD_ANY] =0;	/* EXPERIMENTAL */
		key_bindings_[FIM_KBD_ESC] =27;

		exitBinding_ = key_bindings_[FIM_KBD_ENTER];	/*  Enter will terminate loops */
//		key_bindings_[" "]=' ';
//		key_bindings_["."]='.';
//		key_bindings_[","]=',';
//
{
		std::map<fim::string,int>::const_iterator ki;
		for( ki=key_bindings_.begin();ki!=key_bindings_.end();++ki)
		{
			inverse_key_bindings_[(((*ki).second))]=((*ki).first);
			//inverse_key_bindings_[10]="ciao";//((*ki).first);
		}
}
#endif
