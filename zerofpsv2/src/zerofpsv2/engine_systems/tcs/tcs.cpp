#include "tcs.h"
#include "../propertys/p_tcs.h"
#include "../common/heightmap.h"

Tcs::Tcs(): ZFSubSystem("Tcs")
{
	Logf("zerofps","Tiny Collission system created");
	//cout<<"Tiny Collission system created"<<endl;
	
}

Tcs::~Tcs()
{
	delete m_pkBodyCopy1;
	delete m_pkBodyCopy2;
}


bool Tcs::StartUp()	
{ 
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkRender		= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));		
	
	m_iMaxTests = 20;
	
	m_pkBodyCopy1 = new P_Tcs;
	m_pkBodyCopy2 = new P_Tcs;	
	
	return true; 
}

bool Tcs::ShutDown() 
{ 
	return true; 
}
bool Tcs::IsValid()	
{ 
	return true; 
}



void Tcs::AddBody(P_Tcs* pkPTcs)
{
	//check so its not added twice
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i] == pkPTcs)
			return;
	
	}
	
	m_kBodys.push_back(pkPTcs);
	//cout<<"boddy added"<<endl;
}
void Tcs::RemoveBody(P_Tcs* pkPTcs)
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i] == pkPTcs)
		{	
			m_kBodys[i]=m_kBodys.back();			//chacke place whit bak elment 
			m_kBodys.pop_back();
			
			//cout<<"body removed"<<endl;			
			return;
		}
	}
}



void Tcs::Update(float fAlphaTime)
{
	if(m_kBodys.empty())
		return;
	
	float fRTime = fAlphaTime;
	float iNrOfCollissions = 0;
	
	//synd all bodys to entitys
	SyncBodys();
	
	//make sure there is no collissions	
	m_kCollissions.clear();	
	
	UpdateForces();
	
	while(fRTime > 0)
	{
		//do collissiontests
		UpdateCollissions(fRTime);
	
		//did any collission ocur?
		if(m_kCollissions.size() > 0)
		{
			iNrOfCollissions++;
		
			//get next closest collission	
			Tcs_collission* pkCol = FindNextCollission();			
			
			//update all objects up to current time
			UpdateAllVelnPos(pkCol->fAtime);			
			
			//handle collission
			HandleCollission(pkCol);
			
			//dont shrink rest time with to little not good for FPS =)
			if(pkCol->fAtime < 0.001)
				pkCol->fAtime = 0.001;
		
			//set new Resttime			
			fRTime -= pkCol->fAtime;			
			
			//clear all collissions
			m_kCollissions.clear();
		
		}
		//no collissions, just update time and continue
		else 
		{
			UpdateAllVelnPos(fRTime);	
			fRTime = 0;
		}	
	}
	
	//update all character ground line tests
	UpdateLineTests();
	
	//synd all entitys to bodys
	SyncEntitys();
	
	if(iNrOfCollissions > 0)
		cout<<"collissions:"<<iNrOfCollissions<<endl;

}

void Tcs::UpdateLineTests()
{
	float distance;

	for(unsigned int i=0;i<m_kBodys.size();i++)
	{		
		if(m_kBodys[i]->m_bCharacter)
		{
			
			if(TestLine(m_kBodys[i]->m_kNewPos,Vector3(0,-1,0),m_kBodys[i]))
			{
				distance = m_kBodys[i]->m_kNewPos.DistanceTo(m_kLastTestPos);
				if(distance < m_kBodys[i]->m_fLegLength)
				{
					m_kBodys[i]->m_kNewPos = m_kLastTestPos + Vector3(0,m_kBodys[i]->m_fLegLength,0);
					m_kBodys[i]->m_bOnGround = true;
					m_kBodys[i]->m_kVelocity.y = 0;
				}			
				else
					m_kBodys[i]->m_bOnGround = false;				
			}
		}
	}
}

void Tcs::HandleCollission(Tcs_collission* pkCol)
{
//	m_pkRender->Sphere(pkCol->kPos,0.1,1,Vector3(1,1,0),false);
//	m_pkRender->Line(pkCol->pkBody1->m_kNewPos,pkCol->pkBody1->m_kNewPos + pkCol->kNormal);	
			
	float b = 2;		
			
			
	float j  = (-(1+b) * (pkCol->kRelVel * pkCol->kNormal)) /
				  ( (pkCol->kNormal*pkCol->kNormal) *
				  ( 1/pkCol->pkBody1->m_fMass + 1/pkCol->pkBody2->m_fMass)); 	
			
			
	//make sure the power is not to small
	if(j < 0.5)
		j = 0.5;
			
//	if(j > 10)
//		return;
			
	//cout<<"J:"<<j<<endl;
			
			
	//cout<<"handling collission"<<endl;
	if(!pkCol->pkBody1->m_bStatic)
	{		
		pkCol->pkBody1->m_kVelocity += (pkCol->kNormal * j) / pkCol->pkBody1->m_fMass;	
	}

	if(!pkCol->pkBody2->m_bStatic)
	{
		pkCol->pkBody2->m_kVelocity -= (pkCol->kNormal * j) / pkCol->pkBody2->m_fMass;			
	}


	//touch objects
	pkCol->pkBody1->GetObject()->Touch(pkCol->pkBody2->GetObject()->GetEntityID());
	pkCol->pkBody2->GetObject()->Touch(pkCol->pkBody1->GetObject()->GetEntityID());	
}

