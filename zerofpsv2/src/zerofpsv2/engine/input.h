#ifndef _ENGINE_INPUT_H_
#define _ENGINE_INPUT_H_

#include "../basic/basicconsole.h"
#include "../basic/zfsubsystem.h"
#include "../basic/rect.h"
#include "../basic/keys.h"
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
