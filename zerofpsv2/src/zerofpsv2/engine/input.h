#ifndef _ENGINE_INPUT_H_
#define _ENGINE_INPUT_H_

#include "../basic/basicconsole.h"
#include "../basic/zfsubsystem.h"
#include "../basic/rect.h"
#include "../basic/keys.h"
#include "../render/render.h"
#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <queue>
#include <map>
#include <string>
#include "engine_x.h"

using namespace std;

class ZeroFps;
class InputHandle;
class Console;


#define MAX_KEYS		1000
#define MAX_SDLKEYS	SDLK_LAST
#define VKMAPS			3					// Max number of real input devices map to a Vkey.
#define MAX_KEYNAME	64					// Max length of key names.					

/*	This is the data of one real (something that exist on a input device such as keyb/mouse) key that the engine
	nows about. */
struct InputKey 
{	
	bool		m_bDown;						// True if this key is/was down at the last input update.
	string	m_strName;					// Name of this key.
};

/*	This is the data that the engine keeps about each virtual keys it uses. */
class VKData
{
public:

	string	m_strName;					// Name of this virtual key.
	int		m_iInputKey[VKMAPS];		// The real keys that are mapped to this virtual key.
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


/** \brief	Engine Systems that handles input (keyb, mouse, joystick).
	 \ingroup Engine
*/
class ENGINE_API Input : public ZFSubSystem {
	private:
		enum FuncId_e
		{
			FID_TOGGLEGRAB,			
			FID_BINDVK,					// Bind VK to a key.
			FID_MOUSESENS,				// Set sensitivity of mouse.
			FID_VKBINDLIST,			// List all VK and keys they are bind to.
			FID_LOAD,					// Load current key config.
			FID_SAVE,					// Save current key config.
		};		

		BasicConsole*			m_pkConsole;
		ZeroFps*					m_pkZeroFps;
		Render*					m_pkRender;	
		
		vector<VKData>			m_VirtualKeys;								// Virtual keys.
		vector<InputHandle*>	m_kInputHandles;
		
		InputKey					m_akKeyState[MAX_KEYS];					// State of all keys.	
		int						m_akMapToKeyState[MAX_SDLKEYS];		// Table that maps from SDL's keys to the ones used by ZeroFps.

		queue<QueuedKeyInfo>	m_aPressedKeys;
			
		SDL_Event		m_kEvent;
		unsigned int	m_iGrabtime;			
		int				m_iMouseX,m_iMouseY;
		int				m_iAbsMouseX,m_iAbsMouseY;
		unsigned int	m_iQueueLength;
		bool 				m_bKeyRepeat;

		int				m_iSDLMouseX, m_iSDLMouseY;
		float				m_fMouseSensitivity;

		bool				m_bHaveReleasedMWUP,m_bHaveReleasedMWDOWN;
	
		bool				m_bBindMode;
		string			m_strBindKey;
		int				m_iBindKeyIndex;



		void SetupMapToKeyState();
		int  SDLToZeroFpsKey(int iSdlSym);		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void GrabInput(void);
		void ReleaseInput(void);
		void AddQueuedKey(SDL_keysym* kKey);
		bool GetConsole();
		
		void UpdateMousePos();
		
		void UpdateInputHandles();
	


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

		void BindBindMode(int iKey);

public:

		Input();
		bool StartUp();
		bool ShutDown();
		bool IsValid();
			
		void Update(void);

		void StartBindMode(string strBindKey, int iBindIndex);
		
		VKData*	GetVKByName(string strName);
		bool VKBind(string strName, Buttons kKey, int iIndex );
		bool VKBind(string strName, string strKeyName,int iIndex = 0);
		
		void VKList();
		void Save(string strCfgName);
		void Load(string strCfgName);
				
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

		friend class InputHandle;
};

#endif
