#include "test.h"


Test olle("MegaGame",1024,768,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}


void Test::OnInit(void) {
	pkConsole->Print("MegaUltraSuper Duper Game");
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
//	pkFps->m_pkAudioMan->PlayMusic();


//------------Map---------
	test=new HeightMap(pkFile);
//	test->Random();
	test->SetTileSet("file:../data/textures/landbw.bmp");
	
//	test->Save("test.hm");
//	test->Load("file:test.hm");
	test->LoadImageHmap("file:test2.bmp");
	test->GenerateNormals();
	test->GenerateTextures();

	test->SetPosition(Vector3(0,-4,0));
//----------------------------
	
	IntToChar(fps,pkFps->m_iFps);
	fpsupdate=0;


	float b=1.2;

	pkFps->m_pkCmd->Add(&speed,"g_speed",type_float);		
	speed=0.025;
  	
  	m_iGrass=0;	
	pkFps->m_pkCmd->Add(&m_iGrass,"g_grass",type_int);		  	
  	m_iGrassVolyme=150;	
	pkFps->m_pkCmd->Add(&m_iGrassVolyme,"g_grassvolyme",type_int);		  	

  
  pkRender->SetFog(Vector4(.50,.55,.88,1),2,150,200,true);

//	testa.Load("babe.pmd");
	CoreTest.Load("bitch.mad");
	CoreTest.SetTextureManger(pkTexMan);
	iCoreFrame = 0;

	pkLight->SetCamera(pkFps->m_kCamPos);
	
	LightSource *spot=new LightSource();
	Vector3 *spotpos=new Vector3(55,50,40);
	Vector3 *spotrot=new Vector3(0,-1,0);	
		spot->kPos=spotpos;
		spot->kRot=spotrot;
		spot->kDiffuse=Vector4(0,0,6,1);
		spot->kAmbient=Vector4(0,0,0,0);		
		spot->fCutoff=20;	
		spot->fExp=20;
		spot->fConst_Atten=0;
		spot->fQuadratic_Atten=0.001;
		spot->iType=SPOT_LIGHT;			
		spot->iPriority=0;

	LightSource *spot2=new LightSource();
	m_kSpotpos=new Vector3(0,0,0);
//	Vector3 *spotpos2=new Vector3(55,50,50);
	Vector3 *spotrot2=new Vector3(0,-1,0);	
		spot2->kPos=m_kSpotpos;
		spot2->kRot=spotrot2;
		spot2->kDiffuse=Vector4(6,0,0,1);
		spot2->kAmbient=Vector4(0,0,0,0);		
		spot2->fCutoff=20;	
		spot2->fExp=20;
		spot2->fConst_Atten=0;
		spot2->fQuadratic_Atten=0.001;
		spot2->iType=SPOT_LIGHT;			
		spot2->iPriority=0;


	LightSource *sol=new LightSource;	
	Vector3 *solrot=new Vector3(.4,.4,.2);	
	Vector3 *solpos=new Vector3(1000,1000,1000);
		sol->kRot=solrot;
		sol->kPos=solpos;		
		sol->kDiffuse=Vector4(2,2,2,2);
		sol->kAmbient=Vector4(0.01,0.01,0.01,0.01);
		sol->iType=POINT_LIGHT;			
		sol->iPriority=10;

	pkLight->Add(spot2);			
	pkLight->Add(spot);	
	pkLight->Add(sol);
	
	
	Object *kul=new Object;
//	PlayerControlProperty *fuck=new PlayerControl;
	kul->AddProperty(new PlayerControlProperty(test));
	ModelProperty *model=new ModelProperty();
	
	kul->AddProperty(model);	
	pkObjectMan->Add(kul);
	
}


void Test::OnIdle(void) {

//	pkLight->Update();
	pkRender->DrawSkyBox(*pkFps->m_kCamPos);
	pkRender->DrawHMlod(test,*pkFps->m_kCamPos,pkFps->m_iFps);		


	m_kSpotpos->x=sin(SDL_GetTicks()/1000.0)*50.0+80;
	m_kSpotpos->z=cos(SDL_GetTicks()/1000.0)*50.0+80;
	m_kSpotpos->y=50;

/*	glPushMatrix();
		glTranslatef(m_kSolpos->x,m_kSolpos->y,m_kSolpos->z);
		glColor3f(1,1,0);
//		glDepthMask(GL_FALSE);
		glutSolidSphere(20,10,10);
//		glDepthMask(GL_TRUE);		
	glPopMatrix();
		*/
		
	
	if(m_iGrass>0)
		for(int ix=0;ix<500;ix+=25)
			for(int iy=0;iy<500;iy+=25)
				pkRender->DrawGrassPatch(*pkFps->m_kCamPos,Vector3(ix,0,iy),Vector3(1.5,.3,1.5),25,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);

	glPushMatrix();
		glTranslatef(5,test->Height(5,5),5);
		glColor3f(0,0,1);
		glScalef(.02,.02,.02);
		CoreTest.SetFrameI(iCoreFrame);
		iCoreFrame++;
		if(iCoreFrame >= 289)
			iCoreFrame = 0;
		CoreTest.draw();
	glPopMatrix();

	pkRender->DrawWater(*pkFps->m_kCamPos,Vector3(512,0,512),Vector3(0,0,0),1200,30);	

	
//	cout<<pkFps->m_iFps<<endl;
	input();
	float z=pkFps->m_kCamPos->z;
	float x=pkFps->m_kCamPos->x;	
	
	if(pkFps->m_kCamPos->y<test->Height(x,z)+1)
		pkFps->m_kCamPos->y=test->Height(x,z)+1;	

}

void Test::OnHud(void) {	
	pkRender->SetFont("file:../data/textures/text/console.tga");
	fpsupdate++;
	if(fpsupdate>100){	
		IntToChar(fps,pkFps->m_iFps);
		fpsupdate=0;
	}
	pkRender->Print(Vector3(-1.1,.85,-1),Vector3(0,0,0),Vector3(0.06,.06,.06),"FPS:");	
	pkRender->Print(Vector3(-.9,.85,-1),Vector3(0,0,0),Vector3(0.06,.06,.06),fps);

}


void Test::input() {
	if(pkInput->Pressed(RIGHT)){
		pkFps->CamPos().x+=cos((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(LEFT)){
		pkFps->CamPos().x+=cos((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}
	
	if(pkInput->Pressed(UP))	{
			pkFps->CamPos().x+=cos((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
			pkFps->CamPos().z+=sin((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(DOWN))	{
		pkFps->CamPos().x+=cos((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(HOME))
		pkFps->CamPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(END))
		pkFps->CamPos().y-=2*pkFps->GetFrameTime()*speed;


	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->CamRot().x+=z/5.0;
	pkFps->CamRot().y+=x/5.0;
}

