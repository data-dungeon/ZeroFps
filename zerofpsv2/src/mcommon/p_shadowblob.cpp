#include "p_shadowblob.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_ShadowBlob::P_ShadowBlob()
{
	strcpy(m_acName,"P_ShadowBlob");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
//	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkEntityMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	bNetwork = true;
	m_iSortPlace	=	11;
	
	
	m_pkTexture = new ZFResourceHandle;
	m_kOffset.Set(0,0.01,0);
	m_kScale.Set(1,1,1);
	m_bHaveSet = false;
}

P_ShadowBlob::~P_ShadowBlob()
{
	delete m_pkTexture;

}

void P_ShadowBlob::Update()
{
	if(!m_bHaveSet)
	{
		m_bHaveSet = true;
		
		if(P_Mad* pkMad = (P_Mad*)m_pkObject->GetProperty("P_Mad"))
		{
			m_kScale.Set(pkMad->GetRadius(),pkMad->GetRadius(),pkMad->GetRadius());
			//m_kOffset.Set(0,-pkMad->GetRadius(),0);			
		}
	}


	if(ResTexture* pkRt = (ResTexture*)m_pkTexture->GetResourcePtr())
	{
		int iTexture = (pkRt)->m_iTextureID;
		
		glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);		
		glBlendFunc(ONE_MINUS_SRC_ALPHA_BLEND_SRC,ONE_BLEND_DST);
		glDepthMask(GL_FALSE);
		
		m_pkRender->Quad(m_pkObject->GetIWorldPosV()+m_kOffset,Vector3(-90,0,0),m_kScale,iTexture);
		
		glPopAttrib();
	}
}

void P_ShadowBlob::Init()
{
	
	m_pkTexture->SetRes("data/textures/shadowblob.bmp");

}


vector<PropertyValues> P_ShadowBlob::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_ShadowBlob()
{
	return new P_ShadowBlob;
}








