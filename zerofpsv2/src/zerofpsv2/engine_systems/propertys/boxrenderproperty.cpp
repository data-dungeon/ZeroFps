#include "boxrenderproperty.h"
#include "../../basic/zfobjectmanger.h"
#include "../../engine/object.h"
#include "../../engine/zerofps.h"
 
BoxRenderProperty::BoxRenderProperty()
{
	strcpy(m_acName,"BoxRenderProperty");		

	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_kTex1="NONE";
	m_kScale.Set(1,1,1);
		
	UpdateSet();



}

void BoxRenderProperty::SetTexture(const char* acTex1)
{
	m_kTex1=acTex1;	
	m_iTex1=m_pkTexMan->Load(acTex1,0);
}

void BoxRenderProperty::UpdateSet()
{
	SetTexture(m_kTex1.c_str());
}

void BoxRenderProperty::Update()
{
	

	if(!m_pkFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetWorldPosV(),GetBoundingRadius()))
		return;
		
	m_pkRender->DrawBox(m_pkObject->GetWorldPosV(),m_pkObject->GetWorldRotV(),m_kScale,m_iTex1);//,m_iTex2);
}

vector<PropertyValues> BoxRenderProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "m_kTex1";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kTex1;

	kReturn[1].kValueName = "m_kScale";
	kReturn[1].iValueType = VALUETYPE_VECTOR3;
	kReturn[1].pkValue    = (void*)&m_kScale;

	return kReturn;
}

bool BoxRenderProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kTex1") == 0) {
		SetTexture(kValue.c_str());
		return true;
	}
	
/*
	if(strcmp(kValueName.c_str(), "m_kTex2") == 0) {
		SetTexture(m_kTex1.c_str(),kValue.c_str());
		return false;
	}
*/	

	return false;
}


void BoxRenderProperty::Save(ZFMemPackage* pkPackage)
{
	char temp[128];
	strcpy(temp,m_kTex1.c_str());	
	pkPackage->Write((void*)&temp,128);

	pkPackage->Write((void*)&m_kScale,12);

}

void BoxRenderProperty::Load(ZFMemPackage* pkPackage)
{
	char temp[128];
	
	pkPackage->Read((void*)&temp,128);
	m_kTex1=temp;

	pkPackage->Read((void*)&m_kScale,12);
	
	UpdateSet();
}


float BoxRenderProperty::GetBoundingRadius()
{
	if(m_kScale.x>m_kScale.y)
		if(m_kScale.x>m_kScale.z)
			return m_kScale.x;
		else
			return m_kScale.z;
	else
		if(m_kScale.y>m_kScale.z)
			return m_kScale.y;
		else
			return m_kScale.z;
}

Property* Create_BoxRenderProperty()
{
	return new BoxRenderProperty;

}


