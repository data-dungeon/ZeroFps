#ifndef _BASIC_ZFIOFILE_H_
#define _BASIC_ZFIOFILE_H_

#include "zfio.h"
#include "basic_x.h"
#include <cstdio>
#include <errno.h>
#include "basic_x.h"

class BASIC_API ZFIoFile : public ZFIo {
	private:
		FILE *m_kFile;

	public:
		bool Open(const char* file,bool bWritable);
		bool Close();
			
		bool Write(void *data,int iSize);
		bool Read(void *data,int iSize);

		int GetPos();
		bool SetPos(int iPos);
};


#endif




