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

// Remove the console window
#ifdef WIN32
	#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif 

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
			FID_MAD_DRAW_MODE,
			FID_TOGGLE_BKCOLOR,
		};

		enum ObjectRotationMode_e
		{
			OBJ_ROT_NONE,
			OBJ_ROT_X,
			OBJ_ROT_Y,
			OBJ_ROT_Z,
			OBJ_ROT_XYZ
		};

		enum SelectFileMode_e
		{
			SEL_MAD,
			SEL_MATERIAL,
		};

		int m_iSelFileMode;
		int m_iObjRotMode;
		float m_fObjRotDelay;
		float m_fRotTimer;
		float m_fObjRotX, m_fObjRotY, m_fObjRotZ;
		int m_iCurrRotAngle;
		float* m_apObjRotAngles[3];

		Mad_CoreSubMesh* m_pkSelMesh;
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
										
		//camera stuff
		void  CreateCamera();
				
		// gui stuff
		void	SetupGuiEnviroment();

		// light
		void	ToogleLight(bool bEnabled);

		//input
		bool	DelayCommand();			//make a delay

		// common
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void CreateViewObject();
		void ChangeMad(string strName);

		void RenderInterface();
				
	public:

		MadView(char* aName,int iWidth,int iHeight,int iDepth);
		
		void OnClickListbox( int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd* pkMainWnd);
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, char *szClickNodeText, bool bHaveChilds);
		
	 	void OnInit();
		void OnIdle();

		void Init();
		void Input();

		
		bool IsValid()		{ return true; }
		
		bool StartUp();
		bool ShutDown();
};

#endif // _MADVIEW_H_











