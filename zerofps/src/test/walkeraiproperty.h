#ifndef _WALKERAIPROPERTY_H_
#define _WALKERAIPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "walker.h"


using namespace std;


class WalkerAIProperty: public Property {
	private:
		ZeroFps		*m_pkFps;
		HeightMap	*m_pkMap;
		Input		*m_pkInput;

		enum {
			AIWALKER_IDLE,
			AIWALKER_WALK
			};

		Vector3 m_kTargetPos;
		float	m_kWaitTime;

		int		m_iState;

	public:
		
		WalkerAIProperty();
		
		void Update();


};


#endif



