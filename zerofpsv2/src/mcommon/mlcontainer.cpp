#include "../zerofpsv2/engine/entitymanager.h"
#include "mlcontainer.h"
#include <iomanip>

MLContainer::MLContainer(EntityManager* pkEntMan,int iOwnerID,int iX ,int iY ,bool bDisable,int iContainerID )
{
	m_pkEntMan				= pkEntMan;
	m_iOwnerID 				= iOwnerID;
	m_bDisableItems 		= true;
	m_iMaxItems 			= 0;
	m_iContainerID			= iContainerID;
	m_iContainerType		= eNormal;
	m_kItemTypes.clear();
	
	SetSize(iX,iY);
	
	if(Entity* pkOwner = m_pkEntMan->GetEntityByID(m_iOwnerID) )	
	{
		//pkOwner->SetUpdateStatus(UPDATE_NOCHILDS);
	}
	else
		cout<<"ERROR: container could not find its owner entity"<<endl;
		

	//cout<<"Container created with size:"<< iX <<" x "<<iY<<endl;
}

void MLContainer::SetSize(int iX,int iY)
{
	m_iSizeX = iX;
	m_iSizeY = iY;

	m_kSlots.clear();
	
	for(int i = 0;i<iX*iY;i++)
		m_kSlots.push_back(-1);
}

bool MLContainer::ItemTypeOK(int iType)
{
	if(m_kItemTypes.empty())
		return true;

	for(int i = 0;i<m_kItemTypes.size();i++)
		if(m_kItemTypes[i] == iType)
			return true;
			
	return false;
}


int* MLContainer::GetItem(int iX,int iY)
{
	if(iX >= 0 && iX < m_iSizeX && iY >= 0 && iY < m_iSizeY)
		return &m_kSlots[iY*m_iSizeX + iX];
	else
		return NULL;
}

bool MLContainer::SetItem(int iID,int iX,int iY,int iW,int iH)
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
	
	return true;
}

bool MLContainer::HaveItem(int iID)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)
			if( *GetItem(iX,iY) == iID)
				return true;

	return false;
}

int MLContainer::HaveItem(const string strItemName)
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

void MLContainer::ClearItem(int iID)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)
		{
			int* i = GetItem(iX,iY);
			
			if(*i == iID)
				*i = -1;
		}	
}

bool MLContainer::GetItemPos(int iID,int& iRX,int& iRY)
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

bool MLContainer::AddItem(int iID,int iX,int iY)
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
		
	if(Entity* pkOwner = m_pkEntMan->GetEntityByID(m_iOwnerID))
	{
		if(Entity* pkItem = m_pkEntMan->GetEntityByID(iID))
		{
			if(P_Item* pkPItem = (P_Item*)pkItem->GetProperty("P_Item"))
			{
				if(!ItemTypeOK(pkPItem->m_iType))
				{
					cout<<"Item type not allowed in this container"<<endl;
					return false;
				}

				if(!SetItem(iID,iX,iY,pkPItem->m_iSizeX,pkPItem->m_iSizeY))		
				{
					cout<<"no space in container for an item of size: "<<pkPItem->m_iSizeX<<"x"<<pkPItem->m_iSizeY<<endl;
					return false;
				}
				
				pkItem->SetParent(pkOwner);				
				pkItem->SetUseZones(false);
				
				if(m_bDisableItems)				
				{
					pkItem->SetUpdateStatus(UPDATE_NONE);
										
					cout<<"disabling item"<<endl;
					//this will also stop the entity from beeing sent to the client, therefore we tell the client to delete it
					m_pkEntMan->AddEntityToAllClientDeleteQueues(pkItem->GetEntityID());
				}
				else				
				{
					pkItem->SetUpdateStatus(UPDATE_ALL);
					cout<<"enabling item"<<endl;
				}

				
				//set item's owned by setting
				pkPItem->m_iInContainerID = m_iContainerID;
				pkPItem->m_iInContainerPosX = iX;
				pkPItem->m_iInContainerPosY = iY;
				
				//Print();
				
				return true;
			}
			else
				cout<<"WARNING: trying to add item that does not have any P_DMItem property"<<endl;
		}
		else
			cout<<"WARNING: trying to add item that does not exist"<<endl;			
	}
	else
		cout<<"WARNING: could not find owner entity"<<endl;

	
	return false;
}

bool MLContainer::AddItem(int iID)
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

bool MLContainer::RemoveItem(int iID)
{
	if(!HaveItem(iID))
		return false;	

	ClearItem(iID);	
	m_pkEntMan->Delete(iID);
	
	return true;
}

bool MLContainer::RemoveItem(int iX,int iY)
{
	int* i = GetItem(iX,iY);
	if(i)
		return RemoveItem(*i);
	
	return false;
}

