#include "input.h"
#include "../basic/zfsystem.h"
#include "zerofps.h"
#include "inputhandle.h"

Input::Input() 
 : ZFSubSystem("Input") {
	
	m_bBindMode = 0;

	// Set Default values
	m_fMouseSensitivity = 1;	

	// Register Variables
	RegisterVariable("m_sens",	&m_fMouseSensitivity,	CSYS_FLOAT);

	// Register Commands
	Register_Cmd("i_togglegrab",		FID_TOGGLEGRAB);
	Register_Cmd("i_mousesens",		FID_MOUSESENS);		
	Register_Cmd("i_bindvk",			FID_BINDVK);
	Register_Cmd("i_listvk",			FID_VKBINDLIST);
	Register_Cmd("i_save",				FID_SAVE);
	Register_Cmd("i_load",				FID_LOAD);
};

bool Input::StartUp()	
{ 
	m_pkZeroFps	=	static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));
	m_pkRender = 	static_cast<Render*>(GetSystem().GetObjectPtr("Render"));
	GetConsole();	

	m_bKeyRepeat	= true;
	m_iQueueLength	= 100;
	m_iGrabtime		= SDL_GetTicks();

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	
	m_iAbsMouseX = 0;
	m_iAbsMouseY = 0;
	
	m_bHaveReleasedMWUP = false;
	m_bHaveReleasedMWDOWN = false;
	
	for(int i =0; i<MAX_KEYS; i++) 
	{
		m_akKeyState[i].m_bDown = false;
	};
	
	SetupMapToKeyState();	

	return true;
}

bool Input::ShutDown()	{	return true;	}
bool Input::IsValid()	{	return true;	}

void Input::UpdateInputHandles()
{
	for(unsigned int i = 0;i<m_kInputHandles.size();i++)
	{
		m_kInputHandles[i]->m_bTempDisable = false;
	}
}

void Input::ClearActiveInputHandles()
{
	//reset input
	Reset();

	//deactivate all inputhandles
	for(unsigned int i = 0;i<m_kInputHandles.size();i++)
	{
		m_kInputHandles[i]->m_bActive = false;
	}
}

bool Input::AddActiveInputHandle(string strName)
{
	InputHandle* pkHandle = GetInputHandle(strName);
	
	if(pkHandle)
	{
		pkHandle->m_bActive = true;		
		//cout<<"Activating inputhandle "<<strName<<endl;		
		
		return true;
	}
	else
	{
		cout<<"Input handle "<<strName<<" not registered"<<endl;
	}
	
	return false;
}

bool Input::RemoveActiveInputHandle(string strName)
{
	InputHandle* pkHandle = GetInputHandle(strName);
	
	if(pkHandle)
	{	
		pkHandle->m_bActive = false;
		//cout<<"Removing activ input handle "<<strName<<endl;
		return true;
	}
	else
		cout<<"Input handle "<<strName<<" not registered"<<endl;
	
	return false;
}

bool Input::SetActiveInputHandle(string strName)
{
	
	InputHandle* pkHandle = GetInputHandle(strName);
	
	if(pkHandle)
	{
		ClearActiveInputHandles();	
		pkHandle->m_bActive = true;
		
		//cout<<"Setting active input handle "<<strName<<endl;
		
		return true;
	}
	else
	{
		cout<<"Input handle "<<strName<<" not registered"<<endl;
	}
	
	return false;
}

bool Input::RegisterInputHandle(InputHandle* pkInputHandle)
{
	if(GetInputHandle(pkInputHandle->m_strHandleName))
	{
		cout<<"WARNING: A handle whit the name "<<pkInputHandle->m_strHandleName<<" is already registered"<<endl;
	}
	else
	{
		m_kInputHandles.push_back(pkInputHandle);
		// cout<<"InputHandle "<<pkInputHandle->m_strHandleName<<" registered"<<endl;
		return true;
	}

	return false;
}

bool Input::UnregisterInputHandle(InputHandle* pkInputHandle)
{
   for ( vector<InputHandle*>::iterator kIte = m_kInputHandles.begin(); kIte != m_kInputHandles.end(); kIte++ )	
	{	
		if((*kIte) == pkInputHandle)
		{
			
			m_kInputHandles.erase(kIte);
			cout<<"InputHandle "<<pkInputHandle->m_strHandleName<<" unregistered"<<endl;			
			return true;
		}		
	}
	
	return false;	
}

InputHandle* Input::GetInputHandle(string strName)
{
	for(unsigned int i = 0;i<m_kInputHandles.size();i++)
	{
		if(m_kInputHandles[i]->m_strHandleName == strName)
			return m_kInputHandles[i];
	}
	
	return NULL;
}

void Input::PrintInputHandlers()
{
	cout<<"Registered input handles"<<endl;

	for(unsigned int i = 0;i<m_kInputHandles.size();i++)
	{
		cout<<m_kInputHandles[i]->m_strHandleName;
	
		if(m_kInputHandles[i]->m_bActive)
			cout<<"  Active";
			
		cout<<endl;
	}
}

void Input::StartBindMode(string strBindKey, int iBindIndex)
{
	m_bBindMode		= true;
	m_strBindKey	= strBindKey;
	m_iBindKeyIndex = iBindIndex;
}

