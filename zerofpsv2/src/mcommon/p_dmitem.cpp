#include "p_dmitem.h" 

P_DMItem::P_DMItem()
{
	strcpy(m_acName,"P_DMItem");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	bNetwork = true;
	

	m_strName = "Unnamed Item";
	m_strIcon = "default.bmp";
	m_iSizeX = 2;	
	m_iSizeY = 2;	
	m_iType = DMItemDefault;

}

P_DMItem::~P_DMItem()
{


}

void P_DMItem::Init()
{
	cout<< "New item created :"<<m_iSizeX<<" x "<<m_iSizeY<<endl;

}


vector<PropertyValues> P_DMItem::GetPropertyValues()
{
	vector<PropertyValues> kReturn(5);
		
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


	kReturn[4].kValueName = "type";
	kReturn[4].iValueType = VALUETYPE_INT;
	kReturn[4].pkValue    = &m_iType;		

	return kReturn;
}

void P_DMItem::Save(ZFIoInterface* pkPackage)
{	
	char temp[128];
	
	strcpy(temp,m_strName.c_str());
	pkPackage->Write(temp,128,1);
	
	strcpy(temp,m_strIcon.c_str());
	pkPackage->Write(temp,128,1);

	
	pkPackage->Write(&m_iSizeX,sizeof(m_iSizeX),1);	
	pkPackage->Write(&m_iSizeY,sizeof(m_iSizeY),1);	
	
	pkPackage->Write(&m_iType,sizeof(m_iType),1);	
	
}

void P_DMItem::Load(ZFIoInterface* pkPackage)
{
	char temp[128];

	pkPackage->Read(temp,128,1);
	m_strName = temp;

	pkPackage->Read(temp,128,1);
	m_strIcon = temp;

	pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);	
	pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);	

	pkPackage->Read(&m_iType,sizeof(m_iType),1);	

}

Property* Create_P_DMItem()
{
	return new P_DMItem;
}







