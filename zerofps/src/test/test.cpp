#include "test.h"
#include "walker.h"

Test olle("MegaGame",1024,768,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}

void Test::OnInit(void) {
	
	g_ZFObjSys.Register_Cmd("loadmap",FID_LOADMAP,this);	
	g_ZFObjSys.Register_Cmd("savemap",FID_SAVEMAP,this);		
	
	pkConsole->Printf(" ZeroFps test program =) ");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Use loadmap/savemap to load and save heightmaps ");
	
	
//	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
//	pkFps->m_pkAudioMan->PlayMusic();

          
//------------Map---------
	test=new HeightMap();
	test->SetTileSet("file:../data/textures/landbw.bmp");
	if(!test->LoadImageHmap("file:islands.bmp"))
		cout<<"Error loading map"<<endl;
	test->GenerateNormals(); 
	test->GenerateTextures();

//	test->Save("blub.hm");
//	test->Load("blub.hm");

	// Detta är ett test
	int apa = 2;

	test->SetPosition(Vector3(0,-4,0));
//----------------------------
	
	pkFps->m_pkCmd->Add(&speed,"g_speed",type_float);		
	speed=30; 
  	
	m_iGrass=0;	
	pkFps->m_pkCmd->Add(&m_iGrass,"g_grass",type_int);		  	
	m_iGrassVolyme=150;	
	pkFps->m_pkCmd->Add(&m_iGrassVolyme,"g_grassvolyme",type_int);		  	

  
//	pkRender->SetFog(Vector4(.50,.55,.88,1),8,100,200,true);
	pkRender->SetFog(Vector4(0,0,0,1),8,100,200,true);
	
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
//		sol->kDiffuse=Vector4(1.8,1.8,1.8,1);	//Dag
		sol->kDiffuse=Vector4(1,1,1,1);	//natt
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
	for( i=0;i<1;i++) {
		Object *ball=new BallObject();
		float x=289;	// + rand()%200;
		float y=828;	// + rand()%100;
		ball->GetPos()=Vector3(x,test->Height(x,y)+5,y);
		pkObjectMan->Add(ball);
		pkCollisionMan->Add(ball);
	}

	glEnable(GL_LIGHTING );
	
	cam1=new Camera(Vector3(5,50,5),Vector3(0,0,0),85,1.333,0.25,250);	
	cam2=new Camera(Vector3(5,50,5),Vector3(0,0,0),85,1.333,0.25,250);	
	cam2->SetViewPort(0.7,0.7,0.29,0.29);

	//camera rabit
	Object *sussi = new BunnyObject();
	float x=300 + rand()%100;
	float y=750 + rand()%100;
	sussi->GetPos()=Vector3(x,test->Height(x,y),y);
	sussi->AddProperty(new CameraProperty(cam2));
	pkObjectMan->Add(sussi);
	pkCollisionMan->Add(sussi);

	//player
	m_pkPlayer=new PlayerObject(test,pkInput);
	m_pkPlayer->GetPos().Set(400,25,705);		
	m_pkPlayer->AddProperty(new CameraProperty(cam1));
	pkObjectMan->Add(m_pkPlayer);
	pkCollisionMan->Add(m_pkPlayer);
	
	
	//skybox
	SkyBoxObject *sky=new SkyBoxObject("file:../data/textures/skybox-hor.bmp","file:../data/textures/skybox-topbotom.bmp");
	sky->SetRotate(Vector3(.5,0,0));
	pkObjectMan->Add(sky);	

	
	//water
	WaterObject *water=new WaterObject(1200,50,"file:../data/textures/water2.bmp");
	water->GetPos().Set(512,0,512);
	pkObjectMan->Add(water);	
	
	
	//Heightmap
	HeightMapObject *hm=new HeightMapObject(test);
	hm->GetPos().Set(0,-4,0);			
	pkObjectMan->Add(hm);	
	pkCollisionMan->Add(hm);



	Sound *welcome=new Sound();
	welcome->m_acFile="file:../data/sound/welcome.wav";
	welcome->m_kPos.Set(300,25,785);
	welcome->m_bLoop=true;
	pkAlSys->AddSound(welcome);

	ZFObjectManger::GetInstance()->PrintObjects();

}

void Test::OnServerStart(void)
{	
	int i;

	Object *sussi;
	for(i=0;i<1;i++) {
		sussi=new BunnyObject();
		float x=300 + rand()%100;
		float y=750 + rand()%100;
		sussi->GetPos()=Vector3(x,test->Height(x,y),y);
		
		pkObjectMan->Add(sussi);
		pkCollisionMan->Add(sussi);
	}
}

void Test::OnClientStart(void)
{

}



void Test::OnIdle(void) {
//	m_pkPlayer->GetPos().print();
//	glFogCoordfEXT(-20);


//	m_kSpotpos->x=sin(SDL_GetTicks()/1000.0)*50.0+80;
//	m_kSpotpos->z=cos(SDL_GetTicks()/1000.0)*50.0+80;
//	m_kSpotpos->y=50;

/*
	if(m_iGrass>0){
		for(int ix=0;ix<1000;ix+=30)
			for(int iy=0;iy<1000;iy+=30){
				pkRender->DrawGrassPatch(pkFps->GetCam()->GetPos(),Vector3(ix,0,iy),Vector3(1.5,.3,1.5),30,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);
				pkRender->DrawGrassPatch(pkFps->GetCam()->GetPos(),Vector3(ix,0,iy),Vector3(1,.5,1),30,m_iGrassVolyme/4,test,pkTexMan->Load("file:../data/textures/grass3.tga",T_NOMIPMAPPING),pkFps->m_iFps);
			}
	}
*/

	pkFps->SetCamera(cam1);		
		pkFps->GetCam()->ClearViewPort();	

//		pkRender->DrawSkyBox(pkFps->GetCam()->GetPos());
//		pkRender->DrawHMlod(test,pkFps->GetCam()->GetPos(),pkFps->m_iFps);			
		pkObjectMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);


		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.3);
			pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),Vector3(140,50,450),20,pkTexMan->Load("file:../data/textures/star.tga",T_NOMIPMAPPING));	
	
//			pkRender->DrawWater(pkFps->GetCam()->GetPos(),Vector3(512,0,512),Vector3(0,0,0),1200,50,pkTexMan->Load("file:../data/textures/water2.bmp",0));	
//		pkRender->DrawWater(pkFps->GetCam()->GetPos(),Vector3(512,0,512),Vector3(0,0,0),1200,50);	
	

	
	
	
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
	//pkRender->Print(Vector3(-1.1,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),"FPS:");	
	//pkRender->Print(Vector3(-.9,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),fps);

	pkFps->DevPrintf("Fps: %d",pkFps->m_iFps);

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
/*
	if(pkInput->Pressed(HOME)) {
		//pkFps->m_kPropertyFactory.Display();
		cout << "Gaaaaaaaaaaaaa " << endl;
		//g_ZFObjSys.PrintObjects();
		NetPacket paket;
		paket.Write_Str("Hello World :)");
		paket.SetTarget("192.168.0.111:4242");
		pkFps->m_pkNetWork->Send(&paket);
		}
*/		
		
	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;
}

void Test::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOADMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			if(!test->Load(kCommand->m_kSplitCommand[1].c_str()))
				pkConsole->Printf("Could not load map =(");
			break;
	
		case FID_SAVEMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				return;
			}
			
			if(!test->Save(kCommand->m_kSplitCommand[1].c_str()))
				pkConsole->Printf("Could not save map =(");			
			break;
	
	}


}







