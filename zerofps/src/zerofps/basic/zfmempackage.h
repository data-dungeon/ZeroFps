#ifndef _BASIC_ZFMEMPACKAGE_H_
#define _BASIC_ZFMEMPACKAGE_H_

#include <vector>
#include "basic_x.h"
#include "zffile.h"

using namespace std;

#define DECLSPECIFIER __declspec(dllimport)
#define EXPIMP_TEMPLATE extern

/*EXPIMP_TEMPLATE template class DECLSPECIFIER Any;*/

class BASIC_API ZFMemPackage{
	private:
		vector<char> m_acData;	
		int m_iPos;
		
	public:
		ZFMemPackage();
	
		int GetSize();
		int GetPos();
		void SetPos(int iPos);
		void Clear();
	
		bool SaveToFile(ZFFile* pkFile);
		bool LoadFromFile(ZFFile* pkFile);
	
		bool Read(void* pData,int iSize);
		bool Write(void* pData,int iSize);		
	
		void* GetDataPointer() {return (void*)&m_acData[0];};
	
		template <class Any>
		bool Read(Any &data)
		{
			return Read((void*)&data,sizeof(data));
		}
		
		template <class Any>	
		bool Write(Any &data)
		{
			return Write((void*)&data,sizeof(data));
		}
};


#endif





