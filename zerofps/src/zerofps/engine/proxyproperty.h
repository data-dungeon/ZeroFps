#ifndef _PROXYPROPERTY_H_
#define _PROXYPROPERTY_H_

#include "../basic/basic.pkg"
//#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>

#include <string.h>
//#include "GL/glut.h"

using namespace std;

enum PROXYPROPERTY_AKTIONS{
	UPDATECHILDS,
	WAKEUP
};


class ENGINE_API ProxyProperty: public Property {
	private:
		float m_fRadius;	
		int m_iAktion;
			
		ZeroFps* m_pkFps;
			
	public:
		ProxyProperty();
		void Update();
		
		void SetRadius(float fRadius) {m_fRadius= fRadius;};
		float GetRadius() {return m_fRadius;};
	
		void SetAktion(int iAktion) {m_iAktion=iAktion;};
		int GetAktion() {return m_iAktion;};
};

Property* Create_ProxyProperty();

#endif




