// dark_metropolis.h

#ifndef _DARK_METROPOLIS_H_
#define _DARK_METROPOLIS_H_

#include <iostream>
#include <cmath>
#include <list>

using namespace std;

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/propertys/p_camera.h"

//#include "../mcommon/p_clientcontrol.h"
#include "../mcommon/p_event.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../mcommon/p_dmhq.h"
#include "../mcommon/p_dmcharacter.h"
#include "../mcommon/p_shadowblob.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "p_dmgameinfo.h"


bool GUIPROC(ZGuiWnd* win, unsigned int msg, int numparms, void *params );

class DarkMetropolis : public Application, public ZGuiApp 
{
	private:
		
		enum FuncId_e
		{
			FID_LOAD,
			FID_SAVE,
		};

		struct StartBaseInfo {
			ZGuiSkin* pkIcon;
			char* szName;
		};
		
		enum Formations
		{
			FORMATION_CIRCLE = 0,
			FORMATION_SQUARE = 1,
		
		};
	
		vector<StartBaseInfo*> m_vkStartBaseList;
		vector<StartBaseInfo*>::iterator m_itStartBase;

		P_DMGameInfo*	m_pkGameInfoProperty;
		Entity*			m_pkGameInfoEntity;

		LightSource	m_kSun;

		Camera*	m_pkCamera;
		Entity*	m_pkCameraEntity;
		P_Camera* m_pkCameraProp;
		float		m_fDistance;		
		float		m_fAngle;
		float		m_fMinCamDistance;
		float		m_fMaxCamDistance;
		
		float		m_fDelayTimer;
		bool		m_bActionPressed;
		
		vector<int>	m_kSelectedEntitys;					//list of selected entitys		
		int			m_iCurrentFormation;					//what formation to use when moving characters
		
		int			m_iHQID;									//network id of current selected hq, if any
		
		Vector3		m_kSelectSquareStart;				//start of selection square
		Vector3		m_kSelectSquareStop;					//stop/current pos of selection square
		bool			m_bSelectSquare;						//are we currently drawing a selection square
		
		Vector3	m_kPickPos;									//exact possition of last cursor pick operation
		string	m_strSaveDirectory;						//directory in wich savegames are stored

		int m_iEnableMusic; 									//if the music should be played or not
		
		
		Vector3 GetFormationPos(int iType,int iTotal,int iPos);
		
		//picking
		Vector3	Get3DMousePos(bool m_bMouse);		
		Entity*	GetTargetObject();			
		
		void Input();
		void RegisterPropertys();
		
		bool StartNewGame(string strClanName,string strClanColor);
		bool LoadGame(string strClanName);
		bool SaveGame(string strsavegame);

		void GUI_LoadSave(bool bSave);
		bool GUI_NewGame(ZGuiWnd *pkMainWnd);
		bool m_bSaveGame; // gui state variable
		
		void StartSong(char* szName);
	
	public:
		DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth);
		
		// Application, network and system stuff
		void OnInit();
		void OnIdle();
		void OnHud();
		void OnSystem();
		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass) { return true; }
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void OnServerStart();
		void OnClientStart();
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		void RenderInterface();

		// Gui
		void GUI_OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd);
		void GUI_OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
		void GUI_OnDoubleClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain);
		void GUI_OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain);
		void GUI_OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
		void GUI_OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
		void GUI_OnKeyPress(int iKey, ZGuiWnd *pkWnd);
		void GUI_OnSelectLB(int iID, int iIndex, ZGuiWnd* pkMainWnd);
};

extern DarkMetropolis g_kDM;

#endif