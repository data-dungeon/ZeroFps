// ZGuiSkin.cpp: implementation of the ZGuiSkin class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiskin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiSkin::ZGuiSkin(const long iBkTexID, const long iHorzBorderTexID, 
				   const long iVertBorderTexID, const long iBorderCornerTexID, 
				   const unsigned char byBkR, const unsigned char byBkG, const unsigned char byBkB,
				   const unsigned char byBorderR, const unsigned char byBorderG, const unsigned char byBorderB,
				   const unsigned short unBorderSize, const bool bTileBkSkin)
{
	m_afBkColor[0] = (1.0f/255)*byBkR;
	m_afBkColor[1] = (1.0f/255)*byBkG;
	m_afBkColor[2] = (1.0f/255)*byBkB;

	m_afBorderColor[0] = (1.0f/255)*byBorderR;
	m_afBorderColor[1] = (1.0f/255)*byBorderG;
	m_afBorderColor[2] = (1.0f/255)*byBorderB;

	m_iBkTexID = iBkTexID;
	m_iHorzBorderTexID = iHorzBorderTexID;
	m_iVertBorderTexID = iVertBorderTexID;
	m_iBorderCornerTexID = iBorderCornerTexID;

	m_unBorderSize = unBorderSize;

	m_bTileBkSkin = bTileBkSkin;
}

ZGuiSkin::ZGuiSkin(long iBkTexID, const bool bTileBkSkin)
{
	m_iBkTexID = iBkTexID;
	m_bTileBkSkin = bTileBkSkin;
	m_afBkColor[0] = m_afBkColor[1] = m_afBkColor[2] = 1;
	m_afBorderColor[0] = m_afBorderColor[1] = m_afBorderColor[2] = 1;
	m_iHorzBorderTexID = m_iVertBorderTexID = m_iBorderCornerTexID = -1;
	m_unBorderSize	= 0;
}

/*ZGuiSkin::ZGuiSkin(const unsigned char byBorderR, const unsigned char byBorderG, 
				   const unsigned char byBorderB, const unsigned short unBorderSize)
{
	m_afBorderColor[0] = (1.0f/255)*byBorderR;
	m_afBorderColor[1] = (1.0f/255)*byBorderG;
	m_afBorderColor[2] = (1.0f/255)*byBorderB;
	m_unBorderSize = unBorderSize;

	m_iBkTexID = -1;
	m_bTileBkSkin = false;
	m_afBkColor[0] = m_afBkColor[1] = m_afBkColor[2] = 1;
	m_iHorzBorderTexID = m_iVertBorderTexID = m_iBorderCornerTexID = -1;
}*/

ZGuiSkin::~ZGuiSkin()
{

}
