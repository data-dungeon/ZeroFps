#include "p_dmcharacter.h"
#include "p_dmgun.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"

#include "../zerofpsv2/engine/p_pfpath.h"
#include "p_dmhq.h"
#include "p_dmclickme.h"

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
	m_fAim =		10;
				
	m_fExperience = 0;
	m_fNextLevel =  1000;
	m_iLevel =		1;
}

void DMCharacterStats::Randomize()
{
	FILE* pkFile;
	static vector<string> kNames;
	static vector<string> kSNames;

	const int MAX_LINE = 127;
	char strLine[MAX_LINE+1];

	if(kNames.empty())
	{
		if((pkFile = fopen("names1.txt", "r")) != NULL) {
			while (!feof(pkFile)) {
				if(fgets(strLine, MAX_LINE, pkFile))
				{
					kNames.push_back(strLine);
				}
			}
			fclose(pkFile);
		}
	}

	if(kSNames.empty())
	{
		if((pkFile = fopen("names2.txt", "r")) != NULL) {
			while (!feof(pkFile)) {
				if(fgets(strLine, MAX_LINE, pkFile))
				{
					kSNames.push_back(strLine);
				}
			}
			fclose(pkFile);
		}
	}

	if(!(kNames.empty() || kSNames.empty()))
	{
		int iFirstName = rand()%kNames.size();
		int iSureName = rand()%kSNames.size();
		kNames[iFirstName].erase(kNames[iFirstName].size()-1);
		kSNames[iSureName].erase(kSNames[iSureName].size()-1);
		m_strName = kNames[iFirstName] + string(" ") + kSNames[iSureName];
	}
	else
	{
		m_strName = "hoda";
	}

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
	m_bNewOrder = true;
	
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

	if ( m_kStats.m_iLife < 0 )
		m_kStats.m_iLife = 0;
	
	// oh madness, blood psystem in the nick of time :D
	if ( m_iTeam != 10 )
		m_pkObject->m_pkEntityMan->CreateObjectFromScriptInZone("data/script/objects/dm/t_blood.lua", m_pkObject->GetWorldPosV());


	//cout<<"LifeLeft:" << m_kStats.m_iLife << endl;

	if(m_kStats.m_iLife <= 0)
	{
		int iNumSounds = m_vkDeathSounds.size();
		if(iNumSounds > 0)
		{
			m_pkAudioSys->StartSound(m_vkDeathSounds[rand() % iNumSounds], 
				m_pkObject->GetWorldPosV());
		}
		
		ChangeState (DEAD);
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
		//cout << "Warning! P_DMCharacter::Shoot: Tried to shoot without a weapon!" << endl;
		return;
	}

	pkP_Gun = (P_DMGun*)pkWeapon->GetProperty ("P_DMGun");

	if ( pkP_Gun == 0)
	{
		//cout << "Error! P_DMCharacter::Shoot: Tried to shoot with a non-weapon!" << endl;
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

	// bad aim
	Vector3 kTemp = pkWeapon->GetWorldPosV();
	float fDist = kLocation.DistanceTo( kTemp );

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
			pkMad->SetAnimation ((char*)pkP_Gun->m_strShootAnim.c_str(), 0);
			pkMad->SetNextAnimation ("idle");

			// clear p_path, can't move and shoot at the same time
			P_PfPath* pkPath = (P_PfPath*)m_pkObject->GetProperty("P_PfPath");
			if ( pkPath )
				pkPath->ClearPath();
		}
	}

	if ( fDist < pkP_Gun->Range() )
	{
		// fAim = degrees
		float fRand = sin(m_kStats.m_fAim / (180.f / PI)) * fDist;

		kLocation.x += (rand()%int(fRand*1000)) / 1000.f - (fRand / 2.f);
		kLocation.y += (rand()%int(fRand*1000)) / 1000.f - (fRand / 2.f);
		kLocation.z += (rand()%int(fRand*1000)) / 1000.f - (fRand / 2.f);

		pkP_Gun->Fire (kLocation);

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
	UpdateOrders();

	
	
	
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

				if(m_bPlayWalkSound == false && m_iTeam != 1 && m_iTeam != 2 )
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

	// default, run scriptfunction, item itself decides what happens
	default:
		Entity* pkQItem = m_pkObject->m_pkEntityMan->GetObjectByNetWorkID(kItemList[iBeltIndex].m_iItemID);

		if ( pkQItem == 0 )
			break;
		// get SI
		P_ScriptInterface* pkSI = (P_ScriptInterface*)pkQItem->GetProperty("P_ScriptInterface");

		if (pkSI)
		{
			// send in characterID
			vector<ARG_DATA> kParams;

			int iValue = m_pkObject->GetEntityID();;
			
			ARG_DATA kData;
			kData.eType = tINT;
			kData.pData = &iValue;

			kParams.push_back (kData);

			pkSI->CallFunction ( "Use", &kParams );	
		}

		break;
	}

