#include "p_characterproperty.h"




P_CharacterProperty::P_CharacterProperty()
{
	strcpy(m_acName,"P_CharacterProperty");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = 		true;
	m_iVersion = 	1;
	
}


P_CharacterProperty::~P_CharacterProperty()
{


}





Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}





