#include <SDL/SDL.h>
#include "zfvfs.h"
#include "zfassert.h"
#include "zifanimation.h"
#include "jpegdec.h"

typedef struct s_RGB { BYTE B; BYTE G; BYTE R; } RGB;

const int ZIF_MAX_IMAGE_SIZE = 1024*1024*3; // maximal storlek på en frame i animationen
const int ANIMATIONHEADERSIZE = 17; // storlek på headern (information om zif filen som tex antal frames etc)

unsigned char g_aucZIFConvBuff[ZIF_MAX_IMAGE_SIZE]; // En global 3 megabyte buffert som används för att göra konverteringar
int g_iZIFAnimTexIDCounter=0; // ökas upp för varje animation som skapas och används för att generera ett unikt TexIDName
ZIFMemMan* g_pkZIFMan = ZIFMemMan::GetInstance();

ZIFMemMan::~ZIFMemMan() 
{ 
	map<string, ZIF_DATA*>::iterator itData = m_kZIFData.begin();

	for( ; itData != m_kZIFData.end(); itData++)
	{
		if(itData->second->m_pPixelData)
		{
			delete[] itData->second->m_pPixelData;
			itData->second->m_pPixelData = NULL;
		}

		if(itData->second->m_pkJPGDec)
		{
			delete itData->second->m_pkJPGDec;
			itData->second->m_pkJPGDec = NULL;
		}
	}
}

ZIFMemMan::ZIF_DATA* ZIFMemMan::GetData(ZIFAnimation* pkAnimation)
{
	string search_name = string(pkAnimation->m_szFileName);

	map<string, ZIF_DATA*>::iterator itData;
	itData = m_kZIFData.find(search_name);

	if(itData != m_kZIFData.end())
	{
		if(itData->second->m_kUses.find(pkAnimation) == itData->second->m_kUses.end()) 
		{
			itData->second->m_kUses.insert(pkAnimation);
			printf("Animation %s count: %i", search_name.c_str(), (int) itData->second->m_kUses.size());
		}

		return itData->second;
	}
	
	m_kZIFData.insert(map<string, ZIF_DATA*>::value_type(search_name,new ZIF_DATA));

	itData = m_kZIFData.find(search_name);
	itData->second->m_pPixelData = NULL;
	itData->second->m_pkJPGDec = NULL;
	itData->second->m_iIDTexArrayStart = -1;
	itData->second->m_kUses.insert(pkAnimation);
	return itData->second;
}

ZIFAnimation::ZIFAnimation(char* szFileName, int iMode, int iOptions, Rect* pkTextureRect)
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
	m_pkTextureRect = pkTextureRect;
	
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

	m_bLoop = true;

	if(iOptions!=0) {	
		if((iOptions & ZIF_PLAY_ONES)){
			m_bLoop	=	false;
		}
	}

}

ZIFAnimation::~ZIFAnimation()
{
	if(m_szFileName)
		delete[] m_szFileName;

	if(m_pFramesSizesJPG)
		delete[] m_pFramesSizesJPG;
}

