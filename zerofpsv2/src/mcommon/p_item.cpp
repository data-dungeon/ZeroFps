#include "p_item.h"
#include "p_clientcontrol.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "p_charstats.h"


P_Item::P_Item()
{
	strcpy(m_acName,"P_Item");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = true;
	m_iVersion = 2;

	m_strName = "Unnamed Item";
	m_strIcon = "default.bmp";
	m_iSizeX = 1;	
	m_iSizeY = 1;
	m_iType = 0;

	m_iInContainerID = -1;
	m_iInContainerPosX = 0;
	m_iInContainerPosY = 0;
}

P_Item::~P_Item()
{

}

void P_Item::Init()
{


}

void P_Item::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strName);
	pkPackage->Write_Str(m_strIcon);
	
	pkPackage->Write(m_iSizeX);
	pkPackage->Write(m_iSizeY);	
	pkPackage->Write(m_iType);
	
	pkPackage->Write(m_iInContainerID);
	pkPackage->Write(m_iInContainerPosX);
	pkPackage->Write(m_iInContainerPosY);
}

void P_Item::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
		pkPackage->Read_Str(m_strName);
		pkPackage->Read_Str(m_strIcon);
		
		pkPackage->Read(m_iSizeX);
		pkPackage->Read(m_iSizeY);		
		pkPackage->Read(m_iType);
		
		pkPackage->Read(m_iInContainerID);		
		pkPackage->Read(m_iInContainerPosX);
		pkPackage->Read(m_iInContainerPosY);		
	}
}

vector<PropertyValues> P_Item::GetPropertyValues()
{
	vector<PropertyValues> kReturn(4);
	
		
	kReturn[0].kValueName = "name";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = &m_strName;		

	kReturn[1].kValueName = "sizex";
	kReturn[1].iValueType = VALUETYPE_INT;
	kReturn[1].pkValue    = &m_iSizeX;		

	kReturn[2].kValueName = "sizey";
	kReturn[2].iValueType = VALUETYPE_INT;
	kReturn[2].pkValue    = &m_iSizeY;		

	kReturn[3].kValueName = "icon";
	kReturn[3].iValueType = VALUETYPE_STRING;
	kReturn[3].pkValue    = &m_strIcon;			
	
	return kReturn;
}


Property* Create_P_Item()
{
	return new P_Item;
}

