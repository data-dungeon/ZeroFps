
#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif
 
#include "madview.h"
#include "../../../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../../../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../../../zerofpsv2/basic/zfvfs.h"
#include "../../../zerofpsv2/engine/i_camera.h"

MadView g_kZeroEd("MadView", 0, 0, 0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kZeroEd.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_SELECTLISTITEM:
		g_kZeroEd.OnClickListbox(((int*)params)[0],((int*)params)[1],win);
		break;
	case ZGM_SELECTTREEITEM:
		char** pszParams; pszParams = (char**) params;
		g_kZeroEd.OnClickTreeItem( pszParams[0], pszParams[1], 
			pszParams[2], pszParams[3][0] == '1' ? true : false);		
		break;
	}
	return true;
}


MadView::MadView(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("madview");

	m_bIsEditor = true;
	m_fDelayTime   = 0.0;

	m_iObjRotMode = OBJ_ROT_NONE;
	m_fObjRotDelay = 0.005f;

	m_fObjectRadius = 0.5;
	
	m_iCurrRotAngle = 0;
	m_fObjRotX=m_fObjRotY=m_fObjRotZ=0;
	m_apObjRotAngles[0] = &m_fObjRotX;
	m_apObjRotAngles[1] = &m_fObjRotY;
	m_apObjRotAngles[2] = &m_fObjRotZ;

	m_pkSelMesh = NULL;

	Register_Cmd("open_mad", FID_OPENMAD);	
	Register_Cmd("object_rotation_mode", FID_OBJECTROTATIONMODE);	
	Register_Cmd("object_rotation_speed", FID_OBJECTROTATIONSPEED);	
	Register_Cmd("mad_draw_mode", FID_MAD_DRAW_MODE);	
	Register_Cmd("change_bkcolor_infownd", FID_TOGGLE_BKCOLOR);	
	Register_Cmd("listjoint", FID_LISTJOINT);	


	m_strMadFile = "data/mad/cube.mad";
	m_bHaveOpenMadFromCmdLine = false;
} 

void MadView::OnInit() 
{
	if(!m_pkIni->ExecuteCommands("/madview_autoexec.ini"))
		m_pkConsole->Printf("No madview_autoexec.ini found");
	
	Init();
}

void MadView::Init()
{		
	GuiAppLua::Init(&g_kZeroEd, m_pkScript);

	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", 
		"data/script/gui/menu_madview.txt", true, AUTO_SCALE); 

	SetupGuiEnviroment();
	SetTitle("MadView3");

	m_pkInput->ShowCursor(true);

	CreateViewObject();
	CreateCamera();

	ToogleLight(true);

	m_fRotTimer = m_pkZeroFps->GetTicks();
	m_fDelayTime = m_pkZeroFps->GetEngineTime();

	//open mad from command line
	if(g_ZFObjSys.GetNumberOfArguments() >= 2)
	{
		cout<<"open mad file:"<<g_ZFObjSys.GetArgument(1)<<endl;
		string strMadFile = g_ZFObjSys.GetArgument(1);
		ChangeMad(strMadFile);
	}

		
}

void MadView::OnIdle()
{	
	float fTime = m_pkZeroFps->GetTicks();

	if(m_pkGui->m_bHandledMouse == false)
	{
		Input();	
	}

	if(m_pkViewObject)
	{
		float fTimeSinceLastFrame = fTime - m_fRotTimer;
		float dif = fTimeSinceLastFrame / m_fObjRotDelay;

		m_fRotTimer = fTime;

		if(m_iObjRotMode != OBJ_ROT_NONE)
		{
			(*m_apObjRotAngles)[m_iCurrRotAngle] += dif;

			if((*m_apObjRotAngles)[m_iCurrRotAngle] >= 360 )
			{
				(*m_apObjRotAngles)[m_iCurrRotAngle] = 0.0f;

				if(m_iObjRotMode == OBJ_ROT_XYZ)
					m_iCurrRotAngle++;

				if(m_iCurrRotAngle==3)
					m_iCurrRotAngle=0;
			}
		}

		m_pkViewObject->SetWorldRotV(Vector3(m_fObjRotX,m_fObjRotY,m_fObjRotZ));
	}
}

