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


class ZeroFps;
class InputHandle;

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

class QueuedKeyInfo
{
	public:
		int	m_iKey;
		int	m_iModifiers;		// OR-combined bit list (alt , shift , ctrl)
		
		QueuedKeyInfo()
		{
			m_iKey = -1;
			m_iModifiers = 0;		
		};
		
		QueuedKeyInfo(int iKey,int iMod)
		{
			m_iKey = iKey;
			m_iModifiers = iMod;
		};
};

class Console;



/** \brief	Engine Systems that handles input (keyb, mouse, joystick).
	 \ingroup Engine
*/
class ENGINE_API Input : public ZFSubSystem {
	private:
		BasicConsole*			m_pkConsole;
		ZeroFps*					m_pkZeroFps;	
		
		vector<VKData>			m_VirtualKeys;
		vector<InputHandle*>	m_kInputHandles;
		
		InputKey					m_akKeyState[MAX_KEYS];
		int						m_akMapToKeyState[MAX_SDLKEYS];

		queue<QueuedKeyInfo>	m_aPressedKeys;

		enum FuncId_e
		{
			FID_TOGGLEGRAB,
			FID_BINDVK,			// Bind VK to a key.
			FID_MOUSESENS,
			FID_VKBINDLIST,		// List all VK and keys they are bind to.
			FID_LOAD,
			FID_SAVE,
			//FID_UNBINDALL,
			//FID_BIND,
			//FID_LISTACTIONS,
		};		
		
			
		SDL_Event		m_kEvent;
		unsigned int	m_iGrabtime;			
		int				m_iMouseX,m_iMouseY;
		int				m_iAbsMouseX,m_iAbsMouseY;
		unsigned int	m_iQueueLength;
		bool 				m_bKeyRepeat;

		int				m_iSDLMouseX, m_iSDLMouseY;
		
	
		void SetupMapToKeyState();
		int  SDLToZeroFpsKey(int iSdlSym);		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void GrabInput(void);
		void ReleaseInput(void);
		void AddQueuedKey(SDL_keysym* kKey);
		bool GetConsole();
		
		void UpdateMousePos();

		void UpdateInputHandles();
		
		//int m_iNrActions;		
		//map<const string, pair<const string, int>**>		m_kActions;
		//map<const string, pair<const string, int>*>		m_kPendingActions;
		//map<const string, int>									m_kButtons;
		//pair<const string, int>*									m_aiActionToButton[400];
		//void ListActions();
		//bool Bind(const string kKey, const string kAction);
		//void SetupButtons();


		//input fuctions , called by inputhandles 
		QueuedKeyInfo GetQueuedKey();
		int SizeOfQueue();

		void MouseXY(int &iX,int &iY);
		void UnitMouseXY(float &fX,float &fY);
		void SDLMouseXY(int &iX,int &iY);
		void RelMouseXY(int &iX,int &iY);
		void SetCursorInputPos(int x, int y);
		
		bool Pressed(Buttons eButton);
		
		bool VKIsDown(string strName);		
		void Reset(void);		
		
public:
		float		m_fMouseSensitivity;
		
		Input();
		bool StartUp();
		bool ShutDown();
		bool IsValid();
	
		void Update(void);

		
		
		VKData*	GetVKByName(string strName);
		bool VKBind(string strName, Buttons kKey, int iIndex );
		bool VKBind(string strName, string strKeyName);
		
		void VKList();
		void Save(string strCfgName);
				
				
		void ShowCursor(bool bShow);
				
		void ToggleGrab(void);
		void ToggleGrab(bool bGrab);		

		string  GetKeyName(Buttons eKey);
		Buttons GetNameByKey(string strName);
		Buttons GetKeyCode(string strName);


		//input handling
		bool RegisterInputHandle(InputHandle* pkInputHandle);
		bool UnregisterInputHandle(InputHandle* pkInputHandle);
		InputHandle* GetInputHandle(string strName);

		bool SetActiveInputHandle(string strName);
		bool AddActiveInputHandle(string strName);
		bool RemoveActiveInputHandle(string strName);
		void ClearActiveInputHandles();

		void PrintInputHandlers();

		//map<int,int> m_kGlobalKeyTranslator;
		//bool Action(int iAction);
		//int RegisterAction(const char* pcAction);

		friend class InputHandle;
};

#endif
