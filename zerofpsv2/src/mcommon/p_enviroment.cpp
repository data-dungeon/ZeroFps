#include "p_enviroment.h"


//--------start of envsetting

EnvSetting::EnvSetting()
{
	Clear();

}

EnvSetting::~EnvSetting()
{

}

bool EnvSetting::LoadEnviroment(const char* czName)
{
	Clear();
	
	if(m_kIni.Open(czName,false))
	{
	
		//music
		if(m_kIni.KeyExist("enviroment","music"))
			m_strMusic = m_kIni.GetValue("enviroment","music");

		//---------------RAIN SPLASHES
		if(m_kIni.KeyExist("enviroment","rain"))
			m_iRain = m_kIni.GetIntValue("enviroment","rain");
		
			
		//---------------PARTICLES
		//particle effect
		if(m_kIni.KeyExist("enviroment","particles"))
			m_strParticles = m_kIni.GetValue("enviroment","particles");
	
		//--------------Skybox
		if(m_kIni.KeyExist("enviroment","skybox1"))
			m_strSkybox1 = m_kIni.GetValue("enviroment","skybox1");	
		if(m_kIni.KeyExist("enviroment","skybox2"))
			m_strSkybox2 = m_kIni.GetValue("enviroment","skybox2");	
	
		//---------------FOG
		//fog start/stop
		if(m_kIni.KeyExist("enviroment","fogstart"))
			m_fFogStart = m_kIni.GetFloatValue("enviroment","fogstart");
		if(m_kIni.KeyExist("enviroment","fogstop"))
			m_fFogStop = m_kIni.GetFloatValue("enviroment","fogstop");
		
		//fog color
		if(m_kIni.KeyExist("enviroment","fogcolorR"))
			m_kFogColor.x = m_kIni.GetFloatValue("enviroment","fogcolorR");
		if(m_kIni.KeyExist("enviroment","fogcolorG"))
			m_kFogColor.y = m_kIni.GetFloatValue("enviroment","fogcolorG");
		if(m_kIni.KeyExist("enviroment","fogcolorB"))
			m_kFogColor.z = m_kIni.GetFloatValue("enviroment","fogcolorB");		
		
		m_kFogColor.w = 1;
	
		//---------------SUN
		//diffuse sunlight
		if(m_kIni.KeyExist("enviroment","sundiffuseR"))
			m_kSunDiffuseColor.x = m_kIni.GetFloatValue("enviroment","sundiffuseR");
		if(m_kIni.KeyExist("enviroment","sundiffuseG"))
			m_kSunDiffuseColor.y = m_kIni.GetFloatValue("enviroment","sundiffuseG");
		if(m_kIni.KeyExist("enviroment","sundiffuseB"))
			m_kSunDiffuseColor.z = m_kIni.GetFloatValue("enviroment","sundiffuseB");		
		
		m_kSunDiffuseColor.w = 1;
		
		//ambient sunlight
		if(m_kIni.KeyExist("enviroment","sunambientR"))
			m_kSunAmbientColor.x = m_kIni.GetFloatValue("enviroment","sunambientR");
		if(m_kIni.KeyExist("enviroment","sunambientG"))
			m_kSunAmbientColor.y = m_kIni.GetFloatValue("enviroment","sunambientG");
		if(m_kIni.KeyExist("enviroment","sunambientB"))
			m_kSunAmbientColor.z = m_kIni.GetFloatValue("enviroment","sunambientB");
		
		m_kSunAmbientColor.w = 1;
		
		//sun pos/directon
		if(m_kIni.KeyExist("enviroment","sunposX"))
			m_kSunPos.x = m_kIni.GetFloatValue("enviroment","sunposX");
		if(m_kIni.KeyExist("enviroment","sunposY"))
			m_kSunPos.y = m_kIni.GetFloatValue("enviroment","sunposY");
		if(m_kIni.KeyExist("enviroment","sunposZ"))
			m_kSunPos.z = m_kIni.GetFloatValue("enviroment","sunposZ");
		
		
		m_kIni.Close();
		return true;
	}
	else
	{
		cout<<"Error loading enviroment:"<<czName<<endl;
		return false;		
	}
	
	return true;
}

void EnvSetting::Clear()
{
	m_strMusic		= "";
	m_strParticles = "";
	m_strSkybox1	= "";
	m_strSkybox2	= "";
	
	m_kSunDiffuseColor.Set(1.6,1.6,1.6,1);					
	m_kSunAmbientColor.Set(0.8,0.8,0.8,1.0);		
	m_kSunPos.Set(0.5,0.5,0);		
	m_iRain = 0;
	
	m_fFogStart = 2;
	m_fFogStop = 10;	
	m_kFogColor.Set(1,1,1,1);
}

bool EnvSetting::Create(string strName)
{
	return LoadEnviroment(strName.c_str());
}

int EnvSetting::CalculateSize()
{

	return sizeof(EnvSetting);
}


ZFResource* Create__EnvSetting()
{
	return new EnvSetting;						// LEAK - MistClient, Level loaded.
}

//---------end of envsetting

P_Enviroment::P_Enviroment()
{
	strcpy(m_acName,"P_Enviroment");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	m_pkMusic=static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));		

	m_iSortPlace	=	10;
	
	bNetwork =		true;
	m_bEnabled =	false;
	
	m_StrCurrentEnviroment = "";

}

void P_Enviroment::Init()
{
	ResetEnviroment();
	m_fTimer=m_pkFps->GetTicks();
	m_fRainUpdateTimer = m_pkFps->GetTicks();
	
	m_kCurrentFogColor.Set(0,0,0,1);	
	m_fCurrentFogStart=2;
	m_fCurrentFogStop=20;	
	
	m_iRain = 0;
	
	m_iRainTextureID = m_pkObject->m_pkZeroFps->m_pkTexMan->Load("data/textures/rainsplash.tga", 0);
}

