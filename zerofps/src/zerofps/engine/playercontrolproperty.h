#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "objects.pkg"
#include "../basic/basic.pkg"
#include <iostream>

using namespace std;

class PlayerControlProperty: public Property {
	private:
		HeightMap *m_pkMap;
		

	public:
		PlayerControlProperty(HeightMap* pkHeightMap);

		void Update();


};



#endif



