#ifndef _ENGINE_PSYSTEMPROPERTY_H_
#define _ENGINE_PSYSTEMPROPERTY_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../common/psystem.h"

/// Property for a particle system.
class ENGINE_SYSTEMS_API P_PSystem : public Property {

	private:
		PSystem *m_pkPSystem;
		string m_kPSType;

	public:
		void Update();
		vector<PropertyValues> GetPropertyValues();

		P_PSystem( string kPSType );
		P_PSystem();
      ~P_PSystem();
		
		bool HandleSetValue( string kValueName, string kValue );

      PSystem* GetPSystem()                                 { return m_pkPSystem; }

		void SetPSType ( string kName );

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage);

      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

Property* Create_PSystemProperty();

#endif





