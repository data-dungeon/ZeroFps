#include "p_enviroment.h"

P_Enviroment::P_Enviroment()
{
	strcpy(m_acName,"P_Enviroment");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));

	bNetwork = true;

	m_bEnabled = false;
	
	m_StrCurrentEnviroment = "Default";
}

void P_Enviroment::Update()
{
	
	
}

void P_Enviroment::ZoneChange(int iCurrent,int iNew)
{
	cout<<"Zone change:"<<endl;

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

	ResetEnviroment();

	if(m_StrCurrentEnviroment == "Rain" )
	{
		PSystemProperty* ps = (PSystemProperty*)m_pkObject->AddProperty("PSystemProperty");	
		if(ps)
			ps->SetPSType("rain");
			
			
		LightProperty* pl = (LightProperty*)m_pkObject->AddProperty("LightProperty");	
		if(pl)
		{	
			pl->SetType(DIRECTIONAL_LIGHT);
			pl->SetRot(Vector3(0.5,0.5,0));			
			pl->SetDiffuse(Vector4(.8,.8,.85,1));
		}
	}

	if(m_StrCurrentEnviroment == "Sun" || m_StrCurrentEnviroment == "Default")
	{
		LightProperty* pl = (LightProperty*)m_pkObject->AddProperty("LightProperty");	
		if(pl)
		{
			pl->SetType(DIRECTIONAL_LIGHT);
			pl->SetRot(Vector3(0.5,0.5,0));
			pl->SetDiffuse(Vector4(2,2,2,1));		
			pl->SetAmbient(Vector4(0.8,0.8,0.8,1.0));					
		}
	}



}

void P_Enviroment::ResetEnviroment()
{
	m_pkObject->DeleteProperty("PSystemProperty");
	m_pkObject->DeleteProperty("P_AmbientSound");
	m_pkObject->DeleteProperty("LightProperty");
	
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