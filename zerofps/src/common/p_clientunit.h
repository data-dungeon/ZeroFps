/* Richard Svensson

	Client side property for units in zerorts

*/


#ifndef _P_CLIENTUNIT_H_
#define _P_CLIENTUNIT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;


class COMMON_API P_ClientUnit: public Property {
	private:
	
	public:
		unsigned char		m_cTeam;
		unsigned char		m_cHealth;
		unsigned char		m_cWeapon;
		unsigned char		m_cArmor;
		unsigned char		m_cPropultion;
		string				m_kName;
		bool					m_bSelected;
	
	
		P_ClientUnit();
		void CloneOf(Property* pkProperty) { }

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);

};

COMMON_API Property* Create_P_ClientUnit();


#endif






