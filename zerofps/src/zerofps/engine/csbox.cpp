#include "csbox.h"


CSBox::CSBox(Vector3 kScale)
{
	m_kScale=kScale;
	
	coloffset=0.00001;
}

Collision* CSBox::Test(CollisionShape* kOther,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *kCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(kCs);		
	} else if(bContinue)
	{
		return kOther->Test(this,false);
	};
	
	//cout<<"Unhandled collision"<<endl;
	return NULL;
}

Collision* CSBox::Collide_CSSphere(CSSphere* kOther)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();
	
	Vector3 op=O1->GetPos();
	

	bool hit=false;
	Vector3 HitPos;
	Vector3 HitNormal;
	
	
	if(TestTop(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius)){
		hit=true;
		HitPos=m_kColPos;
		HitNormal.Set(0,1,0);
	}
	
	if(TestBotom(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius)){
		hit=true;
		if(Closer(O2->GetPos(),HitPos,m_kColPos))
		{
			HitPos=m_kColPos;
			HitNormal.Set(0,-1,0);
		}
	}	
	if(TestFront(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius)){
		hit=true;
		if(Closer(O2->GetPos(),HitPos,m_kColPos))
		{
			HitPos=m_kColPos;
			HitNormal.Set(0,0,1);
		}		
	}
	if(TestBack(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius)){
		hit=true;
		if(Closer(O2->GetPos(),HitPos,m_kColPos))
		{
			HitPos=m_kColPos;
			HitNormal.Set(0,0,-1);
		}
	}
	if(TestLeft(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius)){
		hit=true;
		if(Closer(O2->GetPos(),HitPos,m_kColPos))
		{
			HitPos=m_kColPos;
			HitNormal.Set(-1,0,0);
		}
	}
	if(TestRight(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius)){
		hit=true;
		if(Closer(O2->GetPos(),HitPos,m_kColPos))
		{
			HitPos=m_kColPos;
			HitNormal.Set(1,0,0);
		}
	}
	

//	if(!hit)	
	bool inside=false;

	if(TestInside(O2->GetPos() , kOther->m_pkPP->m_kNewPos , kOther->m_fRadius))
	{
		HitPos=m_kColPos;
		hit=true;
		inside=true;
	}
	
	if(!hit)
		return NULL;
	
	if(!inside)
	{
		if(kOther->m_pkPP->m_bGlide)
		{
			Vector3 NewPos=kOther->m_pkPP->m_kNewPos + (HitNormal * kOther->m_fRadius);
	
			Vector3 mov=NewPos - HitPos;
			Vector3 mov2=HitNormal.Proj(mov);

			HitPos=NewPos-mov2;
		}
	}

	//--setup return data
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



bool CSBox::TestTop(Vector3 kPos1,Vector3 kPos2,float fR)
{
//	cout<<"testing top"<<endl;
	
	Vector3 kNormal(0,1,0);
	
	Plane surface;
	Plane side[4];


	surface.Set(kNormal,m_pkPP->GetObject()->GetPos()+(Vector3(0,0.5,0)*m_kScale));	
	side[0].Set(Vector3(0,0,1),m_pkPP->GetObject()->GetPos()+(Vector3(0,0.5,-0.5)*m_kScale));
	side[1].Set(Vector3(0,0,-1),m_pkPP->GetObject()->GetPos()+(Vector3(0,0.5,0.5)*m_kScale));		
	side[2].Set(Vector3(1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0.5,0)*m_kScale));		
	side[3].Set(Vector3(-1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0.5,0)*m_kScale));		
	
	bool hit= surface.LineTest(kPos1+ (-kNormal * fR), kPos2 + (-kNormal * fR),&m_kColPos);
	
	if(hit)
	{
		m_kColPos += (kNormal*coloffset) + (kNormal * fR);			
		for(int i=0;i<4;i++)
		{
			if(!side[i].SphereInside(m_kColPos,fR)){
				hit=false;
				//cout<<"outside "<<i<<endl;
			}
		}	
	}

	return hit;
	

}

bool CSBox::TestBotom(Vector3 kPos1,Vector3 kPos2,float fR)
{
//	cout<<"testing botom"<<endl;
	
	Vector3 kNormal(0,-1,0);
	
	Plane surface;
	Plane side[4];


	surface.Set(kNormal,m_pkPP->GetObject()->GetPos()+(Vector3(0,-0.5,0)*m_kScale));	
	side[0].Set(Vector3(0,0,1),m_pkPP->GetObject()->GetPos()+(Vector3(0,-0.5,-0.5)*m_kScale));
	side[1].Set(Vector3(0,0,-1),m_pkPP->GetObject()->GetPos()+(Vector3(0,-0.5,0.5)*m_kScale));		
	side[2].Set(Vector3(1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,-0.5,0)*m_kScale));		
	side[3].Set(Vector3(-1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,-0.5,0)*m_kScale));		
	
	bool hit= surface.LineTest(kPos1+ (-kNormal * fR), kPos2 + (-kNormal * fR),&m_kColPos);
	
	if(hit)
	{
		m_kColPos += (kNormal*coloffset) + (kNormal * fR);			
		for(int i=0;i<4;i++)
		{
			if(!side[i].SphereInside(m_kColPos,fR)){
				hit=false;
				//cout<<"outside "<<i<<endl;
			}
		}	
	}

	return hit;
	

}


