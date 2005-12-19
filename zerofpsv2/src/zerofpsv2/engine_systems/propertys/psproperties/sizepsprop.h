#ifndef _SIZEPSPROP_H_
#define _SIZEPSPROP_H_

#include "particleproperty.h"

class SizePSProp : public ParticleProperty
{
private:

public:
	void Update(const RenderState* pkRenderState);
	SizePSProp ( PSystem *pkParent );
};

#endif





