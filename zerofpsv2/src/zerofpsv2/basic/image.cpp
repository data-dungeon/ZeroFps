//#include "core.h"
#include <memory.h> 
#include <string.h> 
#include "image.h"
#include <iostream>

using namespace std;

int DEBUG_PRINT = false;			// Sätt till true för att printa bitmap/tga/pcx/whatever loaded.
										
// Defines
#define BITMAP_ID				0x4D42	// universal id for a bitmap
#define PC_NOCOLLAPSE			0x04    /* do not match color to system kPalette */

// TGA FORMAT
#define TGA_IMAGETYPE_NONE		0		// No image data included.
#define TGA_IMAGETYPE_UMAP		1		// Uncompressed, color-mapped images.
#define TGA_IMAGETYPE_URGB		2		// Uncompressed, RGB images.
#define TGA_IMAGETYPE_UBW		3		// Uncompressed, black and white images.
#define TGA_IMAGETYPE_RLEMAP	9		// Runlength encoded color-mapped images.
#define TGA_IMAGETYPE_RLERGB	10		// Runlength encoded RGB images.
#define TGA_IMAGETYPE_CBW		11		// Compressed, black and white images.
#define TGA_IMAGETYPE_CMAP		32		// Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
#define TGA_IMAGETYPE_CMAP2		33		// Compressed color-mapped data, using Huffman, Delta, and
										// runlength encoding.  4-pass quadtree-type process.
enum IMG_FORMAT {
	IMG_FORMAT_RGB,
	IMG_FORMAT_RGBA,
	IMG_FORMAT_ALPHA,
};


// Structures	************************************
#pragma pack( 1 )

/// File Header in TGA Files.
struct tgahead_t 
{
	char	id_length;					///< Num of bytes with extra info.
	char	colormap_type;	 		 	///< 0 = No color map, 1 = color map.
	char	image_type;					///< A TGA_IMAGETYPE_XXX value.

	// Color map spec.
	short	colormap_first;				
	short	colormap_length;
	char	colormap_size;

	short	xorgin;
	short	yorgin;
	short	width;
	short	height;
	char	pixel_depth;				///< Bits per pixel (8,16,24,32).
	char	image_desc;
	
};


/// File Header in PCX Files.
struct pcx_header_s
{
	char	manufacturer;
	char	version;
	char	encoding;
	char	bits_per_pixel;
   short	xmin;
	short	ymin;
	short	xmax;
	short	ymax;
	short	hres;
	short	vres;
	char	palette[48];
	char	reserved;
	char	color_planes;
	short	bytes_per_line;
	short	palette_type;
	char	filler[58];
};

/// Bmp pallete Entry.
struct bmppal_t 
{ 
	unsigned char ucRed; 
	unsigned char ucGreen; 
	unsigned char ucBlue; 
	unsigned char ucFlags; 
}; 

/// Bmp Header.
struct bmpheader_t 
{ 
	unsigned short	usType; 
	unsigned long	ulSize; 
	unsigned short	usReserved1; 
	unsigned short	usReserved2; 
	unsigned long	ulOffBits; 
};

/// Bmp Info Block.
struct bmpinfo_t
{ 
    unsigned long	ulSize; 
    long			lWidth; 
    long			lHeight; 
    unsigned short	usPlanes; 
    unsigned short	usBitCount;
    unsigned long	ulCompression; 
    unsigned long	ulSizeImage; 
    long			lXPelsPerMeter; 
    long			lYPelsPerMeter; 
    unsigned long	ulClrUsed; 
    unsigned long	ulClrImportant; 
}; 

struct bmp_t
{
	bmpheader_t		kFileheader;  
	bmpinfo_t		kInfoheader;
	bmppal_t		kPalette[256];      
	unsigned char	*pkData;           
};

#pragma pack(  )


















static unsigned char getbyte (char*& ptr)
{
  unsigned char c = *ptr++;
  return c;
}

int getsize(int insize)
{
	if(insize >= 256)					return 256;
	if(insize >= 128 && insize < 256)	return 128;
	if(insize >= 64 && insize < 128)	return 64;
	if(insize >= 32 && insize < 64)		return 32;
	return 16;
}

Image::Image()
{
	memset(this,0,sizeof(Image));
}

