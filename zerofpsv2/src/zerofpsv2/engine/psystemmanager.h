#ifndef _ENGINE_PSYSTEMMANAGER_H_
#define _ENGINE_PSYSTEMMANAGER_H_

#include "../engine_systems/common/psystem.h"
#include "../basic/vector3.h"
#include "../basic/zfini.h"
#include "../ogl/zfpsgl.h"

#include <string>
#include <map>
	using namespace std;


const int GL_BlendSRC[8] = {0, GL_ZERO, GL_ONE, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA,
						   GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA};

const int GL_BlendDST[8] = {0, GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, 
							GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA};
	
const unsigned int ui_aGLTest[8] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER,
											 GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };
//

struct rgba {
	float r, g, b, a;
};

//

struct v2 {
	float x, y;
};

//

struct PSystemTypeData
{

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
		m_iBlendSRC,
		m_iBlendDST,
		m_iDepthTest,
		m_iMaxParticles,
		m_iPartCreateRand;

	string 
		m_kSound,
		m_kCullingTest,
		m_kCreateStyle;

	unsigned int 
		m_uiTexture,
		m_uiAlphaTest,
		m_uiDepthMask;

	bool m_bLightOn;

	vector<string> m_kProperties;
};

//

struct ParticleTypeData
{
	string 
		m_kTexture;
	
	rgba 
		m_kStartColor,
		m_kEndColor;
	
	float 
		m_fLifeTime,
		m_fStartSpeed;

	Vector3 
		m_kDirection,
		m_kForce,
		m_kWideness;

	bool 
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
	PSystem* GetPSSystem ( string kPSName );
	PSystemManager();
	static inline PSystemManager* GetInstance()				{ return m_pkInstance; }

};

#endif

