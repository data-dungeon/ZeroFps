#include "../../render/render.h"
#include "../../engine/psystemmanager.h"
#include "psystem.h"
#include "../../ogl/zfpsgl.h"

// ------------------------------------------------------------------------------------------

void PSystem::Draw()
{
	
	if ( m_bInsideFrustum )
	{
		// Set depthmask
		glDepthFunc ( m_pkPSystemType->m_kPSystemBehaviour.m_uiDepthMask );

		m_pkRender->DrawPSystem (this);
	}

	glPopAttrib();
}

// ------------------------------------------------------------------------------------------

bool PSystem::Update( Vector3 kNewPosition, Matrix4 kNewRotation )
{

	// Inherit position from parent
	m_kPosition = kNewPosition;

	// Inherit rotation from parent
	m_kRotation = kNewRotation;

	TestInsideFrustum();

	// don't update live-forevever psystems if not inside frustum
	if ( m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime == -9999999 && !m_bInsideFrustum )
		return false;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable (GL_BLEND);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);	

		// if PSystem loops in infinity, make it have particles from start
	if ( m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime == -9999999 && m_bFirstRun )
	{
		TimeoffSet();
		m_bFirstRun = false;
	}


	// Get Frametime
	m_fFrameTime = m_pkFps->GetFrameTime();// - m_fLastTime;//GetTicks() - m_fLastTime;
	m_fLastTime =  SDL_GetTicks();// m_pkFps->m_pkObjectMan->GetSimTime();


	// Update particlesystem lifetime
	if ( m_fAge != -9999999 )
	{
		m_fAge -= m_fFrameTime;
	}

	// Update age life percent
	m_fAgePercent = m_fAge / m_pkPSystemType->m_kPSystemBehaviour.m_fLifeTime;

	// If all particles hasn't been created...
	if ( (int) m_kParticles.size() < m_uiParticles )
	{
		m_fTimeSinceLastCreatedParticle += m_fFrameTime;

		// create a new particle
		if ( m_fTimeSinceLastCreatedParticle >= m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec )
		{

			int iCreate = int(m_fTimeSinceLastCreatedParticle / m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec);
			float fTempTime = m_fTimeSinceLastCreatedParticle / iCreate;


			// more than one particle could be created / frame
			for ( int i = 0; i < iCreate; i++)
			{
				Particle kNewParticle;

				m_kParticles.push_back ( kNewParticle );

				ResetParticle ( m_kParticles.size() - 1, fTempTime * (iCreate - i) );

				// check so we don't create more particles than there could be in the PSystem
				if ( m_kParticles.size() == m_uiParticles )
					break;
			}

		}

	}

	// Update lifetime for particles
	int i;
	
	for ( i = m_uiFirstParticle; i < m_uiLastParticle + 1; i++ )
	{
		// if particle is active
		if ( m_kParticles[i].m_bActive )
		{
			m_kParticles[i].m_fAge -= m_fFrameTime;

			// if lifetime if over, particle is reseted
			if ( m_kParticles[i].m_fAge < 0 )
				// is particle system finished, don't reset particle
				if ( m_fAge > 0 || m_fAge == -9999999)
					ResetParticle( i, -m_kParticles[i].m_fAge );
				else
					DisableParticle ( i );
		}
	}

	// Update properties
	for ( i = 0; i < (int) m_kPSProperties.size(); i++ )
		m_kPSProperties[i]->Update();

	// Has PSystem reachet its age and has no active particles, return true (PS finished)
	if ( m_fAge != -9999999 && m_fAge < 0 && m_uiLastParticle - m_uiFirstParticle < 1 )
      return true;
   else 
      return false;


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
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkPSystemType = pkPSystemType;

	m_pfVertices = 0;
	m_pfColors = 0;
	m_pfTexCoords = 0;
	m_fTimeSinceLastCreatedParticle = 0;

	m_uiFirstParticle = 0,
	m_uiLastParticle = -1;

	m_kPosition.Set (0,0,0);

	m_bInsideFrustum = true;
	
	m_kPosOffset = pkPSystemType->m_kPSystemBehaviour.m_kPosOffset;

	m_fAge = pkPSystemType->m_kPSystemBehaviour.m_fLifeTime;

	m_fAgePercent = 1;

	m_bFirstRun = true;
	m_fLastTime = m_pkFps->GetTicks();

}

// ------------------------------------------------------------------------------------------

