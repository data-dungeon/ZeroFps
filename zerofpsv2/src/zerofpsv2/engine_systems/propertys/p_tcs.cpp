#include "p_tcs.h"
//#include "p_mad.h"


P_Tcs::P_Tcs()
{
	strcpy(m_acName,"P_Tcs");
	
	//m_iType=PROPERTY_TYPE_NORMAL;
	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;	
		
	bNetwork  = false;

	m_pkTcs	= 		static_cast<Tcs*>(g_ZFObjSys.GetObjectPtr("Tcs"));
	m_pkRender = 	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
	
	m_bHavePolygonData=	false;
	m_bPolygonTest=		false;
	m_fRadius=				0.5;	
	m_bGravity=				false;
	m_bCharacter=			false;
	m_fLegLength=			1;
	m_fScale=				1;
	m_pkMad=					NULL;
	m_pkFaces =				NULL;
	m_pkVertex =			NULL;	
	m_pkNormal =			NULL;	
	m_iModelID=				0;
	m_fMass=					1;
	m_fAirFriction	=		1;
	m_fInertia =			1;
	m_bStatic=				false;
	m_iGroup=				0;
	m_bOnGround=			false;
	m_bActiveMoment =		true;
	m_fBounce =				1;
	m_fFriction = 			0.2;
	m_bSleeping = 			false;
	m_bCantSleep = 		false;
	m_bDisableOnSleep =	false;
	m_bRemoveOnSleep =	false;
	m_bNoColRespons =		false;
	
	ResetGroupFlags();
	ResetWalkGroupFlags();

   m_kRotVel.Set(0,0,0);
	m_kExternalLinearForce.Set(0,0,0);
	m_kExternalRotForce.Set(0,0,0);
	
	m_kLinearVelocity.Set(0,0,0);
	m_kRotVelocity.Set(0,0,0);
	m_kLinearForce.Set(0,0,0);
	m_kRotForce.Set(0,0,0);
	m_kNewPos.Set(0,0,0);
	m_kNewRotation.Identity();
}

P_Tcs::~P_Tcs()
{
	Disable();
}


void P_Tcs::Init()
{


	//cout<<"Tiny Collisions system property Added"<<endl;	
	Enable();
}

void P_Tcs::Enable()
{
	m_pkTcs->AddBody(this);
}
void P_Tcs::Disable()
{
	m_pkTcs->RemoveBody(this);
}

void P_Tcs::Update()
{	
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) {
		if(m_bPolygonTest)
		{
			if(!m_bHavePolygonData)
			{
				m_bHavePolygonData = SetupMeshData();
				m_fRadius = GetBoundingRadius();
			}
		}
		else			//if no polygon test, check if we shuld at least get radius from mesh
		{
			if(m_fRadius == -1)
			{	
				m_fRadius = GetBoundingRadius();
				m_bHavePolygonData = SetupMeshData();
			}
		}
	}
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) 
	{
		if(m_pkTcs->GetDebugGraph() != 0)
			Draw();
	}
}



void P_Tcs::SetData(vector<Mad_Face> kFaces, vector<Vector3> kVertex, vector<Vector3> kNormals, float fRadius )
{
	m_bLocalStoredData	= true;
	m_bPolygonTest			= true;
	m_bHavePolygonData	= true;

	m_pkFaces	= new vector<Mad_Face>;
	*m_pkFaces	= kFaces;

	m_pkVertex	= new vector<Vector3>;
	*m_pkVertex = kVertex;

	m_pkNormal	= new vector<Vector3>;
	*m_pkNormal = kNormals;
	m_pkMad		= NULL;

	m_fRadius				= fRadius;
}