bool ZIFAnimation::Update(bool& r_bRebuildTexture)
{
	float fTick = (float(SDL_GetTicks()) /1000.0f);	

	//if(m_bPlay == false || m_szFileName == NULL)
	if(m_szFileName == NULL)
		return false;

	if(m_bPlay == false && m_iNumFrames != 0)
	{
		r_bRebuildTexture = false;
		return true;
	}

	r_bRebuildTexture = m_bRebuildTexture;

	float update_time = (float) m_iMsecsPerFrame / 1000.0f;

	if(fTick - m_fLastTick > update_time) // har det gått X msek sen sist?
	{
		m_fLastTick = fTick;

		if(m_bStream || m_iNumFrames==0)
		{			
			Read();
		}
			
		//
		// Om vi har loopat igenom alla frames...
		//
		if(m_iCurrentFrame==m_iNumFrames-1)
		{
			m_iCurrentFrame=0;

			// Sluta loopa om animationen bara skall spelas 1 gång.
			if(m_bLoop == false)
				m_bPlay = false;

			m_iFileOffset=ANIMATIONHEADERSIZE;

			if(m_eFormat == ZIF_JPEG)
				m_iFileOffset += (sizeof(int) * m_iNumFrames);

			//
			// Om vi inte återanvänder samma textur, sluta då att streama eftersom
			// texturmanagern redan har skapat alla texturer åt oss. Frigör dessutom
			// allt arbetsminne som nu inte längre behövs.
			//
			if(m_bRebuildTexture == false)
			{
				m_bStream = false;
				DeallocateWorkMemory();
			}
		}

		m_iCurrentFrame++;

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
		iFramesToRead = ReadHeader(kFile.m_pkFilePointer);

	//
	// Hoppa förbi headern (och eventuell extra data för jpg formatet)
	//
	fseek(kFile.m_pkFilePointer, m_iFileOffset, SEEK_SET);

	// 
	// Read image data from file and convert it to RGB data
	// 
	const int PICTURE_WIDTH_IN_BYTES = m_iImageFrameWidth*3;
	const int TEXTURE_WIDTH_IN_BYTES = m_iWidth*3;
	int frame_offset=0, frame_size=m_iWidth*m_iHeight*3;

	switch(m_eFormat)
	{
	// Okomprimerad data
	case ZIF_RGB24:
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
		break;
	
	// 256 färgers palett data
	case ZIF_RGB8:
		unsigned int palette_size;
		unsigned int palette[256];
		char* temp_row;
		temp_row = new char[PICTURE_WIDTH_IN_BYTES];

		for(int f=0; f<iFramesToRead; f++)
		{
			fread(&palette_size, sizeof(unsigned int), 1, kFile.m_pkFilePointer);
			fread(palette, sizeof(unsigned int), palette_size, kFile.m_pkFilePointer);
			fread(g_aucZIFConvBuff, sizeof(unsigned char), 
				m_iImageFrameWidth*m_iImageFrameHeight, kFile.m_pkFilePointer);

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

				// Kopiera in pixlarna rad för rad från bilden som är (320x240) 
				// till texturen som är (512x256)
				if(k == PICTURE_WIDTH_IN_BYTES)
				{
					memcpy(m_pPixelData+offset, temp_row, PICTURE_WIDTH_IN_BYTES);
					offset+=TEXTURE_WIDTH_IN_BYTES;
					k=0;
				}
			}

			m_iFileOffset += ( m_iImageFrameWidth*m_iImageFrameHeight + (4*(palette_size+1))  );
		}

		delete[] temp_row;
		break;
	
	// Komprimerad JPEG data
	case ZIF_JPEG:
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
			if(g_pkZIFMan->GetData(this)->m_pkJPGDec)
			{
				delete g_pkZIFMan->GetData(this)->m_pkJPGDec;
				g_pkZIFMan->GetData(this)->m_pkJPGDec = NULL;
				m_pkJPGDec = NULL;
			}
		};
		break;
	}

	kFile.Close();
	return true;
}

char* ZIFAnimation::GetTexIDName()
{
	// Skapa ett nytt texturID namn om vi inte skall bygga om texturen
	// utan istället skapa en ny. Då 
	if(m_bRebuildTexture == false)
	{
		int id = g_pkZIFMan->GetData(this)->m_iIDTexArrayStart+m_iCurrentFrame;
		sprintf(m_szTexIDName, "ZIFAnimTex%i", id/*m_iIDTexArrayStart+m_iCurrentFrame*/);
	}

	return m_szTexIDName;
}


