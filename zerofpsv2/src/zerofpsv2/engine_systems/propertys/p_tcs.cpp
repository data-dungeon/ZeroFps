#include "p_tcs.h"
#include "../script_interfaces/si_objectmanager.h" 
#include "../common/heightmap.h"

#include "p_hmrp2.h"

using namespace ObjectManagerLua;


P_Tcs::P_Tcs()
{
	strcpy(m_acName,"P_Tcs");
	
	//m_iType=PROPERTY_TYPE_NORMAL;
	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;	
		
	bNetwork		= false;
	m_iVersion	= 2;
	
	
	m_pkTcs	= 		static_cast<Tcs*>(g_ZFObjSys.GetObjectPtr("Tcs"));
	m_pkRender = 	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
	
	m_bHavePolygonData=	false;
	m_kBoxSize=				Vector3(1,1,1);
	m_iTestType=			E_SPHERE;
	m_fRadius=				0.5;	
	m_bGravity=				false;
	m_bCharacter=			false;
	m_fLegLength=			1;
	m_fScale=				1;
	m_pkMad=					NULL;
	m_pkHmap=				NULL;
	m_pkFaces =				NULL;
	m_pkVertex =			NULL;	
	m_pkNormal =			NULL;	
	m_iModelID=				0;
	m_fMass=					1;
	m_fAirFriction	=		1;
	m_fInertia =			1;
	m_bStatic=				false;
	m_bTempStatic=			false;
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
	
	m_kLastPos.Set(0,0,0);
	m_kAABBMax.Set(0,0,0);
	m_kAABBMin.Set(0,0,0);
	m_kAABBRotation.Zero();
	
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
	if( m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL) ) 
	{
		if( (m_iTestType == E_MESH) || (m_iTestType == E_HMAP))
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
				//m_bHavePolygonData = SetupMeshData();
			}
		}
	}
	else if( m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER) ) 
	{
		if(m_pkTcs->GetDebugGraph() != 0)
			Draw();
	}
}



void P_Tcs::SetData(vector<Mad_Face> kFaces, vector<Vector3> kVertex, vector<Vector3> kNormals, float fRadius )
{
	m_bLocalStoredData	= true;
	//m_bPolygonTest			= true;
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
	//pkPackage->Write((void*)&m_bPolygonTest,sizeof(m_bPolygonTest),1);
	pkPackage->Write((void*)&m_kBoxSize,sizeof(m_kBoxSize),1);
	pkPackage->Write((void*)&m_iTestType,sizeof(m_iTestType),1);
	
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
	switch(iVersion)
	{
		case 1:
		{
			bool bPTest;
		
			pkPackage->Read((void*)&bPTest,sizeof(bPTest),1);
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
		
			if(bPTest)
				m_iTestType = E_MESH;
			else
				m_iTestType = E_SPHERE;
							
			break;	
		}
		
		case 2:
		{
			pkPackage->Read((void*)&m_kBoxSize,sizeof(m_kBoxSize),1);
			pkPackage->Read((void*)&m_iTestType,sizeof(m_iTestType),1);
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
		
			break;
		}			
	}					
}

vector<PropertyValues> P_Tcs::GetPropertyValues()
{
	vector<PropertyValues> kReturn(21);

	int dummy;
	
	kReturn[0].kValueName="testtype";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&m_iTestType;

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

	kReturn[20].kValueName="boxsize";
	kReturn[20].iValueType=VALUETYPE_VECTOR3;
	kReturn[20].pkValue=(void*)&m_kBoxSize;	
	
									
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
	if( P_Mad* mp = static_cast<P_Mad*>(m_pkEntity->GetProperty("P_Mad")) )
	{
		return mp->GetRadius();	
	}
	
	P_HMRP2* pkHm = static_cast<P_HMRP2*>(m_pkEntity->GetProperty("P_HMRP2"));	
	if(pkHm)
	{
		return pkHm->GetRadius();
	}

	return -1;
}

