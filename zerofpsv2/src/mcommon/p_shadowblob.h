#ifndef _P_SHADOWBLOB_H_
#define _P_SHADOWBLOB_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_ShadowBlob: public Property {
	private:
		Render*	m_pkRender;
		
		vector<PropertyValues> GetPropertyValues();
	
		
		ZFResourceHandle* m_pkTexture;
		
		bool		m_bHaveSet;
		Vector3	m_kOffset;
		Vector3	m_kScale;
	
	public:
		
		P_ShadowBlob();
		~P_ShadowBlob();

		void CloneOf(Property* pkProperty) { }
		
		void Update();
		void Init();
		
		
			
//		void Save(ZFIoInterface* pkPackage);
//		void Load(ZFIoInterface* pkPackage);
		
};

MCOMMON_API Property* Create_P_ShadowBlob();

#endif







