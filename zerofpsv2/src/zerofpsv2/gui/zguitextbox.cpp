// Textbox.cpp: implementation of the Textbox class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/zguiskin.h"
#include "zguilabel.h"
#include "../render/zguirenderer.h"
#include "zguitextbox.h"
#include "zgui.h"
#include "zguiscrollbar.h"
//#include "../engine/input.h"
#include "../basic/globals.h"
#include <stdio.h>
#include <typeinfo>

#define VERT_SCROLLBAR_TEXBOX_ID 22
#define HORZ_SCROLLBAR_TEXBOX_ID 23

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
	m_iCursorRow = 1	;
	m_iRenderDistFromTop = 0;
	m_bScrollbarUpdated = false;
	m_iNumRows = 0;
	m_bMultiLine = bMultiLine;
	m_iStartrow = 0;
	m_iCursorPos = 0;
	m_bBlinkCursor = false;
	m_iCurrMaxText = 0;
	m_pkScrollbarVertical = NULL;

	CreateInternalControls();

	m_bTextFit = true; 

	SetWindowFlag(WF_CANHAVEFOCUS); // textboxar har focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow

	m_kRowOffsets.reserve(1000); 
	m_kRowOffsets.assign(1000,0);
	
	m_kRowOffsets[0] = 0;
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

	if(m_pkGUI)
	{
		int curr_res_x, curr_res_y;
		m_pkGUI->GetResolution(curr_res_x, curr_res_y);
		Rescale(m_iResolutionX, m_iResolutionY, curr_res_x, curr_res_y);
	}

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_strText != NULL)
	{
		Rect kRect = GetScreenRect();
		kRect.Top += 4; 
		//kRect.Left += m_kHorzOffset;

		int cursor_pos = m_bBlinkCursor ? m_iCursorPos : -1;
		if(m_bReadOnly)
			cursor_pos = -1;

		int iLetters, iRows;
		pkRenderer->RenderText(m_strText, kRect, cursor_pos, 
			m_iRenderDistFromTop, m_bMultiLine, iLetters, iRows);
		m_iNumRows = iRows;

	/*	if(m_bMultiLine == false && (unsigned int) iLetters < strlen(m_strText))
			m_bTextFit = false;
		else*/
			m_bTextFit = true;
	}

	if(m_pkScrollbarVertical)
		m_pkScrollbarVertical->Render(pkRenderer);

	if(!m_bEnabled)
	{
		// Copy back the current background color
		memcpy(m_pkSkin->m_afBkColor,afBkColorBuffer,sizeof(float)*3);
	}

	if(m_bMultiLine && !m_bScrollbarUpdated)
		UpdateScrollbar();

	return true;
}


