#include <windows.h>
#include <iostream>
#include "bitmapmanager.h"
#include <stdio.h>
#include <map>
#include <utility>
#include <math.h>
using namespace std;

BitmapManager::BitmapManager() : max_nyanses(16777215)
{							
	int max_width = 1024;
	int max_height = 1024;

	sortdata = new DATA[max_width*max_height];
	m_pImage8bit = new BYTE[max_width*max_height];
}

BitmapManager::~BitmapManager(void)
{
	delete[] sortdata;
}

int BitmapManager::SaveFile(FILE* pkFile, char* pData, int w, int h, bool bFormat8)
{
	width = w;
	height = h;

	if(bFormat8)
	{
		CreatePalette(pData);
		Create8bitImage(pData);
		
		fwrite(&palette_size, sizeof(unsigned int), 1, pkFile);
		fwrite(palette, sizeof(unsigned int), palette_size, pkFile);
		fwrite(m_pImage8bit, sizeof(unsigned char), width*height, pkFile);
	}
	else
	{
		fwrite(pData, width*height*3, 1, pkFile);
	}

	return true;
}

int BitmapManager::SaveFile24(const char* szFileName, const char* pData)
{
	FILE* pkFile = fopen(szFileName, "wb");

	int bpl = (width * 24) / 8;
	int rest=4-(bpl % 4);
	if (rest==4) rest=0;
	int BytesPerLine = bpl + rest;
	int sizepxldata = BytesPerLine * height;

	BITMAPFILEHEADER fh; 
	fh.bfOffBits = 54;
	fh.bfReserved1 = 0;
	fh.bfReserved2 = 0;
	fh.bfSize = 14 + 40 + sizepxldata;
	fh.bfType = 19778;

	BITMAPINFOHEADER ih;
	ih.biBitCount = 24;
	ih.biClrImportant = 0;
	ih.biClrUsed = 0;
	ih.biCompression = 0;
	ih.biHeight = height;
	ih.biPlanes = 1;
	ih.biSize = 40;
	ih.biSizeImage = 0;
	ih.biWidth = width;
	ih.biXPelsPerMeter = 0;
	ih.biYPelsPerMeter = 0;
	
	fwrite(&fh, sizeof(BITMAPFILEHEADER), 1, pkFile);
	fwrite(&ih, sizeof(BITMAPINFOHEADER), 1, pkFile);

	fwrite(pData, width*height*3, 1, pkFile);

	fclose(pkFile);
	return 0;
}

static int SortCB( const VOID* arg1, const VOID* arg2 )
{
    if (((DATA*)arg1)->freq < ((DATA*)arg2)->freq)
        return 1;
    return -1;
}

/*
int BitmapManager::SaveFile8(const char* szFileName, const char* pData, const int width, const int height)
{
	const int pixels = width*height;
	unsigned int r,g,b,color_key,freq,antal=0,oka=0,i;

	// Skapa en frekvenslista på alla färger.
	for(i=0; i<pixels; i++)
	{
		r = (unsigned char)pData[oka];
		g = (unsigned char)pData[oka+1];
		b = (unsigned char)pData[oka+2];
		oka+=3;

		r <<= 16;
		g <<= 8;

		color_key = r+g+b;
		freq = colorfreq[color_key] + 1;
		colorfreq[color_key] = freq;
	}

	// Se hur ofta varje färg förekommer.
	for(i=0; i<max_nyanses; i++)
	{
		if(colorfreq[i] != 0)
		{
			sortdata[antal].freq = colorfreq[i];
			sortdata[antal++].nyans = i;
			colorfreq[i] = 0; // tvätta bufferten
		}
	}

	qsort(sortdata, antal, sizeof(DATA), SortCB);
	
	PrintInfo(antal);

	return 0;
}*/

void BitmapManager::CreatePalette(const char* pData)
{
	const unsigned int bytes = width*height*3;
	unsigned int r,g,b,i;

	const c_Tol = 3;

	for(i=0; i<bytes; i+=3)
	{
		r = (unsigned char)pData[i+0];
		g = (unsigned char)pData[i+1];
		b = (unsigned char)pData[i+2];

	/*	r = rand() % 0xFF;
		g = rand() % 0xFF;
		b = rand() % 0xFF;*/
		
		g <<= 8;
		b <<= 16;

		r >>= c_Tol; // nytt
		g >>= /*c_Tol*/2;
		b >>= c_Tol;

		r &= 0xFF; // ny grej behövs förmodligen inte
		g &= 0xFF00; //
		b &= 0xFF0000; //

		r <<= c_Tol; // nytt
		g <<= /*c_Tol*/2;
		b <<= c_Tol;


		++freq_table[(unsigned int)(r+g+b)];
	}

	// add and sort
	map<unsigned long, unsigned long>::iterator freq_itor;
	multimap<unsigned long, unsigned long> color_table;
	for(freq_itor=freq_table.begin(); freq_itor!=freq_table.end(); ++freq_itor )
		color_table.insert( std::make_pair(freq_itor->second, freq_itor->first));

	// copy to sortdata...
	multimap<unsigned long, unsigned long>::iterator color_itor=color_table.end();
	const unsigned int size = color_table.size();
	for(i=0; i<size; i++ )
	{
		color_itor--;
		sortdata[i].freq = color_itor->first;
		sortdata[i].nyans = color_itor->second;	
	}

	freq_table.clear();

	// Skapa själva paletten
	palette_size = size;
	if(palette_size > 256)
		palette_size = 256;

	for(int i=0; i<palette_size; i++)
	{
		palette[i] = sortdata[i].nyans;
	}

	if(palette_size != 256){
		Beep(1000, 500);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
// Bestämma vilket index varje pixel skall ha i paletten.
//////////////////////////////////////////////////////////////////////////////////////
void BitmapManager::Create8bitImage(char* pData)
{
	int bytes = width*height*3;
	unsigned int i,j,oka=0, dif;
	int r,g,b,r2,g2,b2;
	unsigned int min_dist, min_dist_index;

	for(i=0; i<bytes; i+=3)
	{
		r = (BYTE) pData[i];
		g = (BYTE) pData[i+1];
		b = (BYTE) pData[i+2];

		//color = r + b + g; 

		min_dist = 0xFFFFFFFF;
		min_dist_index = 0;

		for(j=0; j<palette_size; j++)
		{
			r2 = palette[j];
			g2 = palette[j];
			b2 = palette[j];

			// move color channel to 1 byte
			g2 >>= 8;
			b2 >>= 16;

			r2 = r2 & 0xFF;
			g2 = g2 & 0xFF;
			b2 = b2 & 0xFF;

			dif = (r-r2)*(r-r2) + (g-g2)*(g-g2) + (b-b2)*(b-b2);
			//dif = (unsigned int) sqrt((double)dif);

			if(min_dist > dif)
			{
				min_dist = dif;
				min_dist_index = j;

				if(dif == 0)
					break;
			}
		}

		m_pImage8bit[oka++] = (BYTE) min_dist_index;
	}
}