#include "skyboxrenderproperty.h"

SkyBoxRenderProperty::SkyBoxRenderProperty(const char* acHor,const char* acTop) 
{
	strcpy(m_acName,"SkyBoxRenderProperty");		
//	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		

	m_iType=PROPERTY_TYPE_STATIC;
	
	SetTexture(acHor,acTop);
}

void SkyBoxRenderProperty::SetTexture(const char* acHor,const char* acTop)
{
	m_iHor = m_pkTexMan->Load(acHor,T_CLAMP);
	m_iTop = m_pkTexMan->Load(acTop,T_CLAMP);
}


void SkyBoxRenderProperty::Update() 
{	
//	m_pkHeightMap->SetPosition(m_pkObject->GetPos());
	m_pkRender->DrawSkyBox(m_pkZeroFps->GetCam()->GetPos(),m_iHor,m_iTop);
//	m_pkRender->DrawHMlod(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),m_pkZeroFps->m_iFps);
}
