#ifndef FILE__MDL_H
#define FILE__MDL_H

#include "pmd.h"
#include "qpack.h"

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

struct dstvert_s
{
	int onseam;						// 0 or 0x20 (MDL_SKIN_ONSEAM)
	int s;     						// position, horizontally in range [0,skinwidth[
	int t;							// position, vertically in range [0,skinheight[
};

struct dtriangle_s
{
	int facesfront;					// 0 if part of backskin.
	int vertices[3];				// Index of 3 triangle vertices	in range [0,numverts[
};

struct dtrivertx_s
{
	unsigned char packedposition[3];    // X,Y,Z coordinate, packed on 0-255
	unsigned char lightnormalindex;     // Index of the vertex normal
};

struct daliasframe_s
{
	dtrivertx_s bboxmin;				// Minimum values of X,Y,Z
	dtrivertx_s bboxmax;				// Maximum values of X,Y,Z
	char name[IDPOLY_MAX_FRAME_NAME];	// Name of frame
};

struct mframe_s
{
	daliasframe_s head;					// Frame Info.
	dtrivertx_s *vertex;				// Array of vertices
};

struct mdl_t 
{
	PAKFileFp*    mdlfp;
	mdlhead_s		head;				
	dtriangle_s*	tris;				// Triangles.
	mframe_s*		frames;				// Frames.
	dstvert_s*		skinvert;			// Texture coo.
    int				new_skinwidth;		// New skinsize after resampling to power of 2.
	int				new_skinheight;		
};

// Quake 2
#define MD2_MAX_TRIANGLES	4096	// Check all this.
#define MD2_MAX_VERTS		2048
#define MD2_MAX_FRAMES		512
#define MD2_MAX_SKINS		32
#define MD2_MAX_SKINNAME	64
#define MD2_ALIAS_VERSION	8
#define MD2_IDENT_TAG		0x32504449

struct md2_dstvert_t 
{
	short s;						
	short t;						
};

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

struct md2_dtriangle_t 
{
	short index_xyz[3];					// X,Y,Z coordinate, packed on 0-255
	short index_st[3];					// Index to st coo for vertex.
};

struct md2_dtrivertx_t 
{
	unsigned char v[3];					// X,Y,Z coordinate, packed on 0-255
	unsigned char lightnormalindex;		// index of the vertex normal
};

struct md2_daliasframe_t 
{
	float scale[3];						// Model scale factors.
	float translate[3];					// Model origin.
	char  name[IDPOLY_MAX_FRAME_NAME];	// Frame Name
};

struct q2frame_t
{
	md2_daliasframe_t aliasframe;
	md2_dtrivertx_t* vertex;
};

class ModellMD2
{
public:

	PAKFileFp    md2fp;
	q2mdlhead_s head;
	md2_dstvert_t *skinvert;
	md2_dtriangle_t* tris;
	q2frame_t* frames;

	void ReadMD2( char* filename );
	bool Export(pmd_c* pmd);

};

// Quake 3:

#endif