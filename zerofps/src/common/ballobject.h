#ifndef _BALLOBJECT_H_
#define _BALLOBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "common_x.h"

class COMMON_API BallObject: public Object {
	private:
		ZeroFps *m_pkFps;

	public:
		bool onGround;	

		BallObject();
		
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);
		
		bool Save(void *pkData,int iSize);
};


#endif












