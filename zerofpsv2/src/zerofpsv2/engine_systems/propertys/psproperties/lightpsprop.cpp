#include "lightpsprop.h"
#include "../../common/psystem.h"
#include "../../../engine/psystemmanager.h"
#include "../p_light.h"
// ------------------------------------------------------------------------------------------

void LightPSProp::Update()
{
	// Get lightProp each frame in case it was removed, to prevent crash
//	P_Light* = m_pkParent->
}

// ------------------------------------------------------------------------------------------

LightPSProp::LightPSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------
