#include "zvertexbuffer.h"


ZVertexBuffer::ZVertexBuffer()
{
	m_iBufferID	= 0;
	
	

}

ZVertexBuffer::~ZVertexBuffer()
{
	if(m_iBufferID != 0)
	{
		//cout<<"-VB:"<<m_iBufferID<<endl;
		glDeleteBuffersARB(1,&m_iBufferID);
		
	}
	else
		cout<<"removes unused vertex buffer"<<endl;
}