#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include <iostream>

using namespace std;

class PlayerControlProperty: public Property {
	private:
		Input *m_pkInput;
	public:
		PlayerControlProperty(Input *pkInput);

		void Update();


};



#endif