void P_Tcs::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write((void*)&m_bPolygonTest,sizeof(m_bPolygonTest),1);
	pkPackage->Write((void*)&m_fRadius,sizeof(m_fRadius),1);	
	pkPackage->Write((void*)&m_iGroup,sizeof(m_iGroup),1);	
	pkPackage->Write((void*)&m_akTestGroups,sizeof(m_akTestGroups),1);		
	pkPackage->Write((void*)&m_akWalkableGroups,sizeof(m_akWalkableGroups),1);			
	pkPackage->Write((void*)&m_iModelID,sizeof(m_iModelID),1);		
	pkPackage->Write((void*)&m_bGravity,sizeof(m_bGravity),1);			
	pkPackage->Write((void*)&m_bCharacter,sizeof(m_bCharacter),1);				
	pkPackage->Write((void*)&m_fLegLength,sizeof(m_fLegLength),1);					
	
	pkPackage->Write((void*)&m_fBounce,sizeof(m_fBounce),1);							
	pkPackage->Write((void*)&m_fFriction,sizeof(m_fFriction),1);							
	pkPackage->Write((void*)&m_fMass,sizeof(m_fMass),1);						
	pkPackage->Write((void*)&m_fInertia,sizeof(m_fInertia),1);						
	pkPackage->Write((void*)&m_fAirFriction,sizeof(m_fAirFriction),1);						
	pkPackage->Write((void*)&m_bStatic,sizeof(m_bStatic),1);					
	pkPackage->Write((void*)&m_bSleeping,sizeof(m_bSleeping),1);					
			
	pkPackage->Write((void*)&m_kRotVel,sizeof(m_kRotVel),1);								
	
	pkPackage->Write((void*)&m_kExternalLinearForce,sizeof(m_kExternalLinearForce),1);									
	pkPackage->Write((void*)&m_kExternalRotForce,sizeof(m_kExternalRotForce),1);		
	pkPackage->Write((void*)&m_bActiveMoment,sizeof(m_bActiveMoment),1);									
	
	pkPackage->Write((void*)&m_bCantSleep,sizeof(m_bCantSleep),1);									
	pkPackage->Write((void*)&m_bDisableOnSleep,sizeof(m_bDisableOnSleep),1);									
	pkPackage->Write((void*)&m_bRemoveOnSleep,sizeof(m_bRemoveOnSleep),1);										
	pkPackage->Write((void*)&m_bNoColRespons,sizeof(m_bNoColRespons),1);											

}


void P_Tcs::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read((void*)&m_bPolygonTest,sizeof(m_bPolygonTest),1);
	pkPackage->Read((void*)&m_fRadius,sizeof(m_fRadius),1);	
	pkPackage->Read((void*)&m_iGroup,sizeof(m_iGroup),1);	
	pkPackage->Read((void*)&m_akTestGroups,sizeof(m_akTestGroups),1);		
	pkPackage->Read((void*)&m_akWalkableGroups,sizeof(m_akWalkableGroups),1);			
	pkPackage->Read((void*)&m_iModelID,sizeof(m_iModelID),1);		
	pkPackage->Read((void*)&m_bGravity,sizeof(m_bGravity),1);			
	pkPackage->Read((void*)&m_bCharacter,sizeof(m_bCharacter),1);				
	pkPackage->Read((void*)&m_fLegLength,sizeof(m_fLegLength),1);					

	pkPackage->Read((void*)&m_fBounce,sizeof(m_fBounce),1);		
	pkPackage->Read((void*)&m_fFriction,sizeof(m_fFriction),1);											
	pkPackage->Read((void*)&m_fMass,sizeof(m_fMass),1);						
	pkPackage->Read((void*)&m_fInertia,sizeof(m_fInertia),1);						
	pkPackage->Read((void*)&m_fAirFriction,sizeof(m_fAirFriction),1);						
	pkPackage->Read((void*)&m_bStatic,sizeof(m_bStatic),1);		
	pkPackage->Read((void*)&m_bSleeping,sizeof(m_bSleeping),1);					
					
	pkPackage->Read((void*)&m_kRotVel,sizeof(m_kRotVel),1);								
	
	pkPackage->Read((void*)&m_kExternalLinearForce,sizeof(m_kExternalLinearForce),1);									
	pkPackage->Read((void*)&m_kExternalRotForce,sizeof(m_kExternalRotForce),1);									
	pkPackage->Read((void*)&m_bActiveMoment,sizeof(m_bActiveMoment),1);									
	
	pkPackage->Read((void*)&m_bCantSleep,sizeof(m_bCantSleep),1);									
	pkPackage->Read((void*)&m_bDisableOnSleep,sizeof(m_bDisableOnSleep),1);									
	pkPackage->Read((void*)&m_bRemoveOnSleep,sizeof(m_bRemoveOnSleep),1);											
	pkPackage->Read((void*)&m_bNoColRespons,sizeof(m_bNoColRespons),1);											
}

