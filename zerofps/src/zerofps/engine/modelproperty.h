#ifndef _MODELPROPERTY_H_
#define _MODELPROPERTY_H_

#include "../basic/basic.pkg"
#include "property.h"
#include <iostream>

#include "GL/glut.h"

using namespace std;



class ENGINE_API ModelProperty: public Property {
	public:
		float	m_fRadius;
		int		m_iSlices;
		int		m_iStacks;

		Vector3	m_kColor;
		ModelProperty();
		void Update();

		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

};

Property* Create_ModelProperty();


#endif



