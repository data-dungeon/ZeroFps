#ifndef _BASIC_ZFIO_H_
#define _BASIC_ZFIO_H_

//#include <stdint.h>
#include "basic_x.h"

#include <string>

using namespace std;


/**	\brief	Interface to read/write data.
		\ingroup Basic
*/
class BASIC_API ZFIoInterface 
{
	public:
		bool Write_Str(const string& strString)
		{
			Write(strString.size()+1);
			Write((void*)strString.c_str(),(int)strString.size()+1,1);					
			return true;
		}
		
		bool Read_Str(string& strString)
		{
			int iSize;
			Read(iSize);
			char* czStr = new(char[iSize]);
			
			Read(czStr,iSize,1);	
			strString = czStr;
			delete czStr;					
			return true;
		}
		

		
		template <class Any> 		
		bool Write(Any type) 
			{
				return Write((void*)&type,sizeof(Any),1);		
			}
		
		template <class Any> 					
		bool Read(Any& type) 
			{
				return Read(&type,sizeof(Any),1);			
			}
		
	
		virtual bool Read  (void* pkData, int iSize, int iCount)=0;		// Read data 
		virtual bool Write (void* pkData, int iSize, int iCount)=0;		// Write data 

		virtual void Seek(int iPos, int iOrigin) = 0;						// Seek to place to read/write from.
		virtual int Tell() = 0;														// Tell current read/write pos.
		virtual int GetSize() = 0;													// Get Size of data buffer.
};

#endif