void MadView::ToogleLight(bool bEnabled)
{
	static bool s_bAdded = false;

	if(bEnabled)
	{	
		m_kSun.kRot = Vector3(0.5,1,-1);
		m_kSun.kDiffuse=Vector4(0.8,0.8,0.8,0);
		m_kSun.kAmbient=Vector4(0.3,0.3,0.3,0);
		m_kSun.iType=DIRECTIONAL_LIGHT;			
		m_kSun.iPriority=10;
		m_kSun.fConst_Atten=1;
		m_kSun.fLinear_Atten=0;
		m_kSun.fQuadratic_Atten=0;

		m_pkLight->Add(&m_kSun);
		s_bAdded = true; 
	}
	else
	{
		if(s_bAdded)
			m_pkLight->Remove(&m_kSun);
	}
}

void MadView::CreateCamera()
{
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.01,200);	
	m_pkCamera->m_bForceFullScreen = false;
	m_pkCamera->SetName("persp");
	m_pkCamera->SetDebugGraphs(false);
	m_pkZeroFps->AddRenderCamera(m_pkCamera);
	
	m_pkCameraObject = m_pkEntityManager->CreateEntityFromScript("data/script/objects/cameraedit.lua");
	m_pkCameraObject->SetParent( m_pkEntityManager->GetWorldEntity() );				
	
	P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject->GetProperty("P_Camera");
	m_pkCamProp->SetCamera(m_pkCamera);
	m_pkCameraObject->SetSave(false);	

	m_pkCameraObject->SetWorldPosV(Vector3(0,-2,-10)); 	
}

void MadView::CreateViewObject()
{
	m_pkViewObject = m_pkEntityManager->CreateEntity();
	m_pkViewObject->SetParent( m_pkEntityManager->GetWorldEntity() ); 
// 	m_pkViewObject->AddProperty("P_LightUpdate");
	m_pkViewObject->AddProperty("P_Mad");
	m_pkViewObject->SetWorldPosV(Vector3(0,0,0));
}

void MadView::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_OPENMAD:
			if(m_iSelFileMode != SEL_MAD)
				BuildFileTree("SelectFileTree", "data/mad", ".mad");

			m_iSelFileMode = SEL_MAD;
			ShowWnd("SelectFileWnd", true);
			break;

		case FID_OBJECTROTATIONMODE:
			m_iObjRotMode = atoi(kCommand->m_kSplitCommand[1].c_str());

			if(m_iObjRotMode != 0)
			{
				m_fObjRotX = m_fObjRotY = m_fObjRotZ = 0.0f;
			}
			
			if(m_iObjRotMode != 4)
				m_iCurrRotAngle = m_iObjRotMode-1;
			else
				m_iCurrRotAngle = 0;

			break;

		case FID_OBJECTROTATIONSPEED:
			int speed;
			speed = atoi(kCommand->m_kSplitCommand[1].c_str());
			if(speed == 1) m_fObjRotDelay = 0.100f;
			if(speed == 2) m_fObjRotDelay = 0.009f;
			if(speed == 3) m_fObjRotDelay = 0.002f;
			break;

		case FID_MAD_DRAW_MODE:
			int mode;
			mode = atoi(kCommand->m_kSplitCommand[1].c_str());		

			if(m_pkZeroFps->m_iMadDraw & mode)
				m_pkZeroFps->m_iMadDraw ^= mode;
			else
				m_pkZeroFps->m_iMadDraw |= mode;
			break;

		case FID_LISTJOINT:
			{
			P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");
			Mad_Core* pkCore = static_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 
			pkCore->GetJointAndChildID("root");
			}
			break;
			

		case FID_TOGGLE_BKCOLOR:
			static bool toogle = true;

			if(toogle)
			{
				GetWnd("MadViewInfoWnd")->GetSkin()->m_bTransparent = false;
				GetWnd("SelectFileWnd")->GetSkin()->m_bTransparent = false;

			}
			else
			{
				GetWnd("MadViewInfoWnd")->GetSkin()->m_bTransparent = true;
				GetWnd("SelectFileWnd")->GetSkin()->m_bTransparent = true;
			}

			toogle = !toogle;		
			break;
	}
}

bool MadView::StartUp()
{
	return true; 
}

bool MadView::ShutDown()	
{ 
	return true; 
}

void MadView::RenderInterface()
{
}