#include "heightmapcs.h"
#include "cssphere.h"

HeightMapCS::HeightMapCS(HeightMap* pkMap)
{
	m_pkMap=pkMap;

}

CollisionData* HeightMapCS::Test(CollisionShape* kOther,Sphere* pkSThis,Sphere* pkSOther,float fTime,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *pkCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(pkCs,pkSThis,pkSOther,fTime);		
	} else if(bContinue)
	{
		return kOther->Test(this,pkSOther,pkSThis,fTime,false);
	};
	
	cout<<"Unhandled collision"<<endl;
	return NULL;
}


CollisionData* HeightMapCS::Collide_CSSphere(CSSphere* kOther,Sphere* pkSThis,Sphere* pkSOther,float fTime)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=pkSOther->m_pkPP->GetObject();
	
	
	if(pkSOther->m_kPos.x<m_pkMap->GetPos().x-m_pkMap->GetSize()/2 ||
		pkSOther->m_kPos.x>m_pkMap->GetPos().x+m_pkMap->GetSize()/2 ||
		pkSOther->m_kPos.z<m_pkMap->GetPos().z-m_pkMap->GetSize()/2 ||
		pkSOther->m_kPos.z>m_pkMap->GetPos().z+m_pkMap->GetSize()/2)
		return NULL;
	
	
	
	int i=0;
	float h=0;
	
	/*
	for(float x=pkSOther->m_kPos.x - pkSOther->m_fRadius/2; x<pkSOther->m_kPos.x + pkSOther->m_fRadius/2;x++)
	{
		for(float z=pkSOther->m_kPos.z - pkSOther->m_fRadius/2; z<pkSOther->m_kPos.z + pkSOther->m_fRadius/2;z++)
		{
			i++;
			h+=m_pkMap->Height(x,z);
		};
	};
	
	h /= i;
	*/
	
	
//	h=m_pkMap->Height(pkSOther->m_kPos.x,pkSOther->m_kPos.z);
	h=m_pkMap->Height(pkSOther->m_kPos.x,pkSOther->m_kPos.z);


	Vector3 kNewPos=m_pkPhysEngine->GetNewPos(kOther->m_pkPP);
	
	h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
	if( h < (kNewPos.y-kOther->m_fRadius) )
		return NULL;

	
	kNewPos.y=h+kOther->m_fRadius;//+0.001;
	
	
/*	
	float distance=(O2->GetPos()-kNewPos).Length();
	float fac = distance / O2->GetVel();
	cout<<"BLUB "<<md<<endl;
	
	Vector3 nmv=O2->GetVel()*md;
*/	
//	Vector3 kVel=O2->GetPos()-kPos;


	//assemble collision data
	CollisionData* tempdata = new CollisionData;
	tempdata->m_pkOther = O2;
	tempdata->m_kOtherPos = kNewPos;
	tempdata->m_kOtherVel = O2->GetVel();
	tempdata->m_kOtherAcc = O2->GetAcc();
	tempdata->m_kOtherRot = O2->GetRot();
	
	tempdata->m_kPos = O1->GetPos();
	tempdata->m_kVel = O1->GetVel();
	tempdata->m_kAcc = O1->GetAcc();
	tempdata->m_kRot = O1->GetRot();
	
	tempdata->m_kNormal=Vector3(0,0,0);
	
	return tempdata;
	
}









