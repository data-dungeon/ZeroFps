#ifndef _ZVERTEXBUFFER_H_
#define _ZVERTEXBUFFER_H_

#include "zshadersystem.h"


class RENDER_API ZVertexBuffer
{
	private:
		unsigned int	m_iBufferID;
		unsigned int	m_iIndexBufferID;
		DRAW_MODE		m_eDrawMode;		
				
		int				m_iNrOfVertexs;
		int				m_iNrOfIndexes;		
		
		//what kind of data does this vertexbuffer contain
		bool		m_b2DVertexPointer;
		bool		m_bVertexPointer;
		bool		m_bNormalPointer;	
		bool		m_bTexturePointer0;
		bool		m_bTexturePointer1;		
		bool		m_bTexturePointer2;		
		bool		m_bTexturePointer3;		
		bool		m_bIndexPointer;
		bool		m_bColorPointer;
		bool		m_bTangentPointer;
		bool		m_bBiTangentPointer;
		
		unsigned int		m_i2DVertexPointer;
		unsigned int		m_iVertexPointer;
		unsigned int		m_iNormalPointer;	
		unsigned int		m_iTexturePointer0;
		unsigned int		m_iTexturePointer1;		
		unsigned int		m_iTexturePointer2;		
		unsigned int		m_iTexturePointer3;		
		unsigned int		m_iColorPointer;					
		unsigned int		m_iTangentPointer;			
		unsigned int		m_iBiTangentPointer;			

		
	public:
		ZVertexBuffer();
		~ZVertexBuffer();

	friend class ZShaderSystem;
};


#endif













