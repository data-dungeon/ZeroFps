#include "zeroedit.h"

ZeroEdit Editor("ZeroEdit",1024,768,16);

ZeroEdit::ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) 
{

}

void ZeroEdit::OnInit(void) 
{
	pkConsole->Printf("            ZeroEdit ");
	pkConsole->Printf("--------------------------------");


	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,400);	

	
	glEnable(GL_LIGHTING);
	m_pkMap=new HeightMap();

	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();
	
	//Heightmap
	HeightMapObject *hm=new HeightMapObject(m_pkMap);
	hm->GetPos().Set(0,-4,0);			
	pkObjectMan->Add(hm);	
	pkCollisionMan->Add(hm);

}


void ZeroEdit::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	

	pkObjectMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);
		
	Input();
}


void ZeroEdit::OnHud(void) 
{


}


void ZeroEdit::RunCommand(int cmdid, const CmdArgument* kCommand)
{

}

void ZeroEdit::OnServerStart(void)
{	
}

void ZeroEdit::OnClientStart(void)
{	
}

void ZeroEdit::Input() 
{
	float speed=10;

	if(pkInput->Pressed(RIGHT)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(LEFT)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}
	
	if(pkInput->Pressed(UP))	{
			pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
			pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(DOWN))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(HOME))
		pkFps->GetCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(END))
		pkFps->GetCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;
/*
	if(pkInput->Pressed(HOME)) {
		//pkFps->m_kPropertyFactory.Display();
		cout << "Gaaaaaaaaaaaaa " << endl;
		//g_ZFObjSys.PrintObjects();
		NetPacket paket;
		paket.Write_Str("Hello World :)");
		paket.SetTarget("192.168.0.111:4242");
		pkFps->m_pkNetWork->Send(&paket);
		}
*/		
		
	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;	

}

