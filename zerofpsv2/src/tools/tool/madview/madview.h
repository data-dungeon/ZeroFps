#ifndef _MADVIEW_H_
#define _MADVIEW_H_

#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include "../../../zerofpsv2/engine/application.h"
#include "../../../zerofpsv2/engine_systems/mad/mad_modell.h"
#include "../../../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../../../zerofpsv2/gui/zgui.h"
#include "../../../mcommon/p_enviroment.h"

/**	\brief	Da MadView
		\ingroup MadView
*/
class MadView :public Application , public ZGuiApp 
{
	private:

		//console funktions
		enum FuncId_e
		{
			FID_OPENMAD,
			FID_OBJECTROTATIONMODE,
			FID_OBJECTROTATIONSPEED,
		};

		enum ObjectRotationMode_e
		{
			OBJ_ROT_NONE,
			OBJ_ROT_X,
			OBJ_ROT_Y,
			OBJ_ROT_Z,
			OBJ_ROT_XYZ
		};

		int m_iObjRotMode;
		float m_fObjRotDelay;
				
		float m_fRotTimer;
		float m_fObjRotX, m_fObjRotY, m_fObjRotZ;
		int m_iCurrRotAngle;
		float* m_apObjRotAngles[3];

		Entity* m_pkViewObject;
		string m_strMadFile;
	
		//delay
		float	m_fDelayTime;
		
		//edit sun
		LightSource	m_kSun;				
		
		//camera
		Entity*	m_pkCameraObject;
		Camera*	m_pkCamera;
		float 	m_CamMoveSpeed;		
				
		//physics stuff
		Vector3	m_kLocalGrabPos;
		Vector3	m_kGrabPos;
		Vector3	m_kGrabCurrentPos;
		int		m_iGrabEntity;
		float		m_fArmLength;
						
		//camera stuff
		void  CreateCamera();

		//picking
		Vector3	Get3DMousePos(bool m_bMouse);
		Vector3	Get3DMouseDir(bool bMouse);
		Entity*	GetTargetObject();		
		int		GetTargetTCS(Vector3* pkPos);		
				
		// gui stuff
		void	SetupGuiEnviroment();

		// light
		void	ToogleLight(bool bEnabled);

		//input
		bool	DelayCommand();			//make a delay

		// common
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void CreateViewObject();
				
	public:

		MadView(char* aName,int iWidth,int iHeight,int iDepth);
		
		void OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage);
		void OnClickListbox( int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd* pkMainWnd);
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, char *szClickNodeText, bool bHaveChilds);
		
	 	void OnInit();
		void OnIdle();
		void OnHud(){};
		void OnSystem(){};
			
		void Init();
		void Input();
		void RegisterPropertys();
		void RegisterResources();		

		bool StartUp()		{ return true; }
		bool ShutDown()	{ return true; }
		bool IsValid()		{ return true; }

		// Network (unused)
		void OnServerStart(){};
		void OnClientStart(){};
		bool OnPreConnect(IPaddress, char*, char*, bool bIsEditor, string& strWhy){return true;}
		void OnServerClientJoin(ZFClient*,int, char*, char*, bool bIsEditor){};
		void OnServerClientPart(ZFClient* pkClient,int iConID){};
};

#endif // _MADVIEW_H_











