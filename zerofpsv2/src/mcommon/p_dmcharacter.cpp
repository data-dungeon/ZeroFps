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

	m_fSpeed = 			rand()%5;
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

	m_pkBackPack = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),7,8);
	m_pkBody = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,4);
		m_pkBody->AddItemType(DMItemAmor);				
	m_pkBelt = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),4,1);
		m_pkBelt->AddItemType(DMItemGrenade);				
		m_pkBelt->AddItemType(DMItemClip);						
	m_pkHand = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),2,3,false);
		m_pkHand->AddItemType(DMItemGrenade);	
		m_pkHand->AddItemType(DMItemWeapon);	
	m_pkImplants = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,3);
		m_pkImplants->AddItemType(DMItemImplant);		
	
	
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
		m_pkAudioSys->StartSound("data/sound/death/death9.wav", 
			m_pkObject->GetWorldPosV());
		
		if(P_ScriptInterface* pkSi = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface"))
		{
			pkSi->CallFunction("Dead");
		}
	}
}

void P_DMCharacter::Shoot (Vector3 kLocation)
{
	// check if character is equipped with at weapon
	int iWeapID;
	Entity* pkWeapon;
	P_DMGun* pkP_Gun;

	for ( int y = 0; y < 2; y++ )
		for ( int x = 0; x < 3; x++ )
			if ( m_pkHand->GetItem(0,0) )
				iWeapID = *m_pkHand->GetItem(0,0);

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

	P_Mad* pkMad = (P_Mad*)m_pkObject->GetProperty ("P_Mad");

	pkP_Gun->Fire (kLocation);

	// rotate character towards target
	Vector3 kdiff = kLocation - m_pkObject->GetWorldPosV();
	kdiff.y = 0;
	Matrix4 kRotM;

	kRotM.LookDir( kdiff.Unit(), Vector3(0, 1, 0) );
	kRotM.Transponse();
	m_pkObject->SetLocalRotM(kRotM);


	// Start shoot animation
	if(P_Mad* pkMad = (P_Mad*)m_pkObject->GetProperty("P_Mad"))
	{
		pkMad->SetAnimation ("shoot", 0);
		pkMad->SetNextAnimation ("idle");
	}

}


void P_DMCharacter::Save(ZFIoInterface* pkPackage)
{		

	//saving inventory
	m_pkBackPack->Save(pkPackage);		
	m_pkBody->Save(pkPackage);
	m_pkBelt->Save(pkPackage);
	m_pkHand->Save(pkPackage);
	m_pkImplants->Save(pkPackage);


}

void P_DMCharacter::Load(ZFIoInterface* pkPackage)
{
	
	//loading inventory
	m_pkBackPack->Load(pkPackage);		
	m_pkBody->Load(pkPackage);
	m_pkBelt->Load(pkPackage);
	m_pkHand->Load(pkPackage);
	m_pkImplants->Load(pkPackage);

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
 

vector<PropertyValues> P_DMCharacter::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_DMCharacter()
{
	return new P_DMCharacter;
}










