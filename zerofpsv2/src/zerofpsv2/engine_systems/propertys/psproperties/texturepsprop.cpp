#include "texturepsprop.h"
#include "../p_psystem.h"

// ------------------------------------------------------------------------------------------

void TexturePSProp::Update()
{
	// bind texture
	m_pkParent->m_pkFps->m_pkTexMan->BindTexture(m_uiTexture);
}

// ------------------------------------------------------------------------------------------

TexturePSProp::TexturePSProp ( PSystem *pkParent, unsigned int uiTextureNr )
{
	m_uiTexture = uiTextureNr;
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------







