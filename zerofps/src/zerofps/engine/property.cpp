#include "property.h"

Property::Property()
{
	bNetwork = false;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_pkObject=NULL;
	strcpy(m_acName,"NoName");		
	
}

void Property::PackTo(NetPacket* pkNetPacket) 
{

}

void Property::PackFrom(NetPacket* pkNetPacket) 
{ 

}








