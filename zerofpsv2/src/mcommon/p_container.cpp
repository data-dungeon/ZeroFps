#include "p_clientcontrol.h"
#include "p_container.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 
#include "../zerofpsv2/engine_systems/propertys/p_linktojoint.h" 
#include <iomanip>

// -----------------------------------------------------------------------------------------------

P_Container::P_Container()
{
	m_pkEntMan=			static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	
	strcpy(m_acName,"P_Container");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_bNetwork = 	false;
	m_iVersion = 	3;

	m_bFirstUpdate = true;
	
	
	m_iOwnerID 				= -1;
	m_bStaticOwner			= false;
	m_iMaxItems 			= 0;
	m_iContainerType		= eNormal;
	m_kItemTypes.clear();
	
	SetSize(4,4);		
}

P_Container::~P_Container()
{

}


void P_Container::Init()
{

}

void P_Container::Update()
{
	//try to find items on load
	if(m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		FindMyItems();	
	}

	
	if(!m_bStaticOwner)
	{
		if(m_iOwnerID != -1)
		{
			if(Entity* pkOwner = m_pkEntMan->GetEntityByID(m_iOwnerID))
			{
				if(pkOwner->GetWorldPosV().DistanceTo(GetEntity()->GetWorldPosV()) > 2)
				{
					m_iOwnerID = -1;
					cout<<"Container owner out of distance, reseting owner to -1"<<endl;
				} 
			}
			else
			{
				m_iOwnerID = -1;
				cout<<"container owner does not exist anymore, reseting owner to -1"<<endl;
				
			}
		}
	}
}

vector<PropertyValues> P_Container::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
			
	kReturn[0].kValueName = "sizex";
	kReturn[0].iValueType = VALUETYPE_INT;
	kReturn[0].pkValue    = &m_iSizeX;		
	
	kReturn[1].kValueName = "sizey";
	kReturn[1].iValueType = VALUETYPE_INT;
	kReturn[1].pkValue    = &m_iSizeY;		
	
	
	return kReturn;	
}


bool P_Container::HandleSetValue( string kValueName ,string kValue )
{

	if(strcmp(kValueName.c_str(), "sizex") == 0) {
		m_iSizeX = atoi(kValue.c_str()) ;
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "sizey") == 0) {
		m_iSizeY = atoi(kValue.c_str()) ;
		return true;
	}

	SetSize(m_iSizeX,m_iSizeY);
	
	return false;
}

void P_Container::SetSize(int iX,int iY)
{
	m_iSizeX = iX;
	m_iSizeY = iY;

	m_kSlots.clear();
	
	for(int i = 0;i<iX*iY;i++)
		m_kSlots.push_back(-1);
}

bool P_Container::ItemTypeOK(int iType)
{
	if(m_kItemTypes.empty())
		return true;

	for(int i = 0;i<m_kItemTypes.size();i++)
		if(m_kItemTypes[i] == iType)
			return true;
			
	return false;
}


int* P_Container::GetItem(int iX,int iY)
{
	if(iX >= 0 && iX < m_iSizeX && iY >= 0 && iY < m_iSizeY)
		return &m_kSlots[iY*m_iSizeX + iX];
	else
		return NULL;
}

bool P_Container::SetItem(P_Item* pkItem,int iX,int iY,int iW,int iH)
{
	//check if this space is free
	for(int iYP = iY ;iYP < iY+iH;iYP++)
	{	
		for(int iXP = iX ;iXP< iX+iW;iXP++)
		{
			if(int* i = GetItem(iXP,iYP))
			{
				if(*i != -1)
					return false;
			}		
			else
				return false;				
		}
	}
	
	//get id
	int iID = pkItem->GetEntity()->GetEntityID();
	

	//now set spaces
	for(int iYP = iY ;iYP < iY+iH;iYP++)
	{	
		for(int iXP = iX ;iXP< iX+iW;iXP++)
		{
			int* i = GetItem(iXP,iYP);
			
			if(i)
				*i = iID;
			else
				return false;
		}
	}
	
	//update position saved in item
	pkItem->m_iInContainerPosX = iX;
	pkItem->m_iInContainerPosY = iY;		
	
	return true;
}

