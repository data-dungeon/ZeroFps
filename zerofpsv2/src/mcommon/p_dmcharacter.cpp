#include "p_dmcharacter.h" 
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

	m_pkBackPack = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),8,7);
	m_pkBody = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,4);
	m_pkBelt = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),4,1);
	m_pkHand = 		new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),2,3);
	m_pkImplants = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),3,3);
	
	//cout<< "New character created"<<endl;

}


void P_DMCharacter::Damage(int iType,int iDmg)
{
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










