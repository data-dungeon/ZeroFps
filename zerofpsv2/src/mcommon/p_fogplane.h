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
		ZSSRender*			m_pkRender;

		float					m_fSize;
		
		ZFResourceHandle* m_pkMaterial;
		string				m_strMaterial;		
		
		void DrawFogPlane();		
		
		bool HandleSetValue( const string& kValueName ,const string& kValue );		
		
	public:
		P_FogPlane();
		~P_FogPlane();
		
		
		void SetMaterial(const string& strMaterial);
		
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		

};

MCOMMON_API Property* Create_P_FogPlane();
MCOMMON_API void Register_P_FogPlane(ZSSZeroFps* pkZeroFps);

#endif

