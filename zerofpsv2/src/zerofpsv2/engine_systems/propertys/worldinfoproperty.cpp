#include "worldinfoproperty.h"
#include "../../engine/zerofps.h"
 
WorldInfoProperty::WorldInfoProperty()
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	strcpy(m_acName,"WorldInfoProperty");	

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	
	m_kSkyBoxHor = "";
	m_kSkyBoxTop = "";
	m_kSkyBoxRotate.Set(0,0,0);
	
	m_kFogColor.Set(0,0,0);
	m_fFogStart=50;
	m_fFogStop=200;
	
	m_bWater=0;
	
	m_kSunColor.Set(1,1,1);
	m_kMoonColor.Set(0.5,0.5,0.5);
	m_kAmbientColor.Set(0.2,0.2,0.2);
	
	
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
	
	//suncolor
	pkPackage->Write((void*)&m_kSunColor,12);			
	//mooncolor
	pkPackage->Write((void*)&m_kMoonColor,12);			
	//sun ambient color
	pkPackage->Write((void*)&m_kAmbientColor,12);				
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

	//suncolor
	pkPackage->Read((void*)&m_kSunColor,12);			
	//mooncolor
	pkPackage->Read((void*)&m_kMoonColor,12);			
	//sun ambient color
	pkPackage->Read((void*)&m_kAmbientColor,12);				

}

Property* Create_WorldInfoProperty()
{
	return new WorldInfoProperty();
}
