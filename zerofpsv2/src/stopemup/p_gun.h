#ifndef _P_GUN_H_
#define _P_GUN_H_

#include "../zerofpsv2/engine/property.h"


class P_Gun : public Property
{
	private:
		float m_fLastFire;
		
		string	m_strSound;
		
		void FireBullet();
		
	public:
		bool		m_bFire;
		float 	m_fFireDelay;
		float 	m_fBulletVel;
		int		m_iDamage;
		string 	m_strProjectile;
		string	m_strDirectHitObject;
		int		m_iProjectiles;
		float		m_fAngle;
		
		P_Gun();
	
		
		void Update();
		void SetSound(string strSound);
};


Property* Create_P_Gun();

#endif



