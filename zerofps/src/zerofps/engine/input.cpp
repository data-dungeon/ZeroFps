#include "input.h"


Input::Input() 
 : ZFObject("Input") {
	cout<<"Sdl_Input initializing"<<endl;	
	
	/*
	//init sdl to read events
	if(SDL_Init(SDL_INIT_EVENTTHREAD)<0){
		cout<<"Sdl_Input dont want to work =("<<endl;
		exit(1);
	}*/
	
	//reset all buttons to false
	//for(int i =0;i<400;i++) 
	//	m_akButtonList[i]=false;

	m_bKeyRepeat = true;
	
	m_iQueueLength=100;
	m_fMouseSensitivity=1;	
	m_iGrabtime=SDL_GetTicks();
	m_bInputEnabled=true;

	g_ZFObjSys.Register_Cmd("togglegrab",FID_TOGGLEGRAB,this);
	g_ZFObjSys.Register_Cmd("bind",FID_BIND,this);
	g_ZFObjSys.Register_Cmd("unbindall",FID_UNBINDALL,this);
	g_ZFObjSys.Register_Cmd("listactions",FID_LISTACTIONS,this);		
	g_ZFObjSys.Register_Cmd("mousesens",FID_MOUSESENS,this);		

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	
	
	m_iNrActions=0;
	bool m_bInputEnabled=true;
	for(int i =0;i<400;i++) 
	{
		m_akButtonList[i]=false;
		m_aiActionToButton[i]=NULL;
	};
	
	
//	m_kActions.insert(map<const char*, int>::value_type("attack",ATTACK));
//	m_kActions.insert(map<const char*, int>::value_type("forward",FORWARD));
//	m_kActions.insert(map<const char*, int>::value_type("jump",JUMP));

	//m_kButtons.insert(map<char*, int>::value_type("key1",1));
	//m_kButtons.insert(map<char*, int>::value_type("key2",2));
	//m_kButtons.insert(map<char*, int>::value_type("key3",3));
	m_kButtons.insert(map<char*, int>::value_type("escape",SDLK_ESCAPE));
	m_kButtons.insert(map<char*, int>::value_type("left",SDLK_LEFT));
	m_kButtons.insert(map<char*, int>::value_type("right",SDLK_RIGHT));
	m_kButtons.insert(map<char*, int>::value_type("up",SDLK_UP));
	m_kButtons.insert(map<char*, int>::value_type("down",SDLK_DOWN));
	m_kButtons.insert(map<char*, int>::value_type("space",SDLK_SPACE));
	m_kButtons.insert(map<char*, int>::value_type("backspace",SDLK_BACKSPACE));
	m_kButtons.insert(map<char*, int>::value_type("return",SDLK_RETURN));
	
	m_kButtons.insert(map<char*, int>::value_type("rshift",SDLK_RSHIFT));
	m_kButtons.insert(map<char*, int>::value_type("rctrl",SDLK_RCTRL));
	m_kButtons.insert(map<char*, int>::value_type("lshift",SDLK_LSHIFT));
	m_kButtons.insert(map<char*, int>::value_type("lctrl",SDLK_LCTRL));
	
	m_kButtons.insert(map<char*, int>::value_type("f12",SDLK_F12));
	m_kButtons.insert(map<char*, int>::value_type("f11",SDLK_F11));
	m_kButtons.insert(map<char*, int>::value_type("f10",SDLK_F10));
	m_kButtons.insert(map<char*, int>::value_type("f9",SDLK_F9));	
	m_kButtons.insert(map<char*, int>::value_type("f8",SDLK_F8));	
	m_kButtons.insert(map<char*, int>::value_type("f7",SDLK_F7));	
	m_kButtons.insert(map<char*, int>::value_type("f6",SDLK_F6));
	m_kButtons.insert(map<char*, int>::value_type("f5",SDLK_F5));	
	m_kButtons.insert(map<char*, int>::value_type("f4",SDLK_F4));	
	m_kButtons.insert(map<char*, int>::value_type("f3",SDLK_F3));	
	m_kButtons.insert(map<char*, int>::value_type("f2",SDLK_F2));
	m_kButtons.insert(map<char*, int>::value_type("f1",SDLK_F1));
	
	m_kButtons.insert(map<char*, int>::value_type("pageup",SDLK_PAGEUP));
	m_kButtons.insert(map<char*, int>::value_type("pagedown",SDLK_PAGEDOWN));
	m_kButtons.insert(map<char*, int>::value_type("home",SDLK_HOME));
	m_kButtons.insert(map<char*, int>::value_type("end",SDLK_END));
	m_kButtons.insert(map<char*, int>::value_type("tab",SDLK_TAB));
	m_kButtons.insert(map<char*, int>::value_type("key_delete",SDLK_DELETE));
	m_kButtons.insert(map<char*, int>::value_type("insert",SDLK_INSERT));	
	
	m_kButtons.insert(map<char*, int>::value_type("1",SDLK_1));
	m_kButtons.insert(map<char*, int>::value_type("2",SDLK_2));
	m_kButtons.insert(map<char*, int>::value_type("3",SDLK_3));
	m_kButtons.insert(map<char*, int>::value_type("4",SDLK_4));
	m_kButtons.insert(map<char*, int>::value_type("5",SDLK_5));
	m_kButtons.insert(map<char*, int>::value_type("6",SDLK_6));
	m_kButtons.insert(map<char*, int>::value_type("7",SDLK_7));
	m_kButtons.insert(map<char*, int>::value_type("8",SDLK_8));
	m_kButtons.insert(map<char*, int>::value_type("9",SDLK_9));
	m_kButtons.insert(map<char*, int>::value_type("0",SDLK_0));
	
	m_kButtons.insert(map<char*, int>::value_type("q",SDLK_q));
	m_kButtons.insert(map<char*, int>::value_type("w",SDLK_w));	
	m_kButtons.insert(map<char*, int>::value_type("e",SDLK_e));	
	
	m_kButtons.insert(map<char*, int>::value_type("r",SDLK_r));	
	m_kButtons.insert(map<char*, int>::value_type("t",SDLK_t));	
	m_kButtons.insert(map<char*, int>::value_type("y",SDLK_y));	
	m_kButtons.insert(map<char*, int>::value_type("u",SDLK_u));	
	m_kButtons.insert(map<char*, int>::value_type("i",SDLK_i));	
	m_kButtons.insert(map<char*, int>::value_type("o",SDLK_o));	
	m_kButtons.insert(map<char*, int>::value_type("p",SDLK_p));	

	m_kButtons.insert(map<char*, int>::value_type("a",SDLK_a));	
	m_kButtons.insert(map<char*, int>::value_type("s",SDLK_s));	
	m_kButtons.insert(map<char*, int>::value_type("d",SDLK_d));	
	m_kButtons.insert(map<char*, int>::value_type("f",SDLK_f));	
	m_kButtons.insert(map<char*, int>::value_type("g",SDLK_g));	
	m_kButtons.insert(map<char*, int>::value_type("h",SDLK_h));	
	m_kButtons.insert(map<char*, int>::value_type("j",SDLK_j));	
	m_kButtons.insert(map<char*, int>::value_type("k",SDLK_k));	
	m_kButtons.insert(map<char*, int>::value_type("l",SDLK_l));
	m_kButtons.insert(map<char*, int>::value_type("z",SDLK_z));	
	m_kButtons.insert(map<char*, int>::value_type("x",SDLK_x));	
	m_kButtons.insert(map<char*, int>::value_type("c",SDLK_c));	
	m_kButtons.insert(map<char*, int>::value_type("v",SDLK_v));	
	m_kButtons.insert(map<char*, int>::value_type("b",SDLK_b));	
	m_kButtons.insert(map<char*, int>::value_type("n",SDLK_n));	
	m_kButtons.insert(map<char*, int>::value_type("m",SDLK_m));
	
	m_kButtons.insert(map<char*,int>::value_type("exclaim",SDLK_EXCLAIM));
	m_kButtons.insert(map<char*,int>::value_type("quotedbl",SDLK_QUOTEDBL));
	m_kButtons.insert(map<char*,int>::value_type("hash",SDLK_HASH));
	m_kButtons.insert(map<char*,int>::value_type("dollar",SDLK_DOLLAR));
	m_kButtons.insert(map<char*,int>::value_type("ampersand",SDLK_AMPERSAND));
	m_kButtons.insert(map<char*,int>::value_type("quote",SDLK_QUOTE));
	m_kButtons.insert(map<char*,int>::value_type("left_parenthesis",SDLK_LEFTPAREN));
	m_kButtons.insert(map<char*,int>::value_type("right_parenthesis",SDLK_RIGHTPAREN));
	m_kButtons.insert(map<char*,int>::value_type("asterisk",SDLK_ASTERISK));
	m_kButtons.insert(map<char*,int>::value_type("plus_sign",SDLK_PLUS));
	m_kButtons.insert(map<char*,int>::value_type("comma",SDLK_COMMA));
	m_kButtons.insert(map<char*,int>::value_type("minus_sign",SDLK_MINUS));
	m_kButtons.insert(map<char*,int>::value_type("period",SDLK_PERIOD));
	m_kButtons.insert(map<char*,int>::value_type("forward_slash",SDLK_SLASH));

	m_kButtons.insert(map<char*,int>::value_type("colon",SDLK_COLON));
	m_kButtons.insert(map<char*,int>::value_type("semicolon",SDLK_SEMICOLON));
	m_kButtons.insert(map<char*,int>::value_type("less_than_sign",SDLK_LESS));
	m_kButtons.insert(map<char*,int>::value_type("equals_sign",SDLK_EQUALS));
	m_kButtons.insert(map<char*,int>::value_type("greater_than_sign",SDLK_GREATER));
	m_kButtons.insert(map<char*,int>::value_type("question_mark",SDLK_QUESTION));
	m_kButtons.insert(map<char*,int>::value_type("at",SDLK_AT));
	m_kButtons.insert(map<char*,int>::value_type("left_bracket",SDLK_LEFTBRACKET));
	m_kButtons.insert(map<char*,int>::value_type("backslash",SDLK_BACKSLASH));
	m_kButtons.insert(map<char*,int>::value_type("right_bracket",SDLK_RIGHTBRACKET));
	m_kButtons.insert(map<char*,int>::value_type("caret",SDLK_CARET));
	m_kButtons.insert(map<char*,int>::value_type("underscore",SDLK_UNDERSCORE));
	m_kButtons.insert(map<char*,int>::value_type("grave",SDLK_BACKQUOTE));

	m_kButtons.insert(map<char*,int>::value_type("keypad_period",SDLK_KP_PERIOD));
	m_kButtons.insert(map<char*,int>::value_type("keypad_divide",SDLK_KP_DIVIDE));
	m_kButtons.insert(map<char*,int>::value_type("keypad_multiply",SDLK_KP_MULTIPLY));
	m_kButtons.insert(map<char*,int>::value_type("keypad_minus",SDLK_KP_MINUS));
	m_kButtons.insert(map<char*,int>::value_type("keypad_plus",SDLK_KP_PLUS));
	m_kButtons.insert(map<char*,int>::value_type("keypad_enter",SDLK_KP_ENTER));
	m_kButtons.insert(map<char*,int>::value_type("keypad_equals",SDLK_KP_EQUALS));
	
	m_kButtons.insert(map<char*,int>::value_type("numlock",SDLK_NUMLOCK));
	m_kButtons.insert(map<char*,int>::value_type("capslock",SDLK_CAPSLOCK));
	m_kButtons.insert(map<char*,int>::value_type("scrollock",SDLK_SCROLLOCK));

	
	m_kButtons.insert(map<char*,int>::value_type("mouseleft",MOUSELEFT));
	m_kButtons.insert(map<char*,int>::value_type("mousemiddle",MOUSEMIDDLE));
	m_kButtons.insert(map<char*,int>::value_type("mouseright",MOUSERIGHT));


};

