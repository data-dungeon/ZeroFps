#include "p_dmcharacter.h" 
#include "p_dmgun.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h" 
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"

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
	
	
	m_iTeam =		0;
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
	m_pkBelt = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),4,1);
	m_pkHand = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),2,3);
	m_pkImplants = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,3);
	
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

	cout << "X:" << pkWeapon->GetWorldPosV().x << " y:" << pkWeapon->GetWorldPosV().y << " z:" << pkWeapon->GetWorldPosV().z << endl;

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
 

vector<PropertyValues> P_DMCharacter::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_DMCharacter()
{
	return new P_DMCharacter;
}










