/*
Image class for OpenGl (mostly).
*/
#ifndef _BASIC_IMG_H_   
#define _BASIC_IMG_H_

#include <cstdio>
#include "basic_x.h"

#ifdef WIN32
	#pragma pack( push )
	#pragma pack( 1 )
#endif

struct color_rgb {
	unsigned char r,g,b;
};

struct color_rgba {
	unsigned char r,g,b,a;
};

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
} ;

#ifdef WIN32
	#pragma pack( pop )
#endif


/// Class to load images in diffrent file formats.
class BASIC_API Image 
{
private:
	void read_pixel(color_rgba* dst, char* src, int pixsize);	///< Read image data.
	void read_rgb(color_rgba* dst,  FILE* fp, int pixsize);		///< Read image data.

	

public:
	int width, height;					///< Size of image.
	color_rgba* pixels;					///< Ptr to pixels in image.
	bool	bHasAlpha;					///< True if image was loaded with alpha.

	Image();
	Image( const Image& v );
	~Image();

	Image& operator = ( Image& v );		///< Make a copy of image.

	bool load_tga(char* filename);					///< Load a .tga image.
	bool load_tga(FILE *fp);						///< Load a .tga image.
	bool load_pcx(char* filename);					///< Load a .pcx image.
	bool load_pcx(FILE *fp, color_rgb* pal);		///< Load a .pcx image.

	bool load(char* filename);						///< Load pcx or tga.

	void save(char* filename, bool alpha = false);	///< Save image as a 24 / 32 bit .tga file.

	void flip(bool flipx, bool flipy);				///< Flip image along one or two axis.
	void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);	///< Set color of a pixel.
	// Fill part of image in one color.
	void fill(int sx, int sy, int w, int h, unsigned char r, unsigned char g, unsigned char b);

	void free(void);								///< Remove image.

	void downsample(void);							///< Sample image down so it's size is a power of two.

	void custom_filter(float* matrix, float div, float bias);	///< DO NOT USE
	void Test_Filter(void);										///< DO NOT USE

	void create_empty(int setwidth, int setheight);

	void MapColorToAlpha(float fR, float fG, float fB, float fAlpha);
};

#endif


