#ifndef _ENGINE_PSYSTEMPROPERTY_H_
#define _ENGINE_PSYSTEMPROPERTY_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../common/psystem.h"

/// Property for a particle system.
class ENGINE_SYSTEMS_API PSystemProperty: public Property {

	private:
		PSystem *m_pkPSystem;
		string m_kPSType;

	public:
		void Update();
		void CloneOf(Property* pkProperty) {}
		vector<PropertyValues> GetPropertyValues();

		PSystemProperty( string kPSType );
		PSystemProperty();

		bool HandleSetValue( string kValueName, string kValue );

		void SetPSType ( string kName );

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage);
};

Property* Create_PSystemProperty();

#endif





