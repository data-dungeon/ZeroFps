#include "p_ml.h" 
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"

P_Ml::P_Ml()
{
	strcpy(m_acName,"P_Ml");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));


	bNetwork = true;
}

void P_Ml::Update()
{
/*	//random jump
	Vector3 pos = m_pkObject->GetLocalPosV();
	pos += Vector3((rand() % 1000)/1000.0 - 0.5,(rand() % 1000)/1000.0-0.5,(rand() % 1000)/1000.0-0.5)*4;
	m_pkObject->SetLocalPosV(pos);
*/

}

void P_Ml::PackTo( NetPacket* pkNetPacket ) 
{

} 

void P_Ml::PackFrom( NetPacket* pkNetPacket ) 
{

}

Property* Create_P_Ml()
{
	return new P_Ml;
}
