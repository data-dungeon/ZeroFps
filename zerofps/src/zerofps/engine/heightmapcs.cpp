#include "heightmapcs.h"
#include "cssphere.h"

HeightMapCS::HeightMapCS(HeightMap* pkMap)
{
	m_pkMap=pkMap;

}

Collision* HeightMapCS::Test(CollisionShape* kOther,float fTime,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *pkCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(pkCs,fTime);		
	} else if(bContinue)
	{
		return kOther->Test(this,fTime,false);
	};
	
	//cout<<"Unhandled collision"<<endl;
	return NULL;
}


Collision* HeightMapCS::Collide_CSSphere(CSSphere* kOther,float fTime)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();
	
	
	if(O2->GetPos().x<m_pkMap->GetPos().x-m_pkMap->GetSize()/2 ||
		O2->GetPos().x>m_pkMap->GetPos().x+m_pkMap->GetSize()/2 ||
		O2->GetPos().z<m_pkMap->GetPos().z-m_pkMap->GetSize()/2 ||
		O2->GetPos().z>m_pkMap->GetPos().z+m_pkMap->GetSize()/2)
		return NULL;
	
	
	Vector3 kNewPos;
	if(kOther->m_pkPP->m_bGlide)
	{
		kNewPos=kOther->m_pkPP->m_kNewPos;
	
		float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
		if( h < (kNewPos.y-kOther->m_fRadius) )
			return NULL;
	
		kNewPos.y=h+kOther->m_fRadius;
	}
	else
	{
		kNewPos=kOther->m_pkPP->GetObject()->GetPos();
	
		float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
		if( h < (kNewPos.y-kOther->m_fRadius) )
			return NULL;
	
		kNewPos.y=h+kOther->m_fRadius;
	
	}
	
	
	//assemble collision data
	Collision* tempdata = new Collision;
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = kNewPos;
	tempdata->m_fDistance2 = (kNewPos - O2->GetPos()).Length();	
	tempdata->m_kNormal2.Set(0,1,0);	
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = m_pkPP->m_kNewPos;
	tempdata->m_fDistance1 = 0;
	tempdata->m_kNormal1=m_pkMap->Tilt(kNewPos.x,kNewPos.z);
	
	return tempdata;
	
}









