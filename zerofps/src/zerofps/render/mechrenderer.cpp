#include "mechrenderer.h"
#include "GL/gl.h"
#include "GL/glut.h"

MechRenderer::MechRenderer(): ZFObject("MechRenderer")
{
	cout<<"Mesh renderer created"<<endl;


}

void MechRenderer::AddMesh(vector<MR_Vertex>* pkVerts,vector<MR_Polygon>* pkPols)
{
	cout<<"adding mesh"<<endl;

	cout<<"Vertexs : "<<pkVerts->size()<<endl;
	cout<<"Polygons: "<<pkPols->size()<<endl;

	MR_Level firstlevel;
		firstlevel.kPolygons =	*pkPols;
		firstlevel.kVertexs =	 *pkVerts;
	
		for(int i=0;i<firstlevel.kPolygons.size();i++)
			firstlevel.kPolygons[i].bEnabled = true;
	
	MR_Mesh mesh;
		mesh.kLevels.push_back(firstlevel);

	m_kMeshs.push_back(mesh);
	
	cout<<"total nr of meshs:"<<m_kMeshs.size()<<endl;
	
	CalcWeights(m_kMeshs.size()-1,0);

	GenerateNewLevel(m_kMeshs.size()-1,0.1);
	GenerateNewLevel(m_kMeshs.size()-1,0.2);	
	GenerateNewLevel(m_kMeshs.size()-1,0.3);
	GenerateNewLevel(m_kMeshs.size()-1,0.4);	
}

void MechRenderer::DrawMesh(int iMeshID,int iLevel,Vector3 kPos)
{
	if(iLevel >= m_kMeshs[iMeshID].kLevels.size())
	{	
		cout<<"Trying to render a lod level that does not exist"<<endl;
		return;
	}
	
	MR_Level* mp = &(m_kMeshs[iMeshID].kLevels[iLevel]);
	MR_Vertex* v1;
	MR_Vertex* v2;
	MR_Vertex* v3;
	
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor3f(1,1,1);

	glTranslatef(kPos.x,kPos.y,kPos.z);

	srand(1);

	glBegin(GL_TRIANGLES);	
	
//	int tris = 0;
	
		for(int i=0;i<mp->kPolygons.size();i++)
		{
			if(!mp->kPolygons[i].bEnabled)
				continue;
		
			//tris++;
		
			glColor3f(rand() %255 /255.0,rand()%255/255.0,rand()%255/255.0);
		
			v1 = &mp->kVertexs[mp->kPolygons[i].iVerts[0]];
			v2 = &mp->kVertexs[mp->kPolygons[i].iVerts[1]];
			v3 = &mp->kVertexs[mp->kPolygons[i].iVerts[2]];
		
			glVertex3f(v1->kPos.x,v1->kPos.y,v1->kPos.z);
			glVertex3f(v2->kPos.x,v2->kPos.y,v2->kPos.z);
			glVertex3f(v3->kPos.x,v3->kPos.y,v3->kPos.z);

		}
		
		
	glEnd();


	glPopMatrix();
	glPopAttrib();
	
	//cout<<"Triangles rendered:"<<tris<<endl;
}


void MechRenderer::CalcWeights(int iMeshID,int iLevel)
{
	MR_Level* mp = &(m_kMeshs[iMeshID].kLevels[iLevel]);
	MR_Vertex* v1;
	MR_Vertex* v2;
	MR_Vertex* v3;

	for(int i=0;i<mp->kVertexs.size();i++)
	{	
		mp->kVertexs[i].fWeight = 0;
		mp->kVertexs[i].iPolygons = 0;
		mp->kVertexs[i].fCVD = 99999999;
	}

	for(int i=0;i<mp->kPolygons.size();i++)
	{
		v1 = &mp->kVertexs[mp->kPolygons[i].iVerts[0]];
		v2 = &mp->kVertexs[mp->kPolygons[i].iVerts[1]];
		v3 = &mp->kVertexs[mp->kPolygons[i].iVerts[2]];

		float d1=(v1->kPos - v2->kPos).Length();
		float d2=(v1->kPos - v3->kPos).Length();
		float d3=(v2->kPos - v3->kPos).Length();

		float sum = d1+d2+d3;
		
		
		//check closest vertex
		if(d1 < v1->fCVD)
		{
			v1->fCVD = d1;
			v1->iFriend = mp->kPolygons[i].iVerts[1];
		}
		if(d2 < v1->fCVD)
		{
			v1->fCVD = d2;
			v1->iFriend = mp->kPolygons[i].iVerts[2];			
		}
		if(d1 < v2->fCVD)
		{
			v2->fCVD = d1;
			v2->iFriend = mp->kPolygons[i].iVerts[0];
		}
		if(d3 < v2->fCVD)
		{
			v2->fCVD = d3;
			v2->iFriend = mp->kPolygons[i].iVerts[2];			
		}
		if(d2 < v3->fCVD)
		{
			v3->fCVD = d2;
			v3->iFriend = mp->kPolygons[i].iVerts[0];			
		}
		if(d3 < v3->fCVD)
		{
			v3->fCVD = d3;
			v3->iFriend = mp->kPolygons[i].iVerts[1];			
		}
		
		v1->fWeight += sum;
		v2->fWeight += sum;
		v3->fWeight += sum;
		
		v1->iPolygons++;
		v2->iPolygons++;
		v3->iPolygons++;
		
	}

	for(int i=0;i<mp->kVertexs.size();i++)
	{
		mp->kVertexs[i].fInterest = (mp->kVertexs[i].fWeight / mp->kVertexs[i].iPolygons)*mp->kVertexs[i].fCVD;
		
		//cout<<"---Vertex: "<<i<<" -------------------------------------------"<<endl;
		//cout<<"Weight of V      : "<<mp->kVertexs[i].fWeight<<endl;
		//cout<<"Poly C of V      : "<<mp->kVertexs[i].iPolygons<<endl;		
		//cout<<"closest v dist   : "<<mp->kVertexs[i].fCVD<<endl;
		//cout<<"interest         : "<<mp->kVertexs[i].fInterest<<endl;
	}
}

