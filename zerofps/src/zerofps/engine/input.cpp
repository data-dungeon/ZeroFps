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
	for(int i =0;i<400;i++) 
		m_akButtonList[i]=false;
	
	m_fMouseSensitivity=1;	
	m_iGrabtime=SDL_GetTicks();
	m_bInputEnabled=true;

	g_ZFObjSys.Register_Cmd("togglegrab",FID_TOGGLEGRAB,this);
	
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
				//set button ass pressed		
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
	switch(cmdid) {
		case FID_TOGGLEGRAB:
			ToggleGrab();
			break;

		}
}

int Input::GetQueuedKey()
{
	int value;
	
	if(!m_aPressedKeys.empty()){
		value = m_aPressedKeys.front();
		m_aPressedKeys.pop();
		return value;
	}
	
	return -1;
}

int Input::SizeOfQueue()
{
	return m_aPressedKeys.size();
}

bool Input::Pressed(int iButton){
	if(m_bInputEnabled) {	
		return m_akButtonList[iButton];
	} else {
		return false;
	}
	
}