void Input::Update(void) {
	m_iMouseX=-1;	
	m_iMouseY=-1;
	
	while(SDL_PollEvent(&m_kEvent)) {
		switch(m_kEvent.type) {
			
			
			//keyboard
			case SDL_KEYDOWN:
				//put key in list
				m_aPressedKeys.push(m_kEvent.key.keysym.sym);
				if(m_aPressedKeys.size()>m_iQueueLength)
					m_aPressedKeys.pop();

				//set button as pressed		
				m_akButtonList[m_kEvent.key.keysym.sym]=true;					    	
    		break;			
			case SDL_KEYUP:
				//set  button as unpressed
				m_akButtonList[m_kEvent.key.keysym.sym]=false;					    	
    		break;    		
    		
	    	//mouse    		
   	 	case SDL_MOUSEBUTTONDOWN:
    			switch(m_kEvent.button.button){
    				case SDL_BUTTON_LEFT:
    					m_akButtonList[MOUSELEFT]=true;
	    			break;
   	 			case SDL_BUTTON_MIDDLE:
    					m_akButtonList[MOUSEMIDDLE]=true;
    				break;
    				case SDL_BUTTON_RIGHT:
    					m_akButtonList[MOUSERIGHT]=true;
	    			break;	
   	 		}
   	 		break;
	    	case SDL_MOUSEBUTTONUP:
   	 		switch(m_kEvent.button.button){
    				case SDL_BUTTON_LEFT:
    					m_akButtonList[MOUSELEFT]=false;
    				break;
	    			case SDL_BUTTON_MIDDLE:
   	 				m_akButtonList[MOUSEMIDDLE]=false;
    				break;
    				case SDL_BUTTON_RIGHT:
    					m_akButtonList[MOUSERIGHT]=false;
	    			break;	
   	 		}    	
    			 break;
		}	
	}
}

