#ifndef _P_DMCHARACTER_H_
#define _P_DMCHARACTER_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_DMCharacter: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
	
	public:
		
		P_DMCharacter();
		~P_DMCharacter();

		void CloneOf(Property* pkProperty) { }
		
//		void Update();
		void Init();
		
//		void Save(ZFIoInterface* pkPackage);
//		void Load(ZFIoInterface* pkPackage);
		
};

MCOMMON_API Property* Create_P_DMCharacter();

#endif