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
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();
	
	
//	cout<<"FYUCUASIDK POS:"<<O1->GetPos().x<<" "<<O1->GetPos().y<<" "<<O1->GetPos().z<<endl;	
	
	if(O2->GetPos().x<m_pkMap->GetPos().x-m_pkMap->GetSize()/2 ||
		O2->GetPos().x>m_pkMap->GetPos().x+m_pkMap->GetSize()/2 ||
		O2->GetPos().z<m_pkMap->GetPos().z-m_pkMap->GetSize()/2 ||
		O2->GetPos().z>m_pkMap->GetPos().z+m_pkMap->GetSize()/2)
		return NULL;
	
	
	
//	int i=0;
//	float h=0;
	
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
//	h=m_pkMap->Height(kOther->m_pkPP->m_kNewPos.x,kOther->m_pkPP->m_kNewPos.z);


	Vector3 kNewPos=kOther->m_pkPP->m_kNewPos;//m_pkPhysEngine->GetNewPos(kOther->m_pkPP);
	
	float h=m_pkMap->Height(kNewPos.x,kNewPos.z);
	
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
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = kNewPos;
	tempdata->m_kVel2 = O2->GetVel();
	tempdata->m_kAcc2 = O2->GetAcc();
	tempdata->m_kRot2 = O2->GetRot();
	tempdata->m_fDistance2 = (kNewPos - O2->GetPos()).Length();	
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = m_pkPP->m_kNewPos;//O1->GetPos();
	tempdata->m_kVel1 = O1->GetVel();
	tempdata->m_kAcc1 = O1->GetAcc();
	tempdata->m_kRot1 = O1->GetRot();
	tempdata->m_fDistance1 = 0;
		
	tempdata->m_kNormal=Vector3(0,0,0);
	
//	cout<<"Pos1:"<<tempdata->m_kPos1.x<<" "<<tempdata->m_kPos1.y<<" "<<tempdata->m_kPos1.z<<endl;
//	cout<<"Pos2:"<<tempdata->m_kPos2.x<<" "<<tempdata->m_kPos2.y<<" "<<tempdata->m_kPos2.z<<endl;
	
//	exit(1);
	
	return tempdata;
	
}









