#ifndef _P_PLAYER_H_
#define _P_PLAYER_H_

#include "../zerofpsv2/engine/property.h"

class StopEmUp;

class P_Player : public Property
{
	private:
		StopEmUp*	m_pkStopEmUp;
	
		float		m_fInv;
		float		m_fSecondaryTime;	
		
	public:
		int		m_iEnergy;
		int		m_iMaxEnergy;
		int		m_iScore;
		int		m_iKills;
		string	m_strGunName;
		bool		m_bSecondary;
		int		m_iStopers;
					
		P_Player();

		void Update();
		
		void Touch(int iID);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		
		
		
		void AddScore(int iScore)			{	m_iScore+= iScore;SetNetUpdateFlag(true);};
		void AddKill()							{	m_iKills++		  ;SetNetUpdateFlag(true);};
		void Damage(int iDmg);
};


Property* Create_P_Player();

#endif

