// zguifont.cpp: implementation of the ZGuiFont class.
//
//////////////////////////////////////////////////////////////////////

#include "zguifont.h"
#include <memory.h>
#include "image.h"
#include "zfvfs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiFont::ZGuiFont(char cCharsOneRow, char cCharacterCellSize, char cPixelGapBetweenChars, int iID)
{
	m_iID = iID;
	m_iBMPWidth = 256;
	m_cCharCellSize = cCharacterCellSize;
	m_cCharsOneRow = cCharsOneRow;
	m_cPixelGapBetweenChars = cPixelGapBetweenChars;
	m_szFileName = ""; // "NONE"
	m_iType=0;

	int counter=0;
	int cRowWidth = m_cCharCellSize*m_cCharsOneRow;
	for(int y=0; y<cRowWidth; y+=m_cCharCellSize)
	{
		for(int x=0; x<cRowWidth; x+=m_cCharCellSize)
		{
			m_aChars[counter].iPosX = x;
			m_aChars[counter].iPosY = y;
			m_aChars[counter].iSizeX = m_cCharCellSize;
			m_aChars[counter].iSizeY = m_cCharCellSize;
			counter++;

			if(counter > 255)
				goto stop;
		}
	}
	stop: { /*printf("jolly jumper\n");*/ }
}

ZGuiFont::~ZGuiFont()
{

}

bool ZGuiFont::CreateFromFile(char* szFileName)
{

/*	ZFVFileSystem* pkFileSys;
	pkFileSys = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	string strFull = pkFileSys->GetFullPath(szFileName);*/

	ZFVFile kFile;
	if(!kFile.Open(szFileName,0,false))
	{
		printf("Failed to open file for creating font\n");
		return false;
	}

	Image kImage;
	bool bSuccess = kImage.load(kFile.m_pkFilePointer, szFileName);

	if(!bSuccess)
	{
		printf("font generation: Failed to find bitmap file [%s]!\n", szFileName);
		return false;
	}

	m_szFileName = string(szFileName);	

	m_iType = 0; // bmp

	FILE* pkFile = NULL;

	bool bIsTGA = false;
	if(m_szFileName.find(".tga") != string::npos)
	{
		bIsTGA = true;
		m_iType = 1;

		//pkFile = fopen("apa.txt", "wt");

		//for(int y=0; y<kImage.m_iWidth; y++)
		//{
		//	for(int x=0; x<kImage.m_iWidth; x++)
		//	{
		//		color_rgba kCurrColor;
		//		kImage.get_pixel(x, kImage.m_iWidth-1-y, kCurrColor); 
		//		fprintf(pkFile, "%i ", (int) (kCurrColor.a));

		//		if(kCurrColor.a < 10)
		//			fprintf(pkFile, " ");
		//		if(kCurrColor.a < 100)
		//			fprintf(pkFile, " ");
		//	}

		//	fprintf(pkFile, "\n");
		//}

		//return false;

	}

	m_iBMPWidth = kImage.m_iWidth;

	color_rgba kBkColor, kCurrColor;
	kImage.get_pixel(0,0,kBkColor); 

	int sqr = 0;
	int cRowWidth = m_cCharCellSize*m_cCharsOneRow;
	for(int ry=0; ry<cRowWidth; ry+=m_cCharCellSize)
		for(int rx=0; rx<cRowWidth; rx+=m_cCharCellSize)
		{
			int min_x=m_cCharCellSize+1, max_x=0, min_y=m_cCharCellSize+1, max_y=0;
			for(int py=0;py<m_cCharCellSize;py++)
				for(int px=0;px<m_cCharCellSize;px++)
				{				
					if(bIsTGA == false)
						kImage.get_pixel(rx + px, ry + py,kCurrColor); 
					else
						kImage.get_pixel(rx + px, m_iBMPWidth-1-(ry + py),kCurrColor); 

					bool bBkColor = true; // vi antar att det är bakgrundfärg

					if(bIsTGA)
					{
						if(kCurrColor.a > 5)
							bBkColor = false;
					}
					else
					{
						//if(!(kCurrColor.r == 255 && kCurrColor.g == 255 && kCurrColor.b == 255)) // < 85)
						if(kCurrColor.r < 85)
							bBkColor = false;
					}

					if(!bBkColor && px > max_x)
						max_x=px;
					if(!bBkColor && px < min_x)
						min_x=px-1;
					if(!bBkColor && py > max_y)
						max_y=py;
					if(!bBkColor && py < min_y)
						min_y=py-1;
				}

			if(max_x && max_y)
			{
				m_aChars[sqr].iPosX = rx+min_x-1;
				m_aChars[sqr].iPosY = ry;//ry+min_y;
				m_aChars[sqr].iSizeX = max_x-min_x+3;
				m_aChars[sqr].iSizeY = m_cCharCellSize;//max_y-min_y+1;
			}
			else
			{
				m_aChars[sqr].iPosX = rx;
				m_aChars[sqr].iPosY = ry;
				m_aChars[sqr].iSizeX = m_cCharCellSize;
				m_aChars[sqr].iSizeY = m_cCharCellSize;
			}

			sqr++;

			if(sqr > 255)
				goto stop;
		}
		stop: { /*printf("jolly jumper\n");*/ }

	m_aChars[' '-32].iSizeX = m_aChars['t'-32].iSizeX;
	m_aChars[' '-32].iSizeY = m_cCharCellSize;

	if(pkFile != NULL)
		fclose(pkFile);

	return true;
}

unsigned short ZGuiFont::GetLength(const char *c_szText) const
{
	unsigned short usLength = 0;
	const int c_iTextLegth = strlen(c_szText);

	for(int i=0; i<c_iTextLegth; i++)
	{
		int pos = c_szText[i]-32;
		if(pos >= 0 && pos < 256)
		{
			usLength += m_aChars[pos].iSizeX;

			if(c_szText[i] != ' ')
				usLength += (unsigned short) m_cPixelGapBetweenChars;
		}
	}

	return usLength;
}
