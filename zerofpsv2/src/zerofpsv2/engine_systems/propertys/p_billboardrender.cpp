#include "p_billboardrender.h"
#include "../../engine/zerofps.h"
 
P_BillBoardRender::P_BillBoardRender()
{
	strcpy(m_acName,"P_BillBoardRender");		

	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_kTexture="NONE";
	m_fScale=1;	
	m_kAxis.Set(1,1,1);
	
	UpdateSet();



}

void P_BillBoardRender::SetTexture(const char* acTexure)//,const char* acTex2)
{
	m_kTexture=acTexure;	
	m_iTexture=m_pkTexMan->Load(acTexure,0);
}

void P_BillBoardRender::UpdateSet()
{
	SetTexture(m_kTexture.c_str());
}

void P_BillBoardRender::Update()
{
	if(!m_pkFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetWorldPosV(),m_fScale))
		return;
		
	Vector3 pos = m_pkObject->GetWorldPosV();
	m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),pos,m_fScale,m_iTexture);
}

vector<PropertyValues> P_BillBoardRender::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "m_kTexture";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kTexture;

	kReturn[1].kValueName = "m_fScale";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = (void*)&m_fScale;
	return kReturn;
}

bool P_BillBoardRender::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kTexture") == 0) {
		SetTexture(kValue.c_str());
		return true;
	}

	return false;
}


void P_BillBoardRender::Save(ZFIoInterface* pkPackage)
{
	char temp[128];
	strcpy(temp,m_kTexture.c_str());	
	pkPackage->Write((void*)&temp,128,1);

	pkPackage->Write((void*)&m_fScale,4,1);

}

void P_BillBoardRender::Load(ZFIoInterface* pkPackage)
{
	char temp[128];
	
	pkPackage->Read((void*)&temp,128,1);
	m_kTexture=temp;

	pkPackage->Read((void*)&m_fScale,4,1);
	
	UpdateSet();
}


Property* Create_BillBoardRenderProperty()
{
	return new P_BillBoardRender;

}


