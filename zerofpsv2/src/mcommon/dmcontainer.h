#ifndef _DMCONTAINER_H_
#define _DMCONTAINER_H_

#include <vector>
#include "p_dmitem.h"
#include "mcommon_x.h"

using namespace std;


class MCOMMON_API ContainerInfo
{
	public:
		string	m_strName;
		string	m_strIcon;
	
		int	m_iItemID;		
		int	m_iItemX;
		int	m_iItemY;		
		int	m_iItemW;
		int	m_iItemH;
		
		int	m_iType;
};


class MCOMMON_API DMContainer
{
	private:
		EntityManager*	m_pkEntMan;
	
		vector<int>	m_kSlots;				//slots
		int	m_iSizeX;						//width
		int	m_iSizeY;						//height
		int	m_iOwnerID;						//owner entity ID, (owner is gona be the parent for all item entitys)

		bool				m_bDisableItems;
		vector<int>		m_kItemTypes;		//contains item types that can be put in this container, empty = all 


		void SetSize(int iX,int iY);
		bool SetItem(int iID,int iX,int iY,int iW,int iH);
		bool HaveItem(int iID);
		bool GetItemPos(int iID,int& iRX,int& iRY);
		void ClearItem(int iID);
	
		bool ItemTypeOK(int iType);
	
	public:
		
		DMContainer(EntityManager* pkEntMan,int iOwnerID,int iX = 4,int iY = 4,bool bDisable = true);		
		
		int GetOwnerID()			{ return m_iOwnerID; }
		
		void SetDisableItems(bool bDisable) {m_bDisableItems = bDisable;};
		bool GetDisableItems() {return m_bDisableItems;};
		
		void AddItemType(int iType) {m_kItemTypes.push_back(iType);};
		void ClearItemTypes() {m_kItemTypes.clear();};
		
		int* GetItem(int iX,int iY); // Zeb: Flytta denna så den blev public..

		bool MoveItem(int iID,int iX,int iY);
		bool MoveItem(int iID,DMContainer* pkDest,int iX,int iY);
		bool MoveItem(int iID,DMContainer* pkDest);
		
		bool AddItem(int iID,int iX,int iY);
		bool AddItem(int iID);
		bool DropItem(int iID);
		void DropAll();		
		
		void GetItemList(vector<ContainerInfo>* pkItemList);
		void Print();


		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
};



#endif











