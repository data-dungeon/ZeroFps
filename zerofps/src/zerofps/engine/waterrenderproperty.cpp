#include "waterrenderproperty.h"

WaterRenderProperty::WaterRenderProperty(int iSize,int iStep,const char* acTexture)
{
	strcpy(m_acName,"WaterRenderProperty");		
//	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
//	m_kPos=kPos;
//	m_kHead=kHead;
	m_iSize=iSize;
	m_iStep=iStep;
	
	SetTexture(acTexture);
}

void WaterRenderProperty::SetTexture(const char* acTexture)
{
	m_iTexture = m_pkTexMan->Load(acTexture,0);
}

void WaterRenderProperty::Update() 
{	

//	m_pkRender->DrawWater(m_pkZeroFps->GetCam()->GetPos(),m_pkObject->GetPos(),Vector3(0,0,0),m_iSize,m_iStep,m_iTexture);	
//	m_pkRender->DrawWater(m_pkZeroFps->GetCam()->GetPos(),Vector3(512,0,512),Vector3(0,0,0),1200,50,m_pkTexMan->Load("file:../data/textures/water2.bmp",0));	
	m_pkRender->DrawWater(m_pkZeroFps->GetCam()->GetPos(),m_pkObject->GetPos(),m_pkObject->GetRot(),m_iSize,m_iStep,m_iTexture);
}



