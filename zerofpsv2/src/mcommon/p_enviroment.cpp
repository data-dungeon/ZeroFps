#include "p_enviroment.h"


Property* Create_P_Enviroment()
{
	return new P_Enviroment;
}

P_Enviroment::P_Enviroment()
{
	strcpy(m_acName,"P_Enviroment");		
	m_iType=			PROPERTY_TYPE_RENDER;
	m_iSide=			PROPERTY_SIDE_CLIENT;
	m_iSortPlace=	-10;	
	bNetwork =		true;
	
	m_pkFps=					static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkEntityManager=	static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkAudioSystem =		static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkEnviroment =		static_cast<ZSSEnviroment*>(g_ZFObjSys.GetObjectPtr("ZSSEnviroment"));
	m_pkMLTime =			static_cast<ZSSMLTime*>(g_ZFObjSys.GetObjectPtr("ZSSMLTime"));
	
	m_bEnabled = 						false;	
	m_strCurrentZoneEnviroment =	"Default";
	m_strLastSetEnviroment=			"";	
	m_pkZoneEnvSetting = 			NULL;
	
	m_iMusicID =						-1;
	
	m_fTimeScale =						1;
	m_iCurrentSecond = 				0;
	
	if(!m_pkEnviroment)
	{
		cout<<"WARNING: no enviroment subsystem found in application, enviroments will be disabled"<<endl;	
	}
	
	if(!m_pkMLTime)
	{
		cout<<"WARNING: no mistlands time system found in application ( client dont need this )"<<endl;
	}
}

P_Enviroment::~P_Enviroment()
{
	if(m_pkEnviroment)
	{
		m_pkEnviroment->UnRegister(this);
	}
	
}


void P_Enviroment::Init()
{
	SetNetUpdateFlag(false);

	if(m_pkEnviroment)
	{
		m_pkEnviroment->Register(this);
	}

}

void P_Enviroment::Update()
{
	if(m_bEnabled)
	{
		DrawSky();
		UpdateEnviroment();
	
	
	}
}

