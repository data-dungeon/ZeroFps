#include "lightpsprop.h"
#include "../../common/psystem.h"
#include "../../common/psystemmanager.h"
#include "../p_light.h"
#include "../p_psystem.h"
// ------------------------------------------------------------------------------------------

void LightPSProp::Update(const RenderState* pkRenderState)
{
	// Get lightProp each frame in case it was removed, to prevent crash
	P_Light* pkLight = (P_Light*)m_pkParent->m_pkParent->GetEntity()->GetProperty("P_Light");

	if ( !pkLight )
		return;

	float fFrameTime = m_pkParent->m_fFrameTime;

	m_kColor += m_kColorChange * fFrameTime;

	// Prevent color from getting negative
	if (m_kColor.x < 0 )
			m_kColor.x = 0;
	if (m_kColor.y < 0 )
			m_kColor.y = 0;
	if (m_kColor.z < 0 )
			m_kColor.z = 0;

	pkLight->SetAmbient(m_kColor);
	pkLight->SetDiffuse(m_kColor);

}

// ------------------------------------------------------------------------------------------

LightPSProp::LightPSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;

	m_kColor.x = pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightStartColorR;
	m_kColor.y = pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightStartColorG;
	m_kColor.z = pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightStartColorB;
	m_kColor.w = 0;

	m_kColorChange.x = (pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightEndColorR -
		pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightStartColorR) / 
		(pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * (pkParent->m_pkPSystemType->m_kParticleBehaviour.m_iLifeTimeRandom/100.f) + 
		pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime + pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime);
	m_kColorChange.y = (pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightEndColorG -
		pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightStartColorG) / 
		(pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * (pkParent->m_pkPSystemType->m_kParticleBehaviour.m_iLifeTimeRandom/100.f) + 
		pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime + pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime);
	m_kColorChange.z = (pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightEndColorB -
		pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLightStartColorB) / 
		(pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * (pkParent->m_pkPSystemType->m_kParticleBehaviour.m_iLifeTimeRandom/100.f) + 
		pkParent->m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime + pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime);
	m_kColorChange.w = 0;
}

// ------------------------------------------------------------------------------------------
