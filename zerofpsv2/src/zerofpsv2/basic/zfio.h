#ifndef _BASIC_ZFIO_H_
#define _BASIC_ZFIO_H_

//#include <stdint.h>
#include "basic_x.h"

/*
class BASIC_API ZFIo {
	private:
		
	
	public:
		virtual bool Open(const char* file,bool bWriteable)=0;
		virtual bool Close()=0;
			
		virtual bool Write(void *data,int iSize)=0;
		virtual bool Read(void *data,int iSize)=0;
		
		virtual int GetPos()=0;
		virtual bool SetPos(int iPos)=0;
};*/

/// Interface to read/write data.
class BASIC_API ZFIoInterface 
{
	public:
		virtual bool Read  (void* pkData, int iSize, int iCount)=0;		// Read data 
		virtual bool Write (void* pkData, int iSize, int iCount)=0;		// Write data 

		virtual void Seek(int iPos, int iOrigin) = 0;						// Seek to place to read/write from.
		virtual int Tell() = 0;														// Tell current read/write pos.
		virtual int GetSize() = 0;													// Get Size of data buffer.
};

#endif