void P_Enviroment::UpdateEnviroment()
{
	m_iCurrentSecond += m_pkZeroFps->GetFrameTime()*m_fTimeScale;

	float fIf = m_pkZeroFps->GetFrameTime()*0.3;
	
	if(fIf > 1.0)
		fIf = 1.0;


	//light
	static Vector4 kCurrentDiffuse(0,0,0,0);
	static Vector4 kCurrentAmbient(0,0,0,0);
	
	P_Light* pkLight;
	if( !(pkLight = (P_Light*)GetEntity()->GetProperty("P_Light")) )
		pkLight = (P_Light*)GetEntity()->AddProperty("P_Light");
	
	
	if(m_kCurrentEnvSetting.m_kSunDiffuseColor.x == -1)
	{
		float fT = float(m_iCurrentSecond) / 86400.0;
		float fA = 2*PI * fT;
		float fS = sin(fA - PI/2)/2.5 + 0.4;
				
		Vector4 kSunDColor(1,0.9,0.9,1);
		Vector4 kSunAColor(0.9,0.9,1,1);
		
		kSunDColor*=fS;
		kSunAColor*=fS * 0.5;
		
		kCurrentDiffuse.Lerp(kCurrentDiffuse,kSunDColor,fIf);
		kCurrentAmbient.Lerp(kCurrentAmbient,kSunAColor,fIf);
	}
	else
	{		
		kCurrentDiffuse.Lerp(kCurrentDiffuse,m_kCurrentEnvSetting.m_kSunDiffuseColor,fIf);
		kCurrentAmbient.Lerp(kCurrentAmbient,m_kCurrentEnvSetting.m_kSunAmbientColor,fIf);
	}
	
	pkLight->SetType(DIRECTIONAL_LIGHT);
	pkLight->SetDiffuse(kCurrentDiffuse);
	pkLight->SetAmbient(kCurrentAmbient);		
	pkLight->SetRot(m_kCurrentEnvSetting.m_kSunPos);	
			

	//particles
	//setup particle property
	static string strCurrentParticles;
	if(m_kCurrentEnvSetting.m_strParticles != "")
	{	
		if(m_kCurrentEnvSetting.m_strParticles != strCurrentParticles)
		{	
			strCurrentParticles = m_kCurrentEnvSetting.m_strParticles;
		
			P_PSystem* pkPS;
			if( !(pkPS = (P_PSystem*)GetEntity()->GetProperty("P_PSystem")) )
				pkPS = (P_PSystem*)GetEntity()->AddProperty("P_PSystem");
		
			pkPS->SetPSType(m_kCurrentEnvSetting.m_strParticles);			
		}		
	}
	else
	{
		strCurrentParticles = "";
		GetEntity()->DeleteProperty("P_PSystem");
	}
	
	//fog
	static Vector4 kCurrentFogColor(0,0,0,0);
	static float fCurrentStart = 0;
	static float fCurrentStop = 0;
	
	kCurrentFogColor.Lerp(kCurrentFogColor,m_kCurrentEnvSetting.m_kFogColor,fIf);	
	fCurrentStart = fCurrentStart*(1-fIf) + m_kCurrentEnvSetting.m_fFogStart*fIf;
	fCurrentStop =  fCurrentStop*(1-fIf) +  m_kCurrentEnvSetting.m_fFogStop*fIf;				
		
	m_pkZeroFps->GetCam()->SetClearColor(kCurrentFogColor);
	m_pkZeroFps->GetCam()->SetFog(kCurrentFogColor,fCurrentStart,fCurrentStop,true);			
	

	//music
		
	static string strCurrentMusic;
	if(m_kCurrentEnvSetting.m_strMusic != strCurrentMusic)
	{
		if(m_iMusicID != -1)
		{
			m_pkAudioSystem->StopAudio(m_iMusicID,true);
			m_iMusicID = -1;
			strCurrentMusic = "";
		}
	
		if(m_kCurrentEnvSetting.m_strMusic.length() != 0)
		{
			strCurrentMusic = m_kCurrentEnvSetting.m_strMusic;		
			m_iMusicID = m_pkAudioSystem->PlayAudio(m_kCurrentEnvSetting.m_strMusic, 
																Vector3(0,0,0), Vector3(0,0,1), ZFAUDIO_LOOP);
			
		}	
	}
	
/*	
	//sky box
	//setup skybox property
	static string strCurrentSpace;
	if(m_kCurrentEnvSetting.m_strSpace != "" )
	{
		if( strCurrentSpace != m_kCurrentEnvSetting.m_strSpace  )
		{
			strCurrentSpace = m_kCurrentEnvSetting.m_strSpace;
		
			P_SkyBoxRender* pkSB;
			if( !(pkSB = (P_SkyBoxRender*)m_pkEntity->GetProperty("P_SkyBoxRender") ) )		
				pkSB = (P_SkyBoxRender*)GetEntity()->AddProperty("P_SkyBoxRender");
			
			pkSB->SetTexture(m_kCurrentEnvSetting.m_strSpace.c_str(),m_kCurrentEnvSetting.m_strSpace.c_str());
		}
	}
	else
	{
		strCurrentSpace = "";
		GetEntity()->DeleteProperty("P_SkyBoxRender");
	}
*/		
}

void P_Enviroment::UpdateEnvSetting(EnvSetting* pkEnvSetting)
{
	m_pkZoneEnvSetting = pkEnvSetting;	
	
	
	//cout<<"got new enviroment setting"<<endl;
	if(m_pkZoneEnvSetting)
		SetNetUpdateFlag(true);
}

void P_Enviroment::ZoneChange(int iCurrent,int iNew)
{
	if(ZoneData* zd = m_pkEntityManager->GetZoneData(iNew))	
	{	
		//cout<<"current enviroment:"<<zd->m_strEnviroment<<endl;
		if(zd->m_strEnviroment == "" || zd->m_strEnviroment == "Default")
			return;
		
		m_strCurrentZoneEnviroment = zd->m_strEnviroment;
	}
}



void P_Enviroment::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	if(!m_pkZoneEnvSetting)
		cout<<"ERROR: tried to send unset zone"<<endl;

	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strMusic);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strParticles);
	
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSpace);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSky);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strCloudHi);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strCloudLow);

	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunDiffuseColor);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunAmbientColor);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunPos);	
		
	pkNetPacket->Write(m_pkZoneEnvSetting->m_fFogStart);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_fFogStop);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kFogColor);		
	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_iRain);			
	
	//pack time of day
	int iTotalSecond = m_pkMLTime->GetHour()*3600 + m_pkMLTime->GetMinute()*60 + m_pkMLTime->GetSecond();
	pkNetPacket->Write(iTotalSecond);	
	pkNetPacket->Write(m_pkMLTime->GetScale());	
	
	SetNetUpdateFlag(iConnectionID,false);
}