vector<PropertyValues> P_Tcs::GetPropertyValues()
{
	vector<PropertyValues> kReturn(20);

	int dummy;

	kReturn[0].kValueName="polygontest";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bPolygonTest;

	kReturn[1].kValueName="groupflag";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&dummy;

	kReturn[2].kValueName="radius";
	kReturn[2].iValueType=VALUETYPE_FLOAT;
	kReturn[2].pkValue=(void*)&m_fRadius;

	kReturn[3].kValueName="modelid";
	kReturn[3].iValueType=VALUETYPE_INT;
	kReturn[3].pkValue=(void*)&m_iModelID;
	
	kReturn[4].kValueName="gravity";
	kReturn[4].iValueType=VALUETYPE_BOOL;
	kReturn[4].pkValue=(void*)&m_bGravity;
	
	kReturn[5].kValueName="character";
	kReturn[5].iValueType=VALUETYPE_BOOL;
	kReturn[5].pkValue=(void*)&m_bCharacter;
		
	kReturn[6].kValueName="group";
	kReturn[6].iValueType=VALUETYPE_INT;
	kReturn[6].pkValue=(void*)&m_iGroup;
	
	kReturn[7].kValueName="leglength";
	kReturn[7].iValueType=VALUETYPE_FLOAT;
	kReturn[7].pkValue=(void*)&m_fLegLength;
	
	kReturn[8].kValueName="walkablegroupflag";
	kReturn[8].iValueType=VALUETYPE_INT;
	kReturn[8].pkValue=(void*)&dummy;
	
	kReturn[9].kValueName="mass";
	kReturn[9].iValueType=VALUETYPE_FLOAT;
	kReturn[9].pkValue=(void*)&m_fMass;	
	
	kReturn[10].kValueName="static";
	kReturn[10].iValueType=VALUETYPE_BOOL;
	kReturn[10].pkValue=(void*)&m_bStatic;	

	kReturn[11].kValueName="airfriction";
	kReturn[11].iValueType=VALUETYPE_FLOAT;
	kReturn[11].pkValue=(void*)&m_fAirFriction;	

	kReturn[12].kValueName="inertia";
	kReturn[12].iValueType=VALUETYPE_FLOAT;
	kReturn[12].pkValue=(void*)&m_fInertia;	

	kReturn[13].kValueName="activemoment";
	kReturn[13].iValueType=VALUETYPE_BOOL;
	kReturn[13].pkValue=(void*)&m_bActiveMoment;	

	kReturn[14].kValueName="bounce";
	kReturn[14].iValueType=VALUETYPE_FLOAT;
	kReturn[14].pkValue=(void*)&m_fBounce;	

	kReturn[15].kValueName="friction";
	kReturn[15].iValueType=VALUETYPE_FLOAT;
	kReturn[15].pkValue=(void*)&m_fFriction;	

	kReturn[16].kValueName="cantsleep";
	kReturn[16].iValueType=VALUETYPE_BOOL;
	kReturn[16].pkValue=(void*)&m_bCantSleep;	
	
	kReturn[17].kValueName="disableonsleep";
	kReturn[17].iValueType=VALUETYPE_BOOL;
	kReturn[17].pkValue=(void*)&m_bDisableOnSleep;	

	kReturn[18].kValueName="removeonsleep";
	kReturn[18].iValueType=VALUETYPE_BOOL;
	kReturn[18].pkValue=(void*)&m_bRemoveOnSleep;	

	kReturn[19].kValueName="nocolrespons";
	kReturn[19].iValueType=VALUETYPE_BOOL;
	kReturn[19].pkValue=(void*)&m_bNoColRespons;	
								
	return kReturn;
}

bool P_Tcs::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "groupflag") == 0 ) 
	{
		int flag = atoi(kValue.c_str());
		//cout << "setting flag:" << flag << endl;

		SetTestGroupFlag(flag,true);
		
		return true;
	}
	
	if( strcmp(kValueName.c_str(), "walkablegroupflag") == 0 ) 
	{
		int flag = atoi(kValue.c_str());
		//cout << "setting walkable group flag:" << flag << endl;

		SetWalkGroupFlag(flag,true);
		
		return true;
	}


	return false;
}

float P_Tcs::GetBoundingRadius()
{
	P_Mad* mp = static_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));
	if(mp)
	{
		float frad = mp->GetRadius();
		//cout<<"got radius from mad " << frad << endl;
		return frad;	
	}
	
	return -1;
}

