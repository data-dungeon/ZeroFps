#include "alphatestprop.h"

// ------------------------------------------------------------------------------------------

void AlphaTestPSProp::Update()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc( m_iAlphaTestValue, 0.5);
}

// ------------------------------------------------------------------------------------------

AlphaTestPSProp::AlphaTestPSProp ( int iAlphaTest )
{
	m_iAlphaTestValue = iAlphaTest;
}

// ------------------------------------------------------------------------------------------
