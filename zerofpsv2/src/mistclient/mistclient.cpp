/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#include "mistclient.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../mcommon/si_mistland.h"
 
MistClient g_kMistClient("MistClient",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistClient.OnCommand(((int*)params)[0], win);
		break;

	case ZGM_LBUTTONDOWN:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], true, true, win);
		break;
	case ZGM_LBUTTONUP:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], false, true, win);
		break;
	case ZGM_RBUTTONDOWN:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], true, false, win);
		break;
	case ZGM_RBUTTONUP:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], false, false, win);
		break;

	case ZGM_LBUTTONDBLCLK:
		g_kMistClient.OnDClick(((int*)params)[0], ((int*)params)[1], true, win);
		break;
	case ZGM_RBUTTONDBLCLK:
		g_kMistClient.OnDClick(((int*)params)[0], ((int*)params)[1], false, win);
		break;

	case ZGM_MOUSEMOVE:
		g_kMistClient.OnMouseMove(((int*)params)[1], ((int*)params)[2], 
			((int*)params)[0] == 1 ? true : false, win);
		break;
	case ZGM_SCROLL:
		g_kMistClient.OnScroll(((int*)params)[0], ((int*)params)[2], win);
		break;

	}
	return true;
}

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	m_iActiveCaracterObjectID = -1;
	m_iActiveCaracter			= -1;
	m_iSelfObjectID			= -1;
	m_pkClientObject			= NULL;
	m_pkClientControlP		= NULL;
	m_pkServerInfo				= NULL;
	m_pkActiveCharacter		= NULL;
	
	g_ZFObjSys.Log_Create("mistclient");


 
} 

void MistClient::OnInit() 
{
	pkConsole->Printf(" MistClient , im scarred  =/");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" ugga?  blub?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");



}

void MistClient::Init()
{	
	m_fClickDelay = pkFps->GetTicks();

	//register commmands bös
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("unload",FID_UNLOAD);			
	Register_Cmd("massspawn",FID_MASSSPAWN);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	
	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();

	m_pkMap2 = new Heightmap2(/*"HeightMap"*/);
	m_pkMap2->CreateHMFromImage("/data/textures/hmap.tga");

	// set caption
	SDL_WM_SetCaption("MistClient", NULL);
	 
	// create gui script
	GuiAppLua::Init(&g_kMistClient, GetScript());

	// init gui
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan, 
		"data/textures/text/paternoster8.bmp",
		"data/script/gui/gui_create_client.lua");

	//init mistland script intreface
	MistLandLua::Init(pkObjectMan,pkScript);
	
	// hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	// Zeb: Tar bort detta, det görs ju i OnServerStart!
	//pkObjectMan->CreateObjectFromScript("data/script/objects/t_player.lua");

	// Varde ljus!
	pkLight->SetLighting(true);
	pkZShader->SetForceLighting(ALWAYS_OFF);
	
}

void MistClient::RegisterActions()
{
	m_iActionDoOrder=pkInput->RegisterAction("DoOrder");
	m_iActionPrintServerInfo=pkInput->RegisterAction("PrintServerInfo");
	m_iActionUnExploreAll=pkInput->RegisterAction("UnExploreAll");
	m_iActionExploreAll=pkInput->RegisterAction("ExploreAll");
	m_iActionCamLeft=pkInput->RegisterAction("CamLeft");
	m_iActionCamRight=pkInput->RegisterAction("CamRight");
	m_iActionCamUp=pkInput->RegisterAction("CamUp");
	m_iActionCamDown=pkInput->RegisterAction("CamDown");
	m_iActionSelect=pkInput->RegisterAction("Select");
	m_iActionScroll=pkInput->RegisterAction("Scroll");	
	m_iActionSelectManyModifier=pkInput->RegisterAction("SelectManyModifier");	
}

