#include "sizepsprop.h"
#include "../../common/psystem.h"
#include "../../../engine/psystemmanager.h"

// ------------------------------------------------------------------------------------------

void SizePSProp::Update()
{
	// Update size for particles

	float fFrameTime = m_pkParent->m_fFrameTime;

	vector<Particle>* pkParticles = &m_pkParent->m_kParticles;

	for ( int i = m_pkParent->Start(); i < m_pkParent->End(); i++ )
	{
		pkParticles->at(i).m_kSize.x += m_fWidth * fFrameTime;
		pkParticles->at(i).m_kSize.y += m_fHeight * fFrameTime;
	}
	
}

// ------------------------------------------------------------------------------------------

SizePSProp::SizePSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;

	// Calculate size change / timeunit
	m_fWidth  = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndSize.x - 
				  	  m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.x ) /
				  	  m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime;

	m_fHeight = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndSize.y - 
					  m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y ) /
					  m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime;
}

// ------------------------------------------------------------------------------------------
