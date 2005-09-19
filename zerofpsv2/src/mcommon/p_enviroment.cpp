#include "p_enviroment.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

const float MAX_VOL = 0.25f;

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
	m_bNetwork =		true;
	
	m_pkFps=					static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkEntityManager=	static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));	
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkAudioSystem =		static_cast<ZSSAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZSSAudioSystem"));
	m_pkEnviroment =		static_cast<ZSSEnviroment*>(g_ZFObjSys.GetObjectPtr("ZSSEnviroment"));
	m_pkMLTime =			static_cast<ZSSMLTime*>(g_ZFObjSys.GetObjectPtr("ZSSMLTime"));
	m_pkLight = 			static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
	
	m_bEnabled = 						false;	
	m_strCurrentZoneEnviroment =	"Default";
	m_strLastSetEnviroment=			"";	
	m_pkZoneEnvSetting = 			NULL;
		
	m_fTimeScale =						1;
	m_iCurrentSecond = 				0;
	
	m_iPartOfDay =						0;
	m_iNextPart =						1;
	m_fPosOfDay = 						0;
	m_fPosOfPart =						0;
	
	m_kThunderSounds.push_back("thunder/thunder1.wav");
	m_kThunderSounds.push_back("thunder/thunder2.wav");
	m_kThunderSounds.push_back("thunder/thunder3.wav");
	
	m_pkRainSplashMat = new ZMaterial;
		m_pkRainSplashMat->GetPass(0)->m_kTUs[0]->SetRes("rainsplash.tga");
		m_pkRainSplashMat->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		m_pkRainSplashMat->GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;		
		m_pkRainSplashMat->GetPass(0)->m_bLighting = 			true;			
		m_pkRainSplashMat->GetPass(0)->m_bFog = 					true;		
		m_pkRainSplashMat->GetPass(0)->m_bDepthTest = 			true;		
		m_pkRainSplashMat->GetPass(0)->m_bBlend = 				true;		
		m_pkRainSplashMat->GetPass(0)->m_iBlendSrc =				SRC_ALPHA_BLEND_SRC;
		m_pkRainSplashMat->GetPass(0)->m_iBlendDst =				ONE_BLEND_DST;

	m_pkSunMat = new ZMaterial;
		m_pkSunMat->GetPass(0)->m_kTUs[0]->SetRes("sun.tga");
		m_pkSunMat->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		m_pkSunMat->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		m_pkSunMat->GetPass(0)->m_bLighting = 				false;			
		m_pkSunMat->GetPass(0)->m_bFog = 					false;		
		m_pkSunMat->GetPass(0)->m_bDepthTest = 			true;		
		m_pkSunMat->GetPass(0)->m_bBlend = 					true;		
		m_pkSunMat->GetPass(0)->m_iBlendSrc =				SRC_ALPHA_BLEND_SRC;
		m_pkSunMat->GetPass(0)->m_iBlendDst =				ONE_BLEND_DST;

	m_pkSunFlareMat = new ZMaterial;
		m_pkSunFlareMat->GetPass(0)->m_kTUs[0]->SetRes("sunflare.tga");
		m_pkSunFlareMat->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		m_pkSunFlareMat->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		m_pkSunFlareMat->GetPass(0)->m_bLighting = 				false;			
		m_pkSunFlareMat->GetPass(0)->m_bFog = 						false;		
		m_pkSunFlareMat->GetPass(0)->m_bDepthTest = 				true;		
		m_pkSunFlareMat->GetPass(0)->m_bDepthMask = 				false;				
		m_pkSunFlareMat->GetPass(0)->m_bBlend = 					true;		
		m_pkSunFlareMat->GetPass(0)->m_iBlendSrc =				SRC_ALPHA_BLEND_SRC;
		m_pkSunFlareMat->GetPass(0)->m_iBlendDst =				ONE_BLEND_DST;	
	
		
		
	/*
	if(!m_pkEnviroment)
	{
		cout<<"WARNING: no enviroment subsystem found in application, enviroments will be disabled"<<endl;	
	}
	
	if(!m_pkMLTime)
	{
		cout<<"WARNING: no mistlands time system found in application ( client dont need this )"<<endl;
	}
	*/
}

