#ifndef _COLORPSPROP_H_
#define _COLORPSPROP_H_

#include "particleproperty.h"

class ColorPSProp: public ParticleProperty
{
private:
	// how much a value is changed by a time unit
	float
		m_fR,
		m_fG,
		m_fB,
		m_fA;
public:
	void Update();
	ColorPSProp ( PSystem *pkParent );
};

#endif








