// ZGuiSkin.cpp: implementation of the ZGuiSkin class.
//
///////////////////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#include "zfvfs.h"
#include "zfassert.h"
#include "zguiskin.h"
#include "jpegdec.h"

const int ZIF_MAX_IMAGE_SIZE = 1024*1024*3; // maximal storlek på en frame i animationen
const int ANIMATIONHEADERSIZE = 17;

unsigned char g_aucZIFConvBuff[ZIF_MAX_IMAGE_SIZE]; // En global 3 megabyte buffert som används för att göra konverteringar
int g_iZIFAnimTexIDCounter=0; // ökas upp för varje animation som skapas och används för att generera ett unikt TexIDName

typedef struct s_RGB {
			 BYTE B;
		     BYTE G;
		     BYTE R;
} RGB;

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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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

	m_pkZIFAnimation		= NULL;
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
	m_pkZIFAnimation		= pkSkinSrc.m_pkZIFAnimation;

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

	m_pkZIFAnimation		= pkCopy->m_pkZIFAnimation;
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

	if(m_pkZIFAnimation != d.m_pkZIFAnimation) return false;

	return true;
} 

///////////////////////////////////////////////////////////////////////////////
// ZIFAnimation
///////////////////////////////////////////////////////////////////////////////

ZIFAnimation::ZIFAnimation(char* szFileName, int iMode)
{
	m_fLastTick=0;
	m_szFileName=new char[strlen(szFileName)+1];
	strcpy(m_szFileName,szFileName);
	m_iWidth=0;
	m_iHeight=0;
	m_iNumFrames=0;
	m_iMsecsPerFrame=0;
	m_iCurrentFrame=0;
	m_bPlay=false;
	m_pPixelData=NULL;
	m_iPixelDataSize=0;
	
	g_iZIFAnimTexIDCounter++;
	sprintf(m_szTexIDName, "ZIFAnimTex%i", g_iZIFAnimTexIDCounter);

	m_eMode = ZIFAnimationMode(iMode);

	switch(m_eMode)
	{
	case READ_EVERY_FRAME_and_WRITE_IN_TEXTURE_EVERY_FRAME:
		m_bRebuildTexture = m_bStream = true;
		break;
	case READ_ALL_and_WRITE_IN_TEXTURE_EVERY_FRAME:
		m_bStream = false;
		m_bRebuildTexture = true;
		break;
	case READ_EVERY_FRAME_TO_VIDEOMEMORY:
		m_bStream = true;
		m_bRebuildTexture = false;
		break;
	case READ_ALL_TO_VIDEOMEMORY:
		m_bRebuildTexture = m_bStream = false;
		break;
	default:
		m_bRebuildTexture = m_bStream = true;
	}
	
	m_iFileOffset = 0;
	m_pkJPGDec=NULL;
	m_pFramesSizesJPG=NULL;
}

ZIFAnimation::~ZIFAnimation()
{
	if(m_pPixelData)
		delete[] m_pPixelData;

	if(m_szFileName)
		delete[] m_szFileName;

	if(m_pkJPGDec)
		delete m_pkJPGDec;
}

bool ZIFAnimation::Update()
{
	float fTick = (float(SDL_GetTicks()) /1000.0f);	

	if(m_bPlay == false || m_szFileName == NULL)
		return false;

	float update_time = (float) m_iMsecsPerFrame / 1000.0f;

	if(fTick - m_fLastTick > update_time) // har det gått X msek sen sist?
	{
		m_fLastTick = fTick;

		if(m_bStream || m_iNumFrames==0)
		{
			Read();
		}
			
		m_iCurrentFrame++;

		//
		// Om vi har loopat igenom alla frames...
		//
		if(m_iCurrentFrame==m_iNumFrames-1)
		{
			m_iFileOffset=ANIMATIONHEADERSIZE;

			if(m_eFormat == JPEG)
			{
				m_iFileOffset += (sizeof(int) * m_iNumFrames);
			}

			m_iCurrentFrame=0;

			//
			// Om vi inte återanvänder samma textur, sluta då att streama eftersom
			// texturmanagern redan har skapat alla texturer åt oss. Frigör dessutom
			// allt arbetsminne som nu inte längre behövs.
			//
			if(m_bRebuildTexture == false)
			{
				m_bStream = false;

				if(m_pPixelData)
				{
					delete[] m_pPixelData;
					m_pPixelData = NULL;
				}

				if(m_pkJPGDec)
				{
					delete m_pkJPGDec;
					m_pkJPGDec = NULL;
				}
			}
		}

		return true; // behöver läsa in ny bild till texturen
	}

	return false; // behöver inte uppdatera
}

