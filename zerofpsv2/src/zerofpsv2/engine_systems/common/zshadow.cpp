#include "zshadow.h"


bool ZShadow::ShutDown()	{ return true; }
bool ZShadow::IsValid()		{ return true; }


ZShadow::ZShadow(): ZFSubSystem("ZShadow")
{
	Logf("zerofps","ZShadow system created");

	m_iNrOfShadows =		2;
 	m_fExtrudeDistance = 100;
	m_iCurrentShadows = 	0;
	m_iCurrentVerts = 	0;
	m_bHaveCheckedBits = false;
	m_bDisabled = 			false;
	m_iDebug = 				0;
	m_iShadowMode =		ezFail;
	m_fFrontCapOffset =	0.025;

	RegisterVariable("r_shadows",				&m_iNrOfShadows,CSYS_INT);
	RegisterVariable("r_shadowmode",			&m_iShadowMode,CSYS_INT);
	RegisterVariable("r_shadowdebug",		&m_iDebug,CSYS_INT);
}

bool ZShadow::StartUp()
{
	m_pkLight 		= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));
	m_pkRender		= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));
	m_pkEntityMan	= static_cast<EntityManager*>(GetSystem().GetObjectPtr("EntityManager"));


	//EnableShadowGroup(0);
	//EnableShadowGroup(1);
	EnableShadowGroup(2);

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

	//push attribs
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	//setup gl states for shadows rendering
	SetupGL();

	//get all render propertys
	vector<Property*>	kRenderPropertys;
	m_pkEntityMan->GetWorldObject()->GetAllPropertys(&kRenderPropertys,PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT);

	m_iCurrentShadows = 0;
	m_iCurrentVerts = 0;

	for(int i = 0;i<kRenderPropertys.size();i++)
	{
		if(strcmp(kRenderPropertys[i]->m_acName,"P_Mad") == 0)
		{
			P_Mad* pkMad = (P_Mad*)kRenderPropertys[i];

			//is this shadow group enabled
			if(!m_kShadowGroups[pkMad->GetShadowGroup()])
				continue;

			//get mesh and transform vertexs
			if(SetupMesh(pkMad))
			{
				//find witch lights to enable
				vector<LightSource*>	kLights;
				m_pkLight->GetClosestLights(&kLights,m_iNrOfShadows, m_pkMad->GetObject()->GetIWorldPosV(),false);

				for(int i = 0;i<kLights.size();i++)
				{
					if(kLights[i]->iType == POINT_LIGHT)
					{
						//this light is to faint
						if(kLights[i]->fIntensity < 0.5)
							continue;

						MakeStencilShadow(kLights[i]->kPos);
						m_iCurrentShadows++;
					}

					if(kLights[i]->iType == DIRECTIONAL_LIGHT)
					{
						MakeStencilShadow(kLights[i]->kRot * 10000);
						m_iCurrentShadows++;
					}
				}
			}
			else
			{
				cout<<"error setting up shadow mesh"<<endl;
			}
		}
	}

	//draw shadow
	if(m_iCurrentShadows != 0)
	{
		DrawShadow(0.3);
	}

	glPopAttrib();

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
			//pkCore->SetupBonePose();
			pkCore->PrepareMesh(pkCore->GetMeshByID(iShadowMesh));

			//setup mesh pointers
			m_pkMad =		pkMad;
			m_pkFaces =		pkMad->GetFacesPtr();
			m_pkVertex =	pkMad->GetVerticesPtr();
			m_iNrOfFaces = pkMad->GetNumFaces();
			m_iNrOfVerts = pkMad->GetNumVertices();


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


			return true;
		}
	}

	return false;
}


