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
//	test->GenerateNormals();
//	test->GenerateTextures();
	
//	test->Save("test.hm");
//	test->Load("file:test.hm");
	test->LoadImageHmap("file:test2.bmp");
	test->GenerateNormals();
	test->GenerateTextures();

	test->SetPosition(Vector3(20,-4,0));
	

//	test->Height(1.02,4.99);
//	exit(0);

	IntToChar(fps,pkFps->m_iFps);
	fpsupdate=0;

	light_position=Vector4(0.5,0.5,0.2,0);
	float b=1.5;
	white_light = Vector4(b,b,b,b);
	lmodel_ambient = Vector4(0.5*b,0.5*b,0.5*b,0.5*b);

  
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
  
  pkRender->SetFog(Vector4(.2,.2,.6,1),0.50,300,400,true);

}


void Test::OnIdle(void) {
	input();
	
	glLightfv(GL_LIGHT0,GL_POSITION,&light_position[0]);	

	pkRender->DrawSkyBox(*pkFps->m_kCamPos);
	pkRender->DrawHMlod(test,*pkFps->m_kCamPos,pkFps->m_iFps);		
	
	float z=pkFps->m_kCamPos->z;
	float x=pkFps->m_kCamPos->x;	
	
	if(pkFps->m_kCamPos->y<test->Height(x,z)+2.4)
		pkFps->m_kCamPos->y=test->Height(x,z)+2.5;	
//	pkFps->m_kCamPos->y=test->Height(x,z)+4;	
//	if(pkRender->SphereInFrustum(*pkFps->m_kCamPos,Vector4(100,0,100,10)))
//		cout<<"HORA"<<endl;
	
//	if(pkRender->CubeInFrustum(100,0,100,10))
//		cout<<"HORA"<<endl;
	
//	cout<<">>>> "<<pkRender->SphereInFrustum(*pkFps->m_kCamPos,Vector4(100,0,100,0))<<endl;	

//		cout<<"you se it=)"<<endl;

	glPushMatrix();
		glTranslatef(100,0,100);
		glutSolidSphere(10,10,10);
	glPopMatrix();

	pkRender->DrawWater(*pkFps->m_kCamPos,Vector3(512,0,512),Vector3(0,0,0),1200,30);
	
//	cout<<pkFps->m_iFps<<endl;
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
	float speed=0.05;

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

