/*
	Offical FulHack.cpp fil =).
	For Engine.DLL.

	All code in this file have to be inside name tags that state the name of he who dares to write
	code in this file.
*/

/*** Start: Your Name ***/
	// Ugly Code From Hell Here...
/*** End: Your Name ***/

#include "fh.h"
#include "zerofps.h"

/*** Start: Vim ***/


ResTexture::ResTexture()
{
	m_iTextureID = -1;

}

ResTexture::~ResTexture()
{
	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	pkTex->UnLoad(m_iTextureID);
}


bool ResTexture::Create(string strName)
{
	strTextureName = strName;

	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_iTextureID = pkTex->Load(strName.c_str(), 0);
		
	return true;
}

ZFResource* Create__ResTexture()
{
	return new ResTexture;
}

/*** End: Vim ***/