void MistClient::RegisterPropertys()
{
	pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	pkPropertyFactory->Register("P_Event", Create_P_Event);
	pkPropertyFactory->Register("P_CharStats", Create_P_CharStats);
   pkPropertyFactory->Register("P_Item", Create_P_Item);
}


void MistClient::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	
 	pkFps->UpdateCamera(); 	
	
	if(m_pkMap2) {
		m_pkMap2->SetPos(Vector3(0,0,0));
		//pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());	
		}

	Vector3 pos = Get3DMousePos();
	pos+=pkFps->GetCam()->GetPos();
	
	pkRender->Line(pos,pos+Vector3(10,0,0));
	pkRender->Line(pos,pos+Vector3(0,10,0));	
	pkRender->Line(pos,pos+Vector3(0,0,10));	
	
	// FULHACK Tm Vim
		pkObjectMan->OwnerShip_Take( pkObjectMan->GetObjectByNetWorkID( pkFps->GetClientObjectID() ) );
	

	if(m_pkServerInfo)
	{
		pkFps->DevPrintf("client","ServerName: %s", m_pkServerInfo->m_sServerName.c_str());
		pkFps->DevPrintf("client","Players: %d", m_pkServerInfo->GetNrOfPlayers());
	
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
			pkFps->DevPrintf("client","PlayerName: %s", pi->sPlayerName.c_str());		
	}	

}

void MistClient::OnSystem() 
{
	//setup client
	if(pkFps->m_bClientMode && !pkFps->m_bServerMode)
	{
		GetSystem().Logf("net","??? m_iSelfObjectID %d\n", m_iSelfObjectID);

		if(!m_pkClientControlP)
		{
			m_iSelfObjectID = pkFps->GetClientObjectID();	
			if(m_iSelfObjectID != -1)
			{		
				m_pkClientObject = pkObjectMan->GetObjectByNetWorkID(m_iSelfObjectID);		
				if(m_pkClientObject)
				{			
					pkConsole->Printf("Got client object, Trying to get client control");
			
					m_pkClientControlP = (P_ClientControl*)m_pkClientObject->GetProperty("P_ClientControl");				
					if(m_pkClientControlP)
					{
						pkConsole->Printf("Got client control");				
					}
				
				}
			}
		}
		
		if(!m_pkServerInfo)
		{
			Object* pkServerI = pkObjectMan->GetObject("A t_serverinfo.lua");
			if(pkServerI)
				m_pkServerInfo = (P_ServerInfo*)pkServerI->GetProperty("P_ServerInfo");
				
			if(m_pkServerInfo)
			{
				pkConsole->Printf("Got server info");
			}
		}
	};

	if(m_pkServerInfo)
	{		
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
		{
			int id = pi->kControl[m_iActiveCaracter].first;	
			Object* pkObj = pkObjectMan->GetObjectByNetWorkID(id);
			
			if(pkObj)
			{
				CameraProperty* cp = (CameraProperty*)pkObj->GetProperty("CameraProperty");
			
				if(!cp)
					CameraProperty* cp = (CameraProperty*)pkObj->AddProperty("CameraProperty");
		
				if(cp)
				{
					cp->SetCamera(m_pkCamera);
					cp->SetType(CAM_TYPE3PERSON);
					m_pkCamProp = cp;
				}

				m_pkActiveCharacter = pkObj;
			}		
		}else
			cout<<"cant find player object id"<<pkFps->GetConnectionID()<<endl;
	}

	SetActiveCaracter(0);
}