void MechRenderer::GenerateNewLevel(int iMeshID,float fMinSize)
{
	cout<<"generating a new level for mesh "<<iMeshID<<" with "<<fMinSize<<"% less verts"<<endl;
	
	MR_Level newlevel;
	MR_Level* oldlevel = &m_kMeshs[iMeshID].kLevels[0];
	
	float diff = fMinSize * GetMeshSize(iMeshID,0);
	cout<<"diff is:"<<diff<<endl;
	
	vector<int> dellist;
	dellist.clear();
	
	newlevel.kPolygons = oldlevel->kPolygons;
	newlevel.kVertexs = oldlevel->kVertexs;
	
	
	for(int i=0;i<oldlevel->kVertexs.size();i++)
	{
		cout<<oldlevel->kVertexs[i].fCVD<<endl;
		if(oldlevel->kVertexs[i].iPolygons >2)
			if(oldlevel->kVertexs[i].fCVD < diff )
			{
				dellist.push_back(i);
				//dellist.push_back(oldlevel->kVertexs[i].iFriend);
			}
	}	
	
	vector<MR_Polygon*> delpollist;
	delpollist.clear();
	
	for(int i=0;i<oldlevel->kPolygons.size();i++)
	{
		bool Remove = false;
		
		for(int k=0;k<dellist.size();k++)
		{
			Vector3 temp;
			int iDel = dellist[k];
			int iDelFriend = newlevel.kVertexs[dellist[k]].iFriend;
			
			temp.Lerp(newlevel.kVertexs[iDel].kPos,newlevel.kVertexs[iDelFriend].kPos,0.5);
			newlevel.kVertexs[iDel].kPos = temp;
			newlevel.kVertexs[iDelFriend].kPos = temp;
		
		
			int iFound = 0;
			
			for(int j=0;j<3;j++)
				if(newlevel.kPolygons[i].iVerts[j] == iDel || newlevel.kPolygons[i].iVerts[j] == iDelFriend)
				{	
					iFound++;
				}
				
			if(iFound >= 2)
				Remove = true;
		}	
		
		if(Remove)
			newlevel.kPolygons[i].bEnabled = false;
		else
			newlevel.kPolygons[i].bEnabled = true;
		
	}

/*	int i=0;	
	int max = newlevel.kPolygons.size();
	
	bool end = false;
	for(vector<MR_Polygon>::iterator it = newlevel.kPolygons.begin(); it != newlevel.kPolygons.end(); it++) 
	{
		i++;
	
		for(int k=0;k<delpollist.size();k++)
		{
			if(&(*it) == delpollist[k])
			{	
				vector<MR_Polygon>::iterator it2 = it; 
				it++;
				
				cout<<"Removing polygon"<<endl;
				newlevel.kPolygons.erase(it2);
				
				delpollist[k] = NULL;
			}
		}
		
		if(i >= max)
			break;
	}
*/
	
	
	m_kMeshs[iMeshID].kLevels.push_back(newlevel);
	CalcWeights(iMeshID,m_kMeshs[iMeshID].kLevels.size()-1);
}

float MechRenderer::GetMeshSize(int iMeshID,int iLevel)
{
	MR_Level* ml = &(m_kMeshs[iMeshID].kLevels[iLevel]);
	float maxsize =0;
	
	for(int i=0;i<ml->kVertexs.size();i++)
	{
		for(int j=0;j<ml->kVertexs.size();j++)
		{
			float size = (ml->kVertexs[i].kPos - ml->kVertexs[j].kPos).Length();
			if( size > maxsize)
				maxsize = size; 
		
		}
	}
	
	cout<<"size is:"<<maxsize<<endl;
	return maxsize;
}

/*
void MechRenderer::RemoveVertex(int iMeshID,int iVert)
{
	MR_Mesh* mp = &m_kMeshs[iMeshID];


	for(int i=0;i<mp->kPolygons.size();i++)
	{
		for(int j=0;j<3;j++)
			if(mp->kPolygons[i].iVerts[j] == iVert)
				mp->kPolygons[i].bEnabled = false;
	
	}
	
	
}*/


