// zguifont.h: interface for the ZGuiFont class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASIC_ZGUIFONT_H_
#define _BASIC_ZGUIFONT_H_
 
#include <string>
#include "zfio.h"
#include "basic_x.h"
using namespace std;

class BASIC_API ZGuiFont
{
public:
	
	ZGuiFont(char* szName);
	~ZGuiFont();

	struct CHARINFO
	{
		unsigned short iPosX;
		unsigned short iPosY;
		unsigned short iSizeX;
		unsigned short iSizeY;
	};

	CHARINFO m_aChars[255];

	char m_szImageName[80]; // Name of the TGA image
	char m_szNameID[40]; // Font Identication
	int m_iTextureID;
	int m_iTextureWidth, m_iTextureHeight;
	int m_iRowHeight;	// Nr pixels each row
	int m_iPixelsAboveBaseLine; // pixels above the base line
	int m_iSpaceWidth; // Nr pixels a ' ' is
	int m_iNumLetters; // Nr of letters in bitmap font
	
	bool Create(char* szInfoFile, int iTexID);
	unsigned short GetLength(const char* c_szText, int iLength=-1) const;
};

#endif // !defined(AFX_ZGUIFONT_H__52B0ABD1_BF56_4D1D_A173_BB1CCEE56658__INCLUDED_)
