#include "p_spawn.h"
#include "p_ml.h"


P_Spawn::P_Spawn()
{
	strcpy(m_acName,"P_Spawn");		
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	


	//default behaviour
	m_iEntityCounter = 0;
	m_iMaxEntitys = 10;
	m_fSpawnDelay = 2;
	m_iSpawnMode = AREA_SPAWN;
	m_fSpawnRadius = 5;
	m_strTemplate = "";
	
	m_fTimer  = m_pkFps->m_pkObjectMan->GetGameTime();
}


void P_Spawn::Update()
{
	if(!m_pkFps->m_bServerMode)	
		return;
	
	//draw a ball on the server
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) 
	{
		m_pkRender->Sphere(m_pkObject->GetWorldPosV(),0.5,1,Vector3(1,0,0),true);
		return;
	}
	
	if(m_strTemplate == "")
		return;
	
	
	switch(m_iSpawnMode)
	{
		case POINT_SPAWN:
		{
			if(m_pkFps->m_pkObjectMan->GetGameTime() - m_fTimer >= m_fSpawnDelay)
			{				
				//dont do anything if spawnded objects is at max
				if(m_iEntityCounter < m_iMaxEntitys)
					SpawnEntity(m_pkObject->GetLocalPosV());				
			
				m_fTimer  = m_pkFps->m_pkObjectMan->GetGameTime();			
			}
		
			break;
		}
		case AREA_SPAWN:
		{			
			float fAtime = m_pkFps->m_pkObjectMan->GetGameTime() - m_fTimer;
			if(fAtime < 0)
				m_fTimer = m_pkFps->m_pkObjectMan->GetGameTime();
			
			if(fAtime >= m_fSpawnDelay)
			{				
				//dont do anything if spawnded objects is at max
				if(m_iEntityCounter < m_iMaxEntitys)
				{
					int nr = int(fAtime / m_fSpawnDelay);
					
					if(m_iEntityCounter + nr >= m_iMaxEntitys)
						nr = m_iMaxEntitys - m_iEntityCounter;
						
					//cout<<"spawning: "<<nr<<endl;
					for(int i = 0;i<nr;i++)
					{
						/***************
							HACK FOR DEMO: No Spawn radius
						****************/
						//Vector3 kPos = m_pkObject->GetLocalPosV();
						Vector3 kPos = m_pkObject->GetLocalPosV() + Vector3( (rand() % (int)m_fSpawnRadius) - m_fSpawnRadius/2 ,0,(rand() % (int)m_fSpawnRadius) - m_fSpawnRadius/2);					
						SpawnEntity(kPos);				
					}
				}				
				m_fTimer  = m_pkFps->m_pkObjectMan->GetGameTime();				
			}
		
			break;
		}
		
	}
}

void P_Spawn::RemoveEntity(Entity* pkEnt)
{
/*	for(vector<int>::iterator it = m_kEntitys.begin(); it != m_kEntitys.end(); it++) 
	{
		if((*it) == pkEnt->iNetWorkID)
		{
			m_kEntitys.erase(it);*/
			m_iEntityCounter--;
			
			//cout<<"counter is at :"<<m_iEntityCounter<<endl;
			
/*			break;
		}
	}*/
}



void P_Spawn::SpawnEntity(Vector3 kPos)
{
	Entity* ent = m_pkObjectMan->CreateObjectFromScriptInZone(m_strTemplate.c_str(),kPos,-1);

	if(ent)
	{
		P_Ml* pkMl = (P_Ml*)ent->GetProperty("P_Ml");
		
		if(pkMl)
		{
			pkMl->SetSpawnPointer(m_pkObject->iNetWorkID,m_pkObject->GetCurrentZone());
			m_iEntityCounter++;
			//m_kEntitys.push_back(ent->iNetWorkID);
		}	
		else
		{
			cout<<"Error spawning:"<<m_strTemplate<<endl;
			cout<<"Does not contain an P_Ml"<<endl;
			m_pkObjectMan->Delete(ent);			
		}
	}
}

vector<PropertyValues> P_Spawn::GetPropertyValues()
{
	vector<PropertyValues> kReturn(5);

	kReturn[0].kValueName = "SpawnDelay";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_fSpawnDelay;
	
	kReturn[1].kValueName = "SpawnMode";
	kReturn[1].iValueType = VALUETYPE_INT;
	kReturn[1].pkValue    = (void*)&m_iSpawnMode;

	kReturn[2].kValueName = "Template";
	kReturn[2].iValueType = VALUETYPE_STRING;
	kReturn[2].pkValue    = (void*)&m_strTemplate;
	
	kReturn[3].kValueName = "SpawnArea";
	kReturn[3].iValueType = VALUETYPE_FLOAT;
	kReturn[3].pkValue    = (void*)&m_fSpawnRadius;

	kReturn[4].kValueName = "MaxSpawn";
	kReturn[4].iValueType = VALUETYPE_INT;
	kReturn[4].pkValue    = (void*)&m_iMaxEntitys;
		
	return kReturn;
}

void P_Spawn::Save(ZFIoInterface* pkPackage)
{	
	char temp[256];
	strcpy(temp,m_strTemplate.c_str());

	pkPackage->Write((void*)&m_iEntityCounter,sizeof(m_iEntityCounter),1);
	pkPackage->Write((void*)&m_iMaxEntitys,sizeof(m_iMaxEntitys),1);	
	pkPackage->Write((void*)&m_fSpawnDelay,sizeof(m_fSpawnDelay),1);		
	pkPackage->Write((void*)&m_iSpawnMode,sizeof(m_iSpawnMode),1);	
	pkPackage->Write((void*)&m_fSpawnRadius,sizeof(m_fSpawnRadius),1);	
	pkPackage->Write((void*)&m_fTimer,sizeof(m_fTimer),1);	

	pkPackage->Write((void*)temp,256,1);	

}

void P_Spawn::Load(ZFIoInterface* pkPackage)
{
	char temp[256];

	pkPackage->Read((void*)&m_iEntityCounter,sizeof(m_iEntityCounter),1);
	pkPackage->Read((void*)&m_iMaxEntitys,sizeof(m_iMaxEntitys),1);	
	pkPackage->Read((void*)&m_fSpawnDelay,sizeof(m_fSpawnDelay),1);		
	pkPackage->Read((void*)&m_iSpawnMode,sizeof(m_iSpawnMode),1);	
	pkPackage->Read((void*)&m_fSpawnRadius,sizeof(m_fSpawnRadius),1);	
	pkPackage->Read((void*)&m_fTimer,sizeof(m_fTimer),1);	


	pkPackage->Read((void*)temp,256,1);	
	m_strTemplate = temp;
	
}


Property* Create_P_Spawn()
{
	return new P_Spawn;
}