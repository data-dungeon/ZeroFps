#include "zeroedit.h"

ZeroEdit Editor("ZeroEdit",800,600,16);

ZeroEdit::ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) 
{

}

void ZeroEdit::OnInit(void) 
{
	
	pkRender->SetFog(Vector4(0,0,0,1),8,100,150,true);
	
	//register commands
	g_ZFObjSys.Register_Cmd("loadmap",FID_LOADMAP,this);	
	g_ZFObjSys.Register_Cmd("loadimagemap",FID_LOADIMAGEMAP,this);		
	g_ZFObjSys.Register_Cmd("savemap",FID_SAVEMAP,this);		
	g_ZFObjSys.Register_Cmd("newmap",FID_NEWMAP,this);		
	g_ZFObjSys.Register_Cmd("objecttree",FID_OBJECTTREE,this);		



	//start text =)
	pkConsole->Printf("            ZeroEdit ");
	pkConsole->Printf("--------------------------------");

	
	m_pkMap=new HeightMap();	
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	

	glEnable(GL_LIGHTING);
	

	m_fTimer=pkFps->GetTicks();
	m_kDrawPos.Set(0,0,0);
	pkObjectMan->SetNoUpdate(true);
	
	m_pkCurentChild=NULL;
	
	m_fPointerHeight=1;
	
	m_iRandom=1;
	pkFps->m_pkCmd->Add(&m_iRandom,"g_Random",type_int);		
	
	m_fDrawRate=0.1;
	pkFps->m_pkCmd->Add(&m_fDrawRate,"g_DrawRate",type_float);		
	
	m_iMode=ADDOBJECT;		
	pkFps->m_pkCmd->Add(&m_iMode,"g_mode",type_int);		
	
	m_iTexture=1;	
	pkFps->m_pkCmd->Add(&m_iTexture,"g_texture",type_int);		
	
	m_fPointDistance=10;
	pkFps->m_pkCmd->Add(&m_fPointDistance,"g_PointDistance",type_float);	
	
	//Heightmap		
//	m_pkMap->GenerateNormals(); 
//	m_pkMap->GenerateTextures();

//	m_pkHeightMapObject->GetPos().Set(0,-4,0);			
//	pkObjectMan->Add(m_pkHeightMapObject);	
//	pkCollisionMan->Add(m_pkHeightMapObject);
	CreateNew(100);


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
//	m_pkHeightMapObject->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);
		
	SetPointer();
	DrawMarkers();

	
	Input();
}


void ZeroEdit::OnHud(void) 
{
	glPushAttrib(GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);

	pkRender->SetFont("file:../data/textures/text/console.tga");
	//pkRender->Print(Vector3(-1.1,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),"FPS:");	
	//pkRender->Print(Vector3(-.9,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),fps);

	pkFps->DevPrintf("Fps: %d",pkFps->m_iFps);
	pkFps->DevPrintf("Mode: %d",m_iMode);
	pkFps->DevPrintf("Active Propertys: %d",pkObjectMan->GetActivePropertys());
	pkFps->DevPrintf("Pointer Altidude: %f",m_fPointerHeight);

	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);

	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkCamera->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
	glDisable(GL_ALPHA_TEST);
	
	glPopAttrib();


}


