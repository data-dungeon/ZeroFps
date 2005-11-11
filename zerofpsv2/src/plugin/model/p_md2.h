/// \cond Model

#ifndef _P_MD2_H
#define _P_MD2_H

#include "../../zerofpsv2/engine/property.h"
#include "../../zerofpsv2/engine/zerofps.h"
#include "zssqpack.h"

using namespace std;

// Shared Defines & Structures
#define MDL_ALIAS_VERSION	6
#define MDL_IDENT_TAG		0x4F504449

#define IDPOLY_MAX_FRAME_NAME	16

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

class P_Md2 : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		TextureManager*	m_pkTextureManger;
		ZSSRender*			m_pkRender;

		bool HandleSetValue( const string& kValueName ,const string& kValue );

		PAKFileFp			m_kFile;
		q2mdlhead_s			m_kHead;
		md2_dstvert_t*		m_pakSkinVert;
		md2_dtriangle_t*	m_pakTris;
		q2frame_t*			m_pakFrames;

		string				m_strFileName;
		float					m_fScale;

		Vector3*				m_pkVertex;
		Vector3*				m_pkNormal;

		int					m_iRenderFrame;
		float					m_fNextFrame;
		float					m_fLastFrame;

		int					m_iTexture;

	public:
		P_Md2();
		~P_Md2();
		
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID )		{ }
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID )	{ }	

		int GetQuakeModellVersion(PAKFileFp *mdlfp, const char* filename);
		void LoadModel(string strFileName);
};

Property* Create_P_Md2();
void Register_P_Md2(ZSSZeroFps* pkZeroFps);

#endif

/// \endcond 
