#include "p_ml.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_Ml::P_Ml()
{
	strcpy(m_acName,"P_Ml");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkEntityMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	bNetwork = true;
	
	m_iSpawn= -1;
	m_iSpawnZon = -1;
	
	m_bJustSaved= false;
}

P_Ml::~P_Ml()
{
	if(m_bJustSaved)
		return;
	
	if(m_iSpawn != -1)
	{
		//cout<<"Trying to find this object's spawner "<<m_iSpawn<<endl;
		
		Entity* ent = m_pkFps->m_pkObjectMan->GetObjectByNetWorkID(m_iSpawn);
	
		if(!ent)
		{
			//cout<<"spawner not found , trying to load spawner zone"<<endl;
						
			//cout<<"spawner zone:"<<m_iSpawnZon<< " im in:"<<m_pkObject->GetCurrentZone()<<endl;
						
			if(m_iSpawnZon != m_pkObject->GetCurrentZone())
			{
				//cout<<"trying to load zone:"<<	m_iSpawnZon<<endl;		
				m_pkEntityMan->LoadZone(m_iSpawnZon);
				ent = m_pkFps->m_pkObjectMan->GetObjectByNetWorkID(m_iSpawn);
			}
		}
	
	
		if(ent)
		{
			//cout<<"found spawner =)"<<endl;
			
			P_Spawn* pkSpawn = (P_Spawn*)ent->GetProperty("P_Spawn");		
			if(pkSpawn)
			{	
				//cout<<"removed me"<<endl;
				pkSpawn->RemoveEntity(m_pkObject);				
			}
		}
	}
}


void P_Ml::Update()
{
/*	//random jump
	Vector3 pos = m_pkObject->GetLocalPosV();
	pos += Vector3((rand() % 1000)/1000.0 - 0.5,(rand() % 1000)/1000.0-0.5,(rand() % 1000)/1000.0-0.5)*4;
	m_pkObject->SetLocalPosV(pos);
*/
	m_bJustSaved = false;

}

void P_Ml::AddAction(const char* csAction)
{
	//cout<<"registering action:"<<csAction<<endl;
	m_kActions.push_back(string(csAction));
	
	SetNetUpdateFlag(true);		
}


void P_Ml::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int nr = m_kActions.size();
	
	pkNetPacket->Write(&nr,sizeof(nr));
	
	for(unsigned int i=0;i<m_kActions.size();i++)
	{
		pkNetPacket->Write_Str(m_kActions[i].c_str());
	}

	SetNetUpdateFlag(iConnectionID,false);
} 

void P_Ml::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int nr;	
	
	pkNetPacket->Read(&nr,sizeof(nr));
	
	m_kActions.clear();
	for(int i=0;i<nr;i++)
	{
		char temp[128];
		pkNetPacket->Read_Str(temp);
		m_kActions.push_back(string(temp));
	}
}

void P_Ml::Save(ZFIoInterface* pkPackage)
{	

	pkPackage->Write((void*)&m_iSpawn,sizeof(m_iSpawn),1);
	pkPackage->Write((void*)&m_iSpawnZon,sizeof(m_iSpawnZon),1);	
	
	m_bJustSaved = true;
}

void P_Ml::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read((void*)&m_iSpawn,sizeof(m_iSpawn),1);	
	
	pkPackage->Read((void*)&m_iSpawnZon,sizeof(m_iSpawnZon),1);	
}


Property* Create_P_Ml()
{
	return new P_Ml;
}
