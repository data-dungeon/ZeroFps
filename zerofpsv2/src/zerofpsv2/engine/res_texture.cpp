#include "res_texture.h"

ResTexture::ResTexture()
{
	m_iTextureID = NO_TEXTURE;

}

ResTexture::~ResTexture()
{
	if(m_iTextureID == NO_TEXTURE)
		return;

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
/*
ZFResource*	ResTexture::GetResourcePtr()
{
	return GetResourcePtr();
}
*/

ZFResource* Create__ResTexture()
{
	return new ResTexture;
}


