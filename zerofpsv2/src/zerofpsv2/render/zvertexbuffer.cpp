#include "zvertexbuffer.h"


ZVertexBuffer::ZVertexBuffer()
{
	m_iBufferID	= 0;
	m_iIndexBufferID = 0;
	m_eDrawMode = TRIANGLES_MODE;

	m_b2DVertexPointer =			false;
	m_bVertexPointer =			false;
	m_bNormalPointer =			false;
	m_bTexturePointer0 = 		false;
	m_bTexturePointer1 = 		false;
	m_bTexturePointer2 = 		false;
	m_bTexturePointer3 = 		false;
	m_bIndexPointer = 			false;
	m_bColorPointer = 			false;	
	
	m_i2DVertexPointer = 		0;
	m_iVertexPointer	=			0;
	m_iNormalPointer	=			0;	
	m_iTexturePointer0=			0;
	m_iTexturePointer1=			0;			
	m_iTexturePointer2=			0;		
	m_iTexturePointer3=			0;		
	m_iColorPointer=				0;		
	
		
	m_iNrOfVertexs = 				0;
	m_iNrOfIndexes = 				0;			

	
}

ZVertexBuffer::~ZVertexBuffer()
{
	if(m_iBufferID != 0)
		glDeleteBuffersARB(1,&m_iBufferID);

	if(m_iIndexBufferID != 0)
		glDeleteBuffersARB(1,&m_iIndexBufferID);
	
}










