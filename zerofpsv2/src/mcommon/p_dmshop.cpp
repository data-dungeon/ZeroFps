#include "p_dmshop.h" 
#include "si_dm.h"
#include "p_dmgameinfo.h"
#include "p_dmcharacter.h" 
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/gui/zguiwindow.h"

P_DMShop::P_DMShop()
{
	strcpy(m_acName,"P_DMShop");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

	m_pkItems = NULL;
}

P_DMShop::~P_DMShop()
{


}

void P_DMShop::Init()
{
	m_pkItems = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),8,10);
}


vector<PropertyValues> P_DMShop::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
	//	
	//kReturn[0].kValueName = "name";
	//kReturn[0].iValueType = VALUETYPE_STRING;
	//kReturn[0].pkValue    = &pkMissionm_strName;		

	//kReturn[1].kValueName = "difficulty";
	//kReturn[1].iValueType = VALUETYPE_INT;
	//kReturn[1].pkValue    = &m_iDifficulty;	

	return kReturn;
}

void P_DMShop::Save(ZFIoInterface* pkPackage)
{	
	//saving inventory
	m_pkItems->Save(pkPackage);	
}

void P_DMShop::Load(ZFIoInterface* pkPackage)
{
	//loading inventory
	m_pkItems->Load(pkPackage);		
}

bool P_DMShop::SetPrice(int x, int y, int iPrice)
{
	int* pID = m_pkItems->GetItem(x,y);

	if(pID)
	{
		m_kPricemap.insert( map<int, int>::value_type(*pID, iPrice));
		return true;
	}

	return false;
}

int P_DMShop::GetPrice(int x, int y)
{
	int* pID = m_pkItems->GetItem(x,y);

	if(pID)
	{
		map<int, int>::iterator itItem;
		itItem = m_kPricemap.find(*pID);
		
		if(itItem != m_kPricemap.end())
			return itItem->second;
	}

	return -1;
}

Property* Create_P_DMShop()
{
	return new P_DMShop;
}