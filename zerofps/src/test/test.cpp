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
	test->SetTileSet("file:../data/textures/land.bmp");
	test->GenerateNormals();
	test->GenerateTextures();
//	exit(0);

/*
	test2=new HeightMap();
	test2->Random();
	test2->GenerateNormals();
	test2->SetPosition(Vector3(0,0,50));
*/
//	float x,y;
//	pkRender->GiveTexCor(x,y,4);
//s	exit(1);


	GLfloat light_position[] ={0,60,0,1};
	GLfloat white_light[] = {0.6,0.6,0.6,0.6};
	GLfloat lmodel_ambient[] = {0.6,0.6,0.6,0.6};

  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
  glColorMaterial(GL_BACK,GL_AMBIENT_AND_DIFFUSE);

  glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
  glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);  
  glLightfv(GL_LIGHT0,GL_POSITION,light_position);	  
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

}


void Test::OnIdle(void) {
	input();
	
	float x,z;
	x=sin(pkFps->GetTicks()/1000.0)*400;
	z=cos(pkFps->GetTicks()/1000.0)*400;
//  light_position[0]=x;
//  light_position[2]=z;
  glLightfv(GL_LIGHT0,GL_POSITION,light_position);	

	pkRender->DrawHMlod(test,*pkFps->m_kCamPos);		
//	pkRender->DrawHMlod(test2,*pkFps->m_kCamPos);			
//	pkRender->DrawHM(test);		
	pkRender->SetFont("file:../data/textures/text/console.bmp");	
	pkRender->Print(Vector3(x,20,z),Vector3(0,0,0),Vector3(5,5,5),"HEJ JULLE");

	cout<<pkFps->m_iFps<<endl;
	
//	for(int i=2;i<60;i++)
//		pkRender->Pyra(sin(pkFps->GetTicks()/1000.0*i/2)*2,cos(pkFps->GetTicks()/1000.0*i/2)*2,-i/2);


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