bool P_Tcs::SetupMeshData()
{
	//cout<<"setting up mesh:"<<endl;
	//look for mad property
	P_Mad* pkMP = static_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));	
	if(pkMP != NULL)
	{
		//cout<<"found mad property"<<endl;
		//look for core pointer in mad property
		//m_pkCore = pkMP->pkCore;	
		Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMP->kMadHandle.GetResourcePtr()); 

		if(pkCore != NULL)
		{
			//cout<<"found core"<<endl;
			//look for mech pointer in core
			Mad_CoreMesh* pkCoreMech = pkCore->GetMeshByID(m_iModelID);					
			if(pkCoreMech != NULL)		
			{
				//cout<<"found mech"<<endl;
				 
				m_pkMad = pkMP;
				m_pkFaces = pkCoreMech->GetLODMesh(0)->GetFacesPointer();
				m_pkVertex = (*pkCoreMech->GetLODMesh(0)->GetVertexFramePointer())[0].GetVertexPointer();
				m_pkNormal = (*pkCoreMech->GetLODMesh(0)->GetVertexFramePointer())[0].GetNormalPointer();
				
				m_fScale = pkMP->m_fScale;
				
				//printf("TCS: Found The Mesh (%d, %d, %d)\n",m_pkFaces->size(), m_pkVertex->size(), m_pkNormal->size() );
						
				
					
				
				
				
				return true;
			}
		}	
	}
	
	//cout<<"TCS: error mech NOT found"<<endl;
	return false;
}



bool P_Tcs::LineVSMesh(Vector3 &kPos,Vector3 &kDir)
{
	if(!m_bHavePolygonData)
		return false;

	float		fDist = 9999999;
	Vector3	kClosestColPos;
	m_iColFace = -1;
	
	Vector3 data[3];
	float d;

	for(unsigned int i=0;i<m_pkFaces->size();i++)
	{
		
		//get and transform vertexes		
		for(int j=0;j<3;j++)
			data[j] = (*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]];
		
		//setup a virtual point in space
		Vector3 Point2 = kPos+(kDir*1000);
		
		
		if(TestPolygon(data,kPos,Point2))
		{	
			d = m_kColPos.DistanceTo(kPos); 
			
			//if this point is closer than the last one, set it as closest
			if(d < fDist)
			{	
				m_iColFace = i;
				kClosestColPos=m_kColPos;
				fDist = d;
			}
		}
	}		

	//if we colided whit something, set colpos 
	if(m_iColFace != -1)
	{
		m_kColPos = kClosestColPos; 
		return true;
	}
	
	return false;
}

bool P_Tcs::TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2)
{		
	Plane P;
	


	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[0];		
	Vector3 Normal= V1.Cross(V2);
	
	 
	if(Normal.Length() == 0)
	{
		return false;
	}
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kVerts[0]);	
	P.m_kNormal = Normal;

	if(P.LineTest(kPos1 , kPos2 ,&m_kColPos)){
		if(TestSides(kVerts,&Normal,m_kColPos))
		{
			
			return true;
		}
	}
	
	
	return false;
}


bool P_Tcs::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos)
{
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
		if(!side[i].SphereInside(kPos,0.01)){
			inside=false;
		}
	
	}
	
	
	return inside;	
}

void P_Tcs::Draw()
{
	m_pkRender->Sphere(GetObject()->GetIWorldPosV(),m_fRadius,2,Vector3(1,0,1),false);

/*
	if(!m_pkVertex)
		return;

	Vector3 data[3];

	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_LIGHTING_BIT|GL_FOG_BIT);
	glDisable(GL_LIGHTING);	//dont want lighting on the skybox		
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	float fColor = 0.4;

	GenerateModelMatrix();

	for(unsigned int i=0; i<m_pkFaces->size(); i++) {
		for(int j=0;j<3;j++) {
			if(m_pkMad)
				data[j] = m_kModelMatrix.VectorTransform((*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]]);
			else
				data[j] = (*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]];
			}

		glColor3f(fColor,fColor,fColor);
		fColor+= 0.05;
		if(fColor >= 1.0)
			fColor = 0.4;

		glBegin(GL_TRIANGLES );	//GL_TRIANGLES GL_LINE_LOOP
		glVertex3fv( (float*) &data[0] );		
		glVertex3fv( (float*) &data[1] );		
		glVertex3fv( (float*) &data[2] );		
		glEnd();
		}
	glPopAttrib();
*/	
}

