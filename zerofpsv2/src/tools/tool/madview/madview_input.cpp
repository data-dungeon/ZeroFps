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
	static float prev_x = 0, prev_y = 0;
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

		m_fObjRotX = curr_pos.y - press_pos.y + prev_x;
		m_fObjRotY = press_pos.x - curr_pos.x + prev_y;
	}
	else
	{
		pressed = false;
		prev_x = m_fObjRotX;
		prev_y = m_fObjRotY;
	}

	// Move in and out (Z) or Left and Right (X) with Right button...
	static float prev_x2 = 0, prev_y2 = 0;
	static Point press_pos2;
	static bool pressed2 = false;
	if(m_pkInputHandle->Pressed(MOUSERIGHT))
	{
		if(pressed2 == false)
		{
			m_pkInputHandle->SDLMouseXY(press_pos2.x, press_pos2.y);
			pressed2 = true;
		}

		Point curr_pos;
		m_pkInputHandle->SDLMouseXY(curr_pos.x, curr_pos.y);

		if(abs(curr_pos.x - press_pos2.x + prev_y2) > abs(curr_pos.y - press_pos2.y + prev_x2))
			kCamerPos.x += (float)(curr_pos.x - press_pos2.x + prev_y2) / 10000.0f;
		else
			kCamerPos.z += (float)(curr_pos.y - press_pos2.y + prev_x2) / 5000.0f;
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
	if(m_pkInputHandle->Pressed(MOUSEMIDDLE))
	{
		if(pressed3 == false)
		{
			m_pkInputHandle->SDLMouseXY(press_pos3.x, press_pos3.y);
			pressed3 = true;
		}

		Point curr_pos;
		m_pkInputHandle->SDLMouseXY(curr_pos.x, curr_pos.y);

		kCamerPos.y += (float)(curr_pos.y - press_pos3.y + prev_x3) / 5000.0f;
	}
	else
	{
		pressed3 = false;
		prev_x3 = kCamerPos.x;
		prev_y3 = kCamerPos.z;
	}

	m_pkCameraObject->SetWorldPosV(kCamerPos);
	
	char szText[100];
	sprintf(szText, "Camera: X:%.2f, Y:%.2f, Z:%.2f", kCamerPos.x, kCamerPos.y, kCamerPos.z);

	SetText("CameraPosLabel", szText);
};

//	Return 3D postion of mouse in world.
Vector3 MadView::Get3DMouseDir(bool bMouse)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkCamera->GetViewPortSize();
	kViewCorner = m_pkCamera->GetViewPortCorner();
	
	if(bMouse)
	{
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;

		if(m_pkCamera->GetViewMode() == Camera::CAMMODE_PERSP)
		{
			dir.Set(x*xp,y*yp,-1.5);
			dir.Normalize();
		}
		else
		{
			dir.Set(0,0,-1);
			//dir.Normalize();
			//return dir;
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

//	Returns 3D dir of mouse click in world.
Vector3 MadView::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkCamera->GetViewPortSize();
	kViewCorner = m_pkCamera->GetViewPortCorner();

	if(m_bMouse)
	{
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);		
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;
		
		if(m_pkCamera->GetViewMode() == Camera::CAMMODE_PERSP) 
		{
			dir.Set(x*xp,y*yp,-1.5);
			dir.Normalize();
		}
		else 
		{
			dir.x = x* m_pkCamera->GetOrthoSize().x;
			dir.y = y* m_pkCamera->GetOrthoSize().y;
			dir.z = -1.5; 
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

Entity* MadView::GetTargetObject()
{
	Vector3 start	= m_pkCamera->GetPos() + Get3DMousePos(true)*2;
	Vector3 dir		= Get3DMouseDir(true);

	start.Print();
	dir.Print();

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkEntityManager->TestLine(&kObjects,start,dir);
	
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i] == m_pkCameraObject)	continue;

		if(kObjects[i]->GetEntityID() <100000)
			continue;
		
		if(kObjects[i]->GetName() == "ZoneObject")
			continue;
		
		if(kObjects[i]->GetName() == "StaticEntity")
			continue;
		
		if(kObjects[i]->GetName() == "A t_serverinfo.lua")
			continue;
		
		
		float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
		if(d < closest)
		{
			closest = d;
			pkClosest = kObjects[i];
		}
	}
	
	return pkClosest;
}

int MadView::GetTargetTCS(Vector3* pkPos)
{
	Vector3 kStart	= m_pkCamera->GetPos();// + Get3DMousePos(true)*2;
	Vector3 kDir	= Get3DMouseDir(true);

	vector<Entity*> kObjects;		
	m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kObjects);
		
	float d;	
	Vector3 cp;
	float closest = 999999999;
	Entity* pkClosest = NULL;
	Vector3 kPickPos;		
	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//get mad property and do a linetest		
		if(kObjects[i]->GetProperty("P_Tcs"))
		{
			if(P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
			{
				if(mp->TestLine(kStart,kDir))
				{	
					cp = mp->GetLastColPos();
					d = kStart.DistanceTo(cp);
	
					if(d < closest)
					{
						closest = d;
						pkClosest = kObjects[i];
						kPickPos = cp;					
					}
				}
			}
		}
	}
	
	if(pkClosest)
	{
		*pkPos = kPickPos;
		return pkClosest->GetEntityID();
	}
		
	return -1;
}

bool MadView::DelayCommand()
{
	if(m_pkZeroFps->GetEngineTime() < m_fDelayTime)
		return true;

	m_fDelayTime = m_pkZeroFps->GetEngineTime() + float(0.3);
	return false;
}
