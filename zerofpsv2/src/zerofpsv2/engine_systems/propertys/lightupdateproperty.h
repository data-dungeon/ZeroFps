#ifndef _ENGINE_LIGHTUPDATEPROPERTY_H_
#define _ENGINE_LIGHTUPDATEPROPERTY_H_

#include "../../engine/property.h"
#include "../../engine/engine_x.h"
#include "../../engine/object.h"
#include "../engine_systems_x.h"

using namespace std;

class ENGINE_SYSTEMS_API LightUpdateProperty: public Property
{
	private:
		Light* m_pkLight;


	public:
	LightUpdateProperty();
		void CloneOf(Property* pkProperty) { }
	void Update();
};

Property* Create_LightUpdateProperty();


#endif



