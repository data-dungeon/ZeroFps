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

/*
int ZFMemPackage::GetPos()
{
	return m_iPos;
}
*/

int ZFMemPackage::Tell()
{
	return m_iPos;
}

/*
void ZFMemPackage::SetPos(int iPos)
{
	if(iPos<0)
		return;
		
	m_iPos=iPos;
}
*/

void ZFMemPackage::Seek(int iPos, int iOrigin)
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

bool ZFMemPackage::Read(void* pkData, int iSize, int iCount)
{
	int iTotalSize = iSize * iCount;

	char* datapointer=(char*)pkData;
	
	if(m_acData.size() < (unsigned int)(m_iPos + iTotalSize)){
		cout<<"ERROR: Trying to read outside of mempackage"<<endl;
		return false;
	}
	
	for(int i=0;i<iTotalSize;i++)
	{
		datapointer[i] = m_acData[m_iPos + i];
	}

	m_iPos += iTotalSize;
	
	return true;
}


bool ZFMemPackage::Write(void* pkData, int iSize, int iCount)
{
	char* datapointer=(char*)pkData;	
	int iTotalSize = iSize * iCount;
	
	//make the vector bigger if the data dont fit
	if(m_acData.size() < (unsigned int)(m_iPos+iTotalSize))
		m_acData.resize(m_iPos+iTotalSize);
	
	for(int i=0;i<iTotalSize;i++)
	{
		m_acData[m_iPos + i] = datapointer[i];
	}
	
	m_iPos += iSize;
	
	return true;
}

bool ZFMemPackage::SaveToFile(ZFVFile* pkFile)
{
	int iSize=GetSize();
	if(!pkFile->Write(&iSize,sizeof(int),1))
		return false;
	
	if(!pkFile->Write(GetDataPointer(),GetSize(),1))
		return false;
			
	return true;
}

bool ZFMemPackage::LoadFromFile(ZFVFile* pkFile)
{

	int iSize;
	char data;	
	if(!pkFile->Read(&iSize,sizeof(int),1))
		return false;
	
	//read data
	Seek(0,0);
	for(int j=0;j<iSize;j++)
	{
		if(!pkFile->Read(&data,1,1))
			return false;
		Write(&data, 1,1);
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
	Seek(0,0);
	return true;
}


