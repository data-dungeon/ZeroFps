#include "zffile.h"


ZFFile::ZFFile(const char* acName,bool bWritable)
{
	SetIo(new ZFIoFile());
	Open(acName,bWritable);	
	

}

ZFFile::ZFFile()
{
	SetIo(new ZFIoFile());
}


bool ZFFile::Open(const char* acName,bool bWritable)
{
	if(m_kIo->Open(acName,bWritable)){
		m_acName=acName;
		m_bWritable=bWritable;
		m_bOpen=true;
		return true;
	} else {
		m_bOpen=false;
		return false;	
	}
}

bool ZFFile::Close()
{
	return m_kIo->Close();
}

bool ZFFile::Read(void* data,int iSize)
{
	return m_kIo->Read(data,iSize);
}

bool ZFFile::Write(void* data,int iSize)
{
	return m_kIo->Write(data,iSize);
}

template <class Any>
bool ZFFile::Read(Any data)
{
	return m_kIo->Read((void*)&data,sizeof(Any));
}

template <class Any>
bool ZFFile::Write(Any data)
{
	return m_kIo->Write((void*)&data,sizeof(Any));
}



int ZFFile::GetPos()
{
	return m_kIo->GetPos();
}

bool ZFFile::SetPos(int iPos)
{
	return m_kIo->SetPos(iPos);
}

