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
	ZGuiSkin( const long iBkTexID, const bool bTileBkSkin);
	ZGuiSkin( const long iBkTexID=-1, long iHorzBorderTex=-1, const long iVertBorderTex=-1, 
			  const long iBorderCornerTex=-1, const unsigned char byBkR=255, 
			  const unsigned char byBkG=255, const unsigned char byBkB=255,
			  const unsigned char byBorderR=255, const unsigned char byBorderG=255, 
			  const unsigned char byBorderB=255, const unsigned short unBorderSize=0, 
			  const bool bTileBkSkin = false); 
/*	ZGuiSkin::ZGuiSkin(const unsigned char byBorderR, const unsigned char byBorderG, 
				   const unsigned char byBorderB, const unsigned short unBorderSize);*/
	~ZGuiSkin();

	long m_iBkTexID; // -1 innebär att det inte finns nån textur.
	long m_iHorzBorderTexID; // -1 innebär att det inte finns nån textur.
	long m_iVertBorderTexID; // -1 innebär att det inte finns nån textur.
	long m_iBorderCornerTexID; // -1 innebär att det inte finns nån textur.
	float m_afBkColor[3];
	float m_afBorderColor[3];
	unsigned short m_unBorderSize; // Bredden och höjden på ramen kring fönstret.

	bool m_bTileBkSkin;
};

#endif // !defined(AFX_ZGUISKIN_H__E13E6AC7_8EAE_468F_A100_74C5559BE66E__INCLUDED_)
