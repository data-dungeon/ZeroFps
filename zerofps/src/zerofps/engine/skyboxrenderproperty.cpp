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
	
	m_iSortPlace=-100;
	
	m_kRotateSpd.Set(0,0,0);
	m_kHead.Set(0,0,0);
	SetTexture(acHor,acTop);
	
	m_bModeSixSides = false;
	m_iTexture[SKYBOXSIDE_FORWARD]	= -1;
	m_iTexture[SKYBOXSIDE_RIGHT]		= -1;
	m_iTexture[SKYBOXSIDE_BACK]		= -1;
	m_iTexture[SKYBOXSIDE_LEFT]		= -1;
	m_iTexture[SKYBOXSIDE_UP]			= -1;
	m_iTexture[SKYBOXSIDE_DOWN]		= -1;

}

void SkyBoxRenderProperty::SetTexture(const char* acHor,const char* acTop)
{
	string strTexName;
	string strBase(acHor);

	if(strcmp(acTop, "mode6") == 0) {
		m_bModeSixSides = true;
	
		strTexName = strBase + "_ft.bmp";
		m_iTexture[SKYBOXSIDE_FORWARD]	= m_pkTexMan->Load(strTexName.c_str(),T_CLAMP);
		strTexName = strBase + "_rt.bmp";
		m_iTexture[SKYBOXSIDE_RIGHT]	= m_pkTexMan->Load(strTexName.c_str(),T_CLAMP);
		strTexName = strBase + "_bk.bmp";
		m_iTexture[SKYBOXSIDE_BACK]	= m_pkTexMan->Load(strTexName.c_str(),T_CLAMP);
		strTexName = strBase + "_lf.bmp";
		m_iTexture[SKYBOXSIDE_LEFT]	= m_pkTexMan->Load(strTexName.c_str(),T_CLAMP);
		strTexName = strBase + "_up.bmp";
		m_iTexture[SKYBOXSIDE_UP]	= m_pkTexMan->Load(strTexName.c_str(),T_CLAMP);
		strTexName = strBase + "_dn.bmp";
		m_iTexture[SKYBOXSIDE_DOWN]	= m_pkTexMan->Load(strTexName.c_str(),T_CLAMP);
		}
	else {
		m_bModeSixSides = false;
		m_iHor = m_pkTexMan->Load(acHor,T_CLAMP);
		m_iTop = m_pkTexMan->Load(acTop,T_CLAMP);
		}

}

void SkyBoxRenderProperty::SetRotate(Vector3 kRotate)
{
	m_kRotateSpd = kRotate;
}


void SkyBoxRenderProperty::Update() 
{	
	
	m_kHead = m_kHead + m_kRotateSpd * m_pkZeroFps->GetFrameTime();
	
	if(m_bModeSixSides)
		m_pkRender->DrawSkyBox_SixSided(m_pkZeroFps->GetCam()->GetPos(),m_kHead, m_iTexture);
	else
		m_pkRender->DrawSkyBox(m_pkZeroFps->GetCam()->GetPos(),m_kHead,m_iHor,m_iTop);

}

Property* Create_SkyBoxRenderProperty()
{
	return new SkyBoxRenderProperty("NONE","NONE");
}
