#include "dmcontainer.h"
#include <iomanip>

DMContainer::DMContainer(EntityManager* pkEntMan,int iOwnerID,int iX ,int iY )
{
	m_pkEntMan = pkEntMan;
	m_iOwnerID = iOwnerID;

	SetSize(iX,iY);
	
	if(Entity* pkOwner = m_pkEntMan->GetObjectByNetWorkID(m_iOwnerID) )	
	{
		pkOwner->SetUpdateStatus(UPDATE_NOCHILDS);
	}
	else
		cout<<"ERROR: container could not find its owner entity"<<endl;
		

	cout<<"Container created whit size:"<< iX <<" x "<<iY<<endl;
}

void DMContainer::SetSize(int iX,int iY)
{
	m_iSizeX = iX;
	m_iSizeY = iY;

	m_kSlots.clear();
	
	for(int i = 0;i<iX*iY;i++)
		m_kSlots.push_back(-1);
}

int* DMContainer::GetItem(int iX,int iY)
{
	if(iX >= 0 && iX < m_iSizeX && iY >= 0 && iY < m_iSizeY)
		return &m_kSlots[iY*m_iSizeX + iX];
	else
		return NULL;
}

bool DMContainer::SetItem(int iID,int iX,int iY,int iW,int iH)
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

bool DMContainer::HaveItem(int iID)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)
			if( *GetItem(iX,iY) == iID)
				return true;

	return false;
}

void DMContainer::ClearItem(int iID)
{
	for( int iY = 0;iY<m_iSizeY;iY++)
		for( int iX = 0;iX<m_iSizeX;iX++)
		{
			int* i = GetItem(iX,iY);
			
			if(*i == iID)
				*i = -1;
		}	
}

bool DMContainer::GetItemPos(int iID,int& iRX,int& iRY)
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

bool DMContainer::AddItem(int iID,int iX,int iY)
{
	if(Entity* pkOwner = m_pkEntMan->GetObjectByNetWorkID(m_iOwnerID))
	{
		if(Entity* pkItem = m_pkEntMan->GetObjectByNetWorkID(iID))
		{
			if(P_DMItem* pkPItem = (P_DMItem*)pkItem->GetProperty("P_DMItem"))
			{
				if(!SetItem(iID,iX,iY,pkPItem->m_iSizeX,pkPItem->m_iSizeY))		
				{
					cout<<"no space in container for an item of size: "<<pkPItem->m_iSizeX<<"x"<<pkPItem->m_iSizeY<<endl;
					return false;
				}
				
				pkItem->SetParent(pkOwner);				
				
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

bool DMContainer::AddItem(int iID)
{
	if(Entity* pkItem = m_pkEntMan->GetObjectByNetWorkID(iID))
	{
		if(P_DMItem* pkPItem = (P_DMItem*)pkItem->GetProperty("P_DMItem"))
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
	
	return false;
}

bool DMContainer::DropItem(int iID)
{
	if(Entity* pkOwner = m_pkEntMan->GetObjectByNetWorkID(m_iOwnerID))
	{
		if(Entity* pkItem = m_pkEntMan->GetObjectByNetWorkID(iID))
		{
			if(HaveItem(iID))
			{
				ClearItem(iID);
				
				pkItem->SetParent(pkOwner->GetParent());
				pkItem->SetWorldPosV( pkOwner->GetWorldPosV() );
				
				return true;
			}
		}
	}
	
	return false;
}

void DMContainer::DropAll()
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

bool DMContainer::MoveItem(int iID,DMContainer* pkDest,int iX,int iY)
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

bool DMContainer::MoveItem(int iID,DMContainer* pkDest)
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

void DMContainer::GetItemList(vector<ContainerInfo>* pkItemList)
{
	//make sure list is clean
	pkItemList->clear();
	
	set<int>	kItemIDs;

	//find all individual item ids
	for( int i = 0;i<m_kSlots.size();i++)
		kItemIDs.insert(m_kSlots[i]);

	printf("kItemIDs size = %i\n", kItemIDs.size());
	printf("kItemIDs[0] = %i\n", (*kItemIDs.begin()));

	for(set<int>::iterator it=kItemIDs.begin();it!=kItemIDs.end();it++)
	{
		if(Entity* pkEnt = m_pkEntMan->GetObjectByNetWorkID(*it))
		{		
			if(P_DMItem* pkItem = (P_DMItem*)pkEnt->GetProperty("P_DMItem"))
			{		
				int iX,iY;
				GetItemPos(*it,iX,iY);
					
				ContainerInfo kTemp;
				kTemp.m_strName = pkItem->m_strName;
				kTemp.m_strIcon = pkItem->m_strIcon;				
				kTemp.m_iItemID = *it;
				kTemp.m_iItemX = iX;
				kTemp.m_iItemY = iY;
				kTemp.m_iItemW = pkItem->m_iSizeX;
				kTemp.m_iItemH = pkItem->m_iSizeY;
				
				kTemp.m_iType = pkItem->m_iType;
				
				pkItemList->push_back(kTemp);
			}
		}
	}
}



void DMContainer::Print()
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


void DMContainer::Save(ZFIoInterface* pkPackage)
{
	
	pkPackage->Write(&m_iSizeX,sizeof(m_iSizeX),1);
	pkPackage->Write(&m_iSizeY,sizeof(m_iSizeY),1);

	int iSlots = m_kSlots.size();
	pkPackage->Write(&iSlots,sizeof(iSlots),1);	
	
	for(int i = 0 ;i < iSlots;i++)
		pkPackage->Write(&m_kSlots[i],sizeof(m_kSlots[i]),1);	

	//cout<<"saved DMContainer"<<endl;
}

void DMContainer::Load(ZFIoInterface* pkPackage)
{
	
	pkPackage->Read(&m_iSizeX,sizeof(m_iSizeX),1);
	pkPackage->Read(&m_iSizeY,sizeof(m_iSizeY),1);

	SetSize(m_iSizeX,m_iSizeY);
	
	int iSlots;
	pkPackage->Read(&iSlots,sizeof(iSlots),1);	
	
	for(int i = 0 ;i < iSlots;i++)
		pkPackage->Read(&m_kSlots[i],sizeof(m_kSlots[i]),1);	

	//cout<<"loaded DMContainer"<<endl;
}