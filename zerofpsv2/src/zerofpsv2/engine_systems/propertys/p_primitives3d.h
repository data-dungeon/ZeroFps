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
	PLANE,
};

/// Used to display basic simple 3d objects.
class ENGINE_SYSTEMS_API P_Primitives3D : public Property {
	private:
		ZSSZeroFps*	m_pkFps;
		ZSSRender*	m_pkRender;
		PrimType		m_ePrimType;
		
	public:
		float		m_fRadius;
		int		m_iSlices;
		int		m_iStacks;
		Vector3	m_kColor;
		Vector3	m_kMin, m_kMax;
		Plane		m_kPlane;
		
		P_Primitives3D();
		void Update();

		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		bool HandleSetValue( const string& kValueName ,const string& kValue );

};

Property* Create_Prim3DProperty();
void ENGINE_SYSTEMS_API Register_Prim3DProperty(ZSSZeroFps* pkZeroFps);


#endif // #ifndef _ENGINE_P_PRIMITIVES3D_H_



