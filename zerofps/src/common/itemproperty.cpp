#include "itemproperty.h"

ItemProperty::ItemProperty()
{
	strcpy(m_acName,"ItemProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;


	m_bPickable	=	true;
	m_iItemSizeX =	1;
	m_iItemSizeY =	1;
	m_kItemIcon	=	"noicon.tga";
	m_kDescription= "This item does not have any description";
	m_kItemName	=	"NoName";
}

vector<PropertyValues> ItemProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(6);

	kReturn[0].kValueName="m_bPickable";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bPickable;
	
	kReturn[1].kValueName="m_iItemSizeX";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&m_iItemSizeX;
	
	kReturn[2].kValueName="m_iItemSizeY";
	kReturn[2].iValueType=VALUETYPE_INT;
	kReturn[2].pkValue=(void*)&m_iItemSizeY;
	
	kReturn[3].kValueName="m_kItemIcon";
	kReturn[3].iValueType=VALUETYPE_STRING;
	kReturn[3].pkValue=(void*)&m_kItemIcon;
	
	kReturn[4].kValueName="m_kDescription";
	kReturn[4].iValueType=VALUETYPE_STRING;
	kReturn[4].pkValue=(void*)&m_kDescription;
	
	kReturn[5].kValueName="m_kItemName";
	kReturn[5].iValueType=VALUETYPE_STRING;
	kReturn[5].pkValue=(void*)&m_kItemName;
	
	return kReturn;
};

void ItemProperty::Save(ZFMemPackage* pkPackage)
{
	char data[300];	
	
	pkPackage->Write((void*)&m_bPickable,sizeof(m_bPickable));
	pkPackage->Write((void*)&m_iItemSizeX,sizeof(m_iItemSizeX));
	pkPackage->Write((void*)&m_iItemSizeY,sizeof(m_iItemSizeY));
	
	strcpy(data,m_kItemIcon.c_str());		
	pkPackage->Write((void*)&data,300);
	
	strcpy(data,m_kDescription.c_str());		
	pkPackage->Write((void*)&data,300);
	
	strcpy(data,m_kItemName.c_str());		
	pkPackage->Write((void*)&data,300);

}

void ItemProperty::Load(ZFMemPackage* pkPackage)
{
	char data[300];	
	
	pkPackage->Read((void*)&m_bPickable,sizeof(m_bPickable));
	pkPackage->Read((void*)&m_iItemSizeX,sizeof(m_iItemSizeX));
	pkPackage->Read((void*)&m_iItemSizeY,sizeof(m_iItemSizeY));
	
	pkPackage->Read((void*)&data,300);
	m_kItemIcon=data;
	
	pkPackage->Read((void*)&data,300);
	m_kDescription=data;
	
	pkPackage->Read((void*)&data,300);
	m_kItemName=data;

}


Property* Create_ItemProperty()
{
	return new ItemProperty();

}




