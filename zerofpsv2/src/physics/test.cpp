#include "test.h"
#include "../mcommon/p_enviroment.h"

Test g_kMistServer("Test",0,0,0);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth)
{ 
	g_ZFObjSys.Log_Create("mistdemo");



} 

void Test::OnInit() 
{
	pkConsole->Printf(" MistServer (mistland dedicated server)");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Hugga?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("test_autoexec.ini"))
		pkConsole->Printf("No mistdemo_autoexec.ini found");
}

void Test::Init()
{	
	//damn "#¤(="%#( lighting fix bös
	pkLight->SetLighting(true);
	pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);	
	
	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();

	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	

	// hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	SDL_WM_SetCaption("test", NULL);

	//click delay
	m_fClickDelay = pkFps->GetTicks();
}

void Test::RegisterResources()
{
	
}

void Test::RegisterPropertys()
{
	pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
}



void Test::OnIdle()
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	

	if(pkGui->m_bHaveInputFocus == false)
	{
		Input();	
	}

 	pkFps->UpdateCamera(); 		

	
	m_kObjectMarkerPos = pkFps->GetCam()->GetPos() + Get3DMousePos(true)*5;
	DrawCrossMarker(m_kObjectMarkerPos);

	pkRender->DrawColorBox(Vector3(0,-0.5,0),Vector3(0,0,0),Vector3(10,1,10),Vector3(1,0,0));
}

void Test::OnSystem()
{
	
}


void Test::Input()
{
	float speed = 20;

	int x,z;		
	pkInput->RelMouseXY(x,z);	

if(m_pkCameraObject)	
	{	
		if(pkInput->Pressed(KEY_X)){
			speed*=0.25;
		}
	
		float fSpeedScale = pkFps->GetFrameTime()*speed;

		Vector3 newpos = m_pkCameraObject->GetLocalPosV();
		
		Matrix4 kRm;
		kRm = m_pkCameraObject->GetLocalRotM();

		kRm.Transponse();
	
		
		Vector3 xv = kRm.GetAxis(0);
		Vector3 zv = kRm.GetAxis(2);
	
		xv.y = 0;
		zv.y = 0;
		
		xv.Normalize();
		zv.Normalize();
	
		if(pkInput->Pressed(KEY_D))	newpos += xv * fSpeedScale;		
		if(pkInput->Pressed(KEY_A))	newpos += xv * -fSpeedScale;		
		if(pkInput->Pressed(KEY_W))	newpos += zv * -fSpeedScale;
		if(pkInput->Pressed(KEY_S))	newpos += zv * fSpeedScale;	
	
		if(pkInput->Pressed(KEY_Q))	newpos.y += fSpeedScale;
		if(pkInput->Pressed(KEY_E))	newpos.y -= fSpeedScale;
				

		Vector3 rot;
		rot.Set(-z / 5.0,-x / 5.0,0);

		kRm.Transponse();		
		kRm.Rotate(rot);
		kRm.Transponse();		
		Vector3 bla = Vector3(0,0,1);
		bla = kRm.VectorTransform(bla);
		
			
		kRm.LookDir(bla,Vector3(0,1,0));

		
		m_pkCameraObject->SetLocalPosV(newpos);		
		m_pkCameraObject->SetLocalRotM(kRm);	
	}
	
	if(pkInput->Pressed(KEY_ESCAPE))
		pkFps->QuitEngine();	

	if(pkInput->Pressed(KEY_I))
		pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);	
	if(pkInput->Pressed(KEY_O))
		pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
	if(pkInput->Pressed(KEY_P))
		pkZShader->SetForceLighting(LIGHT_MATERIAL);
		



	if(pkInput->Pressed(KEY_SPACE))
	{
		if(pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_fClickDelay = pkFps->GetTicks();				
			
			Entity* pkent = pkObjectMan->CreateObjectFromScript("data/script/objects/t_ptest.lua");
			if(pkent)
			{
				pkent->SetParent(pkObjectMan->GetWorldObject());
				pkent->SetWorldPosV(m_kObjectMarkerPos);
			}
		}
	}
	if(pkInput->Pressed(KEY_C))
	{
		if(pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_fClickDelay = pkFps->GetTicks();				
			
			Entity* pkent = pkObjectMan->CreateObjectFromScript("data/script/objects/t_ground.lua");
			if(pkent)
			{
				pkent->SetParent(pkObjectMan->GetWorldObject());
				pkent->SetWorldPosV(m_kObjectMarkerPos);
			}
		}
	}
	
}

void Test::OnHud(void)
{
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","Collissions: %d",pkPhysics_Engine->m_iNrOfCollissions);			
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();


}

void Test::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
	
	}

}

bool Test::StartUp()	
{ 
	return true;
}

bool Test::ShutDown()	{ return true; }
bool Test::IsValid()	{ return true; }

bool Test::OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass)
{
	return true;
}

void Test::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass)
{
	
}

void Test::OnServerClientPart(ZFClient* pkClient,int iConID)
{
}

void Test::OnServerStart(void)
{		
	m_pkCameraObject = pkObjectMan->CreateObjectFromScript("data/script/objects/t_camera.lua");
	if(m_pkCameraObject)
	{	
		m_pkCameraObject->SetParent(pkObjectMan->GetWorldObject());
		P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCameraObject->GetSave() = false;
		
		
/*		P_Enviroment* pe = (P_Enviroment*)m_pkCameraObject->AddProperty("P_Enviroment");
		pe->SetEnable(true);
		pe->SetEnviroment("data/enviroments/sun.env");
*/		
	}
	
	
	Plane* ground = new Plane();
	ground->Set(Vector3(0,1,0),Vector3(0,0,0));
	
	pkPhysics_Engine->AddPlane(ground);
	
}

void Test::OnClientStart(void)
{
}

void Test::DrawCrossMarker(Vector3 kPos)
{
	pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
	pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
	pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));	
}

Vector3 Test::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;
	
	if(m_bMouse)
	{
		pkInput->UnitMouseXY(x,y);	
		dir.Set(x*xp,-y*yp,-1.5);
		dir.Normalize();
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

