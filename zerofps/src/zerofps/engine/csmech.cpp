#include "csmech.h"


CSMech::CSMech()
{
	m_fcoloffset = 0.001;
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
	
	if(!m_bHavePolygonData)
		return NULL;

	GenerateModelMatrix();

	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();

	Vector3 kPos1 = O2->GetPos();
	Vector3 kPos2 = kOther->m_pkPP->m_kNewPos;

	
	bool hit=false;
	bool newhit=false;
	m_kCollidedFaces.clear();
	Vector3 HitNormal;
	m_bOtherGlide=kOther->m_pkPP->m_bGlide;
	m_kOtherDest=kOther->m_pkPP->m_kNewPos;
	
	Vector3 kGlidePos;
	
	int c=0;

	hit=false;
	do
	{
		newhit=false;
		c++;
			
		Vector3 data[3];
		for(int i=0;i<m_pkFaces->size();i++)
		{
	
			for(int j=0;j<3;j++)
				data[j] = (*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]];
	
			if(TestPolygon(data,kPos1,kPos2,kOther->m_fRadius))
			{
				//cout<<"colliding with :"<<i<<endl;
				hit=true;
				newhit=true;
				kPos2=m_kColPos;
				kGlidePos=m_kGlideColPos;
				HitNormal=m_kColNormal;
			}		
		}
			
		if(m_bOtherGlide){
	 		if(newhit){
				
				//cout<<"diff:"<< (kPos1-kPos2).Length()<<endl;
				
				kPos1=kPos2;			
				kPos2=kGlidePos;					
				
				
				if(c>1){
					newhit=false;
					kPos2=kPos1;
				}

			
			}
		}
		
	}while(newhit && m_bOtherGlide && c < 30);
		
		
	if(!hit)
		return NULL;
			
			
	Collision* tempdata = new Collision;
	
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = kPos2;
	tempdata->m_fDistance2 = (tempdata->m_kPos2 - O2->GetPos()).Length();
	tempdata->m_kNormal2.Set(0,1,0);
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = O1->GetPos();
	tempdata->m_fDistance1 = (tempdata->m_kPos1 - O1->GetPos()).Length();
	tempdata->m_kNormal1 = HitNormal;
	

	return tempdata;
}



bool CSMech::TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2,float fR)
{		
	Vector3 kNLVerts[3];
	Plane P;
	

	if(kPos1 == kPos2)
		return false;

	//add objects possition to vertexs
	for(int i=0;i<3;i++){
		kNLVerts[i] = m_kModelMatrix.VectorTransform(kVerts[i]);
	
	}


	Vector3 V1 = kNLVerts[1] - kNLVerts[0];
	Vector3 V2 = kNLVerts[2] - kNLVerts[0];		
	Vector3 Normal= V1.Cross(V2);
	
	
	if(Normal.Length() == 0)
	{
		return false;
	}
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kNLVerts[0]);	
	P.m_kNormal = Normal;


//	cout<<"Normal "<<P.m_kNormal.x<<" "<<P.m_kNormal.y<<" "<<P.m_kNormal.z<<endl;
//	cout<<"D "<<P.m_fD<<endl;	
	
	
	
	if(P.LineTest(kPos1 - (Normal * (fR*0.95)), kPos2 - (Normal * fR),&m_kColPos)){
//	if(P.LineTest(kPos1 , kPos2 ,&m_kColPos)){	
		if(TestSides(kNLVerts,&Normal,m_kColPos,fR))
		{
			//cout<<"Collision with "<<endl;
			m_kColNormal = Normal;			
			
			
			Vector3 coloffset = Normal  * m_fcoloffset;// -((kPos2 - kPos1).Unit() * m_fcoloffset);
			
			Vector3 bla1=m_kOtherDest -m_kColPos;
			Vector3 bla2=Normal.Proj(bla1);
			m_kGlideColPos=m_kColPos + (bla1-bla2);
							
			m_kColPos += coloffset + (Normal * fR);						
			m_kGlideColPos += coloffset + (Normal * fR);
			
/*			
			Vector3 NewPos=m_kOtherDest + (Normal * fR);	
			Vector3 mov=NewPos - m_kColPos;
			Vector3 mov2=Normal.Proj(mov);
			m_kGlideColPos=NewPos-mov2;		*/
			
			return true;
		}
	}
	
	
	return false;
}

