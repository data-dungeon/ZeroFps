#include "p_dmgun.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_DMGun::P_DMGun()
{
	strcpy(m_acName,"P_DMGun");		
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	bNetwork = true;
	
	
	m_fTimeBulletFired = 0;
	m_fTimeFired = 		0;
	m_fBurstLength = 		0.5;
	m_bFireing =			false;
	m_bFirstUpdateSinceFireing = false;
	m_kGunOffset.Set(0,0,0);
	
	//default gun
	m_strName = 	"YberGun";
	m_strSound =	"data/sound/9m_pistol.wav";
	m_fFireRate = 	10;
	m_fRange = 		10;
	m_iAmmo = 		100;
	m_iMaxAmmo = 	100;
	m_kDir.Set(0,0,-1);
	m_fRandom = 	0.5;
	m_fDamage =		25;
	m_iBulletsPerAmmo = 1;

	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
}

P_DMGun::~P_DMGun()
{


}

void P_DMGun::Init()
{
	//cout<< "New GUN created"<<endl;

}

bool P_DMGun::Fire(Vector3 kTarget)
{	
	float t = m_pkObjMan->GetSimTime();

	if(!m_bFireing)
	{
		m_bFirstUpdateSinceFireing = true;
	}
	
	
	m_fTimeFired = t;
	m_bFireing = true;
	m_kDir = kTarget - (m_pkObject->GetWorldPosV() + m_kGunOffset);
	
	return true;
}

void P_DMGun::SetAmmo (int iAmmo)
{
	m_iAmmo = iAmmo;

	if ( m_iAmmo > m_iMaxAmmo )
		m_iAmmo = m_iMaxAmmo;
}

void P_DMGun::Update()
{
	static float prevAmmoPlayTime = 0;
	static float prevShotPlayTime = 0;

	float t = m_pkObjMan->GetSimTime();

	if(m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		float f;
		Vector3 kColor;
		for(int i = 0 ;i<m_kHitPos.size();i++)
		{
			f = (t - m_kHitPos[i].second )*2;
			kColor.Set(0.8-f,0.8-f,0.8-f);
		
			m_pkZeroFps->m_pkRender->Sphere(m_kHitPos[i].first,0.1,1,kColor,true);
			m_pkZeroFps->m_pkRender->SetColor(kColor);
			m_pkZeroFps->m_pkRender->Line(m_kHitPos[i].first,m_pkObject->GetWorldPosV() + m_kGunOffset);
		}
	
		//vector<pair<Vector3,float> >::iterator kIte2;
	   for ( vector<pair<Vector3,float> >::iterator kIte = m_kHitPos.begin(); kIte != m_kHitPos.end(); kIte++ )	
		{	
			if( (t - (*kIte).second ) > 0.4)
			{
				m_kHitPos.erase(kIte);
				return;
			}
		}	
	
		return;
	}
	


	if(!m_bFireing)	//firing?
		return;

	if(m_iAmmo <= 0)
	{
		if(t - prevAmmoPlayTime > 0.5f) // spela max 2 ljud/ sek
		{
			cout<<"IM OUT OF AMMO"<<endl;
			m_pkAudioSys->StartSound("data/sound/no_ammo.wav",	
				m_pkObject->GetWorldPosV(), m_kDir, false);
			prevAmmoPlayTime = t;
		}
	}
	else
	{
		prevAmmoPlayTime = 0;
	}
	
	
	if( t - m_fTimeFired > m_fBurstLength)   //should we stop fireing
	{	
		m_bFireing = false;
		return;
	}


	//how many bullets to fire?	
	int iAmmoToFire = int((t - m_fTimeBulletFired) * m_fFireRate);
	
	//has the trigger just been pressed, if so make sure we only fire one round
	if(m_bFirstUpdateSinceFireing)
	{
		cout<<"tried to fire:"<<iAmmoToFire<<endl;
		m_bFirstUpdateSinceFireing = false;
		iAmmoToFire = 1;		
		prevShotPlayTime = 0;
	}
	
	//check that we have enough ammo
	if(iAmmoToFire > m_iAmmo)
		iAmmoToFire = m_iAmmo;	
	
	//if no bullets are to be fired, we return
	if(iAmmoToFire <= 0)
	{
		return;
	}
	
	m_fTimeBulletFired = t;
	
	//play sound
	if(t - prevShotPlayTime > 1.0f) // spela max 1 ljud/ sek (eller när nytt skott avlossas)
	{
		m_pkAudioSys->StartSound(m_strSound, m_pkObject->GetWorldPosV(), m_kDir, false);
		prevShotPlayTime = t;
	}
	
	FireBullets(iAmmoToFire * m_iBulletsPerAmmo);

	//remove ammo
	m_iAmmo-=iAmmoToFire;	

}

