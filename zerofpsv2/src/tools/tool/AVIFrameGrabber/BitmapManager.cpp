#include "bitmapmanager.h"

BitmapManager::BitmapManager()
{							
	const int max_width = 1024;
	const int max_height = 1024;

	m_pkSortData8bit = new SORT_DATA_RGB8[max_width*max_height];
	m_pImage8bit = new BYTE[max_width*max_height];
	m_pkJPGEncoder = new JpgEncoder();
}

BitmapManager::~BitmapManager(void)
{
	delete[] m_pkSortData8bit;
	delete[] m_pImage8bit;
	delete m_pkJPGEncoder;
}

int BitmapManager::SaveFile(FILE* pkFile, char* pData, int w, int h, ImageFormat eFormat)
{
	m_iWidth = w;
	m_iHeight = h;

	if(eFormat == RGB24)
	{
		fwrite(pData, m_iWidth*m_iHeight*3, 1, pkFile);
	}
	else
	if(eFormat == RGB8)
	{
		CreatePalette(pData);
		Create8bitImage(pData);
		
		fwrite(&m_iPaletteSize, sizeof(unsigned int), 1, pkFile);
		fwrite(m_akPalette, sizeof(unsigned int), m_iPaletteSize, pkFile);
		fwrite(m_pImage8bit, sizeof(unsigned char), m_iWidth*m_iHeight, pkFile);
	}
	else
	if(eFormat == JPEG)
	{		
		m_pkJPGEncoder->SaveData(pkFile, (JpgEncoder::colorRGB*)pData, w, h);
		//fwrite(m_pImage8bit, sizeof(unsigned char), after-before, pkFile);
		fseek(pkFile, 0, SEEK_END);
	}
	
	return true;
}

typedef struct s_RGB {
			 BYTE B;
		     BYTE G;
		     BYTE R;
} RGB;

void BitmapManager::SaveFile24(const char* szFileName, void* pData, int w, int h) const
{
	FILE* pkFile = fopen(szFileName, "wb");

	int bpl = (w * 24) / 8;
	int rest=4-(bpl % 4);
	if (rest==4) rest=0;
	int BytesPerLine = bpl + rest;
	int sizepxldata = BytesPerLine * h;

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
	ih.biHeight = h;
	ih.biWidth = w;
	ih.biPlanes = 1;
	ih.biSize = 40;
	ih.biSizeImage = 0;
	ih.biXPelsPerMeter = 0;
	ih.biYPelsPerMeter = 0;
	
	fwrite(&fh, sizeof(BITMAPFILEHEADER), 1, pkFile);
	fwrite(&ih, sizeof(BITMAPINFOHEADER), 1, pkFile);

	//fwrite(pData, w*h*3, 1, pkFile);

	//int x,y,i;
	//RGB *pixel;
	//BYTE zero_byte=0;
	//DWORD im_loc_bytes=(DWORD)pData+((DWORD)h-1)*w*4;

	//for (y=0;y<h;y++)
	//{
	//	for (x=0;x<w;x++)
	//	{
	//		pixel=(RGB *)im_loc_bytes;
	//		fwrite(pixel, 3, 1, pkFile);
	//		im_loc_bytes+=4;
	//	}
	//	for (i=0;i<rest;i++)
	//		fwrite(&zero_byte,1,1,pkFile);

	//	im_loc_bytes-=2L*w*4;
	//}

	int x,y,i;
	RGB *pixel;
	BYTE zero_byte=0;
	DWORD im_loc_bytes=(DWORD)pData;

	for (y=0;y<h;y++)
	{
		for (x=0;x<w;x++)
		{
			pixel=(RGB *)im_loc_bytes;
			fwrite(pixel, 3, 1, pkFile);
			im_loc_bytes+=4;
		}

		for (i=0;i<rest;i++)
			fwrite(&zero_byte,1,1,pkFile);
	}   

	fclose(pkFile);
}

void BitmapManager::CreatePalette(const char* pData)
{
	const unsigned int bytes = m_iWidth*m_iHeight*3;
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


		++m_kFreqTable[(unsigned int)(r+g+b)];
	}

	// add and sort
	map<unsigned long, unsigned long>::iterator freq_itor;
	multimap<unsigned long, unsigned long> color_table;
	for(freq_itor=m_kFreqTable.begin(); freq_itor!=m_kFreqTable.end(); ++freq_itor )
		color_table.insert( std::make_pair(freq_itor->second, freq_itor->first));

	// copy to m_pkSortData8bit...
	multimap<unsigned long, unsigned long>::iterator color_itor=color_table.end();
	const unsigned int size = (unsigned int) color_table.size();
	for(i=0; i<size; i++ )
	{
		color_itor--;
		m_pkSortData8bit[i].freq = color_itor->first;
		m_pkSortData8bit[i].nyans = color_itor->second;	
	}

	m_kFreqTable.clear();

	// Skapa själva paletten
	m_iPaletteSize = size;
	if(m_iPaletteSize > 256)
		m_iPaletteSize = 256;

	for(int i=0; i<m_iPaletteSize; i++)
		m_akPalette[i] = m_pkSortData8bit[i].nyans;
}

void BitmapManager::Create8bitImage(char* pData)
{
	int bytes = m_iWidth*m_iHeight*3;
	unsigned int i,j,oka=0, dif;
	int r,g,b,r2,g2,b2;
	unsigned int min_dist, min_dist_index;

	for(i=0; i<bytes; i+=3)
	{
		r = (BYTE) pData[i];
		g = (BYTE) pData[i+1];
		b = (BYTE) pData[i+2];

		min_dist = 0xFFFFFFFF;
		min_dist_index = 0;

		for(j=0; j<m_iPaletteSize; j++)
		{
			r2 = m_akPalette[j];
			g2 = m_akPalette[j];
			b2 = m_akPalette[j];

			// move color channel to 1 byte
			g2 >>= 8;
			b2 >>= 16;

			r2 = r2 & 0xFF;
			g2 = g2 & 0xFF;
			b2 = b2 & 0xFF;

			dif = (r-r2)*(r-r2) + (g-g2)*(g-g2) + (b-b2)*(b-b2);

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

