#include "p_dmhq.h" 
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

P_DMHQ::P_DMHQ()
{
	strcpy(m_acName,"P_DMHQ");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	

	m_pkStockroom = NULL;
	m_bNetwork = true;
	
	m_strName = "Unnamed HQ";
	m_kExitOffset.Set(0,0,2);
	m_bActiveHQ = false;
	
	m_kForHireList.clear();
	
	RandomizeHireList(10);
}

P_DMHQ::~P_DMHQ()
{
	if(m_pkStockroom)
		delete m_pkStockroom;

}

void P_DMHQ::Init()
{
	//cout<< "New HQ created"<<endl;
	m_pkEntity->SetUpdateStatus(UPDATE_NOCHILDS);

	m_pkStockroom = new DMContainer(m_pkEntityManager,m_pkEntity->GetEntityID(),8,10);
	m_pkStockroom->AddItemType(DMITEM_ARMOUR);
	m_pkStockroom->AddItemType(DMITEM_GRENADE);
	m_pkStockroom->AddItemType(DMITEM_CLIP);
	m_pkStockroom->AddItemType(DMITEM_MEDKIT);
	m_pkStockroom->AddItemType(DMITEM_GRENADE);
	m_pkStockroom->AddItemType(DMITEM_WEAPON);
	m_pkStockroom->AddItemType(DMITEM_IMPLANT);

}


void P_DMHQ::SpawnNewCharacter(int iNr)
{
	Entity* pkEnt = m_pkEntityManager->CreateEntityFromScript("data/script/objects/characters/member.lua");
	
	if(iNr != -1)
	{
		if(iNr >= 0 && iNr < m_kForHireList.size())
		{
			if(P_DMCharacter* pkCh = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
			{
				pkCh->GetStats()->Set(m_kForHireList[iNr]);			
				//cout<<"created objet from list"<<endl;
			
			
				int i = 0;
				for(vector<DMCharacterStats>::iterator kIte = m_kForHireList.begin();
	        		kIte != m_kForHireList.end(); kIte++ )
   	     	{
      	  		if(i == iNr)
        			{
        				m_kForHireList.erase(kIte);        			
        				break;
     	   		}        		
	        		i++;
      	  	}			
			}
		}
		else
			cout<<"that character does not exist"<<endl;
	
		//cout<<"gubbar kvar"<<m_kForHireList.size()<<endl;

	}
	
	
	InsertCharacter(pkEnt->GetEntityID());
}

void P_DMHQ::RandomizeHireList(int iNr)
{
	m_kForHireList.clear();
	
	
	for(int i = 0 ;i<iNr;i++)
	{
		DMCharacterStats temp;
		temp.Randomize();	
		m_kForHireList.push_back(temp);
	}
	
	for(int i = 0 ;i<iNr;i++)
	{
		cout<<"names:"<<m_kForHireList[i].m_strName<<endl;
	}
}

bool P_DMHQ::InsertCharacter(int iID)
{
	Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID);
	if(pkEnt)
	{
		P_DMCharacter* pkCharProp;
		if((pkCharProp = (P_DMCharacter*) pkEnt->GetProperty("P_DMCharacter")) != NULL)
		{
			// Stoppa eventuellt walksound
			if(pkCharProp->WalkSoundIsPlaying())
			{
				ZFAudioSystem* pkAudioSys = 
					static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));

	/*			pkAudioSys->StopSound("/data/sound/walk_zombie.wav", 
					m_pkEntity->GetIWorldPosV()); */
			}

			cout<<"character entering HQ"<<endl;
			
			pkEnt->SetParent(m_pkEntity);
			
			return true;
		}
	}
	
	return false;
}

void P_DMHQ::GetCharacters(vector<int>* m_pkEntitys)
{
	vector<Entity*>	m_kEntitys;
	m_pkEntity->GetAllEntitys(&m_kEntitys,true);
	
	
	for(unsigned int i = 0;i<m_kEntitys.size();i++)
	{
		if(m_kEntitys[i]->GetProperty("P_DMCharacter"))
		{
			m_pkEntitys->push_back(m_kEntitys[i]->GetEntityID());
		}
	}
}

bool P_DMHQ::EjectCharacter(int iID)
{
	vector<int> m_kEntitys;
	GetCharacters(&m_kEntitys);
	
	
	for(unsigned int i = 0;i<m_kEntitys.size();i++)
	{
		if(m_kEntitys[i] == iID)	
		{
			if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_kEntitys[i]))
			{				
				Eject(pkEnt);
				return true;
			}
		}
	}
	
	return false;
}

void P_DMHQ::EjectAllCharacters()
{
	vector<int> m_kEntitys;
	GetCharacters(&m_kEntitys);
	
	for(unsigned int i = 0;i<m_kEntitys.size();i++)
	{
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_kEntitys[i]))
		{
			Eject(pkEnt);
		}
	}
}

void P_DMHQ::Eject(Entity* pkEnt)
{
	pkEnt->SetParent(m_pkEntity->GetParent());
	pkEnt->SetWorldPosV(m_pkEntity->GetWorldPosV());
				
	if(P_PfPath* pkPath = (P_PfPath*)pkEnt->GetProperty("P_PfPath"))
		pkPath->MakePathFind(m_pkEntity->GetWorldPosV()+m_kExitOffset+Vector3( (rand()%20)/10.0,0,(rand()%20)/10.0));
}

bool P_DMHQ::FireCharacter(int iID)
{
	if(EjectCharacter(iID))
	{
		m_pkEntityManager->Delete(iID);
		return true;
	}

	return false;
}


vector<PropertyValues> P_DMHQ::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "active";
	kReturn[0].iValueType = VALUETYPE_BOOL;
	kReturn[0].pkValue    = &m_bActiveHQ;		


	return kReturn;
}



void P_DMHQ::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write(&m_bActiveHQ,sizeof(m_bActiveHQ),1);	
//	pkPackage->Write(&m_iReputation,sizeof(m_iReputation),1);	

	m_pkStockroom->Save(pkPackage);		
}

void P_DMHQ::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(&m_bActiveHQ,sizeof(m_bActiveHQ),1);	
//	pkPackage->Read(&m_iReputation,sizeof(m_iReputation),1);	

	m_pkStockroom->Load(pkPackage);		
}

Property* Create_P_DMHQ()
{
	return new P_DMHQ;
}




