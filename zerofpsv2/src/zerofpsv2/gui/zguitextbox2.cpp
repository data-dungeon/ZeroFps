// Textbox.cpp: implementation of the Textbox class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/zguiskin.h"
#include "../basic/keys.h"
#include "zguilabel.h"
#include "../render/zguirenderer.h"
#include "zguitextbox.h"
#include "zgui.h"
#include "zguiresourcemanager.h"
#include "zguiscrollbar.h"
#include "../basic/globals.h"
#include <stdio.h>
#include <typeinfo>

#define VERT_SCROLLBAR_TEXBOX_ID 22
#define HORZ_SCROLLBAR_TEXBOX_ID 23

#define MARG_SIZE 4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiTextbox::ZGuiTextbox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, 
						 int iID, bool bMultiLine) : 
	ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_kHorzOffset = 0;
	m_bNumberOnly = false;
	m_bReadOnly = false;
	m_iCursorRow = 0	;
	m_iRenderDistFromTop = 0;
	m_iNumRows = 0;
	m_bMultiLine = bMultiLine;
	m_iStartrow = 0;
	m_iCursorPos = 0;
	m_bBlinkCursor = false;
	m_iCurrMaxText = 0;
	m_pkScrollbarVertical = NULL;

	CreateInternalControls();

	SetWindowFlag(WF_CANHAVEFOCUS); // textboxar har focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte anv�ndas som mainwindow

	m_kRowOffsets.reserve(1000); 
	m_kRowOffsets.assign(1000,0);
	
	m_pkRowOffsets = NULL;

	m_iTotalTextHeight = 0;

	m_pkGuiRender = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));
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

	static int s_iNumRowsBefore = -1;

	if(m_strText != NULL)
	{
		Rect kRect = GetScreenRect();

		kRect.Left += 2;
		kRect.Right -= 2;

		if(m_bMultiLine)
		{
			kRect.Top += 2;
			kRect.Bottom -= 2;

			if(m_pkScrollbarVertical->IsVisible())
				kRect.Right -= 17;
		}
		
		int cursor_pos = m_bBlinkCursor ? m_iCursorPos : -1;
		if(m_bReadOnly)
			cursor_pos = -1;

		//int iCharsPrinted, iRowsPrinted;
		//pkRenderer->RenderText(m_strText, kRect, cursor_pos, 
		//	m_iRenderDistFromTop, m_bMultiLine, iCharsPrinted, iRowsPrinted, 
		//	m_afTextColor);
		
		if(m_bMultiLine)
		{
			int yPos;
			int top = GetScreenRect().Top;
			int bottom = GetScreenRect().Bottom - MARG_SIZE * 2;

			int px = GetScreenRect().Left, py = top;

			pkRenderer->StartDrawText(); 

			for(int i=m_iCursorRow; i<m_kTextTags.size(); i++)
			{
				yPos = py + m_kTextTags[i].y+m_iRenderDistFromTop;
				if(yPos >= top && yPos + m_kTextTags[i].pkFont->m_iRowHeight <= bottom)
				{
					pkRenderer->DrawString(
						m_strText+m_kTextTags[i].iPos, m_kTextTags[i].iNumChars, 
						px + m_kTextTags[i].x, yPos, 
						m_kTextTags[i].afColor, m_kTextTags[i].pkFont);
				}
			}

		}
		else
		{
			pkRenderer->RenderText(m_strText, kRect, cursor_pos, m_afTextColor, NULL);
		}

	}

	if(!m_bEnabled)
	{
		// Copy back the current background color
		memcpy(m_pkSkin->m_afBkColor,afBkColorBuffer,sizeof(float)*3);
	}

	if(m_bMultiLine && s_iNumRowsBefore != m_iNumRows)
	{
		s_iNumRowsBefore = m_iNumRows;
		UpdateScrollbar();
	}

	if(m_pkScrollbarVertical)
		m_pkScrollbarVertical->Render(pkRenderer);

	return true;
}

void ZGuiTextbox::SetFocus()
{
	m_bBlinkCursor = true;

	if(m_strText)
		m_iCursorPos = strlen(m_strText);
	else
		m_iCursorPos = 0;

	return; 
}

void ZGuiTextbox::KillFocus()
{
	if(m_pkScrollbarVertical)
	{
		if(ZGuiWnd::m_pkWndClicked != m_pkScrollbarVertical->GetButton())
			m_bBlinkCursor = false;
	}
	else
		m_bBlinkCursor = false;
}


