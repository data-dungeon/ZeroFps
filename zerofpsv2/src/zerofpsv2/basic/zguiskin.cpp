// ZGuiSkin.cpp: implementation of the ZGuiSkin class.
//
///////////////////////////////////////////////////////////////////////////////

#include "zguiskin.h"

///////////////////////////////////////////////////////////////////////////////
// Everything : 16 parameters
ZGuiSkin::ZGuiSkin( int t1,int t2,int t3,int t4,	// Textures 
					int a1,int a2,int a3,int a4,	// Alpha Textures
					int r1,int g1,int b1,			// Background color
					int r2,int g2,int b2,			// Border color
					int bsz,bool tile_texture,		// Border size, tile tex.
					bool transparent)
{
	m_iBkTexID					= t1;		
	m_iHorzBorderTexID			= t2;
	m_iVertBorderTexID			= t3;
	m_iBorderCornerTexID		= t4;

	m_iBkTexAlphaID				= a1;	
	m_iHorzBorderTexAlphaID		= a2;
	m_iVertBorderTexAlphaID		= a3;
	m_iBorderCornerTexAlphaID	= a4;

	m_afBkColor[0]				= (1.0f/255)*r1;
	m_afBkColor[1] 				= (1.0f/255)*g1;
	m_afBkColor[2] 				= (1.0f/255)*b1;

	m_afBorderColor[0] 			= (1.0f/255)*r2;
	m_afBorderColor[1] 			= (1.0f/255)*g2;
	m_afBorderColor[2] 			= (1.0f/255)*b2;

	m_unBorderSize				= (unsigned short) bsz; 
	m_bTileBkSkin				= tile_texture;
	m_bTransparent				= transparent;

	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Only textures : 10 parameters
ZGuiSkin::ZGuiSkin( int t1,int t2,int t3,int t4,	// Textures
					int a1,int a2,int a3,int a4,	// Alpha Textures
					int bsz, bool tile_texture)		// Border size, tile tex.
{
	m_iBkTexID					= t1;		
	m_iHorzBorderTexID			= t2;
	m_iVertBorderTexID			= t3;
	m_iBorderCornerTexID		= t4;

	m_iBkTexAlphaID				= a1;	
	m_iHorzBorderTexAlphaID		= a2;
	m_iVertBorderTexAlphaID		= a3;
	m_iBorderCornerTexAlphaID	= a4;

	m_afBkColor[0]				= 1;
	m_afBkColor[1] 				= 1;
	m_afBkColor[2] 				= 1;

	m_afBorderColor[0] 			= 1;
	m_afBorderColor[1] 			= 1;
	m_afBorderColor[2] 			= 1;

	m_unBorderSize				= (unsigned short) bsz; 
	m_bTileBkSkin				= tile_texture;
	m_bTransparent				= false;

	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Only colors: 7 parameters
ZGuiSkin::ZGuiSkin( int r1,int g1,int b1,			// Background color
					int r2,int g2,int b2,			// Border color
					int bsz,						// Border size
					bool bTransparent)				// Transparent
{
	m_iBkTexID					= -1;		
	m_iHorzBorderTexID			= -1;
	m_iVertBorderTexID			= -1;
	m_iBorderCornerTexID		= -1;

	m_iBkTexAlphaID				= -1;	
	m_iHorzBorderTexAlphaID		= -1;
	m_iVertBorderTexAlphaID		= -1;
	m_iBorderCornerTexAlphaID	= -1;

	m_afBkColor[0]				= (1.0f/255)*r1;
	m_afBkColor[1] 				= (1.0f/255)*g1;
	m_afBkColor[2] 				= (1.0f/255)*b1;

	m_afBorderColor[0] 			= (1.0f/255)*r2;
	m_afBorderColor[1] 			= (1.0f/255)*g2;
	m_afBorderColor[2] 			= (1.0f/255)*b2;

	m_unBorderSize				= (unsigned short) bsz; 
	m_bTileBkSkin				= false;
	m_bTransparent				= bTransparent;
	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Only colors: 7 parameters
ZGuiSkin::ZGuiSkin( int r1,int g1,int b1,			// Background color
					int r2,int g2,int b2,			// Border color
					int bsz)						// Border size
{
	m_iBkTexID					= -1;		
	m_iHorzBorderTexID			= -1;
	m_iVertBorderTexID			= -1;
	m_iBorderCornerTexID		= -1;

	m_iBkTexAlphaID				= -1;	
	m_iHorzBorderTexAlphaID		= -1;
	m_iVertBorderTexAlphaID		= -1;
	m_iBorderCornerTexAlphaID	= -1;

	m_afBkColor[0]				= (1.0f/255)*r1;
	m_afBkColor[1] 				= (1.0f/255)*g1;
	m_afBkColor[2] 				= (1.0f/255)*b1;

	m_afBorderColor[0] 			= (1.0f/255)*r2;
	m_afBorderColor[1] 			= (1.0f/255)*g2;
	m_afBorderColor[2] 			= (1.0f/255)*b2;

	m_unBorderSize				= (unsigned short) bsz; 
	m_bTileBkSkin				= false;
	m_bTransparent				= false;

	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Only textures, no alpha : 5 parameters
ZGuiSkin::ZGuiSkin( int t1,int t2,int t3,int t4,	// Textures
					bool tile_texture)				// Tile textures.
{
	m_iBkTexID					= t1;		
	m_iHorzBorderTexID			= t2;
	m_iVertBorderTexID			= t3;
	m_iBorderCornerTexID		= t4;

	m_iBkTexAlphaID				= -1;	
	m_iHorzBorderTexAlphaID		= -1;
	m_iVertBorderTexAlphaID		= -1;
	m_iBorderCornerTexAlphaID	= -1;

	m_afBkColor[0]				= 1;
	m_afBkColor[1] 				= 1;
	m_afBkColor[2] 				= 1;

	m_afBorderColor[0] 			= 1;
	m_afBorderColor[1] 			= 1;
	m_afBorderColor[2] 			= 1;

	m_unBorderSize				= 0; 
	m_bTileBkSkin				= tile_texture;
	m_bTransparent				= false;

	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Only bk texture, no alpha : 2 parameter
ZGuiSkin::ZGuiSkin(	int t1,	int a1,				// Textures
					bool tile_texture)			// Tile textures.
{
	m_iBkTexID					= t1;		
	m_iHorzBorderTexID			= -1;
	m_iVertBorderTexID			= -1;
	m_iBorderCornerTexID		= -1;

	m_iBkTexAlphaID				= a1;	
	m_iHorzBorderTexAlphaID		= -1;
	m_iVertBorderTexAlphaID		= -1;
	m_iBorderCornerTexAlphaID	= -1;

	m_afBkColor[0]				= 1;
	m_afBkColor[1] 				= 1;
	m_afBkColor[2] 				= 1;

	m_afBorderColor[0] 			= 1;
	m_afBorderColor[1] 			= 1;
	m_afBorderColor[2] 			= 1;

	m_unBorderSize				= 0; 
	m_bTileBkSkin				= tile_texture;
	m_bTransparent				= false;

	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Only bk texture, no alpha : 2 parameter
ZGuiSkin::ZGuiSkin(	int t1,						// Textures
					bool tile_texture)			// Tile textures.
{
	m_iBkTexID					= t1;		
	m_iHorzBorderTexID			= -1;
	m_iVertBorderTexID			= -1;
	m_iBorderCornerTexID		= -1;

	m_iBkTexAlphaID				= -1;	
	m_iHorzBorderTexAlphaID		= -1;
	m_iVertBorderTexAlphaID		= -1;
	m_iBorderCornerTexAlphaID	= -1;

	m_afBkColor[0]				= 1;
	m_afBkColor[1] 				= 1;
	m_afBkColor[2] 				= 1;

	m_afBorderColor[0] 			= 1;
	m_afBorderColor[1] 			= 1;
	m_afBorderColor[2] 			= 1;

	m_unBorderSize				= 0; 
	m_bTileBkSkin				= tile_texture;
	m_bTransparent				= false;
	m_bTransparent				= false;

	m_ucRots90Degree		= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Default : 0 parameter
ZGuiSkin::ZGuiSkin()			
{
	m_iBkTexID					= -1;		
	m_iHorzBorderTexID			= -1;
	m_iVertBorderTexID			= -1;
	m_iBorderCornerTexID		= -1;

	m_iBkTexAlphaID				= -1;	
	m_iHorzBorderTexAlphaID		= -1;
	m_iVertBorderTexAlphaID		= -1;
	m_iBorderCornerTexAlphaID	= -1;

	m_afBkColor[0]				= 1;
	m_afBkColor[1] 				= 1;
	m_afBkColor[2] 				= 1;

	m_afBorderColor[0] 			= 1;
	m_afBorderColor[1] 			= 1;
	m_afBorderColor[2] 			= 1;

	m_unBorderSize				= 0; 
	m_bTileBkSkin				= false;
	m_bTransparent				= false;

	m_ucRots90Degree		= 0;
}

const ZGuiSkin& ZGuiSkin::operator=(const ZGuiSkin& pkSkinSrc )
{
	m_iBkTexID					= pkSkinSrc.m_iBkTexID;
	m_iHorzBorderTexID			= pkSkinSrc.m_iHorzBorderTexID;
	m_iVertBorderTexID			= pkSkinSrc.m_iVertBorderTexID;
	m_iBorderCornerTexID		= pkSkinSrc.m_iBorderCornerTexID;

	m_iBkTexAlphaID				= pkSkinSrc.m_iBkTexAlphaID;
	m_iHorzBorderTexAlphaID		= pkSkinSrc.m_iHorzBorderTexAlphaID;
	m_iVertBorderTexAlphaID		= pkSkinSrc.m_iVertBorderTexAlphaID;
	m_iBorderCornerTexAlphaID	= pkSkinSrc.m_iBorderCornerTexAlphaID;

	m_afBkColor[0]				= pkSkinSrc.m_afBkColor[0];
	m_afBkColor[1] 				= pkSkinSrc.m_afBkColor[1];
	m_afBkColor[2] 				= pkSkinSrc.m_afBkColor[2];

	m_afBorderColor[0] 			= pkSkinSrc.m_afBorderColor[0];
	m_afBorderColor[1] 			= pkSkinSrc.m_afBorderColor[1];
	m_afBorderColor[2] 			= pkSkinSrc.m_afBorderColor[2];

	m_unBorderSize				= pkSkinSrc.m_unBorderSize;
	m_bTileBkSkin				= pkSkinSrc.m_bTileBkSkin;
	m_bTransparent				= pkSkinSrc.m_bTransparent;

	m_ucRots90Degree		= pkSkinSrc.m_ucRots90Degree;

	m_rcBkTile				= pkSkinSrc.m_rcBkTile;

	return *this;
}

ZGuiSkin::ZGuiSkin(ZGuiSkin* pkCopy)
{
	m_iBkTexID						= pkCopy->m_iBkTexID;
	m_iHorzBorderTexID			= pkCopy->m_iHorzBorderTexID;
	m_iVertBorderTexID			= pkCopy->m_iVertBorderTexID;
	m_iBorderCornerTexID			= pkCopy->m_iBorderCornerTexID;

	m_iBkTexAlphaID				= pkCopy->m_iBkTexAlphaID;
	m_iHorzBorderTexAlphaID		= pkCopy->m_iHorzBorderTexAlphaID;
	m_iVertBorderTexAlphaID		= pkCopy->m_iVertBorderTexAlphaID;
	m_iBorderCornerTexAlphaID	= pkCopy->m_iBorderCornerTexAlphaID;

	m_afBkColor[0]					= pkCopy->m_afBkColor[0];
	m_afBkColor[1] 				= pkCopy->m_afBkColor[1];
	m_afBkColor[2] 				= pkCopy->m_afBkColor[2];

	m_afBorderColor[0] 			= pkCopy->m_afBorderColor[0];
	m_afBorderColor[1] 			= pkCopy->m_afBorderColor[1];
	m_afBorderColor[2] 			= pkCopy->m_afBorderColor[2];

	m_unBorderSize					= pkCopy->m_unBorderSize;
	m_bTileBkSkin					= pkCopy->m_bTileBkSkin;
	m_bTransparent					= pkCopy->m_bTransparent;	

	m_ucRots90Degree		= pkCopy->m_ucRots90Degree;

	m_rcBkTile				= pkCopy->m_rcBkTile;
}

bool ZGuiSkin::operator==(ZGuiSkin d) 
{   
	if(m_iBkTexID						!= d.m_iBkTexID) return false;
	if(m_iHorzBorderTexID			!= d.m_iHorzBorderTexID) return false;
	if(m_iVertBorderTexID			!= d.m_iVertBorderTexID) return false;
	if(m_iBorderCornerTexID			!= d.m_iBorderCornerTexID) return false;

	if(m_iBkTexAlphaID				!= d.m_iBkTexAlphaID) return false;
	if(m_iHorzBorderTexAlphaID		!= d.m_iHorzBorderTexAlphaID) return false;
	if(m_iVertBorderTexAlphaID		!= d.m_iVertBorderTexAlphaID) return false;
	if(m_iBorderCornerTexAlphaID	!= d.m_iBorderCornerTexAlphaID) return false;

	if(m_afBkColor[0]					!= d.m_afBkColor[0]) return false;
	if(m_afBkColor[1] 				!= d.m_afBkColor[1]) return false;
	if(m_afBkColor[2] 				!= d.m_afBkColor[2]) return false;

	if(m_afBorderColor[0] 			!= d.m_afBorderColor[0]) return false;
	if(m_afBorderColor[1] 			!= d.m_afBorderColor[1]) return false;
	if(m_afBorderColor[2] 			!= d.m_afBorderColor[2]) return false;

	if(m_unBorderSize					!= d.m_unBorderSize) return false;
	if(m_bTileBkSkin					!= d.m_bTileBkSkin) return false;
	if(m_bTransparent					!= d.m_bTransparent) return false;

	if(m_ucRots90Degree			!= d.m_ucRots90Degree) return false;

	if(m_rcBkTile					!= d.m_rcBkTile) return false;

	return true;
} 
