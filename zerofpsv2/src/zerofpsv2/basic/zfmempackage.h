#ifndef _BASIC_ZFMEMPACKAGE_H_
#define _BASIC_ZFMEMPACKAGE_H_

#include <vector>
#include "basic_x.h"
//#include "zffile.h"
#include "zfvfs.h"
#include "zfio.h"

using namespace std;

#define DECLSPECIFIER __declspec(dllimport)
#define EXPIMP_TEMPLATE extern

/*EXPIMP_TEMPLATE template class DECLSPECIFIER Any;*/

/**	\brief	Memory area that one can read/write to.
		\ingroup Basic
*/
class BASIC_API ZFMemPackage : public ZFIoInterface
{
	private:
		vector<char> m_acData;	
		int m_iPos;
		
	public:
		ZFMemPackage();
	
		//int GetSize();
		//int GetPos();
		//void SetPos(int iPos);
		void Clear();
	
		bool SaveToFile(ZFVFile* pkFile);
		bool LoadFromFile(ZFVFile* pkFile);
	
		//bool Read(void* pData,int iSize);
		//bool Write(void* pData,int iSize);		
	
		void* GetDataPointer() {return (void*)&m_acData[0];};
	
		/*template <class Any>
		bool Read(Any &data)
		{
			return Read((void*)&data,sizeof(data));
		}*/
		
		/*template <class Any>	
		bool Write(Any &data)
		{
			return Write((void*)&data,sizeof(data));
		}*/

		bool Read  (void* pkData, int iSize, int iCount);					// Read data from file.
		bool Write (void* pkData, int iSize, int iCount);					// Write data to file.

		void Seek(int iPos, int iOrigin);										// Seek to part of file.
		int Tell();																		// Tell current file pointer pos.
		int GetSize();																	// Get Size of file in bytes.

};


#endif





