#ifndef  _EFFECTSYSTEM_H_
#define  _EFFECTSYSTEM_H_

/*
	Dvoid
	effect system
*/

#include <list>
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include "p_serverunit.h"
#include "common_x.h"

using namespace std;

enum COMMON_API FX_TYPES{
	FX_88MMHIT=		0,
	FX_EXPLOSION1= 1,
	FX_88MMFIRE=	2,
	FX_HEAVYMGHIT= 3,	
};

enum COMMON_API PARTICLE_TYPES{
	PART_SMOKE,
	PART_DEBRI,
	PART_FIRE,
	PART_SMALLDEBRI,
};

class COMMON_API EffectSystem 
{
	private:
		HeightMap*		m_pkMap;
		LevelManager*	m_pkLevelMan;
		ObjectManager* m_pkObjectMan;
		Render*			m_pkRender;		
		ZeroFps*			m_pkFps;
		TextureManager* m_pkTexMan;
		
		int m_iMaxParts;
		
		struct particle
		{
			int 		iType;
			Vector3	kPos;
			Vector3	kVel;
			float		fTTL;
			float		fCTime;
		};
		
		vector<particle>	m_kParts;
		
		void AddPart(int iType,Vector3 kPos);
		void StopPart(int iID);
		
	public:
		static EffectSystem*		m_pkInstance;		
	
		EffectSystem();
		void Setup(int iMaxParts);
		
		void Update();		
		void AddFX(int iType,Vector3 kPos);
		void Draw();		
};


#endif




