#include "zshadow.h"


//------- ShadowMesh

ShadowMesh::ShadowMesh(P_Mad* pkMad,LightSource* pkLightSource,int iShadowMode)
{
	m_bUsed = 			true;
	m_iShadowMode =	iShadowMode;

	m_pkMad = 			pkMad;
	m_kMadPos = 		pkMad->GetObject()->GetIWorldPosV();
	m_kMadRotation = 	pkMad->GetObject()->GetWorldRotM();
	m_fMadScale = 		pkMad->m_fScale;

	m_pkLightSource = pkLightSource;

	if(m_pkLightSource->iType == DIRECTIONAL_LIGHT)
		m_kLightPos = (m_pkLightSource->kRot * 10000);
	else
		m_kLightPos = pkLightSource->kPos;


	m_pkExtrudedSiluetVB = NULL;
	m_pkFrontCapingVB = NULL;
	m_pkBackCapingVB = NULL;
}

ShadowMesh::~ShadowMesh()
{
	if(m_pkExtrudedSiluetVB)
		delete m_pkExtrudedSiluetVB;

	if(m_pkFrontCapingVB)
		delete m_pkFrontCapingVB;
	
	if(m_pkBackCapingVB)
		delete m_pkBackCapingVB;

}

bool ShadowMesh::Equals(P_Mad* pkMad,LightSource* pkLightSource,int iShadowMode)
{
	//mad pointer
	if(pkMad != m_pkMad)
		return false;

	if(m_pkMad->GetCurrentAnimation() != -1 )
		return false;

	//light piinter
	if(pkLightSource != m_pkLightSource)
		return false;

	//mad position
	if(m_kMadPos != pkMad->GetObject()->GetIWorldPosV())
		return false;

	//light position
	if(m_pkLightSource->iType == DIRECTIONAL_LIGHT)
	{
		if(m_kLightPos != (m_pkLightSource->kRot * 10000))
			return false;
	}
	else
	{
		if(m_kLightPos != pkLightSource->kPos)
			return false;
	}

	//mad rotation
	if(m_kMadRotation != pkMad->GetObject()->GetWorldRotM() )
		return false;

	//mad scale
	if(m_fMadScale != pkMad->m_fScale)
		return false;

	//shadow mode
	if(m_iShadowMode != iShadowMode)
		return false;


	return true;
}

//------- ZShadow


bool ZShadow::IsValid()		{ return true; }


ZShadow::ZShadow(): ZFSubSystem("ZShadow")
{
	Logf("zerofps","ZShadow system created");

	m_iNrOfShadows =		1;
 	m_fExtrudeDistance = 100;
	m_iCurrentShadows = 	0;
	m_iCurrentVerts = 	0;
	m_bHaveCheckedBits = false;
	m_bDisabled = 			false;
	m_iDebug = 				0;
	m_iShadowMode =		ezFail;
	m_fFrontCapOffset =	0.05;
	m_fShadowIntensity = 0.3;
	
	m_bVBO=					false;
	
	RegisterVariable("r_shadowvbo",			&m_bVBO,CSYS_BOOL);
	RegisterVariable("r_shadows",				&m_iNrOfShadows,CSYS_INT);
	RegisterVariable("r_shadowmode",			&m_iShadowMode,CSYS_INT);
	RegisterVariable("r_shadowdebug",		&m_iDebug,CSYS_INT);
	RegisterVariable("r_shadowintensity",	&m_fShadowIntensity,CSYS_FLOAT);
}

