#ifndef _NORMALDRAWPROPERTY_H_
#define _NORMALDRAWPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "../zerofps/render/render.pkg"
#include <iostream>

using namespace std;

class NormalDrawProperty: public Property {
	private:
		HeightMap *m_pkMap;
		Render *m_pkRender;

	public:
		NormalDrawProperty(HeightMap* pkHeightMap,Render *pkRender);

		void Update();


};



#endif



