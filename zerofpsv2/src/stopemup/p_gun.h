#ifndef _P_GUN_H_
#define _P_GUN_H_

#include "../zerofpsv2/engine/property.h"


class P_Gun : public Property
{
	private:
		float m_fLastFire;
	
		
		void FireBullet();
		
	public:
		bool		m_bFire;
		float 	m_fFireDelay;
		float 	m_fBulletVel;
		int		m_iDamage;
		string 	m_strProjectile;
		string	m_strDirectHitObject;
		string	m_strSound;
		
		P_Gun();
	
		
		void Update();

};


Property* Create_P_Gun();

#endif



