#ifndef _ENGINE_INPUT_H_
#define _ENGINE_INPUT_H_

#include "../basic/basicconsole.h"
#include "../basic/zfsubsystem.h"
#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <queue>
#include <map>
#include <string>
#include "engine_x.h"

//#include "console.h"
class ZeroFps;

using namespace std;

enum Buttons
{
	KEY_ESCAPE=SDLK_ESCAPE,
	KEY_LEFT=SDLK_LEFT,
	KEY_RIGHT=SDLK_RIGHT,
	KEY_UP=SDLK_UP,
	KEY_DOWN=SDLK_DOWN,
	KEY_SPACE=SDLK_SPACE,
	KEY_BACKSPACE=SDLK_BACKSPACE,
	KEY_RETURN=SDLK_RETURN,
	
	KEY_RSHIFT=SDLK_RSHIFT,
	KEY_RCTRL=SDLK_RCTRL,
	KEY_LSHIFT=SDLK_LSHIFT,
	KEY_LCTRL=SDLK_LCTRL,
	
	KEY_F12=SDLK_F12,
	KEY_F11=SDLK_F11,
	KEY_F10=SDLK_F10,
	KEY_F9=SDLK_F9,	
	KEY_F8=SDLK_F8,	
	KEY_F7=SDLK_F7,	
	KEY_F6=SDLK_F6,
	KEY_F5=SDLK_F5,	
	KEY_F4=SDLK_F4,	
	KEY_F3=SDLK_F3,	
	KEY_F2=SDLK_F2,
	KEY_F1=SDLK_F1,
	
	KEY_PAGEUP=SDLK_PAGEUP,
	KEY_PAGEDOWN=SDLK_PAGEDOWN,
	KEY_HOME=SDLK_HOME,
	KEY_END=SDLK_END,
	KEY_TAB=SDLK_TAB,
	KEY_DELETE=SDLK_DELETE,
	KEY_INSERT=SDLK_INSERT,	
	
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
	KEY_R=SDLK_r,	
	KEY_T=SDLK_t,	
	KEY_Y=SDLK_y,	
	KEY_U=SDLK_u,	
	KEY_I=SDLK_i,	
	KEY_O=SDLK_o,	
	KEY_P=SDLK_p,	
	

	KEY_A=SDLK_a,	
	KEY_S=SDLK_s,	
	KEY_D=SDLK_d,	
	KEY_F=SDLK_f,	
	KEY_G=SDLK_g,	
	KEY_H=SDLK_h,	
	KEY_J=SDLK_j,	
	KEY_K=SDLK_k,	
	KEY_L=SDLK_l,	
	

	KEY_Z=SDLK_z,	
	KEY_X=SDLK_x,	
	KEY_C=SDLK_c,	
	KEY_V=SDLK_v,	
	KEY_B=SDLK_b,	
	KEY_N=SDLK_n,	
	KEY_M=SDLK_m,

	KEY_EXCLAIM=SDLK_EXCLAIM,
	KEY_QUOTEDBL=SDLK_QUOTEDBL,
	KEY_HASH=SDLK_HASH,
	KEY_DOLLAR=SDLK_DOLLAR,
	KEY_AMPERSAND=SDLK_AMPERSAND,
	KEY_QUOTE=SDLK_QUOTE,
	KEY_LEFTPAREN=SDLK_LEFTPAREN,
	KEY_RIGHTPAREN=SDLK_RIGHTPAREN,
	KEY_ASTERISK=SDLK_ASTERISK,
	KEY_PLUS=SDLK_PLUS,
	KEY_COMMA=SDLK_COMMA,
	KEY_MINUS=SDLK_MINUS,
	KEY_PERIOD=SDLK_PERIOD,
	KEY_SLASH=SDLK_SLASH,

	KEY_COLON=SDLK_COLON,
	KEY_SEMICOLON=SDLK_SEMICOLON,
	KEY_LESS=SDLK_LESS,
	KEY_EQUALS=SDLK_EQUALS,
	KEY_GREATER=SDLK_GREATER,
	KEY_QUESTION=SDLK_QUESTION,
	KEY_AT=SDLK_AT,
	KEY_LEFTBRACKET=SDLK_LEFTBRACKET,
	KEY_BACKSLASH=SDLK_BACKSLASH,
	KEY_RIGHTBRACKET=SDLK_RIGHTBRACKET,
	KEY_CARET=SDLK_CARET,
	KEY_UNDERSCORE=SDLK_UNDERSCORE,
	KEY_BACKQUOTE=SDLK_BACKQUOTE,

	KEY_KP_PERIOD=SDLK_KP_PERIOD,
	KEY_KP_DIVIDE=SDLK_KP_DIVIDE,
	KEY_KP_MULTIPLY=SDLK_KP_MULTIPLY,
	KEY_KP_MINUS=SDLK_KP_MINUS,
	KEY_KP_PLUS=SDLK_KP_PLUS,
	KEY_KP_ENTER=SDLK_KP_ENTER,
	KEY_KP_EQUALS=SDLK_KP_EQUALS,
	
	KEY_NUMLOCK=SDLK_NUMLOCK,
	KEY_CAPSLOCK=SDLK_CAPSLOCK,
	KEY_SCROLLOCK=SDLK_SCROLLOCK,


	MOUSELEFT=397,
	MOUSEMIDDLE,
	MOUSERIGHT,


};




class Console;


/// Engine Systems that handles input (keyb, mouse, joystick).
class ENGINE_API Input : public ZFSubSystem {
	private:
		
		enum FuncId_e
		{
			FID_TOGGLEGRAB,
			FID_BIND,
			FID_UNBINDALL,
			FID_LISTACTIONS,
			FID_MOUSESENS,
		};		
		
		//Console* m_pkConsole;
		BasicConsole* m_pkConsole;
		ZeroFps* m_pkZeroFps;
			
		bool				m_akButtonList[400];
		bool				m_bInputEnabled;
		SDL_Event		m_kEvent;
		unsigned int	m_iGrabtime;			
		int				m_iMouseX,m_iMouseY;
		int				m_iAbsMouseX,m_iAbsMouseY;
		unsigned int	m_iQueueLength;
		bool 				m_bKeyRepeat;

		queue<int>		m_aPressedKeys;
	
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void GrabInput(void);
		void ReleaseInput(void);
		map<const string, pair<const string, int>**> m_kActions;
		map<const string, pair<const string, int>*> m_kPendingActions;
		map<const string, int> m_kButtons;
		pair<const string, int>* m_aiActionToButton[400];
		int m_iNrActions;
		bool Bind(const string kKey, const string kAction);
		
		void SetupButtons();
		bool GetConsole();
		void ListActions();
		
	public:
		void FormatKey(int& iKey);
		void SetCursorInputPos(int x, int y);
		float m_fMouseSensitivity;
		
		bool Action(int iAction);
		int RegisterAction(const char* pcAction);
		Input();
		
		void Update(void);
		bool Pressed(int iButton);
		int GetQueuedKey();
		int SizeOfQueue();
		void UpdateMousePos();
		void MouseXY(int &iX,int &iY);
		void UnitMouseXY(float &fX,float &fY);
		void RelMouseXY(int &iX,int &iY);
		void ToggleGrab(void);
		void ToggleGrab(bool bGrab);		
		void Reset(void);
		void SetInputEnabled(bool bInputEnabled) { m_bInputEnabled=bInputEnabled ;};
		bool GetInputEnabled() { return m_bInputEnabled;};


		bool StartUp();
		bool ShutDown();
		bool IsValid();

		map<int,int> m_kGlobalKeyTranslator;
};

#endif
