// ZGuiSkin.h: interface for the ZGuiSkin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASIC_ZGUISKIN_H_
#define _BASIC_ZGUISKIN_H_

#include "basic_x.h"
#include "rect.h"

class JpgDecoder;

enum ZIFAnimationMode
{
	/*----------------------------------------------------------------------------
	 L�s in en bild varje frame till minnet OCH skriv i samma textur varje frame.
	 [+] Kostar lite texturminne efetrsom den �teranv�nnder samma textur.
	 [+] Laddas in efter hand, kr�ver ingen laddningstid.
	 [-] Kostar CPU eftersom den skriver i en textur varje frame.
	 [-] Dekryptering g�rs varje frame, vilket kan sl�a ner f�r jpeg och 8bit.*/
	READ_EVERY_FRAME_and_WRITE_IN_TEXTURE_EVERY_FRAME = 0,
	/*--------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------
	 L�s in alla bilder till minnet OCH skriv i samma textur varje frame.
	 [+] Kostar lite texturminne efetrsom den �teranv�nnder samma textur.
	 [+] Dekryptering g�rs endast n�r animationen l�ses in och ej varje frame (bra f�r jpeg och 8bit).
	 [-] Kostar CPU eftersom den skriver i en textur varje frame.
	 [-] Tar l�ngre tid att ladda in. */
	READ_ALL_and_WRITE_IN_TEXTURE_EVERY_FRAME = 1,
	/*--------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------
	 L�s in en bild varje frame till minnet OCH spara ner varje bild som en ny
	 textur allt eftersom den renderar.
	 Efter f�rsta loopvaret beh�vs ingen l�sning g�ras.
	 [+] Snabb rendering d� varje bild f�r sin egen textur.
	 [+] Laddas in efter hand, kr�ver ingen laddningstid. 
	 [-] Kostar mycket texturminne .
	 [-] Dekryptering g�rs varje frame, vilket kan sl�a ner f�r jpeg och 8bit.*/
	READ_EVERY_FRAME_TO_VIDEOMEMORY = 2,
	/*----------------------------------------------------------------------------

	/*----------------------------------------------------------------------------
	 L�s in alla bilder till minnet OCH spara ner varje bild som en ny 
	 textur allt eftersom den renderar.
	 [+] Snabb rendering d� varje bild f�r sin egen textur.
	 [+] Dekryptering g�rs endast n�r animationen l�ses in och ej varje frame (bra f�r jpeg och 8bit).
	 [-] Kostar mycket texturminne .
	 [-] Tar l�ngre tid att ladda in. */
	READ_ALL_TO_VIDEOMEMORY = 3
	/*--------------------------------------------------------------------------*/
};

class BASIC_API ZIFAnimation
{
public:
	ZIFAnimation(char* szFileName=0, int iMode=0);
	~ZIFAnimation();

	bool Update();
	char* GetFramePixels();
	char* GetTexIDName();
	
	int m_iWidth, m_iHeight;
	bool m_bPlay;
	bool m_bRebuildTexture; // Anv�nd en och samma textur. OBS! _B�R_ vara satt till true annars ryker allt texturminne!!! 
							// Endast sm� animationer (p� typ max: 40 frame) kan dra nytta av att s�tta denna till false f�r att slippa bygga om texturen varje frame.

private:

	enum ZIFFormat
	{
		RGB24=0,
		RGB8=1,
		JPEG=2,
	};

	ZIFAnimationMode m_eMode;

	float m_fLastTick;
	
	int m_iNumFrames, m_iCurrentFrame;
	int m_iMsecsPerFrame;
	int m_iPixelDataSize;

	char* m_pPixelData;
	bool m_bStream;	// L�s en frame i taget fr�n disk
	
	char m_szTexIDName[16]; // unikt TexturID namn f�r varje zif animation
	int m_iIDTexArrayStart; // startID, om varje animationsframe har sin egen textur
	char* m_szFileName;

	bool Read();

	//bool m_b8bitsFormat;
	ZIFFormat m_eFormat;
	unsigned int m_iFileOffset; // beh�vs eftersom paletten kan vara olika stor p� 8 bitars animationer
	int m_iImageFrameWidth, m_iImageFrameHeight;

	JpgDecoder* m_pkJPGDec;

	int* m_pFramesSizesJPG;
	
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

	long m_iBkTexID;			// -1 inneb�r att det inte finns n�n textur.
	long m_iHorzBorderTexID;	// -1 inneb�r att det inte finns n�n textur.
	long m_iVertBorderTexID;	// -1 inneb�r att det inte finns n�n textur.
	long m_iBorderCornerTexID;	// -1 inneb�r att det inte finns n�n textur.

	long m_iBkTexAlphaID;			// -1 inneb�r att det inte finns n�n textur.
	long m_iHorzBorderTexAlphaID;	// -1 inneb�r att det inte finns n�n textur.
	long m_iVertBorderTexAlphaID;	// -1 inneb�r att det inte finns n�n textur.
	long m_iBorderCornerTexAlphaID;	// -1 inneb�r att det inte finns n�n textur.

	float m_afBkColor[3];
	float m_afBorderColor[3];

	unsigned short m_unBorderSize; // Bredden och h�jden p� ramen kring f�nstret.
	unsigned char m_ucRots90Degree; // Hur m�nga g�ngen texturen skall roteras �t v�nster (0,1,2 eller 3)
	bool m_bTileBkSkin;
	bool m_bTransparent; // andv�nds bla. av labels som inte skall ha n�n bakgrundsf�rg.

	ZIFAnimation* m_pkZIFAnimation;
};

#endif







