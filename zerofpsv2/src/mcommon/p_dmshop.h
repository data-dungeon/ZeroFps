#ifndef _P_DMSHOP_H_
#define _P_DMSHOP_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

#include "dmcontainer.h"

using namespace std;

class MCOMMON_API P_DMShop: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
		map<int, int> m_kPricemap; // itemID, price
		
	public:
		
		P_DMShop();
		~P_DMShop();

		void CloneOf(Property* pkProperty) { }
		void Init();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		DMContainer* m_pkItems;		

		int GetPrice(int x, int y);
		bool SetPrice(int x, int y, int iPrice);
};

MCOMMON_API Property* Create_P_DMShop();

#endif // _P_DMSHOP_H_