bool ZGuiTextbox::ProcessKBInput(int iKey)
{	
	if(m_bReadOnly == true)
		return true;

	if(iKey == gKEY_RETURN)
	{
		if(m_bMultiLine == true)
			iKey = '\n';
		else
			return true;
	}

	if(iKey == gKEY_LSHIFT || iKey == gKEY_RSHIFT)
		return false;

	if( (iKey == gKEY_DOWN && m_bMultiLine == true) ||
		(iKey == gKEY_UP   && m_bMultiLine == true) )
	{
		if(iKey == gKEY_DOWN)
			MoveDownOneRow();
		else
			MoveUpOneRow();

		int y_pos = m_iRenderDistFromTop + m_iCursorRow * m_pkFont->m_cCharCellSize; 

		bool bScroll = false;

		if(iKey == gKEY_DOWN &&
			y_pos >= GetScreenRect().Height()-m_pkFont->m_cCharCellSize)
			bScroll = true;

		if(iKey == gKEY_UP &&
			y_pos <= m_pkFont->m_cCharCellSize)
			bScroll = true;

		if(bScroll)
			ScrollText(m_iCursorRow);

		return true;
	}

	if(iKey == gKEY_LEFT)
	{
		if(m_iCursorPos > 0)
			m_iCursorPos--;
		else
			return false;

		int cursor_row = GetCursorRow();

		int y_pos = m_iRenderDistFromTop + cursor_row * m_pkFont->m_cCharCellSize; 

		if(y_pos <= m_pkFont->m_cCharCellSize)
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

	if(iKey == gKEY_RIGHT && m_strText)
	{
		if((unsigned)m_iCursorPos < strlen(m_strText))
			m_iCursorPos++;	
		else
			return false;

		int cursor_row = GetCursorRow();

		int y_pos = m_iRenderDistFromTop + cursor_row * m_pkFont->m_cCharCellSize; 

		if(y_pos >= GetScreenRect().Height()-m_pkFont->m_cCharCellSize)
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

	if(iKey == gKEY_END && m_strText)
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

	if(iKey == gKEY_HOME)
	{
		if(m_bMultiLine == false)
			m_iRenderDistFromTop = 0;

		m_iCursorPos = 0;

		if(m_pkScrollbarVertical)
			m_pkScrollbarVertical->SetScrollPos(0);
		return true;
	}

	if(iKey == gKEY_ESCAPE || (iKey == gKEY_RETURN && m_bMultiLine == false) )
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

	if(iKey == gKEY_BACKSPACE && m_strText)
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
		//return UpdateScrollbar();
	}

	if(iKey == gKEY_DELETE && m_strText)
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
		//return UpdateScrollbar();
	}

	if(!bSkip)
	{
		unsigned int uiSzLength=0;
		if(m_strText)
			uiSzLength = strlen(m_strText); 
		if( uiSzLength >= (unsigned int) (m_iCurrMaxText-2) || uiSzLength == 0)
			ResizeTextBuffer(10);
	}

	if(m_bTextFit == true && !bSkip)
	{
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

/*	int iRows = GetNumRows(m_strText);
	if(!bSkip && iRows > 1 && m_bMultiLine == false && m_iCursorPos != 0)
	{
		// Remove last character put.
		string strLazy = m_strText;
		strLazy.erase(m_iCursorPos-1,1);
		strcpy(m_strText, strLazy.c_str());
		m_iCursorPos--;
	}*/

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
	//if(iKey == gKEY_RETURN)
	{
		int* piParams = new int[1];
		piParams[0] = GetID(); // Listbox ID
		GetGUI()->GetActiveCallBackFunc()(
			GetGUI()->GetActiveMainWnd(), ZGM_EN_CHANGE, 1, piParams);
		delete[] piParams;
	}
	
	UpdateScrollbar();

	return true;
}

void ZGuiTextbox::SetFocus()
{
/*	m_iCursorPos = 0;

	if(m_strText)
	{
		m_iCursorPos = strlen(m_strText);
	}*/
	m_bBlinkCursor = true;

	return; //

//	Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));

	bool bMouseLeftPressed = false;
	int iClickPosX=0, iClickPosY=0;

	ZGui* pkGui = GetGUI();
	if(pkGui)
	{
		bMouseLeftPressed = pkGui->m_bMouseLeftPressed;
		iClickPosX = pkGui->m_iMouseX;
		iClickPosY = pkGui->m_iMouseY;
	}

	if(bMouseLeftPressed/*pkInput->Pressed(MOUSELEFT)*/)
	{
	/*	int iClickPosX, iClickPosY;
		pkInput->MouseXY(iClickPosX,iClickPosY);
		iClickPosX -= GetScreenRect().Left;
		iClickPosY -= GetScreenRect().Top;*/

		// Placera markören där man klickade.
		if(m_pkFont != NULL && m_strText != NULL)
		{
			int rowheight = (m_pkFont != NULL) ? m_pkFont->m_cCharCellSize : 12;
			int row = m_iStartrow + iClickPosY/rowheight;
			if(row < m_iNumRows)
				m_iCursorPos = m_kRowOffsets[row]; 

			int last_row_pos;
			if(row != m_iNumRows-1)
				last_row_pos = m_kRowOffsets[row+1]-1;
			else
				last_row_pos = 100000000;

			int pixel_offset=0;
			while(pixel_offset < iClickPosX && m_iCursorPos < last_row_pos)
			{
				int cell = m_strText[m_iCursorPos];
				int font_size = 0;
				
				if(cell != '\n')
					font_size = m_pkFont->m_aChars[cell].iSizeX;

				if( m_strText[m_iCursorPos] != ' ' &&
					m_strText[m_iCursorPos] != '\n')
					font_size += m_pkFont->m_cPixelGapBetweenChars;

				pixel_offset += font_size;
				m_iCursorPos++;
			}
		}
	}
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

	UpdateScrollbar();

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
			{
				cb(pkActiveWnd, ZGM_EN_CHANGE, 1, piParams);
				delete[] piParams;
			}
		}
	}
}