Image::Image( const Image& v )
{
	m_pkPixels = NULL;
	m_iWidth = m_iHeight = 0;
	if(v.m_pkPixels == NULL)	return;

	m_iWidth		= v.m_iWidth;
	m_iHeight	= v.m_iHeight;
	m_bHasAlpha = v.m_bHasAlpha;
	m_pkPixels = new color_rgba [m_iWidth * m_iHeight]; 
	memcpy(m_pkPixels,v.m_pkPixels,(m_iWidth * m_iHeight) * sizeof(color_rgba));
}

Image::~Image()
{
	if(m_pkPixels)	delete [] m_pkPixels;
	m_pkPixels = NULL;
	m_iWidth = m_iHeight = 0;
}

void Image::Free(void)
{
	if(m_pkPixels)	delete [] m_pkPixels;
	m_pkPixels = NULL;
	m_iWidth = m_iHeight = 0;
	m_bHasAlpha = false;
}

void Image::CreateEmpty(int iSetWidth, int iSetHeight)
{
	Free();
	m_iWidth  = iSetWidth;
	m_iHeight = iSetHeight;
	m_bHasAlpha = false;
	m_pkPixels = new color_rgba [m_iWidth * m_iHeight];
}


// TGA FORMAT
void Image::ReadPixel(color_rgba* pkDst, char* pcSrc, int iPixsize)
{
	char red,grn,blu,alp;
	char j,k; 
	red = grn = blu = alp = 0;
	char *data = pcSrc;

	switch(iPixsize) {
		case 8:		
			break;
		
		case 16:
			j = getbyte( data );
			k = getbyte( data );
			red = ( k & 0x7C ) >> 2;
			grn = ( ( k & 0x03 ) << 3 ) + ( ( j & 0xE0 ) >> 5 );
			blu = j & 0x1F;
			break;

		case 24:
			blu = getbyte(data);
			grn = getbyte(data);
			red = getbyte(data);
			break;
		
		case 32:	
			blu = getbyte(data);
			grn = getbyte(data);
			red = getbyte(data);
			alp = getbyte(data);
			break;
		}

	pkDst->r = red;
	pkDst->g = grn;
	pkDst->b = blu;
	pkDst->a = alp;
}

void Image::ReadRgb(color_rgba* pkDst, FILE* fp, int iPixsize)
{
	char red,grn,blu,alp;
	char j,k; 
	red = grn = blu = alp = 0;

	switch(iPixsize) {
		case 8:		
			red = grn = blu = getc(fp);
			break;
		
		case 16:
			j = getc(fp);
			k = getc(fp);
			red = ( k & 0x7C ) >> 2;
			grn = ( ( k & 0x03 ) << 3 ) + ( ( j & 0xE0 ) >> 5 );
			blu = j & 0x1F;
			break;

		case 24:
			blu = getc(fp);
			grn = getc(fp);
			red = getc(fp);
			break;
		
		case 32:	
			blu = getc(fp);
			grn = getc(fp);
			red = getc(fp);
			alp = getc(fp);
			break;
		}

	pkDst->r = red;
	pkDst->g = grn;
	pkDst->b = blu;
	pkDst->a = alp;
}

void Image::Save(const char* szFileName, bool bAlpha)
{
	tgahead_t head;
	memset(&head,0,sizeof(tgahead_t));

	head.id_length = 0;
	head.colormap_type = 0;
	head.image_type = TGA_IMAGETYPE_URGB;

	head.width  = m_iWidth;
	head.height = m_iHeight;

	if(m_bHasAlpha || bAlpha)	head.pixel_depth = 32;
		else	head.pixel_depth = 24;

	FILE *fp = fopen(szFileName,"wb");
	if(!fp)	return;

	fwrite(&head, sizeof(tgahead_t),1,fp);

	for(int i=0; i<m_iWidth * m_iHeight; i++) {
		putc(m_pkPixels[i].b,fp);
		putc(m_pkPixels[i].g,fp);
		putc(m_pkPixels[i].r,fp);
		if(bAlpha)
			putc(m_pkPixels[i].a,fp);
		}

	fclose(fp);
}

