#ifndef _BLENDPSPROP_H_
#define _BLENDPSPROP_H_

#include "particleproperty.h"
#include "../../../ogl/zfpsgl.h"

class BlendPSProp: public ParticleProperty
{
private:
	int 
		m_iBlendDST,
		m_iBlendSRC;

public:
	void Update();
	BlendPSProp ( int iBlendDST, int iBlendSRC );
};


#endif