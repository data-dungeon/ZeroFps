#ifndef _SIZEPSPROP_H_
#define _SIZEPSPROP_H_

#include "particleproperty.h"

class SizePSProp : public ParticleProperty
{
private:

public:
	void Update();
	SizePSProp ( PSystem *pkParent );
};

#endif