#include "texturepsprop.h"
#include "../../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void TexturePSProp::Update()
{
	// bind texture
	glBindTexture (GL_TEXTURE_2D, m_uiTexture);
}

// ------------------------------------------------------------------------------------------

TexturePSProp::TexturePSProp ( PSystem *pkParent, unsigned int uiTextureNr )
{
	m_uiTexture = uiTextureNr + 1;
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------
