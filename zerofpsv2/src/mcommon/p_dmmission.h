#ifndef _P_DMMISSION_H_
#define _P_DMMISSION_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_DMMission: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
		
	public:
		
		P_DMMission();
		~P_DMMission();

		void CloneOf(Property* pkProperty) { }
		
		void Init();
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		string m_strName;
		string m_strMissionScript;

		int m_iDifficulty;

		ZFScriptSystem* m_pkScriptSys;
		ZFResourceHandle* m_pkScriptResHandle;

};

MCOMMON_API Property* Create_P_DMMission();

#endif // _P_DMMISSION_H_