void MistClient::Input()
{
	float speed = 20;
	
	int x,z;		
	pkInput->RelMouseXY(x,z);	
		
	if(pkInput->Pressed(MOUSEMIDDLE))
	{
		if(m_pkCamProp)
		{
			m_fAngle +=x/100.0;
			m_fDistance += z/60.0;
	
			if(m_fDistance < 0.5)
				m_fDistance = 0.5;
		
			if(m_fDistance > 2)
				m_fDistance = 2;
				
			m_pkCamProp->Set3PYAngle(m_fAngle);
			m_pkCamProp->Set3PDistance(m_fDistance);
		}
	}
	

	Object* m_pkME = m_pkTestobj;
	
	if(m_pkME)	
	{	
		float fSpeedScale = pkFps->GetFrameTime()*speed;	
		
		Vector3 newpos;// = m_pkME->GetLocalPosV();
		newpos.Set(0,0,0);
	
		if(pkInput->Pressed(KEY_D)){
			//newpos.x+=fSpeedScale;			
			newpos.x = 10;
		}
		if(pkInput->Pressed(KEY_A)){
			//newpos.x-=fSpeedScale;			
			newpos.x = -10;		
		}	
		if(pkInput->Pressed(KEY_W))	{
			//newpos.z+=fSpeedScale;			
			newpos.z = 10;
		}					
		if(pkInput->Pressed(KEY_S))	{
			//newpos.z-=fSpeedScale;
			newpos.z = -10;
		}		

		if(pkInput->Pressed(KEY_Q))
			newpos.y = 10;
			//newpos.y+=2*fSpeedScale;			
		if(pkInput->Pressed(KEY_E))
			newpos.y = -10;
			//newpos.y-=2*fSpeedScale;
		
		//gubbe rotation

		Vector3 rot;
		rot.Set(0,0,0);
		
		if(pkInput->Pressed(KEY_U))
			rot.x+=fSpeedScale*5;
		if(pkInput->Pressed(KEY_J))
			rot.x-=fSpeedScale*5;		
		if(pkInput->Pressed(KEY_I))
			rot.y+=fSpeedScale*5;
		if(pkInput->Pressed(KEY_K))
			rot.y-=fSpeedScale*5;
		if(pkInput->Pressed(KEY_O))
			rot.z+=fSpeedScale*5;
		if(pkInput->Pressed(KEY_L))
			rot.z-=fSpeedScale*5;

		//m_pkME->SetLocalPosV(newpos);
		if(newpos.Length() > 0)
			m_pkME->GetVel() = newpos;
		m_pkME->RotateLocalRotV(rot);
	} 

	if(pkInput->Pressed(MOUSELEFT))
	{
		if(pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			Object* pkObject = GetTargetObject();
			
			if(pkObject)
			{
				P_Ml* pkMistLandProp = static_cast<P_Ml*>(pkObject->GetProperty("P_Ml")); 

				if(pkMistLandProp)
				{
					vector<string> vkActionNames = pkMistLandProp->GetActions();

					if(m_pkClientControlP && !vkActionNames.empty())
					{
						ClientOrder order;
						
						order.m_sOrderName = vkActionNames[0]; //"Klicka";
						order.m_iClientID = pkFps->GetConnectionID();
						order.m_iObjectID = pkObject->iNetWorkID;				
						order.m_iCaracter = m_iActiveCaracterObjectID;
						
						m_pkClientControlP->AddOrder(order);
					} 
				}
			}
			
			m_fClickDelay = pkFps->GetTicks();					
		}

		PickUp();
	}
}

void MistClient::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","SelfID: %d", m_iSelfObjectID);	
	
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

}

void MistClient::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("load [mapname]");
				break;				
			}
			
			cout<<"loading world:"<<kCommand->m_kSplitCommand[1].c_str()<<endl;
			
			if(!pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1].c_str()))
			{
				cout<<"Error loading world"<<endl;
				break;
			}
						
			cout<<"starting server"<<endl;
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			
			break;	
		
		case FID_UNLOAD:
			break;
	
	}
}



void MistClient::ClientInit()
{
	cout<<"Client Join granted"<<endl;


	cout<<"Join Complete"<<endl;
	
}

void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding er to client" << endl;
//	pkClient->m_pkObject->AddProperty("TrackProperty");	
	
}

void MistClient::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}


