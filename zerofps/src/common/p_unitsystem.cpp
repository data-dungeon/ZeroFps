#include "p_unitsystem.h"

P_UnitSystem::P_UnitSystem()
{
	strcpy(m_acName,"P_UnitSystem");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	

	m_pkServerInfo = NULL;
	m_iPlayers = 4;

	SetupSystems();

	//add base systems to all players
	for(int i =0;i<m_iPlayers;i++)
		m_kPlayerSystems.push_back(m_kBaseSystems);

	cout<<"unit system manager created"<<endl;
}

void P_UnitSystem::Update()
{
	
	if(!m_pkServerInfo)
	{
		Object* sio = m_pkObject->m_pkObjectMan->GetObject("A ServerInfoObject");
		
		if(sio)
			m_pkServerInfo =  (P_ServerInfo*)sio->GetProperty("P_ServerInfo");		
	}		


}

void P_UnitSystem::SetupSystems()
{
	//setup weapons
	
	Weapon Gun80mm;
		Gun80mm.sName = 			"Gun80mm";
		Gun80mm.iType = 			WT_DIRECT;
		Gun80mm.iDamage = 		30;
		Gun80mm.afModifiers[0] =0.5;
		Gun80mm.afModifiers[1] =0.8;	
		Gun80mm.afModifiers[2] =1.0;	
		Gun80mm.afModifiers[3] =1.0;	
		Gun80mm.fRange = 		  6;
		Gun80mm.fFireRate =	  1;
		Gun80mm.iHit = 			FX_88MMHIT;
		Gun80mm.iFire = 			FX_88MMFIRE;	
	
	Weapon HeavyMG;
		HeavyMG.sName = 			"HeavyMG";
		HeavyMG.iType = 			WT_DIRECT;
		HeavyMG.iDamage = 		5;
		HeavyMG.afModifiers[0] =1.0;
		HeavyMG.afModifiers[1] =0.5;	
		HeavyMG.afModifiers[2] =0.3;	
		HeavyMG.afModifiers[3] =0.5;	
		HeavyMG.fRange = 		  3;
		HeavyMG.fFireRate =	  5;
		HeavyMG.iHit = 			FX_HEAVYMGHIT;
		HeavyMG.iFire = 			FX_88MMFIRE;	
											
	m_kBaseSystems.kWeapons.push_back(Gun80mm);
	m_kBaseSystems.kWeapons.push_back(HeavyMG);	

	//setup armors

	Armor BodyArmor;
		BodyArmor.sName =				"BodyArmor";
		BodyArmor.fModifier = 		1;

	Armor LightArmor;
		LightArmor.sName =			"LightArmor";
		LightArmor.fModifier = 		1;
	
	Armor HeavyArmor;
		HeavyArmor.sName =			"HeavyArmor";
		HeavyArmor.fModifier = 		1;
	
	Armor BuildingArmor;
		BuildingArmor.sName =		"BuildingArmor";
		BuildingArmor.fModifier = 	1;
	
	m_kBaseSystems.kArmors.push_back(BodyArmor);	
	m_kBaseSystems.kArmors.push_back(LightArmor);		
	m_kBaseSystems.kArmors.push_back(HeavyArmor);		
	m_kBaseSystems.kArmors.push_back(BuildingArmor);		
}

Weapon* P_UnitSystem::GetWeaponPointer(int iPlayer,int iWeapon)
{
	if(iPlayer == 255)
		iPlayer = 0;

	if(iPlayer >= m_iPlayers )
		return NULL;
		
	if(iWeapon >= m_kPlayerSystems[iPlayer].kWeapons.size())
		return NULL;

	return &(m_kPlayerSystems[iPlayer].kWeapons[iWeapon]);
}

Armor* P_UnitSystem::GetArmorPointer(int iPlayer,int iArmor)
{
	if(iPlayer == 255)
		iPlayer = 0;

	if(iPlayer >= m_iPlayers)
		return NULL;
		
	if(iArmor >= m_kPlayerSystems[iPlayer].kArmors.size())
		return NULL;

	return &(m_kPlayerSystems[iPlayer].kArmors[iArmor]);
}


bool P_UnitSystem::FireWeapon(P_ServerUnit* pkSu,Point kTarget,int iWeapon)
{

	Weapon* pkWeapon = GetWeaponPointer((int)pkSu->m_kInfo.m_Info2.m_cTeam,(int)pkSu->m_kInfo.m_Info2.m_cWeapon);
	if(!pkWeapon)
	{
		cout<<"FireWeapon: no weapon!" <<endl;
		return false;
	}
	
	cout<<"Fireing weapon "<<pkWeapon->sName<<endl;
	
	
	Tile* tg = TileEngine::m_pkInstance->GetTile(kTarget.x,kTarget.y);
	
	if(!tg)
	{
		cout<<"FireWeapon: no weapon!" <<endl;
		return false;
	}

	
	switch(pkWeapon->iType)
	{
		case WT_DIRECT:
		{
			if(tg->kUnits.empty())
			{		
				cout<<"boom..miss"<<endl;
				return true;
			}
			
			for(list<int>::iterator it = tg->kUnits.begin();it != tg->kUnits.end();it++)		
			{
				Object* ob = m_pkObject->m_pkObjectMan->GetObjectByNetWorkID(*it);
				if(ob)				
				{
					P_ServerUnit* pkTarget = (P_ServerUnit*)ob->GetProperty("P_ServerUnit");
			
					if(pkTarget)
					{
						Armor* pkArmor = GetArmorPointer((int)pkTarget->m_kInfo.m_Info2.m_cTeam,(int)pkTarget->m_kInfo.m_Info2.m_cArmor);
						if(!pkArmor)
						{
							cout<<"FireWeapon: no Armour!" <<endl;
							return false;
						}
					
					
						if(m_pkServerInfo)
						{
							Event temp;
							temp.m_iType = pkWeapon->iHit;
							temp.m_kPos = ob->GetPos();
							
							m_pkServerInfo->AddEvent(temp);
									
							temp.m_iType = pkWeapon->iFire;
							temp.m_kPos = pkSu->GetObject()->GetPos();							
							m_pkServerInfo->AddEvent(temp);							
						}
					
						int iDamage = int(pkWeapon->iDamage * pkWeapon->afModifiers[pkTarget->m_kInfo.m_Info2.m_cArmor] * pkArmor->fModifier);
						return pkTarget->Damage(iDamage);
					
					}
				}
			}	
		
	
			break;
		}
	
	
	}


	cout<<"FireWeapon: oh no! reached the end of this function" <<endl;
	return false;
}

COMMON_API Property* Create_P_UnitSystem()
{
	return new P_UnitSystem();

}







