#ifndef _ENGINE_MODELPROPERTY_H_
#define _ENGINE_MODELPROPERTY_H_

#include "../basic/basic.pkg"
#include "property.h"
#include <iostream>
#include "../render/render.pkg"



using namespace std;

/// Used to display basic simple 3d objects.
class ENGINE_API ModelProperty: public Property {
	private:
		Frustum *m_pkFrustum;
		Render *m_pkRender;		
		vector<PropertyValues> GetPropertyValues();
	public:
		float	m_fRadius;
		int		m_iSlices;
		int		m_iStacks;

		Vector3	m_kColor;
		ModelProperty();
		void CloneOf(Property* pkProperty) { }
		void Update();

		void PackTo(NetPacket* pkNetPacket) ;
		void PackFrom(NetPacket* pkNetPacket) ;

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);

};

Property* Create_ModelProperty();


#endif



