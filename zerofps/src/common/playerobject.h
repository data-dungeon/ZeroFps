#ifndef _PLAYEROBJECT_H_
#define _PLAYEROBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "playercontrolproperty.h"
#include "ballobject.h"
#include "massdriverproperty.h"
#include "machinegunproperty.h"
#include "statusproperty.h"
#include "common_x.h"

class COMMON_API PlayerObject: public Object {
	private:
		ZeroFps *m_pkFps;

	public:
		PlayerObject(HeightMap *pkMap,Input *pkInput);
};


#endif



