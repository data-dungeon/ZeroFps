#ifndef _MLCONTAINER_H_
#define _MLCONTAINER_H_

#include <vector>
#include "p_item.h"
#include "mcommon_x.h"

using namespace std;


enum MCOMMON_API ContainerTypes
{
	eNormal,
	eInventory,
	eLeftHand,
	eRightHand,
	eHead,
	eBody,
};


class MCOMMON_API MLContainerInfo
{
	public:
		string	m_strName;
		string	m_strIcon;
			
		int	m_iItemID;		
		char	m_cItemX;
		char	m_cItemY;		
		char	m_cItemW;
		char	m_cItemH;
		
		int	m_iStackSize;
};


class MCOMMON_API MLContainer
{
	private:
		EntityManager*	m_pkEntMan;
	
		vector<int>	m_kSlots;				//slots
		int	m_iSizeX;						//width
		int	m_iSizeY;						//height
		int	m_iOwnerID;						//owner entity ID, (owner is gona be the parent for all item entitys)

		bool				m_bDisableItems;
		vector<int>		m_kItemTypes;		//contains item types that can be put in this container, empty = all 
		int				m_iMaxItems;
		
		int				m_iContainerType;

		bool SetItem(P_Item* pkItem,int iX,int iY,int iW,int iH);
		bool HaveItem(int iID);
		bool GetItemPos(int iID,int& iRX,int& iRY);
		void ClearItem(int iID);
	
		bool ItemTypeOK(int iType);

		
	public:

		MLContainer(EntityManager* pkEntMan,int iOwnerID,int iX = 4,int iY = 4,bool bDisable = true);		
		
		void SetSize(int iX,int iY);

		int GetOwner()									{return m_iOwnerID;};
						
		void SetDisableItems(bool bDisable) 	{m_bDisableItems = bDisable;};
		bool GetDisableItems() 						{return m_bDisableItems;};
		
		void AddItemType(int iType) 				{m_kItemTypes.push_back(iType);};
		void ClearItemTypes() 						{m_kItemTypes.clear();};
		
		void SetMaxItems(int iItems) 				{m_iMaxItems = iItems;}; 

		void SetContainerType(int iType)			{m_iContainerType = iType;};
		int  GetContainerType()						{return m_iContainerType;};
		
		char GetSizeX()								{return char(m_iSizeX);};
		char GetSizeY()								{return char(m_iSizeY);};
		
		bool MoveItem(int iID,int iX,int iY);
		bool MoveItem(int iID,MLContainer* pkDest,int iX,int iY);
		bool MoveItem(int iID,MLContainer* pkDest);
		
		bool AddItem(int iID,int iX,int iY);
		bool AddItem(int iID);
		bool DropItem(int iID,const Vector3& kPos);
		void DropAll(const Vector3& kPos);		
		
		int HaveItem(const string strItemName);		
		
		bool RemoveItem(int iID);
		bool RemoveItem(int iX,int iY);
		
		void 				GetItemList(vector<MLContainerInfo>* pkItemList);
		vector<int>* 	GetItemTypes ()		{ return &m_kItemTypes; }
		int 				GetNrOfItems();
		int 				GetOwnerID()			{ return m_iOwnerID; }
		int* 				GetItem(int iX,int iY); // Zeb: Flytta denna så den blev public..EEEVIL

		void FindMyItems();					//must be called after loading, and after childs has been created
		
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		void Print();

};



#endif