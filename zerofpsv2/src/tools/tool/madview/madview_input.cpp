#include "madview.h"
#include "../../../zerofpsv2/engine/inputhandle.h"

void MadView::Input()
{
	if(GetWnd("MainMenu"))
	{
		if(((ZGuiMenu*)GetWnd("MainMenu"))->IsOpen())
		{
			ShowWnd("MadViewInfoWnd", false);
		}
		else
		{
			if(!IsWndVisible("SelectFileWnd"))
				ShowWnd("MadViewInfoWnd", true);	
		}
	}
	Vector3 kCamerPos = m_pkCameraObject->GetWorldPosV();

	if(m_pkInputHandle->Pressed(KEY_W))
		kCamerPos.z += 0.01f;
	if(m_pkInputHandle->Pressed(KEY_S))
		kCamerPos.z -= 0.01f;

	if(m_pkInputHandle->Pressed(KEY_Q))
		kCamerPos.y += 0.01f;
	if(m_pkInputHandle->Pressed(KEY_E))
		kCamerPos.y -= 0.01f;

	if(m_pkInputHandle->Pressed(KEY_A))
		kCamerPos.x += 0.01f;
	if(m_pkInputHandle->Pressed(KEY_D))
		kCamerPos.x -= 0.01f;

	if(m_pkInputHandle->Pressed(KEY_LEFT))
		m_fObjRotY-=1;
	if(m_pkInputHandle->Pressed(KEY_RIGHT))
		m_fObjRotY+=1;

	if(m_pkInputHandle->Pressed(KEY_UP))
		m_fObjRotX-=1;
	if(m_pkInputHandle->Pressed(KEY_DOWN))
		m_fObjRotX+=1;

	if(m_pkInputHandle->Pressed(KEY_PAGEUP))
		m_fObjRotZ-=1;
	if(m_pkInputHandle->Pressed(KEY_PAGEDOWN))
		m_fObjRotZ+=1;

	if(m_pkInputHandle->Pressed(KEY_SPACE) && !DelayCommand())
	{
		static int prev_mode = 0;

		if(m_iObjRotMode != 0)
		{
			prev_mode = m_iObjRotMode;
			m_iObjRotMode = 0;
		}
		else
		{
			m_iObjRotMode = prev_mode;
		}		
	}

	// Rotate object with Left mouse button...
	static float prev_x = 0, prev_y = 0, prev_z = 0;
	static Point press_pos;
	static bool pressed = false;
	if(m_pkInputHandle->Pressed(MOUSELEFT))
	{
		if(pressed == false)
		{
			m_pkInputHandle->SDLMouseXY(press_pos.x, press_pos.y);
			pressed = true;
		}

		Point curr_pos;
		m_pkInputHandle->SDLMouseXY(curr_pos.x, curr_pos.y);

		if(!m_pkInputHandle->Pressed(KEY_LSHIFT))
		{
			m_fObjRotX = curr_pos.y - press_pos.y + prev_x;
			m_fObjRotY = press_pos.x - curr_pos.x + prev_y;
		}
		else
		{
			m_fObjRotZ = curr_pos.y - press_pos.y + prev_z;
		}
	}
	else
	{
		pressed = false;
		prev_x = m_fObjRotX;
		prev_y = m_fObjRotY;
		prev_z = m_fObjRotZ;
	}

	// Move in and out (Z) or Left and Right (X) with Right button...
	static float prev_x2 = 0, prev_y2 = 0;
	static Point press_pos2;
	static bool pressed2 = false;
	//if(m_pkInputHandle->Pressed(MOUSERIGHT))
	if(m_pkInputHandle->Pressed(MOUSERIGHT) && !m_pkInputHandle->Pressed(KEY_LSHIFT))
	{
		if(pressed2 == false)
		{
			m_pkInputHandle->SDLMouseXY(press_pos2.x, press_pos2.y);
			pressed2 = true;
		}

		Point curr_pos;
		m_pkInputHandle->SDLMouseXY(curr_pos.x, curr_pos.y);
		if(press_pos2 != curr_pos)
		{
			if(abs(curr_pos.x - press_pos2.x + prev_y2) > abs(curr_pos.y - press_pos2.y + prev_x2))
				kCamerPos.x += (float)(curr_pos.x - press_pos2.x + prev_y2) / 10000.0f;
			else
				kCamerPos.z += (float)(curr_pos.y - press_pos2.y + prev_x2) / 5000.0f;
		}
	}
	else
	{
		pressed2 = false;
		prev_x2 = kCamerPos.x;
		prev_y2 = kCamerPos.z;
	}

	// Move Up and Down (Y) with 3rd Mouse button...
	static float prev_x3 = 0, prev_y3 = 0;
	static Point press_pos3;
	static bool pressed3 = false;
	//if(m_pkInputHandle->Pressed(MOUSEMIDDLE))
	if(m_pkInputHandle->Pressed(MOUSERIGHT) && m_pkInputHandle->Pressed(KEY_LSHIFT))
	{
		if(pressed3 == false)
		{
			m_pkInputHandle->SDLMouseXY(press_pos3.x, press_pos3.y);
			pressed3 = true;
		}

		Point curr_pos;
		m_pkInputHandle->SDLMouseXY(curr_pos.x, curr_pos.y);
		if(press_pos3 != curr_pos)
			kCamerPos.y += (float)(curr_pos.y - press_pos3.y + prev_x3) / 5000.0f;
	}
	else
	{
		pressed3 = false;
		prev_x3 = kCamerPos.x;
		prev_y3 = kCamerPos.z;
	}

	m_pkCameraObject->SetWorldPosV(kCamerPos);
	
	// Change text in CameraPosLabel and update window size if needed.
	char szText[100];
	sprintf(szText, "Camera: X:%.2f, Y:%.2f, Z:%.2f", kCamerPos.x, kCamerPos.y, kCamerPos.z);
	SetText("CameraPosLabel", szText, true);
	ZGuiWnd* pkWnd = GetWnd("CameraPosLabel");
	static int prev_width = 0;
	if(prev_width != pkWnd->GetWndRect().Width())
	{
		prev_width = pkWnd->GetScreenRect().Width()+10;
		GetWnd("MadViewInfoWnd")->Resize(prev_width, -1); 
	}
};

bool MadView::DelayCommand()
{
	if(m_pkZeroFps->GetEngineTime() < m_fDelayTime)
		return true;

	m_fDelayTime = m_pkZeroFps->GetEngineTime() + float(0.3);
	return false;
}
