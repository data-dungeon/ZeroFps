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
		int	m_iGoalID;
		
		float m_fParaTime;
		float m_fParaLength;		
		float	m_fFindNewTargetTime;
		
		bool	m_bSetSize;
		
		void Paralize(float fTime);
		int ClosestPlayer();
		void CreateBonus();
		
	public:
		int 	m_iState;
		int 	m_iLife;
		int	m_iMaxLife;
		
			
		P_Walker();
		void Update();

		void Touch(int iID);
		void Damage(int iDmg,int iKiller);
};


Property* Create_P_Walker();

#endif

