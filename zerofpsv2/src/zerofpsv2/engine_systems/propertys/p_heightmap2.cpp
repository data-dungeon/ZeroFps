#include "p_heightmap2.h"

P_Heightmap2::P_Heightmap2() // : m_kMap("HeightMap2")
{
	strcpy(m_acName,"P_Heightmap2");		

	m_pkRender	=	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkZeroFps	=	static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
	
	m_iType	=	PROPERTY_TYPE_RENDER;
	m_iSide	=	PROPERTY_SIDE_CLIENT;
	
	bNetwork	=	true;
	
	m_kMap.CreateHMFromImage("test.tga");	
}

void P_Heightmap2::Update()
{
	m_kMap.SetPos(m_pkObject->GetWorldPosV());
	m_pkRender->DrawHM2(&m_kMap,m_pkZeroFps->GetCam()->GetPos());
}

Property* Create_P_Heightmap2()
{
	return new P_Heightmap2();
}