void ZeroEdit::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_OBJECTTREE:
			pkObjectMan->GetWorldObject()->PrintTree(0);
			break;		
			
		case FID_LOADMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			Clear();		//clear objects
			if(!m_pkMap->Load(kCommand->m_kSplitCommand[1].c_str()))
				pkConsole->Printf("Could not load map =(");
			break;
		case FID_LOADIMAGEMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			Clear();		//clear objects
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
	float childmovespeed=2;
	float childrotatespeed=15;
	float speed=40;

	//camera movements
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
	
	//pointer
	if(pkInput->Pressed(KEY_R)){
		m_fPointerHeight+=pkFps->GetFrameTime();
	}
	if(pkInput->Pressed(KEY_F)){
		m_fPointerHeight-=pkFps->GetFrameTime();
	}
	
	
	
	if(m_pkCurentChild!=NULL){	
		//child movements
		if(pkInput->Pressed(LEFT)) {
			m_pkCurentChild->GetPos().x-=pkFps->GetFrameTime()*childmovespeed;
		}
		if(pkInput->Pressed(RIGHT)) {
			m_pkCurentChild->GetPos().x+=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(UP)) {
			m_pkCurentChild->GetPos().z-=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(DOWN)) {
			m_pkCurentChild->GetPos().z+=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(RSHIFT)) {
			m_pkCurentChild->GetPos().y+=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(RCTRL)) {
			m_pkCurentChild->GetPos().y-=pkFps->GetFrameTime()*childmovespeed;	
		}

	
		//child rotation
		if(pkInput->Pressed(HOME)) {
			m_pkCurentChild->GetRot().y+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(END)) {
			m_pkCurentChild->GetRot().y-=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(INSERT)) {
			m_pkCurentChild->GetRot().x+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_DELETE)) {
			m_pkCurentChild->GetRot().x-=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(PAGEUP)) {
			m_pkCurentChild->GetRot().z+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(PAGEDOWN)) {
			m_pkCurentChild->GetRot().z-=pkFps->GetFrameTime()*childrotatespeed;
		}
		
		//child delete mohahaha
		if(pkInput->Pressed(BACKSPACE)) {
			if(m_pkCurentChild!=NULL)
				if(m_pkCurentChild!=m_pkHeightMapObject) 
					if(m_pkCurentChild!=pkObjectMan->GetWorldObject()){					
						delete m_pkCurentChild;
						m_pkCurentChild=NULL;
						m_pkCurentParent=m_pkHeightMapObject;
					}

		}
		
		
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
				int texture=m_pkMap->GetVert(int(m_kDrawPos.x),int(m_kDrawPos.z))->texture;
				Vector3 color=m_pkMap->GetVert(int(m_kDrawPos.x),int(m_kDrawPos.z))->color;
//				float height=m_pkMap->Height(m_kDrawPos.x,m_kDrawPos.z);
				for(int xp=-2;xp<3;xp++){
					for(int yp=-2;yp<3;yp++){
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->texture=texture;
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->color=color;//Vector3(.6,.45,0.3);		
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->height=height;				
					}
				}
				m_pkMap->GenerateNormals((int)m_kDrawPos.x-4,(int)m_kDrawPos.z-4,8,8);
				
			}
			break;
			
		case TEXTURE:
			if(pkInput->Pressed(MOUSELEFT))
			{
				for(int xp=-2;xp<3;xp++){
					for(int yp=-2;yp<3;yp++){
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->texture=m_iTexture;
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->color=Vector3(.6,.45,0.3);		
					}
				}
			}
			break;
		case ADDOBJECT:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();
			
				Object *object = new BallObject();
				object->GetPos()=m_kDrawPos-Vector3(0,1,0);
				
				if(m_iRandom){
					object->GetRot().y+=rand()%360;					
					object->GetRot().x+=((rand()%25000)-12500)/1000.0;

					object->GetRot().z+=((rand()%25000)-12500)/1000.0;					
				}
//				object->SetParent(m_pkCurentParent);
//				pkObjectMan->Add(object);				
				
				object->AttachToClosestZone();

				m_pkCurentChild=object;
//				pkCollisionMan->Add(object);
	
//				cout<<"CHILDS: "<<m_pkCurentParent->NrOfChilds()<<endl;
	
			}
			if(pkInput->Pressed(MOUSERIGHT))
			{
				SelectChild();
			}
			if(pkInput->Pressed(SPACE))
			{
				SelectParent();
			}
			if(pkInput->Pressed(KEY_C))
			{
				m_pkCurentParent=m_pkHeightMapObject;
			}
			if(pkInput->Pressed(KEY_V))
			{
				if(pkFps->GetTicks()-m_fTimer < .5)
					break;			
				m_fTimer=pkFps->GetTicks();
				
				Object *object = new ZoneObject();
				object->GetPos()=m_kDrawPos-Vector3(0,1,0);
				object->SetParent(pkObjectMan->GetWorldObject());
//				pkObjectMan->Add(object);
				m_pkCurentChild=object;								
				m_pkCurentParent=object;
			}
			if(pkInput->Pressed(KEY_B))
			{
				if(pkFps->GetTicks()-m_fTimer < 1)
					break;			
				m_fTimer=pkFps->GetTicks();
			
				CreateZones();
			}
			
			break;
			
	}
}