// �verlagrad fr�n Zguiwnd
void ZGuiTextbox::SetText(char* strText, bool bResizeWnd)
{
	if(m_strText)
		delete[] m_strText;

	m_strText = new char[strlen(strText)+1];
	strcpy(m_strText, strText);
	BuildTagList();
	BuildTextStrings();
	return;

	if(strText == NULL || strlen(strText) < 1)
	{
		delete[] m_strText;
		m_iTextLength = 1;
		m_strText = new char[m_iTextLength];
		m_strText[0] = '\0';
		return;
	}

	if(m_bNumberOnly)
		for(unsigned int i=0; i<strlen(strText); i++)
			if(strText[i] < 48 || strText[i] > 57)
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
	
	m_iCurrMaxText = strlen(strText);

	if(m_iCurrMaxText < 0)
		m_iCurrMaxText = 0;

	m_iCursorPos = strlen(strText);

	if(m_iCursorPos < 0)
		m_iCursorPos = 0;

	int* piParams = new int[1];
	piParams[0] = GetID(); // Listbox ID
	ZGui* pkGui = GetGUI();
	if(pkGui)
	{
		callbackfunc cb = pkGui->GetActiveCallBackFunc();

		if(cb)
		{
			ZGuiWnd* pkActiveWnd = pkGui->GetActiveMainWnd();

			if(pkActiveWnd)
				cb(pkActiveWnd, ZGM_EN_CHANGE, 1, piParams);
		}
	}

	delete[] piParams;
}

void ZGuiTextbox::CreateInternalControls()
{
	if(m_bMultiLine)
	{
		Rect rc = GetWndRect();
		int x = rc.Width()-20;
		int y = 0;
		int w = 20;
		int h = rc.Height();

		if(m_pkScrollbarVertical)
			delete m_pkScrollbarVertical;

		m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
			this,true,VERT_SCROLLBAR_TEXBOX_ID);
		m_pkScrollbarVertical->SetAutoHide(false);  
		m_pkScrollbarVertical->SetScrollInfo(0,100,1.0f,0); 
		m_pkScrollbarVertical->GetButton()->Show();
	}
	else
	{
		if(m_pkScrollbarVertical)
		{
			delete m_pkScrollbarVertical;
		}

		m_pkScrollbarVertical = NULL;
	}
}

void ZGuiTextbox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, 
								   ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight,
									ZGuiSkin* pkSkinTopBnUp, ZGuiSkin* pkSkinTopBnDown,
									ZGuiSkin* pkSkinBottomBnUp, ZGuiSkin* pkSkinBottomBnDown)
{
	if(m_pkScrollbarVertical)
	{
		m_pkScrollbarVertical->SetSkin(pkSkinScrollArea);
		m_pkScrollbarVertical->SetThumbButtonSkins(pkSkinThumbButton, 
			pkSkinThumbButtonHighLight);

		m_pkScrollbarVertical->SetScrollButtonDownSkins(pkSkinBottomBnUp, pkSkinBottomBnDown);
		m_pkScrollbarVertical->SetScrollButtonUpSkins(pkSkinTopBnUp, pkSkinTopBnDown);
	}
}

bool ZGuiTextbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_MOVE)
	{
		if(pkWnd->GetID() == VERT_SCROLLBAR_TEXBOX_ID)
		{
			if(m_pkScrollbarVertical)
			{
				ScrollText(m_pkScrollbarVertical);
			}
			return true;
		}
	}
	else
	if(iCode == NCODE_CLICK_UP)
	{
		if(m_pkScrollbarVertical)
		{
			ScrollText(m_pkScrollbarVertical);
		}
	}
	
	if(pkWnd == m_pkScrollbarVertical)
	{
		ZGui* pkGui = GetGUI();

		if(iCode == NCODE_CLICK_UP && pkGui)			
		{
			int prev_cursor_pos = m_iCursorPos;
			pkGui->SetFocus(this);
			m_iCursorPos = prev_cursor_pos;
		}
	}

	return true;
}

