// Label.cpp: implementation of the Label class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/zguiskin.h"
#include "zguilabel.h"
#include "../render/zguirenderer.h"
#include "zgui.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiLabel::ZGuiLabel(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{
	m_eTextAlignment = ZGLA_TopLeft;
	m_bEnabled = false; // labels are static by default
   m_bMultiLine = false;
	RemoveWindowFlag(WF_CANHAVEFOCUS); // fönster har focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
}

ZGuiLabel::~ZGuiLabel()
{

}

bool ZGuiLabel::Render( ZGuiRender* pkRenderer )
{
	pkRenderer->SetSkin(m_pkSkin); 
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_strText != NULL)
	{
		Rect r = GetScreenRect();

		if(m_pkFont)
		{
			pkRenderer->SetFont(m_pkFont);

			if(!m_bMultiLine)
			{
				int tw = m_pkFont->GetLength(m_strText);

				switch(m_eTextAlignment)
				{				
				case ZGLA_Center:
					r.Left = r.Left + r.Width()/2 - tw/2;
					break;

				case ZGLA_BottomRight:
					r.Left = r.Right - tw;
					r.Top = r.Bottom - m_pkFont->m_iRowHeight;
					break;
				}
			}

			if(m_bMultiLine)
			{
  				char text[1024];
				
				for(int i=0; i<m_vkMultiLineRows.size(); i++)
				{
					TEXT_TAG t = m_vkMultiLineRows[i];
					
					unsigned int row_length = t.iNumChars;
					char* dest  = m_strText + t.iPos;
					strncpy(text, dest, row_length);
					if(row_length <= strlen(dest)) 
						text[row_length] = 0;

					Rect b = r;

					b.Top = r.Top + t.iRow * t.iRowHeight;
					b.Bottom = b.Top + t.iRowHeight;

					pkRenderer->RenderText(text, b, -1, m_afTextColor);  
				}
			}
			else
				pkRenderer->RenderText(m_strText, r, -1, m_afTextColor);
		}
	}
	return true;
} 

bool ZGuiLabel::Notify(ZGuiWnd* pkWnd, int iCode)
{
	// Notify the parent...
	if(m_bEnabled)
	{
		if(iCode == NCODE_CLICK_UP)
		{
			m_pkParent->Notify(this, iCode); 
		}
	}
	return false;
}

void ZGuiLabel::SetText(char* strText, bool bResizeWnd)
{
	ZGuiWnd::SetText(strText, bResizeWnd);

	if(m_bMultiLine == false)
		return;

	m_vkMultiLineRows.clear();

	Rect r = GetScreenRect();

	TEXT_TAG t;
	t.iNumChars =strlen(m_strText);
	t.iPos = 0;
	t.iRow = 0;
	t.iRowHeight = m_pkFont->m_iRowHeight;
	t.x = r.Left;
	t.y = r.Top;
	t.pkFont = m_pkFont;
	t.afColor[0] = 0;
	t.afColor[1] = 0;
	t.afColor[2] = 0;

	vector<TEXT_TAG> m_kTextTags;
	m_kTextTags.push_back(t);

	map<int,int> mkMaxRowHeight;
	map<int,int> mkMaxBaselineHeight;

	int LEFT = 0, TOP = 0;

	const int WIDTH = GetWndRect().Width();

	int xPos = LEFT;
	int iRow = 0;

	int w = 0, row=0, max_row=0, max_baseline=0;
	int word_break_pos = -1;

	for(int i=0; i<m_kTextTags.size(); i++)
	{	
		int start_letter = m_kTextTags[i].iPos;
		int end_letter = start_letter + m_kTextTags[i].iNumChars;
		int start=start_letter;
		int row_width=0;

		if(m_kTextTags[i].pkFont->m_iRowHeight > max_row)
			max_row = m_kTextTags[i].pkFont->m_iRowHeight;
		if(m_kTextTags[i].pkFont->m_iPixelsAboveBaseLine > max_baseline)
			max_baseline = m_kTextTags[i].pkFont->m_iPixelsAboveBaseLine;
		
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
				t.y = 0;
				t.iRow = row;
				m_vkMultiLineRows.push_back(t); 

				bool bStillBiggerLastCharInTag = false;

				if(end == end_letter )
				{
					xPos = LEFT + w;
					w+=row_width;	

					if(xPos > WIDTH)
					{
						bStillBiggerLastCharInTag = true;
					}
				}
				else
				{
					max_row=t.pkFont->m_iRowHeight;
					max_baseline=t.pkFont->m_iPixelsAboveBaseLine;
					
					xPos = LEFT;
					row++;		
				}

				if(ch == '\n')
				{
					if(end == end_letter)
					{
						xPos = LEFT;
						row++;
						max_row=t.pkFont->m_iRowHeight;
						max_baseline=t.pkFont->m_iPixelsAboveBaseLine;
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

				if(bStillBiggerLastCharInTag)
				{
					max_row=t.pkFont->m_iRowHeight;
					max_baseline=t.pkFont->m_iPixelsAboveBaseLine;
					w=0;	
					xPos=LEFT;
					row++;
				}

				map<int,int>::iterator itRowHeight;
				itRowHeight = mkMaxRowHeight.find(t.iRow);
				if(itRowHeight != mkMaxRowHeight.end())
				{
					if(itRowHeight->second < max_row)
						itRowHeight->second = max_row;
				}
				else
					mkMaxRowHeight.insert(map<int,int>::value_type(t.iRow, max_row)); 

				map<int,int>::iterator itBaselineHeight;
				itBaselineHeight = mkMaxBaselineHeight.find(t.iRow);
				if(itBaselineHeight != mkMaxBaselineHeight.end())
				{
					if(itBaselineHeight->second < max_baseline)
						itBaselineHeight->second = max_baseline;
				}
				else
					mkMaxBaselineHeight.insert(map<int,int>::value_type(t.iRow, max_baseline)); 
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
}





