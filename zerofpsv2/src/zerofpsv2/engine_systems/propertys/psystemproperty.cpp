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
   bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
	m_iSortPlace =	9;

	strcpy(m_acName,"PSystemProperty");
	m_pkPSystem = 0;

}

// ------------------------------------------------------------------------------------------

PSystemProperty::PSystemProperty( string kPSType )
{
   bNetwork = true;

   m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
	m_iSortPlace =	9;

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
   m_kPSType = kName;
}

// ------------------------------------------------------------------------------------------

void PSystemProperty::Save(ZFIoInterface* pkPackage)
{

   // PSType
   char temp[128];
	strcpy(temp,m_kPSType.c_str());	
	pkPackage->Write((void*)&temp,128,1);

   
   // PSAge
   float fAge = m_pkPSystem->Age();
   pkPackage->Write ( (void*)&fAge, sizeof(float), 1 );

}

// ------------------------------------------------------------------------------------------

void PSystemProperty::Load(ZFIoInterface* pkPackage)
{

   // Read PSType
	char temp[128];
	pkPackage->Read((void*)&temp,128,1);
	m_kPSType=temp;

   // Load PSType data and init PSystem
   SetPSType( m_kPSType );
   
   // Read PSAge
   float fAge = m_pkPSystem->Age();
   pkPackage->Read ( (void*)&fAge, sizeof(float), 1 );
   m_pkPSystem->SetAge (fAge);

}

// ------------------------------------------------------------------------------------------

void PSystemProperty::PackTo( NetPacket* pkNetPacket ) 
{
//   pkNetPacket->Write( m_pkPSystem->m_pkPSystemType->m_kName );
//	pkNetPacket->Write( m_pkPSystem->m_fAge );
}

// ------------------------------------------------------------------------------------------

void PSystemProperty::PackFrom( NetPacket* pkNetPacket ) 
{
//   pkNetPacket->Read( m_pkPSystem->m_pkPSystemType->m_kName );
//	pkNetPacket->Read( m_pkPSystem->m_fAge );

//   m_pkPSystem->TimeoffSet();
}

// ------------------------------------------------------------------------------------------

Property* Create_PSystemProperty()
{
	return new PSystemProperty;
}

