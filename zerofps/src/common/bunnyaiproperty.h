#ifndef _BUNNYAIPROPERTY_H_
#define _BUNNYAIPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "bunnyobject.h"
#include "common_x.h"

using namespace std;


class COMMON_API BunnyAIProperty: public Property {
	private:
		ZeroFps *m_pkFps;
		HeightMap *m_pkMap;
	
		int m_iState;

	public:
		
		BunnyAIProperty();
		void CloneOf(Property* pkProperty) { }
		
		void Update();


};


#endif