void PSystem::ResetParticle (int iParticleIndex, float fTimeOffset)
{
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

	double dRadiusX, dRadiusY, dRadiusZ;
	double dRADx, dRADy, dRADz;

	if ( m_pkPSystemType->m_kPSystemBehaviour.m_kCreateStyle == "box" )
	{
		dRadiusX = (rand()%100) / 100.f;
		dRadiusY = (rand()%100) / 100.f;
		dRadiusZ = (rand()%100) / 100.f;

		dRADx = rand()%6283;
		dRADz = rand()%6283;
		dRADy = rand()%6283;
	}
	else
	{
		dRadiusX = dRadiusY = dRadiusZ = (rand()%100) / 100.f;
		dRADx = dRADz = rand()%6283;
		dRADy = rand()%(6283 - int(dRADx));
	}
	

	dRADx /= 100.0;
	dRADz /= 100.0;
	dRADy /= 100.0;


	// Reset position
	if ( fXOuterRadius != 0 )
		m_kParticles[iParticleIndex].m_kCenter.x = float( float(cos(dRADx) * fXOuterRadius * dRadiusX) + (cos(dRADx) * fXInnerRadius * (1-dRadiusX)) );
	else
		m_kParticles[iParticleIndex].m_kCenter.x = 0;

	if ( fYOuterRadius != 0 )
		m_kParticles[iParticleIndex].m_kCenter.y = float( (cos(dRADy) * fYOuterRadius * dRadiusY) + (cos(dRADy) * fYInnerRadius * (1-dRadiusY)) );
	else
		m_kParticles[iParticleIndex].m_kCenter.y = 0;

	if ( fZOuterRadius != 0 )
		m_kParticles[iParticleIndex].m_kCenter.z = float( (sin(dRADz) * fZOuterRadius * dRadiusZ) + (sin(dRADz) * fZInnerRadius * (1 - dRadiusZ)) );
	else
		m_kParticles[iParticleIndex].m_kCenter.z = 0;

	// Set current particle to active
	m_kParticles[iParticleIndex].m_bActive = true;

	// if PSystem uses colors, reset color
	if ( m_pfColors )
		for ( int i = 0; i < 12; i += 4 )
		{
			m_pfColors[iClrIndex + i    ]	= m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.r + 
													  (( m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.r - 
													  m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.r ) /
													  m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * fTimeOffset);

			m_pfColors[iClrIndex + i + 1] = m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.g + 
													  (( m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.g - 
													  m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.g ) /
													  m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * fTimeOffset);

			m_pfColors[iClrIndex + i + 2] = m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.b + 
													  (( m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.b - 
													  m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.b ) /
													  m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * fTimeOffset);

			m_pfColors[iClrIndex + i + 3] = m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.a + 
													  (( m_pkPSystemType->m_kParticleBehaviour.m_kEndColor.a - 
													  m_pkPSystemType->m_kParticleBehaviour.m_kStartColor.a ) /
													  m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime * fTimeOffset);
		
		}


	// Set and randomize lifetime
	m_kParticles[iParticleIndex].m_fLifeTime = m_pkPSystemType->m_kParticleBehaviour.m_fLifeTime
															 * (1 + ((((rand()%100) / 100.f) 
															 * ((rand()%2) * 2 - 1)) 
															 * (m_pkPSystemType->m_kParticleBehaviour.m_iLifeTimeRandom/100.f)));

	// Set age to max lifetime
	m_kParticles[iParticleIndex].m_fAge = m_kParticles[iParticleIndex].m_fLifeTime - fTimeOffset;

	// Randomize lifetime
	m_kParticles[iParticleIndex].m_fAge *= 1 - (((rand()%m_pkPSystemType->m_kParticleBehaviour.m_iLifeTimeRandom)/100.f));

	m_kParticles[iParticleIndex].m_kForce = m_pkPSystemType->m_kParticleBehaviour.m_kForce;
	
	
	Vector3 kRandomDir = m_pkPSystemType->m_kParticleBehaviour.m_kDirection;

	kRandomDir.x = float( rand()%int(m_pkPSystemType->m_kParticleBehaviour.m_kWideness.x) );
	kRandomDir.z = float( rand()%int(m_pkPSystemType->m_kParticleBehaviour.m_kWideness.z) );
	
	kRandomDir.y *= float( cos(kRandomDir.x / degtorad) * cos(kRandomDir.z / degtorad) );

	kRandomDir.x = float( ((rand()%2 * 2) - 1) * sin(kRandomDir.x / degtorad) + 
						m_pkPSystemType->m_kParticleBehaviour.m_kDirection.x );
	kRandomDir.z = float( ((rand()%2 * 2.f) - 1.f) * sin(kRandomDir.z / (degtorad)) + 
						m_pkPSystemType->m_kParticleBehaviour.m_kDirection.z );

	// Startspeed
	float fStartSpeedRand = ((rand()%100) / 100.f) * ((rand()%2) * 2 - 1);
	float fStartSpeed = m_pkPSystemType->m_kParticleBehaviour.m_fStartSpeed * 
							  (1 + (fStartSpeedRand * (m_pkPSystemType->m_kParticleBehaviour.m_iStartSpeedRand/100.f)));

	m_kParticles[iParticleIndex].m_kVelocity = kRandomDir * fStartSpeed;

	m_kParticles[iParticleIndex].m_kStartSize.x = m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.x;
	m_kParticles[iParticleIndex].m_kStartSize.y = m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y;

	m_kParticles[iParticleIndex].m_kEndSize.x = m_pkPSystemType->m_kParticleBehaviour.m_kEndSize.x;
	m_kParticles[iParticleIndex].m_kEndSize.y = m_pkPSystemType->m_kParticleBehaviour.m_kEndSize.y;


	
	// Randomize size startvalues
	if ( m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom )
	{
		float fStartRand = ((rand()%100) / 100.f) * ((rand()%2) * 2 - 1);

		m_kParticles[iParticleIndex].m_kStartSize.x *= 1 + (fStartRand * (m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom/100.f));
		m_kParticles[iParticleIndex].m_kStartSize.y *= 1 + (fStartRand * (m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom/100.f));

	}

	// Randomize size endvalues
	if ( m_pkPSystemType->m_kParticleBehaviour.m_iStartSizeRandom )
	{
		float fEndRand = ((rand()%100) / 100.f) * ((rand()%2) * 2 - 1);

		m_kParticles[iParticleIndex].m_kEndSize.x *= 1 + (fEndRand * (m_pkPSystemType->m_kParticleBehaviour.m_iEndSizeRandom/100.f));
		m_kParticles[iParticleIndex].m_kEndSize.y *= 1 + (fEndRand * (m_pkPSystemType->m_kParticleBehaviour.m_iEndSizeRandom/100.f));
	}

	// Set startsize
	m_kParticles[iParticleIndex].m_kSize.x = m_kParticles[iParticleIndex].m_kStartSize.x;
	m_kParticles[iParticleIndex].m_kSize.y = m_kParticles[iParticleIndex].m_kStartSize.y;

	// Update startposition
	m_kParticles[iParticleIndex].m_kCenter += m_kParticles[iParticleIndex].m_kVelocity * fTimeOffset + 
															m_kParticles[iParticleIndex].m_kForce * float(pow(fTimeOffset,2)/2.f);

	m_kParticles[iParticleIndex].m_kVelocity += m_kParticles[iParticleIndex].m_kForce * fTimeOffset;


	if ( m_pkPSystemType->m_kParticleBehaviour.m_bStartSpeedInheritDirection )
		m_kParticles[iParticleIndex].m_kVelocity = m_kRotation.VectorRotate(kRandomDir * m_pkPSystemType->m_kParticleBehaviour.m_fStartSpeed);

 	m_kParticles[iParticleIndex].m_kCenter += m_kPosition + m_kRotation.VectorRotate(m_kPosOffset);

   m_kParticles[iParticleIndex].m_kCenter.y += m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y /2.f;

   m_kParticles[iParticleIndex].m_kCenter += m_kRotation.VectorRotate ( Vector3 (m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.x, 
                                                                        m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.y, 
                                                                        m_pkPSystemType->m_kParticleBehaviour.m_kStartSize.x )) ;
      

}