bool Image::load_tga(const char* filename)
{
	FILE *fp = fopen(filename,"rb");
	if(!fp)	return false;
	load_tga(fp);
	fclose(fp);
	return true;

/*	char *data = NULL;
	color_rgba* map = NULL;
	int m_pkPixelsize;

	FILE *fp = fopen(filename,"rb");
	
	if(!fp)	return false;

	tgahead_t head;
	fread(&head,sizeof(tgahead_t),1,fp);

	// Check that it's a tga file and that it is one that we support.
	if(head.image_type != TGA_IMAGETYPE_URGB)	return false;
	
	// Set basic image prop.
	width  = head.width;
	height = head.height;

	if(head.pixel_depth == 8)	m_pkPixelsize = 1;
	if(head.pixel_depth == 16)	m_pkPixelsize = 2;
	if(head.pixel_depth == 24)	m_pkPixelsize = 3;
	if(head.pixel_depth == 32)	m_pkPixelsize = 4;

	// Read color map if there is one.
	if(head.colormap_type != 0) {
		// Get color map memory.
		map = new color_rgba [head.colormap_length];
		for(int i = 0; i<head.colormap_length; i++)	
			read_rgb(&map[i], fp, head.colormap_size);
		}

	// Alloc pixel memory.
	m_pkPixels = new color_rgba [width * height];
	
	data = new char [width*height*m_pkPixelsize];
	fread(data,sizeof(char),width * height * m_pkPixelsize,fp);

	for(int i=0; i<width * height; i++) {
		if(head.image_type == TGA_IMAGETYPE_URGB)	read_pixel(&m_pkPixels[i], &data[i*m_pkPixelsize], head.pixel_depth);
		if(head.image_type == TGA_IMAGETYPE_UMAP)	{
			m_pkPixels[i] = map [ data[i] ];
			}
		}
	
	if(data)	delete [] data;
	if(map)		delete [] map;

	fclose(fp);
	return true;
*/
}

#define BIT_0	0x00000001
#define BIT_1	0x00000002
#define BIT_2	0x00000004
#define BIT_3	0x00000008
#define BIT_4	0x00000010
#define BIT_5	0x00000020
#define BIT_6	0x00000040
#define BIT_7	0x00000080
#define BIT_8	0x00000100

bool Image::load_tga(FILE *fp)
{
	char *data = NULL;
	color_rgba* map = NULL;
	int m_pkPixelsize;

	if(!fp)	return false;

	tgahead_t head;
	fread(&head,sizeof(tgahead_t),1,fp);

	// Check that it's a tga file and that it is one that we support.
//	if(head.image_type != TGA_IMAGETYPE_URGB)	return false;
	
	// Set basic image prop.
	m_bHasAlpha	   = false;
	m_iWidth			= head.width;
	m_iHeight		= head.height;

	if(head.pixel_depth == 8)	m_pkPixelsize = 1;
	if(head.pixel_depth == 16)	m_pkPixelsize = 2;
	if(head.pixel_depth == 24)	m_pkPixelsize = 3;
	if(head.pixel_depth == 32)	{
		m_pkPixelsize = 4;
		m_bHasAlpha = true;
		}

	if(head.id_length != 0) {
		fseek(fp, head.id_length, SEEK_CUR);
		}

	// Read color map if there is one.
	if(head.colormap_type != 0) {
		// Get color map memory.
		map = new color_rgba [head.colormap_length];
		for(int i = 0; i<head.colormap_length; i++)	
			ReadRgb(&map[i], fp, head.colormap_size);
		}

	// Alloc pixel memory.
	m_pkPixels = new color_rgba [m_iWidth * m_iHeight];
	
	data = new char [m_iWidth*m_iHeight*m_pkPixelsize];
	fread(data,sizeof(char),m_iWidth * m_iHeight * m_pkPixelsize,fp);

	if(head.image_type == TGA_IMAGETYPE_URGB || head.image_type == TGA_IMAGETYPE_UMAP) {
		for(int i=0; i<m_iWidth * m_iHeight; i++) {
			if(head.image_type == TGA_IMAGETYPE_URGB)	ReadPixel(&m_pkPixels[i], &data[i*m_pkPixelsize], head.pixel_depth);
			if(head.image_type == TGA_IMAGETYPE_UMAP)	{
				m_pkPixels[i] = map [ data[i] ];

				}
			}
	}

	if(head.image_type == TGA_IMAGETYPE_RLERGB) {
		int iPixelOffset	= 0;
		int iDataOffset	= 0;
		unsigned char ucPacketValue;
		unsigned int iNumOfRLE;
		color_rgba		kColor;
		unsigned int i;

		while(iPixelOffset < (m_iWidth * m_iHeight)) {
			ucPacketValue =  data[iDataOffset++];
			
			if(ucPacketValue & BIT_7) {	// Rle
				iNumOfRLE = ucPacketValue - BIT_7;
				iNumOfRLE += 1;
				ReadPixel(&kColor, &data[iDataOffset], head.pixel_depth);
				iDataOffset += m_pkPixelsize;

				for(i=0; i<iNumOfRLE; i++)
					m_pkPixels[iPixelOffset++] = kColor;
				}
			else {	// Raw
				iNumOfRLE = ucPacketValue + 1;
				for(i=0; i<iNumOfRLE; i++) {
					ReadPixel(&kColor, &data[iDataOffset], head.pixel_depth);
					m_pkPixels[iPixelOffset++] = kColor;
					iDataOffset += m_pkPixelsize;
					}
				}
			}
		}

	if(data)	delete [] data;
	if(map)		delete [] map;
	return true;
}

