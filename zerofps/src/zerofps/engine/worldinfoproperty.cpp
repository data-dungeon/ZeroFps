#include "worldinfoproperty.h"
#include "zerofps.h"

WorldInfoProperty::WorldInfoProperty()
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	strcpy(m_acName,"WorldInfoProperty");	

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
}

void WorldInfoProperty::Update()
{
}

void WorldInfoProperty::Save(ZFMemPackage* pkPackage)
{
	//skybox
	char data[100];

	strcpy(data,m_kSkyBoxHor.c_str());
	pkPackage->Write((void*)&data,100);	
	
	strcpy(data,m_kSkyBoxTop.c_str());
	pkPackage->Write((void*)&data,100);
	
	pkPackage->Write((void*)&m_kSkyBoxRotate,12);	
	
	//fog
	pkPackage->Write((void*)&m_kFogColor,12);	
	pkPackage->Write((void*)&m_fFogStart,4);
	pkPackage->Write((void*)&m_fFogStop,4);
	
	//water
	pkPackage->Write((void*)&m_bWater,4);
	
}

void WorldInfoProperty::Load(ZFMemPackage* pkPackage)
{
	//skybox
	char data[100];
	
	pkPackage->Read((void*)&data,100);
	m_kSkyBoxHor=data;
	
	pkPackage->Read((void*)&data,100);
	m_kSkyBoxTop=data;	
	
	pkPackage->Read((void*)&m_kSkyBoxRotate,12);	
	
	//fog
	pkPackage->Read((void*)&m_kFogColor,12);
	pkPackage->Read((void*)&m_fFogStart,4);
	pkPackage->Read((void*)&m_fFogStop,4);
	
	//water
	pkPackage->Read((void*)&m_bWater,4);
}

Property* Create_WorldInfoProperty()
{
	return new WorldInfoProperty();
}