bool P_Container::HaveItem(int iID)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)
			if( *GetItem(iX,iY) == iID)
				return true;

	return false;
}

int P_Container::HaveItem(const string strItemName)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
	{
		for( int iX = 0;iX<m_iSizeX;iX++)
		{
			if(Entity* pkEnt = m_pkEntMan->GetEntityByID(*GetItem(iX,iY)))
			{
				if(P_Item* pkItem = (P_Item*)pkEnt->GetProperty("P_Item"))
				{
					if(pkItem->m_strName == strItemName)
						return *GetItem(iX,iY);				
				}
			}
		}
	}
		
	return -1;
}

void P_Container::ClearItem(int iID)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)
		{
			int* i = GetItem(iX,iY);
			
			if(*i == iID)
				*i = -1;
		}	
}

bool P_Container::GetItemPos(int iID,int& iRX,int& iRY)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)	
			if( *GetItem(iX,iY) == iID)
			{
				iRX = iX;
				iRY = iY;
				return true;
			}
			
	return false;
}

bool P_Container::AddItem(int iID,int iX,int iY)
{
	if(HaveItem(iID))
	{
		cout<<"Item already in container"<<endl;
		return false;
	}
		
	if(m_iMaxItems != 0)
		if(GetNrOfItems() >= m_iMaxItems)
		{
			//cout<<":"<<GetNrOfItems()<<endl;
			cout<<"max nr of items already in container"<<endl;
			return false;
		}
		
	if(Entity* pkItem = m_pkEntMan->GetEntityByID(iID))
	{
		if(pkItem == GetEntity())
		{
			cout<<"Tried to add self to self"<<endl;
			return false;
		}
		
		
		if(P_Item* pkPItem = (P_Item*)pkItem->GetProperty("P_Item"))
		{
			if(!ItemTypeOK(pkPItem->m_iType))
			{
				cout<<"Item type not allowed in this container"<<endl;
				return false;
			}

			if(!SetItem(pkPItem,iX,iY,pkPItem->m_iSizeX,pkPItem->m_iSizeY))		
			{
				cout<<"no space in container for an item of size: "<<pkPItem->m_iSizeX<<"x"<<pkPItem->m_iSizeY<<endl;
				return false;
			}
			
			pkItem->SetUseZones(false);				
			pkItem->SetParent(GetEntity());				
			
			
			if(m_strAttachToJoint.empty())
			{
				pkItem->SetUpdateStatus(UPDATE_NONE);									
				//this will also stop the entity from beeing sent to the client, therefore we tell the client to delete it
				m_pkEntMan->AddEntityToAllClientDeleteQueues(pkItem->GetEntityID());
			}
			else
			{
				pkItem->SetUpdateStatus(UPDATE_ALL);
			
				if(!pkItem->GetProperty("P_LinkToJoint"))
					pkItem->AddProperty("P_LinkToJoint");
			
				if(P_LinkToJoint* pkLTJ = (P_LinkToJoint*)pkItem->GetProperty("P_LinkToJoint"))
				{
					pkLTJ->SetLinkEntity(GetEntity()->GetParent()->GetEntityID());
					pkLTJ->SetJoint(m_strAttachToJoint);
				}					
			}
			
			//set item's owned by setting
			pkPItem->m_iInContainerID = GetEntity()->GetEntityID();
			//Print();
			
			return true;
		}
		else
			cout<<"WARNING: trying to add item that does not have any P_DMItem property"<<endl;
	}
	else
		cout<<"WARNING: trying to add item that does not exist"<<endl;			

	
	return false;
}

