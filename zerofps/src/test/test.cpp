#include "test.h"
#include "walker.h"


Test olle("MegaGame",1024,768,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}


void Test::OnInit(void) {
	
//	pkConsole->Print("MegaUltraSuper Duper Game");
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
//	pkFps->m_pkAudioMan->PlayMusic();


//------------Map---------
	test=new HeightMap(pkFile);
//	test->Random();
	test->SetTileSet("file:../data/textures/landbw.bmp");
	
//	test->Save("test.hm");
//	test->Load("file:test.hm");
	if(!test->LoadImageHmap("file:islands.bmp"))
		cout<<"Error loading map"<<endl;
	test->GenerateNormals();
	test->GenerateTextures();

	test->SetPosition(Vector3(0,-4,0));
//----------------------------
	
	IntToChar(fps,pkFps->m_iFps);
	fpsupdate=0;

	pkFps->m_pkCmd->Add(&speed,"g_speed",type_float);		
	speed=30;
  	
	m_iGrass=0;	
	pkFps->m_pkCmd->Add(&m_iGrass,"g_grass",type_int);		  	
	m_iGrassVolyme=150;	
	pkFps->m_pkCmd->Add(&m_iGrassVolyme,"g_grassvolyme",type_int);		  	

  
//	pkFps->GetCam()->SetView(90,1.333,0.25,400);
//  	pkFps->GetCam()->SetPos(Vector3(20,50,30));
  
	pkRender->SetFog(Vector4(.50,.55,.88,1),8,100,200,true);

	
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
		sol->kDiffuse=Vector4(1,1,1,1);
		sol->kAmbient=Vector4(0.01,0.01,0.01,0.01);
//		sol->kAmbient=Vector4(0,0,0,0);		
		sol->iType=POINT_LIGHT;			
		sol->iPriority=10;
		sol->fConst_Atten=1;
		sol->fLinear_Atten=0;
		sol->fQuadratic_Atten=0;


/*	pkLight->Add(spot2);			
	pkLight->Add(spot);	*/
	pkLight->Add(sol);
	
	int i;
	
	for( i=0;i<4;i++) {
		Object *ball=new WalkerObject();
		float x=290 + rand()%200;
		float y=730 + rand()%100;
		ball->GetPos()=Vector3(x,test->Height(x,y)+5,y);
		pkObjectMan->Add(ball);
		pkCollisionMan->Add(ball);
	}

/*	Object *sussi;
	for(i=0;i<5;i++) {
		sussi=new BunnyObject();
		float x=340 + rand()%10;
		float y=780 + rand()%10;
		sussi->GetPos()=Vector3(x,test->Height(x,y),y);
		pkObjectMan->Add(sussi);
		pkCollisionMan->Add(sussi);
	}*/



	glEnable(GL_LIGHTING );
	
	cam1=new Camera(Vector3(5,50,5),Vector3(0,0,0),90,1.333,0.25,400);	
	
	m_pkPlayer=new PlayerObject(test,pkInput);
	m_pkPlayer->GetPos()=Vector3(340,25,780);		
	m_pkPlayer->AddProperty(new CameraProperty(cam1));
	pkObjectMan->Add(m_pkPlayer);
	pkCollisionMan->Add(m_pkPlayer);
	
	//add a collisionproperty for our heightmap
	HeightMapObject *hm=new HeightMapObject(test);
	pkCollisionMan->Add(hm);


/*	  
	float fogColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};

  glEnable(GL_FOG);                       // Turn on fog
  glFogi(GL_FOG_MODE, GL_LINEAR);         // Set the fog mode to LINEAR (Important)
  glFogfv(GL_FOG_COLOR, fogColor);        // Give OpenGL our fog color
  glFogf(GL_FOG_START, 0.0);              // Set the start position for the depth at 0
  glFogf(GL_FOG_END, 50.0);               // Set the end position for the detph at 50
*/

}




void Test::OnIdle(void) {
//	m_pkPlayer->GetPos().print();

//	glFogCoordfEXT(-20);
	

	m_kSpotpos->x=sin(SDL_GetTicks()/1000.0)*50.0+80;
	m_kSpotpos->z=cos(SDL_GetTicks()/1000.0)*50.0+80;
	m_kSpotpos->y=50;

	pkFps->SetCamera(cam1);		
	pkFps->GetCam()->ClearViewPort();	

	pkRender->DrawSkyBox(pkFps->GetCam()->GetPos());
	pkRender->DrawHMlod(test,pkFps->GetCam()->GetPos(),pkFps->m_iFps);			
	

	if(m_iGrass>0){
		for(int ix=0;ix<1000;ix+=30)
			for(int iy=0;iy<1000;iy+=30){
				pkRender->DrawGrassPatch(pkFps->GetCam()->GetPos(),Vector3(ix,0,iy),Vector3(1.5,.3,1.5),30,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);
				pkRender->DrawGrassPatch(pkFps->GetCam()->GetPos(),Vector3(ix,0,iy),Vector3(1,.5,1),30,m_iGrassVolyme/4,test,pkTexMan->Load("file:../data/textures/grass3.tga",T_NOMIPMAPPING),pkFps->m_iFps);
			}
	}

//	pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),Vector3(0,70,0),4,pkTexMan->Load("file:../data/textures/ball.tga",T_NOMIPMAPPING));
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.3);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),Vector3(140,50,450),20,pkTexMan->Load("file:../data/textures/star.tga",T_NOMIPMAPPING));	
//	glDisable(GL_ALPHA_TEST);
	
	pkRender->DrawWater(pkFps->GetCam()->GetPos(),Vector3(512,0,512),Vector3(0,0,0),1200,50);	
//	pkRender->DrawSimpleWater(Vector3(0,-5,0),Vector4(.5,.5,.5,.99),1024,pkTexMan->Load("file:../data/textures/water3.bmp",0));	
	
	input();
	float z=pkFps->GetCam()->GetPos().z;
	float x=pkFps->GetCam()->GetPos().x;	
	
	if(pkFps->GetCam()->GetPos().y<test->Height(x,z)+1)
		pkFps->GetCam()->GetPos().y=test->Height(x,z)+1;	

}

void Test::OnHud(void) {	
	glPushAttrib(GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);

	pkRender->SetFont("file:../data/textures/text/console.tga");
	fpsupdate++;
	if(fpsupdate>100){	
		IntToChar(fps,pkFps->m_iFps);
		fpsupdate=0;
	}
	//pkRender->Print(Vector3(-1.1,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),"FPS:");	
	//pkRender->Print(Vector3(-.9,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),fps);

	pkFps->DevPrintf("Fiskarna i havet");
	pkFps->DevPrintf("Fps: %d aa",pkFps->m_iFps);
	pkFps->DrawDevStrings();

	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);

	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkPlayer->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
	glDisable(GL_ALPHA_TEST);
	
	glPopAttrib();
}


void Test::input() {
	if(pkInput->Pressed(RIGHT)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(LEFT)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}
	
	if(pkInput->Pressed(UP))	{
			pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
			pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(DOWN))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(HOME))
		pkFps->GetCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(END))
		pkFps->GetCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;

	if(pkInput->Pressed(HOME)) {
		//pkFps->m_kPropertyFactory.Display();
		g_ZFObjSys.PrintObjects();
		}
		
		
	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;
}








