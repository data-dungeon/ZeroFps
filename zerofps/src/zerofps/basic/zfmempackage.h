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
	
		template <class Any>
		bool Read(Any &data)
		{
			return Read((void*)&data,sizeof(Any));
/*		
			int iSize=sizeof(Any);
			char* datapointer=(char*)&data;
			
			if(m_acData.size() < m_iPos+iSize)
				return false;
			
			for(int i=0;i<iSize;i++)
			{
				datapointer[i] = m_acData[m_iPos+i];	
			}			
			
			m_iPos+=iSize;
			
			return true;*/
		}
		
		template <class Any>		
		bool Write(Any &data)
		{
			return Write((void*)&data,sizeof(Any));
/*			
			int iSize=sizeof(Any);			
			char* datapointer=(char*)&data;			
			
			//make the vector bigger if the data dont fit
			if(m_acData.size() < m_iPos+iSize)
				m_acData.resize(m_iPos+iSize);
				 
			for(int i=0;i<iSize;i++)
			{
				m_acData[m_iPos+i] = datapointer[i];
			}
			m_iPos+=iSize;
			
			return true;*/
		}
};


#endif





