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

enum MCOMMON_API DMItemTypes
{
	DMItemDefault = 0,
	DMItemHelmet = 1,
	DMItemGrenade = 2,
	DMItemAmor = 3,
	DMItemClip = 4,
	DMItemWeapon = 5,
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

		void CloneOf(Property* pkProperty) { }
		
		void Init();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		
	friend class DMContainer;
};

MCOMMON_API Property* Create_P_DMItem();

#endif








