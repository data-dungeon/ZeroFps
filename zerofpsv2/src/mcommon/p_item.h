#ifndef _ENGINE_ITEMPROPERTY_H_
#define _ENGINE_ITEMPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "../zerofpsv2/basic/zfini.h"
#include <list>
   using namespace std;

/**	\brief	Da P_Item
		\ingroup Common
*/

enum MCOMMON_API eMLItemTypes
{
	MLITEM_DEFAULT = 		0,
	MLITEM_HELMET = 		1,
	MLITEM_BOOTS = 		2,
	MLITEM_LIGHT_ARMOR = 3,
	MLITEM_HEAVY_ARMOR = 4,	

};

class MCOMMON_API MLItemStats
{
public:
	int m_iValue;				 // vad föremålet kostar att köpa / sälja
	
	float	m_fArmourVal;
	float	m_fSpeedVal;
	float	m_iMaxLifeVal;
	float	m_fAim;

	MLItemStats()
	{
		m_fArmourVal = 0;
		m_fSpeedVal = 0;
		m_iMaxLifeVal = 0;
		m_fAim = 0;
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
	
		int		m_iInContainerID;					//last known container this item was in, if -1 it was not in any container
		int		m_iInContainerPosX;				//last known position in a container this item had
		int		m_iInContainerPosY;
// 		vector<PropertyValues> GetPropertyValues();
			
	public:
		MLItemStats m_kItemStats;
		
		P_Item();
		~P_Item();
		
		void Init();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		string 	GetIcon() { return m_strIcon; }
		int 		GetType() { return m_iType;	}
		string 	GetName() { return m_strName; }
		
		friend class MLContainer;
};

MCOMMON_API Property* Create_P_Item();

#endif





