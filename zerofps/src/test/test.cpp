#include "test.h"


Test olle("MegaGame",800,600,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}


void Test::OnInit(void) {
	pkConsole->Print("MegaUltraSuper Duper Game");
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
//	pkFps->m_pkAudioMan->PlayMusic();

	test=new HeightMap(pkFile);
//	test->Random();
	test->SetTileSet("file:../data/textures/landbw.bmp");
//	test->GenerateNormals();
//	test->GenerateTextures();
	
//	test->Save("test.hm");
//	test->Load("file:test.hm");
	test->LoadImageHmap("file:test.bmp");
	test->GenerateNormals();
	test->GenerateTextures();

	test->SetPosition(Vector3(-50,-10,-50));

	IntToChar(fps,pkFps->m_iFps);
	fpsupdate=0;

	light_position=Vector4(0.5,0.5,0.2,0);
	white_light = Vector4(1,1,1,1);
	lmodel_ambient = Vector4(0.5,0.5,0.5,1);

  
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

}


void Test::OnIdle(void) {
	input();
	
	glLightfv(GL_LIGHT0,GL_POSITION,&light_position[0]);	

//	glutSolidSphere(5,20,20);

pkRender->DrawHMlod(test,*pkFps->m_kCamPos);		


//	pkRender->DrawHM(test);		
	pkRender->DrawWater(*pkFps->m_kCamPos,Vector3(0,-6,200),Vector3(0,90,0),1000);
	cout<<pkFps->m_iFps<<endl;
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
		pkFps->CamPos().x+=cos((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*0.02;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*0.02;				
	}
	if(pkInput->Pressed(LEFT)){
		pkFps->CamPos().x+=cos((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*0.02;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*0.02;				
	}
	
	if(pkInput->Pressed(UP))	{
			pkFps->CamPos().x+=cos((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*0.02;			
			pkFps->CamPos().z+=sin((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*0.02;			
	}					
	if(pkInput->Pressed(DOWN))	{
		pkFps->CamPos().x+=cos((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*0.02;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*0.02;
	}		

	if(pkInput->Pressed(HOME))
		pkFps->CamPos().y+=2*pkFps->GetFrameTime()*0.01;			
	if(pkInput->Pressed(END))
		pkFps->CamPos().y-=2*pkFps->GetFrameTime()*0.01;


	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->CamRot().x+=z/5.0;
	pkFps->CamRot().y+=x/5.0;
}
