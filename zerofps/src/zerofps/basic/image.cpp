//#include "core.h"
#include <memory.h> 
#include <string.h> 
#include "image.h"

#include <windows.h>

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
	pixels = NULL;
	width = height = 0;
	if(v.pixels == NULL)	return;

	width = v.width;
	height = v.height;
	bHasAlpha = v.bHasAlpha;
	pixels = new color_rgba [width * height]; 
	memcpy(pixels,v.pixels,(width * height) * sizeof(color_rgba));
}

Image::~Image()
{
	if(pixels)	delete [] pixels;
	pixels = NULL;
	width = height = 0;
}

void Image::free(void)
{
	if(pixels)	delete [] pixels;
	pixels = NULL;
	width = height = 0;
	bHasAlpha = false;
}

void Image::create_empty(int setwidth, int setheight)
{
	free();
	width = setwidth;
	height = setheight;
	bHasAlpha = false;
	pixels = new color_rgba [width * height];
}


// TGA FORMAT
void Image::read_pixel(color_rgba* dst, char* src, int pixsize)
{
	char red,grn,blu,alp;
	char j,k; 
	red = grn = blu = alp = 0;
	char *data = src;

	switch(pixsize) {
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

	dst->r = red;
	dst->g = grn;
	dst->b = blu;
	dst->a = alp;
}

void Image::read_rgb(color_rgba* dst, FILE* fp, int pixsize)
{
	char red,grn,blu,alp;
	char j,k; 
	red = grn = blu = alp = 0;

	switch(pixsize) {
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

	dst->r = red;
	dst->g = grn;
	dst->b = blu;
	dst->a = alp;
}

void Image::save(const char* filename, bool alpha)
{
	tgahead_t head;
	memset(&head,0,sizeof(tgahead_t));

	head.id_length = 0;
	head.colormap_type = 0;
	head.image_type = TGA_IMAGETYPE_URGB;

	head.width = width;
	head.height = height;

	if(alpha)	head.pixel_depth = 32;
		else	head.pixel_depth = 24;

	FILE *fp = fopen(filename,"wb");
	if(!fp)	return;

	fwrite(&head, sizeof(tgahead_t),1,fp);

	for(int i=0; i<width * height; i++) {
		putc(pixels[i].b,fp);
		putc(pixels[i].g,fp);
		putc(pixels[i].r,fp);
		if(alpha)
			putc(pixels[i].a,fp);
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
	int pixelsize;

	FILE *fp = fopen(filename,"rb");
	
	if(!fp)	return false;

	tgahead_t head;
	fread(&head,sizeof(tgahead_t),1,fp);

	// Check that it's a tga file and that it is one that we support.
	if(head.image_type != TGA_IMAGETYPE_URGB)	return false;
	
	// Set basic image prop.
	width  = head.width;
	height = head.height;

	if(head.pixel_depth == 8)	pixelsize = 1;
	if(head.pixel_depth == 16)	pixelsize = 2;
	if(head.pixel_depth == 24)	pixelsize = 3;
	if(head.pixel_depth == 32)	pixelsize = 4;

	// Read color map if there is one.
	if(head.colormap_type != 0) {
		// Get color map memory.
		map = new color_rgba [head.colormap_length];
		for(int i = 0; i<head.colormap_length; i++)	
			read_rgb(&map[i], fp, head.colormap_size);
		}

	// Alloc pixel memory.
	pixels = new color_rgba [width * height];
	
	data = new char [width*height*pixelsize];
	fread(data,sizeof(char),width * height * pixelsize,fp);

	for(int i=0; i<width * height; i++) {
		if(head.image_type == TGA_IMAGETYPE_URGB)	read_pixel(&pixels[i], &data[i*pixelsize], head.pixel_depth);
		if(head.image_type == TGA_IMAGETYPE_UMAP)	{
			pixels[i] = map [ data[i] ];
			}
		}
	
	if(data)	delete [] data;
	if(map)		delete [] map;

	fclose(fp);
	return true;
*/
}

bool Image::load_tga(FILE *fp)
{
	char *data = NULL;
	color_rgba* map = NULL;
	int pixelsize;

	if(!fp)	return false;

	tgahead_t head;
	fread(&head,sizeof(tgahead_t),1,fp);

	// Check that it's a tga file and that it is one that we support.
	if(head.image_type != TGA_IMAGETYPE_URGB)	return false;
	
	// Set basic image prop.
	bHasAlpha = false;
	width  = head.width;
	height = head.height;

	if(head.pixel_depth == 8)	pixelsize = 1;
	if(head.pixel_depth == 16)	pixelsize = 2;
	if(head.pixel_depth == 24)	pixelsize = 3;
	if(head.pixel_depth == 32)	{
		pixelsize = 4;
		bHasAlpha = true;
		}

	// Read color map if there is one.
	if(head.colormap_type != 0) {
		// Get color map memory.
		map = new color_rgba [head.colormap_length];
		for(int i = 0; i<head.colormap_length; i++)	
			read_rgb(&map[i], fp, head.colormap_size);
		}

	// Alloc pixel memory.
	pixels = new color_rgba [width * height];
	
	data = new char [width*height*pixelsize];
	fread(data,sizeof(char),width * height * pixelsize,fp);

	for(int i=0; i<width * height; i++) {
		if(head.image_type == TGA_IMAGETYPE_URGB)	read_pixel(&pixels[i], &data[i*pixelsize], head.pixel_depth);
		if(head.image_type == TGA_IMAGETYPE_UMAP)	{
			pixels[i] = map [ data[i] ];
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

	bHasAlpha = false;
	// Alloc image mem.
	width  = head.xmax + 1;
	height = head.ymax + 1;
	int img_size = width * height;

	pixels = new color_rgba [img_size];
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
			pixels[i].r = pal[ data1[i] ].r;
			pixels[i].g = pal[ data1[i] ].g;
			pixels[i].b = pal[ data1[i] ].b;
			}
		}
	else {
		for(i = 0; i<img_size; i++) {
			pixels[i].r = kPalette[ data1[i] ].r;
			pixels[i].g = kPalette[ data1[i] ].g;
			pixels[i].b = kPalette[ data1[i] ].b;
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
	
	if(strcmp(ext,".tga") == 0)		return load_tga(filename);
	if(strcmp(ext,".pcx") == 0)		return load_pcx(filename);
	return false;	// Not supported.
}

void Image::set_pixel(int x, int y, unsigned char r,unsigned char g,unsigned char b)
{
//	assert(pixels);

	if (x < 0 || x >= width)	return;
	if (y < 0 || y >= height)	return;

	int offset = y * width + x;

	pixels[offset].r = r;
	pixels[offset].g = g;
	pixels[offset].b = b;
}

void Image::fill(int sx, int sy, int w, int h,unsigned char r,unsigned char g,unsigned char bl)
{
//	assert(pixels);

	for(int y = 0; y<h; y++) {	
		for(int x = 0; x<w; x++) {
			set_pixel(sx+x,sy+y,r,g,bl);
			}
		}
}

void Image::downsample(void)
{
//	assert(pixels);

	int new_w = getsize(width);
	int new_h = getsize(height);
	if(new_w == width && new_h == height)	return;

	color_rgba* oldimg = pixels;

	pixels = new color_rgba [new_w * new_h];

	float src_x, src_y;
	float stepx, stepy;

	src_x = src_y = 0;

	stepx = (float) width /  new_w;
	stepy = (float) height / new_h;

	for(int y = 0; y < new_h; y++) {
		src_x = 0;

		for(int x = 0; x < new_w; x++) {
			int srcindex = (int) src_y * width + (int) src_x;
			pixels[y * new_w + x] = oldimg[srcindex];
			src_x += stepx;
			}
		
		src_y += stepy;
		}

	height = new_h;
	width  = new_w;

	delete [] oldimg;
}

/*
glPixelStorei(GL_UNPACK_ALIGNMENT ,1); 
glRasterPos2d(50,50);
glDrawPixels(bild.width,bild.height,GL_RGBA ,GL_UNSIGNED_BYTE,bild.pixels);
*/



Image& Image::operator = ( Image& v ) 
{
	if(pixels)	delete [] pixels;
	pixels = NULL;
	width = height = 0;

	if(v.pixels == NULL)	return *this;

	width = v.width;
	height = v.height;
	pixels = new color_rgba [width * height]; 
	memcpy(pixels,v.pixels,(width * height) * sizeof(color_rgba));
	return *this;
}

void Image::flip(bool flipx, bool flipy)
{
//	assert(pixels);

	Image temp_img;
	temp_img = *this;

	int rx, ry;

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(flipx)	rx = width - x - 1;
				else	rx = x;
			if(flipy)	ry = height - y - 1;
				else	ry = y;

			pixels[y*width+x] = temp_img.pixels[(ry)*width+rx];
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

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			rsum = gsum = bsum = 0;

			fd_offs = 0;
			
			for(int yo = -offset; yo <= offset; yo++) {
				for(int xo = -offset; xo <= offset; xo++) {
					py = y + yo;
					px = x + xo;

					if(px < 0)	px = width - 1;
					else if (px >= width)	px = 0;

					if(py < 0)	py = height - 1;
					else if (py >= height)	py = 0;

					poffs = py * width + px;
					
					r = temp_img.pixels[poffs].r;
					rsum += r * matrix[fd_offs];
					
					g = temp_img.pixels[poffs].g;
					gsum += g * matrix[fd_offs];
					
					b = temp_img.pixels[poffs].b;
					bsum += b * matrix[fd_offs];
					fd_offs++;
					}
			}

//			pixels[(y)*width + (x)].r =  abs(int (rsum / div + bias) );
//			pixels[(y)*width + (x)].g =  abs(int (gsum / div + bias) );
//			pixels[(y)*width + (x)].b =  abs(int (bsum / div + bias) );
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
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(pixels[(y)*width + (x)].r > (fR * 255) &&
				pixels[(y)*width + (x)].g > (fG*255) &&
				pixels[(y)*width + (x)].b > (fB*255))

				pixels[(y)*width + (x)].a = (unsigned char)(fAlpha*255);
		}
	}

}

bool Image::load_bmp(char* szFileName)
{
	FILE *pkFile = fopen(szFileName, "rb");
	if(pkFile == NULL)
		return false;

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

	bool bDebug = true;

	if(bDebug)
	{
		printf("kBitmap.kFileheader.usType = %i\n", kBitmap.kFileheader.usType); 
		printf("kBitmap.kFileheader.ulSize = %i\n", kBitmap.kFileheader.ulSize); 
		printf("kBitmap.kFileheader.usReserved1 = %i\n", kBitmap.kFileheader.usReserved1); 
		printf("kBitmap.kFileheader.usReserved2 = %i\n", kBitmap.kFileheader.usReserved2); 
		printf("kBitmap.kFileheader.ulOffBits = %i\n", kBitmap.kFileheader.ulOffBits); 

		printf("kBitmap.kInfoheader.ulSize = %i\n", kBitmap.kInfoheader.ulSize); 
		printf("kBitmap.kInfoheader.lWidth = %i\n", kBitmap.kInfoheader.lWidth); 
		printf("kBitmap.kInfoheader.lHeight = %i\n", kBitmap.kInfoheader.lHeight); 
		printf("kBitmap.kInfoheader.usPlanes = %i\n", kBitmap.kInfoheader.usPlanes); 
		printf("kBitmap.kInfoheader.usBitCount = %i\n", kBitmap.kInfoheader.usBitCount);
		printf("kBitmap.kInfoheader.ulCompression = %i\n", kBitmap.kInfoheader.ulCompression); 
		printf("kBitmap.kInfoheader.ulSizeImage = %i\n", kBitmap.kInfoheader.ulSizeImage); 
		printf("kBitmap.kInfoheader.lXPelsPerMeter = %i\n", kBitmap.kInfoheader.lXPelsPerMeter); 
		printf("kBitmap.kInfoheader.lYPelsPerMeter = %i\n", kBitmap.kInfoheader.lYPelsPerMeter); 
		printf("kBitmap.kInfoheader.ulClrUsed = %i\n", kBitmap.kInfoheader.ulClrUsed); 
		printf("kBitmap.kInfoheader.ulClrImportant = %i\n", kBitmap.kInfoheader.ulClrImportant); 
	}
 
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
	}

	if( kBitmap.kFileheader.usType != BITMAP_ID ||
		kBitmap.kInfoheader.lHeight < 1 || 
		kBitmap.kInfoheader.lWidth  < 1 ||
		kBitmap.kInfoheader.ulSizeImage < 1)
	{
		printf("Error loading bitmap. Bad file.\n");
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

	width = kBitmap.kInfoheader.lWidth;
	height = kBitmap.kInfoheader.lHeight;
	pixels = new color_rgba[width*height];

	int i=0, j=0, x=0, y=0;

	switch(kBitmap.kInfoheader.usBitCount)
	{
	case 8:
		kBitmap.pkData = new unsigned char[kBitmap.kInfoheader.ulSizeImage];
		fread(kBitmap.pkData, sizeof(unsigned char), 
			kBitmap.kInfoheader.ulSizeImage, pkFile);

		for(y=0; y<height; y++)
			for(x=0; x<width; x++)
			{
				int value = kBitmap.pkData[i];
				if(bZeroSize)
				{
					pixels[i].r = kBitmap.kPalette[value].ucRed; 
					pixels[i].b = kBitmap.kPalette[value].ucGreen;
					pixels[i].g = kBitmap.kPalette[value].ucBlue; 
					pixels[i].a = 0;
				}
				else
				{
					pixels[i].b = kBitmap.kPalette[value].ucRed; 
					pixels[i].g = kBitmap.kPalette[value].ucGreen;
					pixels[i].r = kBitmap.kPalette[value].ucBlue; 
					pixels[i].a = 0;
				}
				i++;
			}
		break;

	case 24:
		kBitmap.pkData = new unsigned char[3*kBitmap.kInfoheader.lWidth*
			kBitmap.kInfoheader.lHeight];

		fread(kBitmap.pkData, sizeof(unsigned char), 
			kBitmap.kInfoheader.ulSizeImage, pkFile);

		for(y=0; y<height; y++)
			for(x=0; x<width; x++)
			{
				if(bZeroSize)
				{
					pixels[i].r = kBitmap.pkData[j++]; 
					pixels[i].b = kBitmap.pkData[j++];
					pixels[i].g = kBitmap.pkData[j++]; 
					pixels[i].a = 0;
				}
				else
				{
					pixels[i].b = kBitmap.pkData[j++]; 
					pixels[i].g = kBitmap.pkData[j++];
					pixels[i].r = kBitmap.pkData[j++]; 
					pixels[i].a = 0;
				}
				i++;
			}
		break;

	default:
		printf("Error loading bitmap. Bitcount %i not suported!.\n",
			kBitmap.kInfoheader.usBitCount);
		delete[] pixels;
		return false;
	}

	printf("bitmap loaded (w=%i,h=%i,bpp=%i)\n", width, height, 
		kBitmap.kInfoheader.usBitCount);

	if(kBitmap.pkData)
		delete[] kBitmap.pkData;

	return true;
}