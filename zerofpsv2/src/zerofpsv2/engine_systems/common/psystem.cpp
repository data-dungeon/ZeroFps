#include "../../render/render.h"
#include "../../engine/psystemmanager.h"
#include "psystem.h"
#include "../../engine/zerofps.h"
#include "../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void PSystem::Draw()
{

	// Set depthmask
	glDepthMask ( m_pkPSystemType->m_kPSystemBehaviour.m_uiDepthMask );

	m_pkRender->DrawPSystem (this);

	glPopAttrib();
}

// ------------------------------------------------------------------------------------------

void PSystem::Update( Vector3 kNewPosition )
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable (GL_BLEND);

	// Get Frametime
	m_fFrameTime = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"))->GetFrameTime();

	// Update particlesystem lifetime
	if ( m_fAge != -1 )
		m_fAge -= m_fFrameTime;


	// Update age life percent
	m_fAgePercent = m_fAge / m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime;

	// Inherit position from parent
	m_kPosition = kNewPosition;

	// If all particles hasn't been created...
	if ( m_kParticles.size() < m_uiParticles )
	{
		m_fTimeSinceLastCreatedParticle += m_fFrameTime;


		// create a new particle
		if ( m_fTimeSinceLastCreatedParticle >= m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec )
		{

			int iCreate = m_fTimeSinceLastCreatedParticle / m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec;

			// more than one particle could be created / frame
			for ( int i = 0; i < iCreate; i++)
			{
				Particle kNewParticle;

				m_kParticles.push_back ( kNewParticle );

				ResetParticle ( m_kParticles.size() - 1, 
									 m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec - 
									 m_fTimeSinceLastCreatedParticle - (m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec * i) );

				// check so we don't create more particles than there could be in the PSystem
				if ( m_kParticles.size() == m_uiParticles )
					break;
			}
		}

	}

	// Update lifetime for particles
	for ( int i = m_uiFirstParticle; i < m_uiLastParticle + 1; i++ )
	{
		// if particle is active
		if ( m_kParticles[i].m_bActive )
		{
			m_kParticles[i].m_fAge -= m_fFrameTime;
			// if lifetime if over, particle is reseted
			if ( m_kParticles[i].m_fAge < 0 )
				// is particle system finished, don't reset particle
				if ( m_fAge > 0 || m_fAge == -1)
					ResetParticle( i, m_kParticles[i].m_fAge );
				else
					DisableParticle ( i );
		} 
	}

	// Update properties
	for ( i = 0; i < m_kPSProperties.size(); i++ )
		m_kPSProperties[i]->Update();

	// Has PSystem reachet its age and has no active particles, kill it
	if ( m_fAge != -1 && m_fAge < 0 && m_uiLastParticle - m_uiFirstParticle < 1 )
	{
		// uhm..delete it...somehow...
//		cout << "dead" << endl;
	}


}

// ------------------------------------------------------------------------------------------

void PSystem::AddPSProperty( ParticleProperty *pkPSProperty )
{
	// Add PSproperty to PSystem
	m_kPSProperties.push_back( pkPSProperty );
}

// ------------------------------------------------------------------------------------------

PSystem::~PSystem()
{
	// Delete properties
	for ( unsigned int i = 0; i < m_kPSProperties.size(); i++ )
		delete m_kPSProperties[i];

	// Delete particles
	delete[] m_pfVertices;
	delete[] m_pfColors;
	delete[] m_pfTexCoords;
	delete[] m_pfIndices;

}

// ------------------------------------------------------------------------------------------

PSystem::PSystem(PSystemType* pkPSystemType)
{
	m_pkRender = static_cast<Render*>( g_ZFObjSys.GetObjectPtr("Render") );		

	m_pkPSystemType = pkPSystemType;

	m_pfVertices = 0;
	m_pfColors = 0;
	m_pfTexCoords = 0;
	m_fTimeSinceLastCreatedParticle = 0;

	m_uiFirstParticle = 0,
	m_uiLastParticle = -1;

	m_kPosition.Set (0,0,0);

	m_fAge = pkPSystemType->m_kPSystemBehaviour.m_fLifeTime;

}

// ------------------------------------------------------------------------------------------

