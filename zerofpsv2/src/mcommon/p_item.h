#ifndef _ENGINE_ITEMPROPERTY_H_
#define _ENGINE_ITEMPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"

#include "p_characterproperty.h"

using namespace std;

/**	\brief	Da P_Item
		\ingroup Common
*/

enum MCOMMON_API eMLItemTypes
{
	MLITEM_MISC =	 	0,
	
	MLITEM_HEAD = 		1,
	MLITEM_BODY = 		2,
	MLITEM_HAND = 		3,
	MLITEM_RING =		4,
	MLITEM_NECKLACE = 5,
	MLITEM_BRACERS=	6,
	MLITEM_GLOVES = 	7,
	MLITEM_BELT = 		8,
	MLITEM_FEETS =		9,
	MLITEM_CAPE =		10,
};


class MCOMMON_API MLItemStats
{
public:
	int	m_iValue;				 // vad föremålet kostar att köpa / sälja
	
	//weapon
	string		m_strWeaponType;


	
	
	MLItemStats()
	{
	
	}
};


class MCOMMON_API P_Item: public Property 
{
	private:
		
		string	m_strName;
		string	m_strIcon;
		int		m_iSizeX;	
		int		m_iSizeY;
		int		m_iType;
		int		m_iStackSize;
		int		m_iStackMax;
		
		float		m_fWeight;
		int		m_iValue;
		
		//container
		int		m_iInContainerID;					//last known container this item was in, if -1 it was not in any container
		int		m_iInContainerPosX;				//last known position in a container this item had
		int		m_iInContainerPosY;
 		
		
		//buff
		string	m_strBuffName;
		int		m_iEquipedOnCharacterID;
		int		m_iBuffEntityID;
		
		//item info
		string	m_strInfo;
		string	m_strImage;
		
		
		vector<PropertyValues> GetPropertyValues();
			
	public:
		Stats		m_kItemStats;		

				
		P_Item();
		~P_Item();
		
		void Init();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		string 	GetIcon() 				{ return m_strIcon; 				}
		int 		GetType() 				{ return m_iType;					}
		string 	GetName() 				{ return m_strName; 				}
		int		GetInContainerID()	{ return m_iInContainerID;		}
		
		string	GetInfo()				{ return m_strInfo;				}
		string	GetImage()				{ return m_strImage;				}
		int		GetValue()				{ return m_iValue;				}
		float		GetWeight()				{ return m_fWeight;				}
		
		void		Equip(int iEntity);
		void		UnEquip();
		
		
		friend class MLContainer;
		friend class P_Container;
};

MCOMMON_API Property* Create_P_Item();
MCOMMON_API void Register_P_Item(ZeroFps* pkZeroFps);

#endif