bool P_DMGun::FireBullets(int iAmount)
{
	float t = m_pkObjMan->GetSimTime();
	Vector3 kStart = m_pkObject->GetWorldPosV() + m_kGunOffset;
//	float dist = (kStart - m_kDir).Length();
//	Vector3 kSDir = (m_kDir - kStart).Unit();
	
	vector<Entity*> kObjects;		
	m_pkObjMan->GetZoneObject()->GetAllEntitys(&kObjects);	


	for(int i =0;i<iAmount;i++)
	{
		//Vector3 kDir = (m_kDir +  Vector3( (rand() % int(m_fRandom*1000) / 1000.0)-0.5, (rand() % int(m_fRandom*1000) / 1000.0)-0.5,(rand() % int(m_fRandom*1000) / 1000.0)-0.5)).Unit();
		Vector3 kDir = m_kDir + Vector3( (rand() % 1000 / 1000.0)-0.5, (rand() % 1000 / 1000.0)-0.5,(rand() % 1000 / 1000.0)-0.5) * m_fRandom;
		//Vector3 kDir = (kSDir + Vector3( (rand() % 1000 / 1000.0)-0.5, (rand() % 1000 / 1000.0)-0.5,(rand() % 1000 / 1000.0)-0.5) * m_fRandom) * dist;
		//Vector3 kDir = kStart + (kSDir +  (Vector3( (rand() % 1000 / 1000.0)-0.5, (rand() % 1000 / 1000.0)-0.5,(rand() % 1000 / 1000.0)-0.5) * m_fRandom)) * dist;
	
		float d;	
		Vector3 cp;
		float closest = 999999999;
		Entity* pkClosest = NULL;
		Vector3 kPickPos;
		for(unsigned int i=0;i<kObjects.size();i++)
		{
			if(kObjects[i] == m_pkObject)
				continue;
		
			//get mad property and do a linetest		
			if(P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
			{
				if(mp->TestLine(kStart,kDir))
				{	
					cp = mp->GetLastColPos();
					d = (kStart - cp).Length();
	
					if(d < closest)
					{
						// annoying test, hittest checks with bindpose,
						// so dead models are hit as if they are standing :(
						if( P_DMCharacter* pkChar = (P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter") )
						{
							if ( pkChar->GetStats()->m_iLife > 0 )
							{
								closest = d;
								pkClosest = kObjects[i];
								kPickPos = cp;
							}
						}
						else
						{
							closest = d;
							pkClosest = kObjects[i];
							kPickPos = cp;
						}
					}				
				}
			}				
		}	
		
		if(pkClosest)
		{
			m_kHitPos.push_back(pair<Vector3,float>(kPickPos,t));			
			
			
			if(P_DMCharacter* pkChar = (P_DMCharacter*)pkClosest->GetProperty("P_DMCharacter"))
					pkChar->Damage(0,m_fDamage);
		}
		else
			m_kHitPos.push_back(pair<Vector3,float>(kStart+kDir*100,t));			
			
	
	}
	
	return true;
}


vector<PropertyValues> P_DMGun::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_DMGun()
{
	return new P_DMGun;
}