bool P_Container::AddItem(int iID)
{
	if(HaveItem(iID))
	{
		cout<<"Item already in container"<<endl;
		return false;
	}
		
	if(Entity* pkItem = m_pkEntMan->GetEntityByID(iID))
	{
		if(P_Item* pkPItem = (P_Item*)pkItem->GetProperty("P_Item"))
		{
			int iW = pkPItem->m_iSizeX;
			int iH = pkPItem->m_iSizeY;			
			
			//go trough all possible possitions
			for( int iCY = 0;iCY<m_iSizeY;iCY++)
			{
				for( int iCX = 0;iCX<m_iSizeX;iCX++)
				{
					bool bFree=true;
					//check if this space is free + W & H
					for(int iYP = iCY ;iYP < iCY+iH;iYP++)
					{	
						for(int iXP = iCX ;iXP< iCX+iW;iXP++)
						{
							if(int* i = GetItem(iXP,iYP))
							{
								if(*i != -1)
									bFree = false;
							}		
							else
								bFree = false;				
						}
					}	
					
					//looks like this space is free lets try to add the item here
					if(bFree)
					{
						return AddItem(iID,iCX,iCY);
					}
				}
			}
		}
	}
	
	cout<<"no free space"<<endl;
	
	return false;
}

bool P_Container::RemoveItem(int iID)
{
	if(!HaveItem(iID))
		return false;	

	ClearItem(iID);	
	m_pkEntMan->Delete(iID);
	
	return true;
}

bool P_Container::RemoveItem(int iX,int iY)
{
	int* i = GetItem(iX,iY);
	if(i)
		return RemoveItem(*i);
	
	return false;
}

bool P_Container::DropItem(int iID,const Vector3& kPos)
{
	if(Entity* pkOwner = GetEntity())
	{
		if(Entity* pkItem = m_pkEntMan->GetEntityByID(iID))
		{
			if(P_Item* pkPItem = (P_Item*)pkItem->GetProperty("P_Item"))
			{							
				if(HaveItem(iID))
				{
					ClearItem(iID);
	
					// remove link to joint
					pkItem->DeleteProperty("P_LinkToJoint");						
	
					// reset rotation
					pkItem->SetWorldRotV (Vector3(0,pkItem->GetWorldRotV().y,0));
					
					cout<<"enabling item"<<endl;
					pkItem->SetUpdateStatus(UPDATE_ALL);				
					pkItem->SetUseZones(true);
					
					pkPItem->m_iInContainerID = -1;
	
					pkItem->SetWorldPosV( kPos );
					

													
					return true;
				}
			}
		}
	}

	return false;
}

void P_Container::DropAll(const Vector3& kPos)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
	{
		for( int iX = 0;iX<m_iSizeX;iX++)
		{	
			int* i = GetItem(iX,iY);
		
			if(*i != -1)
			{
				DropItem(*i,kPos);
			}
		}
	}
}

bool P_Container::MoveItem(int iID,P_Container* pkDest,int iX,int iY)
{
	if(HaveItem(iID))
	{
		if(pkDest->AddItem(iID,iX,iY))
		{
			ClearItem(iID);
			return true;
		}
	}
	
	return false;
}

bool P_Container::MoveItem(int iID,P_Container* pkDest)
{
	if(HaveItem(iID))
	{
		if(pkDest->AddItem(iID))
		{
			ClearItem(iID);
			return true;
		}
	}
	
	return false;
}

bool P_Container::MoveItem(int iID,int iX,int iY)
{
	if(HaveItem(iID))
	{
		if(Entity* pkItem = m_pkEntMan->GetEntityByID(iID))
		{
			if(P_Item* pkPItem = (P_Item*)pkItem->GetProperty("P_Item"))
			{	
				int oldx,oldy;
				GetItemPos(iID,oldx,oldy);
		
				ClearItem(iID);
		
				if(SetItem(pkPItem,iX,iY,pkPItem->m_iSizeX,pkPItem->m_iSizeY))
				{
					return true;				
				}
				else
				{
					if(!SetItem(pkPItem,oldx,oldy,pkPItem->m_iSizeX,pkPItem->m_iSizeY))
						cout<<"ERROR: item's size has changed since added to container"<<endl;
				
					return false;
				}
			}
		}
	}
	
	return false;
}

