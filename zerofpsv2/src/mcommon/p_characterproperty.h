#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"


using namespace std;

class P_CharacterProperty: public Property
{
	private:
		string	m_strName;		
	
		bool		m_bIsPlayerCharacter;

	public:

		P_CharacterProperty();
		~P_CharacterProperty();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

};

MCOMMON_API Property* Create_P_CharacterProperty();

#endif








