#include "csmech.h"


CSMech::CSMech()
{
	m_fcoloffset = 0.0001;
	m_bHavePolygonData = false;
	m_pkCore = 	NULL;
	m_iModelID = 0;
	m_fScale = 	1;
	
	m_bOtherGlide=false;
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
	if(!m_bHavePolygonData)
	{	
		if(SetUpMech())
			m_bHavePolygonData=true;
	
	}

	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();

	Vector3 kPos1 = O2->GetPos();
	Vector3 kPos2 = kOther->m_pkPP->m_kNewPos;

	
	bool hit=false;
	m_kCollidedFaces.clear();
	Vector3 HitNormal;
	m_bOtherGlide=kOther->m_pkPP->m_bGlide;
	m_kOtherDest=kOther->m_pkPP->m_kNewPos;

	Vector3 data[3];
	for(int i=0;i<m_pkFaces->size();i++)
	{

		for(int j=0;j<3;j++)
			data[j] = (*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]];
	
		if(TestPolygon(data,kPos1,kPos2,kOther->m_fRadius))
		{
			hit=true;		
			
//			if(Closer(kPos1,kPos2,m_kColPos))
//			{
				//cout<<"Blub"<<endl;
				HitNormal=m_kColNormal;
				kPos2=m_kColPos;
//			}
				
				//go trough all previous added collisions and check if the still occurs
				for(list<int>::iterator it=m_kCollidedFaces.begin();it!=m_kCollidedFaces.end();it++)				
				{
					Vector3 temp[3];
					for(int l=0;l<3;l++)
						temp[l] = (*m_pkVertex)[ (*m_pkFaces)[(*it)].iIndex[l]];

				
					if(TestPolygon(temp,kPos1,kPos2,kOther->m_fRadius))
					{
						cout<<"collision still occurs"<<endl;					
//						if(Closer(kPos1,kPos2,m_kColPos))
//						{						
							
							cout<<"Blub"<<endl;
							hit=true;
							HitNormal=m_kColNormal;
							kPos2=m_kColPos;				
//						}
					}
					else										
					{
						cout<<"removing old collision"<<endl;
						if(m_kCollidedFaces.size() > 1)
						{					
							list<int>::iterator oldit = it;
							it--;
							
							m_kCollidedFaces.erase(oldit);
						}
						else
						{
							it=m_kCollidedFaces.end();
							m_kCollidedFaces.clear();						
						}
					}
				}	
				
				//add face to collided faces
				m_kCollidedFaces.push_back(i);

//			}			
		}
	}



	if(!hit)
		return NULL;
		
				//glide point if wanted
/*				if(kOther->m_pkPP->m_bGlide)
				{
					Vector3 NewPos=kOther->m_pkPP->m_kNewPos + (HitNormal * kOther->m_fRadius);
		
					Vector3 mov=NewPos - kPos2;
					Vector3 mov2=HitNormal.Proj(mov);
					kPos2=NewPos-mov2;
				}						*/
						
		
	cout<<"still collides with"<<m_kCollidedFaces.size()<<endl;	

	
	Collision* tempdata = new Collision;
	
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = kPos2;
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
	for(int i=0;i<3;i++){
		kNLVerts[i] = (kVerts[i] * m_fScale)  + m_pkPP->GetObject()->GetPos();
		
//		cout<<"vertex "<<kNLVerts[i].x<<" "<<kNLVerts[i].y<<" "<<kNLVerts[i].z<<endl;		
//		Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
//		pkRender->DrawBox(kNLVerts[i],Vector3(0,0,0),Vector3(0.1,0.1,0.1),-1);
		
	}

	Vector3 V1 = kNLVerts[1] - kNLVerts[0];
	Vector3 V2 = kNLVerts[2] - kNLVerts[0];		
	Vector3 Normal= V1.Cross(V2);
	
	if(Normal.Length() == 0)
	{
		Normal.Set(0,1,0);
	}
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kNLVerts[0]);	
	P.m_kNormal = Normal;


