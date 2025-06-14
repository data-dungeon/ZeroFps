#ifndef _P_DMITEM_H_
#define _P_DMITEM_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

enum MCOMMON_API eDMItemTypes
{
	DMITEM_DEFAULT = 0,
	DMITEM_HELMET = 1,
	DMITEM_GRENADE = 2,
	DMITEM_ARMOUR = 3,
	DMITEM_CLIP = 4,
	DMITEM_WEAPON = 5,
	DMITEM_IMPLANT = 6,	
	DMITEM_MEDKIT = 7,
};

class MCOMMON_API DMItemStats
{
public:
	float	m_fArmourVal;
	float	m_fSpeedVal;
	float	m_iMaxLifeVal;
	float	m_fAim;

	DMItemStats()
	{
		m_fArmourVal = 0;
		m_fSpeedVal = 0;
		m_iMaxLifeVal = 0;
		m_fAim = 0;
	}
};


class MCOMMON_API P_DMItem: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
		string	m_strName;
		string	m_strIcon;
		int		m_iSizeX;	
		int		m_iSizeY;
		int		m_iType;
	
	public:
		
		P_DMItem();
		~P_DMItem();

		DMItemStats m_kItemStats;

		int m_iValue; // vad f�rem�let kostar att k�pa / s�lja

		
		void Init();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		string GetIcon() { return m_strIcon; }
		int GetType() { return m_iType; }
		string GetName() { return m_strName; }
		

		
	friend class DMContainer;
};

MCOMMON_API Property* Create_P_DMItem();

#endif