void ZGuiTextbox::CreateInternalControls()
{
	if(m_bMultiLine)
	{
		Rect rc = GetWndRect();
		int x = rc.Width();
		int y = 0;
		int w = 20;
		int h = rc.Height();

		if(m_pkScrollbarVertical)
			delete m_pkScrollbarVertical;

		m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
			this,true,VERT_SCROLLBAR_TEXBOX_ID);
		m_pkScrollbarVertical->SetAutoHide(false);  
		m_pkScrollbarVertical->SetScrollInfo(0,100,1.0f,0); 
		m_pkScrollbarVertical->Show();
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

	int row_size = 12;

	if(m_pkFont)
		row_size = m_pkFont->m_cCharCellSize;

	m_iRenderDistFromTop = -row_size*m_iStartrow;

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;
}

void ZGuiTextbox::ScrollText(int row)
{	
	if(m_pkScrollbarVertical == NULL)
		return;

	unsigned int min,max,pos;
	m_pkScrollbarVertical->GetScrollInfo(min,max,pos);
	float fPos = (float)(max-min) * ((float)row / m_iNumRows);
	m_pkScrollbarVertical->SetScrollPos((int)fPos);
	
	ZGui* pkGui = GetGUI();
	if(pkGui)
	{
		int prev_cursor_pos = m_iCursorPos;
		pkGui->SetFocus(this);
		m_iCursorPos = prev_cursor_pos;
	}
}

void ZGuiTextbox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
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

	if(m_bNumberOnly == true)
	{
		if(iKey >= gKEY_0 && iKey <= gKEY_9)
			bIgnore = false;
		else
			bIgnore = true;
	}

	if( iKey == gKEY_BACKSPACE || iKey == gKEY_DELETE || iKey == gKEY_LEFT || 
		iKey == gKEY_RIGHT	   || iKey == gKEY_END    || iKey == gKEY_HOME ||
		iKey == gKEY_RETURN )// || iKey == gKEY_DOWN || iKey == gKEY_UP)
		bIgnore = false;

	if(m_bNumberOnly == false && (iKey >= 32 && iKey < 256) )
		bIgnore = false;

	if(iKey == gKEY_LSHIFT || iKey == gKEY_RSHIFT)
		bIgnore = true;

	return bIgnore;
}

int ZGuiTextbox::GetNumRows(char* text)
{
	if(!m_pkFont)
	{
		//printf("Can't get num rows because szTextbox have no font!\n");
		return -1;
	}

	if(text == NULL || strlen(text) < 1)
	{
		//printf("Can't get num rows because string is bad!\n");
		return -1;
	}

	int characters_totalt = strlen(text);
	int width = GetScreenRect().Width();
	int xpos=0, ypos=0, row_height = m_pkFont->m_cCharCellSize;
	int rows = 0, offset = 0, max_width = width;

	pair<int,int> kLength; // first = character, second = pixels.

	while(offset < characters_totalt) // antal ord
	{
		int prev_row = rows;

		kLength = GetWordLength(text, offset, max_width);

		if(xpos+kLength.second >= width)
		{
			// Räkna inte in sista tecknets längd om det är ett space.
			// och om längden på ordet överskrider den tillåtna längden.
			if(text[offset+kLength.first-1] == ' ')
			{
				int index = ' ' - 32;
				kLength.second -= m_pkFont->m_aChars[index].iSizeX;
			}

			if(xpos+kLength.second >= width)
			{
				if(text[offset+kLength.first-1] == ' ')
				{
					int index = ' ' - 32;
					kLength.second += m_pkFont->m_aChars[index].iSizeX;
				}

				rows++;
				ypos += row_height;
				xpos = 0;
			}
		}

		if(prev_row != rows)
		{
			m_kRowOffsets[rows] = offset;

			if(m_iCursorPos < m_kRowOffsets[rows] &&
			   m_iCursorPos >= m_kRowOffsets[rows-1])
			   m_iCursorRow = rows-1;

			prev_row = rows;
		}

		bool bRowBreak = false;
		
		if(text[offset+kLength.first-1] == '\n')
		{	
			rows++;
			ypos += row_height;	
			xpos = 0;

			bRowBreak = true;
		}

		if(bRowBreak == false)
			xpos += kLength.second;

		offset += kLength.first;

		if(prev_row != rows)
		{
			m_kRowOffsets[rows] = offset;

			if(m_iCursorPos < m_kRowOffsets[rows] &&
			   m_iCursorPos >= m_kRowOffsets[rows-1])
			   m_iCursorRow = rows-1;
		}
	}

	return rows;
}

