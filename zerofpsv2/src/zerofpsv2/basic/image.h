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

/**	\brief	Class to load images in diffrent file formats.
		\ingroup Basic

  Class to load Images. All loaded images are converted into RGBA-32 format.
*/
class BASIC_API Image 
{
private:
	void ReadPixel(color_rgba* pkDst, char* pcSrc, int iPixsize);	///< Read image data.
	void ReadRgb(color_rgba* pkDst,  FILE* fp, int iPixsize);		///< Read image data.

public:
	
	Image();
	~Image();
	Image( const Image& v );

	Image& operator = (const Image& v );					///< Make a copy of image.

	int				m_iWidth, m_iHeight;						///< Size of image.
	color_rgba*		m_pkPixels;								///< Ptr to pixels in image.
	bool				m_bHasAlpha;							///< True if image was loaded with alpha.

	// Load Functions
	bool load_jpg(const char* filename);					///< Load a .jpg/.jpeg image
	bool load_jpg(FILE *fp);								///< Load a .jpg/.jpeg image
	bool load_tga(const char* filename);					///< Load a .tga image.
	bool load_tga(FILE *fp);									///< Load a .tga image.
	bool load_pcx(const char* filename);					///< Load a .pcx image.
	bool load_pcx(FILE *fp, color_rgb* pal);				///< Load a .pcx image.
	bool load_bmp(const char* szFileName);					///< Load a .bmp 
	bool load_bmp(FILE* pkFile);								///< Load a .bmp
	bool load(const char* filename);							///< Try to find a good loader from filename
	bool load(FILE* fp, const char* filename);			///<  Try to find a good loader from filename

	void CreateEmpty(int setwidth, int setheight);
	void Free(void);												///< Remove image.

	bool Save(const char* szFileName, bool bAlpha = false);		///< Save image as a 24 / 32 bit .tga file.
	void Save(FILE* fp, bool bAlpha = false);							///< Save image as a 24 / 32 bit .tga file.

	// Tools
	void Flip(bool bFlipX, bool bFlipY);				///< Flip image along one or two axis.
	void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0);	///< Set color of a pixel.
	bool get_pixel(int x, int y, color_rgba& rkColor);
	void fill(int sx, int sy, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char al = 0);

	void DownSample(void);							///< Sample image down so it's size is a power of two.

	void custom_filter(float* matrix, float div, float bias);	///< DO NOT USE
	void Test_Filter(void);													///< DO NOT USE

	void MapColorToAlpha(float fR, float fG, float fB, float fAlpha);
};

#endif


