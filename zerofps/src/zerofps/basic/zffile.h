#ifndef _ZFFILE_H_
#define _ZFFILE_H_

#include <string>
#include "fileio.h"
#include "zfio.h"
#include "zfiofile.h"

using namespace std;

class ZFFile {
	private:
		bool m_bOpen;
		bool m_bWritable;
		string m_acName;
		
		ZFIo* m_kIo;
		
	public:
		ZFFile();
		ZFFile(char* acName,bool bWritable);
		
		bool Open(char* acName,bool bWritable);
		bool Close();
		
		bool Read(void* data,int iSize);
		bool Write(void* data,int iSize);

		int GetPos();
		bool SetPos(int iPos);
		
		void SetIo(ZFIo* kIo) {m_kIo=kIo;};
		ZFIo *GetIo() {return m_kIo;};
};



#endif



