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
	
	bNetwork = true;
	

	m_strName = "Unnamed Item";
	m_strIcon = "default.bmp";
	m_iSizeX = 1;	
	m_iSizeY = 1;	
	m_iType = 0;

}

P_Item::~P_Item()
{

}

void P_Item::Init()
{


}

void P_Item::Save(ZFIoInterface* pkPackage)
{


}

void P_Item::Load(ZFIoInterface* pkPackage,int iVersion)
{


}


Property* Create_P_Item()
{
	return new P_Item;
}