int ZIFAnimation::ReadHeader(FILE* pkFile)
{
	int iFramesToRead = 1;

	unsigned char format;
	fread(&m_iImageFrameWidth, sizeof(int), 1, pkFile);
	fread(&m_iImageFrameHeight, sizeof(int), 1, pkFile);
	fread(&m_iMsecsPerFrame, sizeof(int), 1, pkFile);
	fread(&m_iNumFrames, sizeof(int), 1, pkFile);
	fread(&format, sizeof(unsigned char), 1, pkFile);

	printf("apa = %i\n", m_iImageFrameWidth);
	printf("apa2 = %i\n", m_iImageFrameHeight);

	m_iFileOffset = ANIMATIONHEADERSIZE;

	int bytes_per_pixel;
	if(format == 0) {m_eFormat = ZIF_RGB24; bytes_per_pixel = 3; }
	else if(format == 1) {m_eFormat = ZIF_RGB8; bytes_per_pixel = 1; }
	else if(format == 2) 
	{
		m_eFormat = ZIF_JPEG; bytes_per_pixel = 4; 

		if(g_pkZIFMan->GetData(this)->m_pkJPGDec == NULL)
			g_pkZIFMan->GetData(this)->m_pkJPGDec = new JpgDecoder;

		m_pkJPGDec = g_pkZIFMan->GetData(this)->m_pkJPGDec;
		
		if(m_pFramesSizesJPG)
			delete[] m_pFramesSizesJPG;

		// Läs in storleken på varje frame.
		m_pFramesSizesJPG = new int[m_iNumFrames];
		fread(m_pFramesSizesJPG, m_iNumFrames, sizeof(int), pkFile);
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

	// Registera renderingsrektagelns dimension i ZGuiskin via pekaren 'm_pkTextureRect'
	if(!(m_iHeight == m_iImageFrameHeight && m_iWidth == m_iImageFrameWidth))
	{
		m_pkTextureRect->Left = 0;
		m_pkTextureRect->Top = 0;
		m_pkTextureRect->Right = m_iImageFrameWidth;
		m_pkTextureRect->Bottom = m_iImageFrameHeight;
	}
	else
	{
		m_pkTextureRect->Left =
		m_pkTextureRect->Top =
		m_pkTextureRect->Right =
		m_pkTextureRect->Bottom = 0;
	}

	m_iPixelDataSize = m_iWidth*m_iHeight*3;

	// Kolla så att animationen inte är för stor
	ZFAssert((m_iPixelDataSize <= ZIF_MAX_IMAGE_SIZE), 
		"ZIFAnimation::Read - Animation to big (max 1024x1024)!");

	// Vi ökar på ID räknaren för texturen med antalet frames om denna
	// animation kommer använda så många texturer (1 för varje frame)
	if(m_bRebuildTexture==false)
	{
		if(g_pkZIFMan->GetData(this)->m_iIDTexArrayStart == -1)
		{
			g_pkZIFMan->GetData(this)->m_iIDTexArrayStart = g_iZIFAnimTexIDCounter;
			g_iZIFAnimTexIDCounter += m_iNumFrames;
		}
	}

	if(m_bStream == false)
		iFramesToRead = m_iNumFrames;

	// Allokera minne för RGB bufferten till texturen sammt tempbufferten för konvertering.
	if(g_pkZIFMan->GetData(this)->m_pPixelData == NULL)
		g_pkZIFMan->GetData(this)->m_pPixelData = new char[m_iPixelDataSize*iFramesToRead];

	m_pPixelData = g_pkZIFMan->GetData(this)->m_pPixelData;

	return iFramesToRead;
}

void ZIFAnimation::DeallocateWorkMemory()
{
	if(g_pkZIFMan->GetData(this)->m_kUses.size() == 1)
	{
		if(g_pkZIFMan->GetData(this)->m_pPixelData)
		{
			delete[] g_pkZIFMan->GetData(this)->m_pPixelData;
			g_pkZIFMan->GetData(this)->m_pPixelData = NULL;
			m_pPixelData = NULL;
		}

		if(g_pkZIFMan->GetData(this)->m_pkJPGDec)
		{
			delete g_pkZIFMan->GetData(this)->m_pkJPGDec;
			g_pkZIFMan->GetData(this)->m_pkJPGDec = NULL;
			m_pkJPGDec = NULL;
		}
	}
}