//	cout<<"Normal "<<P.m_kNormal.x<<" "<<P.m_kNormal.y<<" "<<P.m_kNormal.z<<endl;
//	cout<<"D "<<P.m_fD<<endl;	
	
	if(P.LineTest(kPos1 + (-Normal * fR), kPos2 + (-Normal * fR),&m_kColPos)){
//	if(P.LineTest(kPos1 + (-Normal * fR), kPos2  ,&m_kColPos)){
		if(TestSides(kNLVerts,&Normal,m_kColPos,fR))
		{
			//cout<<"Collision"<<endl;
			m_kColNormal = Normal;
			m_kColPos += (Normal*m_fcoloffset) + (Normal * fR);			
			
			
			//glide point
			if(m_bOtherGlide)
			{
				Vector3 NewPos=m_kOtherDest + (Normal * fR);	
				Vector3 mov=NewPos - m_kColPos;
				Vector3 mov2=Normal.Proj(mov);
				m_kColPos=NewPos-mov2;		
			}							
			
			return true;
		}
	}
	
	
	return false;
}

bool CSMech::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR)
{
//	fR+=0.5;

	Plane side[3];
	
	if(kVerts[0] == kVerts[1] || 
		kVerts[2] == kVerts[1] ||
		kVerts[1] == kVerts[2])
		return false;
	
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
//		if(!side[i].PointInside(kPos))
//			inside=false;
	
	}
	

/*	

	Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	
	
	pkRender->Line(kVerts[0],kVerts[0]+side[0].m_kNormal);
	pkRender->Line(kVerts[1],kVerts[1]+side[0].m_kNormal);		
	
	pkRender->Line(kVerts[0],kVerts[0]+side[1].m_kNormal);
	pkRender->Line(kVerts[2],kVerts[2]+side[1].m_kNormal);		
	
	pkRender->Line(kVerts[1],kVerts[1]+side[2].m_kNormal);
	pkRender->Line(kVerts[2],kVerts[2]+side[2].m_kNormal);		
*/	
	
	return inside;	
//	return true;
}


bool CSMech::SetUpMech()
{
	//look for mad property
	MadProperty* pkMP = static_cast<MadProperty*>(m_pkPP->GetObject()->GetProperty("MadProperty"));	
	if(pkMP != NULL)
	{
		cout<<"found mad property"<<endl;
		//look for core pointer in mad property
		m_pkCore = pkMP->pkCore;	
		if(m_pkCore != NULL)
		{
			cout<<"found core"<<endl;
			//look for mech pointer in core
			m_pkCoreMech = m_pkCore->GetMeshByID(m_iModelID);					
			if(m_pkCoreMech != NULL)		
			{
				cout<<"found mech"<<endl;
				
				m_pkFaces = m_pkCoreMech->GetFacesPointer();
				m_pkVertex = (*m_pkCoreMech->GetVertexFramePointer())[0].GetVertexPointer();
				m_pkNormal = (*m_pkCoreMech->GetVertexFramePointer())[0].GetNormalPointer();
				
				//found the mech return true
				return true;
			}
		}
	
	}
	
	cout<<"error mech not found"<<endl;
	return false;

/*
	//did not find any mad property return false
	if(m_pkMP == NULL)
		return false;
	
	//get core pointer
	m_pkCore = m_pkMadProperty->pkCore;
	
	//is the core pointer vallid?
	if(m_pkCore == NULL)
		return false;
	
	cout<<"found mad property"<<endl;
	
	//get mech pointer
	m_pkCoreMech = m_pkCore->GetMeshByID(m_iModelID);
		
	//is the mech pointer valid?
	if(m_pkCoreMech == NULL)
		return false;
		
	//everything was ok return true
	return true;
*/	
}

Vector3& CSMech::Closest(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2)
{
	if( (kCurPos-OPos1).Length() < (kCurPos-OPos2).Length())
		return OPos1;
	else
		return OPos2;
}

bool CSMech::Closer(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2)
{
	if( (kCurPos-OPos2).Length() < (kCurPos-OPos1).Length())
		return true;
	else
		return false;
}
