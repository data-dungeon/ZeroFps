#ifndef _P_DMGAMEINFO_H_
#define _P_DMGAMEINFO_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class P_DMGameInfo: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
	
	public:
		//clan name and logo info
		string	m_strClanName;
		string	m_strClanColor;
		string	m_strClanLogo;		
		
		int		m_iMoney;
				
		Vector3	m_kCameraPos;		
		
		
		
		P_DMGameInfo();
		~P_DMGameInfo();

		void CloneOf(Property* pkProperty) { }		
		void Init();		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
};

Property* Create_P_DMGameInfo();

#endif