// ------------------------------------------------------------------------------------------

void PSystem::DisableParticle ( int iParticleIndex )
{
	m_kParticles[iParticleIndex].m_bActive = false;

	int i;

	// last...
	for ( i = m_uiLastParticle; i > m_uiFirstParticle; i-- )
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

void PSystem::TimeoffSet (bool bUseAge)
{
	// create particles
	for ( int i = 0; i < m_pkPSystemType->m_kPSystemBehaviour.m_iMaxParticles; i++ )
	{
		Particle kNewParticle;

		m_kParticles.push_back ( kNewParticle );

		ResetParticle ( m_kParticles.size() - 1, i * m_pkPSystemType->m_kPSystemBehaviour.m_fParticlesPerSec );
	}

}

// ------------------------------------------------------------------------------------------

void PSystem::TestInsideFrustum()
{
	Frustum *pkFrustum = m_pkFps->GetCam()->GetFrustum();

	// test culling
	if ( m_pkPSystemType->m_kPSystemBehaviour.m_kCullingTest == "cube" )
	{
		Vector3 kScale = m_pkPSystemType->m_kPSystemBehaviour.m_kMaxSize;

		Vector3 kPos = m_kPosition + 
							m_pkPSystemType->m_kPSystemBehaviour.m_kPosOffset;
		
		if ( m_pkPSystemType->m_kParticleBehaviour.m_bStartSpeedInheritDirection )
			m_bInsideFrustum = pkFrustum->CubeInFrustum ( kPos, m_pkPSystemType->m_kPSystemBehaviour.m_kCullPosOffset,
																		 kScale, m_kRotation );
		else
			m_bInsideFrustum = pkFrustum->CubeInFrustum ( kPos, m_pkPSystemType->m_kPSystemBehaviour.m_kCullPosOffset,
																		 kScale, Matrix4(1,0,0,0,
																							  0,1,0,0,
																							  0,0,1,0,
																							  0,0,0,1) );

	}
	else if ( m_pkPSystemType->m_kPSystemBehaviour.m_kCullingTest == "point" )
	{		
		Vector3 kPos = m_kPosition + 
							m_pkPSystemType->m_kPSystemBehaviour.m_kPosOffset;

		if ( m_pkPSystemType->m_kParticleBehaviour.m_bStartSpeedInheritDirection )
			m_bInsideFrustum = pkFrustum->PointInFrustum ( m_kRotation.VectorRotate( kPos ) );
		else
			m_bInsideFrustum = pkFrustum->PointInFrustum ( kPos );
	}
	else
		m_bInsideFrustum = true;
}