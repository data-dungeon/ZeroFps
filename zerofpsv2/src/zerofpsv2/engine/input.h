#ifndef _ENGINE_INPUT_H_
#define _ENGINE_INPUT_H_

#include "../basic/basicconsole.h"
#include "../basic/zfsubsystem.h"
#include "../basic/rect.h"
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

/*
	This is the input keys from the keyboard. They are mapped as far as possible to map to ASCII.
*/
enum Buttons
{
	KEY_BACKSPACE	= 8,		
	KEY_TAB			= 9,				
	// Clear			
	KEY_RETURN		= 13,			
	//PAUSE
	KEY_ESCAPE		= 27,			
	KEY_SPACE		= 32,	
	KEY_EXCLAIM		= 33,				
	KEY_QUOTEDBL	= 34,				
	KEY_HASH			= 35,				
	KEY_DOLLAR		= 36,				
	KEY_AMPERSAND	= 38,				
	KEY_QUOTE		= 39,			
	KEY_LEFTPAREN	= 40,				
	KEY_RIGHTPAREN	= 41,				
	KEY_ASTERISK	= 42,				
	KEY_PLUS			= 43,		
	KEY_COMMA		= 44,				
	KEY_MINUS		= 45,			
	KEY_PERIOD		= 46,				
	KEY_SLASH		= 47,				

	KEY_0				= 48,				
	KEY_1				= 49,				
	KEY_2				= 50,			
	KEY_3				= 51,			
	KEY_4				= 52,			
	KEY_5				= 53,			
	KEY_6				= 54,			
	KEY_7				= 55,				
	KEY_8				= 56,			
	KEY_9				= 57,				
	
	KEY_COLON		= 58,			
	KEY_SEMICOLON	= 59,			
	KEY_LESS			= 60,				
	KEY_EQUALS		= 61,				
	KEY_GREATER		= 62,				
	KEY_QUESTION	= 63,			
	KEY_AT			= 64,				

	KEY_LEFTBRACKET	= 91,			
	KEY_BACKSLASH		= 92,				
	KEY_RIGHTBRACKET	= 93,			
	KEY_CARET			= 94,				
	KEY_UNDERSCORE		= 95,				
	KEY_BACKQUOTE		= 96,	

	KEY_A					= 97,				
	KEY_B					= 98,					
	KEY_C					= 99,				
	KEY_D					= 100,		
	KEY_E					= 101,			
	KEY_F					= 102,		
	KEY_G					= 103,		
	KEY_H					= 104,			
	KEY_I					= 105,					
	KEY_J					= 106,				
	KEY_K					= 107,				
	KEY_L					= 108,				
	KEY_M					= 109,			
	KEY_N					= 110,			
	KEY_O					= 111,		
	KEY_P					= 112,				
	KEY_Q					= 113,			
	KEY_R					= 114,				
	KEY_S					= 115,				
	KEY_T					= 116,				
	KEY_U					= 117,			
	KEY_V					= 118,				
	KEY_W					= 119,				
	KEY_X					= 120,			
	KEY_Y					= 121,					
	KEY_Z					= 122,				
	// End Of Evil Ascii.


	/* Num Keypad */
	KEY_KP0,
	KEY_KP1,
	KEY_KP2,
	KEY_KP3,
	KEY_KP4,
	KEY_KP5,
	KEY_KP6,
	KEY_KP7,
	KEY_KP8,
	KEY_KP9,
	KEY_KP_PERIOD,				
	KEY_KP_DIVIDE,			
	KEY_KP_MULTIPLY,			
	KEY_KP_MINUS,				
	KEY_KP_PLUS,				
	KEY_KP_ENTER,				
	KEY_KP_EQUALS,				

	/* Arrow Pad */
	KEY_LEFT,			
	KEY_RIGHT,			
	KEY_UP,				
	KEY_DOWN,	

	/* Ins/Home/Page Pad*/
	KEY_PAGEUP,			
	KEY_PAGEDOWN,		
	KEY_HOME,			
	KEY_END,			
	KEY_DELETE,		
	KEY_INSERT,		

	/* Function keys */
	KEY_F1,				
	KEY_F2,				
	KEY_F3,				
	KEY_F4,				
	KEY_F5,				
	KEY_F6,				
	KEY_F7,				
	KEY_F8,				
	KEY_F9,			
	KEY_F10,				
	KEY_F11,				
	KEY_F12,				
	KEY_F13,				
	KEY_F14,				
	KEY_F15,				

	KEY_NUMLOCK,			
	KEY_CAPSLOCK,				
	KEY_SCROLLOCK,	
	KEY_RSHIFT,		
	KEY_LSHIFT,			
	KEY_RCTRL,		
	KEY_LCTRL,			
	KEY_RALT,		
	KEY_LALT,			


	


	MOUSELEFT=397,
	MOUSEMIDDLE,
	MOUSERIGHT,


};

#define MAX_KEYS		1000
#define MAX_SDLKEYS	SDLK_LAST

struct InputKey 
{	
	bool		m_bDown;
	string	m_strName;
};

#define VKMAPS	3		// Max number of real input devices map to a Vkey.

class VKData
{
public:

	string	m_strName;
	int		m_iInputKey[VKMAPS];
};

class Console;


/** \brief	Engine Systems that handles input (keyb, mouse, joystick).
	 \ingroup Engine
*/
class ENGINE_API Input : public ZFSubSystem {
	private:
		vector<VKData>		m_VirtualKeys;

		InputKey				m_akKeyState[MAX_KEYS];
		int					m_akMapToKeyState[MAX_SDLKEYS];

		void SetupMapToKeyState();
		int  SDLToZeroFpsKey(int iSdlSym);		

		enum FuncId_e
		{
			FID_TOGGLEGRAB,
			FID_BINDVK,			// Bind VK to a key.
			FID_MOUSESENS,
			FID_VKBINDLIST,		// List all VK and keys they are bind to.
			//FID_UNBINDALL,
			//FID_BIND,
			//FID_LISTACTIONS,
		};		
		
		BasicConsole*	m_pkConsole;
		ZeroFps*			m_pkZeroFps;
			
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
		
		//int m_iNrActions;
		
		bool GetConsole();
		
		//map<const string, pair<const string, int>**>		m_kActions;
		//map<const string, pair<const string, int>*>		m_kPendingActions;
		//map<const string, int>									m_kButtons;
		//pair<const string, int>*									m_aiActionToButton[400];
		//void ListActions();
		//bool Bind(const string kKey, const string kAction);
		//void SetupButtons();

public:
		int		m_iSDLMouseX, m_iSDLMouseY;
		float		m_fMouseSensitivity;
		
		Input();
		bool StartUp();
		bool ShutDown();
		bool IsValid();
	
		void Update(void);

		bool Pressed(Buttons eButton);
		
		VKData*	GetVKByName(string strName);
		bool VKBind(string strName, Buttons kKey, int iIndex );
		bool VKBind(string strName, string strKeyName);
		bool VKIsDown(string strName);
		
		void VKList();
		
		void FormatKey(int& iKey);
		void SetCursorInputPos(int x, int y);
		
		
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

		void ShowCursor(bool bShow);



		string  GetKeyName(Buttons eKey);
		Buttons GetNameByKey(string strName);
		Buttons GetKeyCode(string strName);



		//map<int,int> m_kGlobalKeyTranslator;
		//bool Action(int iAction);
		//int RegisterAction(const char* pcAction);

};

#endif
