#include "p_dmhq.h" 
#include "../zerofpsv2/engine/p_pfpath.h"

P_DMHQ::P_DMHQ()
{
	strcpy(m_acName,"P_DMHQ");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
//	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkEntityMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
//	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	bNetwork = true;
	

	m_strName = "Unnamed HQ";
	m_kExitOffset.Set(0,0,2);
}

P_DMHQ::~P_DMHQ()
{


}

void P_DMHQ::Init()
{
	//cout<< "New HQ created"<<endl;
	m_pkObject->SetUpdateStatus(UPDATE_NOCHILDS);
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
	m_pkObject->GetAllObjects(&m_kEntitys);
	
	
	for(int i = 0;i<m_kEntitys.size();i++)
	{
		if(m_kEntitys[i]->GetProperty("P_DMCharacter"))
		{
			m_pkEntitys->push_back(m_kEntitys[i]->iNetWorkID);
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

vector<PropertyValues> P_DMHQ::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_DMHQ()
{
	return new P_DMHQ;
}







