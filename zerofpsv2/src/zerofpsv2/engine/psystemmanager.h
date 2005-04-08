#ifndef _ENGINE_PSYSTEMMANAGER_H_
#define _ENGINE_PSYSTEMMANAGER_H_

#include "../engine_systems/common/psystem.h"
#include "../basic/vector3.h"
#include "../basic/zfini.h"

#include <string>
#include <map>
	using namespace std;

//

struct v2 {
	float x, y;
};

//

struct PSystemTypeData
{
	float m_fLightStartColorR,
			m_fLightStartColorG,
			m_fLightStartColorB,
			m_fLightEndColorR,
			m_fLightEndColorG,
			m_fLightEndColorB,
			m_fLightStartQuadAtt,
			m_fLightEndQuadAtt,
			m_fLightStartConstAtt,
			m_fLightEndConstAtt,
			m_fLightStartLinAtt,
			m_fLightEndLinAtt;

	Vector3 
		m_kStart_OuterStartArea,
		m_kStart_InnerStartArea,
		m_kEnd_OuterStartArea,
		m_kEnd_InnerStartArea,
		m_kPosOffset,
		m_kMaxSize,
		m_kCullPosOffset;

	float 
		m_fLifeTime,
		m_fStartArea,
		m_fParticlesPerSec;
	
	int 
		m_iMaxParticles,
		m_iPartCreateRand;

	string 
		m_kSound,
		m_kCullingTest,
		m_kCreateStyle;

	bool 
      m_bRemoveParentOnFinish;

	vector<string> m_kProperties;
};

//

struct ParticleTypeData
{
	string 
		m_strMaterialFile;

	ZFResourceHandle* m_pkMaterial;	

	Vector4 
		m_kStartColor,
		m_kEndColor,
		m_kStartColorRandom,
		m_kEndColorRandom;
	
	float 
		m_fLifeTime,
		m_fStartSpeed;

	Vector3 
		m_kDirection,
		m_kForce,
		m_kWideness;

	bool 
		m_bSameEndColorRandom,
		m_bSameStartColorRandom,
		m_bBillboardX,
		m_bBillBoardY,
		m_bBillBoardZ,
		m_bForceInheritDirection,
		m_bStartSpeedInheritDirection;

	int
		m_iLifeTimeRandom,
		m_iStartSizeRandom,
		m_iEndSizeRandom,
		m_iStartSpeedRand;
	

	v2 
		m_kStartSize,
		m_kEndSize;

};

//

struct PSystemType
{
   string m_kName;
	ParticleTypeData m_kParticleBehaviour;
	PSystemTypeData m_kPSystemBehaviour;
};


class PSystemManager {

private:
	map<string, PSystemType> m_kPSystemTypes;

	static PSystemManager *m_pkInstance;

	// Creating PS
	float* CreateTexCoords (int iParticles);
	float* CreateVertices ( PSystemType *kPSystemType );
	float* CreateVerticeColors ( int iParticles );
	unsigned int* CreateIndices ( int iParticles );

	// Loading PS from file
	bool LoadNewPSystem ( string kName );
	bool LoadData ( PSystemType *pkPSType );
	void SetProperties ( PSystemType *pkPSType );
	void CalculateMaxSize ( PSystemType *pkPSType );

	ZFIni m_kIniLoader;

public:
	PSystem* GetPSystem ( string kPSName );
	PSystemManager();
	static inline PSystemManager* GetInstance()				{ return m_pkInstance; }

};

#endif

