#include "p_ml.h" 
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"

P_Ml::P_Ml()
{
	strcpy(m_acName,"P_Ml");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_bHaveSetRadius=false;
}

void P_Ml::Update()
{
	if(!m_bHaveSetRadius)
	{
		MadProperty* mp = (MadProperty*)m_pkObject->GetProperty("MadProperty");
	
		if(mp)
		{
			m_pkObject->GetRadius() = mp->GetRadius();
			m_bHaveSetRadius=true;
			//cout<<"setting radius "<<m_pkObject->GetRadius()<<endl;
		}
	}
}

Property* Create_P_Ml()
{
	return new P_Ml;
}