void Input::BindBindMode(int iKey)
{
	m_bBindMode = false;
	VKBind(m_strBindKey, (Buttons)iKey, m_iBindKeyIndex);
}

void Input::Update(void) 
{
	m_iMouseX=-1;	
	m_iMouseY=-1;
	
	static int bah = 0;
	bah++;
	
	UpdateInputHandles();
	UpdateMousePos();
	
	//release mousewheel buttons (to make sure mousewheels are pressed atleast one frame
	if(m_bHaveReleasedMWUP)
		m_akKeyState[MOUSEWUP].m_bDown=false;
	if(m_bHaveReleasedMWDOWN)
		m_akKeyState[MOUSEWDOWN].m_bDown=false;
		
	
	Buttons iZfKey;
	while(SDL_PollEvent(&m_kEvent)) {
		switch(m_kEvent.type) {
			//keyboard
			case SDL_KEYDOWN:
				//add key to queuedkeys
				AddQueuedKey(&m_kEvent.key.keysym,true);

				//set button as pressed		
				iZfKey = (Buttons) SDLToZeroFpsKey(m_kEvent.key.keysym.sym);

				//linux backquote hack deluxe
				if(m_kEvent.key.keysym.sym == 167)
					iZfKey = KEY_BACKQUOTE;
								
				//set status up
				m_akKeyState[iZfKey].m_bDown = true;	

				if(m_bBindMode)
					BindBindMode( SDLToZeroFpsKey(m_kEvent.key.keysym.sym) );
				
				break;			

			case SDL_KEYUP:
				//set  button as unpressed
				AddQueuedKey(&m_kEvent.key.keysym,false);
				
				//set status up
				m_akKeyState[SDLToZeroFpsKey(m_kEvent.key.keysym.sym) ].m_bDown = false;	
    		break;    		
 
	    	//mouse    		
   	 	case SDL_MOUSEBUTTONDOWN:
				//cout << "SDL_MOUSEBUTTONDOWN: " << (int) m_kEvent.button.button << endl;
    			switch(m_kEvent.button.button)
				{
    				case SDL_BUTTON_LEFT:		m_akKeyState[MOUSELEFT].m_bDown = true;	break;
   	 			case SDL_BUTTON_MIDDLE:		m_akKeyState[MOUSEMIDDLE].m_bDown=true;	break;
    				case SDL_BUTTON_RIGHT:		m_akKeyState[MOUSERIGHT].m_bDown=true;		break;	
    				case SDL_BUTTON_WHEELUP:	
						m_akKeyState[MOUSEWUP].m_bDown=true;		
						m_bHaveReleasedMWUP = false;
						break;	
    				case SDL_BUTTON_WHEELDOWN:	
						m_akKeyState[MOUSEWDOWN].m_bDown=true;
						m_bHaveReleasedMWDOWN = false;
						break;	
				}
				
				if(m_bBindMode)
					BindBindMode( SDLToZeroFpsKey(m_kEvent.button.button) );
				break;

	    	case SDL_MOUSEBUTTONUP:
 				//cout << "SDL_MOUSEBUTTONUP: " << (int) m_kEvent.button.button << endl;

				switch(m_kEvent.button.button)
				{
    				case SDL_BUTTON_LEFT:		m_akKeyState[MOUSELEFT].m_bDown=false;		break;
	    			case SDL_BUTTON_MIDDLE:		m_akKeyState[MOUSEMIDDLE].m_bDown=false;	break;
    				case SDL_BUTTON_RIGHT:		m_akKeyState[MOUSERIGHT].m_bDown=false;	break;	
    				case SDL_BUTTON_WHEELUP:	
						m_bHaveReleasedMWUP = true;
						//m_akKeyState[MOUSEWUP].m_bDown=false;		
						break;	
    				case SDL_BUTTON_WHEELDOWN:	
						m_bHaveReleasedMWDOWN = true;
						//m_akKeyState[MOUSEWDOWN].m_bDown=false;	
						break;	
   	 		}    	
    			break;
		}	
	}
}

void Input::AddQueuedKey(SDL_keysym* kKey,bool bPressed)
{
	int iModifier = 0;
	
	//check modifiers
	if(kKey->mod & KMOD_CTRL)
		iModifier = iModifier | MODIFIER_CTRL;
	if(kKey->mod & KMOD_SHIFT)
		iModifier = iModifier | MODIFIER_SHIFT;
	if(kKey->mod & KMOD_ALT)
		iModifier = iModifier | MODIFIER_ALT;
	if(kKey->mod & KMOD_META)
		iModifier = iModifier | MODIFIER_META;
		

	//put key in list
	m_aPressedKeys.push(  QueuedKeyInfo(SDLToZeroFpsKey(kKey->sym),iModifier,bPressed)  );	
	//cout << "Key Qued: " << kKey->sym <<"  modifiers:"<<iModifier<< endl;


	//make sure the queue does't get to big
	while(m_aPressedKeys.size()>m_iQueueLength)
		m_aPressedKeys.pop();	

}

VKData* Input::GetVKByName(string strName)
{
	for(unsigned int i=0; i<m_VirtualKeys.size(); i++) {
		if(m_VirtualKeys[i].m_strName == strName)
			return &m_VirtualKeys[i];
		}
	
	return NULL;
}

