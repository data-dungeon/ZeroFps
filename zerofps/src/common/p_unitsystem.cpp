#include "p_unitsystem.h"

P_UnitSystem::P_UnitSystem()
{
	strcpy(m_acName,"P_UnitSystem");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	

	cout<<"unit system manager created"<<endl;

	UnitSystems base;

	for(int i =0;i<4;i++)
		m_kPlayerSystems.push_back(base);
}

COMMON_API Property* Create_P_UnitSystem()
{
	return new P_UnitSystem();

}