P_Enviroment::~P_Enviroment()
{
	if(m_pkEnviroment)
	{
		m_pkEnviroment->UnRegister(this);	
	}
	
	if(m_bEnabled)
	{
		if(!m_strCurrentMusic.empty())
			m_pkAudioSystem->SetMusic("");
	}
}


void P_Enviroment::Init()
{
	SetNetUpdateFlag(false);

	if(m_pkEnviroment)
	{
		m_pkEnviroment->Register(this);
	}

	
	//check zone on load	
	ZoneChange(-1,m_pkEntity->GetCurrentZone());
}

void P_Enviroment::Update()
{
	if(m_bEnabled)
	{
		if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_NOSHADOWFIRST)
		{		
			UpdateTime();
			
			
			
			DrawSky();
			UpdateEnviroment();
			MakeThunder();
		}

		
		if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_NOSHADOWLAST)		
		{		
			if(m_kCurrentEnvSetting.m_iRain != 0)
			{
				MakeRainSplashes();
				DrawRainSplashes();
			}
		
			if(m_kCurrentEnvSetting.m_bSunFlare)
				DrawSun();
		}
	}
}

void P_Enviroment::UpdateTime()
{
	//update current time
	m_iCurrentSecond += int(m_pkZeroFps->GetFrameTime()*m_fTimeScale);
	if(m_iCurrentSecond > 86400)
		m_iCurrentSecond = 0;
	
		
	m_fPosOfDay = float(m_iCurrentSecond) / 86400.0;
	if(m_fPosOfDay >= 1)
		m_fPosOfDay = 0;
			
	//get current part and next part
	m_iPartOfDay = int(m_fPosOfDay / 0.25);
	m_iNextPart = m_iPartOfDay +1;
	if(m_iNextPart > 3)
		m_iNextPart = 0;

	//position within part 
	m_fPosOfPart = ((m_fPosOfDay / 0.25) - (m_iPartOfDay) );			


}

