#include "movepsprop.h"
#include "../../common/psystem.h"
#include "../../../engine/psystemmanager.h"
#include "../../../engine/zerofps.h"
#include "../../../basic/matrix4.h"

// ------------------------------------------------------------------------------------------

void MovePSProp::Update()
{
	// move ALL vertices...*puh*
	float *pfVertices = m_pkParent->GetVertices();

	float fFrameTime = m_pkParent->m_fFrameTime;

	vector<Particle>* pkParticles = &m_pkParent->m_kParticles;

	int i;


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
															  m_pkParent->m_kParticles[i].m_kForce * pow(fFrameTime,2)/2.f;
 
	// get modelviewmatrix
	float *afM = new float[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, afM);
	
	Vector3 x;
	Vector3 y;
	Vector3 a;
	Vector3 b;	
	Vector3 c;	

	x.Set(afM[0], afM[4], afM[8]);
	y.Set(afM[1], afM[5], afM[9]);

	a = y;
   b = x - y;
   c = -x - y;

	// which dimesions to billboard
	if ( !m_pkParent->m_pkPSystemType->m_kParticleBehaviour.m_bBillBoardY )
	{
		a.x = 0;
		a.z = 0;
		a.y = 1;
	}

	for ( i = m_pkParent->Start() * 9; i < m_pkParent->End() * 9; i += 9 )
	{
		int iPartIndex = i / 9;

		float fWidth =  pkParticles->at(iPartIndex).m_kSize.x;
		float fHeight = pkParticles->at(iPartIndex).m_kSize.y;

		pfVertices[i + 0] = pkParticles->at(iPartIndex).m_kCenter.x + a.x; 
		pfVertices[i + 1] = pkParticles->at(iPartIndex).m_kCenter.y + a.y * (fHeight / 2.333f);
		pfVertices[i + 2] = pkParticles->at(iPartIndex).m_kCenter.z + a.z;

		pfVertices[i + 6] = pkParticles->at(iPartIndex).m_kCenter.x + b.x * fWidth;
		pfVertices[i + 7] = pkParticles->at(iPartIndex).m_kCenter.y + b.y * fHeight;
		pfVertices[i + 8] = pkParticles->at(iPartIndex).m_kCenter.z + b.z * fWidth;

		pfVertices[i + 3] = pkParticles->at(iPartIndex).m_kCenter.x + c.x * fWidth;
		pfVertices[i + 4] = pkParticles->at(iPartIndex).m_kCenter.y + c.y * fHeight;
		pfVertices[i + 5] = pkParticles->at(iPartIndex).m_kCenter.z + c.z * fWidth;
	}
	
	delete [] afM;
}

// ------------------------------------------------------------------------------------------

MovePSProp::MovePSProp ( PSystem *pkParent )
{
	m_pkParent = pkParent;
}

// ------------------------------------------------------------------------------------------
