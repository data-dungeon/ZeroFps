#ifndef _ALPHATESTPROP_H_
#define _ALPHATESTPROP_H_

#include "particleproperty.h"
#include "../../../ogl/zfpsgl.h"

class AlphaTestPSProp: public ParticleProperty
{
private:
	int m_iAlphaTestValue;
public:
	void Update();
	AlphaTestPSProp ( int iAlphaTest );
};

#endif