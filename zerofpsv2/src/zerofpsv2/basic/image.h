#ifndef _BASIC_IMG_H_   
#define _BASIC_IMG_H_

#include <cstdio>
#include "basic_x.h"

#pragma pack( 1 )

/// Color (Red Green Blue)
struct color_rgb {
	unsigned char r,g,b;
};

/// Color (Red Green Blue Alpha)
struct color_rgba {
	unsigned char r,g,b,a;
};

#pragma pack(  )

/// Class to load images in diffrent file formats.
class BASIC_API Image 
{
private:
	void read_pixel(color_rgba* dst, char* src, int pixsize);	///< Read image data.
	void read_rgb(color_rgba* dst,  FILE* fp, int pixsize);		///< Read image data.

public:
	
	Image();
	~Image();
	Image( const Image& v );

	Image& operator = ( Image& v );					///< Make a copy of image.

	int width, height;									///< Size of image.
	color_rgba* pixels;									///< Ptr to pixels in image.
	bool	bHasAlpha;										///< True if image was loaded with alpha.

	// Load Functions
	bool load_tga(const char* filename);			///< Load a .tga image.
	bool load_tga(FILE *fp);							///< Load a .tga image.
	bool load_pcx(const char* filename);			///< Load a .pcx image.
	bool load_pcx(FILE *fp, color_rgb* pal);		///< Load a .pcx image.
	bool load_bmp(const char* szFileName);			///< Load a .bmp 
	bool load_bmp(FILE* pkFile);						///< Load a .bmp
	bool load(const char* filename);					///< Try to find a good loader from filename
	bool load(FILE* fp, const char* filename);	///<  Try to find a good loader from filename

	void create_empty(int setwidth, int setheight);
	void free(void);												///< Remove image.

	void save(const char* filename, bool alpha = false);		///< Save image as a 24 / 32 bit .tga file.

	// Tools
	void flip(bool flipx, bool flipy);				///< Flip image along one or two axis.
	void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);	///< Set color of a pixel.
	bool get_pixel(int x, int y, color_rgba& rkColor);
	void fill(int sx, int sy, int w, int h, unsigned char r, unsigned char g, unsigned char b);

	void downsample(void);							///< Sample image down so it's size is a power of two.

	void custom_filter(float* matrix, float div, float bias);	///< DO NOT USE
	void Test_Filter(void);													///< DO NOT USE

	void MapColorToAlpha(float fR, float fG, float fB, float fAlpha);
};

#endif