bool Image::load_pcx(const char* filename)
{
	FILE *fp = fopen(filename,"rb");
	if(!fp)	return false;
	load_pcx(fp, NULL);
	fclose(fp);
	return true;
}

bool Image::load_pcx(FILE *fp, color_rgb* pal)
{
	color_rgb kPalette[768];
	int i;

	pcx_header_s head;
	fread(&head,sizeof(pcx_header_s),1,fp);

	m_bHasAlpha = false;
	// Alloc image mem.
	m_iWidth  = head.xmax + 1;
	m_iHeight = head.ymax + 1;
	int img_size = m_iWidth * m_iHeight;

	m_pkPixels = new color_rgba [img_size];
	unsigned char *data1 = new unsigned char [img_size];

	// Load and unpack image.
	int count = 0;
	int readc;
	int num_bytes;
	
	while(count <= img_size) {	
		readc = getc(fp);

		// Check if RLE
		if(readc > 192 && readc <= 255) {
			num_bytes = readc - 192;	// Get nr of bytes in rle.
			readc = getc(fp);			// Get data in rle.
			while(num_bytes-- > 0)	data1[count++] = readc;
			 }
		 
		else	data1[count++] = readc;
		}

	// Read kPalette from pcx image.
	fseek(fp, -768L, SEEK_END);
	for(i=0; i<256; i++) {
		kPalette[i].r = (getc(fp));
		kPalette[i].g = (getc(fp));
		kPalette[i].b = (getc(fp));
		}

	// Translate image to rgb-24.
	if(pal) {
		for(i = 0; i<img_size; i++) {
			m_pkPixels[i].r = pal[ data1[i] ].r;
			m_pkPixels[i].g = pal[ data1[i] ].g;
			m_pkPixels[i].b = pal[ data1[i] ].b;
			}
		}
	else {
		for(i = 0; i<img_size; i++) {
			m_pkPixels[i].r = kPalette[ data1[i] ].r;
			m_pkPixels[i].g = kPalette[ data1[i] ].g;
			m_pkPixels[i].b = kPalette[ data1[i] ].b;
			}
		}

	//delete [] data1;	// ** Fix_me ** [Vim], [Läcker som ett såll :)]
	return true;
}

bool Image::load(const char* filename)
{	
	// Find file exten.
	char *ext = strrchr( filename, '.');
	if(ext == NULL)		return false;
	
	if(strcmp(ext,".tga") == 0) {
		return load_tga(filename);
		}

	//if(strcmp(ext,".pcx") == 0)		return load_pcx(filename);
	if(strcmp(ext,".bmp") == 0) 
	{
		//this->create_empty(64,64);	
		//this->fill(0,0,this->width,this->height,255,0,0);

		return load_bmp(filename);
	}	

	return false;	// Not supported.
}

bool Image::load(FILE* fp, const char* filename)
{
	// Find file exten.
	char *ext = strrchr( filename, '.');
	if(ext == NULL)		return false;
	
	if(strcmp(ext,".tga") == 0)	return load_tga(fp);
	if(strcmp(ext,".bmp") == 0) return load_bmp(fp);

	return false;	// Not supported.
}