char* ZIFAnimation::GetFramePixels()
{
	if(m_bStream == true)
		return m_pPixelData;
	else
	{
		if(m_pPixelData)
			return m_pPixelData+(m_iPixelDataSize*m_iCurrentFrame);	
		else
			return NULL; // returnera NULL då alla frames har loopats igenom och 
						 // m_pPixelData har deallokerats eftersom texturmanagern ej längre 
						 // bygger om texturen utan har skapat en textur för varje frame.
	}
}

bool ZIFAnimation::Read()
{
	ZFVFile kFile;

	//
	// Open file
	//

	if(kFile.Open(m_szFileName,0, false) == false) 
	{
		fprintf(stderr, "Unable to Open %s: \n", m_szFileName);
		return false;
	}

	unsigned int iFramesToRead = 1;

	//
	// Read header
	//

	if(m_iNumFrames==0) // not initialized
	{
		unsigned char format;
		fread(&m_iImageFrameWidth, sizeof(int), 1, kFile.m_pkFilePointer);
		fread(&m_iImageFrameHeight, sizeof(int), 1, kFile.m_pkFilePointer);
		fread(&m_iMsecsPerFrame, sizeof(int), 1, kFile.m_pkFilePointer);
		fread(&m_iNumFrames, sizeof(int), 1, kFile.m_pkFilePointer);
		fread(&format, sizeof(unsigned char), 1, kFile.m_pkFilePointer);

		m_iFileOffset = ANIMATIONHEADERSIZE;

		int bytes_per_pixel;
		if(format == 0) {m_eFormat = RGB24; bytes_per_pixel = 3; }
		else if(format == 1) {m_eFormat = RGB8; bytes_per_pixel = 1; }
		else if(format == 2) 
		{
			m_pkJPGDec = new JpgDecoder();
			m_eFormat = JPEG; bytes_per_pixel = 4; 
			
			if(m_pFramesSizesJPG)
				delete[] m_pFramesSizesJPG;

			// Läs in storleken på varje frame.
			m_pFramesSizesJPG = new int[m_iNumFrames];
			fread(m_pFramesSizesJPG, m_iNumFrames, sizeof(int), kFile.m_pkFilePointer);
			m_iFileOffset += (sizeof(int) * m_iNumFrames);

			int biggest = 0;
			for(int i=0; i<m_iNumFrames; i++)
				if(m_pFramesSizesJPG[i] > biggest)
					biggest = m_pFramesSizesJPG[i];

			// Allokera minne för den största framen
			m_pkJPGDec->AllocateBuffers(biggest,m_iImageFrameWidth,m_iImageFrameHeight);
		}

		m_iWidth = 0;
		m_iHeight = 0;

		// Set dimension to next valid texture size.
		int valid_sizes[] = {2048,1024,512,256,128,64,32,16,8,4,2,1};
		for(int i=0; i<12; i++)
		{
			if(m_iWidth == 0)
			{
				if(m_iImageFrameWidth > valid_sizes[i]) m_iWidth = valid_sizes[i-1];
				else if(m_iImageFrameWidth == valid_sizes[i]) m_iWidth = valid_sizes[i];
			}
			if(m_iHeight == 0)
			{
				if(m_iImageFrameHeight > valid_sizes[i]) m_iHeight = valid_sizes[i-1];
				else if(m_iImageFrameHeight == valid_sizes[i]) m_iHeight = valid_sizes[i];
			}
		}

		m_iPixelDataSize = m_iWidth*m_iHeight*3;

		// Kolla så att animationen inte är för stor
		ZFAssert((m_iPixelDataSize <= ZIF_MAX_IMAGE_SIZE), 
			"ZIFAnimation::Read - Animation to big (max 1024x1024)!");

		// Vi ökar på ID räknaren för texturen med antalet frames om denna
		// animation kommer använda så många texturer (1 för varje frame)
		if(m_bRebuildTexture==false)
		{
			m_iIDTexArrayStart = g_iZIFAnimTexIDCounter;
			g_iZIFAnimTexIDCounter += m_iNumFrames;
		}

		if(m_bStream == false)
			iFramesToRead = m_iNumFrames;

		// Allokera minne för RGB bufferten till texturen sammt tempbufferten för konvertering.
		m_pPixelData = new char[m_iPixelDataSize*m_iNumFrames];
	}

	// 
	// Read image data 
	// 

	fseek(kFile.m_pkFilePointer, m_iFileOffset, SEEK_SET);

	const int PICTURE_WIDTH_IN_BYTES = m_iImageFrameWidth*3;
	const int TEXTURE_WIDTH_IN_BYTES = m_iWidth*3;

	if(m_eFormat == RGB8)
	{
		unsigned int palette_size;
		unsigned int palette[256];
		char* temp_row = new char[PICTURE_WIDTH_IN_BYTES];

		for(int f=0; f<iFramesToRead; f++)
		{
			fread(&palette_size, sizeof(unsigned int), 1, kFile.m_pkFilePointer);
			fread(palette, sizeof(unsigned int), palette_size, kFile.m_pkFilePointer);
			fread(g_aucZIFConvBuff, sizeof(unsigned char), m_iImageFrameWidth*m_iImageFrameHeight, kFile.m_pkFilePointer);

			// Konvertera till 24-bitar
			unsigned int r,g,b,i,j=0,k=0,palette_idex,bytes=320*240*3,offset=f*m_iWidth*m_iHeight*3;
			for(i=0; i<bytes; i+=3)
			{
				palette_idex = g_aucZIFConvBuff[j++];
				r = palette[palette_idex];
				g = palette[palette_idex];
				b = palette[palette_idex];

				g >>= 8;
				b >>= 16;

				r = r & 0xFF;
				g = g & 0xFF;
				b = b & 0xFF;

				temp_row[k] = r;
				temp_row[k+1] = g;
				temp_row[k+2] = b;
				k+=3;

				// Kopiera in pixlarna rad för rad från bilden som är (320x240) till texturen som är (512x256)
				if(k == PICTURE_WIDTH_IN_BYTES)
				{
					memcpy(m_pPixelData+offset, temp_row, PICTURE_WIDTH_IN_BYTES);
					offset+=TEXTURE_WIDTH_IN_BYTES;
					k=0;
				}
			}

			m_iFileOffset += ( m_iImageFrameWidth*m_iImageFrameHeight + (4*(palette_size+1))  );
		}

		static int apa = 0; 
		printf("apa %i\n", apa);
		apa++;

		delete[] temp_row;
	}
	else
	if(m_eFormat == RGB24)
	{
		int frame_offset=0, frame_size=m_iWidth*m_iHeight*3;
		for(int f=0; f<iFramesToRead; f++)
		{
			if(m_iImageFrameWidth != m_iWidth || m_iImageFrameHeight != m_iHeight)
			{
				fread(g_aucZIFConvBuff, sizeof(unsigned char), 
					m_iImageFrameWidth*m_iImageFrameHeight*3, kFile.m_pkFilePointer);
			
				for(int i=0; i<m_iImageFrameHeight; i++)
					memcpy(m_pPixelData+(TEXTURE_WIDTH_IN_BYTES*i+frame_offset), 
						g_aucZIFConvBuff+(PICTURE_WIDTH_IN_BYTES*i), PICTURE_WIDTH_IN_BYTES);
			}
			else
			{
				fread(m_pPixelData+frame_offset, sizeof(unsigned char), 
					m_iPixelDataSize, kFile.m_pkFilePointer);
			}

			m_iFileOffset += m_iImageFrameWidth*m_iImageFrameHeight*3;
			frame_offset += frame_size;
		}
	}
	else
	if(m_eFormat == JPEG)
	{
		int frame_offset=0, frame_size=m_iWidth*m_iHeight*3;
		for(int f=0; f<iFramesToRead; f++)
		{
			unsigned char* buffer;
			unsigned int w, h;
			m_pkJPGDec->LoadHeader(kFile.m_pkFilePointer, m_pFramesSizesJPG[m_iCurrentFrame+f], &w, &h);
			m_pkJPGDec->Decode();
			m_pkJPGDec->GetBuffer(w,h,&buffer);

			int x,y,oka=frame_offset;
			RGB *pixel;
			DWORD im_loc_bytes=(DWORD)buffer;
			const int iRowExtra = (m_iWidth-w) * 3;

			for (y=0;y<h;y++)
			{
				for (x=0;x<w;x++)
				{
					pixel=(RGB *)im_loc_bytes;

					m_pPixelData[oka] = pixel->B;
					m_pPixelData[oka+1] = pixel->G;
					m_pPixelData[oka+2] = pixel->R;
					oka+=3;

					im_loc_bytes+=4;
				}

				oka += iRowExtra;
			}  

			m_iFileOffset += m_pFramesSizesJPG[m_iCurrentFrame+f];
			frame_offset += frame_size;
		}

		// Ta bort konverteringsbufferten nu eftersom den inte 
		// kommer att behövas mer och bara tar upp massa minne.
		if(m_bStream == false)
		{
			delete m_pkJPGDec;
			m_pkJPGDec = NULL;
		}
	}

	kFile.Close();
	return true;
}

char* ZIFAnimation::GetTexIDName()
{
	// Skapa ett nytt texturID namn om vi inte skall bygga om texturen
	// utan istället skapa en ny. Då 
	if(m_bRebuildTexture == false)
		sprintf(m_szTexIDName, "ZIFAnimTex%i", m_iIDTexArrayStart+m_iCurrentFrame);

	return m_szTexIDName;
}