bool ZShadow::StartUp()
{
	m_pkLight 			= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));
	m_pkZeroFps			= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));
	m_pkRender			= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));
	m_pkEntityMan		= static_cast<EntityManager*>(GetSystem().GetObjectPtr("EntityManager"));
	m_pkZShaderSystem	= static_cast<ZShaderSystem*>(GetSystem().GetObjectPtr("ZShaderSystem"));
	
	//EnableShadowGroup(0); 
	//EnableShadowGroup(1); 	
	EnableShadowGroup(2);
	
	
	//the material for drawing the shadow models	
	m_pkShadowModel = new ZMaterial();
	m_pkShadowModel->GetPass(0)->m_bStencilTest = 		true;
	m_pkShadowModel->GetPass(0)->m_iStencilFunc = 		STENCILFUNC_ALWAYS;
	m_pkShadowModel->GetPass(0)->m_iStencilFuncRef = 	0;
	m_pkShadowModel->GetPass(0)->m_iStencilFuncMask = 	255;
	m_pkShadowModel->GetPass(0)->m_bDepthTest = 			true;
	m_pkShadowModel->GetPass(0)->m_bColorMask = 			false;
	m_pkShadowModel->GetPass(0)->m_bDepthMask = 			false;
	m_pkShadowModel->GetPass(0)->m_iPolygonModeBack = 	FILL_POLYGON;
	m_pkShadowModel->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;
	m_pkShadowModel->GetPass(0)->m_bLighting = 			false;
	
	
	//the material for drawing the shadow models	
	m_pkShadowMap = new ZMaterial();
	m_pkShadowMap->GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;
	m_pkShadowMap->GetPass(0)->m_bDepthTest = 		false;
	m_pkShadowMap->GetPass(0)->m_bStencilTest =		true;
	m_pkShadowMap->GetPass(0)->m_iStencilFunc = 		STENCILFUNC_NOTEQUAL;
	m_pkShadowMap->GetPass(0)->m_iStencilFuncRef = 	0;
	m_pkShadowMap->GetPass(0)->m_iStencilFuncMask = 255;
	m_pkShadowMap->GetPass(0)->m_iStencilOpFail = 	STENCILOP_KEEP;
	m_pkShadowMap->GetPass(0)->m_iStencilOpZFail = 	STENCILOP_KEEP;
	m_pkShadowMap->GetPass(0)->m_iStencilOpZPass = 	STENCILOP_KEEP;
	m_pkShadowMap->GetPass(0)->m_bBlend = 				true;
	m_pkShadowMap->GetPass(0)->m_iBlendSrc = 			SRC_ALPHA_BLEND_SRC;
	m_pkShadowMap->GetPass(0)->m_iBlendDst = 			ONE_MINUS_SRC_ALPHA_BLEND_DST;
	m_pkShadowMap->GetPass(0)->m_kVertexColor.Set(0,0,0,0.3);
	m_pkShadowMap->GetPass(0)->m_bLighting = 			false;
	m_pkShadowMap->GetPass(0)->m_bColorMaterial =	true;
	
	//material for debug drawings
	m_pkDebug = new ZMaterial();
	m_pkDebug->GetPass(0)->m_iCullFace = 			CULL_FACE_NONE;
	m_pkDebug->GetPass(0)->m_iPolygonModeBack = 	LINE_POLYGON;
	m_pkDebug->GetPass(0)->m_iPolygonModeFront =	LINE_POLYGON;
	m_pkDebug->GetPass(0)->m_kVertexColor.Set(1,0,0,0);
	m_pkDebug->GetPass(0)->m_bColorMaterial =		true;
	m_pkDebug->GetPass(0)->m_bLighting = 			false;
	
	return true;
}

bool ZShadow::ShutDown()	
{
	delete m_pkDebug;
	delete m_pkShadowMap;
	delete m_pkShadowModel;

	return true;
}


void ZShadow::Update()
{
	//do a quick return if no shadows shuld be rendered
	if( (m_iNrOfShadows == 0) || m_bDisabled)
		return;

	//setup stencil buffert
	if(!m_bHaveCheckedBits)
		SetupStencilBuffer();

	//clear stencil buffert
	m_pkZShaderSystem->ClearBuffer(STENCIL_BUFFER);

	//set all shadow meshs ass unused
	SetUnusedMeshs();

	//get all render propertys
	vector<Property*>	kRenderPropertys;
	m_pkEntityMan->GetZoneObject()->GetAllPropertys(&kRenderPropertys,PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT);

	m_iCurrentShadows = 0;
	m_iCurrentVerts = 0;
	m_iCurrentActiveShadows = 0;

	for(int i = 0;i<kRenderPropertys.size();i++)
	{
		if(strcmp(kRenderPropertys[i]->m_acName,"P_Mad") == 0)
		{
			P_Mad* pkMad = (P_Mad*)kRenderPropertys[i];

			//is this shadow group enabled
			if(!m_kShadowGroups[pkMad->GetShadowGroup()])
				continue;

			//reset mesh
			m_bHaveMesh = false;

			//find witch lights to enable
			vector<LightSource*>	kLights;
			m_pkLight->GetClosestLights(&kLights,m_iNrOfShadows, pkMad->GetObject()->GetIWorldPosV(),false);
			//m_pkLight->GetClosestLights(&kLights,m_iNrOfShadows, pkMad->GetObject()->GetIWorldPosV(),true);
			
			for(int i = 0;i<kLights.size();i++)
			{
				if(kLights[i]->fIntensity < 0.5)
					continue;

				//clear
				//SetupGL();
					
				MakeStencilShadow(pkMad,kLights[i]);
				
				//DrawShadow(m_fShadowIntensity);
				//DrawGradedShadow(m_fShadowIntensity,pkMad->GetObject()->GetIWorldPosV(), (pkMad->GetObject()->GetIWorldPosV() - kLights[i]->kPos).Unit() );
				
				m_iCurrentShadows++;
			}
		}
	}

	//remove unused meshs
	ClearUnusedMeshs();

	//draw shadow
	if(m_iCurrentShadows != 0)
	{
		DrawShadow(m_fShadowIntensity);
	}	
}

