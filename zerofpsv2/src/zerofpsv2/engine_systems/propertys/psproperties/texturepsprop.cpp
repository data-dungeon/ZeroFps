#include "texturepsprop.h"
#include "../p_psystem.h"
//#include "../../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void TexturePSProp::Update()
{
	// bind texture
	m_pkParent->m_pkFps->m_pkTexMan->BindTexture(m_uiTexture);
//EVIIIIIIIIIIIL CODE, texturemanagern är den enda som får binda texturer
//	glBindTexture (GL_TEXTURE_2D, m_uiTexture);
}

// ------------------------------------------------------------------------------------------

TexturePSProp::TexturePSProp ( PSystem *pkParent, unsigned int uiTextureNr )
{
	m_uiTexture = uiTextureNr;
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------







