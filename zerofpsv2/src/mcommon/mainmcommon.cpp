#include "mainmcommon.h"

#include "p_ai.h"
//#include "p_car.h"
#include "p_clientcontrol.h"
//#include "p_dmcharacter.h"
#include "p_characterproperty.h"
#include "p_charactercontrol.h"
#include "p_shadowblob.h"
//#include "p_arcadecharacter.h"
#include "p_ml.h"
#include "p_item.h"
#include "p_container.h"
#include "p_enviroment.h"
#include "p_charactercontrol.h"
#include "p_characterproperty.h"
#include "p_fogplane.h"
#include "p_container.h"
/*
#include "p_dmclickme.h"
#include "p_dmgun.h"
#include "p_dmhq.h"
#include "p_dmmission.h"
#include "p_dmshop.h"
*/
#include "p_spell.h"


void MCommon_RegisterResources() { }

void MCommon_RegisterPropertys(ZeroFps* pkZeroFps, PropertyFactory*	pkPropertyFactory)
{
	pkPropertyFactory->Register("P_AI",						Create_P_AI);
	//pkPropertyFactory->Register("P_ArcadeCharacter",	Create_P_ArcadeCharacter);
	//pkPropertyFactory->Register("P_Car",					Create_P_Car);
	Register_P_CharacterControl(pkZeroFps);	
	Register_P_CharacterProperty(pkZeroFps);
	pkPropertyFactory->Register("P_CharStats",			Create_P_CharStats);
	pkPropertyFactory->Register("P_ClientControl",		Create_P_ClientControl);
	pkPropertyFactory->Register("P_Container",			Create_P_Container);
	Register_P_Container(pkZeroFps);
	//pkPropertyFactory->Register("P_DMCharacter",			Create_P_DMCharacter);
	//pkPropertyFactory->Register("P_DMClickMe",			Create_P_DMClickMe);
	//pkPropertyFactory->Register("P_DMGun",					Create_P_DMGun);
	//pkPropertyFactory->Register("P_DMHQ",					Create_P_DMHQ);
	//pkPropertyFactory->Register("P_DMItem",				Create_P_DMItem);
	//pkPropertyFactory->Register("P_DMMission",			Create_P_DMMission);
	//pkPropertyFactory->Register("P_DMShop",				Create_P_DMShop);
	pkPropertyFactory->Register("P_Enviroment",			Create_P_Enviroment);
	Register_P_FogPlane(pkZeroFps);
	pkPropertyFactory->Register("P_Item",					Create_P_Item);
	Register_P_Ml(pkZeroFps);
	pkPropertyFactory->Register("P_ServerInfo",			Create_P_ServerInfo);
	pkPropertyFactory->Register("P_ShadowBlob",			Create_P_ShadowBlob);
	pkPropertyFactory->Register("P_Spawn",					Create_P_Spawn);
	pkPropertyFactory->Register("P_Spell",					Create_P_Spell);

	cout << "Registred MCommon property" << endl;
}
