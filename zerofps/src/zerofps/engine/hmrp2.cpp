#include "hmrp2.h"

HMRP2::HMRP2(HeightMap* pkHeightMap) 
{
	strcpy(m_acName,"HMRP2");		

	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		


	SetHeightMap(pkHeightMap);
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
}

void HMRP2::SetHeightMap(HeightMap* pkHeightMap)
{
	m_pkHeightMap=pkHeightMap;	
	if(m_pkHeightMap!=NULL)
		m_pkHeightMap->SetPosition(m_pkObject->GetPos());	
}


void HMRP2::Update() 
{	
	if(m_pkHeightMap!=NULL){
		m_pkHeightMap->SetPosition(m_pkObject->GetPos());
		m_pkRender->DrawHMLodSplat(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),m_pkZeroFps->m_iFps);
	}
}

Property* Create_HMRP2()
{
	return new HMRP2(NULL);
}



