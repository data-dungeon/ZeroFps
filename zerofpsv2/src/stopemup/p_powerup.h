#ifndef _P_POWERUP_H_
#define _P_POWERUP_H_

#include "../zerofpsv2/engine/property.h"


class P_Powerup : public Property
{
	private:

			
	public:
		int	m_iPowerupType;
	
		
		P_Powerup();
		void Touch(int iID);
};


Property* Create_P_Powerup();

#endif

