#include "heightmapcs.h"
#include "cssphere.h"
#include "csbox.h"

HeightMapCS::HeightMapCS(HeightMap* pkMap)
{
	m_pkMap=pkMap;

}

Collision* HeightMapCS::Test(CollisionShape* kOther,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSBox)){
		CSBox *pkCs=dynamic_cast<CSBox*>(kOther);
		
		return Collide_CSBox(pkCs);		
	} else 
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *pkCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(pkCs);		
	} else 
	if(bContinue)
	{
		return kOther->Test(this,false);
	};
	
	//cout<<"Unhandled collision"<<endl;
	return NULL;
}


Collision* HeightMapCS::Collide_CSSphere(CSSphere* kOther)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();
	
	
	if(O2->GetWorldPosV().x<m_pkMap->GetPos().x-m_pkMap->GetSize()/2 ||
		O2->GetWorldPosV().x>m_pkMap->GetPos().x+m_pkMap->GetSize()/2 ||
		O2->GetWorldPosV().z<m_pkMap->GetPos().z-m_pkMap->GetSize()/2 ||
		O2->GetWorldPosV().z>m_pkMap->GetPos().z+m_pkMap->GetSize()/2)
		return NULL;
	
	
	Vector3 kNewPos;
//	if(kOther->m_pkPP->m_bGlide)
//	{
		kNewPos=kOther->m_pkPP->m_kNewPos;
	
		float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
		if( h < (kNewPos.y-kOther->m_fRadius) )
			return NULL;
	
		kNewPos.y=h+kOther->m_fRadius;
/*	}
	else
	{
		kNewPos=kOther->m_pkPP->GetObject()->GetPos();
	
		float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
		if( h < (kNewPos.y-kOther->m_fRadius) )
			return NULL;
	
		kNewPos.y=h+kOther->m_fRadius;
	
	}
*/	
	
	//assemble collision data
	Collision* tempdata = new Collision;
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = kNewPos;
	tempdata->m_fDistance2 = (kNewPos - O2->GetWorldPosV()).Length();	
	tempdata->m_kNormal2.Set(0,1,0);	
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = m_pkPP->m_kNewPos;
	tempdata->m_fDistance1 = 0;
	tempdata->m_kNormal1=m_pkMap->Tilt(kNewPos.x,kNewPos.z);
	
	return tempdata;
	
}


Collision* HeightMapCS::Collide_CSBox(CSBox* kOther)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();
	
	
	if(O2->GetWorldPosV().x<m_pkMap->GetPos().x-m_pkMap->GetSize()/2 ||
		O2->GetWorldPosV().x>m_pkMap->GetPos().x+m_pkMap->GetSize()/2 ||
		O2->GetWorldPosV().z<m_pkMap->GetPos().z-m_pkMap->GetSize()/2 ||
		O2->GetWorldPosV().z>m_pkMap->GetPos().z+m_pkMap->GetSize()/2)
		return NULL;
	
	
	Vector3 kNewPos;
	float fRadius = kOther->m_kScale.y/2;
	
	if(kOther->m_pkPP->m_bGlide)
	{
		kNewPos=kOther->m_pkPP->m_kNewPos;
	
		float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
		if( h < (kNewPos.y-fRadius) )
			return NULL;
	
		kNewPos.y=h + fRadius;
	}
	else
	{
		kNewPos=kOther->m_pkPP->GetObject()->GetWorldPosV();
	
		float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
		if( h < (kNewPos.y-fRadius) )
			return NULL;
	
		kNewPos.y=h+fRadius;
	
	}
	
	
	//assemble collision data
	Collision* tempdata = new Collision;
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = kNewPos;
	tempdata->m_fDistance2 = (kNewPos - O2->GetWorldPosV()).Length();	
	tempdata->m_kNormal2.Set(0,1,0);	
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = m_pkPP->m_kNewPos;
	tempdata->m_fDistance1 = 0;
	tempdata->m_kNormal1=m_pkMap->Tilt(kNewPos.x,kNewPos.z);
	
	return tempdata;
	
}







