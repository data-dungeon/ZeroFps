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
// 	if(strName.find("data/textures/") == -1)
// 	{
// 		int iOptions = strName.find('#');
// 		if(iOptions == -1)		
// 			strName ="data/textures/" + strName;
// 		else
// 		{
// 			strName = strName.substr(0,iOptions+1) + "data/textures/" + strName.substr(iOptions+1,strName.length());
// 			//cout<<"new name "<<strName<<endl;
// 		}	
// 	}
// 	else
// 		cout<<"Warning: old resource path:"<<strName<<endl;
	
	
	strTextureName = strName;

	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_iTextureID = pkTex->Load(strName.c_str(), 0);
		
	return true;
}

int ResTexture::CalculateSize()
{
	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	return pkTex->GetSizeOfTexture(m_iTextureID);
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


