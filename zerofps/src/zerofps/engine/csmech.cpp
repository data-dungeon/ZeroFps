#include "csmech.h"


CSMech::CSMech()
{
	m_fcoloffset=0.01;
}

Collision* CSMech::Test(CollisionShape* kOther,float fTime,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *kCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(kCs,fTime);		
	} else if(bContinue)
	{
		return kOther->Test(this,fTime,false);
	};
	
	//cout<<"Unhandled collision"<<endl;
	return NULL;
}

Collision* CSMech::Collide_CSSphere(CSSphere* kOther,float fTime)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();


	Vector3 test[3];
	test[0].Set(1,1,-1);
	test[1].Set(1,1,1);	
	test[2].Set(3,-1,-1);
/*
	test[0].Set(1,1,1);
	test[1].Set(-1,1,1);	
	test[2].Set(-1,-1,1);
*/


	bool hit=false;
	Vector3 HitPos,HitNormal;

	if(TestPolygon(test,O2->GetPos() , kOther->m_pkPP->m_kNewPos,kOther->m_fRadius))
	{
		//cout<<"Blub"<<endl;
		hit=true;
		HitPos=m_kColPos;
		HitNormal=m_kColNormal;
	}

	if(!hit)
		return NULL;
		
	
	if(kOther->m_pkPP->m_bGlide)
	{
		Vector3 NewPos=kOther->m_pkPP->m_kNewPos + (HitNormal * kOther->m_fRadius);

		Vector3 mov=NewPos - HitPos;
		Vector3 mov2=HitNormal.Proj(mov);
		HitPos=NewPos-mov2;
	}		

	
	Collision* tempdata = new Collision;
	
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = HitPos;
	tempdata->m_fDistance2 = (tempdata->m_kPos2 - O2->GetPos()).Length();
	tempdata->m_kNormal2.Set(0,1,0);
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = O1->GetPos(); //(movevec1*bla);
	tempdata->m_fDistance1 = (tempdata->m_kPos1 - O1->GetPos()).Length();
	tempdata->m_kNormal1 = HitNormal;
	

	return tempdata;
}



bool CSMech::TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2,float fR)
{		
	Vector3 kNLVerts[3];
	Plane P;
	
	//add objects possition to vertexs
	for(int i=0;i<3;i++)
		kNLVerts[i] = kVerts[i] + m_pkPP->GetObject()->GetPos();

//	cout<<"pos "<< m_pkPP->GetObject()->GetPos().x<<" "<< m_pkPP->GetObject()->GetPos().y<<" "<< m_pkPP->GetObject()->GetPos().z<<endl;

	Vector3 center= (kNLVerts[0]+kNLVerts[1]+kNLVerts[2])/3;	
//	cout<<"center= "<<center.x<<" "<<center.y<<" "<<center.z<<endl;
	
	Vector3 V1 = kNLVerts[1] - kNLVerts[0];
	Vector3 V2 = kNLVerts[2] - kNLVerts[0];		
	Vector3 Normal= (V1.Cross(V2).Unit());
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(center);	
	P.m_kNormal = Normal;


	cout<<"Normal "<<P.m_kNormal.x<<" "<<P.m_kNormal.y<<" "<<P.m_kNormal.z<<endl;
	cout<<"D "<<P.m_fD<<endl;
	
	if(!P.SphereInside(kPos1,-fR))
	{
//		cout<<"Röva"<<endl;
	}
	
	if(P.LineTest(kPos1 + (-Normal * fR), kPos2 + (-Normal * fR),&m_kColPos)){
		cout<<"Collision"<<endl;
		m_kColPos += (Normal*m_fcoloffset) + (Normal * fR);
		m_kColNormal = Normal;
		return true;
	}
	
	
	return false;
}
