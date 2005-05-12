#ifndef _P_OST_H
#define _P_OST_H

#include "../../zerofpsv2/engine/property.h"
#include "../../zerofpsv2/engine/zerofps.h"

using namespace std;

/**	\brief	Da P_Ml
		\ingroup Common
*/
class P_Ost : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		Render*				m_pkRender;
		ZMaterial*			m_pkFogMaterial;

		float					m_fSize;
		
		
		void DrawFogPlane();
		
		vector<PropertyValues> GetPropertyValues();
		
	public:
		P_Ost();
		~P_Ost();
		
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		

};

Property* Create_P_Ost();
void Register_P_Ost(ZeroFps* pkZeroFps);

#endif

