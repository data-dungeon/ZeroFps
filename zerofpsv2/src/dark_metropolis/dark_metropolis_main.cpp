// dark_metropolis_main.cpp

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

#include "dark_metropolis.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

DarkMetropolis g_kDM("DarkMetropolis",0,0,0);

DarkMetropolis::DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("DarkMetropolis");	
} 

void DarkMetropolis::OnHud() 
{	
	m_pkFps->DevPrintf("common","Active Propertys: %d",
		m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
		
	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();
}

void DarkMetropolis::OnInit() 
{
	SetTitle("Dark Metropolis");
	
	m_pkCameraProp	= NULL;	
	m_pkCameraEntity = NULL;
	m_pkFps->m_bClientMode = true;
	m_fMinCamDistance = 2;
	m_fMaxCamDistance = 10;
	m_fDistance = 0;	
	m_fAngle = 0;
	m_strSaveDirectory = "clans/";
	
	Register_Cmd("load",FID_LOAD);			
	Register_Cmd("save",FID_SAVE);	


	m_pkLight->SetLighting(true);
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkFps->SetRenderTarget(m_pkCamera);
	m_pkCamera->m_bRender = true;


	// create gui script
	GuiAppLua::Init(&g_kDM, m_pkScript);
	InitGui(m_pkScript,
		"data/textures/text/ms_sans_serif8.bmp",
		"data/script/gui/defskins.lua",
		NULL);

	LoadGuiFromScript(m_pkScript, "data/script/gui/dm_start.lua");

	m_pkInput->ShowCursor(true);
	m_pkLight->SetLighting(true);
	
	
	//setup default lightsource
	m_kSun.kRot = Vector3(1,2,1);
	m_kSun.kDiffuse=Vector4(1,1,1,0);
	m_kSun.kAmbient=Vector4(0.2,0.2,0.2,0);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	m_kSun.iPriority=10;
	m_kSun.fConst_Atten=1;
	m_kSun.fLinear_Atten=0;
	m_kSun.fQuadratic_Atten=0;	
	
	m_pkLight->Add(&m_kSun);

	
	
	if(!m_pkIni->ExecuteCommands("dark_metropolis_autoexec.ini"))
		m_pkConsole->Printf("No dark_metropolis_autoexec.ini found");
	
}

void DarkMetropolis::OnIdle() 
{
	m_pkFps->SetCamera(m_pkCamera);		
	m_pkFps->GetCam()->ClearViewPort();	

	Input();

	m_pkFps->UpdateCamera(); 	

	

}

void DarkMetropolis::RenderInterface(void)
{
	
	if(m_pkCameraEntity)
	{
		Vector3 kPos = m_pkCamera->GetPos() + Get3DMousePos(true)*4;
		
		m_pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
		m_pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
		m_pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));		
	}
	
}

void DarkMetropolis::OnSystem() 
{				
}

void DarkMetropolis::OnServerClientJoin(ZFClient* pkClient,int iConID, 
													 char* szLogin, char* szPass)
{
}

void DarkMetropolis::OnServerClientPart(ZFClient* pkClient,int iConID)
{
}

void DarkMetropolis::OnServerStart()
{
	m_pkCameraEntity = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_camedit.lua");	
	if(m_pkCameraEntity)
	{
		m_pkCameraEntity->SetParent( m_pkObjectMan->GetWorldObject() );
		m_pkCameraEntity->SetWorldPosV(Vector3(0,0,0));
		
		m_pkCameraProp = (P_Camera*)m_pkCameraEntity->GetProperty("P_Camera");		
		if(m_pkCameraProp)
		{
			m_pkCameraProp->SetCamera(m_pkCamera);
			m_pkCameraProp->SetType(CAM_TYPEBIRDSEYE);
			m_pkCameraEntity->GetSave() = false;
		}
	}
			

}

void DarkMetropolis::OnClientStart()
{
}

bool DarkMetropolis::StartUp()
{
	return true;
}

bool DarkMetropolis::ShutDown()
{
	SaveGame();
	return true;
}

bool DarkMetropolis::IsValid()
{
	return true;
}

void DarkMetropolis::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	m_pkPropertyFactory->Register("P_Event", Create_P_Event);
}

