#include "container.h"
#include "containerproperty.h"

Container::Container()
{
	m_piGrid	=	NULL;
	
	SetSize(8,8);

}

int& Container::GetID(int iX,int iY)
{
	return m_piGrid[ (iY * m_iSizeX) + iX ];
}


void Container::Clean()
{
	m_kObjects.clear();
	
	for(int x=0;x<m_iSizeX;x++)
		for(int y=0;y<m_iSizeY;y++)
			GetID(x,y) = -1;

}


bool Container::CheckFreeSlot(int iX,int iY,int iSizeX,int iSizeY)
{
	for(int x=iX;x<(iX+iSizeX);x++)
	{
		for(int y=iY;y<(iY+iSizeY);y++)
		{
			if(x>=m_iSizeX || y>=m_iSizeY)
				return false;
			
			if(GetID(x,y) != -1)
				return false;
		}
	}
	return true;
}


void Container::SetID(int iX,int iY,int iSizeX,int iSizeY,int iID)
{
	for(int x=iX;x<(iX+iSizeX);x++)
		for(int y=iY;y<(iY+iSizeY);y++)
			if(x < m_iSizeX || y < m_iSizeY)
				GetID(x,y) = iID;
}


int Container::GetItemPos(Object* pkObject)
{
	for(int i=0;i<m_kObjects.size();i++)
		if(m_kObjects[i].pkObject == pkObject)
			return i;
			
	return -1;
}

Object* Container::GetItem(int iX,int iY)
{
	int id = GetID(iX,iY);
	
	if(id == -1)
		return NULL;

	return m_kObjects[id].pkObject;

}


void Container::SetSize(int iX,int iY)
{
	m_iSizeX = iX;
	m_iSizeY = iY;
	
	delete m_piGrid;
	
	m_piGrid = new int[iX * iY];
	
	Clean();
	
//	PrintContainer();
}


ItemProperty*	Container::GetItemProperty(Object* pkObject)
{
	return static_cast<ItemProperty*>(pkObject->GetProperty("ItemProperty"));
}


bool Container::CheckIfAdded(Object* pkObject)
{
	for(int i=0;i<m_kObjects.size();i++)
		if(m_kObjects[i].pkObject == pkObject)
			return true;
			
	return false;
}

bool Container::GetFreeSlot(int iSizeX,int iSizeY,int& iX,int&iY)
{
	for(int y=0;y<m_iSizeY;y++)
	{
		for(int x=0;x<m_iSizeX;x++)
		{
			if(CheckFreeSlot(x,y,iSizeX,iSizeY))
			{
				iX = x;
				iY = y;
				return true;
			}
		}
	}
	
	return false;
}


bool Container::AddItem(Object* pkObject)
{
	//dont add if already added
	if(CheckIfAdded(pkObject))
		return false;	

	ItemProperty* pkIP = GetItemProperty(pkObject);
	
	//dont add if object does not have any itemproperty
	if(pkIP == NULL)
		return false;
	
	//check if trying to put in self
	ContainerProperty* cp = static_cast<ContainerProperty*>(pkObject->GetProperty("ContainerProperty"));
	if(cp != NULL)
		if(&cp->m_kContainer == this)
			return false;
	
	int iX=0;	
	int iY=0;
	
	//get a free slot to put the item
	if(!GetFreeSlot(pkIP->m_iItemSizeX,pkIP->m_iItemSizeY,iX,iY))
		return false;
	
	//create data	
	ItemData kTemp;
	kTemp.pkObject = pkObject;
	kTemp.iX=iX;
	kTemp.iY=iY;
	
	//add item to item list
	m_kObjects.push_back(kTemp);

	//set slot to item ID
//	SetID(iX,iY,pkIP->m_iItemSizeX,pkIP->m_iItemSizeY,GetItemPos(pkObject));
	RebuildGrid();

//	PrintContainer();

	return true;
}

