#ifndef _BASIC_ZFFILE_H_
#define _BASIC_ZFFILE_H_

#include <string>
#include "fileio.h"
#include "zfio.h"
#include "zfiofile.h"
#include "basic_x.h"

using namespace std;

class BASIC_API ZFFile {
	private:
		bool m_bOpen;
		bool m_bWritable;
		string m_acName;
		
		ZFIo* m_kIo;
		
	public:
		ZFFile();
		ZFFile(const char* acName,bool bWritable);
		
		bool Open(const char* acName,bool bWritable);
		bool Close();
		
		bool Read(void* data,int iSize);
		bool Write(void* data,int iSize);

		template <class Any> 
		bool Read(Any &data)
		{
			return m_kIo->Read((void*)&data,sizeof(data));
		};
		
		template <class Any> 
		bool Write(Any &data)
		{
			return m_kIo->Write((void*)&data,sizeof(data));
		};

		int GetPos();
		bool SetPos(int iPos);
		
		void SetIo(ZFIo* kIo) {m_kIo=kIo;};
		ZFIo *GetIo() {return m_kIo;};
		
};



#endif



