#pragma once

#pragma warning (disable: 4786)

#include <windows.h>
#include <stdio.h>
#include <map>
#include "jpegenc.h"
using namespace std;

enum ImageFormat
{
	RGB24=0,
	RGB8=1,
	JPEG=2,
};

class BitmapManager
{
public:
	BitmapManager();
	~BitmapManager(void);
	int SaveFile(FILE* pkFile, char* data, int w, int h, ImageFormat eFormat);
	void SaveFile24(const char* szFileName, void* pData, int w, int h) const;

private:
	
	void CreatePalette(const char* pData);
	void Create8bitImage(char* pData);

	struct SORT_DATA_RGB8
	{
		unsigned int freq;
		unsigned int nyans;
	};

	JpgEncoder* m_pkJPGEncoder;
	SORT_DATA_RGB8* m_pkSortData8bit;
	BYTE* m_pImage8bit;
	unsigned int m_akPalette[256];
	map<unsigned long, unsigned long> m_kFreqTable;
	unsigned int m_iPaletteSize;

	int m_iWidth, m_iHeight;
};