#ifndef _INVENTORYPROPERTY_H_
#define _INVENTORYPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "massdriverprojectile.h"
#include "common_x.h"
//#include "playerobject.h"

using namespace std;

class COMMON_API InventoryProperty: public Property {
		
	public:
		bool m_bUse;
	
		InventoryProperty(){m_bUse=false;};
//		virtual void Use()=0;

};

//Property* Create_InventoryProperty();


#endif





