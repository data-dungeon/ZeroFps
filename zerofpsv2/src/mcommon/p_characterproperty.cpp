#include "p_characterproperty.h"




P_CharacterProperty::P_CharacterProperty()
{
	strcpy(m_acName,"P_CharacterProperty");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = 		true;
	m_iVersion = 	1;
	
	
	
	m_strName =					"NoName";
	m_bIsPlayerCharacter =	false;
}


P_CharacterProperty::~P_CharacterProperty()
{


}

void P_CharacterProperty::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strName);
	pkPackage->Write(m_bIsPlayerCharacter);
}

void P_CharacterProperty::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read_Str(m_strName);	
	pkPackage->Read(m_bIsPlayerCharacter);
}




Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}





