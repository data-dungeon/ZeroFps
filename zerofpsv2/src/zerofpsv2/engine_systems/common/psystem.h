#ifndef _ENGINE_PSYSTEM_H_
#define _ENGINE_PSYSTEM_H_

#include "../propertys/psproperties/particleproperty.h"

#include "../../basic/math.h"
#include "../../engine/zerofps.h"

#include <vector>

using namespace std;

class P_PSystem;
class ZSSRender;

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

	Vector4 m_kStartColor;
	Vector4 m_kEndColor;

	float m_fAge;
	float m_fLifeTime;


	bool m_bActive;
};

class PSystem {
	
private:


	vector<ParticleProperty*> m_kPSProperties;

	ZShaderSystem *m_pkShader;
	ZSSZeroFps* m_pkFps;

	P_PSystem* m_pkParent;

	Vector3 
		m_kPosition;
	
	Matrix4 m_kRotation;

	LightProfile		m_kLightProfile;
	ZSSLight*			m_pkLight;

	float 
		*m_pfVertices,
		*m_pfColors,
		m_fFrameTime,
		m_fTimeSinceLastCreatedParticle,
		m_fAge,
		m_fAgePercent,
		m_fLastTime;

	Vector2* m_pkTexCoords;

	vector<Particle> m_kParticles;

	int 		
		m_iActiveParticles,
		m_uiFirstParticle,
		m_uiParticles,
		m_uiLastParticle;

	unsigned int* m_pfIndices;

	bool 
		m_bFirstRun,
		m_bInsideFrustum;

	PSystemType* m_pkPSystemType;

public:
	Vector3 m_kPosOffset;

	void TestInsideFrustum();
	bool Draw();
	bool Update( Vector3 kNewPosition, Matrix4 kNewRotation );
	void AddPSProperty ( ParticleProperty *pkPSProperty );

   float Age()                               { return m_fAge; }
   void SetAge (float fAge)                  { m_fAge = fAge; }

	float* GetVertices()								{ return m_pfVertices; }
	float* GetColors()								{ return m_pfColors;   }
	Vector2* GetTexCoords()							{ return m_pkTexCoords;}
	unsigned int* GetIndices()						{ return m_pfIndices;  }

	Vector3 GetPosition()							{ return m_kPosition;  }
	Matrix4 GetRotation()							{ return m_kRotation;  }

	void SetVertices (float *pfVertices)		{ m_pfVertices = pfVertices;   }
	void SetTexCoords (Vector2 *pkTexCoords)	{ m_pkTexCoords = pkTexCoords; }
	void SetColors	(float *pfColors)				{ m_pfColors = pfColors;       }
	void SetIndices (unsigned int *pfIndices)	{ m_pfIndices = pfIndices;		 }

	int Particles()									{ return m_uiLastParticle - m_uiFirstParticle + 1;	 }
	int Start()											{ return m_uiFirstParticle; }
	int End()											{ return m_uiLastParticle + 1; }

	void SetParticles (unsigned int uiNrOf)	{ m_uiParticles = uiNrOf;		 }

	void SetParent(P_PSystem* pkP_PSystem)		{ m_pkParent = pkP_PSystem; }

	void ResetParticle (int iParticleIndex, float fTimeOffset);
	void DisableParticle ( int iParticleIndex );

	// Make PSystem have particles from start
	void TimeoffSet ();
   
	PSystem(PSystemType* pkPSystemType);
	~PSystem();

	friend class MovePSProp;
	friend class ColorPSProp;
	friend class SizePSProp;
   friend class TexturePSProp;
	friend class LightPSProp;

	PSystemType* GetPSystemType()					{ return m_pkPSystemType; }

   friend class P_PSystem;

};

#endif








