// Textbox.cpp: implementation of the Textbox class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/zguiskin.h"
#include "zguilabel.h"
#include "../render/zguirenderer.h"
#include "zguitextbox.h"
#include "zgui.h"
#include "zguiscrollbar.h"
#include "../engine/input.h"
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
	m_iCursorRow = 1	;
	m_iRenderDistFromTop = 0;
	m_bScrollbarUpdated = false;
	m_iNumRows = 0;
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

		int cursor_pos = m_bBlinkCursor ? m_iCursorPos : -1;

		int iLetters, iRows;
		pkRenderer->RenderText(m_strText, kRect, cursor_pos, 
			m_iRenderDistFromTop, m_bMultiLine, iLetters, iRows);
		m_iNumRows = iRows;

		if(m_bMultiLine == false && iLetters < strlen(m_strText))
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

	if(m_bMultiLine && !m_bScrollbarUpdated)
		UpdateScrollbar();

	return true;
}


bool ZGuiTextbox::ProcessKBInput(int nKey)
{
	if(IgnoreKey(nKey))
		return true;

	if(nKey == KEY_RETURN && m_bMultiLine == true)
	{
		nKey = '\n';
	}

	if(nKey == KEY_SPACE)
	{
		printf("--------------\n");
		for(int x=0; x<10; x++)
		{
			printf("%i\n", m_kRowOffsets[x]);
		}
		printf("--------------\n");
		return true;
	}

	if( (nKey == KEY_DOWN && m_bMultiLine == true) ||
		(nKey == KEY_UP   && m_bMultiLine == true) )
	{
		if(nKey == KEY_DOWN)
			MoveDownOneRow();
		else
			MoveUpOneRow();

		int y_pos = m_iRenderDistFromTop + m_iCursorRow * m_pkFont->m_cCharCellSize; 

		bool bScroll = false;

		if(nKey == KEY_DOWN &&
			y_pos >= GetScreenRect().Height()-m_pkFont->m_cCharCellSize)
			bScroll = true;

		if(nKey == KEY_UP &&
			y_pos <= m_pkFont->m_cCharCellSize)
			bScroll = true;

		if(bScroll)
			ScrollText(m_iCursorRow);

		return true;
	}

	if(nKey == KEY_LEFT)
	{
		if(m_iCursorPos > 0)
			m_iCursorPos--;	

		int cursor_row = GetCursorRow();

		int y_pos = m_iRenderDistFromTop + cursor_row * m_pkFont->m_cCharCellSize; 

		if(y_pos <= m_pkFont->m_cCharCellSize)
			ScrollText( cursor_row );

		return true;
	}

	if(nKey == KEY_RIGHT && m_strText)
	{
		if((unsigned)m_iCursorPos < strlen(m_strText))
			m_iCursorPos++;	

		int cursor_row = GetCursorRow();

		int y_pos = m_iRenderDistFromTop + cursor_row * m_pkFont->m_cCharCellSize; 

		if(y_pos >= GetScreenRect().Height()-m_pkFont->m_cCharCellSize)
			ScrollText( cursor_row );

		printf("%i\n", m_iCursorPos);
		return true;
	}

	if(nKey == KEY_END && m_strText)
	{
		m_iCursorPos = strlen(m_strText);
		unsigned int min,max,pos;
		m_pkScrollbarVertical->GetScrollInfo(min,max,pos);
		m_pkScrollbarVertical->SetScrollPos(max-1);
		return true;
	}

	if(nKey == KEY_HOME)
	{
		m_iCursorPos = 0;
		m_pkScrollbarVertical->SetScrollPos(0);
		return true;
	}

	if(nKey == KEY_ESCAPE || (nKey == KEY_RETURN && m_bMultiLine == false) )
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

	if(nKey == KEY_BACKSPACE && m_strText)
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
		return UpdateScrollbar();
	}

	if(nKey == KEY_DELETE && m_strText)
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
		return UpdateScrollbar();
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
	//if(nKey == KEY_RETURN)
	{
		int* piParams = new int[1];
		piParams[0] = GetID(); // Listbox ID
		GetGUI()->GetActiveCallBackFunc()(
			GetGUI()->GetActiveMainWnd(), ZGM_EN_CHANGE, 1, piParams);
		delete[] piParams;
	}
	
	return UpdateScrollbar();
}

void ZGuiTextbox::SetFocus()
{
	m_iCursorPos = 0;
	m_bBlinkCursor = true;

	Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));

	if(pkInput->Pressed(MOUSELEFT))
	{
		int iClickPosX, iClickPosY;
		pkInput->MouseXY(iClickPosX,iClickPosY);

		iClickPosX -= GetScreenRect().Left;
		iClickPosY -= GetScreenRect().Top;

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
			while(1)
			{
				int cell = m_strText[m_iCursorPos];
				int font_size = 0;
				
				if(cell != '\n')
					font_size = m_pkFont->m_aChars[cell].iSizeX;

				if( m_strText[m_iCursorPos] != ' ' &&
					m_strText[m_iCursorPos] != '\n')
					font_size += m_pkFont->m_cPixelGapBetweenChars;

				pixel_offset += font_size;
				if( pixel_offset >= iClickPosX ||
					m_iCursorPos >= last_row_pos)
					break;

				m_iCursorPos++;
			}
		}
	}
}

