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

		//---------------PARTICLES
		//particle effect
		if(m_kIni.KeyExist("enviroment","particles"))
			m_strParticles = m_kIni.GetValue("enviroment","particles");
	
	
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
	m_strParticles = "";
	
	m_kSunDiffuseColor.Set(1.6,1.6,1.6,1);					
	m_kSunAmbientColor.Set(0.8,0.8,0.8,1.0);		
	m_kSunPos.Set(0.5,0.5,0);		
	
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
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	

	bNetwork =		true;
	m_bEnabled =	false;
	
	m_StrCurrentEnviroment = "Default";

}

void P_Enviroment::Init()
{
	ResetEnviroment();
	m_fTimer=m_pkFps->GetTicks();
	
	m_kCurrentFogColor.Set(0,0,0,1);	
	m_fCurrentFogStart=2;
	m_fCurrentFogStop=20;	
}

void P_Enviroment::Update()
{
	if(m_bEnabled)
	{
		if(m_pkCurrentLP)
		{
			if(m_pkFps->GetTicks() - m_fTimer > 0.02)
			{
				m_fTimer = m_pkFps->GetTicks();
		
				Vector4 current;
				Vector4 interpolated;
				Vector3 cur;
				Vector3 intr;
			
				float fIf = 0.04;
			
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
	//cout<<"zonechange"<<endl;
	ZoneData* zd = m_pkObjectMan->GetZoneData(iNew);

	if(zd)	
	{
		m_StrCurrentEnviroment = zd->m_strEnviroment;
	}
}


void P_Enviroment::SetEnviroment(char* csEnviroment )
{
	if(m_StrCurrentEnviroment == csEnviroment)
		return;

	m_StrCurrentEnviroment = csEnviroment;

	//reset everything
	ResetEnviroment();

	ZFResourceHandle* pkTempenv = new ZFResourceHandle;	
	
	//load enviroment settings
	if(!pkTempenv->SetRes(string(csEnviroment)))
	{	
		cout<<"FAILED TO LOAD"<<csEnviroment<<endl;
		delete pkTempenv;
		return;
	}



	//get enviroment pointer
	EnvSetting* es = (EnvSetting*)pkTempenv->GetResourcePtr();


	//setup particle property
	if(es->m_strParticles != "")
	{
		PSystemProperty* ps = (PSystemProperty*)m_pkObject->AddProperty("PSystemProperty");	
		if(ps)
			ps->SetPSType(es->m_strParticles);
	}
	
	//setup light property
	
	LightProperty* pl = (LightProperty*)m_pkObject->GetProperty("LightProperty");		
	if(!pl)
		pl = (LightProperty*)m_pkObject->AddProperty("LightProperty");	
	
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
	m_pkObject->DeleteProperty("PSystemProperty");
	m_pkObject->DeleteProperty("P_AmbientSound");
	
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
	pkNetPacket->Write_NetStr(m_StrCurrentEnviroment.c_str());	
}

void P_Enviroment::PackFrom(NetPacket* pkNetPacket, int iConnectionID)
{
	char temp[128];
	
	pkNetPacket->Read_NetStr(temp);
	
	if(m_bEnabled)
		SetEnviroment(temp);
	else
		SetEnviroment("");
		
}

Property* Create_P_Enviroment()
{
	return new P_Enviroment;
}