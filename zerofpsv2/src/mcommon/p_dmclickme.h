#ifndef _P_DMCLICKME_H_
#define _P_DMCLICKME_H_

#include "../zerofpsv2/engine/property.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_DMClickMe: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();

		void *m_pkScript;
	
	public:
		void Click();

		P_DMClickMe();
		~P_DMClickMe();

		void CloneOf(Property* pkProperty) { }
		
//		void Update();
		void Init();	
	
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);		
};

MCOMMON_API Property* Create_P_DMClickMe();

#endif