void ZGuiTextbox::KillFocus()
{
	if(ZGuiWnd::m_pkWndClicked != m_pkScrollbarVertical->GetButton())
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

	UpdateScrollbar();
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
			{
				ScrollText(m_pkScrollbarVertical);
			}
			return true;
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

	if( iKey == KEY_BACKSPACE || iKey == KEY_DELETE || iKey == KEY_LEFT || 
		iKey == KEY_RIGHT || iKey == KEY_END || iKey == KEY_HOME ||
		iKey == KEY_RETURN || KEY_DOWN || KEY_UP)
		bIgnore = false;

	if(iKey >= 32 && iKey < 256)
		bIgnore = false;

	return bIgnore;
}

int ZGuiTextbox::GetNumRows(char* szText)
{
	if(!m_pkFont)
	{
		printf("Can't get num rows because szTextbox have no font!\n");
		return -1;
	}

	if(szText == NULL || strlen(szText) < 1)
	{
		printf("Can't get num rows because string is bad!\n");
		return -1;
	}

	int characters_totalt = strlen(szText);
	int width = GetScreenRect().Width();
	int xpos=0, ypos=0, row_height = m_pkFont->m_cCharCellSize;
	int rows = 0, offset = 0, max_width = width;

	pair<int,int> kLength; // first = character, second = pixels.

	while(offset < characters_totalt) // antal ord
	{
		kLength = GetWordLength(szText, offset, max_width);

		offset += kLength.first;
		xpos += kLength.second;

		bool bRowBreak = false, bCarriageReturn = false;

		if(xpos >= max_width)
			bRowBreak = true;
		if(szText[offset-1] == '\n')
			bCarriageReturn = true;

		if(bRowBreak || bCarriageReturn)
		{
			if(bCarriageReturn)
				xpos = 0;
			if(bRowBreak)
				xpos = kLength.second;

			ypos += row_height;
			rows++;
			m_kRowOffsets[rows] = offset;
		}
			
	}

	return rows;
}

/*int ZGuiTextbox::GetNumRows(char* szText)
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

	int characters_totalt = strlen(szText);
	int width = GetScreenRect().Width();
	int xpos=0, ypos=0, row_height = m_pkFont->m_cCharCellSize;
	int rows = 0, offset = 0, max_widht = width;

	pair<int,int> kLength; // first = character, second = pixels.

	static bool apa = true;

	while(1) // antal ord
	{
		kLength = GetWordLength(szText, offset, max_widht);
		
		offset += kLength.first;
		xpos += kLength.second;
		
		if(xpos >= width || szText[offset-1] == '\n')
		{
			// Registrera offset in i texten för första tecknet
			// på varje rad.
			rows++;

			if(rows > m_kRowOffsets.size())
				m_kRowOffsets.reserve(rows+100);

			m_kRowOffsets[rows] = offset;

			if(m_iCursorPos < m_kRowOffsets[rows] &&
			   m_iCursorPos >= m_kRowOffsets[rows-1])
			   m_iCursorRow = rows-1;

			ypos += row_height;	

			if(xpos >= width)
			{
				m_kRowOffsets[rows] -= kLength.first;
			}
			
			if(szText[offset-1] != '\n')
				xpos = kLength.second;
			else
				xpos = 0;
		}

		if(offset >= characters_totalt)
			break;
	}

	printf("antal rader: %i\n", rows);

	apa = false;

	return rows;
}*/

/*pair<int,int> ZGuiTextbox::GetWordLength(char *text, int offset)
{
	int char_counter = 0;
	int length_counter = 0;

	int iLength = strlen(text);
	for(int i=offset; i<iLength; i++)
	{
		int index = text[i];
		if(index < 0 || index > 255)
			continue;

		if(text[i] != '\n')
			length_counter += m_pkFont->m_aChars[index].iSizeX;

		if(text[i] != ' ' && text[i] != '\n')
			length_counter += m_pkFont->m_cPixelGapBetweenChars;

		char_counter++;

		if(text[i] == ' ' || text[i] == '\n')
			break;
	}

	return pair<int,int>(char_counter, length_counter);
}*/

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
		if(length_counter > max_width)
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
	if(m_bMultiLine == false)
		return true;

	if(m_pkFont != NULL)
		m_iMaxVisibleRows = GetScreenRect().Height() / m_pkFont->m_cCharCellSize;

	int rows = GetNumRows();
	if(rows == -1)
		return false;

	int rows_to_much = rows-m_iMaxVisibleRows;

	int iRows = rows;
	
	// Får alla elementen plats? Nehe, hur många för mycket är det då?
	int iRowSize = m_pkFont->m_cCharCellSize * iRows;
	int iTextboxSize = GetScreenRect().Height();
	if(iRowSize <= 0) iRowSize = 1; // don´t devide by zero
	float fThumbSize = (float) iTextboxSize / (float) iRowSize;
	m_pkScrollbarVertical->SetScrollInfo(0,iRows,fThumbSize,0);

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

	for(int i=0; i<m_kRowOffsets.size(); i++)
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
