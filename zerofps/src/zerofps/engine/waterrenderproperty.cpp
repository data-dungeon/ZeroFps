#include "waterrenderproperty.h"

WaterRenderProperty::WaterRenderProperty()
{
	strcpy(m_acName,"WaterRenderProperty");		

//	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		

	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL ;
	m_iSide = PROPERTY_SIDE_CLIENT;
	
	SetProperty(100,10,"file:../data/textures/water2.bmp");
	m_iSortPlace	=	10;
	m_fBlendValue	=	0.7;
	m_bBlendDirUp	=  false;

	bNetwork	=	true;
}

void WaterRenderProperty::CloneOf(Property* pkProperty)
{
	bNetwork				= pkProperty->bNetwork;
	m_iType				= pkProperty->m_iType;
	m_iSide				= pkProperty->m_iSide;
	m_iSortPlace		= pkProperty->m_iSortPlace;
	m_bSortDistance	= pkProperty->m_bSortDistance;
}

void WaterRenderProperty::SetProperty(int iSize,int iStep,const char* acTexture)
{
	m_iSize=iSize;
	m_iStep=iStep;
	
	SetTexture(acTexture);
}

void WaterRenderProperty::SetTexture(const char* acTexture)
{
	m_iTexture = m_pkTexMan->Load(acTexture,0);
	m_sTexture = acTexture;
}

void WaterRenderProperty::Update() 
{	
	if(m_pkObject->m_pkObjectMan->m_iUpdateFlags & PROPERTY_TYPE_NORMAL &&
		m_pkObject->m_pkObjectMan->m_iUpdateFlags & PROPERTY_SIDE_CLIENT) {
		if(m_bBlendDirUp) {
			m_fBlendValue += 0.05;
			if(m_fBlendValue > 1.0)
				m_bBlendDirUp = false;
			}
		else {
			m_fBlendValue -= 0.05;
			if(m_fBlendValue < 0.0)
				m_bBlendDirUp = true;
			}
		}
	else {
		m_pkRender->DrawWater(m_pkZeroFps->GetCam()->GetPos(),m_pkObject->GetPos(),m_pkObject->GetRot(),m_iSize,m_iStep,m_iTexture,m_fBlendValue);
		}
}

void WaterRenderProperty::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write(m_iSize);
	pkNetPacket->Write(m_iStep);
}
 
void WaterRenderProperty::PackFrom(NetPacket* pkNetPacket)
{
	pkNetPacket->Read(m_iSize);
	pkNetPacket->Read(m_iStep);
}


void WaterRenderProperty::Save(ZFMemPackage* pkPackage)
{

	char data[256];	
	
	pkPackage->Write((void*)&m_iSize,sizeof(m_iSize));
	pkPackage->Write((void*)&m_iStep,sizeof(m_iStep));
	
	strcpy(data,m_sTexture.c_str());		
	pkPackage->Write((void*)&data,256);

}

void WaterRenderProperty::Load(ZFMemPackage* pkPackage)
{
	char data[300];	
	
	pkPackage->Read((void*)&m_iSize,sizeof(m_iSize));
	pkPackage->Read((void*)&m_iStep,sizeof(m_iStep));
	
	strcpy(data,m_sTexture.c_str());		
	pkPackage->Read((void*)&data,256);
	m_sTexture = data;
	
	SetTexture(m_sTexture.c_str());
}

vector<PropertyValues> WaterRenderProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);
			
	kReturn[0].kValueName="m_iSize";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&m_iSize;
	
	kReturn[1].kValueName="m_iStep";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&m_iStep;
	
	kReturn[2].kValueName="m_sTexture";
	kReturn[2].iValueType=VALUETYPE_STRING;
	kReturn[2].pkValue=(void*)&m_sTexture;

	return kReturn;
}

bool WaterRenderProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_sTexture") == 0) {
		SetTexture(kValue.c_str());		
		return true;
	}

	return false;
}



Property* Create_WaterRenderProperty()
{
	return new WaterRenderProperty;
}