void ZGuiTextbox::ScrollText(ZGuiScrollbar* pkScrollbar)
{	
	if(pkScrollbar == NULL)
		return;

	m_iStartrow = pkScrollbar->GetPos();
	if(m_iStartrow < 0)
		m_iStartrow = 0;

	m_iRenderDistFromTop = 0; 

	set<int> m_kAdded;
	for(int i=0; i<m_kTextTags.size(); i++)
	{
		if(m_kTextTags[i].iRow < m_iStartrow)
		{
			if(m_kAdded.find(m_kTextTags[i].iRow) == m_kAdded.end())
			{
				m_iRenderDistFromTop -= m_kTextTags[i].iRowHeight;
				m_kAdded.insert(m_kTextTags[i].iRow);
			}
		}
	}

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;

	m_iCursorRow = m_iStartrow;
}

void ZGuiTextbox::ScrollText(int row)
{	
	if(m_pkScrollbarVertical == NULL)
		return;

	unsigned int min,max,pos;
	m_pkScrollbarVertical->GetScrollInfo(min,max,pos);
	float fPos = (float)(max-min) * ((float)row / m_iNumRows);
	m_pkScrollbarVertical->SetScrollPos((int)fPos);
	
	//ZGui* pkGui = GetGUI();
	//if(pkGui)
	//{
	//	int prev_cursor_pos = m_iCursorPos;

	//	if(!m_bReadOnly)
	//		pkGui->SetFocus(this);

	//	m_iCursorPos = prev_cursor_pos;
	//}
}

void ZGuiTextbox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*)m_pkSkin, string("Textbox")) );

	if(m_pkScrollbarVertical) // only multiline textbox have scrollbars
	{
		int iStart = pkSkinDesc.size(); 
		m_pkScrollbarVertical->GetWndSkinsDesc(pkSkinDesc);
		for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
			pkSkinDesc[i].second.insert(0, "Textbox: ");
	}
}

void ZGuiTextbox::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiTextbox))
	{
		if(m_pkScrollbarVertical)
			m_pkScrollbarVertical->CopyNonUniqueData( 
				((ZGuiTextbox*)pkSrc)->m_pkScrollbarVertical );
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
}


bool ZGuiTextbox::IsMultiLine()
{
	return m_bMultiLine;
}

bool ZGuiTextbox::UpdateScrollbar()
{
	if(m_bMultiLine == false || m_pkScrollbarVertical == NULL || 
		m_pkFont == NULL || m_iNumRows == -1)
		return true;

	float fThumbSize = (float) (GetScreenRect().Height()-(MARG_SIZE*2)-
		(m_pkScrollbarVertical->GetArrowButtonHeight()*2)) / (float)  m_iTotalTextHeight;

	m_pkScrollbarVertical->SetScrollInfo(0,m_iNumRows,fThumbSize,m_iCursorRow);

	return true;
}

void ZGuiTextbox::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
}

void ZGuiTextbox::ToggleMultiLine(bool bMultiLine)
{
	if(m_pkRowOffsets)
		delete[] m_pkRowOffsets;
	
	m_pkRowOffsets = new unsigned short[1000];

	m_bMultiLine = bMultiLine;
	CreateInternalControls();

	BuildTagList();
	BuildTextStrings();
}

void ZGuiTextbox::SetReadOnly(bool bReadOnly)
{
	m_bReadOnly = bReadOnly;
}

bool ZGuiTextbox::IsReadOnly()
{
	return m_bReadOnly;
}

void ZGuiTextbox::Resize(int Width, int Height, bool bChangeMoveArea)
{
	if(m_pkScrollbarVertical)
	{
		m_pkScrollbarVertical->SetPos(Width, 0, false, true);
		m_pkScrollbarVertical->Resize(20,Height);
	}

	ZGuiWnd::Resize(Width, Height, bChangeMoveArea); 
}











///////////////////////////////////////////////////////////////////////////////
// Processing

int ZGuiTextbox::GetCursorRow()
{
	int row = 0;

	for(unsigned int i=0; i<m_iNumRows; i++)
	{
		if(m_iCursorPos > m_pkRowOffsets[i] &&
		   m_iCursorPos < m_pkRowOffsets[i+1])
		{
			row = i;
			return row;
		}
	}

	return row;
}

bool ZGuiTextbox::SetCursorRow(int row, bool bUpdate)
{
	if(row < 0 || row > m_iNumRows-1)
		return false;

	m_iCursorRow = row;
	m_iCursorPos = m_pkRowOffsets[m_iCursorRow];
	
	if(bUpdate)
		ScrollText(m_iCursorRow);

	return true;
}

void ZGuiTextbox::ScrollRowIntoView(int row)
{
	if(row < 0)
		row = 0;

	ScrollText(row);
}