bool ZShadow::SetupMesh(P_Mad* pkMad)
{
	int iShadowMesh = 0;
	m_kTransFormedVertexs.clear();

	if(Mad_Core* pkCore = (Mad_Core*)pkMad->kMadHandle.GetResourcePtr())
	{
		if(Mad_CoreMesh* pkCoreMech = pkCore->GetMeshByID(iShadowMesh))
		{
		 	//pkCore->SetBoneAnimationTime(iActiveAnimation, fCurrentTime, m_bLoop);
			pkCore->SetupBonePose();
			pkCore->PrepareMesh(pkCore->GetMeshByID(iShadowMesh));

			//setup mesh pointers			
			m_pkFaces =		pkMad->GetFacesPtr();
			m_pkVertex =	pkMad->GetVerticesPtr();
			m_iNrOfFaces = pkMad->GetNumFaces();
			m_iNrOfVerts = pkMad->GetNumVertices();
			
			
			/*
			m_pkFaces =	(int*)pkCoreMech->GetLODMesh(0)->GetFacesPointer();
			m_pkVertex = (Vector3*)(*pkCoreMech->GetLODMesh(0)->GetVertexFramePointer())[0].GetVertexPointer();
			m_iNrOfFaces = pkCoreMech->GetLODMesh(0)->GetFacesPointer()->size();;
			m_iNrOfVerts = (*pkCoreMech->GetLODMesh(0)->GetVertexFramePointer())[0].GetVertexPointer()->size();;
			*/

			//setup model matrix
			Matrix4 m_kModelMatrix;

			Matrix4 ori;
			ori = pkMad->GetObject()->GetWorldRotM();

			m_kModelMatrix.Identity();
			m_kModelMatrix.Scale(pkMad->m_fScale,pkMad->m_fScale,pkMad->m_fScale);
			m_kModelMatrix *= ori;
			m_kModelMatrix.Translate(pkMad->GetObject()->GetIWorldPosV());

			//transform vertexs
			for(int i = 0;i<m_iNrOfVerts;i++)
				m_kTransFormedVertexs.push_back( m_kModelMatrix.VectorTransform(m_pkVertex[i]) );

			m_bHaveMesh = true;

			return true;
		}
	}

	return false;
}




void ZShadow::DrawCapings(ShadowMesh* pkShadowMesh)
{
	if(m_bVBO)
	{
		if(!pkShadowMesh->m_pkFrontCapingVB)
		{
			m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&(pkShadowMesh->m_kFrontCaping[0].x));
			m_pkZShaderSystem->SetNrOfVertexs(pkShadowMesh->m_kFrontCaping.size());
			pkShadowMesh->m_pkFrontCapingVB = m_pkZShaderSystem->CreateVertexBuffer(TRIANGLES_MODE);
		
			m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&(pkShadowMesh->m_kBackCaping[0].x));
			m_pkZShaderSystem->SetNrOfVertexs(pkShadowMesh->m_kBackCaping.size());
			pkShadowMesh->m_pkBackCapingVB = m_pkZShaderSystem->CreateVertexBuffer(TRIANGLES_MODE);		
		}		
		m_pkZShaderSystem->DrawVertexBuffer(pkShadowMesh->m_pkFrontCapingVB);
		m_pkZShaderSystem->DrawVertexBuffer(pkShadowMesh->m_pkBackCapingVB);
	}
	else
	{
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&(pkShadowMesh->m_kFrontCaping[0].x));
		m_pkZShaderSystem->SetNrOfVertexs(pkShadowMesh->m_kFrontCaping.size());
		m_pkZShaderSystem->DrawArray(TRIANGLES_MODE);
	
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&(pkShadowMesh->m_kBackCaping[0].x));
		m_pkZShaderSystem->SetNrOfVertexs(pkShadowMesh->m_kBackCaping.size());
		m_pkZShaderSystem->DrawArray(TRIANGLES_MODE);
	}
}