void DarkMetropolis::Input()
{
	//get mouse
	int x,z;		
	m_pkInputHandle->RelMouseXY(x,z);	
		

	//setup player controls
	if(m_pkInputHandle->VKIsDown("camera"))	//do we want to zoom?
	{
		m_pkInput->ShowCursor(false);

		if(m_pkCameraProp)
		{
			m_fDistance += z/60.f;
			m_fAngle -=x/300.f;
	
			if(m_fDistance < m_fMinCamDistance)
				m_fDistance = m_fMinCamDistance;
		
			if(m_fDistance > m_fMaxCamDistance)
				m_fDistance = m_fMaxCamDistance;
				
			m_pkCameraProp->Set3PDistance(m_fDistance);
			m_pkCameraProp->Set3PYAngle(m_fAngle);
			
		}
	}
	else
		m_pkInput->ShowCursor(true);

	if(m_pkInputHandle->VKIsDown("click"))
	{
		Entity* pkEnt = GetTargetObject();	
	
		if(pkEnt)
		{
			cout<<"you clicked on a:"<<pkEnt->GetName()<<endl;
		
		}
	}

}

void DarkMetropolis::RunCommand(int cmdid, const CmdArgument* kCommand)
{

	switch(cmdid) {

		case FID_LOAD:
		{
			if(kCommand->m_kSplitCommand.size() <= 2)
			{
				m_pkConsole->Printf("load [level] [savegame]");
				break;				
			}
			
			
			m_pkConsole->Printf("loading world: %s",kCommand->m_kSplitCommand[1].c_str());						
			m_pkConsole->Printf("loading savegame: %s",kCommand->m_kSplitCommand[2].c_str());				
				
			if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				


			m_pkConsole->Printf("starting server");

			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			
			
			break;		
		}

			
		case FID_SAVE:
			SaveGame();
			break;
	}
}


bool DarkMetropolis::LoadGame(string strClanName)
{
	m_kGameInfo.strClanName = strClanName;

	string strDir = m_strSaveDirectory + string("/") + strClanName;
	string strBlub = string("load dmworld ") + strDir;

	if(!m_pkBasicFS->DirExist(strDir.c_str()))
	{
		cout<<"ERROR clan "<<strClanName<<" does not exist"<<endl;	
		return false;
	}
	
	GetSystem().RunCommand(strBlub.c_str(),CSYS_SRC_SUBSYS);	
	
	return true;
}

bool DarkMetropolis::StartNewGame(string strClanName,string strClanColor)
{
	m_kGameInfo.strClanName = strClanName;
	m_kGameInfo.strClanColor = strClanColor;
	
	//make sure samegame directory exist
	m_pkBasicFS->CreateDir(m_strSaveDirectory.c_str());
	
	string strDir = m_strSaveDirectory + string("/") + strClanName;
	string strBlub = string("load dmworld ") + strDir;
	
	if(m_pkBasicFS->DirExist(strDir.c_str()))
	{
		cout<<"ERROR clan already exist";
		return false;
	}
	
	

	GetSystem().RunCommand(strBlub.c_str(),CSYS_SRC_SUBSYS);

	return true;
}

void DarkMetropolis::SaveGame()
{
	if(!m_kGameInfo.strClanName.empty())
	{
		m_pkObjectMan->ForceSave();
		m_pkObjectMan->SaveZones();	
	
		cout<<"game "<<m_kGameInfo.strClanName<< " saved"<<endl;
	}
}

Vector3 DarkMetropolis::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	
	//screen propotions
	float xp=4;
	float yp=3;
	float fovshit=-2.15;
	
	if(m_bMouse)
	{
		float x;
		float y;
		
		m_pkInputHandle->UnitMouseXY(x,y);
		
		dir.Set(x*xp,-y*yp,fovshit);
		dir.Normalize();
	}
	else
	{
		dir.Set(0,0,fovshit);
		dir.Normalize();
	}	
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

Entity* DarkMetropolis::GetTargetObject()
{
	Vector3 start = m_pkCamera->GetPos();
	Vector3 dir;

	dir = Get3DMousePos(true);		

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkObjectMan->GetZoneObject()->GetAllObjects(&kObjects);
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		
		//objects that should not be clicked on (special cases)
		if(kObjects[i]->iNetWorkID <100000)
			continue;
		
		//-------------
		
		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{	
				float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
				}				
			}
		}		
		
	}
	
	return pkClosest;
}
