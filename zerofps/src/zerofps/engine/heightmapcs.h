#ifndef _HEIGHTMAPCS_H_
#define _HEIGHTMAPCS_H_

#include "../basic/basic.pkg"
#include "collisionshape.h"
#include "engine_x.h"

using namespace std;

class CSSphere;

class ENGINE_API HeightMapCS : public CollisionShape
{
	

	public:
		HeightMap* m_pkMap;

		HeightMapCS(HeightMap* pkMap);		
		CollisionData* Test(CollisionShape* kOther,float fTime,bool bContinue);
		CollisionData* Collide_CSSphere(CSSphere* kOther,float fTime);

};

#endif








