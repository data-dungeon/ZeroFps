#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>

using namespace std;

class PlayerControlProperty: public Property {
	private:
		Input *m_pkInput;
		ZeroFps *m_pkFps;
		HeightMap *m_pkMap;
		
		float ner;
		
	public:
		PlayerControlProperty(Input *pkInput,ZeroFps *pkFps,HeightMap *pkMap);

		void Update();


};



#endif



