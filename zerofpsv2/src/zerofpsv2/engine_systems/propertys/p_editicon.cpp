#include "p_editicon.h"
#include "../../engine/zerofps.h"
 
P_EditIcon::P_EditIcon()
{
	strcpy(m_acName,"P_EditIcon");		

	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_bSave = false;	
	m_bNetwork = false;
	
	m_kTexture="NONE";
	m_fScale=1;	
	m_kAxis.Set(1,1,1);
	
	UpdateSet();
}

void P_EditIcon::SetTexture(const char* acTexure)
{
	m_kTexture=acTexure;	
	m_iTexture=m_pkTexMan->Load(acTexure,0);
}

void P_EditIcon::UpdateSet()
{
	SetTexture(m_kTexture.c_str());
}

void P_EditIcon::Update()
{
	if(!m_pkFps->GetCam()->GetFrustum()->SphereInFrustum(m_pkEntity->GetWorldPosV(),m_fScale))
		return;
		
	Vector3 pos = m_pkEntity->GetIWorldPosV();

	int iIcon = m_pkEntity->m_ucIcon;
	switch(iIcon)
	{
		case 0:	return;	break;
		case 1:	SetTexture("/data/textures/editicon/camera.tga");	break;
		case 2:	SetTexture("/data/textures/editicon/light.tga");	break;
		case 3:	SetTexture("/data/textures/editicon/psystem.tga");	break;
		case 4:	SetTexture("/data/textures/editicon/water.tga");	break;
		default:
			SetTexture("/data/textures/editicon/noedit.tga");
	}

	m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),pos,m_fScale,m_iTexture, true,true,false);
}

vector<PropertyValues> P_EditIcon::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
 
	kReturn[0].kValueName = "texture";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kTexture;

	kReturn[1].kValueName = "scale";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = (void*)&m_fScale;
	return kReturn;
}

bool P_EditIcon::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "texture") == 0) 
	{
		SetTexture(kValue.c_str());
		return true;
	}

	return false;
}


void P_EditIcon::Save(ZFIoInterface* pkPackage)
{
	char temp[128];
	strcpy(temp,m_kTexture.c_str());	
	pkPackage->Write((void*)&temp,128,1);

	pkPackage->Write((void*)&m_fScale,4,1);
}

void P_EditIcon::Load(ZFIoInterface* pkPackage,int iVersion)
{
	char temp[128];
	strcpy(temp,m_kTexture.c_str());	
	pkPackage->Read((void*)&temp,128,1);
	m_kTexture = temp;
	
	pkPackage->Read((void*)&m_fScale,4,1);
}


Property* Create_EditIcon()
{
	return new P_EditIcon;

}


