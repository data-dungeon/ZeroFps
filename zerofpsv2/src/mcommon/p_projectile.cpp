#include "p_projectile.h"

#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 

P_Projectile::P_Projectile() : Property("P_Projectile")
{	
// 	m_iSide		= PROPERTY_SIDE_SERVER;
// 	m_iType		= PROPERTY_TYPE_NORMAL;
 	m_iSide		= 0;
 	m_iType		= 0;	
	m_bNetwork 	= false;
	m_iVersion	= 1;

	
	m_iOwnerID	=	-1;
	m_bDidHit	=	false;

	
	m_kPropertyValues.push_back(PropertyValues("ownerid",VALUETYPE_INT,(void*)&m_iOwnerID));

}

void P_Projectile::Init()
{
	//never save a projectile
	m_pkEntity->SetSave(false);

}

void P_Projectile::Touch(int iId)
{
	//dont want to hit our owner
	if(iId == m_iOwnerID)
		return;
		
	//already hit something
	if(m_bDidHit)
		return;
		
	m_bDidHit = true;
		
	
 	vector<ScriptFuncArg> kArgs(2);
 	kArgs[0].m_kType.m_eType	=	tINT;
 	kArgs[0].m_pData 				=	&m_iOwnerID;
 	kArgs[1].m_kType.m_eType	=	tINT;
 	kArgs[1].m_pData 				=	&iId;
 

 	m_pkEntityManager->CallFunction(m_pkEntity, "Hit",&kArgs);	

}



// SCRIPT INTERFACE FOR P_Projectile
using namespace ObjectManagerLua;

namespace SI_P_Projectile
{
	
}


Property* Create_P_Projectile()
{
	return new P_Projectile;
}


void Register_P_Projectile(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Projectile", Create_P_Projectile);					

	// Register Property Script Interface
	
	//item handling
// 	g_pkScript->ExposeFunction("SetProjectileOwner",		SI_P_CharacterProperty::SetProjectileOwnerLua);
	

};




// vector<PropertyValues> P_Projectile::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(1);
// 
// 	kReturn[0].kValueName = "ownerid";
// 	kReturn[0].iValueType = VALUETYPE_INT; 
// 	kReturn[0].pkValue    = (void*)&m_iOwnerID;	
// 
// 	return kReturn;	
// }