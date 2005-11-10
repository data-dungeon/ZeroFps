#include "p_spawn.h"
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"
#include "p_ml.h"


P_Spawn::P_Spawn(): Property("P_Spawn")
{
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_iVersion = 1;
	
	m_strEntityScript = "";
	m_fSpawnDelay		=	1;	
	m_fStartTime 		=	m_pkEntityManager->GetSimTime();
	
	
	m_kPropertyValues.push_back(PropertyValues("entityscript",VALUETYPE_STRING,(void*)&m_strEntityScript));
	m_kPropertyValues.push_back(PropertyValues("spawndelay",VALUETYPE_FLOAT,(void*)&m_fSpawnDelay));

}


void P_Spawn::Update()
{
// 	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
// 	{
// 		m_pkRender->Sphere(m_pkEntity->GetWorldPosV(),0.5,1,Vector3(0.5,1,0.5),true);	
// 	}

// 	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
// 	{
		if(m_pkEntityManager->GetSimTime() > m_fStartTime + m_fSpawnDelay)
		{
			//cout<<"spawning "<<m_strEntityScript<<endl;
		
			//create spawn
			if(!m_pkEntityManager->CreateEntityFromScriptInZone(m_strEntityScript.c_str(),m_pkEntity->GetWorldPosV(),m_pkEntity->GetCurrentZone()))
			{
				cout<<"WARNING: could not spawn entity "<<m_strEntityScript<<endl;
			}
			
			//remove self
			m_pkEntityManager->Delete(m_pkEntity);
		}
// 	}
}




void P_Spawn::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write_Str(m_strEntityScript);
	pkPackage->Write(m_fSpawnDelay);
	pkPackage->Write(m_fStartTime);
}

void P_Spawn::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read_Str(m_strEntityScript);
	pkPackage->Read(m_fSpawnDelay);
	pkPackage->Read(m_fStartTime);
}


Property* Create_P_Spawn()
{
	return new P_Spawn;
}












// vector<PropertyValues> P_Spawn::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(2);
// 
// 	kReturn[0].kValueName = "entityscript";
// 	kReturn[0].iValueType = VALUETYPE_STRING;
// 	kReturn[0].pkValue    = (void*)&m_strEntityScript;
// 	
// 	kReturn[1].kValueName = "spawndelay";
// 	kReturn[1].iValueType = VALUETYPE_FLOAT;
// 	kReturn[1].pkValue    = (void*)&m_fSpawnDelay;
// 		
// 	return kReturn;
// }