#include "inputhandle.h"
#include "zerofps.h"
 
/*       START OF INPUT HANDLE CLASS
*/
 
InputHandle::InputHandle(string strHandleName) 
{
	m_pkInput			= static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));		
	m_strHandleName	= strHandleName;
	m_bActive			= false;

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

// INPUT WRAPER FUNCTIOINS

QueuedKeyInfo InputHandle::GetQueuedKey()
{
	if(m_bActive)
		return m_pkInput->GetQueuedKey();
	else
		return QueuedKeyInfo(-1,0);
}

int InputHandle::SizeOfQueue()
{
	if(m_bActive)
		return m_pkInput->SizeOfQueue();
	else
		return 0;
}

void InputHandle::MouseXY(int &iX,int &iY)
{
	if(m_bActive)
		m_pkInput->MouseXY(iX,iY);
}

void InputHandle::SDLMouseXY(int &iX,int &iY)
{
	if(m_bActive)
		m_pkInput->SDLMouseXY(iX,iY);
	else
	{
		iX = 0;
		iY = 0;
	}
}

void InputHandle::UnitMouseXY(float &fX,float &fY)
{	
	if(m_bActive)
		m_pkInput->UnitMouseXY(fX,fY);
}

void InputHandle::RelMouseXY(int &iX,int &iY)
{
	if(m_bActive)
		m_pkInput->RelMouseXY(iX,iY);
}

void InputHandle::SetCursorInputPos(int x, int y)
{
	if(m_bActive)
		m_pkInput->SetCursorInputPos(x,y);
}


bool InputHandle::Pressed(Buttons eButton)
{
	if(m_bActive)
		return m_pkInput->Pressed(eButton);
	else
		return false;
}


bool InputHandle::VKIsDown(string strName)
{
	if(m_bActive)
		return m_pkInput->VKIsDown(strName);
	else
		return false;
}

void InputHandle::Reset()
{
	if(m_bActive)
		m_pkInput->Reset();
}

/*
void InputHandle::ShowCursor(bool bShow)
{
	if(m_bActive)
		m_pkInput->ShowCursor(bShow);
}
*/

/*bool InputHandle::VKBind(string strName, Buttons kKey, int iIndex )
{
	if(m_bActive)
		return m_pkInput->VKBind(strName, kKey, iIndex);
	else
		return false;
}
bool InputHandle::VKBind(string strName, string strKeyName)
{
	if(m_bActive)
		return m_pkInput->VKBind(strName, strKeyName);
	else
		return false;
}*/