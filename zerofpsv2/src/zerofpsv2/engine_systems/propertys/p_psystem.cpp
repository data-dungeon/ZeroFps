#include "../../engine/psystemmanager.h"
#include "p_psystem.h"
#include "../../engine/entity.h"
#include "../../engine/entitymanager.h"

// ------------------------------------------------------------------------------------------

void P_PSystem::Update()
{
	if ( m_pkPSystem )
	{
		Matrix4 kMat;
		kMat = m_pkObject->GetWorldRotM();
		
	
      // returns true if the PSystem is finished
		if ( !m_pkPSystem->Update( m_pkObject->GetIWorldPosV(), kMat ) )
		{
 		   if(m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER))
	 		   m_pkPSystem->Draw();
	 		else
		 		glPopAttrib();
 		}
      else
      {
         if ( m_pkPSystem->m_pkPSystemType->m_kPSystemBehaviour.m_bRemoveParentOnFinish )
            m_pkObject->m_pkObjectMan->Delete ( m_pkObject );

         delete m_pkPSystem;

         m_pkPSystem = 0;

         glPopAttrib();
      }
	}
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_PSystem::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;

	return kReturn;
}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem()
{
   bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT;
	m_iSortPlace =	9;

	strcpy(m_acName,"P_PSystem");
	m_pkPSystem = NULL;

}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem( string kPSType )
{
   bNetwork = true;

   m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
	m_iSortPlace =	9;

	strcpy(m_acName,"P_PSystem");
	SetPSType ( kPSType );
}

// ------------------------------------------------------------------------------------------

bool P_PSystem::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "PSType") == 0 ) 
	{
		//cout << "Setting ParticleSystem:" << kValue.c_str() << endl;

		SetPSType( kValue.c_str() );

		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------

void P_PSystem::SetPSType( string kName )
{

	m_pkPSystem = PSystemManager::GetInstance()->GetPSSystem ( kName );
   m_kPSType = kName;
   
   SetNetUpdateFlag(true);
}

// ------------------------------------------------------------------------------------------

void P_PSystem::Save(ZFIoInterface* pkPackage)
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

void P_PSystem::Load(ZFIoInterface* pkPackage)
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

void P_PSystem::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
   pkNetPacket->Write_Str( m_kPSType.c_str() );
   
	SetNetUpdateFlag(iConnectionID,false);   
}

// ------------------------------------------------------------------------------------------

void P_PSystem::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	char temp[128];
	pkNetPacket->Read_Str(temp);
	m_kPSType = temp;	
	
	if(m_kPSType == "nons")
		return;
	
	if(!m_pkPSystem)
		SetPSType( m_kPSType );
	else	
		if(m_pkPSystem->m_pkPSystemType->m_kName != m_kPSType)
			SetPSType( m_kPSType );		

}

// ------------------------------------------------------------------------------------------

P_PSystem::~P_PSystem()
{
   delete m_pkPSystem;
}

// ------------------------------------------------------------------------------------------

Property* Create_PSystemProperty()
{
	return new P_PSystem;
}

