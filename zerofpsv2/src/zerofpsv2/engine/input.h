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

class ZSSZeroFps;
class InputHandle;
class ZSSConsole;


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

class BasicKey
{
	public:
		int	m_iKey;
		int	m_iModifiers;
};

class QueuedKeyInfo
{
	public:
		int	m_iKey;
		int	m_iModifiers;		// OR-combined bit list (alt , shift , ctrl)
		bool	m_bPressed;			// if false its released
		
		QueuedKeyInfo()
		{
			m_iKey = -1;
			m_iModifiers = 0;	
			m_bPressed = true;	
		};
		
		QueuedKeyInfo(int iKey,int iMod,bool bPressed)
		{
			m_iKey = iKey;
			m_iModifiers = iMod;
			m_bPressed = bPressed;
		};
};


/** \brief	Engine Systems that handles input (keyb, mouse, joystick).
	 \ingroup Engine
*/
class ENGINE_API ZSSInput : public ZFSubSystem {
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
		ZSSZeroFps*					m_pkZeroFps;
		ZSSRender*				m_pkRender;	
		
		vector<VKData>			m_VirtualKeys;								// Virtual keys.
		vector<InputHandle*>	m_kInputHandles;
		
		InputKey					m_akKeyState[MAX_KEYS];					// State of all keys.	
		int						m_akMapToKeyState[MAX_SDLKEYS];		// Table that maps from SDL's keys to the ones used by ZSSZeroFps.

		queue<QueuedKeyInfo>	m_aPressedKeys;
			
		float				m_fGrabtime;			
		float				m_fRelMouseX,m_fRelMouseY;
		float				m_fAbsMouseX,m_fAbsMouseY;
		unsigned int	m_iQueueLength;
		bool 				m_bKeyRepeat;
		bool				m_bGrabInput;
		
		int				m_iSDLMouseX, m_iSDLMouseY;
		float				m_fMouseSensitivity;
		bool				m_bVisibleHWCursor;

		bool				m_bHaveReleasedMWUP,m_bHaveReleasedMWDOWN;
	
		bool				m_bBindMode;
		string			m_strBindKey;
		int				m_iBindKeyIndex;

		//joystick info
		int							m_iNumOfJoySticks;
		vector<SDL_Joystick*>	m_kJoySticks;
		
		

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void Reset(void);		
		void SetupMapToKeyState();
		
		BasicKey TranslateKey(SDL_keysym* pkSdlKey);
		int  SDLToZeroFpsKey(int iSdlSym);		
		
		
		void AddQueuedKey(BasicKey* pkKey,bool bPressed);		
		void UpdateInputHandles();

		
		//input fuctions , called by inputhandles 
		QueuedKeyInfo GetQueuedKey();
		int SizeOfQueue();

		//mouse functions
		void MouseXY(float &fX,float &fY);
		void UnitMouseXY(float &fX,float &fY);
		void SDLMouseXY(int &iX,int &iY);
		void RelMouseXY(float &iX,float &iY);
		void SetCursorInputPos(float x, float y);
		
		bool Pressed(Buttons eButton);		
		bool VKIsDown(string strName);		
		void BindBindMode(int iKey);

		void KeyDown(int iKey);
		void KeyUp(int iKey);
		
public:

		ZSSInput();
		bool StartUp();
		bool ShutDown()		{return true;};
		bool IsValid()			{return true;};
			
		void UpdateMousePos();		
		void Update(void);

		void ShowCursor(bool bShow);				
		void ToggleGrab(void);
		void ToggleGrab(bool bGrab,bool bLog = true);		

		float GetMouseSens() 				{	return m_fMouseSensitivity; 	}
		bool	GetGrabInput()					{	return m_bGrabInput;				}
		
		//virtual keys
		void StartBindMode(string strBindKey, int iBindIndex);
		
		VKData*	GetVKByName(string strName);
		bool VKBind(string strName, Buttons kKey, int iIndex );
		bool VKBind(string strName, string strKeyName,int iIndex = 0);
		
		void VKList();
		void Save(string strCfgName);
		void Load(string strCfgName);		

		string  GetKeyName(Buttons eKey);
		Buttons GetNameByKey(string strName);


		//input handling
		bool RegisterInputHandle(InputHandle* pkInputHandle);
		bool UnregisterInputHandle(InputHandle* pkInputHandle);
		InputHandle* GetInputHandle(string strName);

		//input handles
		bool SetActiveInputHandle(string strName);
		bool AddActiveInputHandle(string strName);
		bool RemoveActiveInputHandle(string strName);
		void ClearActiveInputHandles();

		void PrintInputHandlers();

		friend class InputHandle;
};

#endif