void Input::MouseXY(int &iX,int &iY) {		
	if(m_bInputEnabled) {
		SDL_PumpEvents();
		SDL_GetMouseState(&iX, &iY);
		iX=int(float(iX)*m_fMouseSensitivity);	
		iY=int(float(iY)*m_fMouseSensitivity);		
	} else {
		iX=0;	
		iY=0;			
	}
}

void Input::RelMouseXY(int &iX,int &iY) {		
	if(m_bInputEnabled) {
		if(m_iMouseX==-1)
			SDL_GetRelativeMouseState(&m_iMouseX, &m_iMouseY);		
		iX=int(float(m_iMouseX)*m_fMouseSensitivity);	
		iY=int(float(m_iMouseY)*m_fMouseSensitivity);		
	} else {
		iX=0;	
		iY=0;				
	}
}

void Input::ToggleGrab(void) {	
	if(SDL_GetTicks()-m_iGrabtime>200) {
		m_iGrabtime=SDL_GetTicks();
		if(SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_OFF) {
			GrabInput();
	  } else {
	  	ReleaseInput();
	  }
  } 	
}

void Input::ToggleGrab(bool bGrab) {
	if(bGrab)
		GrabInput();
	else
		ReleaseInput();
}


void Input::GrabInput(void) {
	SDL_WM_GrabInput(SDL_GRAB_ON);
 	SDL_ShowCursor(SDL_DISABLE);
}