void P_Enviroment::PackFrom(NetPacket* pkNetPacket, int iConnectionID)
{

	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strMusic);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strParticles);

	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strSpace);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strSky);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strCloudHi);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strCloudLow);
	

	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunDiffuseColor);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunAmbientColor);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunPos);	
		
	pkNetPacket->Read(m_kCurrentEnvSetting.m_fFogStart);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_fFogStop);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kFogColor);		
	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_iRain);	
	
	//time
	pkNetPacket->Read(m_iCurrentSecond);
	pkNetPacket->Read(m_fTimeScale);
}

void P_Enviroment::SetEnable(bool bNew)
{
	if(bNew == m_bEnabled)
		return;

	m_bEnabled = bNew;

}



void P_Enviroment::DrawSky()
{
	static ZMaterial* pkSkyMat = NULL;
	if(!pkSkyMat)
	{
		pkSkyMat = new ZMaterial;
			pkSkyMat->GetPass(0)->m_kTUs[0]->SetRes("data/textures/enviroment/skyalpha.tga");
			pkSkyMat->GetPass(0)->m_kTUs[1]->SetRes("data/textures/enviroment/sky.tga");
			pkSkyMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;
			
			pkSkyMat->GetPass(0)->m_iPolygonModeFront =FILL_POLYGON;
			pkSkyMat->GetPass(0)->m_iCullFace = 		CULL_FACE_NONE;		
			pkSkyMat->GetPass(0)->m_bLighting = 		false;			
			pkSkyMat->GetPass(0)->m_bFog = 				false;		
			pkSkyMat->GetPass(0)->m_bDepthTest = 		false;		
			pkSkyMat->GetPass(0)->m_bAlphaTest = 		false;		

			pkSkyMat->GetPass(0)->m_bBlend = 			true;		
			pkSkyMat->GetPass(0)->m_iBlendSrc =			SRC_ALPHA_BLEND_SRC;
			pkSkyMat->GetPass(0)->m_iBlendDst =			ONE_MINUS_SRC_ALPHA_BLEND_DST;					
	}
	
	static ZMaterial* pkSpace = NULL;
	if(!pkSpace)
	{
		pkSpace = new ZMaterial;
			pkSpace->GetPass(0)->m_kTUs[0]->SetRes("data/textures/enviroment/skyalpha.tga");
			pkSpace->GetPass(0)->m_kTUs[1]->SetRes("data/textures/enviroment/space.tga");
			//pkSpace->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;
			
			pkSpace->GetPass(0)->m_iPolygonModeFront =FILL_POLYGON;
			pkSpace->GetPass(0)->m_iCullFace = 			CULL_FACE_NONE;		
			pkSpace->GetPass(0)->m_bLighting = 			false;			
			pkSpace->GetPass(0)->m_bFog = 				false;		
			pkSpace->GetPass(0)->m_bDepthTest = 		false;		
			pkSpace->GetPass(0)->m_bAlphaTest = 		false;		

			pkSpace->GetPass(0)->m_bBlend = 				true;		
			pkSpace->GetPass(0)->m_iBlendSrc =			SRC_ALPHA_BLEND_SRC;
			pkSpace->GetPass(0)->m_iBlendDst =			ONE_MINUS_SRC_ALPHA_BLEND_DST;					
	}

	static ZMaterial* pkCloudMatLow = NULL;
	if(!pkCloudMatLow)
	{
		pkCloudMatLow = new ZMaterial;
			pkCloudMatLow->GetPass(0)->m_kTUs[0]->SetRes("data/textures/enviroment/skyalpha.tga");
			pkCloudMatLow->GetPass(0)->m_kTUs[1]->SetRes("data/textures/enviroment/cloud.tga");
			pkCloudMatLow->GetPass(0)->m_iPolygonModeFront =FILL_POLYGON;
			pkCloudMatLow->GetPass(0)->m_iCullFace = 			CULL_FACE_NONE;		
			pkCloudMatLow->GetPass(0)->m_bLighting = 			false;			
			pkCloudMatLow->GetPass(0)->m_bFog = 				false;		
			pkCloudMatLow->GetPass(0)->m_bDepthTest = 		false;		
			pkCloudMatLow->GetPass(0)->m_bAlphaTest = 		false;	
				
			pkCloudMatLow->GetPass(0)->m_bBlend = 				true;		
			pkCloudMatLow->GetPass(0)->m_iBlendSrc =			SRC_ALPHA_BLEND_SRC;
			pkCloudMatLow->GetPass(0)->m_iBlendDst =			ONE_MINUS_SRC_ALPHA_BLEND_DST;				
	}
	
	static ZMaterial* pkCloudMatHi = NULL;
	if(!pkCloudMatHi)
	{
		pkCloudMatHi = new ZMaterial;
			pkCloudMatHi->GetPass(0)->m_kTUs[0]->SetRes("data/textures/enviroment/skyalpha.tga");
			pkCloudMatHi->GetPass(0)->m_kTUs[1]->SetRes("data/textures/enviroment/cloud2.tga");
			pkCloudMatHi->GetPass(0)->m_iPolygonModeFront =FILL_POLYGON;
			pkCloudMatHi->GetPass(0)->m_iCullFace = 		CULL_FACE_NONE;		
			pkCloudMatHi->GetPass(0)->m_bLighting = 		false;			
			pkCloudMatHi->GetPass(0)->m_bFog = 				false;		
			pkCloudMatHi->GetPass(0)->m_bDepthTest = 		false;		
			pkCloudMatHi->GetPass(0)->m_bAlphaTest = 		false;		
			
			pkCloudMatHi->GetPass(0)->m_bBlend = 			true;		
			pkCloudMatHi->GetPass(0)->m_iBlendSrc =		SRC_ALPHA_BLEND_SRC;
			pkCloudMatHi->GetPass(0)->m_iBlendDst =		ONE_MINUS_SRC_ALPHA_BLEND_DST;				
	}
			
	static float afVerts[] = {	-0.5,	0.0,	0.5,	
							 			-0.5,	0.0,	-0.5,	
							 			0.5,	0.0,	-0.5,	
										0.5,	0.0,	0.5};
	
	static float afUvs[] = {	0,1,
							 			1,1,
										1,0,
										0,0};	
	
	static float afSpaceUvs[] = {	0,5,
							 				5,5,
											5,0,
											0,0};	

											
	//UV's for moving clouds
	float fP = m_pkZeroFps->GetTicks()* 0.01;
	float fPS = m_pkZeroFps->GetTicks()*0.005;
	
	float afMUVsF[] = { fP,		1,
							  fP+1,	1,
							  fP+1,	0,
							  fP,		0};
	
	float afMUVsS[] = { fPS,	1,
							  fPS+1,	1,
							  fPS+1,	0,
							  fPS,	0};
										
	//setup textures
	pkSpace->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strSpace);
	pkSkyMat->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strSky);
	pkCloudMatLow->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strCloudLow);
	pkCloudMatHi->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strCloudHi);
						
	//draw sky planes	  
							  
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkZeroFps->GetCam()->GetRenderPos() + Vector3(0,5,0));										
	m_pkZShaderSystem->MatrixScale(100);
	
	m_pkZShaderSystem->ResetPointers();											
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,afVerts);
	m_pkZShaderSystem->SetNrOfVertexs(4);

	//space
	if(m_kCurrentEnvSetting.m_strSpace.length() != 0)
	{
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afSpaceUvs);
	
		m_pkZShaderSystem->BindMaterial(pkSpace);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}

	//sky
	if(m_kCurrentEnvSetting.m_strSky.length() != 0)
	{			
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);		
		m_pkZShaderSystem->BindMaterial(pkSkyMat);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}

	//cloud HI (moving faster
	if(m_kCurrentEnvSetting.m_strCloudHi.length() != 0)
	{	
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);						
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afMUVsS);	
		m_pkZShaderSystem->BindMaterial(pkCloudMatHi);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}
	
	//cloud LOW (moving slower
	if(m_kCurrentEnvSetting.m_strCloudLow.length() != 0)
	{	
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);								
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afMUVsF);	
		m_pkZShaderSystem->BindMaterial(pkCloudMatLow);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}	

	m_pkZShaderSystem->MatrixPop();
	
}





