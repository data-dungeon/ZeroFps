#include "cssphere.h"

CSSphere::CSSphere(float fRadius)
{
	m_fRadius=fRadius;
}

CollisionData* CSSphere::Test(CollisionShape* kOther,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *kCs=dynamic_cast<CSSphere*>(kOther);
		
		cout<<"BAJ"<<endl;
		return NULL;
		
	} else if(bContinue)
	{
		return kOther->Test(this,false);
	};
	
	cout<<"Unhandled collision"<<endl;
	return NULL;
}




