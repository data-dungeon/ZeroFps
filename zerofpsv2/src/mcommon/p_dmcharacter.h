#ifndef _P_DMCHARACTER_H_
#define _P_DMCHARACTER_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

#include "dmcontainer.h"


using namespace std;

enum eSTATE { eIDLE, eDEAD, eCROUCH };
class MCOMMON_API DMCharacterStats
{
	public:
		string	m_strName;
		string	m_strIcon; // porträtt
		
		int		m_iLife;
		int		m_iMaxLife;

		float		m_fSpeed;	
		float		m_fArmour;
		float		m_fWage;
			
		int		m_fExperience;
		int		m_fNextLevel;
		int		m_iLevel;
			

		DMCharacterStats();
		
		void Randomize();
		void Set(DMCharacterStats& kOther);
		void Print();
};

class MCOMMON_API P_DMCharacter: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
		DMCharacterStats	m_kStats;
		ZFAudioSystem* m_pkAudioSys;

		bool m_bCharacterIdle;
		bool m_bPlayWalkSound;
	
	public:
		//item slots
		DMContainer*		m_pkBackPack;
		DMContainer*		m_pkBody;
		DMContainer*		m_pkBelt;
		DMContainer*		m_pkHand;
		DMContainer*		m_pkImplants;
		
		int					m_iTeam;
		int					m_iState; // dead, crouching, shooting, idle..etc

		vector<string> m_vkMoveSounds;
		vector<string> m_vkDeathSounds;
		vector<string> m_vkOffenciveActionQuots;
		vector<string> m_vkDefenciveActionQuots;
		vector<string> m_vkPickUpSounds;
		
		DMCharacterStats* GetStats() {return &m_kStats;};
		void Damage(int iType,int iDmg);

		// Set right animation and calls shoot function i p_dmgun
		void Shoot (Vector3 kLocation);
		bool WalkSoundIsPlaying() { return m_bPlayWalkSound; }
		
		
		void Init();		
		void Update();		
		void CloneOf(Property* pkProperty) { }		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		P_DMCharacter();
		~P_DMCharacter();
		
};

MCOMMON_API Property* Create_P_DMCharacter();

#endif





