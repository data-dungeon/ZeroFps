#ifndef _ZFMEMPACKAGE_H_
#define _ZFMEMPACKAGE_H_

#include <vector>
#include "basic_x.h"

using namespace std;


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





