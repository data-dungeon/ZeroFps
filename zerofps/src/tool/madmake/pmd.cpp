#include "pmd.h"
#include <memory>
#include <cstdio>
#include <iostream>

using namespace std;

pmd_c::pmd_c()
{
	head.num_of_triangles = 0;
	head.num_of_vertex = 0;
	head.version_num = VERSION_NUM;
	triangle = NULL;
	vertex_normals = NULL;
	surface_normals = NULL;
}

pmd_c::~pmd_c()
{
	if(triangle)		delete []	triangle;
	if(vertex_normals)	delete []	vertex_normals;
	if(surface_normals)	delete []	surface_normals;

	if(frames)
	{
		for(int i=0; i<head.num_of_frames; i++)
			delete [] frames[i].vertex;

		delete [] frames;
	}

}

void pmd_c::Save(char* filename)
{
	MakeVertexNormals();

	FILE* fp = fopen(filename, "wb");

	// Write file Head.
	fwrite(&head,sizeof(pmdfile_head_s),1,fp);
	
	// Write Textures
	fwrite((void *)textures,sizeof(pmd_texture_name),head.num_of_textures,fp);

	// Write Texture Coo
	fwrite((void *)texture_coo,sizeof(pmd_texcoo_s),head.num_of_vertexcoo,fp);

	// Write Vertex
	for(int i=0; i<head.num_of_frames;i++)
		fwrite(frames[i].vertex,sizeof(Vector3),head.num_of_vertex,fp);

	// Write Vertex Normals
	//fwrite(vertex_normals,sizeof(Vector3),head.num_of_vertex,fp);

	// Write surface normals
	//fwrite(surface_normals,sizeof(Vector3),head.num_of_triangles,fp);
	

	// Write Triangles
	fwrite(triangle,sizeof(pmd_triangle_s),head.num_of_triangles,fp);

	fclose(fp);
}

void pmd_c::Load(char* filename)
{
	FILE* fp = fopen(filename, "rb");

	// Write file Head.
	fread(&head,sizeof(pmdfile_head_s),1,fp);
	
	// Read Textures
	fread(textures,sizeof(pmd_texture_name),head.num_of_textures,fp);

	// Read Texture Coo
	texture_coo = new pmd_texcoo_s [head.num_of_vertexcoo];
	fread((void *)texture_coo,sizeof(pmd_texcoo_s),head.num_of_vertexcoo,fp);

	// Read Vertex
	frames = new pmd_vertexframe [head.num_of_frames];
	for(int i=0; i<head.num_of_frames;i++)
	{
		frames[i].vertex =  new Vector3 [head.num_of_vertex];
		fread(frames[i].vertex,sizeof(Vector3),head.num_of_vertex,fp);
	}

	// Read Vertex Normals
	//vertex_normals = new Vector3 [head.num_of_vertex];
	//fread(vertex_normals,sizeof(Vector3),head.num_of_vertex,fp);

	// Write surface normals
	//surface_normals = new Vector3 [head.num_of_triangles];
	//fread(surface_normals,sizeof(Vector3),head.num_of_triangles,fp);

	// Read Triangles
	triangle = new pmd_triangle_s [head.num_of_triangles];
	fread(triangle,sizeof(pmd_triangle_s),head.num_of_triangles,fp);

	fclose(fp);
}

/*
vector_c CreateANormal(vector_c *p1,vector_c *p2,vector_c *p3)
{
	vector_c v1,v2;
	v1 = *p1 - *p2;
	v2 = *p3 - *p2;
	v1.normalize();
	v2.normalize();

	vector_c c = v1.cross(v2); 
	c.normalize();
	return -c;
}
*/

/*
vector_c pmd_c::GetNormalForSurface(int index)
{
	vector_c normal;

	normal = CreateANormal (&vertex[ triangle[index].vertex_index[0] ],
		&vertex[ triangle[index].vertex_index[1] ],
		&vertex[ triangle[index].vertex_index[2] ]);

	return normal;
}*/


void pmd_c::MakeVertexNormals(void)
{
	surface_normals = new Vector3 [ head.num_of_triangles ];
	vertex_normals = new Vector3 [ head.num_of_vertex ];

	int i;

	for (i = 0; i < head.num_of_triangles; i++) {
		//surface_normals[i] = GetNormalForSurface(i);
		surface_normals[i].x = 0;
		surface_normals[i].y = 0;
		surface_normals[i].z = 0;
		}


	int v,s;

	memset(vertex_normals,0,sizeof(Vector3) * head.num_of_vertex);

	for(s=0; s<head.num_of_vertex; s++) {
		for(v=0; v < 3; v++) {
			//vertex_normals[ triangle[s].vertex_index[ v ] ] += surface_normals[s];
			vertex_normals[s].x = 0;
			vertex_normals[s].y = 0;
			vertex_normals[s].z = 0;
			}
		}

	for(i=0; i<head.num_of_vertex; i++) {
		//vertex_normals[i].normalize();
		}
}

void Pmd_Animation::Clear(void)
{
	strcpy(Name, "");	
	KeyFrame.clear();
}

void Pmd_Animation::operator=(const Pmd_Animation& kOther)
{
	strcpy(Name, kOther.Name);	
	KeyFrame = kOther.KeyFrame;
}

Pmd_Animation*	pmd_c::GetAnimation(char* ucaName)
{
	vector<Pmd_Animation>::iterator it;

	for(it = kAnimation.begin(); it != kAnimation.end(); it++)
	{
		if(strcmp(it->Name, ucaName) == 0)
			return it;
	}

	// Finns ingen animation med det namnet så skapa den och returnera den.
	Pmd_Animation kNewAnim;
	kNewAnim.Clear();
	strcpy(kNewAnim.Name, ucaName);
	kAnimation.push_back(kNewAnim);
	return &kAnimation.back();
}

void Pmd_KeyFrame::Clear(void)
{
	iVertexFrame	= 0;
	fFrameTime		= 0.0;
}

void Pmd_KeyFrame::operator=(const Pmd_KeyFrame& kOther)
{
	iVertexFrame = kOther.iVertexFrame;
	fFrameTime	= kOther.fFrameTime;
}

void pmd_c::PrintAnimations(void)
{
	vector<Pmd_Animation>::iterator itAnim;
	vector<Pmd_KeyFrame>::iterator itKeyF;

	for(itAnim = kAnimation.begin(); itAnim != kAnimation.end(); itAnim++)
	{
		cout << itAnim->Name << ": ";
		for(itKeyF = itAnim->KeyFrame.begin(); itKeyF != itAnim->KeyFrame.end(); itKeyF++)
		{
			cout << itKeyF->iVertexFrame << ", ";
		}
		cout << endl;
	}
	
}