#ifndef FILE__MDL_H
#define FILE__MDL_H

#include "qpack.h"
#include "madexport.h"

//#include "../../zerofps/basic/basicmath.pkg"
#include "../../../zerofpsv2/engine_systems/mad/mad_core.h"

struct pmd_triangle_s
{
	int				texture_num;		// texture for this triangle.
	int				vertex_index[3];	// Vertex index of triangle.
	int				texcoo_index[3];	// Texture coo for each vertex.
};

/*
	d*_x is on-disk and in-memory. 
	m*_x is in-memory only.

	Quake 1 (MDL) and Quake 2 (MD2) Modells Loading.
*/

// Shared Defines & Structures
#define IDPOLY_MAX_FRAME_NAME	16

// Quake 1
#define MDL_ALIAS_VERSION	6
#define MDL_SKIN_ONSEAM		0x0020
#define MDL_IDENT_TAG		0x4F504449

#define MDL_MAX_TRIANGLES	2048
#define MDL_MAX_VERTS		2048
#define MDL_MAX_FRAMES		256
#define MDL_MAX_SKINS		100

/// Q1 Mdl Header.
struct mdlhead_s 
{
	Vector3		scale;				// Model scale factors.
	Vector3		scale_origin;		// Model origin.
	float		boundingradius;		// Model bounding radius.
	Vector3		eyeposition;		// Eye position (useless?)
	int			numskins;			// the number of skin textures
	int			skinwidth;			// Width of skin texture
	int			skinheight;			// Height of skin texture
	int			numverts;			// Number of vertices
	int			numtris;			// Number of triangles surfaces
	int			numframes;			// Number of frames
	int			synctype;			// 0= synchron, 1= random
	int			flags;				// 0 (see Alias models)
	float		size;				// average size of triangles
};

/// Q1 Mdl Vertex Texture Coo.
struct dstvert_s
{
	int onseam;						// 0 or 0x20 (MDL_SKIN_ONSEAM)
	int s;     						// position, horizontally in range [0,skinwidth[
	int t;							// position, vertically in range [0,skinheight[
};

/// Q1 Mdl Triangle.
struct dtriangle_s
{
	int facesfront;					// 0 if part of backskin.
	int vertices[3];				// Index of 3 triangle vertices	in range [0,numverts[
};

/// Q1 Mdl Vertex
struct dtrivertx_s
{
	unsigned char packedposition[3];    // X,Y,Z coordinate, packed on 0-255
	unsigned char lightnormalindex;     // Index of the vertex normal
};

/// Q1 Mdl Vertex Frame Info
struct daliasframe_s
{
	dtrivertx_s bboxmin;				// Minimum values of X,Y,Z
	dtrivertx_s bboxmax;				// Maximum values of X,Y,Z
	char name[IDPOLY_MAX_FRAME_NAME];	// Name of frame
};

/// Q1 Mdl Vertex Frame
struct mframe_s
{
	daliasframe_s head;						// Frame Info.
	dtrivertx_s *vertex;					// Array of vertices
};

/// Import of Quake Modells to Mad.
class ModellMD1 : public IMadImport
{
private:
	void PrintInfo();

	bool ReadFrames(void);
	void ReadTriangles(void);
	int  ReadSkins(void);
	void TransformTexCoo(int t, int v, Mad_TextureCoo *st);

public:
	ModellMD1() {};
	~ModellMD1() {};

	PAKFileFp			md1fp;
	mdlhead_s			head;				
	dtriangle_s*		tris;				// Triangles.
	mframe_s*			frames;				// Frames.
	dstvert_s*			skinvert;			// Texture coo.
    int					new_skinwidth;		// New skinsize after resampling to power of 2.
	int					new_skinheight;		
	
	vector<Image>		Skins;

	void Read( const char* filename );			// Read data in own format to this.
	bool Export(MadExporter* mad, const char* filename);			// Export this to mad.
};

// Quake 2
#define MD2_MAX_TRIANGLES	4096	// Check all this.
#define MD2_MAX_VERTS		2048
#define MD2_MAX_FRAMES		512
#define MD2_MAX_SKINS		32
#define MD2_MAX_SKINNAME	64
#define MD2_ALIAS_VERSION	8
#define MD2_IDENT_TAG		0x32504449

/// Q2 Md2 Vertex Texture Coo.
struct md2_dstvert_t 
{
	short s;						
	short t;						
};

/// Q2 Md2 Header.
struct q2mdlhead_s 
{
	 int skinwidth;					// Width of skin texture
	 int skinheight;				// Height of skin texture
	 int framesize;					// Size in bytes of one frame
	 int num_skins;					// Num of skins this md2 has.
	 int num_xyz;					// Number of vertices 
	 int num_st;					// Number of texture coo.
	 int num_tris;					// Number of triangles surfaces
	 int num_glcmds;				// Num of glcmds
	 int num_frames;				// Number of frames 
	 int ofs_skins;					// ofs to skin names
	 int ofs_st;					// ofs to st coo.
	 int ofs_tris;					// ofs to triangles
	 int ofs_frames;				// ofs to frames
	 int ofs_glcmds;				// ofs to glcmds
	 int ofs_end;					// ???
};

/// Q2 Md2 Triangle
struct md2_dtriangle_t 
{
	short index_xyz[3];					// X,Y,Z coordinate, packed on 0-255
	short index_st[3];					// Index to st coo for vertex.
};

/// Q2 Md2 Vertex
struct md2_dtrivertx_t 
{
	unsigned char v[3];					// X,Y,Z coordinate, packed on 0-255
	unsigned char lightnormalindex;		// index of the vertex normal
};

/// Q2 Md2 Vertex Frame Info
struct md2_daliasframe_t 
{
	float scale[3];						// Model scale factors.
	float translate[3];					// Model origin.
	char  name[IDPOLY_MAX_FRAME_NAME];	// Frame Name
};

/// Q2 Md2 Vertex Frame
struct q2frame_t
{
	md2_daliasframe_t aliasframe;
	md2_dtrivertx_t* vertex;
};

/// Import of Quake2 Modells to Mad.
class ModellMD2 : public IMadImport
{
public:
	ModellMD2() {};
	~ModellMD2() {};

	PAKFileFp			m_kFile;
	q2mdlhead_s			m_kHead;
	md2_dstvert_t*		m_pakSkinVert;
	md2_dtriangle_t*	m_pakTris;
	q2frame_t*			m_pakFrames;

	void Read( const char* filename );	// Read data in own format to this.
	bool Export(MadExporter* mad, const char* filename);	// Export this to mad.
};

// Quake 3:

#endif