void PSystem::ResetParticle (int iParticleIndex, float fTimeOffset)
{
	if ( m_fAge < 0 && m_fAge != -1 )
		cout << "ERROR!!!" << endl;

	// set size of particle array stuff
	if ( iParticleIndex > m_uiLastParticle )
		m_uiLastParticle = iParticleIndex;

	if ( iParticleIndex < m_uiFirstParticle )
		m_uiFirstParticle = iParticleIndex;

	int iIndex = (iParticleIndex) * 9;
	int iClrIndex = iParticleIndex * 12;

	m_fTimeSinceLastCreatedParticle = 0;

	float fXOuterRadius = (m_pkPSystemType->m_kPSystemBehaviour.m_kStart_OuterStartArea.x * m_fAgePercent) + 
								 ( m_pkPSystemType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.x * (1.f - m_fAgePercent) );

	float fYOuterRadius = m_pkPSystemType->m_kPSystemBehaviour.m_kStart_OuterStartArea.y * m_fAgePercent + 
								 m_pkPSystemType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.y * (1.f - m_fAgePercent);

	float fZOuterRadius = m_pkPSystemType->m_kPSystemBehaviour.m_kStart_OuterStartArea.z * m_fAgePercent + 
							    m_pkPSystemType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.z * (1.f - m_fAgePercent);

	float fXInnerRadius = m_pkPSystemType->m_kPSystemBehaviour.m_kStart_InnerStartArea.x * m_fAgePercent + 
								 m_pkPSystemType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.x * (1.f - m_fAgePercent);

	float fYInnerRadius = m_pkPSystemType->m_kPSystemBehaviour.m_kStart_InnerStartArea.y * m_fAgePercent + 
								 m_pkPSystemType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.y * (1.f - m_fAgePercent);
	
	float fZInnerRadius = m_pkPSystemType->m_kPSystemBehaviour.m_kStart_InnerStartArea.z * m_fAgePercent + 
								 m_pkPSystemType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.z * (1.f - m_fAgePercent);

	
	double dRADx = rand()%6283184;
	double dRADy = rand()%(6283184 - int(dRADx));
	double dRADz = rand()%(6283184 - int(dRADx + dRADy) );

	dRADx /= 10000.0 * 2.0;
	dRADy /= 10000.0 * 2.0;
	dRADz /= 10000.0 * 2.0;
	

	// Reset position
	if ( fXOuterRadius != 0 )
		m_kParticles[iParticleIndex].m_kCenter.x = cos(dRADx) * fXOuterRadius + cos(dRADx) * fXInnerRadius - fXOuterRadius / 2.f;
	else
		m_kParticles[iParticleIndex].m_kCenter.x =  - fXOuterRadius / 2.f;

	if ( fYOuterRadius != 0 )
		m_kParticles[iParticleIndex].m_kCenter.y = m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y + cos(dRADy) * fYOuterRadius + cos(dRADy) * fYInnerRadius - fYOuterRadius / 2.f;
	else
		m_kParticles[iParticleIndex].m_kCenter.y = m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y - fYOuterRadius / 2.f;

	if ( fZOuterRadius != 0 )
		m_kParticles[iParticleIndex].m_kCenter.z = sin(dRADz) * fZOuterRadius + sin(dRADz) * fZInnerRadius - fZOuterRadius / 2.f;
	else
		m_kParticles[iParticleIndex].m_kCenter.z =  - fZOuterRadius / 2.f;

	m_kParticles[iParticleIndex].m_kCenter += m_kPosition;
 
	// Set current particle to active
	m_kParticles[iParticleIndex].m_bActive = true;

	// if PSystem uses colors, reset color
	if ( m_pfColors )
		for ( int i = 0; i < 12; i += 4 )
		{
			m_pfColors[iClrIndex + i    ]	= m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.r;
			m_pfColors[iClrIndex + i + 1] = m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.g;
			m_pfColors[iClrIndex + i + 2] = m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.b;
			m_pfColors[iClrIndex + i + 3] = m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.a;
		}


	m_kParticles[iParticleIndex].m_fAge = m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime + fTimeOffset;
	m_kParticles[iParticleIndex].m_kForce = m_pkPSystemType->m_kParticleBehaviour.m_kForce;
	
	
	Vector3 kRandomDir = m_pkPSystemType->m_kParticleBehaviour.m_kDirection;

	kRandomDir.x = rand()%int(m_pkPSystemType->m_kParticleBehaviour.m_kWideness.x);
	kRandomDir.z = rand()%int(m_pkPSystemType->m_kParticleBehaviour.m_kWideness.z);
	
	kRandomDir.y *= cos(kRandomDir.x / degtorad) * cos(kRandomDir.z / degtorad);

	kRandomDir.x = ((rand()%2 * 2) - 1) * sin(kRandomDir.x / degtorad) + 
						m_pkPSystemType->m_kParticleBehaviour.m_kDirection.x;
	kRandomDir.z = ((rand()%2 * 2) - 1) * sin(kRandomDir.z / degtorad) + 
						m_pkPSystemType->m_kParticleBehaviour.m_kDirection.z;

	

	m_kParticles[iParticleIndex].m_kVelocity = m_pkPSystemType->m_kParticleBehaviour.m_fStartSpeed * 
															 kRandomDir;
	
	m_kParticles[iParticleIndex].m_kSize.x = m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.x;
	m_kParticles[iParticleIndex].m_kSize.y = m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y;

	m_kParticles[iParticleIndex].m_kEndSize.x = m_pkPSystemType->m_kParticleBehaviour.m_kEndSize.x;
	m_kParticles[iParticleIndex].m_kEndSize.y = m_pkPSystemType->m_kParticleBehaviour.m_kEndSize.y;

	// Randomize size startvalues
	if ( m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom )
	{
		m_kParticles[iParticleIndex].m_kSize.x *= rand()%m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom;
		m_kParticles[iParticleIndex].m_kSize.y *= rand()%m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom;
	}

	// Randomize size endvalues
	if ( m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom )
	{
		m_kParticles[iParticleIndex].m_kSize.x *= rand()%m_pkPSystemType->m_kParticleBehaviour.m_iEndSizeRandom;
		m_kParticles[iParticleIndex].m_kSize.y *= rand()%m_pkPSystemType->m_kParticleBehaviour.m_iEndSizeRandom;
	}


}