void Tcs::SyncEntitys()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
	
		m_kBodys[i]->GetObject()->SetWorldPosV(m_kBodys[i]->m_kNewPos);
		m_kBodys[i]->GetObject()->SetVel(m_kBodys[i]->m_kVelocity);		
	}
}

void Tcs::SyncBodys()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
//		m_pkRender->Sphere(m_kBodys[i]->m_kNewPos,m_kBodys[i]->m_fRadius,1,Vector3(1,0,0),false);
		
		m_kBodys[i]->m_kNewPos = m_kBodys[i]->GetObject()->GetWorldPosV();
		m_kBodys[i]->m_kVelocity = m_kBodys[i]->GetObject()->GetVel();				
	}
}

void Tcs::UpdateForces()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic)
			continue;
	
		m_kBodys[i]->m_kForces.Set(0,0,0);
		
		//Apply walk force
		m_kBodys[i]->m_kForces+=m_kBodys[i]->m_kWalkVel;
		
		//apply gravity if enabled
		if(m_kBodys[i]->m_bGravity)
		{
			m_kBodys[i]->m_kForces += Vector3(0,-10,0);
		}
	}
}

void Tcs::UpdateAllVelnPos(float fAtime)
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic)
			continue;		
			
		UpdateBodyVelnPos(m_kBodys[i],fAtime);				
	}
}



void Tcs::UpdateBodyVelnPos(P_Tcs* pkBody,float fAtime)
{
	if(pkBody->m_bStatic)
		return;

	Vector3 kAe;
	
	//apply some air friction		
	pkBody->m_kForces -=	pkBody->m_kVelocity*4;
		
	//Calculate acceleration in world space
	kAe = pkBody->m_kForces / pkBody->m_fMass;
	pkBody->m_fAcceleration = kAe;
		
	//Calculate velocity in world space
	pkBody->m_kVelocity += kAe * fAtime;
		
		
	//Calculate position in world space
	pkBody->m_kNewPos += (pkBody->m_kVelocity * fAtime);// + (pkBody->m_kWalkVel * fAtime);
	
}

void Tcs::UpdateCollissions(float fAtime)
{
	for(unsigned int B1=0;B1<m_kBodys.size();B1++)
	{
		for(unsigned int B2=B1+1;B2<m_kBodys.size();B2++)
		{
/*			bool bDoTest = false;
		
			//check collission groups
			if(m_kBodys[B1]->m_akTestGroups[m_kBodys[B2]->m_iGroup] ||
				m_kBodys[B2]->m_akTestGroups[m_kBodys[B1]->m_iGroup])
				bDoTest = true;
				
			//check walkable
			if(m_kBodys[B1]->m_bCharacter || m_kBodys[B2]->m_bCharacter)
				if(m_kBodys[B1]->m_akWalkableGroups[m_kBodys[B2]->m_iGroup] ||
					m_kBodys[B2]->m_akWalkableGroups[m_kBodys[B1]->m_iGroup])					
					bDoTest = true;
				
			if(bDoTest)*/
			
			
			if(m_kBodys[B1]->m_akTestGroups[m_kBodys[B2]->m_iGroup] ||
				m_kBodys[B2]->m_akTestGroups[m_kBodys[B1]->m_iGroup])			
			{
			
				if(m_kBodys[B1]->m_bPolygonTest && m_kBodys[B2]->m_bPolygonTest)
				{
					cout<<"unsuported collission mesh VS mesh"<<endl;
				}
				else if(m_kBodys[B1]->m_bPolygonTest || m_kBodys[B2]->m_bPolygonTest)
				{				
					TestSphereVsMesh(m_kBodys[B1],m_kBodys[B2],fAtime);
				}
				else
				{
					TestSphereVsSphere(m_kBodys[B1],m_kBodys[B2],fAtime);
				}				
			}
		}
	}
}


void Tcs::HandleCharacterCollission(P_Tcs* pkCharacter,P_Tcs* pkBody)
{
	if(pkCharacter->m_akWalkableGroups[pkBody->m_iGroup])
	{
			
		Vector3 dir = (m_kLastTestPos - pkCharacter->GetObject()->GetWorldPosV()).Unit();
		
		pkCharacter->m_kNewPos = pkCharacter->GetObject()->GetWorldPosV() - dir * 0.1;
	}
}


