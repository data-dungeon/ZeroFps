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

void P_Ml::AddAction(const char* csAction)
{
	//cout<<"registering action:"<<csAction<<endl;
	m_kActions.push_back(string(csAction));
}


void P_Ml::PackTo( NetPacket* pkNetPacket ) 
{
	int nr = m_kActions.size();
	
	pkNetPacket->Write(&nr,sizeof(nr));
	
	for(int i=0;i<m_kActions.size();i++)
	{
		pkNetPacket->Write_NetStr(m_kActions[i].c_str());
	}

} 

void P_Ml::PackFrom( NetPacket* pkNetPacket ) 
{
	int nr;	
	
	pkNetPacket->Read(&nr,sizeof(nr));
	
	m_kActions.clear();
	for(int i=0;i<m_kActions.size();i++)
	{
		char temp[128];
		pkNetPacket->Read_NetStr(temp);
		m_kActions.push_back(string(temp));
	}
}

Property* Create_P_Ml()
{
	return new P_Ml;
}
