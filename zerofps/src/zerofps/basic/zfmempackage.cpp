#include "zfmempackage.h"

ZFMemPackage::ZFMemPackage()
{
	m_acData.clear();
	m_iPos=0;

}

int ZFMemPackage::GetSize()
{
	return m_acData.size();
}

int ZFMemPackage::GetPos()
{
	return m_iPos;
}

void ZFMemPackage::SetPos(int iPos)
{
	if(iPos<0)
		return;
		
	m_iPos=iPos;
}

void ZFMemPackage::Clear()
{
	m_iPos=0;
	m_acData.clear();
}

bool ZFMemPackage::Read(void* pData,int iSize)
{
	char* datapointer=(char*)pData;
	
	if(m_acData.size() < m_iPos+iSize)
		return false;
	
	for(int i=0;i<iSize;i++)
	{
		datapointer[i] = m_acData[m_iPos + i];
	}
	m_iPos += iSize;
	
	return true;
}


bool ZFMemPackage::Write(void* pData,int iSize)
{
	char* datapointer=(char*)pData;	
	
	//make the vector bigger if the data dont fit
	if(m_acData.size() < m_iPos+iSize)
		m_acData.resize(m_iPos+iSize);
	
	for(int i=0;i<iSize;i++)
	{
		m_acData[m_iPos + i] = datapointer[i];
	}
	
	m_iPos += iSize;
	
	return true;
}
