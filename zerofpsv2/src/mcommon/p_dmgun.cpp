#include "p_dmgun.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"

P_DMGun::P_DMGun()
{
	strcpy(m_acName,"P_DMGun");		
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	bNetwork = true;
	
	m_iSortPlace	=	10;
	
	m_fTimeBulletFired = 			0;
	m_fTimeFired = 					0;
	m_fBurstLength = 					0.5;
	m_bFireing =						false;
	m_bFirstUpdateSinceFireing =	false;
	m_kGunOffset.Set(0,0,0);

	m_kDir.Set(0,0,-1);
		
	//default gun
	m_strName = 			"YberGun";
	m_strSound =			"data/sound/auto_gun.wav";
	m_fFireRate = 			30;
	m_fRange = 				10;
	m_iAmmo = 				10000;
	m_iMaxAmmo = 			10000;
	m_fRandom = 			10.0;
	m_fDamage =				50;
	m_iBulletsPerAmmo =	1;
	m_iTeam = 				-1; // belongs to no team

	m_strBarrelFirePS = "data/script/objects/t_bfire_gun.lua";

	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
}

P_DMGun::~P_DMGun()
{


}

void P_DMGun::Init()
{
	//cout<< "New GUN created"<<endl;
	m_iHitSparkleTextureID = m_pkObject->m_pkZeroFps->m_pkTexMan->Load("data/textures/dm/gun_sparkle.tga", 0);
	m_iGunFireTextureID =  m_pkObject->m_pkZeroFps->m_pkTexMan->Load("data/textures/gunfire.tga", 0);
}

