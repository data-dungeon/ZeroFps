#include "blendpsprop.h"
#include "../../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void BlendPSProp::Update()
{

	glBlendFunc ( m_iBlendSRC, m_iBlendDST );
	glEnable (GL_BLEND);

}

// ------------------------------------------------------------------------------------------

BlendPSProp::BlendPSProp ( int iBlendDST, int iBlendSRC )
{
	m_iBlendDST = iBlendDST;
	m_iBlendSRC = iBlendSRC;
}

// ------------------------------------------------------------------------------------------
