#include "p_dmhq.h" 
#include "../zerofpsv2/engine/p_pfpath.h"

P_DMHQ::P_DMHQ()
{
	strcpy(m_acName,"P_DMHQ");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	

	bNetwork = true;
	
	// Start with 1000$$$$$$
	m_iMoney = 1000;

	m_strName = "Unnamed HQ";
	m_kExitOffset.Set(0,0,2);
	m_bActiveHQ = false;
}

P_DMHQ::~P_DMHQ()
{


}

void P_DMHQ::Init()
{
	//cout<< "New HQ created"<<endl;
	m_pkObject->SetUpdateStatus(UPDATE_NOCHILDS);
	m_iReputation = 1;
}


void P_DMHQ::SpawnNewCharacter()
{
	Entity* pkEnt = m_pkObjMan->CreateObjectFromScript("data/script/objects/dm/t_character.lua");
	
	InsertCharacter(pkEnt->GetEntityID());
}

bool P_DMHQ::InsertCharacter(int iID)
{
	Entity* pkEnt = m_pkObjMan->GetObjectByNetWorkID(iID);
	if(pkEnt)
	{
		if(pkEnt->GetProperty("P_DMCharacter"))
		{
			cout<<"character entering HQ"<<endl;
			
			pkEnt->SetParent(m_pkObject);
			
			return true;
		}
	}
	
	return false;
}

void P_DMHQ::GetCharacters(vector<int>* m_pkEntitys)
{
	vector<Entity*>	m_kEntitys;
	m_pkObject->GetAllEntitys(&m_kEntitys,true);
	
	
	for(int i = 0;i<m_kEntitys.size();i++)
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
	
	
	for(int i = 0;i<m_kEntitys.size();i++)
	{
		if(m_kEntitys[i] == iID)	
		{
			if(Entity* pkEnt = m_pkObjMan->GetObjectByNetWorkID(m_kEntitys[i]))
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
	
	for(int i = 0;i<m_kEntitys.size();i++)
	{
		if(Entity* pkEnt = m_pkObjMan->GetObjectByNetWorkID(m_kEntitys[i]))
		{
			Eject(pkEnt);
		}
	}
}

void P_DMHQ::Eject(Entity* pkEnt)
{
	pkEnt->SetParent(m_pkObject->GetParent());
	pkEnt->SetWorldPosV(m_pkObject->GetWorldPosV());
				
	if(P_PfPath* pkPath = (P_PfPath*)pkEnt->GetProperty("P_PfPath"))
		pkPath->MakePathFind(m_pkObject->GetWorldPosV()+m_kExitOffset+Vector3( (rand()%20)/10.0,0,(rand()%20)/10.0));
}

bool P_DMHQ::Pay (int iCost)
{
	if ( iCost > m_iMoney )
		return false;

	m_iMoney -= iCost;

	return true;
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
	pkPackage->Write(&m_iReputation,sizeof(m_iReputation),1);	
}

void P_DMHQ::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read(&m_bActiveHQ,sizeof(m_bActiveHQ),1);	
	pkPackage->Read(&m_iReputation,sizeof(m_iReputation),1);	
}

Property* Create_P_DMHQ()
{
	return new P_DMHQ;
}




