#ifndef _ENGINE_INPUTHANDLE_H_
#define _ENGINE_INPUTHANDLE_H_

#include "input.h"
#include "engine_x.h"

class Input;

class ENGINE_API InputHandle
{
	private:
		string	m_strHandleName;
		Input*	m_pkInput;		
		bool		m_bActive;

	public:
		InputHandle(string strHandleName);
		~InputHandle();
		
		bool IsActive() {return m_bActive;};
		void SetActive(bool bValue) {m_bActive = bValue;};				
		
		// INPUT WRAPER FUCTIONS
		
		QueuedKeyInfo GetQueuedKey();
		int SizeOfQueue();
		
		void MouseXY(int &iX,int &iY);
		void UnitMouseXY(float &fX,float &fY);
		void SDLMouseXY(int &iX,int &iY);
		void RelMouseXY(int &iX,int &iY);
		void SetCursorInputPos(int x, int y);		
		
		bool Pressed(Buttons eButton);		
		bool VKIsDown(string strName);
		
		void Reset();
		
		friend class Input;
};



#endif



