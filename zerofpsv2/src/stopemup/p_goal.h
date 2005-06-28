#ifndef _P_GOAL_H_
#define _P_GOAL_H_

#include "../zerofpsv2/engine/property.h"

class StopEmUp;

class P_Goal : public Property
{
	private:
		StopEmUp*	m_pkStopEmUp;

		
	public:
		int	m_iLives;
			
		P_Goal();
		void Touch(int iID);
};


Property* Create_P_Goal();

#endif








