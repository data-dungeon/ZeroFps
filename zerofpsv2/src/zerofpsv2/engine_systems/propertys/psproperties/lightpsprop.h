#ifndef _LIGHTPSPROP_H_
#define _LIGHTPSPROP_H_

#include "particleproperty.h"
#include "../../../ogl/zfpsgl.h"

class LightPSProp: public ParticleProperty
{
private:
	bool m_bLightOn;
public:
	void Update();
	LightPSProp ( bool m_bLightOn );
};


#endif