#include "blendpsprop.h"
#include "../../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void BlendPSProp::Update()
{

	glBlendFunc (GL_BLEND_DST, m_iBlendDST );
	glBlendFunc (GL_BLEND_SRC, m_iBlendSRC );

	glEnable (GL_BLEND);

}

// ------------------------------------------------------------------------------------------

BlendPSProp::BlendPSProp ( int iBlendDST, int iBlendSRC )
{
	m_iBlendDST = iBlendDST;
	m_iBlendSRC = iBlendSRC;
}

// ------------------------------------------------------------------------------------------