void P_Enviroment::UpdateEnviroment()
{
	float fIf = m_pkZeroFps->GetFrameTime()*0.3;
	
	if(fIf > 1.0)
		fIf = 1.0;


	//light
	static Vector4 kCurrentDiffuse(0,0,0,0);
	static Vector4 kCurrentAmbient(0,0,0,0);
	static Vector4 kCurrentSpecular(0,0,0,0);
	
// 	P_Light* pkLight;
// 	if( !(pkLight = (P_Light*)GetEntity()->GetProperty("P_Light")) )
// 		pkLight = (P_Light*)GetEntity()->AddProperty("P_Light");
	
	
	if(m_kCurrentEnvSetting.m_kSunDiffuseColor.x == -1)
	{
		float fA = 2*PI * m_fPosOfDay;
		float fS = sin(fA - PI/2)/2.5 + 0.4;
				
		Vector4 kSunDColor(1,0.9,0.9,1);
		Vector4 kSunAColor(0.9,0.9,1,1);
		Vector4 kSunSColor(1,1,1,1);
		
		kSunDColor*=fS;
		kSunSColor*=fS;
		kSunAColor*=fS * 0.5;
		
		kCurrentDiffuse.Lerp(kCurrentDiffuse,kSunDColor,fIf);
		kCurrentAmbient.Lerp(kCurrentAmbient,kSunAColor,fIf);
		kCurrentSpecular.Lerp(kCurrentSpecular,kSunSColor,fIf);
	}
	else
	{		
		kCurrentDiffuse.Lerp(kCurrentDiffuse,m_kCurrentEnvSetting.m_kSunDiffuseColor,fIf);
		kCurrentAmbient.Lerp(kCurrentAmbient,m_kCurrentEnvSetting.m_kSunAmbientColor,fIf);
		kCurrentSpecular.Lerp(kCurrentSpecular,m_kCurrentEnvSetting.m_kSunSpecularColor,fIf);
		
	}
	
	LightSource* pkSun = m_pkLight->GetSunPointer();

	pkSun->kSpecular = kCurrentSpecular;
	pkSun->kDiffuse = kCurrentDiffuse;
	pkSun->kAmbient = kCurrentAmbient;
	pkSun->kRot = m_kCurrentEnvSetting.m_kSunPos;

	//particles
	//setup particle property
	if(!m_kCurrentEnvSetting.m_strParticles.empty())
	{	
		if(m_kCurrentEnvSetting.m_strParticles != m_strCurrentPsystem)
		{	
			m_strCurrentPsystem = m_kCurrentEnvSetting.m_strParticles;
		
			P_PSystem* pkPS;
			if( !(pkPS = (P_PSystem*)GetEntity()->GetProperty("P_PSystem")) )
				pkPS = (P_PSystem*)GetEntity()->AddProperty("P_PSystem");
		
			pkPS->SetPSType(m_kCurrentEnvSetting.m_strParticles);			
		}		
	}
	else
	{
		m_strCurrentPsystem = "";
		GetEntity()->DeleteProperty("P_PSystem");
	}
	
	//fog
	static Vector4 kCurrentFogColor(0,0,0,0);
	static float fCurrentStart = 0;
	static float fCurrentStop = 0;
	
	
	
	if(m_kCurrentEnvSetting.m_kSunDiffuseColor.x == -1)
	{
		kCurrentFogColor.Lerp(m_kCurrentEnvSetting.m_kFogColor[m_iPartOfDay],
									m_kCurrentEnvSetting.m_kFogColor[m_iNextPart],
									m_fPosOfPart);		
	}
	else
	{
		kCurrentFogColor.Lerp(kCurrentFogColor,m_kCurrentEnvSetting.m_kFogColor[0],fIf);	
	}

	//interpolate start/stop
	fCurrentStart = fCurrentStart*(1-fIf) + m_kCurrentEnvSetting.m_fFogStart*fIf;
	fCurrentStop =  fCurrentStop*(1-fIf) +  m_kCurrentEnvSetting.m_fFogStop*fIf;					
	
		
	
	m_pkZeroFps->GetCam()->SetClearColor(kCurrentFogColor);
	m_pkZeroFps->GetCam()->SetFog(kCurrentFogColor,fCurrentStart,fCurrentStop,true);			
	

	//music
	if(m_kCurrentEnvSetting.m_strMusic != m_strCurrentMusic)
	{
		m_pkAudioSystem->SetMusic(m_kCurrentEnvSetting.m_strMusic);
		m_strCurrentMusic = m_kCurrentEnvSetting.m_strMusic;
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

void P_Enviroment::MakeThunder()
{
	if(!m_kCurrentEnvSetting.m_bThunder)
		return;

	static float fRate = 30;
	static float fDistance = 2.0;

	static float fNextFlash = m_pkZeroFps->GetEngineTime() + Randomf(fRate);;
	static float fNextThunder = -1;
	static Vector3 kNextPos = Vector3(Randomf(1)-0.5,1,Randomf(1)-0.5);



	if(fNextFlash != -1 && m_pkZeroFps->GetEngineTime() > fNextFlash)
	{
		float fDiff = m_pkZeroFps->GetEngineTime() - fNextFlash;
		if(fDiff > 0.5) 
		{
			fNextFlash = m_pkZeroFps->GetEngineTime() + Randomf(fRate);
			kNextPos = Vector3(Randomf(1)-0.5,1,Randomf(1)-0.5);
		}
		else
		{	
			fNextThunder = m_pkZeroFps->GetEngineTime() +1.0 + Randomf(fDistance);
		
			float fL = Randomf(0.5);
			float fFade = Max(1.0 - (fDiff / 0.5),0.0);
	
			LightSource* pkSun = m_pkLight->GetSunPointer();	
			pkSun->kSpecular = Vector4(0.5+fL,0.5+fL,0.5+fL,1) * fFade;
			pkSun->kDiffuse = Vector4(0.5+fL,0.5+fL,0.5+fL,1) * fFade;
			pkSun->kAmbient = pkSun->kAmbient + Vector4(0.25,0.25,0.25,1);
			pkSun->kRot = kNextPos;
		}
	}

	if(fNextThunder != -1 && m_pkZeroFps->GetEngineTime() > fNextThunder)
	{
		fNextThunder = -1;
		m_pkAudioSystem->PlayAudio(m_kThunderSounds[Randomi(m_kThunderSounds.size())],Vector3(0,0,0),Vector3(0,0,0),ZFAUDIO_2D,0.5);
	}
	

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
	{
		cout<<"ERROR: tried to send unset zone"<<endl;
		//return;
		m_pkZoneEnvSetting = &m_kCurrentEnvSetting;
	}

	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strMusic);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strParticles);
	
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSpace);
	
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSky[0]);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSky[1]);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSky[2]);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strSky[3]);	
	
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strCloudHi);
	pkNetPacket->Write_Str(m_pkZoneEnvSetting->m_strCloudLow);

	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunSpecularColor);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunDiffuseColor);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunAmbientColor);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kSunPos);	
	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_bSunFlare);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_bThunder);		
		
	pkNetPacket->Write(m_pkZoneEnvSetting->m_fFogStart);	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_fFogStop);	
	
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kFogColor[0]);		
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kFogColor[1]);		
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kFogColor[2]);		
	pkNetPacket->Write(m_pkZoneEnvSetting->m_kFogColor[3]);		
	
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
	
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strSky[0]);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strSky[1]);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strSky[2]);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strSky[3]);
	
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strCloudHi);
	pkNetPacket->Read_Str(m_kCurrentEnvSetting.m_strCloudLow);
	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunSpecularColor);
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunDiffuseColor);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunAmbientColor);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kSunPos);	
		
	pkNetPacket->Read(m_kCurrentEnvSetting.m_bSunFlare);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_bThunder);	
	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_fFogStart);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_fFogStop);	
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kFogColor[0]);		
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kFogColor[1]);		
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kFogColor[2]);		
	pkNetPacket->Read(m_kCurrentEnvSetting.m_kFogColor[3]);		
	
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

