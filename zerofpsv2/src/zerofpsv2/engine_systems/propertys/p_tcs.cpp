#include "p_tcs.h"
//#include "p_mad.h"


P_Tcs::P_Tcs()
{
	strcpy(m_acName,"P_Tcs");
	
	//m_iType=PROPERTY_TYPE_NORMAL;
	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;	
		
	bNetwork  = false;

	m_pkTcs	= static_cast<Tcs*>(g_ZFObjSys.GetObjectPtr("Tcs"));

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
	m_bStatic=				false;
	m_iGroup=				0;
	m_bOnGround=			false;
	
	ResetGroupFlags();
	ResetWalkGroupFlags();

   m_kRotVel.Set(0,0,0);
	m_kWalkVel.Set(0,0,0);
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
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) {
		//Draw();
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
	
	pkPackage->Write((void*)&m_fMass,sizeof(m_fMass),1);						
	pkPackage->Write((void*)&m_bStatic,sizeof(m_bStatic),1);						
	pkPackage->Write((void*)&m_kWalkVel,sizeof(m_kWalkVel),1);							
	pkPackage->Write((void*)&m_kRotVel,sizeof(m_kRotVel),1);								
}

void P_Tcs::Load(ZFIoInterface* pkPackage)
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

	pkPackage->Read((void*)&m_fMass,sizeof(m_fMass),1);						
	pkPackage->Read((void*)&m_bStatic,sizeof(m_bStatic),1);						
	pkPackage->Read((void*)&m_kWalkVel,sizeof(m_kWalkVel),1);							
	pkPackage->Read((void*)&m_kRotVel,sizeof(m_kRotVel),1);								
	

}

vector<PropertyValues> P_Tcs::GetPropertyValues()
{
	vector<PropertyValues> kReturn(11);

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
}

void P_Tcs::GenerateModelMatrix()
{
	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(m_fScale,m_fScale,m_fScale);
	Matrix4 kMat;
	kMat = m_pkObject->GetWorldRotM();
	m_kModelMatrix *= kMat;	
	m_kModelMatrix.Translate(m_pkObject->GetWorldPosV());		

}




Property* Create_P_Tcs()
{
	return new P_Tcs;
}



