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

class P_DMGun;

enum eSTATE 
{ 
	IDLE			= 0, 
	DEAD			= 1, 
	CROUCH			= 2, 
	PANIC			= 3, 
	AGGRESIVE		= 4,
	SHOOTING		= 5
};

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
		float		m_fAim;
			
		int		m_fExperience;
		int		m_fNextLevel;
		int		m_iLevel;

		DMCharacterStats();
		
		void Randomize();
		void Set(DMCharacterStats& kOther);
		void Print();
};


enum eOrders
{
	eWalk =	0,
	ePickup = 1,
	eUse = 	2,
	eAttack = 3,
	eEnterHQ = 4,
	eClickMe = 5,
};

class MCOMMON_API DMOrder
{
	public:
		int		m_iOrderType;
		
		Vector3	m_kPosition;
		int		m_iEntityID;
};

class MCOMMON_API P_DMCharacter: public Property 
{
	private:
		vector<PropertyValues> GetPropertyValues();
	
		DMCharacterStats	m_kStats;
		ZFAudioSystem* m_pkAudioSys;

		bool m_bCharacterIdle;
		bool m_bPlayWalkSound;

		queue<DMOrder>			m_kOrderQueue;
		bool					m_bNewOrder;

		void MakeStringLowerCase(string& s);
		void UpdateOrders();
		bool HandleOrder(DMOrder* pkOrder,bool bNew);
		
		int					m_iState; // dead, crouching, shooting, idle..etc

	public:
		//item slots
		DMContainer*		m_pkBackPack;
		DMContainer*		m_pkBody;
		DMContainer*		m_pkBelt;
		DMContainer*		m_pkHand;
		DMContainer*		m_pkImplants;

		int					m_iLeaderOfTeam; // value is leader of that team. -1 is no leader and is def.
		int					m_iTeam;
		int	GetState()		{ return m_iState; }

		Vector3				m_kTarget; // shooting target

		vector<string> m_vkMoveSounds;	// fått en action att gå någonstans
		vector<string> m_vkSelectSounds;		// har blivit markerad
		vector<string> m_vkDeathSounds; // dör
		vector<string> m_vkOffenciveActionQuots; // försöker tuffa sig
		vector<string> m_vkDefenciveActionQuots; // blir skadad, ber om nåd etc
		vector<string> m_vkPickUpSounds; // plockar upp, undersöker något
		
		DMCharacterStats* GetStats() {return &m_kStats;};
		void Damage(int iType,int iDmg);

		void AddOrder(DMOrder kOrder) {m_kOrderQueue.push(kOrder);};
		void ClearOrders() {while(!m_kOrderQueue.empty()) m_kOrderQueue.pop();m_bNewOrder = true;};

		void DropAllItems();

		// Set right animation and calls shoot function i p_dmgun
		void Shoot (Vector3 kLocation);
		bool WalkSoundIsPlaying() { return m_bPlayWalkSound; }

		// add and removes item bonuses (defence, speed, life etc..)
		void Equip (P_DMItem* pkDMItem);
		void UnEquip (P_DMItem* pkDMItem);

		// If bIndexIsItemType = true, then iItemIndex is a value of type eDMItemTypes  
		// and the function looks for the first item item of that index.
		// Else ItemIndex is a slotinde, ie. = 0,1,2,3 ie. slot (0,0), (0,1).
		void UseQuickItem(int iItemIndex, bool bIndexIsItemType); 

		// idle, run, dead, panic etc
		void ChangeState (int iState);
		
		// Speed pathfinding is using
		void SetMoveSpeed (float fSpeed);		
		void AddMoveSpeed (float fSpeed);

		// XP
		void AddXP(int iXP);

		P_DMGun*	GetGun ();
		
		void Init();		
		void Update();		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		bool HasOrders()						{ return m_kOrderQueue.size(); }
		
		P_DMCharacter();
		~P_DMCharacter();
		
};

MCOMMON_API Property* Create_P_DMCharacter();

#endif





