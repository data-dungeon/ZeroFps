#ifndef _LIGHTUPDATEPROPERTY_H_
#define _LIGHTUPDATEPROPERTY_H_

#include "../render/render.pkg"
#include "property.h"
#include "engine_x.h"
#include "object.h"

using namespace std;

class ENGINE_API LightUpdateProperty: public Property
{
	private:
		Light* m_pkLight;


	public:
	LightUpdateProperty();
	void Update();
};

Property* Create_LightUpdateProperty();


#endif