bool MLContainer::DropItem(int iID)
{
	if(Entity* pkOwner = m_pkEntMan->GetEntityByID(m_iOwnerID))
	{
		if(Entity* pkItem = m_pkEntMan->GetEntityByID(iID))
		{
			if(P_Item* pkPItem = (P_Item*)pkItem->GetProperty("P_Item"))
			{							
				if(HaveItem(iID))
				{
					ClearItem(iID);
	
					// check for joint stuff...
					//if ( pkItem->GetProperty("P_LinkToJoint") )
					//	pkItem->RemoveProperty (pkItem->GetProperty("P_LinkToJoint"));
					pkItem->DeleteProperty("P_LinkToJoint");
						
	
					// reset rotation
					pkItem->SetWorldRotV (Vector3(0,pkItem->GetWorldRotV().y,0));
					
					cout<<"enabling item"<<endl;
					pkItem->SetUpdateStatus(UPDATE_ALL);				
					pkItem->SetUseZones(true);
					pkItem->SetParent(pkOwner->GetParent());				
					
	
					pkItem->SetWorldPosV( Vector3( pkOwner->GetWorldPosV().x + ((rand()%200)-100)/100.f,
													pkOwner->GetWorldPosV().y,
													pkOwner->GetWorldPosV().z + ((rand()%200)-100)/100.f) );
					
					pkPItem->m_iInContainerID = -1;
													
					return true;
				}
			}
		}
	}

	return false;
}

void MLContainer::DropAll()
{
	for( int iY = 0;iY<m_iSizeY;iY++)
	{
		for( int iX = 0;iX<m_iSizeX;iX++)
		{	
			int* i = GetItem(iX,iY);
		
			if(*i != -1)
			{
				DropItem(*i);
			}
		}
	}
}

bool MLContainer::MoveItem(int iID,MLContainer* pkDest,int iX,int iY)
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

bool MLContainer::MoveItem(int iID,MLContainer* pkDest)
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

bool MLContainer::MoveItem(int iID,int iX,int iY)
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
		
				if(SetItem(iID,iX,iY,pkPItem->m_iSizeX,pkPItem->m_iSizeY))
				{
					return true;				
				}
				else
				{
					if(!SetItem(iID,oldx,oldy,pkPItem->m_iSizeX,pkPItem->m_iSizeY))
						cout<<"ERROR: item's size has changed since added to container"<<endl;
				
					return false;
				}
			}
		}
	}
	
	return false;
}

void MLContainer::GetItemList(vector<MLContainerInfo>* pkItemList)
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

int MLContainer::GetNrOfItems()
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

void MLContainer::Print()
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


void MLContainer::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(&m_iMaxItems,sizeof(m_iMaxItems),1);
	pkPackage->Write(&m_bDisableItems,sizeof(m_bDisableItems),1);
	
	pkPackage->Write(&m_iSizeX,sizeof(m_iSizeX),1);
	pkPackage->Write(&m_iSizeY,sizeof(m_iSizeY),1);
	
	pkPackage->Write(m_iContainerID);
	
	
/*	
	//save slots
	int iSlots = m_kSlots.size();
	pkPackage->Write(&iSlots,sizeof(iSlots),1);		
	for(int i = 0 ;i < iSlots;i++)
		pkPackage->Write(&m_kSlots[i],sizeof(m_kSlots[i]),1);	
*/
		
	//save item types
	int iTypes = m_kItemTypes.size();
	pkPackage->Write(&iTypes,sizeof(iTypes),1);		
	for(int i = 0 ;i < iTypes;i++)
		pkPackage->Write(&m_kItemTypes[i],sizeof(m_kItemTypes[i]),1);	
	
}

void MLContainer::Load(ZFIoInterface* pkPackage)
{
	
	pkPackage->Read(&m_iMaxItems,sizeof(m_iMaxItems),1);	
	pkPackage->Read(&m_bDisableItems,sizeof(m_bDisableItems),1);	
	
	pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);
	pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);

	SetSize(m_iSizeX,m_iSizeY);

	pkPackage->Read(m_iContainerID);
		
/*	
	//load slots
	int iSlots;
	pkPackage->Read(&iSlots,sizeof(iSlots),1);		
	for(int i = 0 ;i < iSlots;i++)
		pkPackage->Read(&m_kSlots[i],sizeof(m_kSlots[i]),1);	
*/

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

void MLContainer::FindMyItems()
{
	if(Entity* pkOwner = m_pkEntMan->GetEntityByID( m_iOwnerID ))
	{
	
		vector<Entity*>	kEntitys;
		pkOwner->GetChilds(&kEntitys);
	
		//cout<<"parent has :"<<kEntitys.size()<<" childs"<<endl;
		
		for(int i = 0;i<kEntitys.size();i++)
		{
			if(P_Item* pkItem = (P_Item*)kEntitys[i]->GetProperty("P_Item"))
			{
				//cout<<"found item"<<endl;
				
				if(pkItem->m_iInContainerID == m_iContainerID)
				{
					//cout<<"its mine =D"<<endl;
					if(!AddItem(kEntitys[i]->GetEntityID(),pkItem->m_iInContainerPosX,pkItem->m_iInContainerPosY))
					{
						cout<<"item did not find on its last known container position, adding it anywhere"<<endl;
						AddItem(kEntitys[i]->GetEntityID());
					}
				}
				//else
				//	cout<<"its not mine :("<<endl;
			}
		}
	}
	else
		cout<<"ERROR: could not find parent"<<endl;
}