void ZeroEdit::Clear() 
{
	CreateNew(100);
}

void ZeroEdit::CreateNew(int iSize) 
{
	pkObjectMan->Clear();
	m_pkCurentChild=NULL;

	m_pkHeightMapObject=new HeightMapObject(m_pkMap);		
	m_pkHeightMapObject->SetParent(pkObjectMan->GetWorldObject());
	
//	pkObjectMan->Add(m_pkHeightMapObject);	
	pkCollisionMan->Add(m_pkHeightMapObject);

	m_pkCurentParent=m_pkHeightMapObject;
	
	m_pkMap->Create(iSize);
	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();

	m_pkHeightMapObject->GetPos().Set(0,-4,0);				
	m_pkMap->SetPosition(Vector3(0,-4,0));


	cout<<"new map"<<endl;
}



void ZeroEdit::SetPointer()
{
//	float degtorad=57.3248;
	Vector3 kPos=pkFps->GetCam()->GetPos();
	Vector3 kHead=pkFps->GetCam()->GetRot();
	
	m_fPointDistance=-(sin((kHead.x-90)/degtorad)/(sin((180-90-(kHead.x-90))/degtorad)/kPos.y));
	m_kDrawPos.Set(pkFps->GetCam()->GetPos().x,0,pkFps->GetCam()->GetPos().z);	
	m_kDrawPos.x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*m_fPointDistance;			
	m_kDrawPos.z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*m_fPointDistance;			
	m_kDrawPos.y = m_fPointerHeight+m_pkMap->Height(m_kDrawPos.x,m_kDrawPos.z);
}


void ZeroEdit::DrawMarkers()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.3);
	
	pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_kDrawPos,1,pkTexMan->Load("file:../data/textures/pointer.tga",T_NOMIPMAPPING));	
	
	if(m_pkCurentParent!=NULL){
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_pkCurentParent->GetPos(),m_pkCurentParent->GetBoundingRadius()*2,pkTexMan->Load("file:../data/textures/parentmarker.tga",T_NOMIPMAPPING));	
	}
	
	if(m_pkCurentChild!=NULL){
		float size=m_pkCurentChild->GetBoundingRadius();
		if(size < .5)
			size=.5;
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_pkCurentChild->GetPos(),size*2,pkTexMan->Load("file:../data/textures/childmarker.tga",T_NOMIPMAPPING));	
		
		if(m_pkCurentChild->GetParent()){
			pkRender->Line(m_pkCurentChild->GetParent()->GetPos(),m_pkCurentChild->GetPos());
		}
	}	
}


void ZeroEdit::SelectChild()
{
	Object* temp=GetClosest(m_kDrawPos);
	
	if(temp!=NULL)
		m_pkCurentChild=temp;	

}

void ZeroEdit::SelectParent()
{
	Object* temp=GetClosest(m_kDrawPos);
	
	if(temp!=NULL)
		m_pkCurentParent=temp;	
}

Object* ZeroEdit::GetClosest(Vector3 kPos)
{
	list<Object*> temp;
	float mindistance=999999999;
	Object* minobject=NULL;
	
	pkObjectMan->GetWorldObject()->GetAllObjects(&temp);
	
	for(list<Object*>::iterator it=temp.begin();it!=temp.end();it++) {
		float distance = abs(((*it)->GetPos() - kPos).Length());
		if(distance<mindistance){
			mindistance=distance;
			minobject=(*it);
		}
	}			
	
	temp.clear();
	
	return minobject;
}

void ZeroEdit::CreateZones()
{
	int radius=250;

//	cout<<"SIZE"<<m_pkMap->m_iHmSize<<endl;

	for(int x=0;x<m_pkMap->m_iHmSize;x+=radius/3){
		for(int z=0;z<m_pkMap->m_iHmSize;z+=radius/3){
			if(m_pkMap->Height(x,z)>-1){
				ZoneObject *object = new ZoneObject();
				object->GetPos()=Vector3(x,m_pkMap->Height(x,z),z);
				object->SetRadius(radius);
				object->SetParent(pkObjectMan->GetWorldObject());			
			}
		}
	}
}


void ZeroEdit::RegisterPropertys()
{
//	pkPropertyFactory->Register("MadProperty", Create_MadProperty);


}
