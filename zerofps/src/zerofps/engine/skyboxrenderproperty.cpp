#include "skyboxrenderproperty.h"

SkyBoxRenderProperty::SkyBoxRenderProperty(const char* acHor,const char* acTop) 
{
	strcpy(m_acName,"SkyBoxRenderProperty");		
//	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_kRotateSpd.Set(0,0,0);
	m_kHead.Set(0,0,0);
	SetTexture(acHor,acTop);
}

void SkyBoxRenderProperty::SetTexture(const char* acHor,const char* acTop)
{
	m_iHor = m_pkTexMan->Load(acHor,T_CLAMP);
	m_iTop = m_pkTexMan->Load(acTop,T_CLAMP);
}

void SkyBoxRenderProperty::SetRotate(Vector3 kRotate)
{
	m_kRotateSpd = kRotate;
}


void SkyBoxRenderProperty::Update() 
{	
	m_kHead = m_kHead + m_kRotateSpd * m_pkZeroFps->GetFrameTime();
	
	m_pkRender->DrawSkyBox(m_pkZeroFps->GetCam()->GetPos(),m_kHead,m_iHor,m_iTop);
}

