#include "texturepsprop.h"
#include "../../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void TexturePSProp::Update()
{
	// bind texture

//EVIIIIIIIIIIIL CODE, texturemanagern �r den enda som f�r binda texturer
	glBindTexture (GL_TEXTURE_2D, m_uiTexture);
}

// ------------------------------------------------------------------------------------------

TexturePSProp::TexturePSProp ( PSystem *pkParent, unsigned int uiTextureNr )
{
	m_uiTexture = uiTextureNr;
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------







