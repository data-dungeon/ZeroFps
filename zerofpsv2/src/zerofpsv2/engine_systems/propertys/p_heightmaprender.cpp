#include "p_heightmaprender.h"
#include "../../engine/zerofps.h"

P_HeightMapRender::P_HeightMapRender(HeightMap* pkHeightMap) 
{
	strcpy(m_acName,"P_HeightMapRender");		

	m_pkTexMan	=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps	=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender	=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		


	SetHeightMap(pkHeightMap);
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
}

void P_HeightMapRender::SetHeightMap(HeightMap* pkHeightMap)
{
	m_pkHeightMap=pkHeightMap;	
	if(m_pkHeightMap!=NULL)
		m_pkHeightMap->SetPosition(m_pkObject->GetWorldPosV());	
}


void P_HeightMapRender::Update() 
{	
	if(m_pkHeightMap!=NULL){
		m_pkHeightMap->SetPosition(m_pkObject->GetWorldPosV());
//		m_pkRender->DrawHMlod(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),m_pkZeroFps->m_iFps);
	}
}

Property* Create_HeightMapRenderProperty()
{
	return new P_HeightMapRender(NULL);
}



