#include "tcs.h"


Tcs::Tcs(): ZFSubSystem("Tcs")
{
	cout<<"Tiny Collission system created"<<endl;
	
}

Tcs::~Tcs()
{
	
}


bool Tcs::StartUp()	
{ 
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkRender		= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));		
	
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



void Tcs::Update()
{
	if(m_kBodys.empty())
		return;
	
	UpdateVel();					//calculate new vel
	CalcMotionSpheres();			//calculate motionspheres and new pos	
	UpdateCollissions();			//check for collissions
	UpdateMotion();				//move bodys
	
	UpdateLineTests();	
}

void Tcs::UpdateLineTests()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{		
		if(m_kBodys[i]->m_bCharacter)
		{
			if(TestLine(m_kBodys[i]->GetObject()->GetWorldPosV(),Vector3(0,-1,0),m_kBodys[i]))
			{
				float distance = (m_kBodys[i]->GetObject()->GetWorldPosV() - m_kLastTestPos).Length();
				if(distance < m_kBodys[i]->m_fLegLength)
				{
					m_kBodys[i]->GetObject()->SetWorldPosV(m_kLastTestPos + Vector3(0,m_kBodys[i]->m_fLegLength,0));
					Vector3 kVel  = m_kBodys[i]->GetObject()->GetVel();
					kVel.y = 0;
					m_kBodys[i]->GetObject()->SetVel(kVel);
				}			
			}		
		}
	}
}

void Tcs::UpdateVel()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		//apply gravity if enabled
		if(m_kBodys[i]->m_bGravity)
		{
			Vector3 kVel = m_kBodys[i]->GetObject()->GetVel() + Vector3(0,-4,0)*m_pkZeroFps->GetGameFrameTime();
			m_kBodys[i]->GetObject()->SetVel(kVel);
		}
	}
}

void Tcs::UpdateMotion()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
//		if(!m_kBodys[i]->m_bStatic)
		m_kBodys[i]->GetObject()->SetWorldPosV(m_kBodys[i]->m_kNewPos);
		m_kBodys[i]->GetObject()->RotateLocalRotV (m_kBodys[i]->m_kRotVel * m_pkZeroFps->GetGameFrameTime() );
		
		
//		m_pkRender->Sphere(m_kBodys[i]->GetObject()->GetWorldPosV(),m_kBodys[i]->m_fRadius,2,Vector3(1,0,0),false);
	}
}