bool Tcs::TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere)
{
	Vector3 kPos1 = pkSphere->m_kNewPos;

	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[0];		
	Vector3 Normal= V1.Cross(V2);
	
	if(Normal.Length() == 0)
	{
		return false;
	}
	
	Normal.Normalize();
	Plane P;
	P.m_fD = -Normal.Dot(kVerts[0]);	
	P.m_kNormal = Normal;

	Vector3 kColPos;
	
	if(P.LineTest(kPos1 + (Normal * pkSphere->m_fRadius),kPos1 - (Normal * pkSphere->m_fRadius) ,&kColPos))
	{

		if(TestSides(kVerts,&Normal,kColPos,0))
		{
			m_kLastTestNormal = Normal;
			m_kLastTestPos = kColPos;
			return true;
		}
	}
	
	//do edge tests
	bool didcollide = false;
	float closest = 99999999;
	
	int p1,p2;
	float d;
	
	for(int i = 0;i<3;i++)
	{
		switch(i)
		{
			case 0:
				p1 = 0;
				p2 = 1;
				break;
			case 1:
				p1 = 1;
				p2 = 2;
				break;
			case 2:
				p1 = 2;
				p2 = 0;
				break;
		}
			
	
		if(TestLineVSSphere(kVerts[p1], kVerts[p2],pkSphere))
		{
			d = pkSphere->m_kNewPos.DistanceTo(m_kLastTestPos);
			if( d < closest)
			{
				closest = d;
				kColPos = m_kLastTestPos;
				didcollide = true;
			}
		}		
	}
	
	if(didcollide)
	{
		m_kLastTestNormal = Normal;
		m_kLastTestPos = kColPos;
		return true;
	}
	
	
	return false;
}

bool Tcs::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR)
{
	Plane side[6];
	
	
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
	}
	
	return inside;	
}


void Tcs::GenerateModelMatrix(P_Tcs* pkMesh)
{
	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(pkMesh->m_fScale,pkMesh->m_fScale,pkMesh->m_fScale);
	m_kModelMatrix*=pkMesh->GetObject()->GetWorldOriM();

}


P_Tcs* Tcs::TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester)
{
	
	
	m_kLastTestPos = kStart;		
	float closest = 999999999;
	P_Tcs* pkClosest = NULL;	
	float d;
	
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{		
		if(m_kBodys[i] == pkTester)
			continue;
			
		if(!pkTester->m_akWalkableGroups[m_kBodys[i]->m_iGroup])
			continue;			
		
		
		if(TestLineVSMesh(kStart,kDir,m_kBodys[i]))
		{
			d = kStart.DistanceTo(m_kLastLineTestColPos);				
			if(d < closest)
			{
				
				m_kLastTestPos = m_kLastLineTestColPos;
				closest = d;
				pkClosest = m_kBodys[i];
			}				
		}		
	}		
	
	return pkClosest;
}

bool Tcs::TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB)
{
	Vector3 kColPos;

	if(pkB->m_pkMad)
	{	
		if(pkB->m_pkMad->TestLine(kStart,kDir))
		{
			m_kLastLineTestColPos = pkB->m_pkMad->GetLastColPos();
			return true;
		}
	}
	else if (pkB->m_pkHmap) {
			if(pkB->LineVSMesh(kStart,kDir))
			{
				m_kLastLineTestColPos = pkB->m_kColPos;
				return true;
			}
		}



	return false;	
}

bool Tcs::TestLineVSSphere(Vector3 kP1,Vector3 kP2,P_Tcs* pkB)
{
	Vector3 kDir = kP2 - kP1;

	Vector3 c=pkB->m_kNewPos - kP1;		

	float d = kDir.Unit().Dot(c);
	if(d < 0)
		return false;
	if(d > kDir.Length())
		return false;
	
	kDir.Normalize();		
	Vector3 k=kDir.Proj(c);		
	float cdis=c.Length();
	float kdis=k.Length();
	float Distance = (float) sqrt((cdis*cdis)-(kdis*kdis));
	

	if(Distance < pkB->m_fRadius)
	{			
		m_kLastTestPos = kP1 + k;
		return true;
	}		
	
	return false;
}


Tcs_collission* Tcs::FindNextCollission()
{
	if(m_kCollissions.size() == 0)
		return NULL;
		
	float fClosest = 9999999999.0;
	Tcs_collission* pkClosest = NULL;
	
	for(unsigned int i=0;i<m_kCollissions.size();i++)
	{
		if(m_kCollissions[i].fAtime < fClosest)
		{
			fClosest = m_kCollissions[i].fAtime;
			pkClosest = &m_kCollissions[i];
		}
	}
	
	return pkClosest;
}