void ZShadow::DrawExtrudedSiluet(ShadowMesh* pkShadowMesh)
{
	if(m_bVBO)
	{
		if(!pkShadowMesh->m_pkExtrudedSiluetVB)
		{
			m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&pkShadowMesh->m_kExtrudedSiluet[0].x);
			m_pkZShaderSystem->SetNrOfVertexs(pkShadowMesh->m_kExtrudedSiluet.size());		
			pkShadowMesh->m_pkExtrudedSiluetVB = m_pkZShaderSystem->CreateVertexBuffer(QUADS_MODE);
		}				
		m_pkZShaderSystem->DrawVertexBuffer(pkShadowMesh->m_pkExtrudedSiluetVB);
	}
	else
	{	

		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&pkShadowMesh->m_kExtrudedSiluet[0].x);
		m_pkZShaderSystem->SetNrOfVertexs(pkShadowMesh->m_kExtrudedSiluet.size());
		m_pkZShaderSystem->DrawArray(QUADS_MODE);
	}	
}


void ZShadow::MakeStencilShadow(P_Mad* pkMad,LightSource* pkLightSource)
{
	//first fint the mesh or generate it if its not found
	ShadowMesh* pkShadowMesh = GetShadowMesh(pkMad,pkLightSource);

	if(!pkShadowMesh)
	{
		cout<<"Shadow mesh not found nor generated..this is bad. returning"<<endl;
		return;
	}

	//bind material
	if(m_iDebug == 0)
		m_pkZShaderSystem->BindMaterial(m_pkShadowModel);			
	else
		m_pkZShaderSystem->BindMaterial(m_pkDebug);			
	
	//reset all pointers
	m_pkZShaderSystem->ResetPointers();
	
	switch(m_iShadowMode)
	{
		case ezFail:
		{
			//carmac inverse (zfail)
			//back			
			m_pkShadowModel->GetPass(0)->m_iCullFace = 			CULL_FACE_FRONT;
			m_pkShadowModel->GetPass(0)->m_iStencilOpFail = 	STENCILOP_KEEP;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZFail = 	STENCILOP_INCR;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZPass = 	STENCILOP_KEEP;	
			m_pkZShaderSystem->ReloadMaterial();
			DrawExtrudedSiluet(pkShadowMesh);
			DrawCapings(pkShadowMesh);

			//front
			m_pkShadowModel->GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;
			m_pkShadowModel->GetPass(0)->m_iStencilOpFail = 	STENCILOP_KEEP;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZFail = 	STENCILOP_DECR;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZPass = 	STENCILOP_KEEP;	
			m_pkZShaderSystem->ReloadMaterial();			
			DrawExtrudedSiluet(pkShadowMesh);
			DrawCapings(pkShadowMesh);

		break;
		}

		case ezPass:
		{
			//the other one ;)
			//draw front
			m_pkShadowModel->GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;
			m_pkShadowModel->GetPass(0)->m_iStencilOpFail = 	STENCILOP_KEEP;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZFail = 	STENCILOP_KEEP;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZPass = 	STENCILOP_INCR;	
			m_pkZShaderSystem->ReloadMaterial();			
			DrawExtrudedSiluet(pkShadowMesh);

			//draw back
			m_pkShadowModel->GetPass(0)->m_iCullFace = 			CULL_FACE_FRONT;
			m_pkShadowModel->GetPass(0)->m_iStencilOpFail = 	STENCILOP_KEEP;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZFail = 	STENCILOP_KEEP;
			m_pkShadowModel->GetPass(0)->m_iStencilOpZPass = 	STENCILOP_DECR;	
			m_pkZShaderSystem->ReloadMaterial();			
			DrawExtrudedSiluet(pkShadowMesh);

		break;
		}
	}
}

void ZShadow::SetupStencilBuffer()
{
	m_iStencilBits = m_pkZShaderSystem->GetStencilBits();

	if(m_iStencilBits == 0)
	{
		cout<<"WARNING: No stencil buffer found, disabling shadows"<<endl;
		m_bDisabled = true;
	}

	m_bHaveCheckedBits = true;
}



void ZShadow::DrawShadow(float fItensity)
{
	static float fQuad[] = {0,0,0,0,1,0,1,1,0,1,0,0};

		
	m_pkShadowMap->GetPass(0)->m_kVertexColor.Set(0,0,0,fItensity);
	m_pkZShaderSystem->BindMaterial(m_pkShadowMap);
	
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,fQuad);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
	m_pkZShaderSystem->MatrixPush();
		m_pkZShaderSystem->MatrixIdentity();
		m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
		m_pkZShaderSystem->MatrixPush();
			m_pkZShaderSystem->MatrixIdentity();
			m_pkZShaderSystem->MatrixGenerateOrtho(0,1,1,0,0,1);
			
			m_pkZShaderSystem->DrawArray(QUADS_MODE);
			
		m_pkZShaderSystem->MatrixPop();
		m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPop();

}