void Input::ReleaseInput(void) {
	SDL_WM_GrabInput(SDL_GRAB_OFF);
 	SDL_ShowCursor(SDL_ENABLE);
}
	

void Input::Reset(void) {
	SDL_Event kTemp;

	while(SDL_PollEvent(&kTemp)){
//		cout<<"flushing"<<endl;
	}
	
	for(int i =0;i<400;i++) 
	m_akButtonList[i]=false;
	
	//clear queue
	while(!m_aPressedKeys.empty()){
		m_aPressedKeys.pop();
	}
}




void Input::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	int i;
	switch(cmdid) 
	{
		case FID_TOGGLEGRAB:
			ToggleGrab();
			break;
		
		case FID_BIND:
			if(kCommand->m_kSplitCommand.size()==3)
			{
				if(Bind(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2]))
					cout<<"bind succes!" <<endl;
				else 
					cout<<"bind failed.." <<endl;
				
			} else
				if(GetConsole())
					m_pkConsole->Printf("bind <key> <action>");	
			break;

		
		case FID_UNBINDALL:
			for(i=0; i<400; i++)
				m_aiActionToButton[i]=NULL;
			break;
		case FID_LISTACTIONS:
			ListActions();
			break;
		case FID_MOUSESENS:
			if(kCommand->m_kSplitCommand.size()!=2)
				if(GetConsole())
					m_pkConsole->Printf("MouseSensitivity: %.3f", m_fMouseSensitivity);
			else
				m_fMouseSensitivity = float( atof(kCommand->m_kSplitCommand[1].c_str()) );
			break;
	}
}

