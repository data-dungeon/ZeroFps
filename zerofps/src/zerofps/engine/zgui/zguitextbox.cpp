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

ZGuiTextbox::ZGuiTextbox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID, bool bMultiLine) :
	ZGuiControl(kArea, pkParent, bVisible, iID)
{
	m_bMultiLine = bMultiLine;
	m_iStartrow = 0;
	m_iCursorPos = 0;
	m_bBlinkCursor = false;
	m_iCurrMaxText = 0;
	m_iFontSize = 12;

	CreateInternalControls();

	int Width = kArea.Width() - m_iFontSize;
	int Height = kArea.Height() - 10;

	m_iMaxCharsOneRow = 1; 
	m_iMaxVisibleRows = Height / m_iFontSize;

	if(m_iMaxVisibleRows < 1)
		m_iMaxVisibleRows = 1;

	SetWindowFlag(WF_CANHAVEFOCUS); // textboxar har focus by default
}

ZGuiTextbox::~ZGuiTextbox()
{

}

bool ZGuiTextbox::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	float afBkColorBuffer[3];

	if(!m_bEnabled)
	{
		// Copy the current background color to a buffer.
		memcpy(afBkColorBuffer,m_pkSkin->m_afBkColor,sizeof(float)*3);

		// Set current background color to gray (disabled).
		m_pkSkin->m_afBkColor[0] = 0.839f; // (1.0f / 255) * 214;
		m_pkSkin->m_afBkColor[1] = 0.827f; // (1.0f / 255) * 211;
		m_pkSkin->m_afBkColor[2] = 0.807f; // (1.0f / 255) * 206;
	}

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_strText != NULL)
	{
		Rect kRect = GetScreenRect();

		int cursor_pos = m_bBlinkCursor ? m_iCursorPos : -1;

		if(!pkRenderer->RenderText(m_strText, kRect, cursor_pos, false))
			m_iMaxCharsOneRow = 0;
		else
			m_iMaxCharsOneRow = 1;
	}

	if(m_bMultiLine)
		m_pkScrollbarVertical->Render(pkRenderer);

	if(!m_bEnabled)
	{
		// Copy back the current background color
		memcpy(m_pkSkin->m_afBkColor,afBkColorBuffer,sizeof(float)*3);
	}

	return true;
}


