// zguifont.cpp: implementation of the ZGuiFont class.
//
//////////////////////////////////////////////////////////////////////

#include "zguifont.h"
#include <memory.h>
#include "image.h"
#include "zfvfs.h"

ZGuiFont::ZGuiFont(char* szName)
{
	memset(m_aChars, 0, 255*sizeof(CHARINFO));
	strcpy(m_szNameID, szName);
	strcpy(m_szImageName, "");
}

ZGuiFont::~ZGuiFont()
{

}

bool ZGuiFont::Create(char* szInfoFile, int iTexID, int iGlyphWidth)
{
	ZFVFile kFile;
	if(!kFile.Open(szInfoFile,0,false))
	{
		printf("Failed to open info [%s] file for creating font\n", szInfoFile);
		return false;
	}

	m_iTextureID = iTexID;

	fread(&m_iTextureWidth, sizeof(int), 1, kFile.m_pkFilePointer);
	fread(&m_iTextureHeight, sizeof(int), 1, kFile.m_pkFilePointer);
	fread(&m_iRowHeight, sizeof(int), 1, kFile.m_pkFilePointer);
	fread(&m_iPixelsAboveBaseLine, sizeof(int), 1, kFile.m_pkFilePointer); 
	fread(&m_iSpaceWidth, sizeof(int), 1, kFile.m_pkFilePointer);
	fread(&m_iNumLetters, sizeof(int), 1, kFile.m_pkFilePointer);

	if(m_iNumLetters < 1 || m_iNumLetters > 255)
	{
		printf("Failed to create font\n");
		return false;
	}

	int* piLettersRect = new int[4*m_iNumLetters];
	fread(piLettersRect, sizeof(int)*4, m_iNumLetters, kFile.m_pkFilePointer);

	CHARINFO undef =
	{
		piLettersRect[(m_iNumLetters-1)*4], 
		piLettersRect[(m_iNumLetters-1)*4+1],
		piLettersRect[(m_iNumLetters-1)*4+2]-piLettersRect[(m_iNumLetters-1)*4], 
		piLettersRect[(m_iNumLetters-1)*4+3]-piLettersRect[(m_iNumLetters-1)*4+1]
	};

	for(int i=0; i<255; i++)
	{
		m_aChars[i].iPosX = undef.iPosX; m_aChars[i].iPosY = undef.iPosY;
		m_aChars[i].iSizeX = undef.iSizeX; m_aChars[i].iSizeY = undef.iSizeY;
	}

	static char aLetters[] =
	{
		'!','"','#','$','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8',
		'9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
		'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','a','b','c','d','e','f','g',
		'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','}','|',''
	};

	for(int i=0; i<m_iNumLetters; i++)
	{
		int index = (int)aLetters[i];
		m_aChars[index].iPosX = piLettersRect[i*4]-iGlyphWidth;
		m_aChars[index].iPosY = piLettersRect[i*4+1];
		m_aChars[index].iSizeX = piLettersRect[i*4+2]-m_aChars[index].iPosX+iGlyphWidth;
		m_aChars[index].iSizeY = piLettersRect[i*4+3]-m_aChars[index].iPosY;
	}

	m_aChars[' '].iSizeX = m_iSpaceWidth;
	m_aChars['\t'].iSizeX = 3*m_iSpaceWidth;
	m_aChars['\n'].iSizeX = 0;
	m_aChars['\0'].iSizeX = 0;
	
	return true;
}

unsigned short ZGuiFont::GetLength(const char* c_szText, int iLength) const
{
	unsigned short usLength = 0;

	if(iLength == -1)
		iLength = strlen(c_szText);

	for(int i=0; i<iLength; i++)
	{
		int pos = c_szText[i];
		if(pos >= 0 && pos < 256)
		{
			usLength += m_aChars[pos].iSizeX;
		}
	}

	return usLength;
}