void Image::set_pixel(int x, int y, unsigned char r,unsigned char g,unsigned char b, unsigned char a)
{
//	assert(m_pkPixels);

	if (x < 0 || x >= m_iWidth)	return;
	if (y < 0 || y >= m_iHeight)	return;

	int offset = y * m_iWidth + x;

	m_pkPixels[offset].r = r;
	m_pkPixels[offset].g = g;
	m_pkPixels[offset].b = b;
	m_pkPixels[offset].a = a;
}

bool Image::get_pixel(int x, int y, color_rgba& rkColor)
{
	if(x > m_iWidth || y > m_iHeight)
		return false;

	int offset = y * m_iWidth + x;

	rkColor.r = m_pkPixels[offset].r;
	rkColor.g = m_pkPixels[offset].g;
	rkColor.b = m_pkPixels[offset].b;
	rkColor.a = m_pkPixels[offset].a;

	return true;
}

void Image::fill(int sx, int sy, int w, int h,unsigned char r,unsigned char g,unsigned char bl, unsigned char al)
{
//	assert(m_pkPixels);

	for(int y = 0; y<h; y++) {	
		for(int x = 0; x<w; x++) {
			set_pixel(sx+x,sy+y,r,g,bl,al);
			}
		}
}

void Image::DownSample(void)
{
//	assert(m_pkPixels);

	int new_w = getsize(m_iWidth);
	int new_h = getsize(m_iHeight);
	if(new_w == m_iWidth && new_h == m_iHeight)	return;

	color_rgba* oldimg = m_pkPixels;

	m_pkPixels = new color_rgba [new_w * new_h];

	float src_x, src_y;
	float stepx, stepy;

	src_x = src_y = 0;

	stepx = (float) m_iWidth /  new_w;
	stepy = (float) m_iHeight / new_h;

	for(int y = 0; y < new_h; y++) {
		src_x = 0;

		for(int x = 0; x < new_w; x++) {
			int srcindex = (int) src_y * m_iWidth + (int) src_x;
			m_pkPixels[y * new_w + x] = oldimg[srcindex];
			src_x += stepx;
			}
		
		src_y += stepy;
		}

	m_iHeight = new_h;
	m_iWidth  = new_w;

	delete [] oldimg;
}

/*
glm_pkPixelstorei(GL_UNPACK_ALIGNMENT ,1); 
glRasterPos2d(50,50);
glDrawm_pkPixels(bild.width,bild.height,GL_RGBA ,GL_UNSIGNED_BYTE,bild.m_pkPixels);
*/



Image& Image::operator = ( Image& v ) 
{
	if(m_pkPixels)	delete [] m_pkPixels;
	m_pkPixels = NULL;
	m_iWidth = m_iHeight = 0;

	if(v.m_pkPixels == NULL)	return *this;

	m_iWidth = v.m_iWidth;
	m_iHeight = v.m_iHeight;
	m_pkPixels = new color_rgba [m_iWidth * m_iHeight]; 
	memcpy(m_pkPixels,v.m_pkPixels,(m_iWidth * m_iHeight) * sizeof(color_rgba));
	return *this;
}

void Image::Flip(bool bFlipX, bool bFlipY)
{
//	assert(m_pkPixels);

	Image temp_img;
	temp_img = *this;

	int rx, ry;

	for(int y = 0; y < m_iHeight; y++) {
		for(int x = 0; x < m_iWidth; x++) {
			if(bFlipX)	rx = m_iWidth - x - 1;
				else	rx = x;
			if(bFlipY)	ry = m_iHeight - y - 1;
				else	ry = y;

			m_pkPixels[y*m_iWidth+x] = temp_img.m_pkPixels[(ry)*m_iWidth+rx];
		}
	}
}

void Image::custom_filter(float* matrix, float div, float bias)
{
	Image temp_img;
	temp_img = *this;
	
	int fd_offs = 0;
	float r,g,b;
	float rsum,gsum,bsum;
	r = g = b = 0;
	int offset = 1;
	int poffs;

	int py, px;

	for(int y = 0; y < m_iHeight; y++) {
		for(int x = 0; x < m_iWidth; x++) {
			rsum = gsum = bsum = 0;

			fd_offs = 0;
			
			for(int yo = -offset; yo <= offset; yo++) {
				for(int xo = -offset; xo <= offset; xo++) {
					py = y + yo;
					px = x + xo;

					if(px < 0)	px = m_iWidth - 1;
					else if (px >= m_iWidth)	px = 0;

					if(py < 0)	py = m_iHeight - 1;
					else if (py >= m_iHeight)	py = 0;

					poffs = py * m_iWidth + px;
					
					r = temp_img.m_pkPixels[poffs].r;
					rsum += r * matrix[fd_offs];
					
					g = temp_img.m_pkPixels[poffs].g;
					gsum += g * matrix[fd_offs];
					
					b = temp_img.m_pkPixels[poffs].b;
					bsum += b * matrix[fd_offs];
					fd_offs++;
					}
			}

//			m_pkPixels[(y)*width + (x)].r =  abs(int (rsum / div + bias) );
//			m_pkPixels[(y)*width + (x)].g =  abs(int (gsum / div + bias) );
//			m_pkPixels[(y)*width + (x)].b =  abs(int (bsum / div + bias) );
			}
		}
}

