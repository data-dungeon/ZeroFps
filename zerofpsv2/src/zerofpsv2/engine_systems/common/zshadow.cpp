#include "zshadow.h"


bool ZShadow::ShutDown()	{ return true; }
bool ZShadow::IsValid()		{ return true; }


ZShadow::ZShadow(): ZFSubSystem("ZShadow")
{
	Logf("zerofps","ZShadow system created");

	m_pkLight 		= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));
	m_pkRender		= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));
	m_pkEntityMan	= static_cast<EntityManager*>(GetSystem().GetObjectPtr("EntityManager"));
}

bool ZShadow::StartUp()
{
	m_iNrOfShadows = 1;
 	m_fExtrudeDistance = 1000;

	RegisterVariable("r_shadows",		&m_iNrOfShadows,CSYS_INT);
	RegisterVariable("r_shadowlength",		&m_fExtrudeDistance,CSYS_FLOAT);

	return true;
}

void ZShadow::Update()
{
	vector<Property*>	kRenderPropertys;
	m_pkEntityMan->GetWorldObject()->GetAllPropertys(&kRenderPropertys,PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT);

	glClear(GL_STENCIL_BUFFER_BIT);

	int	m_iRenderedShadows = 0;

	for(int i = 0;i<kRenderPropertys.size();i++)
	{
		if(strcmp(kRenderPropertys[i]->m_acName,"P_Mad") == 0)
		{
			P_Mad* pkMad = (P_Mad*)kRenderPropertys[i];

			//no shadows if group is -1
			if(pkMad->GetShadowGroup() == -1)
				continue;

			if(SetupMesh(pkMad))
			{
				vector<LightSource*>	kLights;
				m_pkLight->GetClosestLights(&kLights,m_iNrOfShadows,m_pkMad->GetObject()->GetIWorldPosV(),false);

				for(int i = 0;i<kLights.size();i++)
				{
					if(kLights[i]->iType == POINT_LIGHT)
					{
						MakeStencilShadow(kLights[i]->kPos);
						m_iRenderedShadows++;
					}

					if(kLights[i]->iType == DIRECTIONAL_LIGHT)
					{
						MakeStencilShadow(kLights[i]->kRot * 10000);
						m_iRenderedShadows++;
					}
				}
			}
			else
			{
				cout<<"error setting up shadow mesh"<<endl;
			}
		}
	}

	//finaly draw the shadow
	if(m_iRenderedShadows != 0)
		DrawShadow();
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
			pkCore->PrepareMesh(pkCore->GetMeshByID(0));

			m_pkMad = pkMad;
			//m_pkFaces = pkCoreMech->GetLODMesh(0)->GetFacesPointer();
			//m_pkVertex = pkCore->GetVerticesPtr();
			m_pkFaces = pkMad->GetFacesPtr();
			m_pkVertex = pkMad->GetVerticesPtr();
			m_iNrOfFaces = pkMad->GetNumFaces();
			m_iNrOfVerts = pkMad->GetNumVertices();

			//m_pkVertex = (*pkCoreMech->GetLODMesh(0)->GetVertexFramePointer())[0].GetVertexPointer();

			Matrix4 ori;
			ori = pkMad->GetObject()->GetWorldRotM();

			Matrix4 m_kModelMatrix;
			m_kModelMatrix.Identity();
			m_kModelMatrix.Scale(pkMad->m_fScale,pkMad->m_fScale,pkMad->m_fScale);
			m_kModelMatrix *= ori;
			m_kModelMatrix.Translate(pkMad->GetObject()->GetIWorldPosV());

			//transform vertexs
			for(int i = 0;i<m_iNrOfVerts;i++)
			{
				//m_kTransFormedVertexs.push_back( m_kModelMatrix.VectorTransform((*m_pkVertex)[i]) );
				m_kTransFormedVertexs.push_back( m_kModelMatrix.VectorTransform(m_pkVertex[i]) );
			}

			return true;
		}
	}

	return false;
}

void ZShadow::FindSiluetEdges(Vector3 kSourcePos)
{
	Vector3 v[3];

	int verts = m_iNrOfFaces*3;
	for(int i = 0;i<verts; i+=3)
	{
		v[0] = m_kTransFormedVertexs[ m_pkFaces[i] ];
		v[1] = m_kTransFormedVertexs[ m_pkFaces[i+1] ];
		v[2] = m_kTransFormedVertexs[ m_pkFaces[i+2]];

		Vector3 Normal = (v[1] - v[0]).Cross(v[2] - v[0]).Unit();
		Vector3 RefV = ( kSourcePos - (v[0]) ).Unit();

		float d = Normal.Dot(RefV);
		//cout<<"DOT:"<<<<endl;

		if(d > 0)
		{
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
				for(vector<pair<int,int> >::iterator it=m_kTowardsEdges.begin();it!=m_kTowardsEdges.end();it++)
				{
//					if( ( ( (*it).first == p.first ) 	&& ( (*it).second == p.second ) ) ||
//						 ( ( (*it).second == p.first ) 	&& ( (*it).first == p.second  ) ) )
					if( ( ( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.second] ) ) ||
							( ( m_kTransFormedVertexs[(*it).second] == m_kTransFormedVertexs[p.first] ) 	&&
							( m_kTransFormedVertexs[(*it).first] == m_kTransFormedVertexs[p.second]  ) ) )
					{
						m_kTowardsEdges.erase(it);
						it = m_kTowardsEdges.begin();
						bFound =true;

						//cout<<"removing"<<endl;
						break;
					}
				}

				if(!bFound)
				{
					m_kTowardsEdges.push_back(p);
				}
			}
		}
	}
}

void ZShadow::ExtrudeSiluet(Vector3 kSourcePos)
{
	Vector3 v[2];
	Vector3 ev[2];

	for(int i =0 ;i<m_kTowardsEdges.size();i++)
	{
		v[0] = m_kTransFormedVertexs[m_kTowardsEdges[i].first];
		v[1] = m_kTransFormedVertexs[m_kTowardsEdges[i].second];

		ev[0] = v[0] + ( v[0] - kSourcePos).Unit() * m_fExtrudeDistance;
		ev[1] = v[1] + ( v[1] - kSourcePos).Unit() * m_fExtrudeDistance;

		glBegin(GL_TRIANGLES);
			glVertex3f(v[0].x,v[0].y,v[0].z);
			glVertex3f(ev[0].x,ev[0].y,ev[0].z);
			glVertex3f(v[1].x,v[1].y,v[1].z);

			glVertex3f(v[1].x,v[1].y,v[1].z);
			glVertex3f(ev[0].x,ev[0].y,ev[0].z);
			glVertex3f(ev[1].x,ev[1].y,ev[1].z);
		glEnd();
	}
}

void ZShadow::MakeStencilShadow(Vector3 kSourcePos)
{
	//first calculate the siluet
	m_kTowardsEdges.clear();
	FindSiluetEdges(kSourcePos);

	//setop
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	glEnable(GL_CULL_FACE);

	//draw back
	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	ExtrudeSiluet(kSourcePos);

	//draw front
	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	ExtrudeSiluet(kSourcePos);

}

void ZShadow::DrawShadow()
{
	//draw shadow
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthFunc(GL_EQUAL);
	glStencilFunc(GL_EQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glCullFace(GL_BACK);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	//draw
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_QUADS);
				glVertex2i(0, 0);
				glVertex2i(0, 1);
				glVertex2i(1, 1);
				glVertex2i(1, 0);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
}
