#ifndef _AUTO_LIST_H_
#define _AUTO_LIST_H_

#include <list>
#include <iostream>
#include "basic_x.h"

using namespace std;

template <class data_type>
class AutoList
{
	private:
		static list<data_type*>	m_kObjects;


	public:
		typedef typename list<data_type*>::iterator m_kIterator;
		typedef typename list<data_type*>::const_iterator m_kConstIterator;
		
		inline AutoList();
		inline virtual ~AutoList();

 		inline static m_kIterator Begin()			{	return m_kObjects.begin();	}; 		
 		inline static m_kIterator End()				{	return m_kObjects.end();	}; 		
 		inline static int Size()						{	return m_kObjects.size();	};
 		inline static bool Empty()						{	return m_kObjects.empty();	};
 		
};


// typename AutoList<data_type>::list<data_type*> AutoList<data_type>::m_kObjects;
template <class data_type>
list<data_type*> AutoList<data_type>::m_kObjects;
// typename list<data_type*> AutoList<data_type>::m_kObjects;


template <class data_type>
inline AutoList<data_type>::AutoList()
{
	m_kObjects.push_back(static_cast<data_type*>(this));	
}

template <class data_type>
inline AutoList<data_type>::~AutoList()
{
	m_kObjects.remove(static_cast<data_type*>(this));	
}


#endif

