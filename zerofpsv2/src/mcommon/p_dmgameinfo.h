#ifndef _P_DMGAMEINFO_H_
#define _P_DMGAMEINFO_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_DMGameInfo: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
		int		m_iReputation;	// Börjar på 1 och bestämmer vilka uppdrag som finns tillgängliga,
										// ökas när uppdrag lyckas, minskas när uppdrag misslyckas / avbryts.

	public:
		//clan name and logo info
		string	m_strClanName;
		string	m_strClanColor;
		string	m_strClanLogo;		
		
		int		m_iMoney;
				
		Vector3	m_kCameraPos;		


		
		
		void ChangeReputation(int mod) { m_iReputation += mod; }
		int  GetReputation() { return m_iReputation; }
		
		P_DMGameInfo();
		~P_DMGameInfo();

		void CloneOf(Property* pkProperty) { }		
		void Init();		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
};

MCOMMON_API Property* Create_P_DMGameInfo();

#endif