bool ZGuiTextbox::ProcessKBInput(int nKey)
{
	if(IgnoreKey(nKey))
		return true;

	if(nKey == KEY_LEFT)
	{
		if(m_iCursorPos > 0)
			m_iCursorPos--;	
		return true;
	}

	if(nKey == KEY_RIGHT)
	{
		if((unsigned)m_iCursorPos < strlen(m_strText))
			m_iCursorPos++;	
		return true;
	}

	if(nKey == KEY_END)
	{
		m_iCursorPos = strlen(m_strText);
		return true;
	}

	if(nKey == KEY_HOME)
	{
		m_iCursorPos = 0;
		return true;
	}

	if(nKey == KEY_ESCAPE || nKey == KEY_RETURN)
	{
		KillFocus();
		if(m_pkGUI)
		{
			ZGuiWnd* pkParent = GetParent();
			
			if(pkParent)
				m_pkGUI->SetFocus(pkParent);
			else
				printf("Textbox have no parent.\n");
		}
		else
		{
			printf("Textbox have no GUI pointer.\n");
		}
		return true;
	}

	if(nKey == KEY_BACKSPACE)
	{
		if(m_iCursorPos > 0)
		{
			if((unsigned)m_iCursorPos == strlen(m_strText))
			{
				m_iCursorPos--;
				m_strText[m_iCursorPos] = '\0';
			}
			else
			{
				string strLazy = m_strText;
				strLazy.erase(m_iCursorPos-1,1);
				strcpy(m_strText, strLazy.c_str());
				m_iCursorPos--;
			}
		}
		return true;
	}

	if(nKey == KEY_DELETE)
	{
		if((unsigned)m_iCursorPos < strlen(m_strText))
		{
			if((unsigned)m_iCursorPos == strlen(m_strText)-1)
			{
				m_strText[m_iCursorPos] = '\0';
			}
			else
			{
				string strLazy = m_strText;
				strLazy.erase(m_iCursorPos,1);
				strcpy(m_strText, strLazy.c_str());
			}
		}
		return true;
	}

	unsigned int uiSzLength=0;
	if(m_strText)
		uiSzLength = strlen(m_strText); 
	if( uiSzLength >= m_iCurrMaxText-2 || uiSzLength == 0)
		ResizeTextBuffer(10);

	if(m_iMaxCharsOneRow == 1)
	{
		if((unsigned)m_iCursorPos == strlen(m_strText))
		{
			m_strText[m_iCursorPos] = nKey;
			m_iCursorPos++;
		}
		else
		{
			char szLetter[2] = {nKey, '\0'};
			string strLazy = m_strText;
			strLazy.insert(m_iCursorPos,szLetter);
			strcpy(m_strText, strLazy.c_str());
			m_iCursorPos++;
		}
	}

	// Scroll scrollbar
	if(m_bMultiLine == false)
	{
/*		if(m_iCursorPos > m_iMaxCharsOneRow)
			m_iCursorPos = m_iMaxCharsOneRow;*/
	}
	else
	if(m_pkScrollbarVertical)
	{
		int iCurrRow = m_iCursorPos / m_iMaxCharsOneRow;
		if(iCurrRow >= m_iMaxVisibleRows-1)
		{
			m_iStartrow = iCurrRow;
			if(m_iStartrow < 0)
				m_iStartrow = 0;

			Rect kRect = GetScreenRect();
			kRect.Right -= m_pkScrollbarVertical->GetScreenRect().Width();
			kRect.Top += 5;
			kRect.Bottom -= 5;

			int iMax = iCurrRow;
			int iMin = 0;
			int iElementSize = m_iFontSize * iMax;
			int iEditboxSize = kRect.Height() - m_iFontSize; // lägg på lite extra för att 
															 // slippa avrundingsfel...
			float fThumbSize = (float) iEditboxSize / (float) iElementSize;

			m_pkScrollbarVertical->SetScrollInfo(iMin,iMax,fThumbSize,iMax); 
		}
	}

	// Send a message to the main winproc...
	if(nKey == KEY_RETURN)
	{
		int* piParams = new int[1];
		piParams[0] = GetID(); // Listbox ID
		GetGUI()->GetActiveCallBackFunc()(
			GetGUI()->GetActiveMainWnd(), ZGM_EN_CHANGE, 1, piParams);
		delete[] piParams;
	}
	
	return true;
}

void ZGuiTextbox::SetFocus()
{
	m_iCursorPos = 0;
	m_bBlinkCursor = true;

	Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));
	int iClickPosX, iClickPosY;
	pkInput->MouseXY(iClickPosX,iClickPosY);

	// Placera markören där man klickade.
	Rect rcTextbox = GetScreenRect();
	if(rcTextbox.Inside(iClickPosX,iClickPosY) && pkInput->Pressed(MOUSELEFT))
	{
		int iHorzOffset = iClickPosX-GetScreenRect().Left;
		int iVertOffset = iClickPosY-GetScreenRect().Top;

		if(m_pkFont && m_strText && strlen(m_strText) > 0)
		{
			// Adderar för vertikalt avstånd från ovankant.
			int row = iVertOffset / m_pkFont->m_cCharCellSize;
			int rows = GetNumRows();

			int i;

			for(i=0; i<row; i++)
				m_iCursorPos += GetRowLength(i);

			// Adderar för horizontellt avstånd från vänsterkant.
			int offset = 0;
			int row_length = GetRowLength(row);
			for( i=0; i<row_length; i++)
			{
				if(offset > iHorzOffset)
					break;

				int pos = m_strText[m_iCursorPos+i]-32;
				if(pos >= 0 && pos < 256)
					offset += m_pkFont->m_aChars[pos].iSizeX;
			}

			m_iCursorPos += i;
		}
	}
}

