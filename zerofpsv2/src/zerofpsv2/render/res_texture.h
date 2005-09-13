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
		int		m_iTextureID;				
		string	strTextureName;
	
	public:
	
		ResTexture();
		~ResTexture();
	
		bool Create(const string& strName);
		int  CalculateSize();
				
		friend class ZShaderSystem;
};

RENDER_API ZFResource* Create__ResTexture();

#endif