void Tcs::UpdateCollissions()
{
	for(int B1=0;B1<m_kBodys.size();B1++)
	{
		for(int B2=B1+1;B2<m_kBodys.size();B2++)
		{
			bool bDoTest = false;
		
			//dont check collission groups
			if(m_kBodys[B1]->m_akTestGroups[m_kBodys[B2]->m_iGroup] ||
				m_kBodys[B2]->m_akTestGroups[m_kBodys[B1]->m_iGroup])
				bDoTest = true;
				
			if(m_kBodys[B1]->m_bCharacter || m_kBodys[B2]->m_bCharacter)
				if(m_kBodys[B1]->m_akWalkableGroups[m_kBodys[B2]->m_iGroup] ||
					m_kBodys[B2]->m_akWalkableGroups[m_kBodys[B1]->m_iGroup])					
					bDoTest = true;
				
			if(bDoTest)
			{
			
				bool bCollission=false;
			
				if(TestMotionSpheres(m_kBodys[B1],m_kBodys[B2]))
				{
					if(m_kBodys[B1]->m_bPolygonTest || m_kBodys[B2]->m_bPolygonTest)
					{
						if(TestMotionSphereVSMesh(m_kBodys[B1],m_kBodys[B2]))
							bCollission=true;
					}
					else
						bCollission=true;
				}
			
				if(bCollission)
				{
					m_kBodys[B1]->GetObject()->Touch(m_kBodys[B2]->GetObject()->iNetWorkID);
					m_kBodys[B2]->GetObject()->Touch(m_kBodys[B1]->GetObject()->iNetWorkID);				
					//cout<<"collission detected:"<<endl;
					
					//character handling
					if(m_kBodys[B1]->m_bCharacter)
						HandleCharacterCollission(m_kBodys[B1],m_kBodys[B2]);
					if(m_kBodys[B2]->m_bCharacter)
						HandleCharacterCollission(m_kBodys[B2],m_kBodys[B1]);
						
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
		
		pkCharacter->m_kNewPos = pkCharacter->GetObject()->GetWorldPosV() - dir * 0.05;
	}
}

Vector3 Tcs::GetNewPos(P_Tcs* pkBody)
{
	Entity* pkEnt = pkBody->GetObject();
	
	Vector3 NewPos = pkEnt->GetWorldPosV();
	NewPos += pkEnt->GetVel()*m_pkZeroFps->GetGameFrameTime();
		
	return NewPos;
}


bool Tcs::TestMotionSpheres(P_Tcs* pkB1,P_Tcs* pkB2)
{
	float Dist= (pkB1->m_kMSPos-pkB2->m_kMSPos).Length();

	if(Dist < (pkB1->m_fMSRadius + pkB2->m_fMSRadius))
		return true;
	else 		
		return false;
}

void Tcs::CalcMotionSpheres()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		//get new pos
		m_kBodys[i]->m_kNewPos = GetNewPos(m_kBodys[i]);
		
		//calculate motion sphere
		Vector3 temppos = m_kBodys[i]->GetObject()->GetWorldPosV();
		m_kBodys[i]->m_kMSPos.Lerp(temppos,m_kBodys[i]->m_kNewPos,0.5);				
		m_kBodys[i]->m_fMSRadius = ((temppos - m_kBodys[i]->m_kNewPos).Length()/2) + m_kBodys[i]->m_fRadius;	
	}
}


bool Tcs::TestMotionSphereVSMesh(P_Tcs* pkB1,P_Tcs* pkB2)
{
	bool b1pd=false;
	bool b2pd=false;
	
	//check if data is correct
	if(pkB1->m_bPolygonTest)
	{
		if(!pkB1->m_bHavePolygonData)
		{
			cout<<"error Mesh has polygon test but no meshdata was found"<<endl;
			return false;
		}
		else
			b1pd=true;
	}
	
	if(pkB2->m_bPolygonTest)
	{
		if(!pkB2->m_bHavePolygonData)
		{
			cout<<"error Mesh has polygon test but no meshdata was found"<<endl;
			return false;
		}
		else
			b2pd=true;
	}
	
	//nobody has polygontest
	if(!b1pd && !b2pd)
	{
		cout<<"no object had valid meshdata"<<endl;
		return false;
	}
	
	//both bodys cant have polygon test at the moment
	if(b1pd && b2pd)
	{
		//cout<<"no support for mesh VS mesh collissions"<<endl;
		return false;		
	}
	
	
	//if b1 got data then we assume that b2 does not
	if(b1pd)
		return MeshVSSphereTest(pkB1,pkB2);
	else		
		return MeshVSSphereTest(pkB2,pkB1);
	
	return false;
}

bool Tcs::MeshVSSphereTest(P_Tcs* pkMesh,P_Tcs* pkSphere)
{
	
	GenerateModelMatrix(pkMesh);
	
	
	for(int f=0;f<pkMesh->m_pkFaces->size();f++)
	{
		Vector3 verts[3];
		
		verts[0] = m_kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
		verts[1] = m_kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
		verts[2] = m_kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
	
/*		//debug		
		m_pkRender->SetColor(Vector3(1,1,1));
		m_pkRender->Line(verts[0],verts[1]);
		m_pkRender->Line(verts[1],verts[2]);		
		m_pkRender->Line(verts[2],verts[0]);		
*/		
		
		
		if(TestSphereVSPolygon(verts,pkSphere))
			return true;
		
		
	}
	
	return false;
}

bool Tcs::TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere)
{
	Vector3 kPos1 = pkSphere->GetObject()->GetWorldPosV();
	Vector3 kPos2 = pkSphere->m_kNewPos;

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
	
	if(P.LineTest(kPos1 - (Normal * (pkSphere->m_fRadius)), kPos2 - (Normal * pkSphere->m_fRadius),&kColPos))
	{

		if(TestSides(kVerts,&Normal,kColPos,pkSphere->m_fRadius))
		{
			m_kLastTestPos = kColPos;
			return true;				
		}
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
//		m_pkObject->SetWorldPosV(m_pkObject->GetWorldPosV() + Vector3(0,-1,0) * m_pkFps->GetFrameTime());					
	
	m_kLastTestPos = kStart;		
	float closest = 999999999;
	P_Tcs* pkClosest = NULL;	
	
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{		
		if(m_kBodys[i] == pkTester)
			continue;
			
		if(!pkTester->m_akWalkableGroups[m_kBodys[i]->m_iGroup])
			continue;			
		
		
		if(TestLineVSMesh(kStart,kDir,m_kBodys[i]))
		{
			float d = (kStart - m_kLastLineTestColPos).Length();		
		
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
	if(pkB->m_pkMad)
	{	
		if(pkB->m_pkMad->TestLine(kStart,kDir))
		{
			m_kLastLineTestColPos = pkB->m_pkMad->GetLastColPos();
			return true;
		}
	}



	return false;	
}
/*
bool P_Mad::LineVSSphere(Vector3 &kPos,Vector3 &kDir,P_Tcs* pkB)
{
	Vector3 c = pkB->GetObject()->GetWorldPosV() - kPos;		
	kDir.Normalize();		
	Vector3 k = kDir.Proj(c);		
	
	float cdis=c.Length();
	float kdis=k.Length();
	float Distance = sqrt((cdis*cdis)-(kdis*kdis));
	
	if(Distance < pkB->m_fRadius)
		return true;

	return false;
}
*/



