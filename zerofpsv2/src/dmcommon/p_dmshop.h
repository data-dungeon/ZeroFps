#ifndef _P_DMSHOP_H_
#define _P_DMSHOP_H_

#include "../zerofpsv2/engine/property.h"
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
		map<string, int> m_kPricemap; // name, price

		EntityManager* m_pkEntityManager;

		float m_fTradeRate; // 1.0f = samma pris som varan kostar, <1 = billigare, >1 = dyrare
		string m_strShopName;
		
	public:
		
		P_DMShop();
		~P_DMShop();

		void Init();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		DMContainer* m_pkItems;		

		int GetSellPrice(int iObjectID); // fr�ga aff�ren vad ett f�rem�l i dess "m_pkItems" container kostar.
		bool SetPrice(string strName, int iPrice); // best�m vad ett f�rem�l i aff�ren skall kosta.

		int GetBuyPrice(int iObjectID); // fr�ga aff�ren den k�per ett speciellt f�rem�l f�r.
};

MCOMMON_API Property* Create_P_DMShop();

#endif // _P_DMSHOP_H_