bool Input::VKBind(string strName, Buttons kKey, int iIndex)
{
	// Make sure name is not to long and that the key is not outside the valid range.
	if( (strName.length() + 1) >= MAX_KEYNAME)
		return false;
	if(iIndex < 0 || iIndex >= VKMAPS) 
		return false;

	VKData* pkVk = GetVKByName(strName);
	if(pkVk) 
	{
		pkVk->m_iInputKey[iIndex] = kKey;
	}
	else
	{
	   VKData	kVk;
		kVk.m_strName = strName;
		kVk.m_iInputKey[0] = kVk.m_iInputKey[1] = kVk.m_iInputKey[2] = 0;
		kVk.m_iInputKey[iIndex] = kKey;
		//kVk.m_iInputKey[1] = 0;
		//kVk.m_iInputKey[2] = 0;
		m_VirtualKeys.push_back(kVk);
	}
	

	return true;	//how could this possible hapen
}

bool Input::VKBind(string strName, string strKeyName,int iIndex)
{
	Buttons eKey = GetNameByKey( strKeyName );
	return VKBind(strName, eKey, iIndex);
}


bool Input::VKIsDown(string strName)
{
	VKData* pkVk = GetVKByName(strName);
	
	if(!pkVk)
		return false;

	bool bKeyDown = false;
	//cout << "Checking" << strName << ": ";
	for(int i=0; i<VKMAPS;i++) {
		//cout << pkVk->m_iInputKey[i] << ", ";
		bKeyDown |= m_akKeyState[pkVk->m_iInputKey[i]].m_bDown;
		}

	//cout << endl;
	return bKeyDown;
}

void Input::VKList()
{
	if(!GetConsole())	return;

	m_pkConsole->Printf("VK List");	
	string Key1, Key2, Key3;
	
	for(unsigned int i=0; i<m_VirtualKeys.size(); i++) {
		Key1 = GetKeyName((Buttons) m_VirtualKeys[i].m_iInputKey[0]);
		Key2 = GetKeyName((Buttons) m_VirtualKeys[i].m_iInputKey[1]);
		Key3 = GetKeyName((Buttons) m_VirtualKeys[i].m_iInputKey[2]);

		m_pkConsole->Printf(" %s : %s, %s, %s",m_VirtualKeys[i].m_strName.c_str(), 
			Key1.c_str(), Key2.c_str(), Key3.c_str());	
		}
}



Buttons Input::GetNameByKey(string strName)
{
	for(int i=0; i<MAX_KEYS; i++) {
		if(m_akKeyState[i].m_strName == strName)
			return (Buttons) i;
		}

	return (Buttons) 0;
}

string  Input::GetKeyName(Buttons eKey)
{
	return m_akKeyState[eKey].m_strName;
}

Buttons Input::GetKeyCode(string strName)
{
	return KEY_F1;
}



void Input::MouseXY(int &iX,int &iY) 
{		
	iX=m_iAbsMouseX;
	iY=m_iAbsMouseY;
}

void Input::UnitMouseXY(float &fX,float &fY) 
{	
	fX = float( (m_iAbsMouseX/(float)m_pkRender->GetWidth())-0.5 );
	fY = float( (m_iAbsMouseY/(float)m_pkRender->GetHeight())-0.5 );
}


void Input::UpdateMousePos()
{		
	SDL_GetMouseState(&m_iSDLMouseX,&m_iSDLMouseY);


	int width=m_pkRender->GetWidth();
	int height=m_pkRender->GetHeight();

	int relx;
	int rely;
	
	RelMouseXY(relx,rely);
	
	m_iAbsMouseX+=int(float(relx)*m_fMouseSensitivity);
	m_iAbsMouseY+=int(float(rely)*m_fMouseSensitivity);	
	
	
	if(m_iAbsMouseX <0)
		m_iAbsMouseX = 0;	
	if(m_iAbsMouseY <0)
		m_iAbsMouseY = 0;
		
	if(m_iAbsMouseX > width)
		m_iAbsMouseX = width;
	
	if(m_iAbsMouseY >height)
		m_iAbsMouseY = height;
		
}

void Input::SDLMouseXY(int &iX,int &iY)
{

		iX = m_iSDLMouseX;
		iY = m_iSDLMouseY;
}

void Input::RelMouseXY(int &iX,int &iY) 
{		

		if(m_iMouseX==-1)
			SDL_GetRelativeMouseState(&m_iMouseX, &m_iMouseY);		
		
		iX=int(float(m_iMouseX)*m_fMouseSensitivity);	
		iY=int(float(m_iMouseY)*m_fMouseSensitivity);		

}

