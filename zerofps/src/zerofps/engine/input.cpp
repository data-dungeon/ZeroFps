#include "input.h"


Input::Input() {
	cout<<"Sdl_Input initializing"<<endl;	
	
	//init sdl to read events
	if(SDL_Init(SDL_INIT_EVENTTHREAD)<0){
		cout<<"Sdl_Input dont want to work =("<<endl;
		exit(1);
	}
	
	//reset all buttons to false
	for(int i =0;i<400;i++) 
		m_akButtonList[i]=false;
	
	
	m_iGrabtime=SDL_GetTicks();
};

void Input::Update(void) {
	while(SDL_PollEvent(&m_kEvent)) {
		switch(m_kEvent.type) {
			
			//keyboard
			case SDL_KEYDOWN:
				m_akButtonList[m_kEvent.key.keysym.sym]=true;					    	
    		break;			
			case SDL_KEYUP:
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
	SDL_GetMouseState(&iX, &iY);
}

void Input::RelMouseXY(int &iX,int &iY) {		
	SDL_GetRelativeMouseState(&iX, &iY);
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

}