bool ZGuiTextbox::ProcessKBInput(int iKey)
{	
	if(m_bReadOnly == true)
		return true;

	if(iKey == KEY_RETURN)
	{
		if(m_bMultiLine == true)
			iKey = '\n';
		else
			return true;
	}

	if(iKey == KEY_LSHIFT || iKey == KEY_RSHIFT)
		return false;

	if( m_bMultiLine )
	{
		static int s_row_start=0;

		static int curr_line=0;

		if(iKey == KEY_DOWN)
		{
			/*if(m_iCursorRow < m_iNumRows)
			{
				m_iCursorRow++;
				m_iCursorPos = m_pkRowOffsets[m_iCursorRow];

				int max_visible_row = (GetScreenRect().Height()) / m_pkFont->m_iRowHeight;

				if(m_iCursorRow >= max_visible_row+s_row_start)
				{	
					ScrollText(m_iCursorRow-max_visible_row+1);
					s_row_start++;
				}
			}*/

			//if(m_iCursorRow < m_iNumRows)
			//{
			//	m_iRenderDistFromTop -= m_kTextStrings[m_iCursorRow].h;
			//	m_iCursorRow++;
			//	UpdateScrollbar();
			//}

			if(curr_line != m_iNumRows+1)
			{
				for(int i=0; i<m_kTextTags.size(); i++)
				{
					if(m_kTextTags[i].iRow == curr_line)
					{
						m_iRenderDistFromTop -= m_kTextTags[i].iRowHeight;
						curr_line++;
						break;
					}
				}
			}
			
			return true;
		}
		else
		{
			/*if(m_iCursorRow > 0)
			{
				m_iCursorRow--;
				m_iCursorPos = m_pkRowOffsets[m_iCursorRow];

				if(m_iCursorRow == s_row_start)
				{	
					ScrollText(m_iCursorRow);
					s_row_start--;
				}
			}*/
			
			//if(m_iCursorRow > 0)
			//{
			//	m_iRenderDistFromTop += m_kTextStrings[m_iCursorRow].h;
			//	m_iCursorRow--;
			//	UpdateScrollbar();
			//}

			if(curr_line != 0)
			{
				for(int i=0; i<m_kTextTags.size(); i++)
				{
					if(m_kTextTags[i].iRow == curr_line-1)
					{
						m_iRenderDistFromTop +=  m_kTextTags[i].iRowHeight;					
						curr_line--;
						break;
					}
				}
			}

			return true;
		}

		return true;
	}

	if(iKey == KEY_LEFT)
	{
		if(m_iCursorPos > 0)
			m_iCursorPos--;
		else
			return false;

		int cursor_row = GetCursorRow();

		int y_pos = m_iRenderDistFromTop + cursor_row * m_pkFont->m_iRowHeight; 

		if(y_pos <= m_pkFont->m_iRowHeight)
			ScrollText( cursor_row );

		if(m_bMultiLine == false)
		{
			m_iRenderDistFromTop = 0;

			int length_counter = 0;
			int length = m_iCursorPos;
			
			if((unsigned) length > strlen(m_strText) )
				length = strlen(m_strText);

			for(int i=0; i<length; i++)
			{
				int index = m_strText[i]-32;
				if(!(index < 0 || index > 255))
					length_counter += m_pkFont->m_aChars[index].iSizeX;
			}

			if(length_counter >= GetScreenRect().Width())
			{
				int dif = length_counter-GetScreenRect().Width();
				m_iRenderDistFromTop = dif; 
			}
		}

		return true;
	}

	if(iKey == KEY_RIGHT && m_strText)
	{
		if((unsigned)m_iCursorPos < strlen(m_strText))
			m_iCursorPos++;	
		else
			return false;

		int cursor_row = GetCursorRow();

		int y_pos = m_iRenderDistFromTop + cursor_row * m_pkFont->m_iRowHeight; 

		if(y_pos >= GetScreenRect().Height()-m_pkFont->m_iRowHeight)
			ScrollText( cursor_row );

		if(m_bMultiLine == false)
		{
			m_iRenderDistFromTop = 0;

			int length_counter = 0;
			int length = m_iCursorPos; //strlen(m_strText);

			for(int i=0; i<length; i++)
			{
				int index = m_strText[i]-32;
				if(!(index < 0 || index > 255))
					length_counter += m_pkFont->m_aChars[index].iSizeX;
			}

			if(length_counter >= GetScreenRect().Width())
			{
				int dif = length_counter-GetScreenRect().Width();
				m_iRenderDistFromTop = dif; 
			}
		}
		

		return true;
	}

	if(iKey == KEY_END && m_strText)
	{
		m_iCursorPos = strlen(m_strText);
		
		if(m_pkScrollbarVertical)
		{
			unsigned int min,max,pos;
			m_pkScrollbarVertical->GetScrollInfo(min,max,pos);
			m_pkScrollbarVertical->SetScrollPos(max-1);
		}

		if(m_bMultiLine == false)
		{
			m_iRenderDistFromTop = 0;

			int length_counter = 0;
			int length = m_iCursorPos; //strlen(m_strText);

			for(int i=0; i<length; i++)
			{
				int index = m_strText[i]-32;
				if(!(index < 0 || index > 255))
					length_counter += m_pkFont->m_aChars[index].iSizeX;
			}

			if(length_counter >= GetScreenRect().Width())
			{
				int dif = length_counter-GetScreenRect().Width();
				m_iRenderDistFromTop = dif; 
			}
		}

		return true;
	}

	if(iKey == KEY_HOME)
	{
		if(m_bMultiLine == false)
			m_iRenderDistFromTop = 0;

		m_iCursorPos = 0;

		if(m_pkScrollbarVertical)
			m_pkScrollbarVertical->SetScrollPos(0);
		return true;
	}

	if(iKey == KEY_ESCAPE || (iKey == KEY_RETURN && m_bMultiLine == false) )
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

	bool bSkip = false;

	if(iKey == KEY_BACKSPACE && m_strText)
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
		bSkip = true;
	}

	if(iKey == KEY_DELETE && m_strText)
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
		bSkip = true;
	}

	if(!bSkip)
	{
		unsigned int uiSzLength=0;
		if(m_strText)
			uiSzLength = strlen(m_strText); 
		if( uiSzLength >= (unsigned int) (m_iCurrMaxText-2) || uiSzLength == 0)
			ResizeTextBuffer(10);

		if((unsigned)m_iCursorPos == strlen(m_strText))
		{
			m_strText[m_iCursorPos] = iKey;
			m_iCursorPos++;
		}
		else
		{
			char szLetter[2] = {iKey, '\0'};
			string strLazy = m_strText;
			strLazy.insert(m_iCursorPos,szLetter);
			strcpy(m_strText, strLazy.c_str());
			m_iCursorPos++;
		}
	}

	if(m_bMultiLine == false)
	{
		m_iRenderDistFromTop = 0;

		int length_counter = 0;
		int length = strlen(m_strText);

		if(m_iCursorPos >= length)
		{
			for(int i=0; i<length; i++)
			{
				int index = m_strText[i]-32;
				if(!(index < 0 || index > 255))
					length_counter += m_pkFont->m_aChars[index].iSizeX;
			}

			if(length_counter >= GetScreenRect().Width())
			{
				int dif = length_counter-GetScreenRect().Width();
				m_iRenderDistFromTop = dif; 
			}
		}
	}

	// Send a message to the main winproc...
	//if(iKey == KEY_RETURN)
	{
		int* piParams = new int[1];
		piParams[0] = GetID(); // Listbox ID
		GetGUI()->GetActiveCallBackFunc()(
			GetGUI()->GetActiveMainWnd(), ZGM_EN_CHANGE, 1, piParams);
		delete[] piParams;
	}

	return true;
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

