#include "test.h"


Test olle("MegaGame",1024,768,16);
//Test olle("MegaGame",1024,768,16);

Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {

}


void Test::OnInit(void) {
//	pkFps->SetDisplay(640,480,16);
	
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

	pkFps->m_pkCmd->Add(&speed,"g_speed",type_float);		
	speed=0.025;
  	
	m_iGrass=0;	
	pkFps->m_pkCmd->Add(&m_iGrass,"g_grass",type_int);		  	
	m_iGrassVolyme=150;	
	pkFps->m_pkCmd->Add(&m_iGrassVolyme,"g_grassvolyme",type_int);		  	

  
	pkFps->GetDefaultCam()->SetView(90,1.333,0.25,400);
  	pkFps->GetDefaultCam()->SetPos(Vector3(20,50,30));
  
	pkRender->SetFog(Vector4(.50,.55,.88,1),8,100,200,true);

	FH_SkapaObject();


	
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
	
	
	PlayerBallObject *kul=new PlayerBallObject(test,pkInput,pkFps);
	kul->GetPos()=Vector3(5,5,5);		
	pkObjectMan->Add(kul);
	pkCollisionMan->Add(kul);

	for(int i=0;i<20;i++) {
		Object *ball=new BallObject(test);
		ball->AddProperty(new MadProperty(&akCoreModells[0]));
		float x=rand()%20;
		float y=rand()%20;
		ball->GetPos()=Vector3(x,test->Height(x,y),y);
		ball->GetStatic()=true;
		pkObjectMan->Add(ball);
		pkCollisionMan->Add(ball);
	}


	glEnable(GL_LIGHTING );
	
	

//	Camera *testcam=new Camera(Vector3(50,50,50),Vector3(0,0,0),1.33,90,0.4,300);
//	pkFps->SetGameCam(testcam);

}


