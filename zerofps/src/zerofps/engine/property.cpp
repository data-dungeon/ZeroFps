#include "property.h"

#include "zerofps.h"

Property::Property()
{
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkPropertyFactory = static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));			
	
	bNetwork = false;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_pkObject=NULL;
	strcpy(m_acName,"NoName");		
	
	m_iSortPlace=0;
	m_bSortDistance=false;
}

/*
void Property::PackTo(NetPacket* pkNetPacket) 
{

}

void Property::PackFrom(NetPacket* pkNetPacket) 
{  

}
*/


void Property::Save(ZFMemPackage* pkPackage)
{
	int i=1234;
	pkPackage->Write((void*)&i,4);
}

void Property::Load(ZFMemPackage* pkPackage)
{  
	int i=0;
	pkPackage->Read((void*)&i,4);
	
	if(i != 1234)
		cout<<"ERROR Loading property"<<endl;
}

bool Property::operator<(Property& kOther){
	float d1,d2;	
	
	if(!m_bSortDistance){
		return m_iSortPlace < kOther.m_iSortPlace;
	}else{	
		if(m_iSortPlace < kOther.m_iSortPlace){
			return true;		
		}else {
			if(m_iSortPlace == kOther.m_iSortPlace){			
				d1 = abs( (m_pkZeroFps->GetCam()->GetPos() - m_pkObject->GetPos()).Length() );
				d2 = abs( (m_pkZeroFps->GetCam()->GetPos() - kOther.m_pkObject->GetPos()).Length() );
				return d1<d2;		
			} else {
				return false;
			}
		}
	}
}