bool Container::AddItem(Object* pkObject,int iX,int iY)	
{
	//dont add if already added
	if(CheckIfAdded(pkObject))
		return false;	

	ItemProperty* pkIP = GetItemProperty(pkObject);
	
	//dont add if object does not have any itemproperty
	if(pkIP == NULL)
		return false;
	
	//check if trying to put in self
	ContainerProperty* cp = static_cast<ContainerProperty*>(pkObject->GetProperty("ContainerProperty"));
	if(cp != NULL)
		if(&cp->m_kContainer == this)
			return false;	
	
	
	//check if the slot iX,iY is free
	if(!CheckFreeSlot(iX,iY,pkIP->m_iItemSizeX,pkIP->m_iItemSizeY))
		return false;
	
	//create data
	ItemData kTemp;
	kTemp.pkObject = pkObject;
	kTemp.iX=iX;
	kTemp.iY=iY;
	
	//add item to item list
	m_kObjects.push_back(kTemp);

	//set slot iX,iY to ID
//	SetID(iX,iY,pkIP->m_iItemSizeX,pkIP->m_iItemSizeY,GetItemPos(pkObject));
	RebuildGrid();

	return true;
}


bool Container::RemoveItem(Object* pkObject)
{
	//check if added, return false if not
	if(!CheckIfAdded(pkObject))
		return false;

	int iID = GetItemPos(pkObject);
	vector<ItemData>::iterator kOit = m_kObjects.begin();
	
	//loop trough item list to find the right one
	for(vector<ItemData>::iterator it=m_kObjects.begin();it!=m_kObjects.end();it++)
	{
		if( (*it).pkObject == pkObject)
		{
			kOit = it;
			break;		
		}
	}
	
	if(kOit == m_kObjects.end())
	{
		cout<<" ERROR THIS SHULD NOT HAPEN"<<endl;
		return false;
	}
	
	//remove item from item list
	m_kObjects.erase(kOit);	
	
	RebuildGrid();
	
/*	
	//loop trough the grid and erase the item
	for(int x=0;x<m_iSizeX;x++)
	{
		for(int y=0;y<m_iSizeY;y++)
		{
			if(GetID(x,y) == iID)
				GetID(x,y) = -1;		
		}
	}
*/
	return true;
}


void Container::PrintContainer()
{
	cout<<"-= Container =- "<<m_iSizeX<<" * "<<m_iSizeY<<endl;

	for(int y=0;y<m_iSizeY;y++)
	{
		for(int x=0;x<m_iSizeX;x++)
		{
			if(GetID(x,y) == -1)
				cout<<"-";
			else
				cout<<GetID(x,y);
			
		}
		cout<<endl;
	}
}


GuiData Container::GetGuiData(int iNr)
{
	GuiData kTemp;
	
	Object* pkObject = m_kObjects[iNr].pkObject;
	
	kTemp.pkObject = pkObject;
	kTemp.sTextureName = GetItemProperty(pkObject)->m_kItemIcon;	
	kTemp.sItemName =  GetItemProperty(pkObject)->m_kItemName;	
	kTemp.iPosX = m_kObjects[iNr].iX;	
	kTemp.iPosY = m_kObjects[iNr].iY;	

	kTemp.iSizeX = GetItemProperty(pkObject)->m_iItemSizeX;
	kTemp.iSizeY = GetItemProperty(pkObject)->m_iItemSizeY;

	return kTemp;
}


void Container::RebuildGrid()
{
	for(int x=0;x<m_iSizeX;x++)
		for(int y=0;y<m_iSizeY;y++)
			GetID(x,y) = -1;


	for(int i=0;i<m_kObjects.size();i++)
	{
		ItemProperty* pkIP = GetItemProperty(m_kObjects[i].pkObject);
		SetID(m_kObjects[i].iX,m_kObjects[i].iY,pkIP->m_iItemSizeX,pkIP->m_iItemSizeY,i);					
	}

}