void P_Container::GetItemList(vector<MLContainerInfo>* pkItemList)
{
	//make sure list is clean
	pkItemList->clear();
	
	set<int>	kItemIDs;

	//find all individual item ids
	for( int i = 0;i<m_kSlots.size();i++)
	{
		if(m_kSlots[i] != -1)
			kItemIDs.insert(m_kSlots[i]);
	}

	//printf("kItemIDs size = %i\n", kItemIDs.size());
	//printf("kItemIDs[0] = %i\n", (*kItemIDs.begin()));

	for(set<int>::iterator it=kItemIDs.begin();it!=kItemIDs.end();it++)
	{
		if(Entity* pkEnt = m_pkEntMan->GetEntityByID(*it))
		{		
			if(P_Item* pkItem = (P_Item*)pkEnt->GetProperty("P_Item"))
			{		
				int iX,iY;
				GetItemPos(*it,iX,iY);
					
				MLContainerInfo kTemp;
				kTemp.m_strName = pkItem->m_strName;
				kTemp.m_strIcon = pkItem->m_strIcon;				
				kTemp.m_iItemID = *it;
				kTemp.m_cItemX = iX;
				kTemp.m_cItemY = iY;
				kTemp.m_cItemW = pkItem->m_iSizeX;
				kTemp.m_cItemH = pkItem->m_iSizeY;
				kTemp.m_iStackSize = pkItem->m_iStackSize;
				
				
				pkItemList->push_back(kTemp);
			}
		}
	}
}

int P_Container::GetNrOfItems()
{
	set<int>	kItemIDs;

	//find all individual item ids
	for( int i = 0;i<m_kSlots.size();i++)
	{
		if(m_kSlots[i] != -1)
			kItemIDs.insert(m_kSlots[i]);
	}
	
	return kItemIDs.size();
}

void P_Container::Print()
{
	cout<<"Printing Inventory :"<<m_iSizeX<<" x "<<m_iSizeY<<endl;
	
	for( int iY = 0;iY<m_iSizeY;iY++)
	{
		for( int iX = 0;iX<m_iSizeX;iX++)
		{
			int* i = GetItem(iX,iY);
			
			cout<<setw(10)<<*i;
		}
		cout<<endl;
	}
}


void P_Container::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(&m_iMaxItems,sizeof(m_iMaxItems),1);
	
	pkPackage->Write(&m_iSizeX,sizeof(m_iSizeX),1);
	pkPackage->Write(&m_iSizeY,sizeof(m_iSizeY),1);
	
	pkPackage->Write(m_iContainerType);
	pkPackage->Write(m_bStaticOwner);	
	
	pkPackage->Write_Str(m_strAttachToJoint);	
	
	
	//save item types
	int iTypes = m_kItemTypes.size();
	pkPackage->Write(&iTypes,sizeof(iTypes),1);		
	for(int i = 0 ;i < iTypes;i++)
		pkPackage->Write(&m_kItemTypes[i],sizeof(m_kItemTypes[i]),1);	
	
}