/*
	DrawSun();

	if(m_bEnabled) 
	{
			
		//interpolate light and fog 
		if(m_pkCurrentLP)
		{
			if(m_pkFps->GetTicks() - m_fTimer > 0.02)
			{
				m_fTimer = m_pkFps->GetTicks();
		
				Vector4 current;
				Vector4 interpolated;
				Vector3 cur;
				Vector3 intr;
			
				float fIf = 0.02;

							
				//interpolate diffuse light
				current=m_pkCurrentLP->GetDiffuse();
				interpolated.Lerp(current,m_kSunDiffuseColor,fIf);
				m_pkCurrentLP->SetDiffuse(interpolated);
							
				//interpolate ambient light
				current=m_pkCurrentLP->GetAmbient();
				interpolated.Lerp(current,m_kSunAmbientColor,fIf);
				m_pkCurrentLP->SetAmbient(interpolated);
			
				//interpolate lightpos
				cur=m_pkCurrentLP->GetRot();
				intr.Lerp(cur,m_kSunPos,fIf);
				m_pkCurrentLP->SetRot(intr);
			
				//setup fog
				current=m_pkCurrentLP->GetAmbient();
				interpolated.Lerp(m_kCurrentFogColor,m_kFogColor,fIf);
				m_kCurrentFogColor=interpolated;
				
				m_fCurrentFogStart = m_fCurrentFogStart*(1-fIf) + m_fFogStart*fIf;
				m_fCurrentFogStop = m_fCurrentFogStop*(1-fIf) + m_fFogStop*fIf;				
									
				
				m_pkCurrentLP->SetRot(m_kSunPos);
			}
			
			//set theses things per frame. else we get problems whit multiple cameras
			m_pkZeroFps->GetCam()->SetClearColor(m_kCurrentFogColor);
							
			if( (m_fFogStart < 0) )
				m_pkZeroFps->GetCam()->SetFog(m_kCurrentFogColor,m_fCurrentFogStart,m_fCurrentFogStop,false);
			else
				m_pkZeroFps->GetCam()->SetFog(m_kCurrentFogColor,m_fCurrentFogStart,m_fCurrentFogStop,true);			

		}
		
		//make rain splashes on ground
		if(m_iRain != 0)
		{
			//update rain splashes
			if(m_pkFps->GetTicks() - m_fRainUpdateTimer > 0.1)
			{		
				m_fRainUpdateTimer = m_pkFps->GetTicks();			
				MakeRainSplashes();
			}
	
			DrawRainSplashes();
		}
		
	}
*/	


