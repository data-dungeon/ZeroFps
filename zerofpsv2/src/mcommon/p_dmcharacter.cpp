#include "p_dmcharacter.h" 
#include "p_dmgun.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h" 
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"

#include "../zerofpsv2/engine/p_pfpath.h"

// ---- DMCharacterStats
DMCharacterStats::DMCharacterStats()
{
	m_strName = 	"NoName";
	m_strIcon =		"portrait1.bmp";
			
	m_iLife = 		100;
	m_iMaxLife =	100;
		
	m_fSpeed =		1;
	m_fArmour =		10;
	m_fWage =		10;
	m_fAim =		1;
				
	m_fExperience =0;
	m_fNextLevel = 1000;
	m_iLevel =		1;
}

void DMCharacterStats::Randomize()
{
	vector<string>	kNames;
		kNames.push_back("Dvoid");
		kNames.push_back("Vim");
		kNames.push_back("Zeb");
		kNames.push_back("Zerom");
		kNames.push_back("Eld");
		kNames.push_back("Arnold");
		kNames.push_back("BIG");
		kNames.push_back("Cobra");
		kNames.push_back("Captain");
		kNames.push_back("Data");
		kNames.push_back("Lotta");

	vector<string>	kSNames;
		kSNames.push_back("Larsson");
		kSNames.push_back("Andersson");
		kSNames.push_back("Svensson");
		kSNames.push_back("Kallsong");
		kSNames.push_back("Blubbson");
		kSNames.push_back("Arnoldsson");


	m_strName = 		kNames[rand()%kNames.size()] + string(" ") + kSNames[rand()%kSNames.size()];
	m_strIcon =			"portrait1.bmp";

	m_iMaxLife = 		rand()%20 + 80;
	m_iLife = 			m_iMaxLife;

	m_fSpeed = 			float(rand()%5);
	m_fArmour = 		0;
	m_fWage =			10;
	
	m_fExperience = 	0;
	m_fNextLevel = 	1000;
	m_iLevel = 			1;
} 

void DMCharacterStats::Set(DMCharacterStats& kOther)
{
	m_strName = 		kOther.m_strName;
	m_strIcon = 		kOther.m_strIcon;
			
	m_iLife = 			kOther.m_iLife;
	m_iMaxLife = 		kOther.m_iMaxLife;
			
	m_fSpeed = 			kOther.m_fSpeed;
	m_fArmour = 		kOther.m_fArmour;
	m_fWage = 			kOther.m_fWage;
		
	m_fExperience = 	kOther.m_fExperience;
	m_fNextLevel = 	kOther.m_fNextLevel;
	m_iLevel = 			kOther.m_iLevel;
}

void DMCharacterStats::Print()
{
	cout<<"-= character stats =-"<<endl;
	cout<<"Name:"<<m_strName<<endl;
	cout<<"Icon:"<<m_strIcon<<endl;
	
	cout<<"life:"<<m_iLife<<" / "<<m_iMaxLife<<endl;
	
}

// ----------

P_DMCharacter::P_DMCharacter()
{
	strcpy(m_acName,"P_DMCharacter");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;


	bNetwork = true;
	
	
	m_pkBackPack = NULL;
	m_pkBody =		NULL;	
	m_pkBelt =		NULL;
	m_pkHand = 		NULL;
	m_pkImplants = NULL;	

	m_bCharacterIdle = true;
	m_bPlayWalkSound = false;
	
	
	m_iTeam =		0;
	
	m_kStats.Randomize();

	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
}

P_DMCharacter::~P_DMCharacter()
{
	if(m_pkBackPack)
		delete m_pkBackPack;
	if(m_pkBody)
		delete m_pkBody;
	if(m_pkBelt)
		delete m_pkBelt;
	if(m_pkHand)
		delete m_pkHand;
	if(m_pkImplants)
		delete m_pkImplants;


}

void P_DMCharacter::Init()
{

	m_pkBackPack = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),6,5);
	m_pkBody = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,4);
	m_pkBody->AddItemType (DMITEM_ARMOUR);
	m_pkBody->SetMaxItems(1);
	m_pkBelt = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),4,1);
	m_pkBelt->AddItemType(DMITEM_GRENADE);
	m_pkBelt->AddItemType(DMITEM_CLIP);		
	m_pkBelt->AddItemType(DMITEM_MEDKIT);
	m_pkHand = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),2,3,false);
	m_pkHand = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,2,false);
	m_pkHand->AddItemType(DMITEM_GRENADE);	
	m_pkHand->AddItemType(DMITEM_WEAPON);	
	m_pkHand->SetMaxItems(1);	
	m_pkImplants = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,3);
	m_pkImplants->AddItemType(DMITEM_IMPLANT);
	
	//cout<< "New character created"<<endl;

}


