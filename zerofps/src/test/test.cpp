#include "test.h"


Test olle("MegaGame",800,600,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}


void Test::OnInit(void) {
	pkConsole->Print("MegaUltraSuper Duper Game");
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
	pkFps->m_pkAudioMan->PlayMusic();

	test=new HeightMap();
	test->Random();

	GLfloat light_position[] ={10,15,-20,0};
	GLfloat white_light[] = {1.0,1.0,1.0,1.0};
	GLfloat lmodel_ambient[] = {0.6,0.6,0.6,0.6};

  glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
  glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);
//  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

  
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);

}


void Test::OnIdle(void) {
//   glLightfv(GL_LIGHT0,GL_POSITION,light_position);	
	input();
  glLightfv(GL_LIGHT0,GL_POSITION,light_position);	

	pkRender->DrawHM(test);		
	pkRender->
	SetFont("file:../data/textures/text/console.bmp");	
	pkRender->Print(Vector3(10,15,-20),Vector3(0,0,0),Vector3(1,1,1),"HEJ JULLE");

	cout<<pkFps->m_iFps<<endl;
	
//	for(int i=2;i<60;i++)
//		pkRender->Pyra(sin(pkFps->GetTicks()/1000.0*i/2)*2,cos(pkFps->GetTicks()/1000.0*i/2)*2,-i/2);

//	pkRender->Pyra(0,0,0);

}


void Test::input() {
	if(pkInput->Pressed(RIGHT)){
		pkFps->CamPos().x+=cos((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*0.01;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*0.01;				
	}
	if(pkInput->Pressed(LEFT)){
		pkFps->CamPos().x+=cos((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*0.01;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*0.01;				
	}
	
	if(pkInput->Pressed(UP))	{
			pkFps->CamPos().x+=cos((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*0.01;			
			pkFps->CamPos().z+=sin((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*0.01;			
	}					
	if(pkInput->Pressed(DOWN))	{
		pkFps->CamPos().x+=cos((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*0.01;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*0.01;
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
