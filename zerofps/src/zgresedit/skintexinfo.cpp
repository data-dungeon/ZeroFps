// skintexinfo.cpp: implementation of the SkinTexInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "skintexinfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkinTexInfo::SkinTexInfo()
{
	memset(m_strTexBK,0,MAX_FILENAME_LENGTH);
	memset(m_strTexHB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexVB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexCB,0,MAX_FILENAME_LENGTH);

	memset(m_strTexBKa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexHBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexVBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexCBa,0,MAX_FILENAME_LENGTH);	

	m_pkSkin = NULL;		
}

// All textures, 9  parameters.
SkinTexInfo::SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK, char* strTexHB, 
						 char* strTexVB, char* strTexCB, char* strTexBKa, 
						 char* strTexHBa, char* strTexVBa, char* strTexCBa)
{
	if(strTexBK != NULL) strcpy(m_strTexBK,strTexBK);	
	if(strTexHB != NULL) strcpy(m_strTexHB,strTexHB);	
	if(strTexVB != NULL) strcpy(m_strTexVB,strTexVB);	
	if(strTexCB != NULL) strcpy(m_strTexCB,strTexCB);	

	if(strTexBKa != NULL) strcpy(m_strTexBKa,strTexBKa);	
	if(strTexHBa != NULL) strcpy(m_strTexHBa,strTexHBa);	
	if(strTexVBa != NULL) strcpy(m_strTexVBa,strTexVBa);	
	if(strTexCBa != NULL) strcpy(m_strTexCBa,strTexCBa);	

	m_pkSkin = pkSkin;
}

// Only solid textures, 5 parameters
SkinTexInfo::SkinTexInfo(ZGuiSkin* pkSkin,
						 char* strTexBK, char* strTexHB,
						 char* strTexVB, char* strTexCB)
{
	if(strTexBK != NULL) strcpy(m_strTexBK,strTexBK);	
	if(strTexHB != NULL) strcpy(m_strTexHB,strTexHB);	
	if(strTexVB != NULL) strcpy(m_strTexVB,strTexVB);	
	if(strTexCB != NULL) strcpy(m_strTexCB,strTexCB);	
	
	memset(m_strTexBKa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexHBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexVBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexCBa,0,MAX_FILENAME_LENGTH);

	m_pkSkin = pkSkin;
}

// bk texture with alpha. 3 parameters.
SkinTexInfo::SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK, char* strTexBKa)
{
	if(strTexBK != NULL) strcpy(m_strTexBK,strTexBK);
	if(strTexBKa != NULL) strcpy(m_strTexBKa,strTexBKa);
	
	memset(m_strTexHB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexVB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexCB,0,MAX_FILENAME_LENGTH);

	memset(m_strTexHBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexVBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexCBa,0,MAX_FILENAME_LENGTH);	

	m_pkSkin = pkSkin;
}

// Only bk texture, no alpha. 2 parameters
SkinTexInfo::SkinTexInfo(ZGuiSkin* pkSkin, char* strTexBK)
{
	if(strTexBK != NULL) strcpy(m_strTexBK,strTexBK);
	
	memset(m_strTexHB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexVB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexCB,0,MAX_FILENAME_LENGTH);

	memset(m_strTexBKa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexHBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexVBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexCBa,0,MAX_FILENAME_LENGTH);

	m_pkSkin = pkSkin;
}

// No textures (only colors), 1 parameter
SkinTexInfo::SkinTexInfo(ZGuiSkin* pkSkin)
{

	memset(m_strTexBK,0,MAX_FILENAME_LENGTH);
	memset(m_strTexHB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexVB,0,MAX_FILENAME_LENGTH);
	memset(m_strTexCB,0,MAX_FILENAME_LENGTH);

	memset(m_strTexBKa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexHBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexVBa,0,MAX_FILENAME_LENGTH);	
	memset(m_strTexCBa,0,MAX_FILENAME_LENGTH);

	m_pkSkin = pkSkin;
}

SkinTexInfo::~SkinTexInfo()
{

}
