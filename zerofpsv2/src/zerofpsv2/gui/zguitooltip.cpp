// zguitooltip.cpp: implementation of the ZGuiToolTip class.
//
//////////////////////////////////////////////////////////////////////

#include "zguitooltip.h"
#include "zgui.h"
#include "zguiresourcemanager.h"
#include "../basic/zguifont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiToolTip::ZGuiToolTip(float fDisplayTime) : m_fDisplayTime(fDisplayTime)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkToolTipWnd = NULL;

	m_pkLastToolTipWnd = NULL;
	m_fToolTipDisplayTime = 0.0f;

	m_iPrevCursorX = m_iPrevCursorY = -1;

	m_pkToolTipWnd = new ZGuiLabel(Rect(300,300,400,400),  NULL, false, 432342);
	m_pkToolTipWnd->SetSkin(new ZGuiSkin( 255, 0, 0, 255, 0, 0,  2));
   ((ZGuiLabel*)m_pkToolTipWnd)->m_bMultiLine = true;
}

ZGuiToolTip::~ZGuiToolTip()
{

}

void ZGuiToolTip::Update(int mouse_x, int mouse_y, bool bMouseClick, float fGameTime)
{
	if(m_pkToolTipWnd->GetFont() == NULL )
		m_pkToolTipWnd->SetFont(m_pkGui->GetResMan()->Font("defguifont"));  

	if(!(mouse_x == m_iPrevCursorX && mouse_y == m_iPrevCursorY))
	{
		if(m_pkLastToolTipWnd)
		{
			if(!m_pkLastToolTipWnd->GetScreenRect().Inside(mouse_x,mouse_y))
				m_pkLastToolTipWnd = NULL;
		}
	}

	int screen_w, screen_h;
	m_pkGui->GetResolution(screen_w, screen_h);
	
	m_pkToolTipWnd->Hide();

	if(m_pkGui->m_bMouseLeftPressed || m_pkGui->m_bMouseRightPressed)
	{
		m_fToolTipDisplayTime = 0.0f;
		return;
	}

	for(unsigned int i=0; i<m_vkWindowList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkWindowList[i].pkWnd;
//		ZGuiWnd* pkParent = pkWnd->GetParent();

		if(pkWnd->IsVisible() )
		{
			if(pkWnd->GetScreenRect().Inside(mouse_x,mouse_y))
			{
				m_pkToolTipWnd->Show(); 
				m_pkToolTipWnd->SetText((char*)m_vkWindowList[i].strText.c_str(), false);

				Rect rc = pkWnd->GetScreenRect();




               int iHeight=0;

               // Change height...
               int iFontHeight = 16;
               if(m_pkToolTipWnd->GetFont())
                  iFontHeight = m_pkToolTipWnd->GetFont()->m_iRowHeight;

               int oka = 0;
               int iNumRows = 0;
               int iCurrentLength=0, max_length=0;
               int iTextLength = strlen(m_pkToolTipWnd->GetText());
               char* temp = new char[iTextLength+1];

               for(int i=0; i<iTextLength; i++)
               {
                  temp[oka] = m_pkToolTipWnd->GetText()[i];
                  iCurrentLength += m_pkToolTipWnd->GetFont()->m_aChars[temp[oka]].iSizeX;
                  oka++;

                  if(m_pkToolTipWnd->GetText()[i] == '\n' || i == iTextLength-1)
                  {
                     if(max_length < iCurrentLength)
                        max_length = iCurrentLength;

                     iHeight += iFontHeight;
                     iCurrentLength=0;
                     oka=0;
                  }
               }

               delete[] temp;

               m_pkToolTipWnd->Resize(max_length, iHeight); 





				int w = m_pkToolTipWnd->GetScreenRect().Width();
				int h = m_pkToolTipWnd->GetScreenRect().Height();

				int x = rc.Left + (rc.Right - rc.Left)/2 - w/2;
				int y = rc.Top - h;

				if(x + w > screen_w)
					x = screen_w-w-2;

				if(y + h > screen_h)
					y = screen_h-h-2;

				m_pkToolTipWnd->SetPos( x, y, true, true);




				if(m_pkLastToolTipWnd != pkWnd)
				{
					m_fToolTipDisplayTime = fGameTime;
					m_pkLastToolTipWnd = pkWnd;
				}
				break;
			}
		}
	}

	if(fGameTime - m_fToolTipDisplayTime > m_fDisplayTime)
	{
		m_pkToolTipWnd->Hide();
	}
	else
	{
		// show tooltip first after 0,75 sec
		if(fGameTime - m_fToolTipDisplayTime > 0.75f && m_pkLastToolTipWnd)
		{
			if(m_pkLastToolTipWnd->GetScreenRect().Inside(mouse_x,mouse_y))
				m_pkToolTipWnd->Show();
		}
		else
		{
			m_pkToolTipWnd->Hide();
		}
	}

	m_iPrevCursorX = mouse_x;
	m_iPrevCursorY = mouse_y;
}

void ZGuiToolTip::AddToolTip(ZGuiWnd* pkWnd, string strText)
{
	TOOLTIP_INFO ttinfo;

	ttinfo.pkWnd = pkWnd;
	ttinfo.strText = strText;

	m_vkWindowList.push_back(ttinfo);
}

void ZGuiToolTip::CloseAll()
{
	for(unsigned int i=0; i<m_vkWindowList.size(); i++)
		m_vkWindowList[i].pkWnd->Hide();

	m_pkLastToolTipWnd = NULL;
	m_pkToolTipWnd->Hide();
	m_fToolTipDisplayTime = 0.0f;
}

void ZGuiToolTip::RemoveToolTip(ZGuiWnd* pkWnd)
{
	vector<TOOLTIP_INFO>::iterator it = m_vkWindowList.begin();
	for( ; it != m_vkWindowList.end(); it++ )
	{
		if((*it).pkWnd == pkWnd)
		{
			if(m_pkLastToolTipWnd == pkWnd)
				m_pkLastToolTipWnd = NULL;

			m_vkWindowList.erase(it);
			break;
		}
	}
}



void ZGuiToolTip::SetSkin(ZGuiSkin kSkin)
{
	ZGuiSkin* pkSkin = m_pkToolTipWnd->GetSkin();
	memcpy(pkSkin, &kSkin, sizeof(ZGuiSkin));
}
