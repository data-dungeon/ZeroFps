#include "../../engine/psystemmanager.h"
#include "p_psystem.h"
#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../engine/entitymanager.h"
#include "../script_interfaces/si_objectmanager.h" 

using namespace ObjectManagerLua;

// ------------------------------------------------------------------------------------------

void P_PSystem::Update()
{

// 	StartProfileTimer("r___PSystem");

	//m_pkZShaderSystem->Push("P_PSystem::Update");

// 	static Matrix4 kMat;
// 	kMat = m_pkEntity->GetWorldRotM();
// 
// 	int iFinishedPS = 0;
// 	cout<<"crashing"<<endl;
// 	cout<<"psystemid: "<<m_pkEntity->GetEntityID()<<"   pss: "<<m_kPSystems.size()<<endl;
	for (int i = 0; i < m_kPSystems.size(); i++)
	{
		if ( m_kPSystems[i].m_pkPSystem )
		{
		
			if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
			{		
				if( !m_pkZeroFps->GetRenderOn() || m_pkZeroFps->GetMinimized() )
				{
					UpdatePS(i);
				}		
				
				return;
			}

			if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER) && m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_NOSHADOWLAST)
			{
				if(UpdatePS(i))
				{
			
					if(m_kPSystems[i].m_pkPSystem->m_bInsideFrustum)
					{				
						m_kPSystems[i].m_pkPSystem->m_pkLight->Update(&m_kPSystems[i].m_pkPSystem->m_kLightProfile, GetEntity()->GetWorldPosV());	
						m_kPSystems[i].m_pkPSystem->Draw();
					}
				}
			}
		
		
// 			Vector3 kJointPos = Vector3(0,0,0);
// 			// lol!!!1111 l0l! ugly h4c|<
// 			if ( m_kPSystems[i].m_pkPSystem->GetPSystemType()->m_kPSystemBehaviour.m_bInheritPosFromJoint )
// 			{
// 				P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad");
// 
// 				pkMad->UpdateBones();
// 				kJointPos = pkMad->GetJointPosition("root");
// 			}
// 
// 			// returns true if the PSystem is finished
// 			if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
// 			{
// 			
// 			}
// 			
// 			
// 				if ( !m_kPSystems[i].m_pkPSystem->Update( m_pkEntity->GetIWorldPosV() + kJointPos, kMat ) )
// 				{
// 						m_kPSystems[i].m_pkPSystem->m_pkLight->Update(&m_kPSystems[i].m_pkPSystem->m_kLightProfile, GetEntity()->GetWorldPosV());
// 		
// 						if(m_kPSystems[i].m_pkPSystem->m_bInsideFrustum && m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER)
// 							&& m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_NOSHADOWLAST)
// 								m_kPSystems[i].m_pkPSystem->Draw();
// 				}
// 				else
// 				{
// 					if ( m_kPSystems[i].m_pkPSystem->m_pkPSystemType->m_kPSystemBehaviour.m_bRemoveParentOnFinish &&
// 						m_pkEntity->m_eRole == NETROLE_AUTHORITY )
// 					{
// 						m_pkEntity->m_pkEntityManager->Delete ( m_pkEntity );
// 					}
// 	
// 					delete m_kPSystems[i].m_pkPSystem;
// 	
// 					m_kPSystems[i].m_pkPSystem = 0;
// 				}
// 			}
		}
	}

	//m_pkZShaderSystem->Pop();
	
// 	StopProfileTimer("r___PSystem");	
}