bool P_Tcs::SetupMeshData()
{
	// cout << "Setting up TCS Mesh: ";

	P_Mad* pkMP = static_cast<P_Mad*>(m_pkEntity->GetProperty("P_Mad"));	
	if(pkMP != NULL)
	{
		// cout << "Found MAD" <<endl;
		//look for core pointer in mad property
		Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMP->kMadHandle.GetResourcePtr()); 

		if(pkCore != NULL)
		{
			//cout<<"found core"<<endl;
			//look for mech pointer in core
	
			Mad_CoreMesh* pkCoreMech = pkCore->GetMeshByID(pkMP->GetCollisionMeshID());					
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
	
	P_HMRP2* pkHm = static_cast<P_HMRP2*>(m_pkEntity->GetProperty("P_HMRP2"));	
	if(pkHm)
	{
		// cout << "found Hmap" << endl;
		// Setup Collision Data
		vector<Mad_Face>	kFace;
		vector<Vector3>	kVertex;
		vector<Vector3>	kNormal;
		pkHm->m_pkHeightMap->SetPosition(m_pkEntity->GetWorldPosV());
		pkHm->m_pkHeightMap->GetCollData(&kFace,&kVertex,&kNormal);

		printf("TCS: Found The Mesh (%d, %d, %d)\n",kFace.size(), kVertex.size(), kNormal.size() );

		// Transform all vertex from Local to World.
		for(unsigned int i=0; i<kVertex.size(); i++) 
			kVertex[i] += pkHm->m_pkHeightMap->m_kCornerOffset;	//m_pkEntity->GetWorldPosV();	// + pkHm->m_pkHeightMap->m_kCornerPos;

		// Set Data.
		//SetPolygonTest(true);	
		SetTestType(E_HMAP);
		SetStatic(true);			
		SetData(kFace,kVertex,kNormal, 1);	 
		SetHmap(pkHm->m_pkHeightMap);
		SetGroup(0);

		return true;
	}

	// cout<<"TCS: error mech NOT found"<<endl;
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
	// Draw TCS bound sphere.
//	m_pkRender->Sphere(GetEntity()->GetIWorldPosV(),m_fRadius,2,Vector3(1,0,1),false);
	
	if(m_bStatic)
		m_pkRender->DrawAABB(GetEntity()->GetIWorldPosV() + m_kAABBMin,GetEntity()->GetIWorldPosV() + m_kAABBMax,Vector3(1,1,1) );
	else if(m_bSleeping)
		m_pkRender->DrawAABB(GetEntity()->GetIWorldPosV() + m_kAABBMin,GetEntity()->GetIWorldPosV() + m_kAABBMax,Vector3(0,0,1) );
	else
		m_pkRender->DrawAABB(GetEntity()->GetIWorldPosV() + m_kAABBMin,GetEntity()->GetIWorldPosV() + m_kAABBMax,Vector3(0,1,0) );
				
	/*
	
	if(!m_pkVertex)
		return;

	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_LIGHTING_BIT|GL_FOG_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	GenerateModelMatrix();

	Vector3 data[3];
	float fColor = 0.4;

	for(unsigned int i=0; i<m_pkFaces->size(); i++)
	{
		for(int j=0;j<3;j++)
         data[j] = m_kModelMatrix.VectorTransform((*m_pkVertex)[ (*m_pkFaces)[i].iIndex[j]]);

		// Update Color
		glColor3f(fColor,fColor,fColor);
		fColor+= 0.05;
		if(fColor >= 1.0)
			fColor = 0.4;

		// Draw Triangle
		glEnable(GL_POLYGON_OFFSET_FILL);	glPolygonOffset(-1,-5);
		glBegin(GL_TRIANGLES );	
			glVertex3fv( (float*) &data[0] );		
			glVertex3fv( (float*) &data[1] );		
			glVertex3fv( (float*) &data[2] );		
		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);	glPolygonOffset(0,0);	

		// Calculate Center of Triangle
		Vector3 kCenter(0,0,0);
		for(int iIndex=0; iIndex<3; iIndex++)
			kCenter += data[ iIndex ];
		kCenter *= 0.333;

		// Draw Normal of triangle
		Vector3 kNormEnd = kCenter + (*m_pkNormal)[i];
		glColor3f(1,0,0);
		glBegin( GL_LINES  );	
			glVertex3fv( (float*) &kCenter );		
			glVertex3fv( (float*) &kNormEnd );		
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
	kMat = m_pkEntity->GetWorldRotM();
	m_kModelMatrix *= kMat;	
	m_kModelMatrix.Translate(m_pkEntity->GetWorldPosV());		

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
			kAttachPos = GetEntity()->GetLocalRotM().VectorTransform(kAttachPos);	
		else
			kAttachPos = kAttachPos - GetEntity()->GetWorldPosV();
		
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
	if(m_bStatic ||  m_bTempStatic)
		return;

	Wakeup();

	//add linear force
	m_kLinearVelocity +=  kForce / m_fMass;
	
	if(m_bActiveMoment)
	{	
		//are we using local cordinats or not, if so rotate it
		if(bLocal)
			kAttachPos = m_kNewRotation.VectorTransform(kAttachPos);	
		else
			kAttachPos = kAttachPos - m_kNewPos;
				
			
		m_kRotVelocity += kForce.Cross(kAttachPos) * m_fInertia; 
		//m_kRotVelocity += kAttachPos.Cross(kForce) / m_fInertia;
	}
}


void P_Tcs::ApplyImpulsForce(const Vector3& kForce)
{
	if(m_bStatic ||  m_bTempStatic)
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
			kPos = GetEntity()->GetLocalRotM().VectorTransform(kPos);	
		else
			kPos = kPos - GetEntity()->GetWorldPosV();
	
	
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
		GetEntity()->m_pkEntityManager->Delete(GetEntity());
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

/* ********************************** SCRIPT INTERFACE ****************************************/
/**	\brief Script functions for Tcs
	\ingroup si
*/
class SITcs { };


namespace SI_PTcs
{

/**	\fn ApplyImpuls( Entity, Table.xyz)
 	\relates SITcs
	\brief Adds a one time impulse to the entity.

	Impulse is given in the direction specified by xyz.
*/
int ApplyImpulsLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;
		vector<TABLE_DATA> vkData;

		g_pkScript->GetArgNumber(pkLua, 0, &dId);
		g_pkScript->GetArgTable(pkLua, 2, vkData);

		Vector3 kDir((float) (*(double*) vkData[0].pData),
						(float) (*(double*) vkData[1].pData),
						(float) (*(double*) vkData[2].pData));

		if(Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId))
		{
			if(P_Tcs* pkTcs = (P_Tcs*)pkEnt->GetProperty("P_Tcs"))
			{
				pkTcs->ApplyImpulsForce(kDir);
				return 1;
			}
		}
		return 0;
	}

   return 0;	

}

