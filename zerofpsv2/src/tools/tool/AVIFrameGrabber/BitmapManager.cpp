#include <windows.h>
#include <iostream>
#include "bitmapmanager.h"

BitmapManager::BitmapManager(void)
{
}

BitmapManager::~BitmapManager(void)
{
}

int BitmapManager::SaveFile(const char* szFileName, char* pData, int width, int height)
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
