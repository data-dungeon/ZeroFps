#include "zfresource.h"

ZFResource::ZFResource() 
{
//	Hora = 0;
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

/*void ZFResource::Clear() 
{

}*/
	
/*bool ZFResource::Create(string strName) 
{
	return true;
}*/

/*void ZFResource::Destroy() 
{

}*/