/*
void P_Enviroment::MakeRainSplashes()
{
	m_kDrops.clear();

	vector<Entity*> kObjects;		
	if(ZoneData* pkZD = m_pkEntityManager->GetZone(m_pkEntity->GetWorldPosV()))
	{
		if(pkZD->m_pkZone)
		{
			vector<Entity*> m_kZones;				
		
			if(pkZD->m_pkZone->GetZoneNeighbours(&m_kZones))
			{
				for(unsigned int i = 0;i<m_kZones.size();i++)
				{
					m_kZones[i]->GetAllEntitys(&kObjects);
				}
				
			}
			else
				return;
		}
		else
			return;
	}
	else
		return;
	
	
	for(int j = 0;j < m_iRain;j++)
	{
		Vector3 kDropStart = m_pkEntity->GetWorldPosV() + Vector3( ( (rand()%2000)/100.0)-10.0,20,((rand()%2000)/100.0)-10.0);
		
		float fTop = -999999;
		Vector3 kPos;
		for(unsigned int i = 0;i < kObjects.size() ;i++)
		{
			if(P_Mad* pkMad = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
			{	
				if(pkMad->TestLine(kDropStart,Vector3(0,-1,0)))
				{	
					if(pkMad->GetLastColPos().y > fTop)
					{
						fTop = pkMad->GetLastColPos().y;
						kPos = pkMad->GetLastColPos();
					}
				
//					break;
				}
			}
		}
		
		if(fTop != -999999)
		{
			kPos.y += 0.1;
			m_kDrops.push_back(kPos);	
		}
		
	}
}
*/