void Tcs::TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime)
{

	bool retry = true;
	bool didpen = false;
	int nroftests = 0;
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));
		
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	
		if(CollideSphereVSSphere(m_pkBodyCopy1,m_pkBodyCopy2))
		{
			//if first penetration do a check at time 0
			if(!didpen)
			{
				memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
				memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
			
				UpdateBodyVelnPos(m_pkBodyCopy1,0);
				UpdateBodyVelnPos(m_pkBodyCopy2,0);	
			
				if(CollideSphereVSSphere(m_pkBodyCopy1,m_pkBodyCopy2))
				{
					didpen = true;			
					break;					
				}
			}		

			
			didpen = true;			
			retry = true;
			fAtime /=1.5;
		
			continue;
		}
		else if(didpen)
			break;
		else
			return;
		
	}	
	
	if(didpen)
	{	
		
		
		Tcs_collission temp;
		temp.pkBody1 = pkBody1;
		temp.pkBody2 = pkBody2;
	
		temp.kNormal = (m_pkBodyCopy1->m_kNewPos - m_pkBodyCopy2->m_kNewPos).Unit();
		temp.kPos = m_pkBodyCopy1->m_kNewPos - (temp.kNormal * m_pkBodyCopy1->m_fRadius);	
		temp.kRelVel = (m_pkBodyCopy1->m_kVelocity ) - (m_pkBodyCopy2->m_kVelocity);						
		temp.fAtime =	fAtime;
		
		m_kCollissions.push_back(temp);		
	}

}

bool Tcs::CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	float d = pkBody1->m_kNewPos.DistanceTo(pkBody2->m_kNewPos) - (pkBody1->m_fRadius + pkBody2->m_fRadius);
				
	if(d <= 0)
		return true;
				
	return false;	
}


void Tcs::TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime)
{
	bool retry = true;
	bool didpen = false;
	int nroftests = 0;
	
	
	//if body1 is the mesh just flip them,body2 shuld always be the mesh
	if(pkBody1->m_bPolygonTest)
	{
		P_Tcs* b2c = pkBody2;
	
		pkBody2 = pkBody1;
		pkBody1 = b2c;
	}
	
	//make sure body2 have all the pointers else get angry and cry
	if(!pkBody2->m_pkVertex)
		return;
		
	if(!pkBody2->m_pkFaces)
		return;
		
		
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
			
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
		
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	
		if(CollideSphereVSSphere(m_pkBodyCopy1,m_pkBodyCopy2))
			if(CollideSphereVSMesh(m_pkBodyCopy1,m_pkBodyCopy2))
			{
				//if first penetration do a check at time 0
				if(!didpen)
				{
					memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
					memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
				
					UpdateBodyVelnPos(m_pkBodyCopy1,0);
					UpdateBodyVelnPos(m_pkBodyCopy2,0);	
				
					if(CollideSphereVSMesh(m_pkBodyCopy1,m_pkBodyCopy2))
					{
						//cout<<"Stuck Object detected"<<endl;
						didpen = true;			
						break;
					}
				}
			
			
			
				didpen = true;			
				retry = true;
				fAtime /=1.5;
		
				continue;
			}
			else if(didpen)
				break;
			else
				return;
		
	}	
	
	if(didpen)
	{	
		
		
		Tcs_collission temp;
		temp.pkBody1 = pkBody1;
		temp.pkBody2 = pkBody2;
	
		temp.kNormal = m_kLastTestNormal;
		temp.kPos = m_kLastTestPos;	
		temp.kRelVel = (m_pkBodyCopy1->m_kVelocity ) - (m_pkBodyCopy2->m_kVelocity);				
		temp.fAtime =	fAtime;
		
		m_kCollissions.push_back(temp);		
	}	
}

bool Tcs::CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh)
{
	
	GenerateModelMatrix(pkMesh);
	
	float closest = 99999999;
	Vector3 kClosestNormal;
	Vector3 kClosestPos;
	bool bHaveColided = false;
	
	Vector3 verts[3];	
	float d;


	for(unsigned int f=0;f<pkMesh->m_pkFaces->size();f++)
	{
		
		verts[0] = m_kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
		verts[1] = m_kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
		verts[2] = m_kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
	
		if(TestSphereVSPolygon(verts,pkSphere))
		{	
			d = pkSphere->m_kNewPos.DistanceTo(m_kLastTestPos);
		
			if( d < closest)
			{
				closest = d;
				bHaveColided = true;
				kClosestPos = m_kLastTestPos;
				kClosestNormal = m_kLastTestNormal;
			}
		}
	
	}
	
	if(bHaveColided)
	{
		m_kLastTestPos = kClosestPos;
		m_kLastTestNormal = kClosestNormal;	
		return true;
	}
	
	return false;

}



