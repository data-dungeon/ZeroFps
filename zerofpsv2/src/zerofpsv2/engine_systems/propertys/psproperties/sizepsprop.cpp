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
		pkParticles->at(i).m_kSize.x += ( (pkParticles->at(i).m_kEndSize.x - 
				  								    pkParticles->at(i).m_kStartSize.x) /
				  								    pkParticles->at(i).m_fLifeTime )
												  * fFrameTime;

		pkParticles->at(i).m_kSize.y += ( (pkParticles->at(i).m_kEndSize.y - 
				  								    pkParticles->at(i).m_kStartSize.y) /
				  								    pkParticles->at(i).m_fLifeTime )
												  * fFrameTime;
	}
	 
}

// ------------------------------------------------------------------------------------------

SizePSProp::SizePSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------
