#include "p_clientcontrol.h"
#include "p_container.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 
#include "../zerofpsv2/engine_systems/propertys/p_linktojoint.h" 

#include "p_item.h"

#include <iomanip>

// -----------------------------------------------------------------------------------------------

P_Container::P_Container()
{
	m_pkEntMan=			static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	
	strcpy(m_acName,"P_Container");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_bNetwork = 	false;
	m_iVersion = 	4;

	m_bFirstUpdate = true;
	
	
	m_iOwnerID 				= -1;
	m_bStaticOwner			= false;
	m_iMaxItems 			= 0;
	m_iContainerType		= eNormal;
	m_bEquip					= false;
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

bool P_Container::StackItem(P_Item* pkItem,int iX,int iY,int iCount)
{
	//set correct count
	if(iCount < 1)
	{
		iCount = pkItem->m_iStackSize;
	}


	int* i = GetItem(iX,iY);

	if(!i)
		return false;	
	
	//get target item
	if(P_Item* pkTargetItem = (P_Item*)m_pkEntMan->GetPropertyFromEntityID(*i,"P_Item"))
	{
		//check if stackable
		if(CanStack(pkTargetItem,pkItem))
		{
			cout<<"trying to stack"<<endl;
			
			//get free stack		
			int iFree = pkTargetItem->m_iStackMax - pkTargetItem->m_iStackSize;
			if(iFree <= 0)
			{
				cout<<"no free space"<<endl;
				return true;
			}
			
			//not enough space
			if(iCount > iFree)
			{
				cout<<"not enough free space, adding to full"<<endl;
				pkTargetItem->m_iStackSize += iFree;
				pkItem->m_iStackSize -= iFree;
			}
			else //enough space
			{
				pkTargetItem->m_iStackSize += iCount;
				pkItem->m_iStackSize -= iCount;					
			}
			
			//check if item is totaly moved
			if(pkItem->m_iStackSize <= 0)
			{
				//remove from world
				//in container?
				if(pkItem->m_iInContainerID != -1)
				{
					//get container and remove item
					if(P_Container* pkContainer = (P_Container*)m_pkEntMan->GetPropertyFromEntityID(pkItem->m_iInContainerID,"P_Container"))
					{
						pkContainer->RemoveItem(pkItem->GetEntity()->GetEntityID());
					}
				}
				else
				{
					//not in a container, just remove it from the face of the earth
					m_pkEntMan->Delete(pkItem->GetEntity());
				}
			}
			
			cout<<"successfull stacking"<<endl;
			
			//have now handled the stacking
			return true;
		}
	}
		
	return false;
}

bool P_Container::SetItem(P_Item* pkItem,int iX,int iY)
{
	int iW = pkItem->m_iSizeX;
	int iH = pkItem->m_iSizeY;

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

bool P_Container::AddMove(int iID,int iX,int iY,int iCount)
{
	P_Item* pkItem = NULL;
		
	//get item
	if(!(pkItem = (P_Item*)m_pkEntMan->GetPropertyFromEntityID(iID,"P_Item")))
	{		
		cout<<"WARNING: entity "<<iID<<" does not have P_Item"<<endl;
		return false;
	}
	
	//check if *this is the item
	if(GetEntity() == pkItem->GetEntity())
		return false;
	
	
	//place on free slot?
	if(iX == -1)
	{
		cout<<"moving to freepos ,or free stack"<<endl;
		
		int iTargetPosX = -1;
		int iTargetPosY = -1;
								
		//try to find a place to put item
		if(!FindFreePos(pkItem,iTargetPosX,iTargetPosY))
			return false;
		
		if(AddItemAtPos(pkItem,iTargetPosX,iTargetPosY,iCount))
			return true;
	}
	else
	{
		cout<<"moving to position "<<iX<<" "<<iY<<endl;
		if(AddItemAtPos(pkItem,iX,iY,iCount))
			return true;			
	}

	
	return false;
}

bool P_Container::AddItemAtPos(P_Item* pkItem,int iX,int iY,int iCount)
{
	Entity* pkItemEnt = pkItem->GetEntity();

	//stack and return if possible
	if(StackItem(pkItem,iX,iY,iCount))
		return true;			

	//no stacking, lets add item here	
	if(!IsFree(iX,iY,pkItem->m_iSizeX,pkItem->m_iSizeY,pkItemEnt->GetEntityID()))
	{
		cout<<"position not free"<<endl;
		return false;
	}
	
	if(!ItemTypeOK(pkItem->m_iType))
	{
		cout<<"type not allowed in container"<<endl;
		return false;
	}
	
	cout<<"setting item to new location"<<endl;
		
	//get current container, if any, and clear item from its current position
	if(P_Container* pkContainer = (P_Container*)m_pkEntMan->GetPropertyFromEntityID(pkItem->m_iInContainerID,"P_Container"))
	{
		pkContainer->ClearItem(pkItemEnt->GetEntityID());
	}
	
	//set item on its new position
	SetItem(pkItem,iX,iY);
	
		
	//setup item ----------------------------------
	
	//remove old  stuff
	pkItem->UnEquip();
	
	
	//set item's owned by setting
	pkItem->m_iInContainerID = GetEntity()->GetEntityID();	
	
	//rebind entity
	pkItemEnt->SetUseZones(false);				
	pkItemEnt->SetParent(GetEntity());				
	
	
	//shuld items in this container be eqiped
	if(m_bEquip)
	{
		pkItem->Equip(m_iOwnerID);
	}
	
	//shuld item be linked to joint?
	if(m_strAttachToJoint.empty())
	{
		//remove link to joint if any
		pkItemEnt->DeleteProperty("P_LinkToJoint");	
		
		//disable updates
		pkItemEnt->SetUpdateStatus(UPDATE_NONE);									
		
		//this will also stop the entity from beeing sent to the client, therefore we tell the client to delete it
		m_pkEntMan->AddEntityToAllClientDeleteQueues(pkItemEnt->GetEntityID());
	}
	else
	{
		//enable updates
		pkItemEnt->SetUpdateStatus(UPDATE_ALL);
	
		//setup link to joint
		if(!pkItemEnt->GetProperty("P_LinkToJoint"))
			pkItemEnt->AddProperty("P_LinkToJoint");
	
		if(P_LinkToJoint* pkLTJ = (P_LinkToJoint*)pkItemEnt->GetProperty("P_LinkToJoint"))
		{
			pkLTJ->SetLinkEntity(GetEntity()->GetParent()->GetEntityID());
			pkLTJ->SetJoint(m_strAttachToJoint);
		}					
	}

	
	//set container owner setting
	if(P_Container* pkCon = (P_Container*)pkItemEnt->GetProperty("P_Container"))
	{
		pkCon->SetOwnerID(m_iOwnerID);
	}
	
	
	return true;
}

bool P_Container::IsFree(int iX,int iY,int iW,int iH,int iSelfID)
{
	for(int y = iY ;y < iY+iH;y++)
	{	
		for(int x = iX ;x< iX+iW;x++)
		{
			if(int* i = GetItem(x,y))
			{
				if( (*i != -1) && (*i != iSelfID) )
					return false;
			}
			else
				return false;
		}
	}
	
	return true;
}	
	


bool P_Container::FindFreePos(P_Item* pkItem,int& iX,int& iY)
{	
	int iW = pkItem->m_iSizeX;
	int iH = pkItem->m_iSizeY;			
	bool bStackable = false;	
	
	if(pkItem->m_iStackMax > 1)
		bStackable = true;
	
	
	//frist try to find a stackable position	
	if(bStackable)
	{
		set<int>	kItemIDs;
	
		//find all individual item ids
		for( int i = 0;i<m_kSlots.size();i++)
		{
			if(m_kSlots[i] != -1)
				kItemIDs.insert(m_kSlots[i]);
		}
		
		for(set<int>::iterator it=kItemIDs.begin();it!=kItemIDs.end();it++)
		{
			if(P_Item* pkSlotItem = (P_Item*)m_pkEntMan->GetPropertyFromEntityID(*it,"P_Item"))
			{
				if(CanStack(pkSlotItem,pkItem))
				{
					iX = pkSlotItem->m_iInContainerPosX;
					iY = pkSlotItem->m_iInContainerPosY;
					return true;
				}		
			}				
		}
	}
		
			
	//go trough all possible possitions
	int iSelfID = pkItem->GetEntity()->GetEntityID();
	for( int iCY = 0;iCY<m_iSizeY;iCY++)
	{
		for( int iCX = 0;iCX<m_iSizeX;iCX++)
		{
			if(IsFree(iCX,iCY,iW,iH,iSelfID))
			{
				iX = iCX;
				iY = iCY;
				return true;				
			}		
		}
	}
		

	return false;
}


bool P_Container::CanStack(P_Item* pkTarget,P_Item* pkItem)
{
	if(pkTarget->m_iStackMax > 1)
		if(pkTarget->m_strName == pkItem->m_strName)
			if(pkTarget->m_iStackMax - pkTarget->m_iStackSize > 0)
				return true;
	
	
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
				kTemp.m_cItemType = pkItem->m_iType;
				kTemp.m_iStackSize = pkItem->m_iStackSize;
				
				//check if its a container
				if(pkEnt->GetProperty("P_Container"))
					kTemp.m_bIsContainer = true;
				else
					kTemp.m_bIsContainer = false;
				
				
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
	pkPackage->Write(m_bEquip);	
	
	pkPackage->Write_Str(m_strAttachToJoint);	
	
	
	//save item types
	int iTypes = m_kItemTypes.size();
	pkPackage->Write(&iTypes,sizeof(iTypes),1);		
	for(int i = 0 ;i < iTypes;i++)
		pkPackage->Write(&m_kItemTypes[i],sizeof(m_kItemTypes[i]),1);	
	
}

void P_Container::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 4)
	{
		pkPackage->Read(&m_iMaxItems,sizeof(m_iMaxItems),1);	
		
		pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);
		pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);
	
		SetSize(m_iSizeX,m_iSizeY);
	
		pkPackage->Read(m_iContainerType);
		pkPackage->Read(m_bStaticOwner);
		pkPackage->Read(m_bEquip);
		
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
	else if(iVersion == 3)
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
			
			
				if(!AddMove(kEntitys[i]->GetEntityID(),pkItem->m_iInContainerPosX,pkItem->m_iInContainerPosY,-1))
				{
					cout<<"item did not find on its last known container position, adding it anywhere"<<endl;
					AddMove(kEntitys[i]->GetEntityID(),-1,-1,-1);									
				}
				
				/*	
					if(!AddItem(kEntitys[i]->GetEntityID(),pkItem->m_iInContainerPosX,pkItem->m_iInContainerPosY))
					{
						cout<<"item did not find on its last known container position, adding it anywhere"<<endl;
						AddItem(kEntitys[i]->GetEntityID());					
				}*/
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
					if(pkCP->AddMove(pkEnt->GetEntityID(),-1,-1,-1))
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