void ZShadow::DrawGradedShadow(float fItensity,Vector3 kPos,Vector3 kDir)
{	
/*	glPushAttrib(GL_ENABLE_BIT);

	//colormas
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	//culling
	glCullFace(GL_BACK);

	//depth buffer settings
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_EQUAL);
	glDisable(GL_DEPTH_TEST);

	//stencil buffer settings
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0, 255);
	glStencilFunc(GL_ALWAYS, 0, 255);
	glStencilFunc(GL_NOTEQUAL, 0, 255);
	//glStencilFunc(GL_GREATER, 1, 255);			//inverse1

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	//blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//disable lighting
	glDisable(GL_LIGHTING);


	float fSize = 15;
	
	Vector3 kCamPos = m_pkZeroFps->GetCam()->GetPos();
	Vector3 kBah = (kDir.Cross( kCamPos - kPos).Unit())*fSize;
	
	kPos -= kDir * 3;
	
	Vector3 kV1 = kPos - kBah;	
	Vector3 kV2 = kPos + kBah;	
	
	Vector3 kV3 = kPos + kBah + (kDir *fSize);	
	Vector3 kV4 = kPos - kBah + (kDir *fSize);	

	
	
	//draw
	glBegin(GL_QUADS);
		glColor4f(0.0f, 0.0f, 0.0f, fItensity);		
		glVertex3fv(&kV1.x);
		glVertex3fv(&kV2.x);
		
		glColor4f(0.0f, 0.0f, 0.0f, 0);				
		glVertex3fv(&kV3.x);
		glVertex3fv(&kV4.x);
		
	glEnd();
	
	
/*	
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);

	glBegin(GL_QUADS);
				glVertex2i(0, 0);
				glVertex2i(0, 1);
				glVertex2i(1, 1);
				glVertex2i(1, 0);
	glEnd();
*
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	glPopAttrib();	
*/
}


ShadowMesh*	ZShadow::GetShadowMesh(P_Mad* pkMad,LightSource* pkLightSource)
{
	//try to find the shadow
	for(int i = 0;i<m_kShadowMeshs.size();i++)
	{
		if(m_kShadowMeshs[i]->Equals(pkMad,pkLightSource,m_iShadowMode))
		{
			m_kShadowMeshs[i]->m_bUsed = true;
			return m_kShadowMeshs[i];
		}
	}

	//no shadow mesh found,
	return AddShadowMesh(pkMad,pkLightSource);
}

ShadowMesh* ZShadow::AddShadowMesh(P_Mad* pkMad,LightSource* pkLightSource)
{
	//if no mesh has been generated , generate it
	if(!m_bHaveMesh)
		SetupMesh(pkMad);

	ShadowMesh* pkNewMesh = new ShadowMesh(pkMad,pkLightSource,m_iShadowMode);
	GenerateShadowMesh(pkNewMesh);

	m_kShadowMeshs.push_back(pkNewMesh);

	m_iCurrentActiveShadows++;

	//cout<<"Regenerated a shadow"<<endl;
	return pkNewMesh;
}


void ZShadow::SetUnusedMeshs()
{
	for(int i = 0;i<m_kShadowMeshs.size();i++)
	{
		m_kShadowMeshs[i]->m_bUsed = false;
	}
}

void ZShadow::ClearUnusedMeshs()
{
	for(vector<ShadowMesh*>::iterator it = m_kShadowMeshs.begin();it!=m_kShadowMeshs.end();it++)
	{
		if( !(*it)->m_bUsed)
		{
			//cout<<"removing old shadow"<<endl;

			delete (*it);
			m_kShadowMeshs.erase(it);

			if(m_kShadowMeshs.empty())
				return;

			it = m_kShadowMeshs.begin();
		}
	}
}


