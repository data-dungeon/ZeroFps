#ifndef _TEXTUREPSPROP_H_
#define _TEXTUREPSPROP_H_

#include "particleproperty.h"

class TexturePSProp : public ParticleProperty
{
private:
	unsigned int m_uiTexture;

public:
	void Update();
	TexturePSProp ( PSystem *pkParent, unsigned int uiTextureNr );
};

#endif