void P_DMCharacter::Damage(int iType,int iDmg)
{
	// one death should be enough for anyone ;)
	if ( m_kStats.m_iLife <= 0 )
		return;

	m_kStats.m_iLife -= iDmg;
	
	cout<<"damaged:"<<m_kStats.m_iLife<<endl;

	if(m_kStats.m_iLife <= 0)
	{
		cout<<"ARRRGGg *dead*"<<endl;		

		int iNumSounds = m_vkDeathSounds.size();
		if(iNumSounds > 0)
		{
			m_pkAudioSys->StartSound(m_vkDeathSounds[rand() % iNumSounds], 
				m_pkObject->GetWorldPosV());
		}
		
		if(P_ScriptInterface* pkSi = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface"))
		{
			pkSi->CallFunction("Dead");
		}
	}
	else
	{
		float t = m_pkObjMan->GetSimTime();
		static float prevSoundPlayTime = 0;

		if(t - prevSoundPlayTime > 2.0f) // spela max 1 ljud varannan sek
		{
			int iNumSounds = m_vkDefenciveActionQuots.size();
			if(iNumSounds > 0)
			{
				m_pkAudioSys->StartSound(m_vkDefenciveActionQuots[rand() % iNumSounds], 
					m_pkObject->GetWorldPosV());
				prevSoundPlayTime = t;
			}
		}

	}
}

void P_DMCharacter::Shoot (Vector3 kLocation)
{
	float t = m_pkObjMan->GetSimTime();

	static float prevSoundPlayTime = 0;

	// check if character is equipped with at weapon

	int iWeapID = -1;

	Entity* pkWeapon;
	P_DMGun* pkP_Gun;

	for ( int y = 0; y < 2; y++ )
		for ( int x = 0; x < 3; x++ )
			if ( *m_pkHand->GetItem(x,y) != -1 )
				iWeapID = *m_pkHand->GetItem(x,y);

	if(iWeapID == -1)
		return;

	pkWeapon = m_pkObjMan->GetObjectByNetWorkID ( iWeapID );

	if ( pkWeapon == 0 )
	{
		cout << "Warning! P_DMCharacter::Shoot: Tried to shoot without a weapon!" << endl;
		return;
	}

	pkP_Gun = (P_DMGun*)pkWeapon->GetProperty ("P_DMGun");

	if ( pkP_Gun == 0)
	{
		cout << "Error! P_DMCharacter::Shoot: Tried to shoot with a non-weapon!" << endl;
		return;
	}

	if(t - prevSoundPlayTime > 2.0f) // spela max 1 ljud varannan sek
	{
		int iNumSounds = m_vkOffenciveActionQuots.size();
		if(iNumSounds > 0)
		{
			m_pkAudioSys->StartSound(m_vkOffenciveActionQuots[rand() % iNumSounds], 
				m_pkObject->GetWorldPosV());
			prevSoundPlayTime = t;
		}
	}

	P_Mad* pkMad = (P_Mad*)m_pkObject->GetProperty ("P_Mad");

	pkP_Gun->m_iTeam = m_iTeam;

	pkP_Gun->Fire (kLocation);

	// rotate character towards target
	Vector3 kdiff = kLocation - m_pkObject->GetWorldPosV();
	kdiff.y = 0;
	Matrix4 kRotM;

	kRotM.LookDir( kdiff.Unit(), Vector3(0, 1, 0) );
	kRotM.Transponse();
	m_pkObject->SetLocalRotM(kRotM);

	// Start shoot animation, if gun isn't empty
	if(P_Mad* pkMad = (P_Mad*)m_pkObject->GetProperty("P_Mad"))
	{
		if ( pkP_Gun->HasAmmo() && pkP_Gun->ReadyToFire() )
		{
			pkMad->SetAnimation ("shoot", 0);
			pkMad->SetNextAnimation ("idle");

			// clear p_path, can't move and shoot at the same time
			P_PfPath* pkPath = (P_PfPath*)m_pkObject->GetProperty("P_PfPath");
			if ( pkPath )
				pkPath->ClearPath();
		}
	}

}


void P_DMCharacter::Save(ZFIoInterface* pkPackage)
{		
	char temp[128];

	//saving inventory
	m_pkBackPack->Save(pkPackage);		
	m_pkBody->Save(pkPackage);
	m_pkBelt->Save(pkPackage);
	m_pkHand->Save(pkPackage);
	m_pkImplants->Save(pkPackage);

	// Saving data stats
	pkPackage->Write ( &m_kStats.m_iLife, sizeof (m_kStats.m_iLife), 1 );
	pkPackage->Write ( &m_kStats.m_iMaxLife, sizeof (m_kStats.m_iMaxLife), 1 );
	pkPackage->Write ( &m_kStats.m_fSpeed, sizeof (m_kStats.m_fSpeed), 1 );
	pkPackage->Write ( &m_kStats.m_fArmour, sizeof (m_kStats.m_fArmour), 1 );
	pkPackage->Write ( &m_kStats.m_fWage, sizeof (m_kStats.m_fWage), 1 );
	pkPackage->Write ( &m_kStats.m_fExperience, sizeof (m_kStats.m_fExperience), 1 );
	pkPackage->Write ( &m_kStats.m_fNextLevel, sizeof (m_kStats.m_fNextLevel), 1 );
	pkPackage->Write ( &m_kStats.m_iLevel, sizeof (m_kStats.m_iLevel), 1 );
	pkPackage->Write ( &m_iTeam, sizeof (m_iTeam), 1 );
	pkPackage->Write ( &m_iState, sizeof (m_iState), 1 );

	strcpy( temp, m_kStats.m_strName.c_str() );
	pkPackage->Write(temp,128,1);
	
	strcpy( temp, m_kStats.m_strIcon.c_str() );
	pkPackage->Write(temp,128,1);
}

void P_DMCharacter::Load(ZFIoInterface* pkPackage)
{
	char temp[128];

	//loading inventory
	m_pkBackPack->Load(pkPackage);		
	m_pkBody->Load(pkPackage);
	m_pkBelt->Load(pkPackage);
	m_pkHand->Load(pkPackage);
	m_pkImplants->Load(pkPackage);

	// Saving data stats
	pkPackage->Read ( &m_kStats.m_iLife, sizeof (m_kStats.m_iLife), 1 );
	pkPackage->Read ( &m_kStats.m_iMaxLife, sizeof (m_kStats.m_iMaxLife), 1 );
	pkPackage->Read ( &m_kStats.m_fSpeed, sizeof (m_kStats.m_fSpeed), 1 );
	pkPackage->Read ( &m_kStats.m_fArmour, sizeof (m_kStats.m_fArmour), 1 );
	pkPackage->Read ( &m_kStats.m_fWage, sizeof (m_kStats.m_fWage), 1 );
	pkPackage->Read ( &m_kStats.m_fExperience, sizeof (m_kStats.m_fExperience), 1 );
	pkPackage->Read ( &m_kStats.m_fNextLevel, sizeof (m_kStats.m_fNextLevel), 1 );
	pkPackage->Read ( &m_kStats.m_iLevel, sizeof (m_kStats.m_iLevel), 1 );
	pkPackage->Read ( &m_iTeam, sizeof (m_iTeam), 1 );
	pkPackage->Read ( &m_iState, sizeof (m_iState), 1 );

	pkPackage->Read(temp,128,1);
	m_kStats.m_strName = temp;
	
	pkPackage->Read(temp,128,1);
	m_kStats.m_strIcon = temp;
}


void P_DMCharacter::Update()
{
	if(P_PfPath* pkPF = (P_PfPath*)m_pkObject->GetProperty("P_PfPath"))
	{
		if(pkPF->HavePath() == false)
		{
			if(m_bCharacterIdle == false)
			{
				m_bCharacterIdle = true;
			}
			else
			{
				if(m_bPlayWalkSound == true)
				{
					m_pkAudioSys->StopSound("/data/sound/walk_zombie.wav", 
						m_pkObject->GetIWorldPosV()); 	
					m_bPlayWalkSound = false;
					//printf("stoppa ljud!\n");
				}
			}
		}
		else
		{
			if(m_bCharacterIdle)
			{
				m_bCharacterIdle = false;

				if(m_bPlayWalkSound == false)
				{
					m_pkAudioSys->StartSound("/data/sound/walk_zombie.wav", 
						m_pkObject->GetIWorldPosV(), 
						Vector3(0,0,1), true );
					m_bPlayWalkSound = true;
					//printf("starta ljud!\n");
				}
			}
			else
			{
				if(m_bPlayWalkSound)
				{
					// WARNING! Detta kan bli slött som ASET, kommentera bort i värsta fall :)
					m_pkAudioSys->MoveSound("/data/sound/walk_zombie.wav", 
						m_pkObject->GetIWorldPosV(),
						m_pkObject->GetIWorldPosV());
				}
			}
		}
	}
}

// -----------------------------------------------------------------------------------------------
 
void P_DMCharacter::Equip (P_DMItem* pkDMItem)
{
	if ( pkDMItem == 0 )
		return;
	
	m_kStats.m_fArmour += pkDMItem->m_kItemStats.m_fArmourVal;
	m_kStats.m_iMaxLife += pkDMItem->m_kItemStats.m_iMaxLifeVal;
	AddMoveSpeed ( pkDMItem->m_kItemStats.m_fSpeedVal );

}

// -----------------------------------------------------------------------------------------------

void P_DMCharacter::UnEquip (P_DMItem* pkDMItem)
{
	if ( pkDMItem == 0 )
		return;
	
	m_kStats.m_fArmour -= pkDMItem->m_kItemStats.m_fArmourVal;
	m_kStats.m_iMaxLife -= pkDMItem->m_kItemStats.m_iMaxLifeVal;
	AddMoveSpeed ( -pkDMItem->m_kItemStats.m_fSpeedVal );
}

// -----------------------------------------------------------------------------------------------


void P_DMCharacter::UseQuickItem(int iItemIndex, bool bIndexIsItemType)
{
	int iBeltIndex = -1;
	vector<ContainerInfo> kItemList;
	m_pkBelt->GetItemList(&kItemList);

	if(bIndexIsItemType == false)
	{
		for(unsigned int i=0; i<kItemList.size(); i++)
		{
			if(kItemList[i].m_iItemX == iItemIndex)
			{
				iBeltIndex = i;
				break;
			}
		}
	}
	else
	{
		for(unsigned int i=0; i<kItemList.size(); i++)
		{
			if(kItemList[i].m_iType == iItemIndex)
			{
				iBeltIndex = i;
				break;
			}
		}
	}

	if(iBeltIndex == -1)
		return;

	bool bSuccess = false;
	int iObjectID = -1;
	string strClipName;

	printf("Character using quickitem %i %i\n", 
		kItemList[iBeltIndex].m_iItemX, kItemList[iBeltIndex].m_iItemY);

	switch(kItemList[iBeltIndex].m_iType)
	{
	case DMITEM_CLIP:
		
		strClipName = kItemList[iBeltIndex].m_strName;

		// format string of clipname to lowercase
		MakeStringLowerCase(strClipName);
		
		// check if character is equipped with at weapon
		Entity* pkWeapon;
		P_DMGun* pkP_Gun; pkP_Gun = NULL;

		for ( int y = 0; y < 2; y++ )
			for ( int x = 0; x < 3; x++ )
				if ( *m_pkHand->GetItem(x,y) != -1 )
				{
					iObjectID = *m_pkHand->GetItem(x,y);

					if( iObjectID != -1)
					{
						pkWeapon = m_pkObjMan->GetObjectByNetWorkID ( iObjectID );

						string strGunItemName = 
							((P_DMItem*)pkWeapon->GetProperty("P_DMItem"))->GetName();

						MakeStringLowerCase(strGunItemName);
						if(strClipName.find(strGunItemName) != string::npos)
						{
							pkP_Gun = (P_DMGun*)pkWeapon->GetProperty ("P_DMGun");
							break;
						}
					}
				}

		if( pkP_Gun != NULL )
		{
			pkP_Gun->Reload();
			bSuccess = true;
		}
		else
		{
			printf("Failed to reload weapon, not correct weapon in hand\n");
			return; // return
		}
		break;

	case DMITEM_MEDKIT:
		printf("using medkit\n");
		m_kStats.m_iLife = m_kStats.m_iMaxLife;
		bSuccess = true;
		break;
	}

	if(m_pkBelt->RemoveItem(kItemList[iBeltIndex].m_iItemX,
		kItemList[iBeltIndex].m_iItemY))
	{
		printf("Removing quickitem\n");
		return;
	}
	else
	{
		printf("Failed to remove quickitem\n");
		return;
	}
}

/*
void P_DMCharacter::UseQuickItem(int iItemIndex, bool bIndex) // <- iItemIndex = 0,1,2,3 dvs slot (0,0), (0,1) osv
{
	ContainerInfo kFind;
	vector<ContainerInfo> kItemList;
	m_pkBelt->GetItemList(&kItemList);
	
	for(unsigned int i=0; i<kItemList.size(); i++)
	{
		if(kItemList[i].m_iItemX == iItemIndex)
		{
			printf("Character using quickitem %i %i\n", 
				kItemList[i].m_iItemX, kItemList[i].m_iItemY);

			switch(kItemList[i].m_iType)
			{
			case DMITEM_CLIP:
				{
					string strClipName = kItemList[i].m_strName;

					// format string of clipname to lowercase
					MakeStringLowerCase(strClipName);
					
					// check if character is equipped with at weapon
					int iWeapID = -1;
					Entity* pkWeapon;
					P_DMGun* pkP_Gun = NULL;

					for ( int y = 0; y < 2; y++ )
						for ( int x = 0; x < 3; x++ )
							if ( *m_pkHand->GetItem(x,y) != -1 )
							{
								iWeapID = *m_pkHand->GetItem(x,y);

								if( iWeapID != -1)
								{
									pkWeapon = m_pkObjMan->GetObjectByNetWorkID ( iWeapID );

									string strGunItemName = 
										((P_DMItem*)pkWeapon->GetProperty("P_DMItem"))->GetName();

									MakeStringLowerCase(strGunItemName);
									if(strClipName.find(strGunItemName) != string::npos)
									{
										pkP_Gun = (P_DMGun*)pkWeapon->GetProperty ("P_DMGun");
										break;
									}
								}
							}

					if( pkP_Gun != NULL )
					{
						pkP_Gun->Reload();

						if(m_pkBelt->RemoveItem(kItemList[i].m_iItemX,kItemList[i].m_iItemY))
						{
							printf("Removing clip\n");
							return;
						}
						else
						{
							printf("Failed to remove clip\n");
							return;
						}
					}
					else
					{
						printf("Failed to reload weapon, not correct weapon in hand\n");
						return; // return
					}

					break;
				} // END case DMITEM_CLIP:

			case DMITEM_MEDKIT:
				int iMedPack;
				iMedPack = *m_pkBelt->GetItem(x,0);

				if(iMedPack != -1)
				{
					m_kStats.m_iLife = m_kStats.m_iMaxLife;
				}

				break;
			}

			break;
		}
	}
}
*/

void P_DMCharacter::MakeStringLowerCase(string& s)
{
	for(int i=0; i<s.size(); i++)
	{
		char test = s[i];

		if(test > 'A' && test < 'Z')
		{
			test += 32;
			s[i] = test;
		}
	}
}


void P_DMCharacter::SetMoveSpeed (float fSpeed)
{
	// get pf_path property
	P_PfPath* pkPath = (P_PfPath*)m_pkObject->GetProperty("P_PfPath");

	m_kStats.m_fSpeed = fSpeed;

	if ( m_kStats.m_fSpeed < 0 )
		m_kStats.m_fSpeed = 0;

	if ( pkPath )
		pkPath->SetSpeed (m_kStats.m_fSpeed);
}

// -----------------------------------------------------------------------------------------------

void P_DMCharacter::AddMoveSpeed (float fSpeed)
{
	// get pf_path property
	P_PfPath* pkPath = (P_PfPath*)m_pkObject->GetProperty("P_PfPath");

	m_kStats.m_fSpeed += fSpeed;

	if ( m_kStats.m_fSpeed < 0 )
		m_kStats.m_fSpeed = 0;

	if ( pkPath )
		pkPath->SetSpeed (m_kStats.m_fSpeed);
}

// -----------------------------------------------------------------------------------------------

void P_DMCharacter::AddXP(int iXP)
{
	m_kStats.m_fExperience += iXP;

	if(m_kStats.m_fExperience >= m_kStats.m_fNextLevel)
	{
		m_kStats.m_iLevel++;
		m_kStats.m_fNextLevel = m_kStats.m_fNextLevel + 1000; // Låt nästa grad vara 1000 XP svårare.
	}
}

vector<PropertyValues> P_DMCharacter::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
	return kReturn;
}


Property* Create_P_DMCharacter()
{
	return new P_DMCharacter;
}