void P_Tcs::GenerateModelMatrix() 
{
//	m_kModelMatrix.Identity();
//	m_kModelMatrix.Scale(m_fScale,m_fScale,m_fScale);
//	m_kModelMatrix *= GetObject()->GetWorldOriM();


	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(m_fScale,m_fScale,m_fScale);
	
	Matrix4 kMat;
	kMat = m_pkObject->GetWorldRotM();
	m_kModelMatrix *= kMat;	
	m_kModelMatrix.Translate(m_pkObject->GetWorldPosV());		

}

Matrix4 P_Tcs::GetModelMatrix()
{
	Matrix4 kMat;
	kMat.Identity();
	kMat.Scale(m_fScale,m_fScale,m_fScale);	
	kMat *= m_kNewRotation; 
	kMat.Translate(m_kNewPos);
	
	return kMat;
}

void P_Tcs::ApplyForce(Vector3 kAttachPos,const Vector3& kForce,bool bLocal)
{
	Wakeup();

	//add motion force
	m_kExternalLinearForce += kForce;

	if(m_bActiveMoment)
	{
		//are we using local cordinats or not, if so rotate it
		if(bLocal)
			kAttachPos = GetObject()->GetLocalRotM().VectorTransform(kAttachPos);	
		else
			kAttachPos = kAttachPos - GetObject()->GetWorldPosV();
		
		//calculate and add momental force		
		m_kExternalRotForce += kForce.Cross(kAttachPos);
	}	
}

void P_Tcs::ClearExternalForces()
{
	m_kExternalLinearForce.Set(0,0,0);
	m_kExternalRotForce.Set(0,0,0);
}

void P_Tcs::ApplyImpulsForce(Vector3 kAttachPos,const Vector3& kForce,bool bLocal)
{
	if(m_bStatic)
		return;

	Wakeup();

	//add linear force
	m_kLinearVelocity +=  kForce / m_fMass;
	
	if(m_bActiveMoment)
	{	
		//are we using local cordinats or not, if so rotate it
		if(bLocal)
			kAttachPos = GetObject()->GetLocalRotM().VectorTransform(kAttachPos);	
		else
			kAttachPos = kAttachPos - GetObject()->GetWorldPosV();
				
			
		m_kRotVelocity += kForce.Cross(kAttachPos) * m_fInertia; 
		//m_kRotVelocity += kAttachPos.Cross(kForce) / m_fInertia;
	}
}


void P_Tcs::ApplyImpulsForce(const Vector3& kForce)
{
	if(m_bStatic)
		return;
	
	Wakeup();
	
	m_kLinearVelocity +=  kForce / m_fMass;
} 

Vector3 P_Tcs::GetVel(Vector3 kPos,bool bLocal)
{
	if(m_bActiveMoment)
	{
		//are we using local cordinats or not, if so rotate it
		if(bLocal)
			kPos = GetObject()->GetLocalRotM().VectorTransform(kPos);	
		else
			kPos = kPos - GetObject()->GetWorldPosV();
	
	
		return m_kLinearVelocity - m_kRotVelocity.Cross(kPos);
	}
	else
		return m_kLinearVelocity; 

}

void P_Tcs::Sleep()
{
	if(m_bStatic || m_bCantSleep)
		return;
/*		
	ClearExternalForces();
		
	m_kLinearVelocity.Set(0,0,0);
	m_kRotVelocity.Set(0,0,0);		
	m_kLinearForce.Set(0,0,0);
	m_kRotForce.Set(0,0,0);*/
	m_bSleeping = true;
	
		
	if(m_bRemoveOnSleep)
		GetObject()->m_pkEntityMan->Delete(GetObject());
}

void P_Tcs::AddRestingBody(P_Tcs* pkBody)
{
	if(!m_bSleeping)
		return;

				
	cout<<"adding resting body"<<endl;
		
	m_kSleepingFriends.push_back(pkBody);	
}


void P_Tcs::Wakeup(bool bWakeChilds)
{
	if( !m_bSleeping)
		return;
		
		
//	cout<<"wakup"<<endl;
		
	m_bSleeping = false;

	/*
	//wakeup all sleeping friends
	if(bWakeChilds)
	{
		for(int i = 0 ;i<m_kSleepingFriends.size();i++)
		{
			m_kSleepingFriends[i]->Wakeup();
		}
	}
		
	m_kSleepingFriends.clear();
	*/
}

Property* Create_P_Tcs()
{
	return new P_Tcs;
}