pair<int,int> ZGuiTextbox::GetWordLength(char *text, int offset, int max_width)
{
	int char_counter = 0;
	int length_counter = 0;

	int iMax = strlen(text);
	for(int i=offset; i<iMax; i++)
	{
		if(text[i] == '\n')
		{
			char_counter++;
			break;
		}

		int index = text[i]-32;
		if(index < 0 || index > 255)
		{
			char_counter++;
			continue;
		}

		if(text[i] != '\n')
			length_counter += m_pkFont->m_aChars[index].iSizeX;

		// Break words bigger then then the length
		// of one row in the textbox.
		if(length_counter >= max_width)
			break;

		char_counter++;

		if(text[i] == ' ')
			break;

	}

	return pair<int,int>(char_counter, length_counter);
}

int ZGuiTextbox::GetNumRows()
{
	return GetNumRows(m_strText);
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
	if(m_bMultiLine == false || m_pkScrollbarVertical == NULL)
		return true;

	int max_visible_row;

	if(m_pkFont != NULL)
		max_visible_row = GetScreenRect().Height() / m_pkFont->m_cCharCellSize;

	int rows = GetNumRows();
	if(rows == -1)
		return false;

	int rows_to_much = rows-max_visible_row;
	
	// Får alla elementen plats? Nehe, hur många för mycket är det då?
	int iRowSize = m_pkFont->m_cCharCellSize * rows;
	int iTextboxSize = GetScreenRect().Height();
	if(iRowSize <= 0) iRowSize = 1; // don´t devide by zero
	float fThumbSize = (float) iTextboxSize / (float) iRowSize;
	m_pkScrollbarVertical->SetScrollInfo(0,rows,fThumbSize,0);

	m_pkScrollbarVertical->Show();

	m_bScrollbarUpdated = true;

	return true;

}

void ZGuiTextbox::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
	UpdateScrollbar();
}

void ZGuiTextbox::MoveDownOneRow()
{
	if(m_pkFont == NULL)
		return;

	int curr_row = 0;
	int curr_x=0;
	int old_cursor_pos = m_iCursorPos;

	for(int j=0; j<m_iNumRows; j++)
	{
		if(old_cursor_pos == m_kRowOffsets[j])
		{
			if(j+1 != m_iNumRows)
			{
				m_iCursorRow = j+1;
				m_iCursorPos = m_kRowOffsets[m_iCursorRow];
			}
			return;
		}
	}

	for(int i=0; i<m_iNumRows; i++)
	{
		if(i==m_iNumRows-1)
		{
			m_iCursorRow = m_iNumRows-1;
			m_iCursorPos = strlen(m_strText);
			return;
		}

		// Är nästa rads offset större än cursor pos?
		if(m_kRowOffsets[i+1] >= m_iCursorPos)
		{
			// Registera nuvarande rad.
			curr_row = i;

			// Räkna ut x värdet på current cursor pos.
			int char_counter  = m_kRowOffsets[i];
			int curr_row_length = 0; 

			while(m_strText[char_counter] != '\n' && char_counter < m_iCursorPos)
			{
				int letter_size = 0;
				int gap_size = 0;

				int element = m_strText[char_counter];
				
				letter_size = m_pkFont->m_aChars[element].iSizeX;  

				if(element != ' ')
					gap_size = m_pkFont->m_cPixelGapBetweenChars;

				curr_row_length += (letter_size + gap_size);

				char_counter++;
			}

			// Flytta m_iCursorPos till nästa rad och motsvarande x pos
			// eller mindre (om raden är kortare)

			m_iCursorPos = m_kRowOffsets[i+1];
			m_iCursorRow = i+1;
			int pixel_counter = 0; 

			while(1)
			{
				if(pixel_counter >= curr_row_length)
					break;

				int letter_size = 0;
				int gap_size = 0;

				int element = m_strText[m_iCursorPos];

				if(element == '\n')
					break;

				letter_size = m_pkFont->m_aChars[element].iSizeX;  

				if(element != ' ')
					gap_size = m_pkFont->m_cPixelGapBetweenChars;

				pixel_counter += (letter_size + gap_size);

				m_iCursorPos++;
			}

			break;
		}
	}
}

