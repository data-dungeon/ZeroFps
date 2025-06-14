#ifndef _BUNNYAIPROPERTY_H_
#define _BUNNYAIPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "bunnyobject.h"


using namespace std;


class BunnyAIProperty: public Property {
	private:
		ZeroFps *m_pkFps;
		HeightMap *m_pkMap;
	
		int m_iState;

	public:
		
		BunnyAIProperty();
		
		void Update();


};


#endif



