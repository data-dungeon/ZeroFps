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

	Vector3 V1 = kNLVerts[1] - kNLVerts[0];
	Vector3 V2 = kNLVerts[2] - kNLVerts[0];		
	Vector3 Normal= (V1.Cross(V2).Unit());
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kNLVerts[0]);	
	P.m_kNormal = Normal;


//	cout<<"Normal "<<P.m_kNormal.x<<" "<<P.m_kNormal.y<<" "<<P.m_kNormal.z<<endl;
//	cout<<"D "<<P.m_fD<<endl;	
	
	if(P.LineTest(kPos1 + (-Normal * fR), kPos2 + (-Normal * fR),&m_kColPos)){
		if(TestSides(kNLVerts,&Normal,m_kColPos,fR))
		{
//			cout<<"Collision"<<endl;
			m_kColPos += (Normal*m_fcoloffset) + (Normal * fR);
			m_kColNormal = Normal;
			return true;
		}
	}
	
	
	return false;
}

bool CSMech::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR)
{
	Plane side[3];
	
	Vector3 V1 = kVerts[0] - kVerts[1];
	Vector3 V2 = kVerts[2] - kVerts[0];	
	Vector3 V3 = kVerts[1] - kVerts[2];	
	
	side[0].m_kNormal = (V1.Cross(*pkNormal).Unit());
	side[1].m_kNormal = (V2.Cross(*pkNormal).Unit());	
	side[2].m_kNormal = (V3.Cross(*pkNormal).Unit());
	
	side[0].m_fD = -side[0].m_kNormal.Dot(kVerts[0]);
	side[1].m_fD = -side[1].m_kNormal.Dot(kVerts[0]);	
	side[2].m_fD = -side[2].m_kNormal.Dot(kVerts[1]);	
	
	
	bool inside = true;
	
	for(int i=0;i<3;i++)
	{
		if(!side[i].SphereInside(kPos,fR))
			inside=false;
	
	}
	
	return inside;
	
/*
	Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	
	
	pkRender->Line(kVerts[0],kVerts[0]+side[0].m_kNormal);
	pkRender->Line(kVerts[1],kVerts[1]+side[0].m_kNormal);		
	
	pkRender->Line(kVerts[0],kVerts[0]+side[1].m_kNormal);
	pkRender->Line(kVerts[2],kVerts[2]+side[1].m_kNormal);		
	
	pkRender->Line(kVerts[1],kVerts[1]+side[2].m_kNormal);
	pkRender->Line(kVerts[2],kVerts[2]+side[2].m_kNormal);		
*/	
	
//	return true;
}
