#ifndef _P_WALKER_H_
#define _P_WALKER_H_

#include "../zerofpsv2/engine/property.h"

class StopEmUp;

class P_Walker : public Property
{
	private:
		StopEmUp*	m_pkStopEmUp;
	
		enum eAI_States
		{
			eKILL_PLAYERS,
			eKILL_GOAL,
			eRANDOM_WALK,
			eSLEEP,		
		};
		
		int	m_iTarget;
		float m_fParaTime;
		float m_fParaLength;
		
		void Paralize(float fTime);
		
	public:
		int m_iState;
		int m_iLife;
		
			
		P_Walker();
		void Update();

		void Touch(int iID);
		void Damage(int iDmg);
};


Property* Create_P_Walker();

#endif