int Input::GetQueuedKey()
{
	int value;	
	
	if(m_bInputEnabled){
		if(!m_aPressedKeys.empty()){
			value = m_aPressedKeys.front();
			m_aPressedKeys.pop();

			if(value < 0)
			{
				printf("GetQueuedKey: value < 0");
			}
			return value;
		}
	}
	
	return -1;
}

int Input::SizeOfQueue()
{
	return m_aPressedKeys.size();
}

bool Input::Pressed(int iButton)
{
	
	if(m_bInputEnabled) 
	{	

		return m_akButtonList[iButton];
	} 
	else 
	{
		return false;
	}
}


bool Input::Bind(const string kKey, const string kAction)
{
	unsigned int iAcSize = m_kActions.size();
	unsigned int iKeySize = m_kButtons.size();
	map<string, int>::iterator kIt = m_kButtons.find(kKey);
	if(kIt!= m_kButtons.end())
	{
		m_kActions[kAction];
		if(m_kActions.size()!=iAcSize)
		{
			m_kActions.erase(kAction);
			if(GetConsole())
			m_pkConsole->Printf("putting'%s'in pending actions list",kAction.c_str());	
			//cout<<"putting'" <<kAction<<"'in pending actions list" <<endl;
			m_kPendingActions[kAction]=&(*kIt);
		}
		else 
		{
		
			(*m_kActions[kAction])=&(*kIt);
		}
		return true;
	}
	else
	{
	if(GetConsole())
		m_pkConsole->Printf("bind <key> <action>");	
		
		return false;
	}
};


void Input::ListActions()
{
	if(GetConsole())
	{
		if(m_kActions.empty())
			//if(GetConsole())
			m_pkConsole->Printf("No actions defined!");	
		else
		{
			map<const string, pair<const string, int>**>::iterator kItor = m_kActions.begin();
			while (kItor != m_kActions.end())
			{
				
			
				if((*kItor->second) !=NULL)
					m_pkConsole->Printf("%s : %s",kItor->first.c_str(),((*kItor->second)->first).c_str());
				else 
					m_pkConsole->Printf("%s : no key defined",(kItor->first).c_str());
				kItor++;
				
			}
		}
	}
}


bool Input::GetConsole()
{
	m_pkConsole	= static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
	
	if(m_pkConsole==NULL) 
		return false;
	else 
		return true;
}

int Input::RegisterAction(const char *pcAction)
{
	string kAction=string(pcAction);
	if(!m_kPendingActions.empty())
	{
		map<const string, pair<const string, int>*>::iterator kIt = m_kPendingActions.find(kAction);
		
		if(kIt != m_kPendingActions.end())
		{
			m_aiActionToButton[m_iNrActions]=&(*kIt->second);
			m_kPendingActions.erase(kIt);
		}
		else m_aiActionToButton[m_iNrActions]=NULL;
		
	}
	else m_aiActionToButton[m_iNrActions]=NULL;
	unsigned int iAcSize = m_kActions.size();
	m_kActions[kAction];
	if( iAcSize == m_kActions.size())
	{
	

		for(int i=0; i<m_iNrActions;i++)
		{
		
			if(m_kActions[kAction]==&m_aiActionToButton[i])
				return i;
		}
	}
	else
		m_kActions[kAction]=&m_aiActionToButton[m_iNrActions];

	m_iNrActions++;
	return (m_iNrActions-1);
}


bool Input::Action(int iAction)
{
	if(m_bInputEnabled) 
	{	
		if(m_aiActionToButton[iAction]!=NULL)
			return m_akButtonList[m_aiActionToButton[iAction]->second];
		else return false;
	} 
	else 
	{
		return false;
	}
}
