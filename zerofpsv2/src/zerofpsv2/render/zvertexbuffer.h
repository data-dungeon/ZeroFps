#ifndef _ZVERTEXBUFFER_H_
#define _ZVERTEXBUFFER_H_

#include "zshadersystem.h"


class RENDER_API ZVertexBuffer
{
	private:
		int		m_iBufferID;		
		int		m_iVertexType ;

		//what kind of data does this vertexbuffer contain
		bool		m_bNormals;
		bool		m_bTexture1;
		bool		m_bTexture2;
		bool		m_bTexture3;
		bool		m_bTexture4;
		bool		m_bIndex;
		bool		m_bColor;
		
		
	public:

		

};


#endif