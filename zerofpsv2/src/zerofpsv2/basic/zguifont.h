// zguifont.h: interface for the ZGuiFont class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASIC_ZGUIFONT_H_
#define _BASIC_ZGUIFONT_H_
 
#include <string>
#include "zfio.h"
#include "basic_x.h"
using namespace std;

//#include "zffile.h"

class BASIC_API ZGuiFont  
{
public:
	unsigned short GetLength(const char* c_szText) const;
	ZGuiFont(char m_sCharsOneRow=16, char cCharacterCellSize=16, 
		char cPixelGapBetweenChars=0, int iID=0);
	~ZGuiFont();

	struct CHARINFO
	{
		unsigned short iPosX;
		unsigned short iPosY;
		unsigned short iSizeX;
		unsigned short iSizeY;
	};

	CHARINFO m_aChars[256];
	int m_iBMPWidth;
	char m_cCharsOneRow; // Number of characters in rows and cols (must be same)
	char m_cCharCellSize;
	char m_cPixelGapBetweenChars;
	int m_iID; // used to compare with other fonts to prevent rebuilding
	string m_szFileName;
	int m_iType; // 0 = BMP, 1 = TGA

	bool CreateFromFile(char* szFileName);
};

#endif // !defined(AFX_ZGUIFONT_H__52B0ABD1_BF56_4D1D_A173_BB1CCEE56658__INCLUDED_)
