// zguifont.cpp: implementation of the ZGuiFont class.
//
//////////////////////////////////////////////////////////////////////

#include "zguifont.h"
#include <memory.h>

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
	m_szFileName = "NONE";

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

bool ZGuiFont::CreateFromFile(char* strFileName)
{
	SDL_Surface *pkImage;	
	pkImage=IMG_Load(strFileName);
	if(!pkImage)
	{
		printf("font generation: Failed to find bitmap file [%s]!\n", strFileName);
		return false;
	}

	m_szFileName = string(strFileName);	
	m_iBMPWidth = pkImage->w;

	Uint32 bkcol=GetPixel(pkImage,0,0), pixel=0;

	int sqr = 0;
	int cRowWidth = m_cCharCellSize*m_cCharsOneRow;
	for(int ry=0; ry<cRowWidth; ry+=m_cCharCellSize)
		for(int rx=0; rx<cRowWidth; rx+=m_cCharCellSize)
		{
			int min_x=m_cCharCellSize+1, max_x=0, min_y=m_cCharCellSize+1, max_y=0;
			for(int py=0;py<m_cCharCellSize;py++)
				for(int px=0;px<m_cCharCellSize;px++)
				{				
					pixel=GetPixel(pkImage, rx + px, ry + py);			

					if(pixel!=bkcol && px > max_x)
						max_x=px;
					if(pixel!=bkcol && px < min_x)
						min_x=px-1;
					if(pixel!=bkcol && py > max_y)
						max_y=py;
					if(pixel!=bkcol && py < min_y)
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

//	delete pkImage;
	SDL_FreeSurface(pkImage);

	return true;
}

Uint32 ZGuiFont::GetPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/*
0    32   64 @ 96  ` 
1  . 33 ! 65 A 97  a 
2  . 34 " 66 B 98  b 
3  . 35 # 67 C 99  c 
4  . 36 $ 68 D 100 d 
5  . 37 % 69 E 101 e 
6  . 38 & 70 F 102 f 
7  . 39 ' 71 G 103 g 
8  . 40 ( 72 H 104 h 
9  . 41 ) 73 I 105 i 
10 . 42 * 74 J 106 j 
11 . 43 + 75 K 107 k 
12 . 44 , 76 L 108 l 
13 . 45 - 77 M 109 m 
14 . 46 . 78 N 110 n 
15 . 47 / 79 O 111 o 
16 . 48 0 80 P 112 p 
17 . 49 1 81 Q 113 q 
18 . 50 2 82 R 114 r 
19 . 51 3 83 S 115 s 
20 . 52 4 84 T 116 t 
21 . 53 5 85 U 117 u 
22 . 54 6 86 V 118 v 
23 . 55 7 87 W 119 w 
24 . 56 8 88 X 120 x 
25 . 57 9 89 Y 121 y 
26 . 58 : 90 Z 122 z 
27 . 59 ; 91 [ 123 { 
28 . 60 < 92 \ 124 |
29 . 61 = 93 ] 125 } 
30 . 62 > 94 ^ 126 ~ 
31 . 63 ? 95 _ 127 
*/