/*
	case DMITEM_MEDKIT:
		printf("using medkit\n");
		m_kStats.m_iLife = m_kStats.m_iMaxLife;
		bSuccess = true;
		break;
	}
**/
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
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "team";
	kReturn[0].iValueType = VALUETYPE_INT;
	kReturn[0].pkValue    = &m_iTeam;		

	return kReturn;
}


void P_DMCharacter::UpdateOrders()
{
	if(m_kOrderQueue.empty() || m_iState == DEAD)
		return;

	if(HandleOrder(&m_kOrderQueue.front(),m_bNewOrder))
	{
		if(!m_kOrderQueue.empty())
			m_kOrderQueue.pop();
			
		m_bNewOrder = true;
		
		//cout<<"order complete"<<endl;
	}
	else
		m_bNewOrder = false;
} 

bool P_DMCharacter::HandleOrder(DMOrder* pkOrder,bool bNew)
{
	//cout<<"handling order:"<<pkOrder->m_iOrderType<<" left in queue "<<m_kOrderQueue.size()<<endl;
	
	switch(pkOrder->m_iOrderType)
	{
		case eWalk:
			{
				if(P_PfPath* pkPF = (P_PfPath*)m_pkObject->GetProperty("P_PfPath"))
				{
					//new order do pathfind
					if(bNew)
					{
						pkPF->MakePathFind(pkOrder->m_kPosition);
						return false;					
					}
				
					//doing order
					if(pkPF->HavePath())
					{										
						return false;					
					}
					else
						return true; //order complete
				}			
				break;
			}
			
		case ePickup:
			{
				if(Entity* pkPickEnt = m_pkObjMan->GetObjectByNetWorkID(pkOrder->m_iEntityID))
				{
					if( pkPickEnt->GetWorldPosV().DistanceTo(m_pkObject->GetWorldPosV()) < 1) 
					{
						if(pkPickEnt->GetUseZones())
						{	
							if(m_pkBackPack->AddItem(pkPickEnt->GetEntityID()))
							{	
								m_pkAudioSys->StartSound("data/sound/pick_up.wav", 
									pkPickEnt->GetWorldPosV());
								return true;
							}
							else
								return true;

						}
					}
				}
			
				break;
			}
			
		case eEnterHQ:
			{
				if(Entity* pkPickEnt = m_pkObjMan->GetObjectByNetWorkID(pkOrder->m_iEntityID))
				{
					if(P_DMHQ* pkHQ = (P_DMHQ*)pkPickEnt->GetProperty("P_DMHQ"))
					{
						Vector3 kPos1 = pkPickEnt->GetWorldPosV();
						Vector3 kPos2 = m_pkObject->GetWorldPosV();
						kPos1.y = 0;
						kPos2.y = 0;


						if( kPos1.DistanceTo(kPos2) < 1)
						{
							ClearOrders();
							pkHQ->InsertCharacter(m_pkObject->GetEntityID());							
							return true;
						}
						else
						{
							//cout<<"Cant reach the door"<<endl;
							return true;
						}
					}
				}
				break;
			}

		case eClickMe:
			{
				if(Entity* pkPickEnt = m_pkObjMan->GetObjectByNetWorkID(pkOrder->m_iEntityID))
				{
					if(P_DMClickMe* pkClick = (P_DMClickMe*)pkPickEnt->GetProperty("P_DMClickMe"))
					{
						Vector3 kPos1 = pkPickEnt->GetWorldPosV();
						Vector3 kPos2 = m_pkObject->GetWorldPosV();
						kPos1.y = 0;
						kPos2.y = 0;

						if( kPos1.DistanceTo(kPos2) < 2)
						{
							pkClick->Click( m_pkObject->GetEntityID() );
							//pkHQ->InsertCharacter(m_pkObject->GetEntityID());
							return true;
						}
						else
						{
							//cout<<"Cant reach clickme entity"<<endl;
							return true;
						}
					}
				}
				break;
			}

		case eAttack:
			{
				Shoot(pkOrder->m_kPosition);
			
				return true;
			
				break;
			}
	}
	
	return true;
}

void P_DMCharacter::ChangeState (int iState)
{
	if ( m_iState == iState )
		return;

	m_iState = iState;

	// if went to panic state, run script function
	if ( m_iState == PANIC )
	{
		if(P_ScriptInterface* pkSi = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface"))
			pkSi->CallFunction("Panic");
	}
	// character died :_(
	else if ( m_iState == DEAD )
	{
		if(P_ScriptInterface* pkSi = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface"))
			pkSi->CallFunction("Dead");

		// clear orders
		while ( !m_kOrderQueue.empty() )
			m_kOrderQueue.pop();

		DropAllItems();
	}

}

void P_DMCharacter::DropAllItems()
{
	m_pkBackPack->DropAll();
	m_pkBody->DropAll();
	m_pkBelt->DropAll();
	m_pkHand->DropAll();
	m_pkImplants->DropAll();
}

int P_DMCharacter::GetGun()
{
	vector<ContainerInfo> kItems;

	m_pkHand->GetItemList ( &kItems );

	if ( kItems.size() )
		return kItems.at(0).m_iItemID;
	else
		return 0;
}

Property* Create_P_DMCharacter()
{
	return new P_DMCharacter;
}

