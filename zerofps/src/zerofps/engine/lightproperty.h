#ifndef _LIGHTPROPERTY_H_
#define _LIGHTPROPERTY_H_

#include "../render/render.pkg"
#include "property.h"

using namespace std;

class ENGINE_API LightProperty: public Property {
	private:
		LightSource*	spot;
		Vector3*		spotpos;
		Vector3*		spotrot;	

		Light*			m_pkLight;

	public:
		LightProperty();
		~LightProperty();

		void Update();

		void SetColor(float r, float g, float b);
		void SetLightPos(Vector3 kNewPos);
};

Property* Create_LightProperty();

#endif