/*
void P_Enviroment::DrawRainSplashes()
{
	static Vector3 v1(-0.1,0,0.1);
	static Vector3 v2( 0.1,0,0.1);
	static Vector3 v3( 0.1,0,-0.1);
	static Vector3 v4(-0.1,0,-0.1);

	for(unsigned int i = 0 ;i<m_kDrops.size();i++)
	{
		m_pkRender->Polygon4(m_kDrops[i] + v1,
									m_kDrops[i] + v2,
									m_kDrops[i] + v3,
									m_kDrops[i] + v4,
									m_iRainTextureID);
		//m_pkRender->Quad(m_kDrops[i],Vector3(-90,0,0),Vector3(0.3,0.3,0.3),m_iRainTextureID);
	}
}


void P_Enviroment::DrawSun()
{
	//textures
	static int iSunTex = m_pkEntity->m_pkZeroFps->m_pkTexMan->Load("data/textures/sun.tga", 0);
	static int iSunFlareTex = m_pkEntity->m_pkZeroFps->m_pkTexMan->Load("data/textures/sun.tga", 0);
	
	//max number of samples 
	static int iMaxSamples = 0.001*float(m_pkRender->GetWidth()*m_pkRender->GetHeight());
	static float fMaxAngle = 45.0;
	static float fFlareSize = 1.5;
	
	//positions for sun and flare
	Vector3 kSunPos =   m_pkZeroFps->GetCam()->GetRenderPos() + m_kSunPos * 100;
	Vector3 kFlarePos = m_pkZeroFps->GetCam()->GetRenderPos() + m_kSunPos;
	
	//calculate angle betwen sun and camera center
	Matrix4 kRot = m_pkZeroFps->GetCam()->GetRotM();
	kRot.Transponse();
	Vector3 kDir = kRot.VectorTransform(Vector3(0,0,-1));	
	float fAmp = 0;
	float fAngle = RadToDeg(kDir.Angle(m_kSunPos.Unit()));
	
	//check if  angle is lower than maxangle
	if( fAngle < fMaxAngle)
		fAmp = 1.0 - fAngle / fMaxAngle;
	
	
	if(m_pkZShaderSystem->SupportOcculusion())
	{
		//do occulusion test	
		m_pkZShaderSystem->OcculusionBegin();
		//m_pkRender->DrawBillboard(m_pkZeroFps->GetCam()->GetModelViewMatrix(),kSunPos,10,iSunTex);
		m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),kSunPos,10,m_pkSunMat);
		int iSamples = m_pkZShaderSystem->OcculusionEnd();
		
		float fSize = float(iSamples) / float(iMaxSamples);
		if(fSize > 1.0)
			fSize = 1.0;
			
		//draw flare
		//m_pkRender->DrawBillboard(m_pkZeroFps->GetCam()->GetModelViewMatrix(),kFlarePos,fFlareSize*fSize*fAmp,iSunFlareTex);
		m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),kFlarePos,fFlareSize*fSize*fAmp,m_pkSunFlareMat);
	}
	else
	{
		m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),kSunPos,10,m_pkSunMat);	
		m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),kFlarePos,fFlareSize*fAmp,m_pkSunFlareMat);
		//m_pkRender->DrawBillboard(m_pkZeroFps->GetCam()->GetModelViewMatrix(),kSunPos,10,iSunTex);
		//m_pkRender->DrawBillboard(m_pkZeroFps->GetCam()->GetModelViewMatrix(),kFlarePos,fFlareSize*fAmp,iSunFlareTex);	
	}
}
*/