void Test::OnIdle(void) {
//	cout<<pkFps->m_kCamPos->y<<endl;


//	pkLight->Update();
	pkRender->DrawSkyBox(pkFps->GetGameCam()->GetPos());
	pkRender->DrawHMlod(test,pkFps->GetGameCam()->GetPos(),pkFps->m_iFps);		


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
		
	
	if(m_iGrass>0){
		for(int ix=0;ix<1000;ix+=30)
			for(int iy=0;iy<1000;iy+=30){
				pkRender->DrawGrassPatch(pkFps->GetGameCam()->GetPos(),Vector3(ix,0,iy),Vector3(1.5,.3,1.5),30,m_iGrassVolyme,test,pkTexMan->Load("file:../data/textures/grass2.tga",T_NOMIPMAPPING),pkFps->m_iFps);
				pkRender->DrawGrassPatch(pkFps->GetGameCam()->GetPos(),Vector3(ix,0,iy),Vector3(1,.5,1),30,m_iGrassVolyme/4,test,pkTexMan->Load("file:../data/textures/grass3.tga",T_NOMIPMAPPING),pkFps->m_iFps);
			}
	}

	pkRender->DrawBillboard(pkFps->GetGameCam()->GetPos(),Vector3(0,70,0),Vector3(2,2,2),pkTexMan->Load("file:../data/textures/ball.tga",T_NOMIPMAPPING));

	
	pkRender->DrawWater(pkFps->GetGameCam()->GetPos(),Vector3(512,0,512),Vector3(0,0,0),1200,30);	

	//FH_UpdateraObject();
	//FH_RitaObject();

//	cout<<pkFps->m_iFps<<endl;
	input();
	float z=pkFps->GetDefaultCam()->GetPos().z;
	float x=pkFps->GetDefaultCam()->GetPos().x;	
	
	if(pkFps->GetDefaultCam()->GetPos().y<test->Height(x,z)+1)
		pkFps->GetDefaultCam()->GetPos().y=test->Height(x,z)+1;	

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
		pkFps->GetDefaultCam()->GetPos().x+=cos((pkFps->GetDefaultCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->GetDefaultCam()->GetPos().z+=sin((pkFps->GetDefaultCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(LEFT)){
		pkFps->GetDefaultCam()->GetPos().x+=cos((pkFps->GetDefaultCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetDefaultCam()->GetPos().z+=sin((pkFps->GetDefaultCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}
	
	if(pkInput->Pressed(UP))	{
			pkFps->GetDefaultCam()->GetPos().x+=cos((pkFps->GetDefaultCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
			pkFps->GetDefaultCam()->GetPos().z+=sin((pkFps->GetDefaultCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(DOWN))	{
		pkFps->GetDefaultCam()->GetPos().x+=cos((pkFps->GetDefaultCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetDefaultCam()->GetPos().z+=sin((pkFps->GetDefaultCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(HOME))
		pkFps->GetDefaultCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(END))
		pkFps->GetDefaultCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;


	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetDefaultCam()->GetRot().x+=z/5.0;
	pkFps->GetDefaultCam()->GetRot().y+=x/5.0;
}

char *MdlNames[] =
{
	"bitch.mad",
/*	"band.mad",
	"berserk.mad",
	"bitch.mad",
	"body.mad",
	"boss1.mad",
	"boss2.mad",
	"brain.mad",
	"breath.mad",
	"bullets.mad",
	"cells.mad",
	"chead.mad",
	"combat.mad",
	"commandr.mad",

	"datacd.mad",
	"deadbod.mad",
	"effect.mad",
	"enviro.mad",
	"flipper.mad",
	"float.mad",
	"flyer.mad",
	"gladiatr.mad",
	"grenades.mad",
	"gunner.mad",
	"infantry.mad",
	"insane.mad",
	"invulner.mad",
	"jacket.mad",
	"key.mad",
	"large.mad",
	"medic.mad",
	"medium.mad",
	"megah.mad",
	"mines.mad",
	"mutant.mad",
	"nuke.mad",
	"pack.mad",
	"pass.mad",
	"power.mad",
	"pyramid.mad",
	"quaddama.mad",
	"redkey.mad",
	"rockets.mad",
	"screen.mad",
	"shard.mad",
	"shells.mad",
	"shield.mad",
	"silenc.mad",
	"slugs.mad",
	"soldier.mad",
	"spinner.mad",
	"stim.mad",
	"target.mad",*/
};





void Test::FH_SkapaObject(void)
{
	int iAntalMdl = (sizeof(MdlNames) / 4)  ;

	akCoreModells.reserve(iAntalMdl+ 5);
	Core CoreAddBitch;
	Core* pCoreMdl;


	int i;
	for(i=0; i<iAntalMdl; i++)
	{
		akCoreModells.push_back(CoreAddBitch);
		pCoreMdl = &akCoreModells.back();
		pCoreMdl->Load(MdlNames[i]);
		pCoreMdl->SetTextureManger(pkTexMan);
	}
	
	MadInstans AddBitch;
	MadInstans* pMdl;

	float x,z;

	for(i=0; i<iAntalMdl; i++)
	{
		akModells.push_back(AddBitch);
		pMdl = &akModells.back();
		pCoreMdl = &akCoreModells[i];
		pMdl->SetBase(pCoreMdl);
		pMdl->PlayAnimation(0,0.0);

		x = 70 + (i % 10) * 2;
		z = 55 + (i / 10) * 2;
		pMdl->FH_Pos.Set(x,test->Height(x,z) + 1,z);
	}



	int anim = 0;
}

void Test::FH_UpdateraObject(void)
{
	vector<MadInstans>::iterator itMad;
	for(itMad=akModells.begin(); itMad != akModells.end(); itMad++)
	{
		itMad->UpdateAnimation();
	}
}

void Test::FH_RitaObject(void)
{
	vector<MadInstans>::iterator itMad;
	for(itMad=akModells.begin(); itMad != akModells.end(); itMad++)
	{
		glPushMatrix();
		glTranslatef(itMad->FH_Pos.x,itMad->FH_Pos.y,itMad->FH_Pos.z);
		glScalef(.02,.02,.02);
		itMad->Draw();
		glPopMatrix();
	}
}