bool CSMech::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR)
{
//	cout<<"kollde point :"<<kPos.x<<" "<<kPos.y<<" "<<kPos.z<<endl;

//	fR-=0.05;

	Plane side[6];
	
	
/*	if(kVerts[0] == kVerts[1] || 
		kVerts[2] == kVerts[1] ||
		kVerts[0] == kVerts[2])
		return false;*/
	
	
	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[1];	
	Vector3 V3 = kVerts[0] - kVerts[2];	
	
	side[0].m_kNormal = pkNormal->Cross(V1).Unit();
	side[1].m_kNormal = pkNormal->Cross(V2).Unit();	
	side[2].m_kNormal = pkNormal->Cross(V3).Unit();

	side[3].m_kNormal = (side[0].m_kNormal + side[2].m_kNormal).Unit();
	side[4].m_kNormal = (side[0].m_kNormal + side[1].m_kNormal).Unit();
	side[5].m_kNormal = (side[1].m_kNormal + side[2].m_kNormal).Unit();


	side[0].m_fD = -side[0].m_kNormal.Dot(kVerts[0]);
	side[1].m_fD = -side[1].m_kNormal.Dot(kVerts[1]);	
	side[2].m_fD = -side[2].m_kNormal.Dot(kVerts[2]);	

	side[3].m_fD = -side[3].m_kNormal.Dot(kVerts[0]);
	side[4].m_fD = -side[4].m_kNormal.Dot(kVerts[1]);	
	side[5].m_fD = -side[5].m_kNormal.Dot(kVerts[2]);	
	
	
	bool inside = true;
	
	for(int i=0;i<6;i++)
	{
		if(!side[i].SphereInside(kPos,fR)){
			inside=false;
		}
//		if(!side[i].PointInside(kPos))
//			inside=false;
	
	}
	
		
/*		
	if(inside)	
	{
		Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
		
		pkRender->Line(kVerts[0],kVerts[0]+*pkNormal);
		pkRender->Line(kVerts[1],kVerts[1]+*pkNormal);		
	
		pkRender->Line(kVerts[1],kVerts[1]+*pkNormal);
		pkRender->Line(kVerts[2],kVerts[2]+*pkNormal);		
	
		pkRender->Line(kVerts[0],kVerts[0]+*pkNormal);
		pkRender->Line(kVerts[2],kVerts[2]+*pkNormal);		
		
/*		pkRender->Line(kVerts[0],kVerts[0]+side[0].m_kNormal);
		pkRender->Line(kVerts[1],kVerts[1]+side[0].m_kNormal);		
	
		pkRender->Line(kVerts[1],kVerts[1]+side[1].m_kNormal);
		pkRender->Line(kVerts[2],kVerts[2]+side[1].m_kNormal);		
	
		pkRender->Line(kVerts[0],kVerts[0]+side[2].m_kNormal);
		pkRender->Line(kVerts[2],kVerts[2]+side[2].m_kNormal);		
	}
*/	
	
	
	return inside;	
}


bool CSMech::SetUpMech()
{
	//look for mad property
	MadProperty* pkMP = static_cast<MadProperty*>(m_pkPP->GetObject()->GetProperty("MadProperty"));	
	if(pkMP != NULL)
	{
		//cout<<"found mad property"<<endl;
		//look for core pointer in mad property
		//m_pkCore = pkMP->pkCore;	
		m_pkCore = dynamic_cast<Mad_Core*>(pkMP->kMadHandle.GetResourcePtr()); 

		if(m_pkCore != NULL)
		{
			//cout<<"found core"<<endl;
			//look for mech pointer in core
			m_pkCoreMech = m_pkCore->GetMeshByID(m_iModelID);					
			if(m_pkCoreMech != NULL)		
			{
				//cout<<"found mech"<<endl;
				
				m_pkFaces = m_pkCoreMech->GetFacesPointer();
				m_pkVertex = (*m_pkCoreMech->GetVertexFramePointer())[0].GetVertexPointer();
				m_pkNormal = (*m_pkCoreMech->GetVertexFramePointer())[0].GetNormalPointer();
				
				m_fScale = pkMP->m_fScale;
				
				//found the mech return true
				return true;
			}
		}
	
	}
	
	cout<<"error mech not found"<<endl;
	return false;
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

void CSMech::GenerateModelMatrix()
{
	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(m_fScale,m_fScale,m_fScale);
	m_kModelMatrix.Rotate( m_pkPP->GetObject()->GetRot());	
	m_kModelMatrix.Translate(m_pkPP->GetObject()->GetPos());		

}





/*


	Vector3 data[3];
	for(int i=0;i<m_pkFaces->size();i++)
	{

		for(int j=0;j<3;j++)
			data[j] = (*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]];
	
		if(TestPolygon(data,kPos1,kPos2,kOther->m_fRadius))
		{
			hit=true;		
			HitNormal=m_kColNormal;
			kPos2=m_kColPos;			
			
			//go trough all previous added collisions and check if the still occurs
			for(list<int>::iterator it=m_kCollidedFaces.begin();it!=m_kCollidedFaces.end();it++)				
			{
				Vector3 temp[3];
				for(int l=0;l<3;l++)
					temp[l] = (*m_pkVertex)[ (*m_pkFaces)[(*it)].iIndex[l]];
				
				if(TestPolygon(temp,kPos1,kPos2,kOther->m_fRadius))
				{
					cout<<"collision still occurs "<<(*it)<<endl;					
						
					//hit=true;
					HitNormal=m_kColNormal;
					kPos2=m_kColPos;				
				}
				else										
				{
					cout<<"removing old collision "<<(*it)<<endl;
					
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
			cout<<"added "<<i<<" To collided faces"<<endl;			
		}
	}
*/





