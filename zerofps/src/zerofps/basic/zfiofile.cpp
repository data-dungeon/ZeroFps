#include "zfiofile.h"


bool ZFIoFile::Open(char* file,bool bWritable) 
{
	if(bWritable)
		m_kFile = fopen(file, "w+");
	else
		m_kFile = fopen(file, "r");

	if(!m_kFile)
		return false;
	else
		return true;
}

bool ZFIoFile::Close() 
{
	int result=fclose(m_kFile);
	
	if(result==0)
		return true;
	else
		return false;

}

bool ZFIoFile::Write(void *data,int iSize)
{
	int result = fwrite(data,iSize,1,m_kFile);
	if(result==1)
		return true;
	else
		return false;
}

bool ZFIoFile::Read(void *data,int iSize)
{
	int result = fread(data,iSize,1,m_kFile);
	if(result==1)
		return true;
	else
		return false;
}

int ZFIoFile::GetPos()
{
	return ftell(m_kFile);
}

bool ZFIoFile::SetPos(int iPos)
{
	fseek(m_kFile,iPos,SEEK_SET);
	return true;
}
