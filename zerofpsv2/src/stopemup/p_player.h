#ifndef _P_PLAYER_H_
#define _P_PLAYER_H_

#include "../zerofpsv2/engine/property.h"

class StopEmUp;

class P_Player : public Property
{
	private:
		StopEmUp*	m_pkStopEmUp;
	
		float		m_fInv;
			
		
	public:
		int		m_iEnergy;
		int		m_iMaxEnergy;
		int		m_iScore;
		string	m_strGunName;
					
		P_Player();

		void Touch(int iID);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		
		
		
		void AddScore(int iScore)			{	m_iScore+= iScore;SetNetUpdateFlag(true);};
		void Damage(int iDmg);
};


Property* Create_P_Player();

#endif