bool CSBox::TestFront(Vector3 kPos1,Vector3 kPos2,float fR)
{
//	cout<<"testing front"<<endl;
	
	Vector3 kNormal(0,0,1);
	
	Plane surface;
	Plane side[4];


	surface.Set(kNormal,m_pkPP->GetObject()->GetPos()+(Vector3(0,0,0.5)*m_kScale));	
	side[0].Set(Vector3(0,-1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0,0.5,0.5)*m_kScale));
	side[1].Set(Vector3(0,1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0,-0.5,0.5)*m_kScale));		
	side[2].Set(Vector3(1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0,0.5)*m_kScale));		
	side[3].Set(Vector3(-1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0,0.5)*m_kScale));		
	
	bool hit= surface.LineTest(kPos1+ (-kNormal * fR), kPos2 + (-kNormal * fR),&m_kColPos);
	
	if(hit)
	{
		m_kColPos += (kNormal*coloffset) + (kNormal * fR);			
		for(int i=0;i<4;i++)
		{
			if(!side[i].SphereInside(m_kColPos,fR)){
				hit=false;
				//cout<<"outside "<<i<<endl;
			}
		}	
	}

	return hit;
	

}

bool CSBox::TestBack(Vector3 kPos1,Vector3 kPos2,float fR)
{
//	cout<<"testing back"<<endl;
	
	Vector3 kNormal(0,0,-1);
	
	Plane surface;
	Plane side[4];


	surface.Set(kNormal,m_pkPP->GetObject()->GetPos()+(Vector3(0,0,-0.5)*m_kScale));	
	side[0].Set(Vector3(0,-1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0,0.5,-0.5)*m_kScale));
	side[1].Set(Vector3(0,1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0,-0.5,-0.5)*m_kScale));		
	side[2].Set(Vector3(1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0,-0.5)*m_kScale));		
	side[3].Set(Vector3(-1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0,-0.5)*m_kScale));		
	
	bool hit= surface.LineTest(kPos1+ (-kNormal * fR), kPos2 + (-kNormal * fR),&m_kColPos);
	
	if(hit)
	{
		m_kColPos += (kNormal*coloffset) + (kNormal * fR);			
		for(int i=0;i<4;i++)
		{
			if(!side[i].SphereInside(m_kColPos,fR)){
				hit=false;
				//cout<<"outside "<<i<<endl;
			}
		}	
	}

	return hit;
	

}

bool CSBox::TestLeft(Vector3 kPos1,Vector3 kPos2,float fR)
{
//	cout<<"testing left"<<endl;
	
	Vector3 kNormal(-1,0,0);
	
	Plane surface;
	Plane side[4];


	surface.Set(kNormal,m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0,0)*m_kScale));	
	side[0].Set(Vector3(0,-1,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0.5,0)*m_kScale));
	side[1].Set(Vector3(0,1,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,-0.5,0)*m_kScale));		
	side[2].Set(Vector3(0,0,1),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0,-0.5)*m_kScale));		
	side[3].Set(Vector3(0,0,-1),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0,0.5)*m_kScale));		
	
	bool hit= surface.LineTest(kPos1+ (-kNormal * fR), kPos2 + (-kNormal * fR),&m_kColPos);
	
	if(hit)
	{
		m_kColPos += (kNormal*coloffset) + (kNormal * fR);			
		for(int i=0;i<4;i++)
		{
			if(!side[i].SphereInside(m_kColPos,fR)){
				hit=false;
				//cout<<"outside "<<i<<endl;
			}
		}	
	}

	return hit;
	

}

bool CSBox::TestRight(Vector3 kPos1,Vector3 kPos2,float fR)
{
//	cout<<"testing right"<<endl;
	
	Vector3 kNormal(1,0,0);
	
	Plane surface;
	Plane side[4];


	surface.Set(kNormal,m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0,0)*m_kScale));	
	side[0].Set(Vector3(0,-1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0.5,0)*m_kScale));
	side[1].Set(Vector3(0,1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,-0.5,0)*m_kScale));		
	side[2].Set(Vector3(0,0,1),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0,-0.5)*m_kScale));		
	side[3].Set(Vector3(0,0,-1),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0,0.5)*m_kScale));		
	
	bool hit= surface.LineTest(kPos1+ (-kNormal * fR), kPos2 + (-kNormal * fR),&m_kColPos);
	
	if(hit)
	{
		m_kColPos += (kNormal*coloffset) + (kNormal * fR);			
		for(int i=0;i<4;i++)
		{
			if(!side[i].SphereInside(m_kColPos,fR)){
				hit=false;
				//cout<<"outside "<<i<<endl;
			}
		}	
	}

	return hit;
	

}