void MistClient::OnServerStart(void)
{		
	m_pkTestobj = pkObjectMan->CreateObjectFromScript("data/script/objects/t_player.lua");
	if(m_pkTestobj)
	{
		m_pkTestobj->SetParent(pkObjectMan->GetWorldObject());
	
		m_pkTestobj->AddProperty("TrackProperty");	

		m_pkCamProp = (CameraProperty*)m_pkTestobj->GetProperty("CameraProperty");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCamProp->SetType(CAM_TYPE3PERSON);
	
		m_pkTestobj->SetWorldPosV(Vector3(0,0.1,0));
		MistLandLua::g_iCurrentPCID = m_pkTestobj->iNetWorkID;
	}


	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());


	}
}

void MistClient::OnClientStart(void)
{
	pkConsole->Printf("Connected, Waiting for client object");
}

bool MistClient::StartUp()	{ return true; }
bool MistClient::ShutDown()	
{ 
	printf("MistClient::ShutDown\n"); 

	if(m_pkInventDlg)
		delete m_pkInventDlg;

	return true; 
}
bool MistClient::IsValid()	{ return true; }


void MistClient::OnCommand(int iID, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = GetWnd(iID);

	string strName = pkMainWnd->GetName();

	if(strName == "MainWnd")
	{
		if(pkWndClicked)
		{
			string strName = pkWndClicked->GetName();

			if(strName == "BackPackButton")
			{
				bool bExist = GetWnd("BackPackWnd") != NULL;
				
				pkScript->Call(m_pkScriptResHandle, "OnClickBackpack", 0, 0); 

	/*			if(bExist == false)
				{
					m_pkInventDlg = new InventoryDlg(GetWnd("BackPackWnd"));

					const int ANTAL = 5;

					ItemStats* pkTestItems = new ItemStats[ANTAL];

					typedef pair<EquipmentCategory, pair<string,string> > tInfo;

					tInfo info[ANTAL] =
					{
						tInfo(Weapon, pair<string, string>("dagger.bmp", "dagger_a.bmp")),
						tInfo(Weapon, pair<string, string>("dagger.bmp", "dagger_a.bmp")),
						tInfo(Belt, pair<string, string>("spellbook.bmp", "spellbook_a.bmp")),
						tInfo(Item, pair<string, string>("gembag1.bmp", "gembag1_a.bmp")),
						tInfo(Item, pair<string, string>("gembag1.bmp", "gembag1_a.bmp")),
					};

					vector<InventoryDlg::itItem> kItems;

					for( int i=0; i<ANTAL; i++)
					{
						pkTestItems[i].SetEquipmentCategory(info[i].first); 
						kItems.push_back( InventoryDlg::itItem(
							pair<string,string>(info[i].second.first, 
							info[i].second.second), &pkTestItems[i]) );
					}

					kItems[3].second->RegisterAsContainer(); 
					kItems[4].second->RegisterAsContainer(); 
					
				m_pkInventDlg->AddItems(kItems);	
				}*/
			}
			if(strName == "StatsButton")
				pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
			if(strName == "MapButton")
				pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
		}
	}
	//else
	//if(strName == "BackPackWnd")
	{
		m_pkInventDlg->OnCommand(iID); 
	}
}



Vector3 MistClient::Get3DMousePos()
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;
	
	pkInput->UnitMouseXY(x,y);	
	dir.Set(x*xp,-y*yp,-1.5);
	dir.Normalize();
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
/*	Vector3 pos = (dir*1)+m_pkCamera->GetPos();//m_pkTestobj->GetLocalPosV();

	pkRender->Line(pos,pos+Vector3(2,0,0));
	pkRender->Line(pos,pos+Vector3(0,2,0));	
	pkRender->Line(pos,pos+Vector3(0,0,2));	
	pkRender->Sphere(pos,0.1,10,Vector3(0,1,0),false);
*/

	return dir;
}

