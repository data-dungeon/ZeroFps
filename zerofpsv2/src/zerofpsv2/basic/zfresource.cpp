#include "zfresource.h"

ZFResource::ZFResource() 
{
	m_iSizeInBytes = 0;
}

ZFResource::~ZFResource() 
{

}

int ZFResource::GetSize()
{
	if(m_iSizeInBytes == 0)
		m_iSizeInBytes = CalculateSize();

	return m_iSizeInBytes;
}

