#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "playerobject.h"

using namespace std;

class PlayerControlProperty: public Property {
	private:
		Input *m_pkInput;
		ZeroFps *m_pkFps;
		HeightMap *m_pkMap;
				
	public:
		
		PlayerControlProperty(Input *pkInput,HeightMap *pkMap);

		void Update();
		void Bounce(Vector3 kPos);
		
};



#endif