/*
void P_Enviroment::SetEnviroment(const char* csEnviroment )
{
	//reset everything
	ResetEnviroment();

	//dont even try to load if theres no string
	if(strlen(csEnviroment) == 0)
		return;

	ZFResourceHandle* pkTempenv = new ZFResourceHandle;	

	//load enviroment settings
	if(!pkTempenv->SetRes(string(csEnviroment)))
	{	
		cout<<"FAILED TO LOAD "<<csEnviroment<<endl;
		delete pkTempenv;
		return;
	}

	//get enviroment pointer
	EnvSetting* es = (EnvSetting*)pkTempenv->GetResourcePtr();

	//set rain 
	m_iRain = es->m_iRain;

	//setup music
	//if(es->m_strMusic == "")
	//	m_pkMusic->Stop();
	//else
	//{
	//	m_pkMusic->LoadFile(es->m_strMusic.c_str());
	//	m_pkMusic->Play();
	//}
	if(es->m_strMusic != "")
		m_iMusicHandle = m_pkAudioSystem->PlayAudio(es->m_strMusic, 
			Vector3(0,0,0), Vector3(0,0,1), ZFAUDIO_LOOP);
	

	//setup skybox property
	if(es->m_strSkybox1 != "" && es->m_strSkybox2 != "")
	{
		P_SkyBoxRender* ps = (P_SkyBoxRender*)m_pkEntity->AddProperty("P_SkyBoxRender");	
		if(ps)
			ps->SetTexture(es->m_strSkybox1.c_str(),es->m_strSkybox2.c_str());
	}


	//setup particle property
	if(es->m_strParticles != "")
	{
		P_PSystem* ps = (P_PSystem*)m_pkEntity->AddProperty("P_PSystem");	
		if(ps)
			ps->SetPSType(es->m_strParticles);
	}

	//setup light property	
	P_Light* pl = (P_Light*)m_pkEntity->GetProperty("P_Light");		
	if(!pl)
		pl = (P_Light*)m_pkEntity->AddProperty("P_Light");	

	if(pl)
	{	
		m_pkCurrentLP = pl;
		m_kSunDiffuseColor=es->m_kSunDiffuseColor;
		m_kSunAmbientColor=es->m_kSunAmbientColor;
		m_kSunPos=es->m_kSunPos;
		
		pl->SetType(DIRECTIONAL_LIGHT);
	}

	m_kFogColor = es->m_kFogColor;
	m_fFogStart = es->m_fFogStart;
	m_fFogStop = es->m_fFogStop;
	
	//setup fog
	//m_pkRender->SetFog(es->m_kFogColor,es->m_fFogStart,es->m_fFogStop,true);	
	
	//setup clear color
	//m_pkRender->SetClearColor(es->m_kFogColor);	
	
	delete pkTempenv;

}

void P_Enviroment::ResetEnviroment()
{
	
	//m_pkRender->SetFog(Vector4(0,0,0,0),0,50,false);
	m_pkEntity->DeleteProperty("P_PSystem");
//	m_pkEntity->DeleteProperty("P_AmbientSound");
	m_pkEntity->DeleteProperty("P_SkyBoxRender");	
	
	m_pkCurrentLP = NULL;
	m_kSunDiffuseColor.Set(1,1,1,1);
	m_kSunAmbientColor.Set(0.5,0.5,0.5,1);
	m_kSunPos.Set(0.5,0.5,0);
	
	m_kFogColor.Set(0,0,0,1);	
	m_fFogStart=2;
	m_fFogStop=20;

}*/

	/*
	m_pkSunMat = new ZMaterial;
		m_pkSunMat->GetPass(0)->m_kTUs[0]->SetRes("data/textures/sun.tga");
		m_pkSunMat->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		m_pkSunMat->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		m_pkSunMat->GetPass(0)->m_bLighting = 				false;			
		m_pkSunMat->GetPass(0)->m_bFog = 					false;		
		m_pkSunMat->GetPass(0)->m_bDepthTest = 			true;		
		m_pkSunMat->GetPass(0)->m_bBlend = 					true;		
		m_pkSunMat->GetPass(0)->m_iBlendSrc =				SRC_ALPHA_BLEND_SRC;
		m_pkSunMat->GetPass(0)->m_iBlendDst =				ONE_BLEND_DST;

	m_pkSunFlareMat = new ZMaterial;
		m_pkSunFlareMat->GetPass(0)->m_kTUs[0]->SetRes("data/textures/sunflare.tga");
		m_pkSunFlareMat->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		m_pkSunFlareMat->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		m_pkSunFlareMat->GetPass(0)->m_bLighting = 				false;			
		m_pkSunFlareMat->GetPass(0)->m_bFog = 						false;		
		m_pkSunFlareMat->GetPass(0)->m_bDepthTest = 				true;		
		m_pkSunFlareMat->GetPass(0)->m_bBlend = 					true;		
		m_pkSunFlareMat->GetPass(0)->m_iBlendSrc =				SRC_ALPHA_BLEND_SRC;
		m_pkSunFlareMat->GetPass(0)->m_iBlendDst =				ONE_BLEND_DST;
	*/