bool ZGuiTextbox::BuildTagList()
{
	m_kTextTags.clear();

	int textsize = strlen(m_strText);
	if(textsize < 1)
		return false;

	vector<int> kEnds;

	int token_pos = -1;
	float clr_mult = 1.0f / 255.0f;

	for(int i=0; i<textsize; i++)
	{
		if(m_strText[i] == '<')
			token_pos = i+1;
		if(m_strText[i] == '>')
		{
			if(token_pos != -1)
			{
				kEnds.push_back(i);

				char syntax[16];
				char data[128];

				strncpy(syntax, m_strText+token_pos, 3);
				syntax[3] = 0;

				strncpy(data, m_strText+token_pos+4, i-token_pos-4);
				data[i-token_pos-4] = 0;

				// Ta bort alla skr�ptecken ur data
				char temp[128], c=0, l=strlen(data);
				for(int j=0; j<l; j++)
					if(data[j] != ' ')
						temp[c++] = data[j];
				temp[c] = 0;
				strcpy(data, temp);
				
				TEXT_TAG token_data;
				token_data.iPos = token_pos; //i+1;
				
				if(strcmp(syntax,"clr")==0)
				{
					char red[5] = "\0\0\0\0", green[5] = "\0\0\0\0", blue[5]= "\0\0\0\0";
					char* colors[] = { red, green, blue };

					int a=0, b=0;
					for(int j=0; j<strlen(data); j++)
						if(data[j] != ',')
							colors[a][b++] = data[j];
						else
						{
							a++;
							b=0;
						}

					token_data.afColor[0] = clr_mult * (float) atoi(colors[0]);
					token_data.afColor[1] = clr_mult * (float) atoi(colors[1]);
					token_data.afColor[2] = clr_mult * (float) atoi(colors[2]);
				}
				else
				if(strcmp(syntax,"fnt")==0)
				{
					token_data.pkFont = m_pkResMan->Font(data);
				}
				else
				{
					ZFAssert(0, "Gui: Bad syntax in text file!\n");
					return false;
				}

				token_pos = -1;
				m_kTextTags.push_back(token_data);
			}
		}
	}

	vector<int> kNewTagPos;

	// Format text
	if(!kEnds.empty() && m_kTextTags.size() == kEnds.size())
	{
		int pos=0;
		for(int i=0; i<textsize; i++)
		{
			bool bIsTagLetter = false;
			for(int j=0; j<m_kTextTags.size(); j++)
			{
				if(i >= (m_kTextTags[j].iPos - 1) && i <= (kEnds[j]))
				{
					bIsTagLetter = true;
					if(m_strText[i] == '<')
						kNewTagPos.push_back(pos); 
					break;
				}
			}

			if(bIsTagLetter==false)
			{
				m_strText[pos] = m_strText[i];
				pos++;
			}
		}

		m_strText[pos] = '\0';
	}
	else
	{
		TEXT_TAG start;
		start.iPos = 0;
		start.iNumChars = strlen(m_strText);
		start.afColor[0] = (unsigned char) (255.0f * m_afTextColor[0]);
		start.afColor[1] = (unsigned char) (255.0f * m_afTextColor[1]);
		start.afColor[2] = (unsigned char) (255.0f * m_afTextColor[2]);
		start.pkFont = m_pkFont;
		m_kTextTags.push_back(start);

		return false;
	}

	// Copy the new tagpos to the taglist
	for(int i=0; i<kNewTagPos.size(); i++)
		m_kTextTags[i].iPos = kNewTagPos[i];

	// Add default font and color as a tag in the very beginning if no exist already
	if(kNewTagPos[0] != 0)
	{
		TEXT_TAG start;
		start.iPos = 0;
		start.afColor[0] = (unsigned char) (255.0f * m_afTextColor[0]);
		start.afColor[1] = (unsigned char) (255.0f * m_afTextColor[1]);
		start.afColor[2] = (unsigned char) (255.0f * m_afTextColor[2]);
		start.pkFont = m_pkFont;
		m_kTextTags.insert(m_kTextTags.begin(), start);
	}

	// Remove doubles
	ZGuiFont* pkPrevFont = m_pkResMan->Font("defguifont");
	float prevcolor[] = {0,0,0};

	vector<TEXT_TAG> temp;
	for(int i=0; i<m_kTextTags.size(); i++)
	{
		bool bAdd = true;

		for(int j=0; j<temp.size(); j++)
		{
			if(temp[j].iPos == m_kTextTags[i].iPos)
			{
				if(m_kTextTags[i].pkFont == NULL)
				{
					memcpy(temp[j].afColor, m_kTextTags[i].afColor, sizeof(float)*3);
				}
				else
				{
					temp[j].pkFont = m_kTextTags[i].pkFont;
				}
				bAdd = false;
			}
		}

		if(bAdd)
		{
			if(m_kTextTags[i].pkFont == NULL)
				m_kTextTags[i].pkFont = pkPrevFont;
			else
				pkPrevFont = m_kTextTags[i].pkFont;

			if(m_kTextTags[i].afColor[0] == -1)
				memcpy(m_kTextTags[i].afColor, prevcolor, sizeof(float)*3);
			else
				memcpy(prevcolor, m_kTextTags[i].afColor, sizeof(float)*3);
				 
			temp.push_back(m_kTextTags[i]);
		}
	}

	// Copy back
	m_kTextTags.clear();
	for(int i=0; i<temp.size(); i++)
		m_kTextTags.push_back(temp[i]); 

	// Calculate text length
	for(int i=0; i<m_kTextTags.size(); i++)
	{
		if(i+1 == m_kTextTags.size())
			m_kTextTags[i].iNumChars = strlen(m_strText)-m_kTextTags[i].iPos;
		else
			m_kTextTags[i].iNumChars = m_kTextTags[i+1].iPos-m_kTextTags[i].iPos;
	}

	return true;
}

