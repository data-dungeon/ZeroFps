#include "zeroedit.h"

ZeroEdit Editor("ZeroEdit",1024,768,16);

ZeroEdit::ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) 
{

}

void ZeroEdit::OnInit(void) 
{
	//register commands
	g_ZFObjSys.Register_Cmd("loadmap",FID_LOADMAP,this);	
	g_ZFObjSys.Register_Cmd("loadimagemap",FID_LOADIMAGEMAP,this);		
	g_ZFObjSys.Register_Cmd("savemap",FID_SAVEMAP,this);		
	g_ZFObjSys.Register_Cmd("newmap",FID_NEWMAP,this);		


	//start text =)
	pkConsole->Printf("            ZeroEdit ");
	pkConsole->Printf("--------------------------------");

	//init
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,400);	

	glEnable(GL_LIGHTING);
	
	m_iMode=FLATTEN;	
	m_kDrawPos.Set(0,0,0);
	m_fPointDistance=10;
	pkFps->m_pkCmd->Add(&m_fPointDistance,"g_fPointDistance",type_float);	
	
	//Heightmap		
	m_pkMap=new HeightMap();

	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();

	HeightMapObject *hm=new HeightMapObject(m_pkMap);
	hm->GetPos().Set(0,-4,0);			
	pkObjectMan->Add(hm);	
	pkCollisionMan->Add(hm);


//default light
	LightSource *sol=new LightSource;	
	Vector3 *solrot=new Vector3(.4,.4,.2);	
	Vector3 *solpos=new Vector3(1000,1000,1000);
		sol->kRot=solrot;
		sol->kPos=solpos;		
		sol->kDiffuse=Vector4(1.8,1.8,1.8,1);	//Dag
		sol->kAmbient=Vector4(0.01,0.01,0.01,0.01);
		sol->iType=POINT_LIGHT;			
		sol->iPriority=10;
		sol->fConst_Atten=1;
		sol->fLinear_Atten=0;
		sol->fQuadratic_Atten=0;

	pkLight->Add(sol);

}


void ZeroEdit::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	

	pkObjectMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);
		
		
		
	m_kDrawPos.Set(pkFps->GetCam()->GetPos().x,0,pkFps->GetCam()->GetPos().z);	
	m_kDrawPos.x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*m_fPointDistance;			
	m_kDrawPos.z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*m_fPointDistance;			
	m_kDrawPos.y = 1+m_pkMap->Height(m_kDrawPos.x,m_kDrawPos.z);
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.3);
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_kDrawPos,1,pkTexMan->Load("file:../data/textures/pointer.tga",0));	

	
	Input();
}


void ZeroEdit::OnHud(void) 
{


}


void ZeroEdit::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOADMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			if(!m_pkMap->Load(kCommand->m_kSplitCommand[1].c_str()))
				pkConsole->Printf("Could not load map =(");
			break;
		case FID_LOADIMAGEMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			if(!m_pkMap->LoadImageHmap(kCommand->m_kSplitCommand[1].c_str())){
				pkConsole->Printf("Could not load map =(");
			} else  {
				m_pkMap->GenerateNormals(); 
				m_pkMap->GenerateTextures();
			}
			
			break;
	
		case FID_SAVEMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			if(!m_pkMap->Save(kCommand->m_kSplitCommand[1].c_str()))
				pkConsole->Printf("Could not save map =(");			
			break;
		case FID_NEWMAP:	
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Type a mapsize");
				return;
			}
			int size=atoi(kCommand->m_kSplitCommand[1].c_str());
			
			pkConsole->Printf("Creating new map with size %d",size);
			CreateNew(size);
			break;
	}


}

void ZeroEdit::OnServerStart(void)
{	
}

void ZeroEdit::OnClientStart(void)
{	
}

void ZeroEdit::Input() 
{
	float speed=40;

	if(pkInput->Pressed(KEY_X)){
		speed*=0.5;
	}
	if(pkInput->Pressed(KEY_D)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(KEY_A)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}
	
	if(pkInput->Pressed(KEY_W))	{
			pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
			pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_S))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(KEY_Q))
		pkFps->GetCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_E))
		pkFps->GetCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;


	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;	


	switch(m_iMode){
		case FLATTEN:
		
			if(pkInput->Pressed(MOUSELEFT))
			{
				float height=m_pkMap->GetVert(int(m_kDrawPos.x),int(m_kDrawPos.z))->height;
//				float height=m_pkMap->Height(m_kDrawPos.x,m_kDrawPos.z);
				for(int xp=-1;xp<2;xp++){
					for(int yp=-1;yp<2;yp++){
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->texture=2;
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->color=Vector3(.6,.45,0.3);		
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->height=height;				
					}
				}
				m_pkMap->GenerateNormals(m_kDrawPos.x-3,m_kDrawPos.z-3,6,6);
				
			}
	}
}


void ZeroEdit::CreateNew(int iSize) 
{
	m_pkMap->Create(iSize);
	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();

}









