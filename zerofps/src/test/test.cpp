#include "test.h"


Test olle("MegaGame",1024,768,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}


void Test::OnInit(void) {
	pkConsole->Print("MegaUltraSuper Duper Game");
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
//	pkFps->m_pkAudioMan->PlayMusic();

	test=new HeightMap(pkFile);
//	test->Random();
	test->SetTileSet("file:../data/textures/landbw.bmp");
	
//	test->Save("test.hm");
//	test->Load("file:test.hm");
	test->LoadImageHmap("file:test2.bmp");
	test->GenerateNormals();
	test->GenerateTextures();

	test->SetPosition(Vector3(20,-4,0));
	
	IntToChar(fps,pkFps->m_iFps);
	fpsupdate=0;

	light_position=Vector4(0.5,0.5,0.2,0);
	float b=1.2;
	white_light = Vector4(b,b,b,b);	
	lmodel_ambient = Vector4(0.2*b,0.2*b,0.2*b,0.2*b);

	pkFps->m_pkCmd->Add(&speed,"g_speed",type_float);		
	speed=0.05;
  	
  	m_iGrass=0;	
	pkFps->m_pkCmd->Add(&m_iGrass,"g_grass",type_int);		  	
  	m_iGrassVolyme=150;	
	pkFps->m_pkCmd->Add(&m_iGrassVolyme,"g_grassvolyme",type_int);		  	

  
//  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
//  glColorMaterial(GL_BACK,GL_AMBIENT_AND_DIFFUSE);

  glLightfv(GL_LIGHT0,GL_DIFFUSE,&white_light[0]);
//  glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);  
  glLightfv(GL_LIGHT0,GL_AMBIENT,&lmodel_ambient[0]);
  glLightfv(GL_LIGHT0,GL_POSITION,&light_position[0]);	  
//  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  pkRender->SetFog(Vector4(.50,.55,.88,1),2,225,350,true);


	testa.Load("babe.pmd");
}


void Test::OnIdle(void) {

	
	glLightfv(GL_LIGHT0,GL_POSITION,&light_position[0]);	

	pkRender->DrawSkyBox(*pkFps->m_kCamPos);
	pkRender->DrawHMlod(test,*pkFps->m_kCamPos,pkFps->m_iFps);		
	
//	cout<<"HORA "<<(1|MIPMAPING)<<endl;
	
	if(m_iGrass>0)
		for(int ix=0;ix<1000;ix+=50)
			for(int iy=0;iy<1000;iy+=50)
				pkRender->DrawGrassPatch(*pkFps->m_kCamPos,Vector3(ix,0,iy),Vector3(3.5,.8,3.5),50,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);


	glPushMatrix();
		glTranslatef(200,test->Height(200,300),300);
		glScalef(.05,.05,.05);
		testa.draw();
	glPopMatrix();
//	cout<<pkFps->m_kCamPos->x<<" "<<pkFps->m_kCamPos->y<<" "<<pkFps->m_kCamPos->z<<endl;
/*	srand(10);
	for(int i=0;i<5000;i++) {
		float x=rand()%500/10.0+290;
		float z=rand()%500/10.0+350;		
		float y=test->Height(x,z)+.4;
		pkRender->DrawCross(*pkFps->m_kCamPos,Vector3(x,y,z),Vector3(0,rand()%320,0),Vector3(3,.8,.8),pkTexMan->Load("file:../data/textures/grass.tga"),pkTexMan->Load("file:../data/textures/tuva-top.tga"));			
//		pkRender->DrawCross(Vector3(x,y,z),Vector3(rand()%30-15,rand()%320,rand()%30-15),Vector3(.8,.8,.8),pkTexMan->Load("file:../data/textures/tuva.tga"),pkTexMan->Load("file:../data/textures/tuva-top.tga"));	
//		pkRender->DrawCross(Vector3(x,y,z),Vector3(rand()%30-15,rand()%320,rand()%30-15),Vector3(8,13,8),pkTexMan->Load("file:../data/textures/tree.tga"),pkTexMan->Load("file:../data/textures/tree-top.tga"));			
	}*/
	
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