void ZShadow::GenerateShadowMesh(ShadowMesh* pkShadowMesh)
{

	Vector3 v[3];
	Vector3 ev[3];
	int iVerts = m_iNrOfFaces*3;
	m_iCurrentVerts += iVerts;

	vector<pair<int,int> >	kTowardsEdges;
	vector<pair<int,int> >::iterator it;

	if(pkShadowMesh->m_iShadowMode == ezFail)
	{
		m_kFrontCaping.clear();
		m_kBackCaping.clear();
	}

	for(int i = 0;i<iVerts; i+=3)
	{
		v[0] = m_kTransFormedVertexs[ m_pkFaces[i]   ];
		v[1] = m_kTransFormedVertexs[ m_pkFaces[i+1] ];
		v[2] = m_kTransFormedVertexs[ m_pkFaces[i+2] ];

		// tror inte .Unit() behövs då d's värde inte spelar så stor roll, bara om det är positivt eller negativt
		//Vector3 Normal = (v[1] - v[0]).Cross(v[2] - v[0]).Unit();
		//Vector3 RefV = ( kSourcePos - (v[0]) ).Unit();

		Vector3 Normal = (v[1] - v[0]).Cross(v[2] - v[0]);
		Vector3 RefV = ( pkShadowMesh->m_kLightPos - (v[0]) );

		//
		if(Normal.Dot(RefV) > 0)
		{
			//here we generate the front and back capings for the zfail shadows
			if(pkShadowMesh->m_iShadowMode == ezFail)
			{
				ev[0] = ( v[0] - pkShadowMesh->m_kLightPos).Unit();
				ev[1] = ( v[1] - pkShadowMesh->m_kLightPos).Unit();
				ev[2] = ( v[2] - pkShadowMesh->m_kLightPos).Unit();

				pkShadowMesh->m_kFrontCaping.push_back(v[0] + ev[0] * m_fFrontCapOffset);
				pkShadowMesh->m_kFrontCaping.push_back(v[1] + ev[1] * m_fFrontCapOffset);
				pkShadowMesh->m_kFrontCaping.push_back(v[2] + ev[2] * m_fFrontCapOffset);

				pkShadowMesh->m_kBackCaping.push_back(v[2] + ev[2] * m_fExtrudeDistance);
				pkShadowMesh->m_kBackCaping.push_back(v[1] + ev[1] * m_fExtrudeDistance);
				pkShadowMesh->m_kBackCaping.push_back(v[0] + ev[0] * m_fExtrudeDistance);
			
			}


			pair<int,int> p;
			for(int j = 0;j<3;j++)
			{
				switch(j)
				{
					case 0:
						p.first = m_pkFaces[i];
						p.second = m_pkFaces[i+1];
						break;
					case 1:
						p.first = m_pkFaces[i+1];
						p.second = m_pkFaces[i+2];
						break;
					case 2:
						p.first = m_pkFaces[i+2];
						p.second = m_pkFaces[i];
						break;

				}


				bool bFound = false;
				for(it=kTowardsEdges.begin();it!=kTowardsEdges.end();it++)
				{
//					if( ( ( (*it).first == p.first ) 	&& ( (*it).second == p.second ) ) ||
//						 ( ( (*it).second == p.first ) 	&& ( (*it).first == p.second  ) ) )

					if( ( ( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.second] ) ) ||

							( ( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.second]  ) ) )
			     {
						kTowardsEdges.erase(it);
						//it = kTowardsEdges.begin();
						bFound =true;

						break;
					}
				}

				if(!bFound)
				{
					kTowardsEdges.push_back(p);
				}
			}
		}
	}

	//now lets calculate the extruded siluet
	pkShadowMesh->m_kExtrudedSiluet.clear();

	for(int i =0 ;i<kTowardsEdges.size();i++)
	{
		v[0] = m_kTransFormedVertexs[kTowardsEdges[i].first];
		v[1] = m_kTransFormedVertexs[kTowardsEdges[i].second];

		v[0] = v[0] + ( v[0] -pkShadowMesh->m_kLightPos).Unit() * m_fFrontCapOffset;
		v[1] = v[1] + ( v[1] -pkShadowMesh->m_kLightPos).Unit() * m_fFrontCapOffset;

		ev[0] = v[0] + ( v[0] - pkShadowMesh->m_kLightPos).Unit() * m_fExtrudeDistance;
		ev[1] = v[1] + ( v[1] - pkShadowMesh->m_kLightPos).Unit() * m_fExtrudeDistance;

		pkShadowMesh->m_kExtrudedSiluet.push_back(v[0]);
		pkShadowMesh->m_kExtrudedSiluet.push_back(ev[0]);
		pkShadowMesh->m_kExtrudedSiluet.push_back(ev[1]);
		pkShadowMesh->m_kExtrudedSiluet.push_back(v[1]);

	}
}




