#include "movepsprop.h"
#include "../../common/psystem.h"
#include "../../common/psystemmanager.h"
#include "../../../engine/zerofps.h"
#include "../../../basic/matrix4.h"
#include "../p_psystem.h"

// ------------------------------------------------------------------------------------------

void MovePSProp::Update()
{

	// move ALL vertices...*puh*
	float *pfVertices = m_pkParent->GetVertices();

	float fFrameTime = m_pkParent->m_fFrameTime;

	vector<Particle>* pkParticles = &m_pkParent->m_kParticles;

	int i;

	// Relative movement?
//	Vector3 kRelPos = m_pkParent->m_pkParent->GetEntity()->GetWorldPosV() - m_kRelPos;
//	m_kRelPos = m_pkParent->m_pkParent->GetEntity()->GetWorldPosV();

	// if force inherits direction of PSystem ( Update velocity )
	if ( m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_bForceInheritDirection )
		for ( i = m_pkParent->Start(); i < m_pkParent->End(); i++ )
			m_pkParent->m_kParticles[i].m_kVelocity += m_pkParent->m_kRotation.VectorRotate(m_pkParent->m_kParticles[i].m_kForce * fFrameTime);
	else
		for ( i = m_pkParent->Start(); i < m_pkParent->End(); i++ )
			m_pkParent->m_kParticles[i].m_kVelocity += m_pkParent->m_kParticles[i].m_kForce * fFrameTime;

	// Update position
	for ( i = m_pkParent->Start(); i < m_pkParent->End(); i++ )
		m_pkParent->m_kParticles[i].m_kCenter += m_pkParent->m_kParticles[i].m_kVelocity * fFrameTime + 
															  m_pkParent->m_kParticles[i].m_kForce * float(pow(fFrameTime,2)/2.f);
					
 
	// get modelviewmatrix
	float *afM = new float[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, afM);
	
	Vector3 kRightVect;
	Vector3 kUpVect;
	Vector3 a,b,c,d;

	kRightVect.Set(afM[0], afM[4], afM[8]); // right_vector
	kUpVect.Set(afM[1], afM[5], afM[9]); // up_vector

	// fix for box in triangle to get size written in psystemfile

	a = -kRightVect - kUpVect;
   b = kRightVect - kUpVect;
   c = kUpVect + kRightVect;
	d = kUpVect - kRightVect;

	// which dimesions to billboard
	if ( !m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_bBillBoardY )
	{
		a.x = 0;
		a.y = 1;
		a.z = 0;
	}

	for ( i = m_pkParent->Start() * 12; i < m_pkParent->End() * 12; i += 12 )
	{

		int iPartIndex = i / 12;
		float fWidth =  pkParticles->at(iPartIndex).m_kSize.x/2.f;
		float fHeight = pkParticles->at(iPartIndex).m_kSize.y/2.f;
		
		pfVertices[i + 0] = pkParticles->at(iPartIndex).m_kCenter.x + a.x * fWidth;
		pfVertices[i + 1] = pkParticles->at(iPartIndex).m_kCenter.y + a.y * fHeight;
		pfVertices[i + 2] = pkParticles->at(iPartIndex).m_kCenter.z + a.z * fWidth;

		pfVertices[i + 3] = pkParticles->at(iPartIndex).m_kCenter.x + b.x * fWidth;
		pfVertices[i + 4] = pkParticles->at(iPartIndex).m_kCenter.y + b.y * fHeight;
		pfVertices[i + 5] = pkParticles->at(iPartIndex).m_kCenter.z + b.z * fWidth;

		pfVertices[i + 6] = pkParticles->at(iPartIndex).m_kCenter.x + c.x * fWidth;
		pfVertices[i + 7] = pkParticles->at(iPartIndex).m_kCenter.y + c.y * fHeight;
		pfVertices[i + 8] = pkParticles->at(iPartIndex).m_kCenter.z + c.z * fWidth;

		pfVertices[i + 9] = pkParticles->at(iPartIndex).m_kCenter.x + d.x * fWidth;
		pfVertices[i + 10] = pkParticles->at(iPartIndex).m_kCenter.y + d.y * fHeight;
		pfVertices[i + 11] = pkParticles->at(iPartIndex).m_kCenter.z + d.z * fWidth;
	}
	
	delete [] afM;
}

// ------------------------------------------------------------------------------------------

MovePSProp::MovePSProp ( PSystem *pkParent )
{
	m_kRelPos.Set(0,0,0);
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------
