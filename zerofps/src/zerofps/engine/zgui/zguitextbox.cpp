// Textbox.cpp: implementation of the Textbox class.
//
//////////////////////////////////////////////////////////////////////

#include "../../basic/zguiskin.h"
#include "zguilabel.h"
#include "../../render/zguirenderer.h"
#include "zguitextbox.h"
#include "zgui.h"
#include "zguiscrollbar.h"
#include "../../engine/input.h"
#include <stdio.h>
#include "../input.h"

#define VERT_SCROLLBAR_TEXBOX_ID 22
#define HORZ_SCROLLBAR_TEXBOX_ID 23

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiTextbox::ZGuiTextbox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kArea, pkParent, bVisible, iID)
{
	m_iStartrow = 0;
	m_iCursorPos = 0;
	m_bBlinkCursor = false;
	m_iCurrMaxText = 0;
	m_iFontSize = 12;

	CreateInternalControls();

	int Width = kArea.Width() - m_pkScrollbarVertical->GetScreenRect().Width() - m_iFontSize;
	int Height = kArea.Height() - 10;

	m_iMaxCharsOneRow = Width / m_iFontSize;
	m_iMaxVisibleRows = Height / m_iFontSize;
}

ZGuiTextbox::~ZGuiTextbox()
{

}

bool ZGuiTextbox::Render( ZGuiRender* pkRenderer )
{
	if(m_iBkMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(),(m_iBkMaskTexture > 0)); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_strText != NULL)
	{
		if(m_pkTextSkin && m_pkTextSkin->m_iBkTexID != -1)
		{
			if(m_iTextMaskTexture > 0)
				pkRenderer->SetMaskTexture(m_iTextMaskTexture);

			pkRenderer->SetSkin(m_pkTextSkin); 

/*			if(m_bBlinkCursor)
			{
				static int oka;
				static char ch = '|';

				if(oka == 50)
				{
					if(ch == '|')
						ch = ' ';
					else
						ch = '|';

					oka = 0;
				}

				oka++;

				m_strText[strlen(m_strText)-1] = ch;
			}
			else
				m_strText[strlen(m_strText)-1] = ' ';*/

			Rect apa = GetScreenRect();
			apa.Right -= m_pkScrollbarVertical->GetScreenRect().Width();
			apa.Top += 5;
			apa.Bottom -= 5;

			int dd = m_iStartrow-m_iMaxVisibleRows+1;
			if(dd<0) dd=0;

			pkRenderer->RenderText(m_strText+(m_iMaxCharsOneRow*(dd)), 
				apa, 12, m_bBlinkCursor ? m_iCursorPos : -1, (m_iTextMaskTexture > 0));
		}
	}

	m_pkScrollbarVertical->Render(pkRenderer);

	return true;
}


bool ZGuiTextbox::ProcessKBInput(unsigned long nKey)
{
	if(m_iCursorPos >= m_iCurrMaxText-2 || m_iCursorPos == 0)
		ResizeTextBuffer(10);

	if(nKey == '\b')
	{
		if(m_iCursorPos > 0)
		{
			m_iCursorPos--;
			m_strText[m_iCursorPos] = '\0';
		}
	}
	else
	{
		m_strText[m_iCursorPos] = nKey;
		m_iCursorPos++;
	}

	if(nKey == KEY_UP)
	{
		m_iCursorPos -= m_iMaxCharsOneRow;
		if(m_iCursorPos < 0)
			m_iCursorPos = 0;
	}

	if(nKey == KEY_DOWN)
		m_iCursorPos += m_iMaxCharsOneRow;

	// Scroll scrollbar
	int iCurrRow = m_iCursorPos / m_iMaxCharsOneRow;
	if(iCurrRow >= m_iMaxVisibleRows-1)
	{
		m_iStartrow = iCurrRow;
		if(m_iStartrow < 0)
			m_iStartrow = 0;

		Rect apa = GetScreenRect();
		apa.Right -= m_pkScrollbarVertical->GetScreenRect().Width();
		apa.Top += 5;
		apa.Bottom -= 5;

		int iMax = iCurrRow;
		int iMin = 0;
		int iElementSize = m_iFontSize * iMax;
		int iEditboxSize = apa.Height() - m_iFontSize; // lägg på lite extra för att slippa avrundingsfel...
		float fThumbSize = (float) iEditboxSize / (float) iElementSize;

		m_pkScrollbarVertical->SetScrollInfo(iMin,iMax,fThumbSize,iMax); 
	}
	
	return true;
}

void ZGuiTextbox::SetFocus()
{
	m_bBlinkCursor = true;
}

void ZGuiTextbox::KillFocus()
{
	m_bBlinkCursor = false;
}

// expand the text buffer to make more room
void ZGuiTextbox::ResizeTextBuffer( int nCharacters )
{
	if( m_iTextLength == 0 || m_iTextLength+nCharacters<0)
	{
		m_iTextLength = 5;
	}

	// create the new buffer
	char *strBuffer = new char[m_iTextLength+nCharacters];
	memset( strBuffer, 0, m_iTextLength+nCharacters );

	// copy over the text
	if( m_strText != NULL )
	{
		if(nCharacters)
			memcpy( strBuffer, m_strText, m_iTextLength );
		else
			memcpy( strBuffer, m_strText, m_iTextLength-nCharacters );

		// delete the old buffer
		delete[] m_strText;
	}

	m_iTextLength += nCharacters;
	m_strText = new char[m_iTextLength];
	memset( m_strText, 0, m_iTextLength );
	memcpy( m_strText, strBuffer, m_iTextLength );
	delete[] strBuffer;

	m_iCurrMaxText = m_iTextLength;
}

// överlagrad från Zguiwnd
void ZGuiTextbox::SetText(char* strText)
{
	if(strText == NULL)
		return;

	int iLength = strlen(strText)+1;

	if(iLength > 1)
	{
		if(m_strText != NULL)
		{
			delete[] m_strText;
			m_iTextLength = 0;
		}

		m_iTextLength = iLength;
		m_strText = new char[m_iTextLength];
		strcpy(m_strText, strText);
	}
	
	m_iCurrMaxText = strlen(strText)-1;
	m_iCursorPos = strlen(strText)-1;
}

void ZGuiTextbox::CreateInternalControls()
{
	Rect rc = GetWndRect();
	int x = rc.Width();
	int y = 0;
	int w = 20;
	int h = rc.Height();

	m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,VERT_SCROLLBAR_TEXBOX_ID); 

	m_pkScrollbarVertical->SetScrollInfo(0,0,1.0f,0); 
}

void ZGuiTextbox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight)
{
	m_pkScrollbarVertical->SetSkin(pkSkinScrollArea);
	m_pkScrollbarVertical->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);
}

bool ZGuiTextbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_MOVE)
	{
		if(pkWnd->GetID() == VERT_SCROLLBAR_TEXBOX_ID)
		{
			ScrollText(m_pkScrollbarVertical);
			return true;
		}
	}

	return true;
}

void ZGuiTextbox::ScrollText(ZGuiScrollbar* pkScrollbar)
{	
	m_iStartrow = pkScrollbar->GetPos() + m_iMaxVisibleRows - 1;
	if(m_iStartrow < 0)
		m_iStartrow = 0;

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;
}