/*
void ZShadow::FindCapings(Vector3 kSourcePos)
{
	int iVerts = m_iNrOfFaces*3;
	m_iCurrentVerts += iVerts;
	Vector3 v[3];
	Vector3 ev[3];
	Vector3 bv[3];

	for(int i = 0;i<iVerts; i+=3)
	{
		v[0] = m_kTransFormedVertexs[ m_pkFaces[i] ];
		v[1] = m_kTransFormedVertexs[ m_pkFaces[i+1] ];
		v[2] = m_kTransFormedVertexs[ m_pkFaces[i+2]];


		Vector3 Normal = (v[1] - v[0]).Cross(v[2] - v[0]);
		Vector3 RefV = ( kSourcePos - (v[0]) );

		if(Normal.Dot(RefV) > 0)
		{


/*			ev[0] = v[0] + ( kSourcePos - v[0]).Unit() * 0.01;
			ev[1] = v[1] + ( kSourcePos - v[1]).Unit() * 0.01;
			ev[2] = v[2] + ( kSourcePos - v[2]).Unit() * 0.01;
*
			ev[0] = v[0] + ( v[0] - kSourcePos).Unit() * 0.01;
			ev[1] = v[1] + ( v[1] - kSourcePos).Unit() * 0.01;
			ev[2] = v[2] + ( v[2] - kSourcePos).Unit() * 0.01;


			bv[0] = v[0] + ( v[0] - kSourcePos).Unit() * m_fExtrudeDistance;
			bv[1] = v[1] + ( v[1] - kSourcePos).Unit() * m_fExtrudeDistance;
			bv[2] = v[2] + ( v[2] - kSourcePos).Unit() * m_fExtrudeDistance;


			glBegin(GL_TRIANGLES);
				glVertex3f(ev[0].x,ev[0].y,ev[0].z);
				glVertex3f(ev[1].x,ev[1].y,ev[1].z);
				glVertex3f(ev[2].x,ev[2].y,ev[2].z);
			glEnd();

/*			glBegin(GL_TRIANGLES);
				glVertex3f(v[0].x,v[0].y,v[0].z);
				glVertex3f(v[1].x,v[1].y,v[1].z);
				glVertex3f(v[2].x,v[2].y,v[2].z);
			glEnd();
*
			glBegin(GL_TRIANGLES);
				glVertex3f(bv[2].x,bv[2].y,bv[2].z);
				glVertex3f(bv[1].x,bv[1].y,bv[1].z);
				glVertex3f(bv[0].x,bv[0].y,bv[0].z);
			glEnd();

		}
	}
}
*/

/*
void ZShadow::ExtrudeSiluet(Vector3 kSourcePos)
{
	Vector3 v[2];
	Vector3 ev[2];

	for(int i =0 ;i<m_kTowardsEdges.size();i++)
	{
		v[0] = m_kTransFormedVertexs[m_kTowardsEdges[i].first];
		v[1] = m_kTransFormedVertexs[m_kTowardsEdges[i].second];

		v[0] = v[0] + ( v[0] -kSourcePos).Unit() * 0.01;
		v[1] = v[1] + ( v[1] -kSourcePos).Unit() * 0.01;

		ev[0] = v[0] + ( v[0] - kSourcePos).Unit() * m_fExtrudeDistance;
		ev[1] = v[1] + ( v[1] - kSourcePos).Unit() * m_fExtrudeDistance;

		glBegin(GL_QUADS);
			glVertex3f(v[0].x,v[0].y,v[0].z);
			glVertex3f(ev[0].x,ev[0].y,ev[0].z);
			glVertex3f(ev[1].x,ev[1].y,ev[1].z);
			glVertex3f(v[1].x,v[1].y,v[1].z);
		glEnd();

		//debug stuff
		if(m_iDebug != 0)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_TEXTURE_2D);
				glColor4f(1,0,0,0);
				glDisable(GL_LIGHTING);

				glBegin(GL_LINES);
					glVertex3f(v[0].x,v[0].y,v[0].z);
					glVertex3f(v[1].x,v[1].y,v[1].z);

					glVertex3f(v[0].x,v[0].y,v[0].z);
					glVertex3f(ev[0].x,ev[0].y,ev[0].z);

					glVertex3f(v[1].x,v[1].y,v[1].z);
					glVertex3f(ev[1].x,ev[1].y,ev[1].z);
				glEnd();
			glPopAttrib();
		}
	}
}
*/

