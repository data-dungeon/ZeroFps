#ifndef _RES_TEXTURE_H_
#define _RES_TEXTURE_H_

#include "../basic/zfresource.h"
#include "../render/texturemanager.h"
#include "../basic/zfsystem.h"
#include "render_x.h"

/// A Texture in the Resource SubSystem.
class RENDER_API ResTexture : public ZFResource
{
private:

public:
	int		m_iTextureID;
	string	strTextureName;

	ResTexture();
	~ResTexture();

	bool Create(string strName);
	int  CalculateSize();		
};

RENDER_API ZFResource* Create__ResTexture();

#endif



