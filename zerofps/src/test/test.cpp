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
	speed=0.05;
  	
  	m_iGrass=0;	
	pkFps->m_pkCmd->Add(&m_iGrass,"g_grass",type_int);		  	
  	m_iGrassVolyme=150;	
	pkFps->m_pkCmd->Add(&m_iGrassVolyme,"g_grassvolyme",type_int);		  	

  
  pkRender->SetFog(Vector4(.50,.55,.88,1),2,225,350,true);

	testa.Load("babe.pmd");
	
	pkLight->SetCamera(pkFps->m_kCamPos);
	
	LightSource *spot=new LightSource;
	Vector3 *spotpos=new Vector3(50,33,50);
	Vector3 *spotrot=new Vector3(0,-1,0);	
		spot->kPos=spotpos;
		spot->kRot=spotrot;
		spot->kConstPos=Vector3(0,0,0);
		spot->kConstRot=Vector3(0,0,0);		
		spot->kDiffuse=Vector4(2,2,1,1);
		spot->kAmbient=Vector4(0,0,0,0);
		spot->kSpecular=Vector4(0,0,0,0);
		spot->fCutoff=20;	
		spot->fExp=20;
		spot->fConst_Atten=1;
		spot->fLinear_Atten=0;
		spot->fQuadratic_Atten=0.001;
		spot->iType=SPOT_LIGHT;			

	LightSource *spot2=new LightSource;
	Vector3 *spotpos2=new Vector3(50,33,70);
	Vector3 *spotrot2=new Vector3(0,-1,0);	
		spot2->kPos=spotpos2;
		spot2->kRot=spotrot2;
		spot2->kConstPos=Vector3(0,0,0);
		spot2->kConstRot=Vector3(0,0,0);		
		spot2->kDiffuse=Vector4(4,0,0,1);
		spot2->kAmbient=Vector4(0,0,0,0);
		spot2->kSpecular=Vector4(0,0,0,0);
		spot2->fCutoff=20;	
		spot2->fExp=20;
		spot2->fConst_Atten=1;
		spot2->fLinear_Atten=0;
		spot2->fQuadratic_Atten=0.001;
		spot2->iType=SPOT_LIGHT;			


	LightSource *sol=new LightSource;	
	Vector3 *solrot=new Vector3(0,0,0);	
	m_kSolpos=new Vector3(300,20,350);
		sol->kPos=m_kSolpos;
		sol->kRot=solrot;
		sol->kConstPos=Vector3(0,0,0);
		sol->kConstRot=Vector3(0,0,0);		
		sol->kDiffuse=Vector4(1.2,1.2,0.9,1);
		sol->kAmbient=Vector4(.1,.1,.1,.1);
		sol->kSpecular=Vector4(0,0,0,0);
		sol->fCutoff=20;	
		sol->fExp=20;
		sol->fConst_Atten=1;
		sol->fLinear_Atten=0;
		sol->fQuadratic_Atten=0;
		sol->iType=POINT_LIGHT;			



	pkLight->Add(spot);
	pkLight->Add(sol);
	pkLight->Add(spot2);	

}


void Test::OnIdle(void) {

//	cout<<"ROT: X"<<pkFps->m_kCamRot->x<<" Y "<<pkFps->m_kCamRot->y<<" Z "<<pkFps->m_kCamRot->z<<endl;

	pkLight->Update();

	pkRender->DrawSkyBox(*pkFps->m_kCamPos);
	pkRender->DrawHMlod(test,*pkFps->m_kCamPos,pkFps->m_iFps);		


	m_kSolpos->x=sin(SDL_GetTicks()/4000.0)*2000+512;
	m_kSolpos->y=cos(SDL_GetTicks()/4000.0)*2000;
	m_kSolpos->z=512;

/*	glPushMatrix();
		glTranslatef(m_kSolpos->x,m_kSolpos->y,m_kSolpos->z);
		glColor3f(1,1,0);
//		glDepthMask(GL_FALSE);
		glutSolidSphere(20,10,10);
//		glDepthMask(GL_TRUE);		
	glPopMatrix();
		*/
		
	
	if(m_iGrass>0)
		for(int ix=0;ix<1000;ix+=50)
			for(int iy=0;iy<1000;iy+=50)
				pkRender->DrawGrassPatch(*pkFps->m_kCamPos,Vector3(ix,0,iy),Vector3(3.5,.8,3.5),50,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);


	glPushMatrix();
		glTranslatef(300,test->Height(300,400),400);
		glColor3f(0,0,1);
		glScalef(.025,.025,.025);
		testa.draw();
	glPopMatrix();

	pkRender->DrawWater(*pkFps->m_kCamPos,Vector3(512,0,512),Vector3(0,0,0),1200,30);	

	
//	cout<<pkFps->m_iFps<<endl;
	input();
	float z=pkFps->m_kCamPos->z;
	float x=pkFps->m_kCamPos->x;	
	
	if(pkFps->m_kCamPos->y<test->Height(x,z)+1.5)
		pkFps->m_kCamPos->y=test->Height(x,z)+1.5;	

}

void Test::OnHud(void) {	
	pkRender->SetFont("file:../data/textures/text/console.bmp");
	fpsupdate++;
	if(fpsupdate>100){	
		IntToChar(fps,pkFps->m_iFps);
		fpsupdate=0;
	}
	pkRender->Print(Vector3(-0.2,0.2,-0.25),Vector3(0,0,0),Vector3(0.01,.01,.01),fps);

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

