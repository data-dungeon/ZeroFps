#ifndef _P_FOGPLANE_H
#define _P_FOGPLANE_H

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "mcommon_x.h"

using namespace std;

/**	\brief	Da P_Ml
		\ingroup Common
*/
class MCOMMON_API P_FogPlane: public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		Render*				m_pkRender;
		ZMaterial*			m_pkFogMaterial;

		float					m_fSize;
		
		
		void DrawFogPlane();
		
		vector<PropertyValues> GetPropertyValues();
		
	public:
		P_FogPlane();
		~P_FogPlane();
		
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		

};

MCOMMON_API Property* Create_P_FogPlane();
MCOMMON_API void Register_P_FogPlane(ZeroFps* pkZeroFps);

#endif

