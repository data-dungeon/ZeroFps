#include "colorpsprop.h"
#include "../../common/psystem.h"
#include "../../../engine/psystemmanager.h"
#include <iostream>
	using namespace std;

// ------------------------------------------------------------------------------------------

void ColorPSProp::Update()
{
	// Update color value for particles
	float *pfColors = m_pkParent->GetColors();

	vector<Particle>* pkParticles = &m_pkParent->m_kParticles;

	float fFrameTime = m_pkParent->m_fFrameTime;

 	for ( int i = m_pkParent->Start(); i < m_pkParent->End(); i++ )
	{
		m_fR = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.r - 
					m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.r ) /
					pkParticles->at(i).m_fLifeTime;
		m_fG = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.g - 
					m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.g ) /
					pkParticles->at(i).m_fLifeTime;
		m_fB = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.b - 
					m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.b ) /
					pkParticles->at(i).m_fLifeTime;
		m_fA = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.a - 
					m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.a ) /
					pkParticles->at(i).m_fLifeTime;

		// Prevent particle from use time_Color after age
		for (int j = i * 12; j < i * 12 + 12; j += 4)
		{
			pfColors[j    ] += m_fR * fFrameTime; 
			pfColors[j + 1] += m_fG * fFrameTime;
			pfColors[j + 2] += m_fB * fFrameTime;
			pfColors[j + 3] += m_fA * fFrameTime;
		}
	}
}

// ------------------------------------------------------------------------------------------

ColorPSProp::ColorPSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------
