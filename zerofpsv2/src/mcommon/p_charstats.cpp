#include "rulesystem/character/characterfactory.h"
#include "p_charstats.h"
#include "../zerofpsv2/engine/object.h"

// ------------------------------------------------------------------------------------------

void CharacterProperty::Update()
{
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> CharacterProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
/*	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;*/

	return kReturn;
}

// ------------------------------------------------------------------------------------------

CharacterProperty::CharacterProperty()
{
	m_iSide = PROPERTY_SIDE_CLIENT;
   
   m_pkCharStats = new CharacterStats( m_pkObject );

	strcpy(m_acName,"P_CharStats");
}

// ------------------------------------------------------------------------------------------

CharacterProperty::CharacterProperty( string kName )
{
   m_pkCharStats = new CharacterStats ( m_pkObject );

	strcpy(m_acName,"P_CharStats");
}

// ------------------------------------------------------------------------------------------

bool CharacterProperty::HandleSetValue( string kValueName, string kValue )
{

	return false;
}
// ------------------------------------------------------------------------------------------

Property* Create_P_CharStats()
{
	return new CharacterProperty;
}