// ------------------------------------------------------------------------------------------

void PSystem::DisableParticle ( int iParticleIndex )
{
	m_kParticles[iParticleIndex].m_bActive = false;

	// last...
	for ( int i = m_uiLastParticle; i > m_uiFirstParticle; i-- )
		if ( !m_kParticles[i].m_bActive )
			m_uiLastParticle--;
		else
			break;

		// first...
	for ( i = m_uiFirstParticle; i < m_uiLastParticle; i++ )
		if ( !m_kParticles[i].m_bActive )
			m_uiFirstParticle++;
		else
			break;
}

// ------------------------------------------------------------------------------------------

unsigned int PSystem::GetAlphaTestValue()
{ 
	return m_pkPSystemType->m_kPSystemBehaviour.m_uiAlphaTest; 
}

// ------------------------------------------------------------------------------------------

unsigned int PSystem::GetDepthValue()
{
	return m_pkPSystemType->m_kPSystemBehaviour.m_uiDepthMask; 
}

// ------------------------------------------------------------------------------------------

void PSystem::TimeoffSet ()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	m_fFrameTime = m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec;

	cout << m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime *
								m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec << endl;

	// create particles
	for ( int i = 0; i < m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime *
								m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec; i++ )
	{
		Particle kNewParticle;

		m_kParticles.push_back ( kNewParticle );

		ResetParticle ( m_kParticles.size() - 1, 0 );

		m_kParticles[ m_kParticles.size() - 1].m_fAge = (m_fFrameTime * i);

		cout << m_kParticles[ m_kParticles.size() - 1].m_fAge << endl;

		// check so we don't create more particles than there could be in the PSystem
		if ( m_kParticles.size() == m_uiParticles )
			break;
	}

	// Update velocity
	for ( int j = 0; j < m_kParticles.size(); j++ )
	{
		m_kParticles[j].m_kVelocity += m_kParticles[j].m_kForce * m_kParticles[j].m_fAge;
		m_kParticles[j].m_kCenter += m_kParticles[j].m_kVelocity * ( pow (m_kParticles[j].m_fAge,2 ) / 2.f );

		cout << m_kParticles[j].m_kVelocity.x << endl;
		cout << m_kParticles[j].m_kVelocity.y << endl;
		cout << m_kParticles[j].m_kVelocity.z << endl << endl;
	}
	

	glPopAttrib();
}

// ------------------------------------------------------------------------------------------