void ZGuiTextbox::BuildTextStrings()
{
	if(m_bMultiLine == false || m_kTextTags.size() < 1)
		return;

	const int LEFT = MARG_SIZE, TOP = MARG_SIZE;
	const int TOTAL_LENGTH = strlen(m_strText);
	const int WIDTH = GetWndRect().Width() - 20 - (LEFT*2);
	int xPos = LEFT;
	int yPos = TOP;
	int iRow = 0, iLettersProcessed=0;

	vector<TEXT_TAG> kFinal;
	int w = 0, row=0, max_row=0;
	int word_break_pos = -1;

	for(int i=0; i<m_kTextTags.size(); i++)
	{	
		int start_letter = m_kTextTags[i].iPos;
		int end_letter = start_letter + m_kTextTags[i].iNumChars;
		int start=start_letter;
		int row_width=0;

		if(m_kTextTags[i].pkFont->m_iRowHeight > max_row)
			max_row = m_kTextTags[i].pkFont->m_iRowHeight;

		for(int j=start_letter; j<end_letter; j++)
		{			
			int ch = m_strText[j];
			w += m_kTextTags[i].pkFont->m_aChars[ch].iSizeX;  

			if(w >= WIDTH || j==end_letter-1 || ch == '\n')
			{
				int end = word_break_pos;
				if(j==end_letter-1)
					end = end_letter;
				if(ch == '\n')
					end = j+1;

				TEXT_TAG t = m_kTextTags[i];
				t.iPos = start;
				t.iNumChars = end-start;
				t.x = xPos;
				t.y = yPos;
				t.iRow = row;
				kFinal.push_back(t); 

				if(end == end_letter)
				{
					xPos = LEFT + w;
					w+=row_width;	
				}
				else
				{
					yPos += max_row; //t.pkFont->m_iRowHeight; 
					xPos = LEFT;
					row++;
					max_row=t.pkFont->m_iRowHeight;
				}

				if(ch == '\n')
				{
					if(end == end_letter)
					{
						yPos += max_row;
						xPos = LEFT;
						row++;
						max_row=t.pkFont->m_iRowHeight;
					}
					word_break_pos=j+1;
					start=j+1;
					w=0;
				}
				else
				{
					start=word_break_pos;
					w-=row_width;	
				}
			}
			else
			{
				if(ch == ' ' || ch == '\t')
				{
					word_break_pos = j+1;
					row_width = w;					
				}
			}
		}
	}

	m_kTextTags.clear();

	for(int i=0; i<kFinal.size(); i++)
		m_kTextTags.push_back(kFinal[i]); 

	m_iNumRows = 0;

	for(int i=0; i<row+1; i++)
	{
		int max_bl_height=0, max_row_height=0;
		for(int j=0; j<kFinal.size(); j++)
		{
			if(m_kTextTags[j].iRow == i)
			{
				if(m_kTextTags[j].pkFont->m_iPixelsAboveBaseLine > max_bl_height)
					max_bl_height = m_kTextTags[j].pkFont->m_iPixelsAboveBaseLine;
				if(m_kTextTags[j].pkFont->m_iRowHeight > max_row_height)
					max_row_height = m_kTextTags[j].pkFont->m_iRowHeight;
			}
		}

		for(int j=0; j<kFinal.size(); j++)
		{
			if(m_kTextTags[j].iRow == i)
			{
				m_kTextTags[j].iRowHeight = max_row_height;
				m_iTotalTextHeight += max_row_height;
				int diff = max_bl_height - m_kTextTags[j].pkFont->m_iPixelsAboveBaseLine;
				m_kTextTags[j].y += diff;
			}

			if(m_kTextTags[j].iRow > m_iNumRows)
				m_iNumRows = m_kTextTags[j].iRow;
		}
	}
}