#ifndef _ENGINE_LIGHTUPDATEPROPERTY_H_
#define _ENGINE_LIGHTUPDATEPROPERTY_H_

#include "../../engine/property.h"
#include "../../engine/engine_x.h"
#include "../../engine/entity.h"
#include "../engine_systems_x.h"

using namespace std;

/// Property that updates lightning.
class ENGINE_SYSTEMS_API P_LightUpdate : public Property
{
	private:
		Light* m_pkLight;


	public:
	P_LightUpdate();
	
	
	void CloneOf(Property* pkProperty) { }
	void Update();
	
	void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
	void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;	
};

Property* Create_LightUpdateProperty();


#endif



