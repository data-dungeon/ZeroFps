/*
	Offical FulHack.cpp fil =).
	For Engine.h.

	All code in this file have to be inside name tags that state the name of he who dares to write
	code in this file.
*/

/*** Start: Your Name ***/
	// Ugly Code From Hell Here...
/*** End: Your Name ***/

#ifndef _ENGINE_FH_H_
#define _ENGINE_FH_H_
#include "zfresource.h"

/*** Start: Vim ***/
void ENGINE_API DrawTest(float x, float y, float z);

class ResTexture : public ZFResource
{
private:

public:
	int		m_iTextureID;
	string	strTextureName;

	ResTexture();
	~ResTexture();

	bool Create(string strName);
};

/*** End: Vim ***/


#endif