Object* MistClient::GetTargetObject()
{
	Vector3 start = pkFps->GetCam()->GetPos();
	Vector3 dir = Get3DMousePos();

	vector<Object*> kObjects;
	kObjects.clear();
	
	pkObjectMan->TestLine(&kObjects,start,dir);
	
	//cout<<"nr of targets: "<<kObjects.size()<<endl;
	
	float closest = 9999999999;
	Object* pkClosest = NULL;	
	for(int i=0;i<kObjects.size();i++)
	{
		
		float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
		if(d < closest)
		{
			closest = d;
			pkClosest = kObjects[i];
		}
	}
	
	return pkClosest;
}

void MistClient::OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain)
{
	if(pkMain == NULL)
		return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
	{
		m_pkInventDlg->OnClick(x,y,bMouseDown,bLeftButton);
	}
}

void MistClient::OnDClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain)
{
	if(pkMain == NULL)
		return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
	{
		m_pkInventDlg->OnDClick(x,y,bLeftButton);
	}
}

void MistClient::OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain)
{
	if(pkMain == NULL)
		return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
	{
		m_pkInventDlg->OnMouseMove(x,y,bMouseDown);
	}	
}

void MistClient::OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
	if(pkMain == NULL)
		return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
	{
		m_pkInventDlg->OnScroll(iID,iPos);
	}	
}

void MistClient::SetActiveCaracter(int iCaracter)
{
	//dont try to set if caracter is already active
	if(m_iActiveCaracter == iCaracter)
		return;
		
	if(m_pkServerInfo)
	{		
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
		{
			//if theres a current carater disable its camera
			if(m_iActiveCaracter != -1)
			{
				int id = pi->kControl[m_iActiveCaracter].first;	
				Object* pkObj = pkObjectMan->GetObjectByNetWorkID(id);
			
				if(pkObj)
				{
					pkObj->DeleteProperty("CameraProperty");
					m_pkCamProp = NULL;
				}
				m_iActiveCaracter = -1;
				m_iActiveCaracterObjectID = -1;
			}
			
			//if the new caracter is valid
			if(iCaracter != -1)
			{
				//check rights
				if(pi->kControl[iCaracter].second & PR_LOOKAT)
				{
					int id = pi->kControl[iCaracter].first;	
					Object* pkObj = pkObjectMan->GetObjectByNetWorkID(id);
		
					//object found
					if(pkObj)
					{
						CameraProperty* cp = (CameraProperty*)pkObj->GetProperty("CameraProperty");
				
						if(!cp)
							CameraProperty* cp = (CameraProperty*)pkObj->AddProperty("CameraProperty");
		
						if(cp)
						{
							cp->SetCamera(m_pkCamera);
							cp->SetType(CAM_TYPE3PERSON);								
							m_pkCamProp = cp;
							
							//set current active caracter
							m_iActiveCaracter = iCaracter;
							m_iActiveCaracterObjectID = id;
							cout<<"current caracter is: "<<m_iActiveCaracter<<endl;													
						}
					}
				}		
			}
		}
		else
			cout<<"Error: cant find player info, You dont exist"<<endl;
	} 
}	


void MistClient::PickUp()
{
	if(m_pkActiveCharacter)
	{
		CharacterProperty* cp = static_cast<CharacterProperty*>(
			m_pkActiveCharacter->GetProperty("P_CharStats"));

		if(cp)
		{
			CharacterStats* stats = cp->GetCharStats();

			if(stats)
			{
				map<string, Object*>* vkEquipmentList = stats->GetEquippedList();

				map<string, Object*>::iterator it;
				for( it=vkEquipmentList->begin(); it!=vkEquipmentList->end(); it++)
				{
					P_Item* pkItemProp = static_cast<P_Item*>(it->second->GetProperty("P_Item"));

					if(pkItemProp)
					{
						ItemStats* pkStats = pkItemProp->GetItemStats();

						if(pkStats)
							m_pkInventDlg->AddItem(pkStats);
					}
				}
			}
		}
	}
}
