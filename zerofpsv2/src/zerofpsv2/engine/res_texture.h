#ifndef _RES_TEXTURE_H_
#define _RES_TEXTURE_H_

#include "../basic/zfresource.h"
#include "../render/texturemanager.h"
#include "../basic/zfobjectmanger.h"
#include "engine_x.h"

class ENGINE_API ResTexture : public ZFResource
{
private:

public:
	int		m_iTextureID;
	string	strTextureName;

	ResTexture();
	~ResTexture();

	bool Create(string strName);
};

ENGINE_API ZFResource* Create__ResTexture();

#endif