void ZGuiTextbox::MoveUpOneRow()
{
	if(m_pkFont == NULL)
		return;

	int curr_row = 0;
	int curr_x=0;
	int old_cursor_pos = m_iCursorPos;

	if(m_iCursorPos < m_kRowOffsets[1])
	{
		m_iCursorPos = 0;
		m_iCursorRow = 0;
		return;
	}

	for(int j=0; j<m_iNumRows; j++)
	{
		if(old_cursor_pos == m_kRowOffsets[j])
		{
			m_iCursorPos = m_kRowOffsets[j-1];
			m_iCursorRow = j-1;
			return;
		}
	}

	for(int i=0; i<m_iNumRows; i++)
	{
		// Är nästa rads offset större än cursor pos?
		if(i == m_iNumRows-1 || m_kRowOffsets[i+1] >= m_iCursorPos)
		{
			// Registera nuvarande rad.
			curr_row = i;

			// Räkna ut x värdet på current cursor pos.
			int char_counter  = m_kRowOffsets[i];
			int curr_row_length = 0; 

			while(1)
			{
				if(char_counter >= m_iCursorPos)
				{
					break;
				}
				else
				{
					int letter_size = 0;
					int gap_size = 0;

					int element = m_strText[char_counter];

					if(element == '\n')
						break;
					
					letter_size = m_pkFont->m_aChars[element].iSizeX;  

					if(element != ' ')
						gap_size = m_pkFont->m_cPixelGapBetweenChars;

					curr_row_length += (letter_size + gap_size);
				}

				char_counter++;
			}

			// Flytta m_iCursorPos till nästa rad och motsvarande x pos
			// eller mindre (om raden är kortare)

			m_iCursorPos = m_kRowOffsets[i-1];
			m_iCursorRow = i-1;
			int pixel_counter = 0; 

			while(1)
			{
				if(pixel_counter >= curr_row_length)
					break;

				int letter_size = 0;
				int gap_size = 0;

				int element = m_strText[m_iCursorPos];

				if(element == '\n')
					break;

				letter_size = m_pkFont->m_aChars[element].iSizeX;  

				if(element != ' ')
					gap_size = m_pkFont->m_cPixelGapBetweenChars;

				pixel_counter += (letter_size + gap_size);

				m_iCursorPos++;
			}

			break;
		}
	}
}

int ZGuiTextbox::GetCursorRow()
{
	int row = 0;

	for(unsigned int i=0; i<m_kRowOffsets.size(); i++)
	{
		if(m_iCursorPos > m_kRowOffsets[i] &&
		   m_iCursorPos < m_kRowOffsets[i+1])
		{
			row = i;
			return row;
		}
	}

	return row;
}

void ZGuiTextbox::ToggleMultiLine(bool bMultiLine)
{
	m_bMultiLine = bMultiLine;
	CreateInternalControls();
}

void ZGuiTextbox::SetReadOnly(bool bReadOnly)
{
	m_bReadOnly = bReadOnly;
}

bool ZGuiTextbox::SetCursorRow(int row, bool bUpdate)
{
	if(row < 0 || row > m_iNumRows-1)
		return false;

	m_iCursorRow = row;
	m_iCursorPos = m_kRowOffsets[m_iCursorRow];
	
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
