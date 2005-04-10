#ifndef _LIGHTPSPROP_H_
#define _LIGHTPSPROP_H_

#include "particleproperty.h"
#include "../../../basic/vector4.h"

class LightPSProp : public ParticleProperty
{
private:
	Vector4 m_kColor;
	Vector4 m_kColorChange;

public:
	void Update();
	LightPSProp ( PSystem *pkParent );
};

#endif