void P_Container::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 3)
	{
		pkPackage->Read(&m_iMaxItems,sizeof(m_iMaxItems),1);	
		
		pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);
		pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);
	
		SetSize(m_iSizeX,m_iSizeY);
	
		pkPackage->Read(m_iContainerType);
		pkPackage->Read(m_bStaticOwner);
		pkPackage->Read_Str(m_strAttachToJoint);
	
		//load types
		int iTypes;
		pkPackage->Read(&iTypes,sizeof(iTypes),1);		
		m_kItemTypes.clear();
		for(int i = 0 ;i < iTypes;i++)
		{
			int iT;
			pkPackage->Read(&iT,sizeof(iT),1);				
			m_kItemTypes.push_back(iT);
		}				
	}
	else if(iVersion == 2)
	{
		pkPackage->Read(&m_iMaxItems,sizeof(m_iMaxItems),1);	
		
		pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);
		pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);
	
		SetSize(m_iSizeX,m_iSizeY);
	
		pkPackage->Read(m_iContainerType);
		pkPackage->Read(m_bStaticOwner);
	
		//load types
		int iTypes;
		pkPackage->Read(&iTypes,sizeof(iTypes),1);		
		m_kItemTypes.clear();
		for(int i = 0 ;i < iTypes;i++)
		{
			int iT;
			pkPackage->Read(&iT,sizeof(iT),1);				
			m_kItemTypes.push_back(iT);
		}	
	
	}
	else
	{
		pkPackage->Read(&m_iMaxItems,sizeof(m_iMaxItems),1);	
		
		pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);
		pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);
	
		SetSize(m_iSizeX,m_iSizeY);
	
		pkPackage->Read(m_iContainerType);
	
		//load types
		int iTypes;
		pkPackage->Read(&iTypes,sizeof(iTypes),1);		
		m_kItemTypes.clear();
		for(int i = 0 ;i < iTypes;i++)
		{
			int iT;
			pkPackage->Read(&iT,sizeof(iT),1);				
			m_kItemTypes.push_back(iT);
		}
	}	
}

void P_Container::FindMyItems()
{
	vector<Entity*>	kEntitys;
	GetEntity()->GetChilds(&kEntitys);

	//cout<<"parent has :"<<kEntitys.size()<<" childs"<<endl;
	
	for(int i = 0;i<kEntitys.size();i++)
	{
		if(P_Item* pkItem = (P_Item*)kEntitys[i]->GetProperty("P_Item"))
		{
			if(!HaveItem(kEntitys[i]->GetEntityID()))
			{		
				//check if item is a container if so run update on it
				if(P_Container* pkContainer = (P_Container*)kEntitys[i]->GetProperty("P_Container"))
				{
					pkContainer->Update();
				}
			
			
				if(!AddItem(kEntitys[i]->GetEntityID(),pkItem->m_iInContainerPosX,pkItem->m_iInContainerPosY))
				{
					cout<<"item did not find on its last known container position, adding it anywhere"<<endl;
					AddItem(kEntitys[i]->GetEntityID());					
				}
			}			
		}
	}
}









// SCRIPT INTERFACE FOR P_CharacterProperty
using namespace ObjectManagerLua;

namespace SI_P_Container
{
	int CreateItemInContainerLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		
		
		int iContainerID;
		double dTemp;
		char czItemName[128];
		
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iContainerID = (int)dTemp;
		
		g_pkScript->GetArgString(pkLua, 1,czItemName);
	
		//get cotnainer entity
		if(Entity* pkContainerEnt = g_pkObjMan->GetEntityByID(iContainerID))
		{
			//get container property
			if(P_Container* pkCP = (P_Container*)pkContainerEnt->GetProperty("P_Container"))
			{			
				//create new item
				if(Entity* pkEnt = g_pkObjMan->CreateEntityFromScript(czItemName))
				{
					//try adding it to the container
					if(pkCP->AddItem(pkEnt->GetEntityID()))
					{
						//cout<<"created "<<czItemName<<" in container "<<iContainerID<<endl;
						return 0;
					}
					else
					{
						cout<<"WARNING: could not create item "<<czItemName<<" in container "<<iContainerID<<endl;
						g_pkObjMan->Delete(pkEnt);
						return 0;
					}
					
				}
				else
					cout<<"WARNING: CreateItemInContainerLua could not create item "<<czItemName<<endl;
			}
		}
		
		return 0;
	}
}

Property* Create_P_Container()
{
	return new P_Container;
}


void Register_P_Container(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Container", Create_P_Container);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("CreateItemInContainer",	SI_P_Container::CreateItemInContainerLua);
	//g_pkScript->ExposeFunction("HaveItem",		SI_P_CharacterProperty::HaveItemLua);

}
