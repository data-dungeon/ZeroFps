#ifndef _P_DMHQ_H_
#define _P_DMHQ_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_DMHQ: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
		string	m_strName;
	
	
	public:
		
		P_DMHQ();
		~P_DMHQ();

		void CloneOf(Property* pkProperty) { }
		
//		void Update();
		void Init();
		
//		void Save(ZFIoInterface* pkPackage);
//		void Load(ZFIoInterface* pkPackage);
		
};

MCOMMON_API Property* Create_P_DMHQ();

#endif