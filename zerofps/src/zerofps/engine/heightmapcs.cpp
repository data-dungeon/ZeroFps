#include "heightmapcs.h"
#include "cssphere.h"

HeightMapCS::HeightMapCS(HeightMap* pkMap)
{
	m_pkMap=pkMap;

}

CollisionData* HeightMapCS::Test(CollisionShape* kOther,float fTime,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *pkCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(pkCs,fTime);		
	} else if(bContinue)
	{
		return kOther->Test(this,fTime,false);
	};
	
	cout<<"Unhandled collision"<<endl;
	return NULL;
}


CollisionData* HeightMapCS::Collide_CSSphere(CSSphere* kOther,float fTime)
{


	return NULL;
}