/*
void ZShadow::FindSiluetEdges(Vector3 kSourcePos)
{
	Vector3 v[3];
	Vector3 ev[3];
	int iVerts = m_iNrOfFaces*3;
	m_iCurrentVerts += iVerts;

	vector<pair<int,int> >	kTowardsEdges;
	vector<pair<int,int> >::iterator it;

	if(m_iShadowMode == ezFail)
	{
		m_kFrontCaping.clear();
		m_kBackCaping.clear();
	}

	for(int i = 0;i<iVerts; i+=3)
	{
		v[0] = m_kTransFormedVertexs[ m_pkFaces[i]   ];
		v[1] = m_kTransFormedVertexs[ m_pkFaces[i+1] ];
		v[2] = m_kTransFormedVertexs[ m_pkFaces[i+2] ];

		// tror inte .Unit() behövs då d's värde inte spelar så stor roll, bara om det är positivt eller negativt
		//Vector3 Normal = (v[1] - v[0]).Cross(v[2] - v[0]).Unit();
		//Vector3 RefV = ( kSourcePos - (v[0]) ).Unit();

		Vector3 Normal = (v[1] - v[0]).Cross(v[2] - v[0]);
		Vector3 RefV = ( kSourcePos - (v[0]) );

		//
		if(Normal.Dot(RefV) > 0)
		{
			//here we generate the front and back capings for the zfail shadows
			if(m_iShadowMode == ezFail)
			{
				ev[0] = ( v[0] - kSourcePos).Unit();
				ev[1] = ( v[1] - kSourcePos).Unit();
				ev[2] = ( v[2] - kSourcePos).Unit();

				m_kFrontCaping.push_back(v[0] + ev[0] * m_fFrontCapOffset);
				m_kFrontCaping.push_back(v[1] + ev[1] * m_fFrontCapOffset);
				m_kFrontCaping.push_back(v[2] + ev[2] * m_fFrontCapOffset);

				m_kBackCaping.push_back(v[2] + ev[2] * m_fExtrudeDistance);
				m_kBackCaping.push_back(v[1] + ev[1] * m_fExtrudeDistance);
				m_kBackCaping.push_back(v[0] + ev[0] * m_fExtrudeDistance);
			}


			pair<int,int> p;
			for(int j = 0;j<3;j++)
			{
				switch(j)
				{
					case 0:
						p.first = m_pkFaces[i];
						p.second = m_pkFaces[i+1];
						break;
					case 1:
						p.first = m_pkFaces[i+1];
						p.second = m_pkFaces[i+2];
						break;
					case 2:
						p.first = m_pkFaces[i+2];
						p.second = m_pkFaces[i];
						break;

				}


				bool bFound = false;
				for(it=kTowardsEdges.begin();it!=kTowardsEdges.end();it++)
				{
//					if( ( ( (*it).first == p.first ) 	&& ( (*it).second == p.second ) ) ||
//						 ( ( (*it).second == p.first ) 	&& ( (*it).first == p.second  ) ) )

					if( ( ( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.second] ) ) ||

							( ( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.second]  ) ) )
			     {
						kTowardsEdges.erase(it);
						//it = kTowardsEdges.begin();
						bFound =true;

						break;
					}
				}

				if(!bFound)
				{
					kTowardsEdges.push_back(p);
				}
			}
		}
	}

	//now lets calculate the extruded siluet
	m_kExtrudedSiluet.clear();

	for(int i =0 ;i<kTowardsEdges.size();i++)
	{
		v[0] = m_kTransFormedVertexs[kTowardsEdges[i].first];
		v[1] = m_kTransFormedVertexs[kTowardsEdges[i].second];

		v[0] = v[0] + ( v[0] -kSourcePos).Unit() * m_fFrontCapOffset;
		v[1] = v[1] + ( v[1] -kSourcePos).Unit() * m_fFrontCapOffset;

		ev[0] = v[0] + ( v[0] - kSourcePos).Unit() * m_fExtrudeDistance;
		ev[1] = v[1] + ( v[1] - kSourcePos).Unit() * m_fExtrudeDistance;

		m_kExtrudedSiluet.push_back(v[0]);
		m_kExtrudedSiluet.push_back(ev[0]);
		m_kExtrudedSiluet.push_back(ev[1]);
		m_kExtrudedSiluet.push_back(v[1]);

	}
}

*/

/*
void ZShadow::SetupGL()
{
	//clear stencil buffert
	glClear(GL_STENCIL_BUFFER_BIT);

	//setop
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 255);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glDisable(GL_ALPHA_TEST);
	

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(0,0,0,0);

	//client states
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
}
*/

/*	glPushAttrib(GL_ENABLE_BIT);

	//colormas
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	//culling
	glCullFace(GL_BACK);

	//depth buffer settings
	glDepthMask(GL_TRUE);
	//glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_EQUAL);
	glDisable(GL_DEPTH_TEST);

	//stencil buffer settings
	glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_EQUAL, 0, 255);
	//glStencilFunc(GL_ALWAYS, 0, 255);
	glStencilFunc(GL_NOTEQUAL, 0, 255);
	//glStencilFunc(GL_GREATER, 1, 255);			//inverse1

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	//blending
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ZERO);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, fItensity);

	//disable lighting
	glDisable(GL_LIGHTING);

*/

	/*
	//draw
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);

	glBegin(GL_QUADS);
				glVertex2i(0, 0);
				glVertex2i(0, 1);
				glVertex2i(1, 1);
				glVertex2i(1, 0);
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
