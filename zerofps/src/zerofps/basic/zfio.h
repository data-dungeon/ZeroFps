#ifndef _ZFIO_H_
#define _ZFIO_H_

//#include <stdint.h>

class BASIC_API ZFIo {
	private:
		
	
	public:
		virtual bool Open(const char* file,bool bWriteable)=0;
		virtual bool Close()=0;
			
		virtual bool Write(void *data,int iSize)=0;
		virtual bool Read(void *data,int iSize)=0;
		
		virtual int GetPos()=0;
		virtual bool SetPos(int iPos)=0;
};


#endif



