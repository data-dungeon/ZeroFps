#ifndef _ENGINE_NETSLAVEOBJECT_H_
#define _ENGINE_NETSLAVEOBJECT_H_

#include "object.h"


class NetSlaveObject: public Object {
	private:

	public:
		NetSlaveObject();
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);
};


#endif


