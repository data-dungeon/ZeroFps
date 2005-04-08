#ifndef _SIZEPSPROP_H_
#define _SIZEPSPROP_H_

#include "particleproperty.h"

class LightPSProp : public ParticleProperty
{
private:

public:
	void Update();
	LightPSProp ( PSystem *pkParent );
};

#endif