void ZShadow::FindSiluetEdges(Vector3 kSourcePos)
{
	Vector3 v[3];
	int iVerts = m_iNrOfFaces*3;
	m_iCurrentVerts += iVerts;

	vector<pair<int,int> >	kTowardsEdges;

	if(m_iShadowMode == ezFail)
	{
		m_kFrontCaping.clear();
		m_kBackCaping.clear();
	}

	for(int i = 0;i<iVerts; i+=3)
	{
		v[0] = m_kTransFormedVertexs[ m_pkFaces[i] ];
		v[1] = m_kTransFormedVertexs[ m_pkFaces[i+1] ];
		v[2] = m_kTransFormedVertexs[ m_pkFaces[i+2]];

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
				m_kFrontCaping.push_back(v[0] + ( v[0] - kSourcePos).Unit() * m_fFrontCapOffset);
				m_kFrontCaping.push_back(v[1] + ( v[1] - kSourcePos).Unit() * m_fFrontCapOffset);
				m_kFrontCaping.push_back(v[2] + ( v[2] - kSourcePos).Unit() * m_fFrontCapOffset);

				m_kBackCaping.push_back(v[2] + ( v[2] - kSourcePos).Unit() * m_fExtrudeDistance);
				m_kBackCaping.push_back(v[1] + ( v[1] - kSourcePos).Unit() * m_fExtrudeDistance);
				m_kBackCaping.push_back(v[0] + ( v[0] - kSourcePos).Unit() * m_fExtrudeDistance);

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
				for(vector<pair<int,int> >::iterator it=kTowardsEdges.begin();it!=kTowardsEdges.end();it++)
				{
//					if( ( ( (*it).first == p.first ) 	&& ( (*it).second == p.second ) ) ||
//						 ( ( (*it).second == p.first ) 	&& ( (*it).first == p.second  ) ) )
					if( ( ( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.second] ) ) ||

							( ( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.second]  ) ) )
					{
						kTowardsEdges.erase(it);
						it = kTowardsEdges.begin();
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
	Vector3 ev[3];
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

void ZShadow::DrawCapings()
{
	//draw front caping
	glVertexPointer(3,GL_FLOAT,0,&m_kFrontCaping[0].x);
	glDrawArrays(GL_TRIANGLES,0,m_kFrontCaping.size());

	//draw back caping
	glVertexPointer(3,GL_FLOAT,0,&m_kBackCaping[0].x);
	glDrawArrays(GL_TRIANGLES,0,m_kBackCaping.size());
}

void ZShadow::DrawExtrudedSiluet()
{
	glVertexPointer(3,GL_FLOAT,0,&m_kExtrudedSiluet[0].x);
	glDrawArrays(GL_QUADS,0,m_kExtrudedSiluet.size());


	//debug stuff
	if(m_iDebug != 0)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_TEXTURE_2D);
			glColor4f(1,0,0,0);
			glDisable(GL_LIGHTING);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

			glVertexPointer(3,GL_FLOAT,0,&m_kExtrudedSiluet[0].x);
			glDrawArrays(GL_QUADS,0,m_kExtrudedSiluet.size());
		glPopAttrib();
	}
}


void ZShadow::MakeStencilShadow(Vector3 kSourcePos)
{
	//first calculate the siluet
	FindSiluetEdges(kSourcePos);

	switch(m_iShadowMode)
	{
		case ezFail:
		{
			//carmac inverse (zfail)
			//back
			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
			DrawExtrudedSiluet();
			DrawCapings();

			//front
 			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
			DrawExtrudedSiluet();
			DrawCapings();
		break;
		}

		case ezPass:
		{
			//the other one ;)
			//draw front
			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			DrawExtrudedSiluet();

			//draw back
			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			DrawExtrudedSiluet();

		break;
		}
	}

}

void ZShadow::SetupStencilBuffer()
{
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	glGetIntegerv(GL_STENCIL_BITS, &m_iStencilBits);

	if(m_iStencilBits == 0)
	{
		cout<<"WARNING: No stencil buffer found, disabling shadows"<<endl;
		m_bDisabled = true;
	}

	m_bHaveCheckedBits = true;
}

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

	//client states
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
}

void ZShadow::DrawShadow(float fItensity)
{
	glPushAttrib(GL_ENABLE_BIT);

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
	glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
	//glStencilFunc(GL_GREATER, 1, 255);			//inverse1

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	//blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, fItensity);

	//disable lighting
	glDisable(GL_LIGHTING);


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


	glPopAttrib();

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