int SetObjectRotVelLua (lua_State* pkLua)
{
	int iNrArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNrArgs != 2)
		return 0;

	double dID;
	g_pkScript->GetArgNumber(pkLua, 0, &dID);		

	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dID);

	if(pkObject)
	{
		vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 2, vkData); // första argumetet startar på 1

      // Get physic-property
      P_Tcs* pkTcs = (P_Tcs*)pkObject->GetProperty("P_Tcs");

      if ( pkTcs )
      {
		   pkTcs->SetRotVel (  Vector3(
			   (float) (*(double*) vkData[0].pData),
			   (float) (*(double*) vkData[1].pData),
			   (float) (*(double*) vkData[2].pData)) );
      }
      else
         cout << "Warning! Tried to set RotVel on a object without P_Tcs!" << endl;

		g_pkScript->DeleteTable(vkData);
	}

	return 1;
}

int BounceLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
		return 0;
	
	double id;	
	g_pkScript->GetArgNumber(pkLua, 0, &id);		


	Entity* ent = g_pkObjMan->GetEntityByID((int)id);
	
	if(ent)
	{
		Vector3 vel = ent->GetVel();
		
		if(vel.y > 0)
			return 0;
		
		if(abs((int)vel.y) < 1)
		{
			ent->SetVel(Vector3(0,0,0));
			P_Tcs* ts = (P_Tcs*)ent->GetProperty("P_Tcs");			
			if(ts)
				ts->SetGravity(false);
			
			return 0;
		}
		
		vel.y = (float) abs((int)vel.y);
	
		vel*=0.9;			//dämpnings faktor
		
		ent->SetVel(vel);
	}
	
	return 0;
}

}


Property* Create_P_Tcs()
{
	return new P_Tcs;
}

void ENGINE_SYSTEMS_API Register_PTcs(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Tcs", Create_P_Tcs);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("ApplyImpuls",	SI_PTcs::ApplyImpulsLua);
	g_pkScript->ExposeFunction("SetRotVel",	SI_PTcs::SetObjectRotVelLua);
	g_pkScript->ExposeFunction("Bounce",		SI_PTcs::BounceLua);				
}