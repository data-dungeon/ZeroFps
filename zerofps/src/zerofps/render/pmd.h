#ifndef FILE__PMD_H
#define FILE__PMD_H

#define VERSION_NUM	2
#define MAX_NUM_OF_TEXTURES	256		// Maximum texture on a single object.

#include "../basic/basic.pkg"
#include "render_x.h"

/*
class Vector3
{
public:
	float x,y,z;
};*/

// Pmd File Header.
struct pmdfile_head_s
{
	int		version_num;				// Version num.
	int		num_of_textures;			// Num of textures used by mesh.
	int		num_of_vertex;				// Num of vertices in mesh.
	int		num_of_triangles;			// Num of triangles in mesh.
	int		num_of_frames;				// Num of vertex frames.
	int		num_of_drawstates;			// Num of drawstates.	
	int		num_of_vertexcoo;			// Num of texture coo.
};

struct pmd_texture_name
{
	char	texname[64];				// path/name of texture.
};

struct pmd_texcoo_s
{
	float	s,t;
};

struct pmd_drawstate
{
	int		TextureIndex;				// Texture used.
	int		FirstTriangle;				// First triangle to use texture.
	int		NumOfTriangles;				// Num of triangles that use texture.
};

struct pmd_triangle_s
{
	int				texture_num;		// texture for this triangle.
	int				vertex_index[3];	// Vertex index of triangle.
	int				texcoo_index[3];	// Texture coo for each vertex.
};

struct pmd_vertexframe
{
	Vector3*	vertex;					// Vertices.
};

class RENDER_API pmd_c
{
private:
	void MakeVertexNormals(void);
	Vector3 GetNormalForSurface(int index);

public:
	pmdfile_head_s		head;
	pmd_texture_name	textures[MAX_NUM_OF_TEXTURES];
	pmd_texcoo_s*		texture_coo;
	pmd_vertexframe*	frames;
	pmd_drawstate*		draw_states;
	pmd_triangle_s*		triangle;
	
	Vector3*			vertex_normals;
	Vector3*			surface_normals;


	pmd_c();
	~pmd_c();

	void Save(char* filename);
	void Load(char* filename);
};

#endif






