#include "../../engine/psystemmanager.h"
#include "p_psystem.h"
#include "../../engine/entity.h"
#include "../../engine/entitymanager.h"
#include "../script_interfaces/si_objectmanager.h" 

using namespace ObjectManagerLua;

// ------------------------------------------------------------------------------------------

void P_PSystem::Update()
{

	StartProfileTimer("r___PSystem");

	m_pkZShaderSystem->Push("P_PSystem::Update");
	
	glDisable(GL_STENCIL_TEST);
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if ( m_pkPSystem )
	{
		Matrix4 kMat;
		kMat = m_pkEntity->GetWorldRotM();
		
      // returns true if the PSystem is finished
		if ( !m_pkPSystem->Update( m_pkEntity->GetIWorldPosV(), kMat ) )
		{
 		   if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER_NOSHADOW))
 		   {
	 		   m_pkPSystem->Draw();
			}
 		}
      else
      {
         if ( m_pkPSystem->m_pkPSystemType->m_kPSystemBehaviour.m_bRemoveParentOnFinish )
         {
            m_pkEntity->m_pkEntityManager->Delete ( m_pkEntity );
			}

         delete m_pkPSystem;

         m_pkPSystem = 0;

//         glPopAttrib();
      }
	}

	glPopAttrib();
	
	m_pkZShaderSystem->Pop();
	
	StopProfileTimer("r___PSystem");	
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

	m_iType = PROPERTY_TYPE_RENDER_NOSHADOW|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;
	m_iSortPlace =	9;
	m_iVersion = 2;
	
	strcpy(m_acName,"P_PSystem");
	m_pkPSystem = NULL;
	

	
	m_pkZShaderSystem =  static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem( string kPSType )
{
   bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER_NOSHADOW|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;	
// m_iType = PROPERTY_TYPE_RENDER_NOSHADOW;
//	m_iSide = PROPERTY_SIDE_CLIENT;
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
	pkPackage->Write_Str(m_kPSType);

   
   // PSAge
   float fAge = 0;
	if(m_pkPSystem)
		fAge = m_pkPSystem->Age();
		
   pkPackage->Write (fAge);

}

// ------------------------------------------------------------------------------------------

void P_PSystem::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
		{
				char temp[128];
				pkPackage->Read((void*)&temp,128,1);
				m_kPSType=temp;
			
				// Load PSType data and init PSystem
				SetPSType( m_kPSType );
				
				// Read PSAge
				float fAge;
				pkPackage->Read ( fAge);
				m_pkPSystem->SetAge (fAge);		
		
			break;		
		}
		
		case 2:
		{
			pkPackage->Read_Str(m_kPSType);
		
			// Load PSType data and init PSystem
			SetPSType( m_kPSType );
			
			// Read PSAge
			float fAge;
			pkPackage->Read ( fAge);
			
			if(m_pkPSystem)
				m_pkPSystem->SetAge (fAge);
		
		
			break;
		}
	
	}	

}

// ------------------------------------------------------------------------------------------

void P_PSystem::PackTo( NetPacket* pkNetPacket, int iConnectionID  )
{	
	pkNetPacket->Write_Str( m_kPSType);
	
	SetNetUpdateFlag(iConnectionID,false);   
}

// ------------------------------------------------------------------------------------------

void P_PSystem::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read_Str(m_kPSType);
	
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

/* ********************************** SCRIPT INTERFACE ****************************************/
/**	\brief Script functions for Particle Systems
	\ingroup si
*/
class SIPSystem { };

namespace SI_PPSystem
{
/**	\fn SetPSystem( Entity, SystemName)
 	\relates SITcs
	\brief Sets the particle system to use.

	Impulse is given in the direction specified by xyz.
*/
int SetPSystemLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 0)
		return 0;

	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
		int iId = (int)dTemp;
		
		Entity* pkObject = g_pkObjMan->GetEntityByID(iId);
		
		if(pkObject)
			pkObject->DeleteProperty("P_PSystem");

		return 0;
	}
	
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
		int iId = (int)dTemp;
		
		Entity* pkObject = g_pkObjMan->GetEntityByID(iId);
		if(pkObject)
		{
			P_PSystem* ps = (P_PSystem*)pkObject->GetProperty("P_PSystem");
			
			if(!ps)
				ps = (P_PSystem*)pkObject->AddProperty("P_PSystem");
			
			if(ps)
			{
				char	acType[128];
				g_pkScript->GetArgString(pkLua, 1, acType);
				
				ps->SetPSType((string)acType);
				return 0;
			}
		}

		
		return 0;
	}
	return 0;
}
}

// ------------------------------------------------------------------------------------------
Property* Create_PSystemProperty()
{
	return new P_PSystem;
}

void ENGINE_SYSTEMS_API Register_PPSystem(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_PSystem", Create_PSystemProperty);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("SetPSystem",	SI_PPSystem::SetPSystemLua);			
}
