#ifndef _INPUT_H_
#define _INPUT_H_

#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include "engine_x.h"

using namespace std;

enum Buttons {
	ESCAPE=SDLK_ESCAPE,
	LEFT=SDLK_LEFT,
	RIGHT=SDLK_RIGHT,
	UP=SDLK_UP,
	DOWN=SDLK_DOWN,
	SPACE=SDLK_SPACE,
	F12=SDLK_F12,
	F11=SDLK_F11,
	F10=SDLK_F10,
	F9=SDLK_F9,	
	F8=SDLK_F8,	
	F7=SDLK_F7,	
	F6=SDLK_F6,
	F5=SDLK_F5,	
	F4=SDLK_F4,	
	F3=SDLK_F3,	
	F2=SDLK_F2,
	F1=SDLK_F1,
	PAGEUP=SDLK_PAGEUP,
	PAGEDOWN=SDLK_PAGEDOWN,
	HOME=SDLK_HOME,
	END=SDLK_END,
	TAB=SDLK_TAB,
	
	KEY_1=SDLK_1,
	KEY_2=SDLK_2,
	KEY_3=SDLK_3,
	KEY_4=SDLK_4,
	KEY_5=SDLK_5,
	KEY_6=SDLK_6,
	KEY_7=SDLK_7,
	KEY_8=SDLK_8,
	KEY_9=SDLK_9,
	KEY_0=SDLK_0,
	
	KEY_Q=SDLK_q,
	KEY_W=SDLK_w,	
	KEY_E=SDLK_e,	
	KEY_A=SDLK_a,	
	KEY_S=SDLK_s,	
	KEY_D=SDLK_d,	
	
	MOUSELEFT=397,
	MOUSEMIDDLE,
	MOUSERIGHT

};

class ENGINE_API Input {
	private:
		bool m_akButtonList[400];
		SDL_Event m_kEvent;
		unsigned int m_iGrabtime;
			
		void GrabInput(void);
		void ReleaseInput(void);
	
	
	public:
		float m_fMouseSensitivity;
	
		Input();
		
		void Update(void);
		inline bool Pressed(int iButton){return m_akButtonList[iButton];};
		void MouseXY(int &iX,int &iY);
		void RelMouseXY(int &iX,int &iY);
		void ToggleGrab(void);
		void ToggleGrab(bool bGrab);		
		void Reset(void);
		
//		inline unsigned int GetTicks(void) {return SDL_GetTicks();};
};

#endif
