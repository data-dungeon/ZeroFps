#ifndef _SIZEPSPROP_H_
#define _SIZEPSPROP_H_

#include "particleproperty.h"

class SizePSProp : public ParticleProperty
{
private:

	// Change value / time unit
	float 
		m_fWidth,
		m_fHeight;
public:
	void Update();
	SizePSProp ( PSystem *pkParent );
};

#endif