void Image::Test_Filter(void)
{
	float filter_data[9];
	float filter_norm = 1;
	float filter_bias = 0;

	int filter_type = 3;

	switch(filter_type) {
		case 0:	// Blur
			filter_data[0] = 1;		filter_data[1] = 1;		filter_data[2] = 1;
			filter_data[3] = 1;		filter_data[4] = 1;		filter_data[5] = 1;
			filter_data[6] = 1;		filter_data[7] = 1;		filter_data[8] = 1;
			filter_norm = 9;
			filter_bias = 0;
			break;
		
		case 1:	// Emboss
			filter_data[0] = 0;		filter_data[1] = 0;		filter_data[2] = 0;
			filter_data[3] = 1;		filter_data[4] = -1;	filter_data[5] = 0;
			filter_data[6] = 0;		filter_data[7] = 0;		filter_data[8] = 0;
			filter_norm = 1;
			filter_bias = 128;
			break;

		case 2:	// Vert Edge
			filter_data[0] = 0;		filter_data[1] = 0;		filter_data[2] = 0;
			filter_data[3] = 1;		filter_data[4] = -2; 	filter_data[5] = 1;
			filter_data[6] = 0;		filter_data[7] = 0;		filter_data[8] = 0;
			filter_norm = 1;
			filter_bias = 0;
			break;

		case 3:	// Horz Edge
			filter_data[0] = 0;		filter_data[1] = 1;		filter_data[2] = 0;
			filter_data[3] = 0;		filter_data[4] = -2;	filter_data[5] = 0;
			filter_data[6] = 0;		filter_data[7] = 1;		filter_data[8] = 0;
			filter_norm = 1;
			filter_bias = 0;
			break;

		case 4:	// Vert & Horz Edge
			filter_data[0] = 0;		filter_data[1] = 1;		filter_data[2] = 0;
			filter_data[3] = 1;		filter_data[4] = -4;	filter_data[5] = 1;
			filter_data[6] = 0;		filter_data[7] = 1;		filter_data[8] = 0;
			filter_norm = 1;
			filter_bias = 0;
			break;
	}

	custom_filter(filter_data,filter_norm,filter_bias);
}

void Image::MapColorToAlpha(float fR, float fG, float fB, float fAlpha)
{
	for(int y = 0; y < m_iHeight; y++) {
		for(int x = 0; x < m_iWidth; x++) {
			if(m_pkPixels[(y)*m_iWidth + (x)].r > (fR * 255) &&
				m_pkPixels[(y)*m_iWidth + (x)].g > (fG*255) &&
				m_pkPixels[(y)*m_iWidth + (x)].b > (fB*255))

				m_pkPixels[(y)*m_iWidth + (x)].a = (unsigned char)(fAlpha*255);
		}
	}

}

bool Image::load_bmp(const char* szFileName)
{
	FILE *pkFile = fopen(szFileName, "rb");
	if(pkFile == NULL)
	{
		printf("Image::load_bmp: Failed to open bitmap %s\n", szFileName);
		return false;
	}

	bool bSuccess = load_bmp(pkFile);
	fclose(pkFile);

	return bSuccess;
}

