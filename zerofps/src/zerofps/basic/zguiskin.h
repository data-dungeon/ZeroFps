// ZGuiSkin.h: interface for the ZGuiSkin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUISKIN_H__E13E6AC7_8EAE_468F_A100_74C5559BE66E__INCLUDED_)
#define AFX_ZGUISKIN_H__E13E6AC7_8EAE_468F_A100_74C5559BE66E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "basic_x.h"

class BASIC_API ZGuiSkin  
{
public:

	//ZGuiSkin(ZGuiSkin* pkSkin)

	const ZGuiSkin& operator=(const ZGuiSkin& pkSkinSrc );

	// Everything : 17 parameters
	ZGuiSkin(int t1,int t2,int t3,int t4,	// Textures
			 int a1,int a2,int a3,int a4,	// Alpha Textures
			 int r1,int g1,int b1,			// Background color
			 int r2,int g2,int b2,			// Border color
			 int bsz,bool tile_texture,		// Border size, tile textures.
			 bool transparent);				// Transparent background

	// Only textures : 10 parameters
	ZGuiSkin(int t1,int t2,int t3,int t4,	// Textures
			 int a1,int a2,int a3,int a4,	// Alpha Textures
			 int bsz, bool tile_texture);	// Border size, tile textures.

	// Only colors and transparence: 8 parameters
	ZGuiSkin(int r1,int g1,int b1,			// Background color
	  		 int r2,int g2,int b2,			// Border color
			 int bsz, bool bTransparent);	// Border size

	// Only colors: 7 parameters
	ZGuiSkin(int r1,int g1,int b1,			// Background color
	  		 int r2,int g2,int b2,			// Border color
			 int bsz);						// Border size

	// Only textures, no alpha : 5 parameters
	ZGuiSkin(int t1,int t2,int t3,int t4,	// Textures
			 bool tile_texture);			// Tile textures.

	// Only bk texture, no alpha : 3 parameter
	ZGuiSkin(int t1,						// Textures
			 int a1,						// Alpha Textures
			 bool tile_texture);			// Tile textures.

	// Only bk texture, no alpha : 2 parameter
	ZGuiSkin(int t1,						// Textures
			 bool tile_texture);			// Tile textures.

	// Default : 0 parameter
	ZGuiSkin();								

	~ZGuiSkin() {}

	long m_iBkTexID;			// -1 innebär att det inte finns nån textur.
	long m_iHorzBorderTexID;	// -1 innebär att det inte finns nån textur.
	long m_iVertBorderTexID;	// -1 innebär att det inte finns nån textur.
	long m_iBorderCornerTexID;	// -1 innebär att det inte finns nån textur.

	long m_iBkTexAlphaID;			// -1 innebär att det inte finns nån textur.
	long m_iHorzBorderTexAlphaID;	// -1 innebär att det inte finns nån textur.
	long m_iVertBorderTexAlphaID;	// -1 innebär att det inte finns nån textur.
	long m_iBorderCornerTexAlphaID;	// -1 innebär att det inte finns nån textur.

	float m_afBkColor[3];
	float m_afBorderColor[3];

	unsigned short m_unBorderSize; // Bredden och höjden på ramen kring fönstret.
	bool m_bTileBkSkin;
	bool m_bTransparent; // andvänds bla. av labels som inte skall ha nån bakgrundsfärg.
};

#endif // !defined(AFX_ZGUISKIN_H__E13E6AC7_8EAE_468F_A100_74C5559BE66E__INCLUDED_)







