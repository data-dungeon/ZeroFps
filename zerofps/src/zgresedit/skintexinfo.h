// skintexinfo.h: interface for the SkinTexInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINTEXINFO_H__D8672BC3_07C0_4062_90BD_7C21C9220FB8__INCLUDED_)
#define AFX_SKINTEXINFO_H__D8672BC3_07C0_4062_90BD_7C21C9220FB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory.h>
#include <string.h>
#include "../zerofps/basic/zguiskin.h"

#define MAX_FILENAME_LENGTH 35

class SkinTexInfo  
{
public:
	SkinTexInfo();

	// All textures, 9  parameters.
	SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK, 
		char* strTexHB, char* strTexVB, char* strTexCB,
		char* strTexBKa, char* strTexHBa, 
		char* strTexVBa, char* strTexCBa);

	// All solid textures, 5  parameters.
	SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK, 
		char* strTexHB, char* strTexVB, char* strTexCB);

	// bk texture with alpha. 3 parameters.
	SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK, char* strTexBKa);

	// Only bk texture, no alpha. 2 parameters
	SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK);

	// No textures (only colors), 1 parameter
	SkinTexInfo(ZGuiSkin* pkSkin);

	~SkinTexInfo();

	char m_strTexBK[MAX_FILENAME_LENGTH];	// Name of the background texture
	char m_strTexHB[MAX_FILENAME_LENGTH];	// Name of the texture for the horizontal border
	char m_strTexVB[MAX_FILENAME_LENGTH];	// Name of the texture for the vertical border
	char m_strTexCB[MAX_FILENAME_LENGTH];	// Name of the texture for the corner border

	char m_strTexBKa[MAX_FILENAME_LENGTH];	// Name of the background alpha texture
	char m_strTexHBa[MAX_FILENAME_LENGTH];	// Name of the alpha texture for the horizontal border
	char m_strTexVBa[MAX_FILENAME_LENGTH];	// Name of the alpha texture for the vertical border
	char m_strTexCBa[MAX_FILENAME_LENGTH];	// Name of the alpha texture for the corner border

	ZGuiSkin* m_pkSkin;
};

#endif // !defined(AFX_SKINTEXINFO_H__D8672BC3_07C0_4062_90BD_7C21C9220FB8__INCLUDED_)
