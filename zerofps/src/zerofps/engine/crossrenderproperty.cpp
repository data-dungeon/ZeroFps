#include "crossrenderproperty.h"

CrossRenderProperty::CrossRenderProperty()
{
	strcpy(m_acName,"CrossRenderProperty");		

	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_kTex1="NONE";
//	m_kTex2="NONE";	
	
	UpdateSet();

	m_kScale.Set(1,1,1);

}

void CrossRenderProperty::SetTexture(const char* acTex1)//,const char* acTex2)
{
	m_kTex1=acTex1;	
	m_iTex1=m_pkTexMan->Load(acTex1,0);
//	m_iTex2=m_pkTexMan->Load(acTex2,0);
}

void CrossRenderProperty::UpdateSet()
{
	SetTexture(m_kTex1.c_str());//,m_kTex2.c_str());
}

void CrossRenderProperty::Update()
{
//	return;
	if(!m_pkFrustum->SphereInFrustum(m_pkObject->GetPos(),m_kScale.x))
		return;
		
	m_pkRender->DrawCross(m_pkObject->GetPos(),m_pkObject->GetRot(),m_kScale,m_iTex1);//,m_iTex2);
}

vector<PropertyValues> CrossRenderProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "m_kTex1";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kTex1;

/*
	kReturn[1].kValueName = "m_kTex2";
	kReturn[1].iValueType = VALUETYPE_STRING;
	kReturn[1].pkValue    = (void*)&m_kTex2;
*/

	kReturn[1].kValueName = "m_kScale";
	kReturn[1].iValueType = VALUETYPE_VECTOR3;
	kReturn[1].pkValue    = (void*)&m_kScale;

	return kReturn;
}

bool CrossRenderProperty::HandleSetValue( string kValueName ,string kValue )
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


void CrossRenderProperty::Save(ZFMemPackage* pkPackage)
{
	char temp[128];
	strcpy(temp,m_kTex1.c_str());	
	pkPackage->Write((void*)&temp,128);

//	strcpy(temp,m_kTex2.c_str());	
//	pkPackage->Write((void*)&temp,128);

	pkPackage->Write((void*)&m_kScale,12);

}

void CrossRenderProperty::Load(ZFMemPackage* pkPackage)
{
	char temp[128];
	
	pkPackage->Read((void*)&temp,128);
	m_kTex1=temp;

//	pkPackage->Read((void*)&temp,128);
//	m_kTex2=temp;

	pkPackage->Read((void*)&m_kScale,12);
	
	UpdateSet();
}


Property* Create_CrossRenderProperty()
{
	return new CrossRenderProperty;

}


