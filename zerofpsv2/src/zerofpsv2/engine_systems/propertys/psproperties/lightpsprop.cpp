#include "lightpsprop.h"

// ------------------------------------------------------------------------------------------

void LightPSProp::Update()
{
	if ( m_bLightOn )
		glEnable (GL_LIGHTING);
	else
		glDisable (GL_LIGHTING);
}

// ------------------------------------------------------------------------------------------

LightPSProp::LightPSProp ( bool bLightOn )
{
	m_bLightOn = bLightOn;
}

// ------------------------------------------------------------------------------------------
