#ifndef _ENGINE_NETSLAVEOBJECT_H_
#define _ENGINE_NETSLAVEOBJECT_H_

#include "objects.pkg"
#include "../basic/basic.pkg"

class NetSlaveObject: public Object {
	private:

	public:
		NetSlaveObject();
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);
};


#endif


