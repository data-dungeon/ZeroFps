#include "property.h"

Property::Property()
{
	bNetwork = false;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_pkObject=NULL;
}

void Property::PackTo(NetPacket* pkNetPacket) 
{

}

void Property::PackFrom(NetPacket* pkNetPacket) 
{ 

}