bool P_DMGun::Fire(Vector3 kTarget)
{	
	// update target position
	Vector3 kTemp = m_pkObject->GetWorldPosV();
	m_fTargetDist = kTarget.DistanceTo(kTemp);

	m_kDir = kTarget - (m_pkObject->GetWorldPosV() + m_kGunOffset);

	if ( m_bFireing )
		return false;

	m_bFirstUpdateSinceFireing = true;
	m_bFireing = true;

	float t = m_pkObjMan->GetSimTime();
	m_fTimeFired = t;

	// Barrelfire
	if ( m_strBarrelFirePS.size() && m_iAmmo )
	{
		Entity *pkBF = m_pkObject->m_pkEntityMan->CreateObjectFromScriptInZone(m_strBarrelFirePS.c_str(), 
			m_pkObject->GetWorldPosV());
		
		pkBF->SetWorldPosV(Vector3(0,0,0));
		pkBF->SetRelativeOri(true);
		pkBF->SetParent(m_pkObject);
	}

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
		//barellfire
		if(m_bFireing)
		{
			//m_strBarrelFirePS
			static bool bOn = true;
			
			if(bOn)
			{
				bOn=false;				
			}else
			{
				bOn=true;
				return;
			}
		/*
			Vector3 kDir = m_pkObject->GetWorldRotM().VectorTransform(Vector3(0,0,1)).Unit();
			Vector3 kPos = m_pkObject->GetIWorldPosV();
			
			kDir*= 0.9+(rand()%100 / 1000.0);
			
			Vector3 kOffset = m_pkObject->GetWorldRotM().VectorTransform(Vector3(0.2,0,0));
			
			m_pkZeroFps->m_pkRender->Polygon4(kPos+kOffset,kPos-kOffset,kPos+kDir-kOffset,kPos+kDir+kOffset,m_iGunFireTextureID);*/
		}
		
		
		float f;
		Vector3 kColor;
		for(unsigned int i = 0 ;i<m_kHitPos.size();i++)
		{
			f = (t - m_kHitPos[i].second )*2;
			kColor.Set(0.8f - f, 0.8f - f, 0.8f - f);
		
			// draw hit-sparkle
			m_pkZeroFps->m_pkRender->DrawBillboard(
				m_pkObject->m_pkZeroFps->GetCam()->GetModelViewMatrix(), 
				m_kHitPos[i].first, 0.4, m_iHitSparkleTextureID);
		}
	
		//vector<pair<Vector3,float> >::iterator kIte2;
	   for ( vector<pair<Vector3,float> >::iterator kIte = m_kHitPos.begin(); kIte != m_kHitPos.end(); kIte++ )	
		{	
			if( (t - (*kIte).second ) > 0.08)
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

	//m_fFireRate  = 50;
	//m_iAmmo = 100;

	//how many bullets to fire?	
	int iAmmoToFire = int((t - m_fTimeBulletFired) * m_fFireRate);
	
	//has the trigger just been pressed, if so make sure we only fire one round
	if(m_bFirstUpdateSinceFireing)
	{
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

		// fAim = degrees
		float fRand = sin(m_fRandom / (180.f / PI)) * m_fTargetDist;

		Vector3 kDir = m_kDir.Unit() + Vector3( (rand()%int(fRand*1000)) / 1000.f - (fRand / 2.f),
														 (rand()%int((fRand/3.0)*1000)) / 1000.f - ((fRand/3.0) / 2.f),
														 (rand()%int(fRand*1000)) / 1000.f - (fRand / 2.f) );

										 
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
					d = kStart.DistanceTo(cp);
	
					if(d < closest)
					{
						
						// annoying test, hittest checks with bindpose,
						// so dead models aren't hit as if they are standing :(
						if( P_DMCharacter* pkChar = (P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter") )
						{
							if ( pkChar->GetStats()->m_iLife > 0 && m_iTeam != pkChar->m_iTeam )
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
			if( P_Tcs* pkTcs = (P_Tcs*)pkClosest->GetProperty("P_Tcs") )
			{
				pkTcs->ApplyImpulsForce(kPickPos,(kPickPos - kStart).Unit(),false);
			}						
			
			
			// smoke psystem
			Entity* pkSmoke = m_pkObject->m_pkEntityMan->CreateObjectFromScript("data/script/objects/t_gunsmoke.lua");
			pkSmoke->SetWorldPosV (kPickPos);
			pkSmoke->AttachToZone();

			// so half the sparkle doesn't always get stuck in the ground
			kPickPos.y += 0.06;
			m_kHitPos.push_back(pair<Vector3,float>(kPickPos,t));		
			

			
			if(P_DMCharacter* pkChar = (P_DMCharacter*)pkClosest->GetProperty("P_DMCharacter"))
			{
					// Inform the victim that he have been hurt by someone.
					//if(P_ScriptInterface* pkSi = (P_ScriptInterface*)
					//	pkClosest->GetProperty("P_ScriptInterface"))
					//{
						vector<ARG_DATA> kParams;

						int iFoe = m_pkObject->GetEntityID();

						ARG_DATA kData;
						kData.eType = tINT;
						kData.pData = &iFoe;
						kParams.push_back (kData);

						m_pkObjMan->CallFunction(pkClosest, "OnTakingDmgFrom", &kParams);
					//}

					pkChar->Damage(0, int(m_fDamage));
			}
		}
		else
		{			
			// smoke psystem
			Entity* pkSmoke = m_pkObject->m_pkEntityMan->CreateObjectFromScript("data/script/objects/t_gunsmoke.lua");
			pkSmoke->SetWorldPosV (kPickPos);
			pkSmoke->AttachToZone();

			// so half the sparkle doesn't always get stuck in the ground
			kPickPos.y += 0.06;
			m_kHitPos.push_back(pair<Vector3,float>(kStart+kDir*100,t));			
		}
			
	
	}
	
	return true;
}


void P_DMGun::Save(ZFIoInterface* pkPackage)
{		
	char temp[128];

	// Saving gun stats
	pkPackage->Write ( &m_fFireRate, sizeof (m_fFireRate), 1 );
	pkPackage->Write ( &m_fDamage, sizeof (m_fDamage), 1 );
	pkPackage->Write ( &m_fRange, sizeof (m_fRange), 1 );
	pkPackage->Write ( &m_iAmmo, sizeof (m_iAmmo), 1 );
	pkPackage->Write ( &m_iMaxAmmo, sizeof (m_iMaxAmmo), 1 );
	pkPackage->Write ( &m_fRandom, sizeof (m_fRandom), 1 );
	pkPackage->Write ( &m_iBulletsPerAmmo, sizeof (m_iBulletsPerAmmo), 1 );
	pkPackage->Write ( &m_fTimeFired, sizeof (m_fTimeFired), 1 );
	pkPackage->Write ( &m_fTimeBulletFired, sizeof (m_fTimeBulletFired), 1 );
	pkPackage->Write ( &m_fBurstLength, sizeof (m_fBurstLength), 1 );
	pkPackage->Write ( &m_bFireing, sizeof (m_bFireing), 1 );
	pkPackage->Write ( &m_bFirstUpdateSinceFireing, sizeof (m_bFirstUpdateSinceFireing), 1 );

	strcpy( temp, m_strName.c_str() );
	pkPackage->Write(temp,128,1);
	
	strcpy( temp, m_strSound.c_str() );
	pkPackage->Write(temp,128,1);
}

void P_DMGun::Load(ZFIoInterface* pkPackage)
{
		char temp[128];

	// Load gun stats
	pkPackage->Read ( &m_fFireRate, sizeof (m_fFireRate), 1 );
	pkPackage->Read ( &m_fDamage, sizeof (m_fDamage), 1 );
	pkPackage->Read ( &m_fRange, sizeof (m_fRange), 1 );
	pkPackage->Read ( &m_iAmmo, sizeof (m_iAmmo), 1 );
	pkPackage->Read ( &m_iMaxAmmo, sizeof (m_iMaxAmmo), 1 );
	pkPackage->Read ( &m_fRandom, sizeof (m_fRandom), 1 );
	pkPackage->Read ( &m_iBulletsPerAmmo, sizeof (m_iBulletsPerAmmo), 1 );
	pkPackage->Read ( &m_fTimeFired, sizeof (m_fTimeFired), 1 );
	pkPackage->Read ( &m_fTimeBulletFired, sizeof (m_fTimeBulletFired), 1 );
	pkPackage->Read ( &m_fBurstLength, sizeof (m_fBurstLength), 1 );
	pkPackage->Read ( &m_bFireing, sizeof (m_bFireing), 1 );
	pkPackage->Read ( &m_bFirstUpdateSinceFireing, sizeof (m_bFirstUpdateSinceFireing), 1 );

	pkPackage->Read(temp,128,1);
	m_strName = temp;
	
	pkPackage->Read(temp,128,1);
	m_strSound = temp;
}

void P_DMGun::Reload()
{
	m_iAmmo = m_iMaxAmmo;
	printf("reloading gun, ammo is now: %i\n", m_iAmmo);
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







