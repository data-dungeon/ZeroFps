#include "inputhandle.h"
#include "zerofps.h"
 
/*       START OF INPUT HANDLE CLASS
*/
 
InputHandle::InputHandle(string strHandleName) 
{
	m_pkInput			= static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));		
	m_strHandleName	= strHandleName;
	m_bActive			= false;
	m_bTempDisable		= false;

	if(!m_pkInput)
	{
		cout<<"ERROR: input handle could not find input subsystem, prepare for a crash ;-) (make sure the handle is dynamicly created)"<<endl;
		return;
	}
	
	m_pkInput->RegisterInputHandle(this);
}
 
InputHandle::~InputHandle()
{
	if(m_pkInput)
	{
		m_pkInput->UnregisterInputHandle(this);	
	}
}

// INPUT WRAPPER FUNCTIOINS

QueuedKeyInfo InputHandle::GetQueuedKey()
{
	if(m_bActive && !m_bTempDisable)
		return m_pkInput->GetQueuedKey();
	else
		return QueuedKeyInfo(-1,0,true); 
}

int InputHandle::SizeOfQueue()
{
	if(m_bActive && !m_bTempDisable)
		return m_pkInput->SizeOfQueue();
	else
		return 0;
}

void InputHandle::MouseXY(float &fX,float &fY)
{
	if(m_bActive && !m_bTempDisable)
		m_pkInput->MouseXY(fX,fY);
}

void InputHandle::SDLMouseXY(int &iX,int &iY)
{
	if(m_bActive && !m_bTempDisable)
		m_pkInput->SDLMouseXY(iX,iY);
	else
	{
		iX = 0;
		iY = 0;
	}
}

void InputHandle::UnitMouseXY(float &fX,float &fY)
{	
	if(m_bActive && !m_bTempDisable)
		m_pkInput->UnitMouseXY(fX,fY);
}

void InputHandle::RelMouseXY(float &fX,float &fY)
{
	if(m_bActive && !m_bTempDisable)
		m_pkInput->RelMouseXY(fX,fY);
	else
	{
		fX = 0;
		fY = 0;	
	}
}

void InputHandle::SetCursorInputPos(float x, float y)
{
	if(m_bActive && !m_bTempDisable)
		m_pkInput->SetCursorInputPos(x,y);
}


bool InputHandle::Pressed(Buttons eButton)
{
	if(m_bActive && !m_bTempDisable)
		return m_pkInput->Pressed(eButton);
	else
		return false;
}


bool InputHandle::VKIsDown(string strName)
{
	if(m_bActive && !m_bTempDisable)
		return m_pkInput->VKIsDown(strName);
	else
		return false;
}

void InputHandle::Reset()
{
	if(m_bActive && !m_bTempDisable)
		m_pkInput->Reset();
}

