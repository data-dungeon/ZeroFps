#include "../common/psystemmanager.h"
#include "p_psystem.h"
#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../engine/entitymanager.h"
#include "../script_interfaces/si_objectmanager.h" 

using namespace ObjectManagerLua;

// ------------------------------------------------------------------------------------------

void P_PSystem::GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState)
{
	for (int i = 0; i < m_kPSystems.size(); i++)
	{
		if ( m_kPSystems[i].m_pkPSystem )
		{
			if(UpdatePS(i,&kRenderState))
			{
				if(RenderPackage* pkRenderPackage = m_kPSystems[i].m_pkPSystem->GetRenderPackage(kRenderState))
					kRenderPackages.push_back(pkRenderPackage);
			}
				
		}
	}
	
	m_pkEntity->SetLocalAABB(10);
}

void P_PSystem::Update()
{
// 	for (int i = 0; i < m_kPSystems.size(); i++)
// 	{
// 		if ( m_kPSystems[i].m_pkPSystem )
// 		{
// 			UpdatePS(i,NULL);
// 		}		
// 	}

// 	for (int i = 0; i < m_kPSystems.size(); i++)
// 	{
// 		if ( m_kPSystems[i].m_pkPSystem )
// 		{
// 		
// 			if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
// 			{		
// 				if( !m_pkZeroFps->GetRenderOn() || m_pkZeroFps->GetMinimized() )
// 				{
// 					UpdatePS(i);
// 				}		
// 				
// 				return;
// 			}
// 
// 			if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER) && m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_NOSHADOWLAST)
// 			{
// 				if(UpdatePS(i))
// 				{			
// 					if(m_kPSystems[i].m_pkPSystem->m_bInsideFrustum)
// 					{				
// 						m_kPSystems[i].m_pkPSystem->m_pkLight->Update(&m_kPSystems[i].m_pkPSystem->m_kLightProfile, GetEntity()->GetWorldPosV());	
// 						m_kPSystems[i].m_pkPSystem->Draw();
// 					}
// 				}
// 			}
// 		}
// 	}
}

bool P_PSystem::UpdatePS(int iPS,const RenderState*	pkRenderState)
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

	if( m_kPSystems[iPS].m_pkPSystem->Update( m_pkEntity->GetIWorldPosV() + kJointPos, kMat,pkRenderState) )
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

vector<PropertyValues>& P_PSystem::GetPropertyValues()
{
// 	vector<PropertyValues> kReturn(m_kPSystems.size());
	static vector<PropertyValues> kReturn;
	
	kReturn.clear();
		
	for (int i = 0; i < m_kPSystems.size(); i++)
	{
		kReturn.push_back(PropertyValues("PSType",VALUETYPE_STRING,(void*)&m_kPSystems[i].m_strPSName));
	
/*		kReturn[i].kValueName = "PSType";
		kReturn[i].iValueType = VALUETYPE_STRING;
		kReturn[i].pkValue    = (void*)&m_kPSystems[i].m_strPSName;*/
	}

	return kReturn;
}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem() : Property("P_PSystem")
{
   m_bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;
	m_iSortPlace =	4;
	m_iVersion = 3;
	
	
	m_pkZShaderSystem =  static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
}

// ------------------------------------------------------------------------------------------

P_PSystem::P_PSystem( string kPSType ) : Property("P_PSystem")
{
   m_bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;	
	m_iSortPlace =	4;

	SetPSType ( kPSType );
}

// ------------------------------------------------------------------------------------------

bool P_PSystem::HandleSetValue( const string& kValueName, const string& kValue )
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
	int iCount = 0;

	// Count Alive psystems
	for (int i = 0; i < m_kPSystems.size();i++)
		if ( m_kPSystems[i].m_pkPSystem )
			iCount++;
	
	pkPackage->Write(iCount);
   
	for (int i = 0; i < m_kPSystems.size(); i++)
	{
		if ( m_kPSystems[i].m_pkPSystem )
		{
			// PSType
			pkPackage->Write_Str(m_kPSystems[i].m_strPSName);
			pkPackage->Write(m_kPSystems[i].m_pkPSystem->m_fAge);
		}
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
	{
		pkNetPacket->Write_Str( m_kPSystems[i].m_strPSName );
		//pkNetPacket->Write(m_kPSystems[i].m_pkPSystem->m_fAge );
	}
	
	SetNetUpdateFlag(iConnectionID,false);   
}

// ------------------------------------------------------------------------------------------

void P_PSystem::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int iSize;
	float fAge;
	string strPSName;
	pkNetPacket->Read(iSize);


	for (int i = 0; i < iSize; i++)
	{
		pkNetPacket->Read_Str(strPSName);
		if(strPSName == "nons")
			continue;

		//pkNetPacket->Read(fAge, sizeof(float);
	
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
		\param SystemName Name of particle system to use.
		\brief Sets a particle system on a entity.

		If only the first parameter is used the particle system of the entity (if any) will be removed.

		If the second parameter is given it must be the name of one of the particle system in /data/psystem. 
		A P_System property will be assigned to the entity if it does not have one.
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

void ENGINE_SYSTEMS_API Register_PPSystem(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_PSystem", Create_PSystemProperty);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("SetPSystem",	SI_PPSystem::SetPSystemLua);			
}
