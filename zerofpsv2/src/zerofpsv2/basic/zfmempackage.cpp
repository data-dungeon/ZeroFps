#include "zfmempackage.h"

#include <iostream>

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
	
	if(m_acData.size() < (unsigned int)(m_iPos+iSize)){
		cout<<"ERROR: Trying to read outside of mempackage"<<endl;
		return false;
	}
	
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
	if(m_acData.size() < (unsigned int)(m_iPos+iSize))
		m_acData.resize(m_iPos+iSize);
	
	for(int i=0;i<iSize;i++)
	{
		m_acData[m_iPos + i] = datapointer[i];
	}
	
	m_iPos += iSize;
	
	return true;
}

bool ZFMemPackage::SaveToFile(ZFFile* pkFile)
{
	int iSize=GetSize();
	if(!pkFile->Write(iSize))
		return false;
	
	if(!pkFile->Write(GetDataPointer(),GetSize()))
		return false;
			
	return true;
}

bool ZFMemPackage::LoadFromFile(ZFFile* pkFile)
{

	int iSize;
	char data;	
	if(!pkFile->Read(iSize))
		return false;
	
	//read data
	SetPos(0);
	for(int j=0;j<iSize;j++)
	{
		if(!pkFile->Read(data))
			return false;
		Write(data);
	}
/*	
	SetPos(0);
	cout<<"data in mem"<<endl;
	for(int i=0;i<50;i++){
		Read(data);
		cout<<data;
	}
	cout<<endl;	
*/	
	SetPos(0);
	return true;
}