void P_Enviroment::LoadEnviroment(const char* czEnv)
{
	m_kCurrentEnvSetting.LoadEnviroment(czEnv);
}



void P_Enviroment::DrawSky()
{
	bool usd = m_pkZShaderSystem->GetUseDefaultGLSLProgram();
	m_pkZShaderSystem->UseDefaultGLSLProgram(false);
	
	// --- SKY's
	static ZMaterial* pkSkyBackMat = NULL;
	if(!pkSkyBackMat)
	{
		pkSkyBackMat = new ZMaterial;
			pkSkyBackMat->GetPass(0)->m_kTUs[0]->SetRes("enviroment/skyalpha.tga");
			pkSkyBackMat->GetPass(0)->m_kTUs[1]->SetRes("enviroment/sky.tga");
			pkSkyBackMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;
			
			pkSkyBackMat->GetPass(0)->m_pkSLP->SetRes("#hdrsky.frag.glsl");
			
			pkSkyBackMat->GetPass(0)->m_iPolygonModeFront =	FILL_POLYGON;
			pkSkyBackMat->GetPass(0)->m_iCullFace = 			CULL_FACE_NONE;		
			pkSkyBackMat->GetPass(0)->m_bLighting = 			false;			
			pkSkyBackMat->GetPass(0)->m_bFog = 					false;		
			pkSkyBackMat->GetPass(0)->m_bDepthTest = 			false;		
			pkSkyBackMat->GetPass(0)->m_bAlphaTest = 			false;		

			pkSkyBackMat->GetPass(0)->m_bBlend = 			true;		
			pkSkyBackMat->GetPass(0)->m_iBlendSrc =		SRC_ALPHA_BLEND_SRC;
			pkSkyBackMat->GetPass(0)->m_iBlendDst =		ONE_MINUS_SRC_ALPHA_BLEND_DST;					
	}
	
	static ZMaterial* pkSkyFrontMat = NULL;
	if(!pkSkyFrontMat)
	{
		pkSkyFrontMat = new ZMaterial;
			pkSkyFrontMat->GetPass(0)->m_kTUs[0]->SetRes("enviroment/skyalpha.tga");
			pkSkyFrontMat->GetPass(0)->m_kTUs[1]->SetRes("enviroment/space.tga");
			pkSkyFrontMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;
			
			pkSkyFrontMat->GetPass(0)->m_iPolygonModeFront =FILL_POLYGON;
			pkSkyFrontMat->GetPass(0)->m_iCullFace = 			CULL_FACE_NONE;		
			pkSkyFrontMat->GetPass(0)->m_bLighting = 			false;			
			pkSkyFrontMat->GetPass(0)->m_bFog = 				false;		
			pkSkyFrontMat->GetPass(0)->m_bDepthTest = 		false;		
			pkSkyFrontMat->GetPass(0)->m_bAlphaTest = 		false;		

			pkSkyFrontMat->GetPass(0)->m_bBlend = 				true;		
			pkSkyFrontMat->GetPass(0)->m_iBlendSrc =			SRC_ALPHA_BLEND_SRC;
			pkSkyFrontMat->GetPass(0)->m_iBlendDst =			ONE_MINUS_SRC_ALPHA_BLEND_DST;					
	
			pkSkyFrontMat->GetPass(0)->m_bColorMaterial = 	true;
			pkSkyFrontMat->GetPass(0)->m_kVertexColor= 		Vector4(1,1,1,0.5);	
	}	
	// --- sky
	
	static ZMaterial* pkSpace = NULL;
	if(!pkSpace)
	{
		pkSpace = new ZMaterial;
			pkSpace->GetPass(0)->m_kTUs[0]->SetRes("enviroment/skyalpha.tga");
			pkSpace->GetPass(0)->m_kTUs[1]->SetRes("enviroment/space.tga");
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
			pkCloudMatLow->GetPass(0)->m_kTUs[0]->SetRes("enviroment/skyalpha.tga");
			pkCloudMatLow->GetPass(0)->m_kTUs[1]->SetRes("enviroment/cloud.tga");
			pkCloudMatLow->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;
 			pkCloudMatLow->GetPass(0)->m_pkSLP->SetRes("#hdrsky.frag.glsl");
			
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
			pkCloudMatHi->GetPass(0)->m_kTUs[0]->SetRes("enviroment/skyalpha.tga");
			pkCloudMatHi->GetPass(0)->m_kTUs[1]->SetRes("enviroment/cloud2.tga");
			pkCloudMatHi->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;
			pkCloudMatHi->GetPass(0)->m_pkSLP->SetRes("#hdrsky.frag.glsl");
			
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
	float fP = m_pkZeroFps->GetEngineTime()* 0.01;
	float fPS = m_pkZeroFps->GetEngineTime()*0.001;
	
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
//	pkSkyMat->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strSky[0]);
	
	pkCloudMatLow->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strCloudLow);
	pkCloudMatHi->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strCloudHi);

	pkCloudMatLow->GetPass(0)->m_kVertexColor = m_kCurrentEnvSetting.m_kSunDiffuseColor;						
	pkCloudMatHi->GetPass(0)->m_kVertexColor = m_kCurrentEnvSetting.m_kSunDiffuseColor;						


	//draw sky planes	  							  
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkZeroFps->GetCam()->GetRenderPos() + Vector3(0,5,0));										
	m_pkZShaderSystem->MatrixScale(100);
	
	m_pkZShaderSystem->ResetPointers();											
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,afVerts);
	m_pkZShaderSystem->SetNrOfVertexs(4);

	
	
	static string strFront;
	static string strBack;	
	static int iCurrent = 0;
	
	
	if(m_kCurrentEnvSetting.m_kSunDiffuseColor.x == -1)
	{
		if(!m_kCurrentEnvSetting.m_strSky[0].empty())
		{													
			
			pkSkyBackMat->GetPass(0)->m_kTUs[1]->SetRes(		m_kCurrentEnvSetting.m_strSky[m_iPartOfDay]	);			
			pkSkyFrontMat->GetPass(0)->m_kTUs[1]->SetRes(	m_kCurrentEnvSetting.m_strSky[m_iNextPart]		);	
			pkSkyFrontMat->GetPass(0)->m_kVertexColor= Vector4(1,1,1,m_fPosOfPart);	
												
			m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);				
			m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afSpaceUvs);			
			
			
			//back			
			if(m_iPartOfDay == 0)
				pkSkyBackMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;
			else
				pkSkyBackMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;
			
			m_pkZShaderSystem->BindMaterial(pkSkyBackMat);															
			m_pkZShaderSystem->DrawArray(QUADS_MODE);				

			//front
			if(m_iNextPart == 0)
				pkSkyFrontMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;
			else
				pkSkyFrontMat->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;
				
			m_pkZShaderSystem->BindMaterial(pkSkyFrontMat);															
			m_pkZShaderSystem->DrawArray(QUADS_MODE);				
			
					
		}
	}
	else
	{
		//sky
		if(!m_kCurrentEnvSetting.m_strSky[0].empty())
		{			
			pkSkyBackMat->GetPass(0)->m_kTUs[1]->SetRes(m_kCurrentEnvSetting.m_strSky[0]);
			pkSkyBackMat->GetPass(0)->m_kVertexColor = m_kCurrentEnvSetting.m_kSunDiffuseColor;
		
			m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);				
			m_pkZShaderSystem->BindMaterial(pkSkyBackMat);												
			m_pkZShaderSystem->DrawArray(QUADS_MODE);	
		}		
	}
	
	
	
