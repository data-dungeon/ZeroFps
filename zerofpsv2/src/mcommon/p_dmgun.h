#ifndef _P_DMGUN_H_
#define _P_DMGUN_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

#include "p_dmcharacter.h"

using namespace std;

class MCOMMON_API P_DMGun: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
	
		
		vector<pair<Vector3,float> >	m_kHitPos;
		
		float		m_fTimeFired;	
		float		m_fTimeBulletFired;
		float		m_fBurstLength;
		bool		m_bFireing;
		bool		m_bFirstUpdateSinceFireing;
		
		Vector3	m_kGunOffset;
		
		//gun info
		string	m_strName;
		string m_strSound;

		float		m_fFireRate;
		float		m_fDamage;
		float		m_fRange;		
		int		m_iAmmo;
		int		m_iMaxAmmo;		
		Vector3	m_kDir;
		float		m_fRandom;
		int		m_iBulletsPerAmmo;
		
		bool		FireBullets(int iAmount);

		ZFAudioSystem* m_pkAudioSys;
		

	public:
		
		P_DMGun();
		~P_DMGun();

		void CloneOf(Property* pkProperty) { }
		
		void Update();
		void Init();
		
		bool Fire(Vector3 kTarget);

		void SetFireRate	 (float fFireRate)	{ m_fFireRate = fFireRate; }
		void SetDamage		 (float fDamage)	{ m_fDamage = fDamage; }
		void SetMaxAmmo		 (int iMaxAmmo)		{ m_iMaxAmmo = iMaxAmmo; }
		void SetRandom		 (float fRandom)	{ m_fRandom = fRandom; }
		void SetRange		 (float fRange)		{ m_fRange = fRange; }
		void SetBulletsPerAmmo	 (int iBullets)	{ m_iBulletsPerAmmo = iBullets; }
		void SetSound		 (string strSound)	{ m_strSound = strSound; }
		void SetAmmo		 (int iAmmo);

		bool HasAmmo		 ()					{ return m_iAmmo; }
		bool ReadyToFire	 ()					{ return m_bFireing; }
		
//		void Save(ZFIoInterface* pkPackage);
//		void Load(ZFIoInterface* pkPackage);
		
};

MCOMMON_API Property* Create_P_DMGun();

#endif