void P_Enviroment::Update()
{
	if(m_bEnabled)
	{
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
				
				m_fCurrentFogStart = m_fCurrentFogStart*(1-fIf) + m_fFogStart*fIf;
				m_fCurrentFogStop = m_fCurrentFogStop*(1-fIf) + m_fFogStop*fIf;				
				
				if( (m_fFogStart < 0) )
				{
					m_pkRender->SetFog(interpolated,m_fCurrentFogStart,m_fCurrentFogStop,false);	
				}
				else
					m_pkRender->SetFog(interpolated,m_fCurrentFogStart,m_fCurrentFogStop,true);	
					
				m_pkRender->SetClearColor(interpolated);	
				
				m_kCurrentFogColor=interpolated;

			
				m_pkCurrentLP->SetRot(m_kSunPos);
			}
		}
		
	}
}

void P_Enviroment::ZoneChange(int iCurrent,int iNew)
{
//	cout<<"zonechange"<<endl;
	ZoneData* zd = m_pkObjectMan->GetZoneData(iNew);

	if(zd)	
	{	
		m_StrCurrentEnviroment = zd->m_strEnviroment;
		SetNetUpdateFlag(true);
	}
}


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
	if(es->m_strMusic == "")
	{
		m_pkMusic->Stop();
	}
	else
	{
		m_pkMusic->LoadFile(es->m_strMusic.c_str());
		m_pkMusic->Play();
	}

	//setup skybox property
	if(es->m_strSkybox1 != "" && es->m_strSkybox2 != "")
	{
		P_SkyBoxRender* ps = (P_SkyBoxRender*)m_pkObject->AddProperty("P_SkyBoxRender");	
		if(ps)
			ps->SetTexture(es->m_strSkybox1.c_str(),es->m_strSkybox2.c_str());
	}


	//setup particle property
	if(es->m_strParticles != "")
	{
		P_PSystem* ps = (P_PSystem*)m_pkObject->AddProperty("P_PSystem");	
		if(ps)
			ps->SetPSType(es->m_strParticles);
	}
	
	//setup light property	
	P_Light* pl = (P_Light*)m_pkObject->GetProperty("P_Light");		
	if(!pl)
		pl = (P_Light*)m_pkObject->AddProperty("P_Light");	
	
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
	m_pkObject->DeleteProperty("P_PSystem");
	m_pkObject->DeleteProperty("P_AmbientSound");
	m_pkObject->DeleteProperty("P_SkyBoxRender");	
	
	m_pkCurrentLP = NULL;
	m_kSunDiffuseColor.Set(1,1,1,1);
	m_kSunAmbientColor.Set(0.5,0.5,0.5,1);
	m_kSunPos.Set(0.5,0.5,0);
	
	m_kFogColor.Set(0,0,0,1);	
	m_fFogStart=2;
	m_fFogStop=20;

}

void P_Enviroment::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str(m_StrCurrentEnviroment.c_str());	
	
	if(m_StrCurrentEnviroment != "")
		SetNetUpdateFlag(iConnectionID,false);
}

void P_Enviroment::PackFrom(NetPacket* pkNetPacket, int iConnectionID)
{
	char temp[128];
	
	pkNetPacket->Read_Str(temp);
	
	if(m_StrCurrentEnviroment  == temp)
		return;
			
	m_StrCurrentEnviroment = temp;
	
	if(m_bEnabled)
		SetEnviroment(m_StrCurrentEnviroment.c_str());
	else
		SetEnviroment("");
		
		
}

void P_Enviroment::SetEnable(bool bNew)
{
	if(bNew == m_bEnabled)
		return;

	m_bEnabled = bNew;

	if(bNew)
	{
		SetEnviroment(m_StrCurrentEnviroment.c_str());
	}	
	else
	{
		SetEnviroment("");
	}
}

void P_Enviroment::MakeRainSplashes()
{
	m_kDrops.clear();

	vector<Entity*> kObjects;		
	if(ZoneData* pkZD = m_pkObjectMan->GetZone(m_pkObject->GetWorldPosV()))
	{
		if(pkZD->m_pkZone)
		{
			if(!pkZD->m_pkZone->GetZoneNeighbours(&kObjects))
			{
				cout<<"not attacthed to a zone"<<endl;
				return;
			}		
		}
		else
			return;
	}
	else
		return;
	
	
	for(int j = 0;j < m_iRain;j++)
	{
		Vector3 kDropStart = m_pkObject->GetWorldPosV() + Vector3( ( (rand()%2000)/100.0)-10.0,20,((rand()%2000)/100.0)-10.0);
		
		for(int i = 0;i < kObjects.size() ;i++)
		{
			if(P_Mad* pkMad = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
			{	
				if(pkMad->TestLine(kDropStart,Vector3(0,-1,0)))
				{	
					m_kDrops.push_back(pkMad->GetLastColPos()+Vector3(0,1,0));	
					break;
				}
			}
		}
	}
}

void P_Enviroment::DrawRainSplashes()
{
	for(int i = 0 ;i<m_kDrops.size();i++)
	{
		m_pkRender->Polygon4(m_kDrops[i] + Vector3(-0.1,0,0.1),
									m_kDrops[i] + Vector3( 0.1,0,0.1),
									m_kDrops[i] + Vector3( 0.1,0,-0.1),
									m_kDrops[i] + Vector3(-0.1,0,-0.1),
									m_iRainTextureID);
		//m_pkRender->Quad(m_kDrops[i],Vector3(-90,0,0),Vector3(0.3,0.3,0.3),m_iRainTextureID);
	}
}


Property* Create_P_Enviroment()
{
	return new P_Enviroment;
}