bool Image::load_bmp(FILE* pkFile)
{

	bmp_t kBitmap;
	memset(&kBitmap, 0, sizeof(bmp_t));
	fread(&kBitmap.kFileheader, sizeof(bmpheader_t), 1, pkFile);
	fread(&kBitmap.kInfoheader, sizeof(bmpinfo_t), 1, pkFile);
 
	// Testa för om ulSizeImage är 0. Adobe Phoshop sätter denna 
	// flagga till noll Windows Paint gör det inte.
	bool bZeroSize = false;
	if(kBitmap.kInfoheader.ulSizeImage == 0)
	{
		bZeroSize = true;
		int antal = 3;
		if (kBitmap.kInfoheader.usBitCount == 8)
			antal = 1;
		kBitmap.kInfoheader.ulSizeImage = kBitmap.kInfoheader.lHeight*
			kBitmap.kInfoheader.lWidth*antal;

		kBitmap.kInfoheader.ulSizeImage += 2; // Fulhack!
	}

	if( kBitmap.kFileheader.usType != BITMAP_ID ||
		kBitmap.kInfoheader.lHeight < 1 || 
		kBitmap.kInfoheader.lWidth  < 1 ||
		kBitmap.kInfoheader.ulSizeImage < 1)
	{
		printf("Image::load_bmp: Error loading bitmap. Bad file.\n");
		return false;
	}

	if (kBitmap.kInfoheader.usBitCount == 8)
	{
		fread(&kBitmap.kPalette, sizeof(bmppal_t), 256, pkFile);

		for(int i=0; i<256; i++)
		{
			int buffer = kBitmap.kPalette[i].ucRed;
			kBitmap.kPalette[i].ucRed  = kBitmap.kPalette[i].ucBlue;
			kBitmap.kPalette[i].ucBlue = buffer;
			kBitmap.kPalette[i].ucFlags = PC_NOCOLLAPSE;
		}
	}

	fseek(pkFile,-(int)(kBitmap.kInfoheader.ulSizeImage),SEEK_END);

	m_iWidth = kBitmap.kInfoheader.lWidth;
	m_iHeight = kBitmap.kInfoheader.lHeight;
	m_pkPixels = new color_rgba[m_iWidth*m_iHeight];

	int i=0, j=0, x=0, y=0;
	switch(kBitmap.kInfoheader.usBitCount)
	{
	case 8:
		kBitmap.pkData = new unsigned char[kBitmap.kInfoheader.ulSizeImage];
		fread(kBitmap.pkData, sizeof(unsigned char), 
			kBitmap.kInfoheader.ulSizeImage, pkFile);

		for(y=0; y<m_iHeight; y++)
			for(x=0; x<m_iWidth; x++)
			{
				int value = kBitmap.pkData[i];
				if(bZeroSize)
				{
					m_pkPixels[i].r = kBitmap.kPalette[value].ucRed; 
					m_pkPixels[i].b = kBitmap.kPalette[value].ucGreen;
					m_pkPixels[i].g = kBitmap.kPalette[value].ucBlue; 
					m_pkPixels[i].a = 0;
				}
				else
				{
					m_pkPixels[i].b = kBitmap.kPalette[value].ucRed; 
					m_pkPixels[i].g = kBitmap.kPalette[value].ucGreen;
					m_pkPixels[i].r = kBitmap.kPalette[value].ucBlue; 
					m_pkPixels[i].a = 0;
				}
				i++;
			}
		break;

	case 24:
		kBitmap.pkData = new unsigned char[3*kBitmap.kInfoheader.lWidth*
			kBitmap.kInfoheader.lHeight];

		if(bZeroSize)
		fread(kBitmap.pkData, sizeof(unsigned char), 
			kBitmap.kInfoheader.ulSizeImage-2, pkFile); // Fulhack!
		else
		fread(kBitmap.pkData, sizeof(unsigned char), 
			kBitmap.kInfoheader.ulSizeImage, pkFile);

		for(y=0; y<m_iHeight; y++)
			for(x=0; x<m_iWidth; x++)
			{
				m_pkPixels[i].b = kBitmap.pkData[j++]; 
				m_pkPixels[i].g = kBitmap.pkData[j++];
				m_pkPixels[i].r = kBitmap.pkData[j++]; 
				m_pkPixels[i++].a = 0;
			}
		break;

	default:
		printf("Image::load_bmp: Error loading bitmap. Bitcount %i not suported!.\n",
			kBitmap.kInfoheader.usBitCount);
		delete[] m_pkPixels;
		return false;
	}

	if(kBitmap.pkData)
		delete[] kBitmap.pkData;

	// check height to see if the image is reversed or not.
	if(kBitmap.kInfoheader.lHeight > 0)
		Flip(false, true);

	return true;
}