void Input::ToggleGrab(void) 
{	
	if(SDL_GetTicks()-m_iGrabtime>200) 
	{
		m_iGrabtime=SDL_GetTicks();
		if(SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_OFF) 
		{
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


void Input::GrabInput(void) 
{
	SDL_WM_GrabInput(SDL_GRAB_ON);
}

void Input::ReleaseInput(void) 
{
	SDL_WM_GrabInput(SDL_GRAB_OFF);
}
	

void Input::Reset(void) {
	SDL_Event kTemp;

	while(SDL_PollEvent(&kTemp)){
//		cout<<"flushing"<<endl;
	}
	
	for(int i =0;i<400;i++) 
		m_akKeyState[i].m_bDown=false;
	
	//clear queue
	while(!m_aPressedKeys.empty()){
		m_aPressedKeys.pop();
	}
}




void Input::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_TOGGLEGRAB:
			ToggleGrab();
			break;
		
		case FID_BINDVK:
			if(kCommand->m_kSplitCommand.size() >= 3)
			{
				bool nice = true;
				
				if(!VKBind(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2],0))
					nice = false;
			
				if(kCommand->m_kSplitCommand.size() >= 4)
					if(!VKBind(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[3],1))
						nice = false;
								
				if(kCommand->m_kSplitCommand.size() >= 5)
					if(!VKBind(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[4],2))
						nice = false;
			
				if(nice)
					m_pkConsole->Printf("bind succes %s", kCommand->m_kSplitCommand[1].c_str() );
					//cout<<"Bind succes "<<kCommand->m_kSplitCommand[1]<<endl;
				else
					m_pkConsole->Printf("bind fail %s", kCommand->m_kSplitCommand[1].c_str() );
					//cout<<"Bind fail "<<kCommand->m_kSplitCommand[1]<<endl;
				
			}
			else			
				if(GetConsole())
					m_pkConsole->Printf("bind <vkey> <key> (<key> <key>  optional)");	
					
			break;

		case FID_VKBINDLIST:		VKList();											break;
		case FID_SAVE:				Save(kCommand->m_kSplitCommand[1]);		break;
		case FID_LOAD:				Load(kCommand->m_kSplitCommand[1]);		break;

		case FID_MOUSESENS:
			if(kCommand->m_kSplitCommand.size()!=2)
				if(GetConsole())
					m_pkConsole->Printf("MouseSensitivity: %.3f", m_fMouseSensitivity);
			else
				m_fMouseSensitivity = float( atof(kCommand->m_kSplitCommand[1].c_str()) );
			break;
	}
}

void Input::Save(string strCfgName)
{
	char szVkName[MAX_KEYNAME];

	ZFVFile kFile;
	kFile.Open(strCfgName,0,true);
	
	string Key1, Key2, Key3;
	

	// Write total number of key configs.
	int iNumOfVkeys = m_VirtualKeys.size();
	kFile.Write(&iNumOfVkeys , sizeof(int),1);

	for(unsigned int i=0; i<m_VirtualKeys.size(); i++) 
	{
		Key1 = GetKeyName((Buttons) m_VirtualKeys[i].m_iInputKey[0]);
		Key2 = GetKeyName((Buttons) m_VirtualKeys[i].m_iInputKey[1]);
		Key3 = GetKeyName((Buttons) m_VirtualKeys[i].m_iInputKey[2]);

		// Write Name.
		strcpy(szVkName, m_VirtualKeys[i].m_strName.c_str());
		kFile.Write(szVkName, sizeof(char), MAX_KEYNAME);

		// Write Keys
		strcpy(szVkName, Key1.c_str());
		kFile.Write(szVkName, sizeof(char), MAX_KEYNAME);
		strcpy(szVkName, Key2.c_str());
		kFile.Write(szVkName, sizeof(char), MAX_KEYNAME);
		strcpy(szVkName, Key3.c_str());
		kFile.Write(szVkName, sizeof(char), MAX_KEYNAME);
	}

	kFile.Close();
}

void Input::Load(string strCfgName)
{
	ZFVFile kFile;
	kFile.Open(strCfgName,0,false);

	int iNumOfVkeys = m_VirtualKeys.size();
	kFile.Read(&iNumOfVkeys , sizeof(int),1);

	char szVkName[MAX_KEYNAME];
	char szKey1[MAX_KEYNAME];
	char szKey2[MAX_KEYNAME];
	char szKey3[MAX_KEYNAME];

	for(unsigned int i=0; i<iNumOfVkeys; i++) 
	{
		kFile.Read(szVkName, sizeof(char), MAX_KEYNAME);

		// Write Keys
		kFile.Read(szKey1, sizeof(char), MAX_KEYNAME);
		kFile.Read(szKey2, sizeof(char), MAX_KEYNAME);
		kFile.Read(szKey3, sizeof(char), MAX_KEYNAME);

		VKBind(szVkName, szKey1, 0);
		VKBind(szVkName, szKey1, 1);
		VKBind(szVkName, szKey1, 2);
	}

	kFile.Close();
}


QueuedKeyInfo Input::GetQueuedKey()
{
	QueuedKeyInfo kKey(-1,0,true);	

	if(!m_aPressedKeys.empty())
	{
		kKey = m_aPressedKeys.front();
		m_aPressedKeys.pop();

/*			if(value < 0)
			{
				printf("GetQueuedKey: value < 0");
			}
*/
			//cout << "GetQueuedKey: " << value << endl;

			return kKey;
	}
	
	return kKey;
}

int Input::SizeOfQueue()
{
	return m_aPressedKeys.size();
}

bool Input::Pressed(Buttons eButton)
{
	return m_akKeyState[eButton].m_bDown;
}



