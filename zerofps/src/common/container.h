/*	Richard Svensson

	This file describes the container wish contains "items"
	
*/

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <vector>
#include "itemproperty.h"
#include "../zerofps/engine/object.h"

using namespace std;

class COMMON_API Container 
{
	private:
		vector<Object*> 	m_kObjects;
		int*					m_piGrid;
		
		int					m_iSizeX;
		int					m_iSizeY;

		ItemProperty*	GetItemProperty(Object* pkObject);

		void	Clean();
		int&	GetID(int iX,int iY);
		bool	CheckIfAdded(Object* pkObject);
		bool	CheckFreeSlot(int iX,int iY,int iSizeX,int iSizeY);
		int	 GetItemPos(Object* pkObject);
		void	SetID(int iX,int iY,int iSizeX,int iSizeY,int iID);
		bool	GetFreeSlot(int iSizeX,int iSizeY,int& iX, int& iY);

	public:
	
		Container();
				
		void	SetSize(int iX,int iY);
		bool	AddItem(Object* pkObject);
		bool	AddItem(Object* pkObject,int iX,int iY);		
		bool	RemoveItem(Object* pkObject);
		Object*	GetItem(const char* acName);
		Object*	GetItem(int iX,int iY);
		int		GetFreeSlots();
		void	GetSize(int& iX,int& iY){iX = m_iSizeX;iY = m_iSizeY;};
		
		void	PrintContainer();

};

#endif










