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

	testa.Load("babe.pmd");
	
	pkLight->SetCamera(pkFps->m_kCamPos);
	
	LightSource *spot=new LightSource();
	Vector3 *spotpos=new Vector3(25,10,25);
	Vector3 *spotrot=new Vector3(.8,-.2,0);	
		spot->kPos=spotpos;
		spot->kRot=spotrot;
		spot->kDiffuse=Vector4(0,0,2,1);
		spot->kAmbient=Vector4(0,0,0,0);		
		spot->fCutoff=20;	
		spot->fExp=20;
		spot->fConst_Atten=0;
		spot->fQuadratic_Atten=0.001;
		spot->iType=SPOT_LIGHT;			


	LightSource *sol=new LightSource;	
	Vector3 *solrot=new Vector3(.4,.4,.2);	
	Vector3 *solpos=new Vector3(1000,1000,1000);
		sol->kRot=solrot;
		sol->kPos=solpos;		
		sol->kDiffuse=Vector4(1.0,1.0,1.0,1);
		sol->kAmbient=Vector4(.1,.1,.1,.1);
		sol->iType=POINT_LIGHT;			
		sol->iPriority=10;


//	pkLight->Add(spot);
	pkLight->Add(sol);

}


void Test::OnIdle(void) {

//	pkLight->Update();
	pkRender->DrawSkyBox(*pkFps->m_kCamPos);
	pkRender->DrawHMlod(test,*pkFps->m_kCamPos,pkFps->m_iFps);		


//	m_kSolpos->x=sin(SDL_GetTicks()/4000.0)*2000+512;
//	m_kSolpos->y=cos(SDL_GetTicks()/4000.0)*2000;
//	m_kSolpos->z=512;

/*	glPushMatrix();
		glTranslatef(m_kSolpos->x,m_kSolpos->y,m_kSolpos->z);
		glColor3f(1,1,0);
//		glDepthMask(GL_FALSE);
		glutSolidSphere(20,10,10);
//		glDepthMask(GL_TRUE);		
	glPopMatrix();
		*/
		
	
	if(m_iGrass>0)
		for(int ix=0;ix<500;ix+=10)
			for(int iy=0;iy<500;iy+=10)
				pkRender->DrawGrassPatch(*pkFps->m_kCamPos,Vector3(ix,0,iy),Vector3(1,.1,1),10,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);


	glPushMatrix();
		glTranslatef(70,test->Height(70,50),50);
		glColor3f(0,0,1);
		glScalef(.01,.01,.01);
		testa.draw();
	glPopMatrix();

	pkRender->DrawWater(*pkFps->m_kCamPos,Vector3(512,0,512),Vector3(0,0,0),1200,30);	

	
//	cout<<pkFps->m_iFps<<endl;
	input();
	float z=pkFps->m_kCamPos->z;
	float x=pkFps->m_kCamPos->x;	
	
	if(pkFps->m_kCamPos->y<test->Height(x,z)+.5)
		pkFps->m_kCamPos->y=test->Height(x,z)+.5;	

}

void Test::OnHud(void) {	
	pkRender->SetFont("file:../data/textures/text/console.tga");
	fpsupdate++;
	if(fpsupdate>100){	
		IntToChar(fps,pkFps->m_iFps);
		fpsupdate=0;
	}
	pkRender->Print(Vector3(-.9,.9,-1),Vector3(0,0,0),Vector3(0.06,.06,.06),"FPS:");	
	pkRender->Print(Vector3(-.7,.9,-1),Vector3(0,0,0),Vector3(0.06,.06,.06),fps);

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