void ZGuiTextbox::KillFocus()
{
	m_bBlinkCursor = false;

/*	// Send a message to the main winproc...
	int* piParams = new int[1];
	piParams[0] = GetID(); // Listbox ID
	GetGUI()->GetActiveCallBackFunc()(
		GetGUI()->GetActiveMainWnd(), ZGM_EN_CHANGE, 1, piParams);
	delete[] piParams;*/
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
void ZGuiTextbox::SetText(char* strText, bool bResizeWnd)
{
	if(strText == NULL || strlen(strText) < 1)
	{
		delete[] m_strText;
		m_iTextLength = 1;
		m_strText = new char[m_iTextLength];
		m_strText[0] = '\0';
		return;
	}

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
	
	m_iCurrMaxText = strlen(strText);

	if(m_iCurrMaxText < 0)
		m_iCurrMaxText = 0;

	m_iCursorPos = strlen(strText);

	if(m_iCursorPos < 0)
		m_iCursorPos = 0;
}

void ZGuiTextbox::CreateInternalControls()
{
	Rect rc = GetWndRect();
	int x = rc.Width();
	int y = 0;
	int w = 20;
	int h = rc.Height();

	if(m_bMultiLine)
	{
		m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
			this,true,VERT_SCROLLBAR_TEXBOX_ID);
		m_pkScrollbarVertical->SetScrollInfo(0,0,1.0f,0); 
	}
	else
	{
		m_pkScrollbarVertical = NULL;
	}
}

void ZGuiTextbox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, 
								   ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight)
{
	if(m_pkScrollbarVertical)
	{
		m_pkScrollbarVertical->SetSkin(pkSkinScrollArea);
		m_pkScrollbarVertical->SetThumbButtonSkins(pkSkinThumbButton, 
			pkSkinThumbButtonHighLight);
	}
}

bool ZGuiTextbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_MOVE)
	{
		if(pkWnd->GetID() == VERT_SCROLLBAR_TEXBOX_ID)
		{
			if(m_pkScrollbarVertical)
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

void ZGuiTextbox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc)
{
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkin, string("Textbox")) );

	if(m_pkScrollbarVertical) // only multiline textbox have scrollbars
	{
		int iStart = pkSkinDesc.size(); 
		m_pkScrollbarVertical->GetWndSkinsDesc(pkSkinDesc);
		for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
			pkSkinDesc[i].second.insert(0, "Textbox: ");
	}
}

bool ZGuiTextbox::IgnoreKey(int iKey)
{
	bool bIgnore = true;

	if(iKey < 32)
		bIgnore = true;

	if( iKey == KEY_BACKSPACE || iKey == KEY_DELETE || iKey == KEY_LEFT || 
		iKey == KEY_RIGHT || iKey == KEY_END || iKey == KEY_HOME )
		bIgnore = false;

	if(iKey >= 32 && iKey < 256)
		bIgnore = false;

	return bIgnore;
}

int ZGuiTextbox::GetNumRows(char* szText)
{
	if(!m_pkFont)
	{
		printf("Can't get num rows because textbox have no font!\n");
		return -1;
	}

	if(szText == NULL || strlen(szText) < 1)
	{
		printf("Can't get num rows because string is bad!\n");
		return -1;
	}

	int iLength = m_pkFont->GetLength(szText);
	int iWidth = GetScreenRect().Width();

	return iLength/iWidth + 1;
}

int ZGuiTextbox::GetNumRows()
{
	return GetNumRows(m_strText);
}


///////////////////////////////////////////////////////////////////////////////
// Name: GetRowLength
// Description: Get number of characters on a row (search_row).
//				The row starts with 0.
int ZGuiTextbox::GetRowLength(int search_row)
{
	if(!m_pkFont)
	{
		printf("Can't get row length because textbox have no font!\n");
		return -1;
	}

	if(m_strText)
	{
		int textbox_width = GetScreenRect().Width();
		int text_width = 0, row_counter = 0, character_counter = 0;

		// Loopa igenom alla bokstäver i texten.
		for(int i=0; i < strlen(m_strText) ; i++)
		{
			// Öka textbredden med storleken på aktuellt tecken.
			text_width += m_pkFont->m_aChars[m_strText[i]-32].iSizeX;

			// Är textbredden störren än textboxens bredd?
			if(text_width >= textbox_width)
			{
				// Öka på rad räknaren,
				row_counter++;
				// Nollställ textbredden.
				text_width = 0;
			}
			// Är textbredden mindre än textboxens bredd?
			else
			{
				// Är den sökta raden lika med den aktuella raden?
				if(search_row == row_counter)
				{
					// Öka tecken räknaren, 
					character_counter++;
				}
			}

		}

		return character_counter;
	}

	return -1;
}