bool P_PSystem::UpdatePS(int iPS)
{
	static Matrix4 kMat;
	kMat = m_pkEntity->GetWorldRotM();

	Vector3 kJointPos = Vector3(0,0,0);
	// lol!!!1111 l0l! ugly h4c|<
	if ( m_kPSystems[iPS].m_pkPSystem->GetPSystemType()->m_kPSystemBehaviour.m_bInheritPosFromJoint )
	{
		P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad");

		pkMad->UpdateBones();
		kJointPos = pkMad->GetJointPosition("root");
	}

	if( m_kPSystems[iPS].m_pkPSystem->Update( m_pkEntity->GetIWorldPosV() + kJointPos, kMat ) )
	{	
		if ( m_kPSystems[iPS].m_pkPSystem->m_pkPSystemType->m_kPSystemBehaviour.m_bRemoveParentOnFinish &&
			m_pkEntity->m_eRole == NETROLE_AUTHORITY )
		{
			m_pkEntity->m_pkEntityManager->Delete ( m_pkEntity );
		}

		delete m_kPSystems[iPS].m_pkPSystem;

		m_kPSystems[iPS].m_pkPSystem = 0;	
		return false;
	}
	else
		return true;
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_PSystem::GetPropertyValues()
{
	vector<PropertyValues> kReturn(m_kPSystems.size());
		
	for (int i = 0; i < m_kPSystems.size(); i++)
	{
		kReturn[i].kValueName = "PSType";
		kReturn[i].iValueType = VALUETYPE_STRING;
		kReturn[i].pkValue    = (void*)&m_kPSystems[i].m_strPSName;
	}

	return kReturn;
}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem()
{
   m_bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;
	m_iSortPlace =	9;
	m_iVersion = 3;
	
	strcpy(m_acName,"P_PSystem");
	
	m_pkZShaderSystem =  static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem( string kPSType )
{
   m_bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;	
	m_iSortPlace =	9;

	strcpy(m_acName,"P_PSystem");
	SetPSType ( kPSType );
}

// ------------------------------------------------------------------------------------------

bool P_PSystem::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "PSType") == 0) 
	{
		SetPSType( kValue.c_str() );
		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------

void P_PSystem::SetPSType( string kName )
{
	PSystem* pkNewSystem = PSystemManager::GetInstance()->GetPSystem ( kName );

	if (pkNewSystem)
	{
		ActiveSystem kNewSystem;

		kNewSystem.m_strPSName = kName;
		kNewSystem.m_pkPSystem = pkNewSystem;

		pkNewSystem->SetParent(this);

		m_kPSystems.push_back(kNewSystem);   
	   
		SetNetUpdateFlag(true);
	}
	else
		cout << "Warning! Failed to load PSystem " << kName << endl;
}

// ------------------------------------------------------------------------------------------

void P_PSystem::Save(ZFIoInterface* pkPackage)
{

	// number of PSystems
	pkPackage->Write(m_kPSystems.size());
   
	for (int i = 0; i < m_kPSystems.size(); i++)
	{
		// PSType
		pkPackage->Write_Str(m_kPSystems[i].m_strPSName);
	   pkPackage->Write(m_kPSystems[i].m_pkPSystem->m_fAge);
	}

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
			
				// Load PSType data and init PSystem
				SetPSType( string(temp) );
				
				// Read PSAge
				float fAge;
				pkPackage->Read (fAge);
//				m_pkPSystem->SetAge (fAge);		
		
			break;		
		}
		
		case 2:
		{
			string kPSname;
			pkPackage->Read_Str(kPSname);
		
			// Load PSType data and init PSystem
			SetPSType( kPSname );
			
			// Read PSAge
			float fAge;
			pkPackage->Read (fAge);
			
			if(m_kPSystems.size())
				m_kPSystems[0].m_pkPSystem->SetAge (fAge);
		
			break;
		}

		case 3:
		{
			int iNumOfPSystems;
			float fAge;
			string strPSName;

			// number of PSystems
			pkPackage->Read(iNumOfPSystems);
			
			for (int i = 0; i < iNumOfPSystems; i++)
			{
				pkPackage->Read_Str(strPSName);
				pkPackage->Read(fAge);

				SetPSType( strPSName );

				if (m_kPSystems.size())
               m_kPSystems[i].m_pkPSystem->SetAge(fAge);
			}
		}
	}	

}

// ------------------------------------------------------------------------------------------

void P_PSystem::PackTo( NetPacket* pkNetPacket, int iConnectionID  )
{	
	pkNetPacket->Write(m_kPSystems.size());
	for (int i = 0; i < m_kPSystems.size(); i++)
		pkNetPacket->Write_Str( m_kPSystems[i].m_strPSName );
	
	SetNetUpdateFlag(iConnectionID,false);   
}

// ------------------------------------------------------------------------------------------

void P_PSystem::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int iSize;
	string strPSName;
	pkNetPacket->Read(iSize);


	for (int i = 0; i < iSize; i++)
	{
		pkNetPacket->Read_Str(strPSName);
		
		if(strPSName == "nons")
			continue;
	
		SetPSType( strPSName );
	}

}

// ------------------------------------------------------------------------------------------

P_PSystem::~P_PSystem()
{
	for (int i = 0; i < m_kPSystems.size(); i++)
		if(m_kPSystems[i].m_pkPSystem)
			delete m_kPSystems[i].m_pkPSystem;
			
}

// ------------------------------------------------------------------------------------------

/* ********************************** SCRIPT INTERFACE ****************************************/
namespace SI_PPSystem
{
/** \class PSystem 
 *  \ingroup Property
 *  */

/**	\fn SetPSystem( Entity, SystemName)
 		\relates PSystem
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
				//char	acType[128];
				string strType;
				g_pkScript->GetArgString(pkLua, 1, strType);
				
				ps->SetPSType(strType);
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
