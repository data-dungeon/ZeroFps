#ifndef _ENGINE_P_CONTAINER_H_
#define _ENGINE_P_CONTAINER_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include <vector>

class P_Item;

using namespace std;

enum MCOMMON_API ContainerTypes
{
	eNormal,
	
	eInventory,
	eLeftHand,
	eRightHand,
	eHead,
	eBody,
	eGloves,
	eCape,
	eNecklace,
	eBracers,
	eBelt,
	eFeets,
	eRing1,
	eRing2,
	eRing3,
	eRing4,
	
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
		bool	m_bIsContainer;
};

/**	\brief	Da P_Container
		\ingroup Common
*/
class MCOMMON_API P_Container: public Property 
{
	private:
		EntityManager*	m_pkEntMan;
	
		vector<int>		m_kSlots;				//slots
		vector<int>		m_kItemTypes;			//contains item types that can be put in this container, empty = all 
				
		int				m_iSizeX;				//width
		int				m_iSizeY;				//height
		int				m_iMaxItems;			//max number of items that can fit in container
		int				m_iContainerType;		//type of container
		int				m_iOwnerID;				//wich character, if any, owns this container
		bool				m_bStaticOwner;		//shuld owner change?
		string			m_strAttachToJoint;	//want items in container to be attached to any coint on the container owner
		bool				m_bFirstUpdate;		//first update, hack to load items
		
				
		bool StackItem(P_Item* pkItem,int iX,int iY,int iCount);		
		bool SetItem(P_Item* pkItem,int iX,int iY);
		void ClearItem(int iID);			
		bool IsFree(int iX,int iY,int iW,int iH,int iSelfID = -1);
		bool AddItemAtPos(P_Item* pkItem,int iX,int iY,int iCount);
		
		bool HaveItem(int iID);
		bool GetItemPos(int iID,int& iRX,int& iRY);
		bool ItemTypeOK(int iType);				
	
		bool FindFreePos(P_Item* pkItem,int& iX,int& iY);		
		bool CanStack(P_Item* pkTarget,P_Item* pkItem);
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
		
	public:		
		P_Container();		
		~P_Container();
		
		void Init();			
		void Update();
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);			
		
		void Print();				
		void FindMyItems();					//must be called after loading, and after childs has been created
		
		//set/get's
		void SetSize(int iX,int iY);
		char GetSizeX()								{return char(m_iSizeX);};
		char GetSizeY()								{return char(m_iSizeY);};
		int  GetOwnerID()								{return m_iOwnerID;};
		void SetOwnerID(int iOwner)				{m_iOwnerID = iOwner;};								
		void SetStaticOwner(bool bStatic)		{m_bStaticOwner = bStatic;};
		void SetJoint(const string& strBone)	{m_strAttachToJoint = strBone;};
		string GetJoint()								{return m_strAttachToJoint;};
		void SetMaxItems(int iItems) 				{m_iMaxItems = iItems;}; 
		void SetContainerType(int iType)			{m_iContainerType = iType;};
		int  GetContainerType()						{return m_iContainerType;};
						
		//type management
		void AddItemType(int iType) 				{m_kItemTypes.push_back(iType);};
		void ClearItemTypes() 						{m_kItemTypes.clear();};				
		
		//item management
		bool AddMove(int iID,int iX,int iY,int iCount);
		bool DropItem(int iID,const Vector3& kPos);		
		bool RemoveItem(int iID);
		
		int HaveItem(const string strItemName);		
				
		void 				GetItemList(vector<MLContainerInfo>* pkItemList);
		vector<int>* 	GetItemTypes ()		{ return &m_kItemTypes; }
		int 				GetNrOfItems();
		int* 				GetItem(int iX,int iY); // Zeb: Flytta denna så den blev public..EEEVIL
		

};

MCOMMON_API Property* Create_P_Container();
MCOMMON_API void Register_P_Container(ZeroFps* pkZeroFps);

#endif