/*	//space
	if(!m_kCurrentEnvSetting.m_strSpace.empty())
	{
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afSpaceUvs);
	
		m_pkZShaderSystem->BindMaterial(pkSpace);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}

	//sky
	if(!m_kCurrentEnvSetting.m_strSky[0].empty())
	{			
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);				
		m_pkZShaderSystem->BindMaterial(pkSkyMat);												
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}
	*/
	//cloud HI (moving slower
	if(!m_kCurrentEnvSetting.m_strCloudHi.empty())
	{	
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);						
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afMUVsS);	
		m_pkZShaderSystem->BindMaterial(pkCloudMatHi);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}
	
	//cloud LOW (moving faster
	if(!m_kCurrentEnvSetting.m_strCloudLow.empty())
	{	
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);								
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,afMUVsF);	
		m_pkZShaderSystem->BindMaterial(pkCloudMatLow);											
		m_pkZShaderSystem->DrawArray(QUADS_MODE);	
	}	


	
	m_pkZShaderSystem->MatrixPop();
	
	
	m_pkZShaderSystem->UseDefaultGLSLProgram(usd);
}





void P_Enviroment::DrawSun()
{
	
	//max number of samples 
	static int iMaxSamples = int(0.001*float(m_pkRender->GetWidth()*m_pkRender->GetHeight()));
	static float fMaxAngle = 30.0;
	static float fFlareSize = 0.6;
	
	//positions for sun and flare
	Vector3 kSunPos =   m_pkZeroFps->GetCam()->GetRenderPos() + m_kCurrentEnvSetting.m_kSunPos * 100;
	Vector3 kFlarePos = m_pkZeroFps->GetCam()->GetRenderPos() + m_kCurrentEnvSetting.m_kSunPos * 0.5;
	
	//calculate angle betwen sun and camera center
	Matrix4 kRot = m_pkZeroFps->GetCam()->GetRotM();
	kRot.Transponse();
	Vector3 kDir = kRot.VectorTransform(Vector3(0,0,-1));	
	float fAmp = 0;
	float fAngle = RadToDeg(kDir.Angle(m_kCurrentEnvSetting.m_kSunPos.Unit()));
	
	//check if  angle is lower than maxangle
	if( fAngle < fMaxAngle)
	{
		fAmp = 1.0 - fAngle / fMaxAngle;
	
		fAmp *= fAmp;
	}
	
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


void P_Enviroment::MakeRainSplashes()
{
// 	static float fTime = 0;
// 	if(m_pkZeroFps->GetEngineTime() > fTime +0.1)
// 	{
// 		fTime = m_pkZeroFps->GetEngineTime();


		vector<Entity*> kObjects;		
		m_pkEntityManager->GetAllEntitys(&kObjects);
				
		static Vector3 kCenterPos,kDropStart,kPos;
		kCenterPos = m_pkEntity->GetWorldPosV(); 

							
		static int iLastAmount = 0;
		if(iLastAmount !=  m_kCurrentEnvSetting.m_iRain)
		{
			m_kDrops.clear();
			
			for(int j = 0;j < m_kCurrentEnvSetting.m_iRain;j++)
			{
				kDropStart = kCenterPos + Vector3(  Randomf(12)-6.0,20,Randomf(12)-6.0);
				
				float fTop = -999999;
				for(unsigned int i = 0;i < kObjects.size() ;i++)
				{
					if(kObjects[i]->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV()) > 20)
						continue;
				
					if(P_Mad* pkMad = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
					{	
						if(pkMad->TestLine(kDropStart,Vector3(0,-1,0)))
						{	
							if(pkMad->GetLastColPos().y > fTop)
							{
								fTop = pkMad->GetLastColPos().y;
								kPos = pkMad->GetLastColPos();
							}
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
		else
		{
			int iAmount = 10;
			for(int j = 0;j < iAmount;j++)
			{
			
				kDropStart = kCenterPos + Vector3(  Randomf(12)-6.0,20,Randomf(12)-6.0);
				
				float fTop = -999999;
				for(unsigned int i = 0;i < kObjects.size() ;i++)
				{
					if(kObjects[i]->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV()) > 20)
						continue;
				
					if(P_Mad* pkMad = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
					{	
						if(pkMad->TestLine(kDropStart,Vector3(0,-1,0)))
						{	
							if(pkMad->GetLastColPos().y > fTop)
							{
								fTop = pkMad->GetLastColPos().y;
								kPos = pkMad->GetLastColPos();
							}
						}
					}
				}
				
				if(fTop != -999999)
				{
					
					kPos.y += 0.1;
					
					
					m_kDrops[Randomi(m_kDrops.size())] = kPos;

				}		
			}		
		}
		
		iLastAmount =  m_kCurrentEnvSetting.m_iRain;
// 	}
}



void P_Enviroment::DrawRainSplashes()
{
	static Vector3 v1(-0.1,0,0.1);
	static Vector3 v2( 0.1,0,0.1);
	static Vector3 v3( 0.1,0,-0.1);
	static Vector3 v4(-0.1,0,-0.1);

	
// 	cout<<"drops "<<m_kDrops.size()<<endl;
	
	m_pkZShaderSystem->BindMaterial(m_pkRainSplashMat);

	m_pkZShaderSystem->SetNormal(Vector3(0,1,0));
	m_pkZShaderSystem->ClearGeometry();
		
	for(unsigned int i = 0 ;i<m_kDrops.size();i++)
	{
		m_pkZShaderSystem->AddQuadV(	m_kDrops[i] + v1,
 												m_kDrops[i] + v2,
 												m_kDrops[i] + v3,
 												m_kDrops[i] + v4);
		m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(0,1));
	}
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);
	
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





