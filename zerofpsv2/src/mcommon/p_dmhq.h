#ifndef _P_DMHQ_H_
#define _P_DMHQ_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

#include "p_dmcharacter.h"

using namespace std;

class MCOMMON_API P_DMHQ: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
		string	m_strName;
		Vector3	m_kExitOffset;
		bool		m_bActiveHQ;

		vector<DMCharacterStats>	m_kForHireList;

		int m_iMoney;
		int		m_iReputation;	// Börjar på 1 och bestämmer vilka uppdrag som finns tillgängliga,
									   // ökas när uppdrag lyckas, minskas när uppdrag misslyckas / avbryts.
		
		
		
		void Eject(Entity* pkEnt);
	
	public:
		
		P_DMHQ();
		~P_DMHQ();

		int* GetMoney()		{ return &m_iMoney; }

		// returns false if the player couln't afford the cost
		bool Pay (int iCost);

		void CloneOf(Property* pkProperty) { }
		
//		void Update();
		void Init();
				
		bool GetActive() {return m_bActiveHQ;};
		void SetActive(bool bActive) {m_bActiveHQ = bActive;};
		
		bool InsertCharacter(int iID);
		void GetCharacters(vector<int>* m_pkEntitys);
		bool EjectCharacter(int iID);
		void EjectAllCharacters();
		
		void SpawnNewCharacter(int iNr = -1);
		void RandomizeHireList(int iNr);
		vector<DMCharacterStats>* GetHireList() {return &m_kForHireList;};
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		void ChangeReputation(int mod) { m_iReputation += mod; }
		int  GetReputation() { return m_iReputation; }
		
};

MCOMMON_API Property* Create_P_DMHQ();

#endif





