#ifndef _ENGINE_PSYSTEMPROPERTY_H_
#define _ENGINE_PSYSTEMPROPERTY_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../common/psystem.h"
#include "../../render/zshadersystem.h"
#include <vector>
	using namespace std;

/// Property for a particle system.
class ENGINE_SYSTEMS_API P_PSystem : public Property 
{

	private:
		struct ActiveSystem { PSystem* m_pkPSystem; string m_strPSName; };

		vector<ActiveSystem> 			m_kPSystems;
		ZShaderSystem*	m_pkZShaderSystem;

		bool UpdatePS(int iPS,const RenderState*	pkRenderState);
		
	public:
	
		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState);
		void Update();
		vector<PropertyValues>& GetPropertyValues();

		P_PSystem( string kPSType );
		P_PSystem();
      ~P_PSystem();
		
		bool HandleSetValue( const string& kValueName, const string& kValue );

      //PSystem* GetPSystem()                                 { return m_pkPSystem; }

		void SetPSType ( string kName );

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage,int iVersion);

      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

Property* Create_PSystemProperty();
void ENGINE_SYSTEMS_API Register_PPSystem(ZSSZeroFps* pkZeroFps);

#endif





