#ifndef _ENGINE_P_PRIMITIVES3D_H_
#define _ENGINE_P_PRIMITIVES3D_H_

#include "../../engine/property.h"
#include <iostream>
#include "../engine_systems_x.h"
#include "../../engine/zerofps.h"

using namespace std;

enum PrimType
{
	SPHERE,
	BBOX,
	AABBOX,
	CONE,
	PYRAMID,
	SOLIDBBOX,
};

/// Used to display basic simple 3d objects.
class ENGINE_SYSTEMS_API P_Primitives3D : public Property {
	private:
		ZeroFps* m_pkFps;
		Render*	m_pkRender;
		PrimType m_ePrimType;
		
		vector<PropertyValues> GetPropertyValues();
		
	public:
		float		m_fRadius;
		int		m_iSlices;
		int		m_iStacks;
		Vector3	m_kColor;
		Vector3	m_kMin, m_kMax;
		
		P_Primitives3D(PrimType eType=SPHERE);
		void CloneOf(Property* pkProperty) { }
		void Update();

		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

};

Property* Create_Prim3DProperty();


#endif // #ifndef _ENGINE_P_PRIMITIVES3D_H_



