#include "zffile.h"


ZFFile::ZFFile(const char* acName,bool bWritable)
{
	m_bOpen=false;
	m_bWritable=false;
	m_acName="";
	
	SetIo(new ZFIoFile());	
	Open(acName,bWritable);	


}

ZFFile::ZFFile()
{
	m_bOpen=false;
	m_bWritable=false;
	m_acName="";
	
	SetIo(new ZFIoFile());
}


bool ZFFile::Open(const char* acName,bool bWritable)
{
	if(m_bOpen)
		Close();

	if(m_kIo->Open(acName,bWritable)){
		m_acName=acName;
		m_bWritable=bWritable;
		m_bOpen=true;
		SetPos(0);
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
	if(!m_bOpen)
		return false;
	
	return m_kIo->Read(data,iSize);
}

bool ZFFile::Write(void* data,int iSize)
{
	if(!m_bOpen)
		return false;
	
	return m_kIo->Write(data,iSize);
}

int ZFFile::GetPos()
{
	return m_kIo->GetPos();
}

bool ZFFile::SetPos(int iPos)
{
	return m_kIo->SetPos(iPos);
}

