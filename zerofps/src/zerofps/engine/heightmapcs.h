#ifndef _ENGINE_HEIGHTMAPCS_H_
#define _ENGINE_HEIGHTMAPCS_H_

#include "../basic/basic.pkg"
#include "collisionshape.h"
#include "engine_x.h"

using namespace std;

class CSSphere;
class CSBox;

class ENGINE_API HeightMapCS : public CollisionShape
{
	

	public:
		HeightMap* m_pkMap;

		HeightMapCS(HeightMap* pkMap);		
		Collision* Test(CollisionShape* kOther,bool bContinue);
		Collision* Collide_CSSphere(CSSphere* kOther);
		Collision* Collide_CSBox(CSBox* kOther);		

};

#endif








