#include "../../engine/psystemmanager.h"
#include "psystemproperty.h"
#include "../../engine/object.h"

// ------------------------------------------------------------------------------------------

void PSystemProperty::Update()
{
	if ( m_pkPSystem )
	{
		m_pkPSystem->Update( m_pkObject->GetWorldPosV(), m_pkObject->GetWorldRotM() );
 		m_pkPSystem->Draw();
	}
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> PSystemProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;

	return kReturn;
}

// ------------------------------------------------------------------------------------------

PSystemProperty::PSystemProperty()
{
	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
	m_iSortPlace =	9;

	strcpy(m_acName,"PSystemProperty");
	m_pkPSystem = 0;

}

// ------------------------------------------------------------------------------------------

PSystemProperty::PSystemProperty( string kPSType )
{
	strcpy(m_acName,"PSystemProperty");
	SetPSType ( kPSType );
}

// ------------------------------------------------------------------------------------------

bool PSystemProperty::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "PSType") == 0 ) 
	{
		cout << "Setting ParticleSystem:" << kValue.c_str() << endl;

		SetPSType( kValue.c_str() );

		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------

void PSystemProperty::SetPSType( string kName )
{
	m_pkPSystem = PSystemManager::GetInstance()->GetPSSystem ( kName );
}

// ------------------------------------------------------------------------------------------


Property* Create_PSystemProperty()
{
	return new PSystemProperty;
}