bool Input::GetConsole()
{
	m_pkConsole	= static_cast<BasicConsole*>(GetSystem().GetObjectPtr("Console"));
	
	if(m_pkConsole==NULL) 
		return false;
	else 
		return true;
}


void Input::SetCursorInputPos(int x, int y)
{
	m_iAbsMouseX=x;
	m_iAbsMouseY=y;

//	SDL_WarpMouse(x,y);
}

void Input::ShowCursor(bool bShow)
{
	if(bShow)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
}

int  Input::SDLToZeroFpsKey(int iSdlSym)
{
	if(iSdlSym < 0)				assert(0);
	if(iSdlSym >=MAX_SDLKEYS)	assert(0);
	
	return m_akMapToKeyState[ iSdlSym ];
}

void Input::SetupMapToKeyState()
{
	memset(m_akMapToKeyState, 0, sizeof(int) * MAX_SDLKEYS);

	m_akMapToKeyState[ SDLK_ESCAPE ] = KEY_ESCAPE;

	/* Function keys */
	m_akMapToKeyState[ SDLK_F1 ]	= KEY_F1;
	m_akMapToKeyState[ SDLK_F2 ]	= KEY_F2;
	m_akMapToKeyState[ SDLK_F3 ]	= KEY_F3;
	m_akMapToKeyState[ SDLK_F4 ]	= KEY_F4;
	m_akMapToKeyState[ SDLK_F5 ]	= KEY_F5;
	m_akMapToKeyState[ SDLK_F6 ]	= KEY_F6;
	m_akMapToKeyState[ SDLK_F7 ]	= KEY_F7;
	m_akMapToKeyState[ SDLK_F8 ]	= KEY_F8;
	m_akMapToKeyState[ SDLK_F9 ]	= KEY_F9;
	m_akMapToKeyState[ SDLK_F10 ] = KEY_F10;
	m_akMapToKeyState[ SDLK_F11 ] = KEY_F11;
	m_akMapToKeyState[ SDLK_F12 ] = KEY_F12;
	m_akMapToKeyState[ SDLK_F13 ] = KEY_F13;
	m_akMapToKeyState[ SDLK_F14 ] = KEY_F14;
	m_akMapToKeyState[ SDLK_F15 ] = KEY_F15;
	
	// Print sqr
	m_akMapToKeyState[ SDLK_SCROLLOCK ] = KEY_SCROLLLOCK;
	// Pause

	// Half?
   m_akMapToKeyState[ SDLK_1 ] = KEY_1;
   m_akMapToKeyState[ SDLK_2 ] = KEY_2;
   m_akMapToKeyState[ SDLK_3 ] = KEY_3;
   m_akMapToKeyState[ SDLK_4 ] = KEY_4;
   m_akMapToKeyState[ SDLK_5 ] = KEY_5;
   m_akMapToKeyState[ SDLK_6 ] = KEY_6;
   m_akMapToKeyState[ SDLK_7 ] = KEY_7;
   m_akMapToKeyState[ SDLK_8 ] = KEY_8;
   m_akMapToKeyState[ SDLK_9 ] = KEY_9;
   m_akMapToKeyState[ SDLK_0 ] = KEY_0;
   m_akMapToKeyState[ SDLK_BACKSPACE ] = KEY_BACKSPACE;

	m_akMapToKeyState[ SDLK_INSERT ] = KEY_INSERT;
	m_akMapToKeyState[ SDLK_HOME ] = KEY_HOME;
	m_akMapToKeyState[ SDLK_PAGEUP ] = KEY_PAGEUP;
	m_akMapToKeyState[ SDLK_DELETE ] = KEY_DELETE;
	m_akMapToKeyState[ SDLK_END ] = KEY_END;
	m_akMapToKeyState[ SDLK_PAGEDOWN ] = KEY_PAGEDOWN;

	m_akMapToKeyState[ SDLK_TAB ] = KEY_TAB;
	m_akMapToKeyState[ SDLK_q ] = KEY_Q;
	m_akMapToKeyState[ SDLK_w ] = KEY_W;
	m_akMapToKeyState[ SDLK_e ] = KEY_E;
	m_akMapToKeyState[ SDLK_r ] = KEY_R;
	m_akMapToKeyState[ SDLK_t ] = KEY_T;
	m_akMapToKeyState[ SDLK_y ] = KEY_Y;
	m_akMapToKeyState[ SDLK_u ] = KEY_U;
	m_akMapToKeyState[ SDLK_i ] = KEY_I;
	m_akMapToKeyState[ SDLK_o ] = KEY_O;
	m_akMapToKeyState[ SDLK_p ] = KEY_P;
	m_akMapToKeyState[ SDLK_RETURN ] = KEY_RETURN;


	m_akMapToKeyState[ SDLK_a ] = KEY_A;
	m_akMapToKeyState[ SDLK_s ] = KEY_S;
	m_akMapToKeyState[ SDLK_d ] = KEY_D;
	m_akMapToKeyState[ SDLK_f ] = KEY_F;
	m_akMapToKeyState[ SDLK_g ] = KEY_G;
	m_akMapToKeyState[ SDLK_h ] = KEY_H;
	m_akMapToKeyState[ SDLK_j ] = KEY_J;
	m_akMapToKeyState[ SDLK_k ] = KEY_K;
	m_akMapToKeyState[ SDLK_l ] = KEY_L;

	m_akMapToKeyState[ SDLK_LSHIFT ] = KEY_LSHIFT;
	m_akMapToKeyState[ SDLK_z ] = KEY_Z;
	m_akMapToKeyState[ SDLK_x ] = KEY_X;
	m_akMapToKeyState[ SDLK_c ] = KEY_C;
	m_akMapToKeyState[ SDLK_v ] = KEY_V;
	m_akMapToKeyState[ SDLK_b ] = KEY_B;
	m_akMapToKeyState[ SDLK_n ] = KEY_N;
	m_akMapToKeyState[ SDLK_m ] = KEY_M;
	m_akMapToKeyState[ SDLK_RSHIFT ] = KEY_RSHIFT;

	m_akMapToKeyState[ SDLK_LCTRL ] = KEY_LCTRL;
	m_akMapToKeyState[ SDLK_LALT ]  = KEY_LALT;
	m_akMapToKeyState[ SDLK_SPACE ] = KEY_SPACE;
	m_akMapToKeyState[ SDLK_RALT ]  = KEY_RALT;
	m_akMapToKeyState[ SDLK_RCTRL ] = KEY_RCTRL;

	m_akMapToKeyState[ SDLK_UP ]		= KEY_UP;
	m_akMapToKeyState[ SDLK_LEFT ]	= KEY_LEFT;
	m_akMapToKeyState[ SDLK_RIGHT ]	= KEY_RIGHT;
	m_akMapToKeyState[ SDLK_DOWN ]	= KEY_DOWN;


	m_akMapToKeyState[ SDLK_EXCLAIM ]	= KEY_EXCLAIM;
	m_akMapToKeyState[ SDLK_QUOTEDBL ]	= KEY_QUOTEDBL;
	m_akMapToKeyState[ SDLK_HASH ]		= KEY_HASH;
	m_akMapToKeyState[ SDLK_DOLLAR ]		= KEY_DOLLAR;
	m_akMapToKeyState[ SDLK_AMPERSAND ] = KEY_AMPERSAND;
	m_akMapToKeyState[ SDLK_QUOTE ]		= KEY_QUOTE;
	m_akMapToKeyState[ SDLK_LEFTPAREN ] = KEY_LEFTPAREN;
	m_akMapToKeyState[ SDLK_RIGHTPAREN ] = KEY_RIGHTPAREN;
	m_akMapToKeyState[ SDLK_ASTERISK ]	= KEY_ASTERISK;
	m_akMapToKeyState[ SDLK_PLUS ]	= KEY_PLUS;
	m_akMapToKeyState[ SDLK_COMMA ]	= KEY_COMMA;
	m_akMapToKeyState[ SDLK_MINUS ]	= KEY_MINUS;
	m_akMapToKeyState[ SDLK_PERIOD ]	= KEY_PERIOD;
	m_akMapToKeyState[ SDLK_SLASH ]	= KEY_SLASH;

	m_akMapToKeyState[ SDLK_COLON ]	= KEY_COLON;
	m_akMapToKeyState[ SDLK_SEMICOLON ]	= KEY_SEMICOLON;
	m_akMapToKeyState[ SDLK_LESS ]	= KEY_LESS;
	m_akMapToKeyState[ SDLK_EQUALS ]	= KEY_EQUALS;
	m_akMapToKeyState[ SDLK_GREATER ]	= KEY_GREATER;
	m_akMapToKeyState[ SDLK_QUESTION ]	= KEY_QUESTION;
	m_akMapToKeyState[ SDLK_AT ]	= KEY_AT;
	m_akMapToKeyState[ SDLK_LEFTBRACKET ]	= KEY_LEFTBRACKET;
	m_akMapToKeyState[ SDLK_BACKSLASH ]	= KEY_BACKSLASH;
	m_akMapToKeyState[ SDLK_RIGHTBRACKET ]	= KEY_RIGHTBRACKET;
	m_akMapToKeyState[ SDLK_CARET ]	= KEY_CARET;
	m_akMapToKeyState[ SDLK_UNDERSCORE ]	= KEY_UNDERSCORE;
	m_akMapToKeyState[ SDLK_BACKQUOTE ]	= KEY_BACKQUOTE;

	m_akMapToKeyState[ SDLK_KP0 ]			= KEY_KP0;
	m_akMapToKeyState[ SDLK_KP1 ]			= KEY_KP1;
	m_akMapToKeyState[ SDLK_KP2 ]			= KEY_KP2;
	m_akMapToKeyState[ SDLK_KP3 ]			= KEY_KP3;
	m_akMapToKeyState[ SDLK_KP4 ]			= KEY_KP4;
	m_akMapToKeyState[ SDLK_KP5 ]			= KEY_KP5;
	m_akMapToKeyState[ SDLK_KP6 ]			= KEY_KP6;
	m_akMapToKeyState[ SDLK_KP7 ]			= KEY_KP7;
	m_akMapToKeyState[ SDLK_KP8 ]			= KEY_KP8;
	m_akMapToKeyState[ SDLK_KP9 ]			= KEY_KP9;
	m_akMapToKeyState[ SDLK_KP_PERIOD ]		= KEY_KP_PERIOD;
	m_akMapToKeyState[ SDLK_KP_DIVIDE ]		= KEY_KP_DIVIDE;
	m_akMapToKeyState[ SDLK_KP_MULTIPLY ]	= KEY_KP_MULTIPLY;
	m_akMapToKeyState[ SDLK_KP_MINUS ]		= KEY_KP_MINUS;
	m_akMapToKeyState[ SDLK_KP_PLUS ]		= KEY_KP_PLUS;
	m_akMapToKeyState[ SDLK_KP_ENTER ]		= KEY_KP_ENTER;
	m_akMapToKeyState[ SDLK_KP_EQUALS ]		= KEY_KP_EQUALS;
	
	m_akMapToKeyState[ SDLK_NUMLOCK ]	= KEY_NUMLOCK;
	m_akMapToKeyState[ SDLK_CAPSLOCK ]	= KEY_CAPSLOCK;

	m_akMapToKeyState[ SDLK_NUMLOCK ]	= KEY_NUMLOCK;


	m_akMapToKeyState[ SDL_BUTTON_LEFT ]	= MOUSELEFT;
	m_akMapToKeyState[ SDL_BUTTON_MIDDLE ]	= MOUSEMIDDLE;
	m_akMapToKeyState[ SDL_BUTTON_RIGHT ]	= MOUSERIGHT;

	m_akMapToKeyState[ SDL_BUTTON_WHEELUP ]	= MOUSEWUP;
	m_akMapToKeyState[ SDL_BUTTON_WHEELDOWN ]	= MOUSEWDOWN;

	m_akKeyState[KEY_ESCAPE].m_strName = "escape";

	m_akKeyState[KEY_F1].m_strName = "f1";
	m_akKeyState[KEY_F2].m_strName = "f2";
	m_akKeyState[KEY_F3].m_strName = "f3";
	m_akKeyState[KEY_F4].m_strName = "f4";
	m_akKeyState[KEY_F5].m_strName = "f5";
	m_akKeyState[KEY_F6].m_strName = "f6";
	m_akKeyState[KEY_F7].m_strName = "f7";
	m_akKeyState[KEY_F8].m_strName = "f8";
	m_akKeyState[KEY_F9].m_strName = "f9";
	m_akKeyState[KEY_F10].m_strName = "f10";
	m_akKeyState[KEY_F11].m_strName = "f11";
	m_akKeyState[KEY_F12].m_strName = "f12";
	m_akKeyState[KEY_F13].m_strName = "f13";
	m_akKeyState[KEY_F14].m_strName = "f14";
	m_akKeyState[KEY_F15].m_strName = "f15";

	m_akKeyState[KEY_SCROLLLOCK].m_strName = "scrolllock";
	m_akKeyState[KEY_1].m_strName = "1";
	m_akKeyState[KEY_2].m_strName = "2";
	m_akKeyState[KEY_3].m_strName = "3";
	m_akKeyState[KEY_4].m_strName = "4";
	m_akKeyState[KEY_5].m_strName = "5";
	m_akKeyState[KEY_6].m_strName = "6";
	m_akKeyState[KEY_7].m_strName = "7";
	m_akKeyState[KEY_8].m_strName = "8";
	m_akKeyState[KEY_9].m_strName = "9";
	m_akKeyState[KEY_0].m_strName = "0";
	m_akKeyState[KEY_BACKSPACE].m_strName = "backspace";

	m_akKeyState[KEY_INSERT].m_strName		= "insert";
	m_akKeyState[KEY_HOME].m_strName			= "home";
	m_akKeyState[KEY_PAGEUP].m_strName		= "pageup";
	m_akKeyState[KEY_PAGEDOWN].m_strName	= "pagedown";	
	m_akKeyState[KEY_DELETE].m_strName		= "delete";
	m_akKeyState[KEY_END].m_strName			= "end";

	m_akKeyState[KEY_TAB].m_strName = "tab";
	m_akKeyState[KEY_Q].m_strName = "q";
	m_akKeyState[KEY_W].m_strName = "w";
	m_akKeyState[KEY_E].m_strName = "e";
	m_akKeyState[KEY_R].m_strName = "r";
	m_akKeyState[KEY_T].m_strName = "t";
	m_akKeyState[KEY_Y].m_strName = "y";
	m_akKeyState[KEY_U].m_strName = "u";
	m_akKeyState[KEY_I].m_strName = "i";
	m_akKeyState[KEY_O].m_strName = "o";
	m_akKeyState[KEY_P].m_strName = "p";
	m_akKeyState[KEY_RETURN].m_strName = "return";

	m_akKeyState[KEY_A].m_strName = "a";
	m_akKeyState[KEY_S].m_strName = "s";
	m_akKeyState[KEY_D].m_strName = "d";
	m_akKeyState[KEY_F].m_strName = "f";
	m_akKeyState[KEY_G].m_strName = "g";
	m_akKeyState[KEY_H].m_strName = "h";
	m_akKeyState[KEY_J].m_strName = "j";
	m_akKeyState[KEY_K].m_strName = "k";
	m_akKeyState[KEY_L].m_strName = "l";

	m_akKeyState[KEY_LSHIFT].m_strName			= "lshift";
	m_akKeyState[KEY_Z].m_strName					= "z";
	m_akKeyState[KEY_X].m_strName					= "x";
	m_akKeyState[KEY_C].m_strName					= "c";
	m_akKeyState[KEY_V].m_strName					= "v";
	m_akKeyState[KEY_B].m_strName					= "b";
	m_akKeyState[KEY_N].m_strName					= "n";
	m_akKeyState[KEY_M].m_strName					= "m";
	m_akKeyState[KEY_RSHIFT].m_strName			= "rshift";

	m_akKeyState[KEY_LCTRL].m_strName			= "lctrl";
	m_akKeyState[KEY_LALT].m_strName				= "lalt";
	m_akKeyState[KEY_SPACE].m_strName			= "space";
	m_akKeyState[KEY_RALT].m_strName				= "ralt";
	m_akKeyState[KEY_RCTRL].m_strName			= "rctrl";

	m_akKeyState[KEY_UP].m_strName				= "up";
	m_akKeyState[KEY_LEFT].m_strName				= "left";
	m_akKeyState[KEY_RIGHT].m_strName			= "right";
	m_akKeyState[KEY_DOWN].m_strName				= "down";

	m_akKeyState[KEY_EXCLAIM].m_strName			= "exclaim";
	m_akKeyState[KEY_QUOTEDBL].m_strName		= "quotedbl";
	m_akKeyState[KEY_HASH].m_strName				= "hash";
	m_akKeyState[KEY_DOLLAR].m_strName			= "dollar";
	m_akKeyState[KEY_AMPERSAND].m_strName		= "ampersand";
	m_akKeyState[KEY_QUOTE].m_strName			= "quote";
	m_akKeyState[KEY_LEFTPAREN].m_strName		= "(";
	m_akKeyState[KEY_RIGHTPAREN].m_strName		= ")";
	m_akKeyState[KEY_ASTERISK].m_strName		= "";
	m_akKeyState[KEY_PLUS].m_strName				= "+";
	m_akKeyState[KEY_COMMA].m_strName			= ",";
	m_akKeyState[KEY_MINUS].m_strName			= "";
	m_akKeyState[KEY_PERIOD].m_strName			= ".";
	m_akKeyState[KEY_SLASH].m_strName			= "/";

	m_akKeyState[KEY_COLON].m_strName			= "";
	m_akKeyState[KEY_SEMICOLON].m_strName		= "semicolon";
	m_akKeyState[KEY_LESS].m_strName				= "<";
	m_akKeyState[KEY_EQUALS].m_strName			= "=";
	m_akKeyState[KEY_GREATER].m_strName			= ">";
	m_akKeyState[KEY_QUESTION].m_strName		= "question";
	m_akKeyState[KEY_AT].m_strName				= "at";
	m_akKeyState[KEY_LEFTBRACKET].m_strName	= "[";
	m_akKeyState[KEY_BACKSLASH].m_strName		= "\\";
	m_akKeyState[KEY_RIGHTBRACKET].m_strName	= "]";
	m_akKeyState[KEY_CARET].m_strName			= "-";
	m_akKeyState[KEY_UNDERSCORE].m_strName		= "_";
	m_akKeyState[KEY_BACKQUOTE].m_strName		= "backquote";

	m_akKeyState[KEY_KP0].m_strName				= "kp0";
	m_akKeyState[KEY_KP1].m_strName				= "kp1";
	m_akKeyState[KEY_KP2].m_strName				= "kp2";
	m_akKeyState[KEY_KP3].m_strName				= "kp3";
	m_akKeyState[KEY_KP4].m_strName				= "kp4";
	m_akKeyState[KEY_KP5].m_strName				= "kp5";
	m_akKeyState[KEY_KP6].m_strName				= "kp6";
	m_akKeyState[KEY_KP7].m_strName				= "kp7";
	m_akKeyState[KEY_KP8].m_strName				= "kp8";
	m_akKeyState[KEY_KP9].m_strName				= "kp9";

	m_akKeyState[KEY_KP_PERIOD].m_strName		= "kpdelete";
	m_akKeyState[KEY_KP_DIVIDE].m_strName		= "kpslash";
	m_akKeyState[KEY_KP_MULTIPLY].m_strName	= "kp*";
	m_akKeyState[KEY_KP_MINUS].m_strName		= "kpminus";
	m_akKeyState[KEY_KP_PLUS].m_strName			= "kpplus";
	m_akKeyState[KEY_KP_ENTER].m_strName		= "kpenter";
	m_akKeyState[KEY_KP_EQUALS].m_strName		= "";

	m_akKeyState[KEY_NUMLOCK].m_strName			= "kpnumlock";
	m_akKeyState[KEY_CAPSLOCK].m_strName		= "capslock";

	m_akKeyState[MOUSELEFT].m_strName			= "mouseleft";
	m_akKeyState[MOUSEMIDDLE].m_strName			= "mousemiddle";
	m_akKeyState[MOUSERIGHT].m_strName			= "mouseright";
	m_akKeyState[MOUSEWUP].m_strName				= "mousewup";
	m_akKeyState[MOUSEWDOWN].m_strName			= "mousewdown";

//	m_akKeyState[KEY_].m_strName = "";
}

