#pragma once

#pragma warning (disable: 4786)

#include <stdio.h>
#include <map>
using namespace std;

struct DATA
{
	unsigned int freq;
	unsigned int nyans;
};

class BitmapManager
{
public:
	BitmapManager();
	~BitmapManager(void);
	int SaveFile(FILE* pkFile, char* data, int w, int h, bool bFormat8);

private:
	int SaveFile24(const char* szFileName, const char* data);
	int SaveFile8(const char* szFileName, const char* data);
	int SaveFile8v2(const char* szFileName, const char* data);

	map<unsigned long, unsigned long> freq_table;
	void CreatePalette(const char* pData);
	void Create8bitImage(char* pData);

	const max_nyanses;
	unsigned short* colorfreq;
	DATA* sortdata;
	unsigned int palette[256];
	unsigned int palette_size;

	int width, height;

	BYTE* m_pImage8bit;
};


/*
int BitmapManager::SaveFile8(const char* szFileName, const char* pData, 
							 const int width, const int height)
{
	const unsigned int bytes = width*height*3;
	unsigned int r,g,b,i;

	for(i=0; i<bytes; i+=3)
	{
		r = (unsigned char)pData[i];
		g = (unsigned char)pData[i+1];
		b = (unsigned char)pData[i+2];

		r <<= 16;
		g <<= 8;

		++freq_table[(unsigned int)(r+g+b)];
	}

	// add and sort
	i=0;
	map<unsigned long, unsigned long>::iterator freq_itor;
	for(freq_itor=freq_table.begin(); freq_itor!=freq_table.end(); ++freq_itor )
	{
		sortdata[i].freq = freq_itor->second;
		sortdata[i++].nyans = freq_itor->first;	
	}

	qsort(sortdata, i, sizeof(DATA), SortCB);

	PrintInfo(i);
	freq_table.clear();

	return 0;
}*/

/*
void BitmapManager::SaveBitmap(char* pData)
{
	int bytes = width*height*3;
	unsigned int r,g,b,i,j=0,color;

	for(i=0; i<bytes; i+=3)
	{
		int palette_idex = m_pImage8bit[j++];
		//palette_idex = rand()%0xFF;
		
		r = palette[palette_idex];
		g = palette[palette_idex];
		b = palette[palette_idex];

		g >>= 8;
		b >>= 16;

		r = r & 0xFF;
		g = g & 0xFF;
		b = b & 0xFF;

		pData[i] = b;
		pData[i+1] = g;
		pData[i+2] = r;
	}

	char neam[50];
	sprintf(neam, "ZZZframe8bit%i", FRAME);

	//FILE* pkFile = fopen(neam, "wb");
	fwrite(pData, sizeof(char), bytes, g_kFile);
	//fclose(pkFile);
}
*/