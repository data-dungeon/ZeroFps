#ifndef _ENGINE_PSYSTEM_H_
#define _ENGINE_PSYSTEM_H_

#include "../propertys/psproperties/particleproperty.h"

#include "../../basic/vector3.h"
#include "../../basic/matrix4.h"
#include "../../basic/vector2.h"

#include <vector>

using namespace std;


class Render;

struct PSystemType;

struct Particle
{
	Vector3 
		m_kCenter,
		m_kForce,
		m_kVelocity;

	Vector2 m_kSize;
	Vector2 m_kEndSize;
	Vector2 m_kStartSize;

	float m_fAge;
	float m_fLifeTime;

	bool m_bActive;
};

class PSystem {
	
private:

	vector<ParticleProperty*> m_kPSProperties;
	
	Render* m_pkRender;

	Vector3 
		m_kPosition,
		m_kPosOffset;
	
	Matrix4 m_kRotation;

	float 
		*m_pfVertices,
		*m_pfColors,
		*m_pfTexCoords,
		m_fFrameTime,
		m_fTimeSinceLastCreatedParticle,
		m_fAge,
		m_fAgePercent;


	vector<Particle> m_kParticles;

	unsigned int 		
		m_uiActiveParticles;
	int
		m_uiFirstParticle,
		m_uiParticles,
		m_uiLastParticle;

	unsigned int* m_pfIndices;

	bool 
		m_bFirstRun,
		m_bInsideFrustum;

	PSystemType* m_pkPSystemType;

public:
	void TestInsideFrustum();
	void Draw();
	void Update( Vector3 kNewPosition, Matrix4 kNewRotation );
	void AddPSProperty ( ParticleProperty *pkPSProperty );

	float* GetVertices()								{ return m_pfVertices; }
	float* GetColors()								{ return m_pfColors;   }
	float* GetTexCoords()							{ return m_pfTexCoords;}
	unsigned int* GetIndices()						{ return m_pfIndices;  }

	Vector3 GetPosition()							{ return m_kPosition;  }
	Matrix4 GetRotation()							{ return m_kRotation;  }

	void SetVertices (float *pfVertices)		{ m_pfVertices = pfVertices;   }
	void SetTexCoords (float *pfTexCoords)		{ m_pfTexCoords = pfTexCoords; }
	void SetColors	(float *pfColors)				{ m_pfColors = pfColors;       }
	void SetIndices (unsigned int *pfIndices)	{ m_pfIndices = pfIndices;		 }

	int Particles()									{ return m_uiLastParticle - m_uiFirstParticle;	 }
	int Start()											{ return m_uiFirstParticle; }
	int End()											{ return m_uiLastParticle + 1; }

	void SetParticles (unsigned int uiNrOf)	{ m_uiParticles = uiNrOf;		 }

	void ResetParticle (int iParticleIndex, float fTimeOffset);
	void DisableParticle ( int iParticleIndex );

	// Make PSystem have particles from start
	void TimeoffSet ();

	unsigned int GetAlphaTestValue();
	unsigned int GetDepthValue();

	PSystem(PSystemType* pkPSystemType);
	~PSystem();

	friend class MovePSProp;
	friend class ColorPSProp;
	friend class SizePSProp;

	PSystemType* GetPSystemType()					{ return m_pkPSystemType; }
};

#endif








