#include "p_item.h"
#include "rulesystem/item/itemdatabase.h"
#include "../zerofpsv2/engine/object.h"

// ------------------------------------------------------------------------------------------

void P_Item::Update()
{
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_Item::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
/*	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;*/

	return kReturn;
}

// ------------------------------------------------------------------------------------------

P_Item::P_Item()
{
	m_iSide = PROPERTY_SIDE_CLIENT;
   
   m_pkItemStats = 0;

	strcpy(m_acName,"P_Item");
}

// ------------------------------------------------------------------------------------------

P_Item::P_Item( string kName )
{
   m_pkItemStats = 0;

	strcpy(m_acName,"P_Item");
}

// ------------------------------------------------------------------------------------------

bool P_Item::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "Type") == 0 ) 
	{
      m_pkItemStats = ItemDataBase::GetItem ( kValue );

		return true;
	}

	return false;
}
// ------------------------------------------------------------------------------------------

Property* Create_P_Item()
{
	return new P_Item;
}

