#include "containerproperty.h"

ContainerProperty::ContainerProperty()
{
	strcpy(m_acName,"ContainerProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bOpen = false;
}

ContainerProperty::~ContainerProperty()
{
	UnRegisterActions();
}


vector<PropertyValues> ContainerProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
		
	int iSizeX;
	int iSizeY;
	
	m_kContainer.GetSize(iSizeX,iSizeY);
		
	kReturn[0].kValueName="iSizeX";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&iSizeX;
	
	kReturn[1].kValueName="iSizeY";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&iSizeY;
	

	return kReturn;
}

bool ContainerProperty::HandleSetValue( string kValueName ,string kValue )
{
	int iSizeX;
	int iSizeY;	
	m_kContainer.GetSize(iSizeX,iSizeY);
	
	if(strcmp(kValueName.c_str(), "iSizeX") == 0) {
		iSizeX = atoi(kValue.c_str());
		m_kContainer.SetSize(iSizeX,iSizeY);
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "iSizeY") == 0) {
		iSizeY = atoi(kValue.c_str());
		m_kContainer.SetSize(iSizeX,iSizeY);
		return true;
	}

	return false;
}

void ContainerProperty::HandleGameMessage(GameMessage& Msg)
{
	if(Msg.m_Name == "OpenContainer")
		OpenContainer();
				
	if(Msg.m_Name == "Register_Actions")
		RegisterActions();
}


void ContainerProperty::Save(ZFMemPackage* pkPackage)
{
	int iSizeX;
	int iSizeY;	
	m_kContainer.GetSize(iSizeX,iSizeY);	

	pkPackage->Write((void*)&iSizeX,4);
	pkPackage->Write((void*)&iSizeY,4);

}

void ContainerProperty::Load(ZFMemPackage* pkPackage)
{
	int iSizeX;
	int iSizeY;	

	pkPackage->Read((void*)&iSizeX,4);
	pkPackage->Read((void*)&iSizeY,4);
	
	m_kContainer.SetSize(iSizeX,iSizeY);

}

void ContainerProperty::Init()
{
	RegisterActions();
}

void ContainerProperty::RegisterActions()
{
	ItemProperty* pkIP = static_cast<ItemProperty*>(m_pkObject->GetProperty("ItemProperty"));

	if(pkIP != NULL)
	{
		pkIP->RegisterAction(NORMALUSE,"Open container","OpenContainer");
	}
}	

void ContainerProperty::UnRegisterActions()
{
	ItemProperty* pkIP = static_cast<ItemProperty*>(m_pkObject->GetProperty("ItemProperty"));

	if(pkIP != NULL)
	{
		pkIP->UnRegisterAction(NORMALUSE,"Open container");
	}
}

Property* Create_ContainerProperty()
{
	return new ContainerProperty();
}

void ContainerProperty::OpenContainer()
{
	m_bOpen = true;
}