bool CSBox::TestInside(Vector3 kPos1,Vector3 kPos2,float fR)
{
	Plane side[6];
	
	side[0].Set(Vector3(0,1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0,0.5,0)*m_kScale));	
	side[1].Set(Vector3(0,-1,0),m_pkPP->GetObject()->GetPos()+(Vector3(0,-0.5,0)*m_kScale));	
	side[2].Set(Vector3(1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(0.5,0,0)*m_kScale));		
	side[3].Set(Vector3(-1,0,0),m_pkPP->GetObject()->GetPos()+(Vector3(-0.5,0,0)*m_kScale));		
	side[4].Set(Vector3(0,0,1),m_pkPP->GetObject()->GetPos()+(Vector3(0,0,0.5)*m_kScale));	
	side[5].Set(Vector3(0,0,-1),m_pkPP->GetObject()->GetPos()+(Vector3(0,0,-0.5)*m_kScale));	
	
	
	bool inside=true;
	float fDist=-9999999;
	int iPlane;
	for(int i=0;i<6;i++)
	{
/*	
		if(!side[i].SphereInside(kPos1,fR))
			inside=false;
		
		if(!side[i].SphereInside(kPos2,fR))
			inside=false;
*/	
	
		if((side[i].SphereTest(kPos1,fR) > 0))
			inside=false;
		
		if((side[i].SphereTest(kPos2,fR) > 0))
			inside=false;
		
		if(!inside)	
			return false;

	
		if(inside)
		{
			float d=side[i].SphereTest(kPos2,fR);
			if(d > fDist){
				fDist=d;
				iPlane=i;
			}
		}
	}
	
	if(inside)
	{
		fDist=abs(fDist);
//		cout<<"dist:"<<fDist<<" Plane:"<<iPlane<<endl;
//		cout<<"moving "<<fDist<<" along "<<side[iPlane].m_kNormal.x<<" "<<side[iPlane].m_kNormal.y<<" "<<side[iPlane].m_kNormal.z<<endl;
		
		m_kColPos=kPos2+side[iPlane].m_kNormal*(fDist+coloffset);
//		Vector3 cola = kPos2-m_pkPP->GetObject()->GetPos();
//		Vector3 jolt = side[iPlane].m_kNormal.Proj(cola);
		
//		m_kColPos = m_pkPP->GetObject()->GetPos() +(cola-jolt) +  (side[iPlane].m_kNormal*(m_kScale/2 + fR + coloffset));
//		cout<<"NewPos: "<<m_kColPos.x<<" "<<m_kColPos.y<<" "<<m_kColPos.z<<endl;
	}
	
	return inside;
}




Vector3& CSBox::Closest(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2)
{
	if( (kCurPos-OPos1).Length() < (kCurPos-OPos2).Length())
		return OPos1;
	else
		return OPos2;
}

bool CSBox::Closer(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2)
{
	if( (kCurPos-OPos2).Length() < (kCurPos-OPos1).Length())
		return true;
	else
		return false;
}


/*
//Vector3 CSBox::TestQuad(Vector3 V1,Vector3 V2,Vector3 V3,Vector3 V4,Vector3 kPos1,Vector3 kPos2,float fR)
bool CSBox::TestQuad(Vector3 kCenter,Vector3 kNormal,float d,float fWidth,float fHeight,Vector3 kPos1,Vector3 kPos2,float fR)
{
	cout<<"testing quad"<<endl;
	
	Plane surface,up,down,left,right;

//	float d=kCenter.Length();
//	if(kCenter.y>0)
//		d=-d;

	surface.Set(kNormal,d);
	
	up.Set(kNormal+Vector3(0,0,0,d);
	
	cout<<"bla "<<d<<endl;
	cout<<"pos1: "<<kPos1.x<<" "<<kPos1.y<<" "<<kPos1.z<<endl;		
	cout<<"pos2: "<<kPos2.x<<" "<<kPos2.y<<" "<<kPos2.z<<endl;		
	
	
	
	bool hit= surface.LineTest(kPos1,kPos2 + (-kNormal * fR),&m_kColPos);
	
	
	m_kColPos += (kNormal*0.0001)+(kNormal * fR);
	return hit;



}
*/

