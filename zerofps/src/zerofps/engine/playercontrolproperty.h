#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "objects.pkg"
#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include <iostream>

using namespace std;

class PlayerControlProperty: public Property {
	private:
		HeightMap *m_pkMap;
		Render *m_pkRender;

	public:
		PlayerControlProperty(HeightMap* pkHeightMap,Render *pkRender);

		void Update();


};



#endif



