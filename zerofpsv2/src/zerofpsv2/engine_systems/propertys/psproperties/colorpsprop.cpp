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

	for ( int i = m_pkParent->Start() * 12; i < m_pkParent->End() * 12; i += 4 )
	{
//    int iPartIndex = i / 12;

		// Prevent particle from use time_Color after age
		if ( pkParticles->at(i / 12).m_fAge - fFrameTime > 0 )
		{
			pfColors[i    ] += m_fR * fFrameTime; 
			pfColors[i + 1] += m_fG * fFrameTime;
			pfColors[i + 2] += m_fB * fFrameTime;
			pfColors[i + 3] += m_fA * fFrameTime;
		}
	}
	
}

// ------------------------------------------------------------------------------------------

ColorPSProp::ColorPSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;

	// Calculate how much a color value is changed by a timeunit
	m_fR = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.r - 
			   m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.r ) /
				m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime;

	m_fG = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.g - 
			   m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.g ) /
				m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime;

	m_fB = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.b - 
			   m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.b ) /
				m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime;

	m_fA = ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.a - 
			   m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.a ) /
				m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime;
}

// ------------------------------------------------------------------------------------------
