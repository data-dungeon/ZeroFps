#include "p_ml.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_Ml::P_Ml()
{
	strcpy(m_acName,"P_Ml");		
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkEntityMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	bNetwork = true;
	
	m_iSpawn= -1;
	m_iSpawnZon = -1;
	
	m_bJustSaved =	false;
	m_bShowText =	false;
	
	m_strText = "";
}

void P_Ml::Init()
{
	if(m_strText == "")
		m_strText = m_pkObject->GetName();

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
				/***************
					HACK FOR DEMO: Krash if -1
				****************/
				if(m_iSpawnZon != -1) {
					m_pkEntityMan->LoadZone(m_iSpawnZon);
					ent = m_pkFps->m_pkObjectMan->GetObjectByNetWorkID(m_iSpawn);
					}
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
/*	Entity* pkEnt = m_pkEntityMan->GetObjectByNetWorkID(m_pkObject->iNetWorkID);
	
	if(m_pkObject != pkEnt)
		cout<<"SOMETHING IS FUCKING WRONG!!!!"<<endl;
*/

/*
	//test
		m_pkObject->SetWorldPosV(m_pkObject->GetWorldPosV() + Vector3(0,-1,0) * m_pkFps->GetFrameTime());					
	
	vector<Entity*> kObjects;
	Vector3 start = m_pkObject->GetWorldPosV();
	Vector3 dir = Vector3(0,-1,0);
	Vector3 pos = start;	
	
	m_pkObjMan->GetZoneObject()->GetAllObjects(&kObjects);
	
	//cout<<"nr of targets: "<<kObjects.size()<<endl;
	
	float closest = 999999999;

	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{	
		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{	
				
				float d = (start - mp->GetLastColPos()).Length();
	
				if(d < closest)
				{
					pos = mp->GetLastColPos();
					closest = d;
					pkClosest = kObjects[i];
				}				
			}
		}					
	}	
	
	if(pkClosest)
	{
		static Vector3 oldpos;
	
		if(closest < 0.8)
			m_pkObject->SetWorldPosV(oldpos);
		else
		{
			oldpos = start;
		
			if(closest < 1)
			{	
				m_pkObject->SetWorldPosV(pos + Vector3(0,1,0));		
			}
		}
		
	}
	
	
	
	
	//end of test
*/	
	
	
	if(m_pkFps->m_bServerMode)
		if(m_pkObjMan->IsUpdate(PROPERTY_TYPE_NORMAL))
			m_bJustSaved = false;
	
		
	if(m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		if(m_bShowText)
		{
			m_pkRender->SetFont("data/textures/text/objecttext.tga");
			m_pkRender->Print(m_pkObject->GetIWorldPosV()+Vector3(0,m_pkObject->GetRadius()+0.2f,0),Vector3(.2f,.2f,.2f),(char*)m_strText.c_str());				
		}
	}
}

void P_Ml::AddAction(const char* csAction)
{
	//cout<<"registering action:"<<csAction<<endl;
	m_kActions.push_back(string(csAction));
	
	SetNetUpdateFlag(true);		
}


void P_Ml::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Write(&m_bShowText,sizeof(m_bShowText));

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
	pkNetPacket->Read(&m_bShowText,sizeof(m_bShowText));

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
	pkPackage->Write((void*)&m_bShowText,sizeof(m_bShowText),1);	
	
	m_bJustSaved = true;
}

void P_Ml::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read((void*)&m_iSpawn,sizeof(m_iSpawn),1);		
	pkPackage->Read((void*)&m_iSpawnZon,sizeof(m_iSpawnZon),1);	
	pkPackage->Read((void*)&m_bShowText,sizeof(m_bShowText),1);		
}



vector<PropertyValues> P_Ml::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "showtext";
	kReturn[0].iValueType = VALUETYPE_BOOL;
	kReturn[0].pkValue    = (void*)&m_bShowText;


	return kReturn;
}

Property* Create_P_Ml()
{
	return new P_Ml;
}
