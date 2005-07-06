#include "colorpsprop.h"
#include "../../common/psystem.h"
#include "../../common/psystemmanager.h"
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
		m_fR = ( pkParticles->at(i).m_kEndColor.x - 
					pkParticles->at(i).m_kStartColor.x ) /
					pkParticles->at(i).m_fLifeTime;
		m_fG = ( pkParticles->at(i).m_kEndColor.y - 
					pkParticles->at(i).m_kStartColor.y ) /
					pkParticles->at(i).m_fLifeTime;
		m_fB = ( pkParticles->at(i).m_kEndColor.z - 
					pkParticles->at(i).m_kStartColor.z ) /
					pkParticles->at(i).m_fLifeTime;
		m_fA = ( pkParticles->at(i).m_kEndColor.w - 
					pkParticles->at(i).m_kStartColor.w ) /
					pkParticles->at(i).m_fLifeTime;

		// Prevent particle from use time_Color after age
		for (int j = i * 16; j < i * 16 + 16; j += 4)
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
