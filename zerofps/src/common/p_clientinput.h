#ifndef _P_CLIENTUNIT_H_
#define _P_CLIENTUNIT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;

class COMMON_API P_ClientInput: public Property {
public:
	P_ClientInput();
	void CloneOf(Property* pkProperty) { }
	void Update();

	void Save(ZFMemPackage* pkPackage) { }
	void Load(ZFMemPackage* pkPackage) { }
};

COMMON_API Property* Create_P_ClientInput();

#endif






