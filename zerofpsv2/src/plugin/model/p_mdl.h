/// \cond Model

#ifndef _P_MDL_H
#define _P_MDL_H

#include "../../zerofpsv2/engine/property.h"
#include "../../zerofpsv2/engine/zerofps.h"
#include "zssqpack.h"

using namespace std;

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

class P_Mdl : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSRender*			m_pkRender;
		TextureManager*	m_pkTextureManger;


		PAKFileFp			md1fp;
		mdlhead_s			head;				
		dtriangle_s*		tris;					// Triangles.
		mframe_s*			frames;				// Frames.
		dstvert_s*			skinvert;			// Texture coo.
		int					new_skinwidth;		// New skinsize after resampling to power of 2.
		int					new_skinheight;		
	
		vector<Image>		Skins;

		Vector3*				m_pkVertex;
		Vector3*				m_pkNormal;

		int					m_iRenderFrame;
		float					m_fNextFrame;
		float					m_fLastFrame;

		string				m_strFileName;

	public:
		P_Mdl();
		~P_Mdl();
		
		void Update();

		void Save(ZFIoInterface* pkPackage)									{}
		void Load(ZFIoInterface* pkPackage,int iVersion)				{}
		void PackTo(NetPacket* pkNetPacket, int iConnectionID )		{}
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID )	{}	

		void LoadModel(string strFileName);
		int GetQuakeModellVersion(PAKFileFp *mdlfp, const char* filename);
		bool HandleSetValue( const string& kValueName ,const string& kValue );

		Vector3 GetTextureCoo(int iTriangle, int iVertex);

		void PrintInfo();
		void TransformTexCoo(int t, int v, Mad_TextureCoo *st);
		void mdl_loadskin(FILE* fp, int w, int h,Image* img);
		int ReadSkins(void);
		void ReadTriangles(void);
		bool ReadFrames(void);
};

Property* Create_P_Mdl();
void Register_P_Mdl(ZSSZeroFps* pkZeroFps);

#endif

/// \endcond 

