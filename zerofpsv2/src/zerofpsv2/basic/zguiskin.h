// ZGuiSkin.h: interface for the ZGuiSkin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASIC_ZGUISKIN_H_
#define _BASIC_ZGUISKIN_H_

#include "basic_x.h"
#include "rect.h"
#include <stdio.h>

class BASIC_API ZIFAnimation
{
public:
	ZIFAnimation();
	ZIFAnimation(char* szFileName, bool bStream=true);
	~ZIFAnimation();

	bool Update();
	char* GetFramePixels();
	
	int m_iWidth, m_iHeight;
	char* m_szFileName;
	char m_szTexIDName[16]; // unikt TexturID namn för varje zif animation
	bool m_bPlay;

private:
	FILE* m_pkFile;
	float m_fLastTick;
	
	int m_iNumFrames, m_iCurrentFrame;
	int m_iMsecsPerFrame;
	int m_iPixelDataSize;

	char* m_pPixelData;
	bool m_bStream;	// Läs en frame i taget från disk

	bool Read();
};

class BASIC_API ZGuiSkin  
{
public:

	//ZGuiSkin(ZGuiSkin* pkSkin)

	const ZGuiSkin& operator=(const ZGuiSkin& pkSkinSrc );

	bool ZGuiSkin::operator==(ZGuiSkin d);

	ZGuiSkin(ZGuiSkin* pkCopy);	

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

	Rect m_rcBkTile; // Right = Width, Bottom = Height. U

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
	unsigned char m_ucRots90Degree; // Hur många gången texturen skall roteras åt vänster (0,1,2 eller 3)
	bool m_bTileBkSkin;
	bool m_bTransparent; // andvänds bla. av labels som inte skall ha nån bakgrundsfärg.

	ZIFAnimation* m_pkZIFAnimation;
};

#endif







