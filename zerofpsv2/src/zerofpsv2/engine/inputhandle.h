#ifndef _ENGINE_INPUTHANDLE_H_
#define _ENGINE_INPUTHANDLE_H_

#include "input.h"
#include "engine_x.h"

class ZSSInput;

class ENGINE_API InputHandle
{
	private:
		string		m_strHandleName;
		ZSSInput*	m_pkInput;		
		bool			m_bActive;
		bool			m_bTempDisable;

	public:
		InputHandle(string strHandleName);
		~InputHandle();
		
		bool IsActive() {return m_bActive;};
		void SetActive(bool bValue) {m_bActive = bValue;};				
		void SetTempDisable(bool bValue) {m_bTempDisable = bValue;};
		
		// INPUT WRAPER FUCTIONS
		
		QueuedKeyInfo GetQueuedKey();
		int SizeOfQueue();
		
		void MouseXY(float &fX,float &fY);
		void UnitMouseXY(float &fX,float &fY);
		void SDLMouseXY(int &iX,int &iY);
		void RelMouseXY(float &fX,float &fY);
		void SetCursorInputPos(float x, float y);		
		
		bool Pressed(Buttons eButton);		
		bool VKIsDown(string strName);
		
		void Reset();
		
		friend class ZSSInput;
};



#endif



