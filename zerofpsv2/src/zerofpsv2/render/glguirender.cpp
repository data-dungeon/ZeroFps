// GLRender.cpp: implementation of the GLRender class.
//
///////////////////////////////////////////////////////////////////////////////
#include "../ogl/zfpsgl.h"
#include "glguirender.h" 
#include "../basic/zguiskin.h"
#include "texturemanager.h"
//#include "../engine/fh.h"
//#include "../engine/zfresource.h"
#include "../basic/zfresourcedb.h"

int g_bLettersPrinted=0;
int textDL=-1;

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
  
GLGuiRender::GLGuiRender()
{
	m_pkFont = NULL;
	m_pkSkin = NULL;
	m_iScreenWidth = 800;
	m_iScreenHeight = 600;
	m_bClipperEnabled = false;
}

bool GLGuiRender::StartUp()	
{ 
	m_pkTextureManger=static_cast<TextureManager*>
		(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	return true; 
}

bool GLGuiRender::ShutDown() 
{ 
	return true; 
}

bool GLGuiRender::IsValid()	{ return true; }

GLGuiRender::~GLGuiRender()
{

}

///////////////////////////////////////////////////////////////////////////////
// Name: SetDisplay
//
bool GLGuiRender::SetDisplay(int w, int h)
{
	m_iScreenWidth = w, 
	m_iScreenHeight = h;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: StartRender
//
bool GLGuiRender::StartRender()
{
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_FOG_BIT | 
		GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, m_iScreenWidth + 1, -1, m_iScreenHeight);

	//
	// Skala om projektions matrisen.
	//
	if(m_iScreenWidth == 1024 && m_iScreenHeight == 768)
	{
		glScalef(1.28f, 1.28f, 1.0f);
		glTranslatef(0,-169, 0);
	}
	else
	if(m_iScreenWidth == 1280 && m_iScreenHeight == 960)
	{
		glScalef(1.6f, 1.6f, 1.0f);
		glTranslatef(0,-360, 0);
	}
	else
	if(m_iScreenWidth == 1280 && m_iScreenHeight == 1024)
	{
		glScalef(1.6f, 1.706667f, 1.0f);
		glTranslatef(0,-430, 0);
	}
	else
	if(m_iScreenWidth == 1600 && m_iScreenHeight == 1200)
	{
		glScalef(2.0f, 2.0f, 1.0f);
		glTranslatef(0,-600, 0);
	}
	 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

/*	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
		
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: EndRender
//
bool GLGuiRender::EndRender()
{
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: RenderQuad
//
bool GLGuiRender::RenderQuad(Rect rc)
{
	if(!m_pkSkin)
		return false;

	float tx = 0.0,  ty = 0.0f; // offset into the texture
	float tw = 1.0f, th = -1.0f; // size of the texture

	if(m_bClipperEnabled)
	{
		float fHeightChange;

		if(rc.Top < m_rcClipperArea.Top)
		{
			fHeightChange = (float) (m_rcClipperArea.Top - rc.Top) / rc.Height();
			ty = 0.0f;
			th = -1.0f+fHeightChange;
			rc.Top = m_rcClipperArea.Top;
		}
		if(rc.Bottom > m_rcClipperArea.Bottom)
		{
			fHeightChange = (float) (m_rcClipperArea.Bottom - rc.Bottom) / rc.Height();
			ty = fHeightChange;
			th = -1.0f;
			rc.Bottom = m_rcClipperArea.Bottom;
		}
		if(rc.Height() <= 0)
			return true;

		if(rc.Left < m_rcClipperArea.Left)
			rc.Left = m_rcClipperArea.Left;
		if(rc.Right > m_rcClipperArea.Right)
			rc.Right = m_rcClipperArea.Right;
		if(rc.Width() <= 0)
			return true;
	}

	// Don't render if skin is transparent.
	if(m_pkSkin->m_bTransparent)
		return true;
	
	bool bMask = m_pkSkin->m_iBkTexAlphaID != -1;

	glLoadIdentity();

	float orginal_pos_x = (float) rc.Left;
	float orginal_pos_y = (float) rc.Top;
	
	bool bDrawMasked = (bMask == true && m_pkSkin->m_iBkTexAlphaID > 0) ? 
		true : false;

	
	int texture = m_pkSkin->m_iBkTexID;

	if(m_pkSkin->m_bTileBkSkin == true && texture > 0)
	{
		glEnable(GL_TEXTURE_2D);
		m_pkTextureManger->BindTexture( texture );

		int iTextureWidth, iTextureHeight;

		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iTextureWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iTextureHeight);

		tw = (float)(rc.Right-rc.Left) / iTextureWidth;
		th = -(float)(rc.Bottom-rc.Top) / iTextureHeight;
	}

	if(bDrawMasked)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);								
		glDisable(GL_DEPTH_TEST);								
		glBlendFunc(GL_DST_COLOR,GL_ZERO);		

		m_pkTextureManger->BindTexture( m_pkSkin->m_iBkTexAlphaID );

		glBegin(GL_QUADS);		
			glColor3f(1,1,1);
			glTexCoord2f(tx,ty);		glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
			glTexCoord2f(tx,th);		glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
			glTexCoord2f(tx+tw,th);  glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
			glTexCoord2f(tx+tw,ty);	  glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);    
		glEnd();							
	}
	 		
	if(texture > 0)
	{
		glEnable(GL_TEXTURE_2D);
		m_pkTextureManger->BindTexture( texture );

		if(bDrawMasked)
			glBlendFunc(GL_ONE, GL_ONE);	// Copy Image 1 Color To The Screen
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	glBegin(GL_QUADS);	 

/*		if(bDrawMasked)
			glColor3f(1,1,1);
		else*/
			glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],
				m_pkSkin->m_afBkColor[2]);

		glTexCoord2f(tx,ty);			glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
		glTexCoord2f(tx,th);			glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
		glTexCoord2f(tx+tw,th);		glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
		glTexCoord2f(tx+tw,ty);		glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);   


	glEnd();

	if(bDrawMasked)
		glDisable(GL_BLEND); // Disable Blending

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: RenderBorder
//
bool GLGuiRender::RenderBorder(Rect rc)
{
	if(!m_pkSkin)
		return false;

	if(m_bClipperEnabled)
	{
		if(m_rcClipperArea.Left > rc.Left || m_rcClipperArea.Right < rc.Right || 
			m_rcClipperArea.Top > rc.Top || m_rcClipperArea.Bottom < rc.Bottom)
			return true;
	}

	unsigned short sz = m_pkSkin->m_unBorderSize;

	if(sz == 0)
		return false;
	
	float wx = (float)(rc.Right-rc.Left) / sz;
	float wy = (float)(rc.Bottom-rc.Top) / sz;

	int texture;
	bool bDrawAlpha;

	glColor3f(m_pkSkin->m_afBorderColor[0],m_pkSkin->m_afBorderColor[1],
		m_pkSkin->m_afBorderColor[2]);

	for(int m=0; m<6; m++)
	{
		switch(m)
		{
		case 0:
			texture = m_pkSkin->m_iHorzBorderTexAlphaID;
			break;
		case 1:
			texture = m_pkSkin->m_iHorzBorderTexID;
			break;			
		case 2:
			texture = m_pkSkin->m_iVertBorderTexAlphaID;
			break;
		case 3:
			texture = m_pkSkin->m_iVertBorderTexID;
			break;	
		case 4:
			texture = m_pkSkin->m_iBorderCornerTexAlphaID;
			break;
		case 5:
			texture = m_pkSkin->m_iBorderCornerTexID;
			break;		
		}
		
		if(texture >= 0)
		{
			glEnable(GL_TEXTURE_2D);
			
			if((m%2)==0)
			{
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);								
				glDisable(GL_DEPTH_TEST);								
				glBlendFunc(GL_DST_COLOR,GL_ZERO);
				bDrawAlpha = true;
			}
			else
			{
				if(bDrawAlpha)
					glBlendFunc(GL_ONE, GL_ONE); // Copy Image 1 Color To The Screen
			}

			m_pkTextureManger->BindTexture( texture );

			// Stäng av filtreringen
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}																			 
		else
		{
			if((m%2)==0)
			{
				bDrawAlpha = false;
				continue;
			}
			else
				glDisable(GL_TEXTURE_2D);
		}

		if(m>3)
		{
			glBegin(GL_QUADS);
				// Översta, vänstra hörnet
				glTexCoord2f(0,0);	glVertex2i(rc.Left-sz,m_iScreenHeight-rc.Top);		 
				glTexCoord2f(0,1);	glVertex2i(rc.Left-sz,m_iScreenHeight-rc.Top+sz);		
				glTexCoord2f(1,1);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top+sz);    
				glTexCoord2f(1,0);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);    
				// Översta, högra hörnet
				glTexCoord2f(1,0);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);		 
				glTexCoord2f(1,1);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top+sz);		
				glTexCoord2f(0,1);	glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Top+sz);    
				glTexCoord2f(0,0);	glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Top);    
				// Understa, vänstra hörnet
				glTexCoord2f(0,1);	glVertex2i(rc.Left-sz,m_iScreenHeight-rc.Bottom-sz);		 
				glTexCoord2f(0,0);	glVertex2i(rc.Left-sz,m_iScreenHeight-rc.Bottom);		
				glTexCoord2f(1,0);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);    
				glTexCoord2f(1,1);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom-sz);    
				// Understa, högra hörnet
				glTexCoord2f(1,1);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom-sz);		 
				glTexCoord2f(1,0);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);		
				glTexCoord2f(0,0);	glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Bottom);    
				glTexCoord2f(0,1);	glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Bottom-sz);    
			glEnd();
		}
		else
		if(m>1)
		{
			glBegin(GL_QUADS);	 
				// Översta raden
				glTexCoord2f(0,0);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		 
				glTexCoord2f(0,1);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top+sz);		
				glTexCoord2f(wx,1);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top+sz);    
				glTexCoord2f(wx,0);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);   
				// Understa raden;	 
				glTexCoord2f(0,1);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom-sz);		 
				glTexCoord2f(0,0);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		
				glTexCoord2f(wx,0);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);    
				glTexCoord2f(wx,1);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom-sz);   
			glEnd();
		}
		else
		{
			glBegin(GL_QUADS);
				// Vänstra kolonnen
				glTexCoord2f(0,0);	glVertex2i(rc.Left-sz,m_iScreenHeight-rc.Bottom);		 
				glTexCoord2f(0,wy);	glVertex2i(rc.Left-sz,m_iScreenHeight-rc.Top);		
				glTexCoord2f(1,wy);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);    
				glTexCoord2f(1,0);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom); 
				// Högra kolonnen
				glTexCoord2f(0,0);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);		 
				glTexCoord2f(0,wy);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);		
				glTexCoord2f(-1,wy);glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Top);    
				glTexCoord2f(-1,0);	glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Bottom);      
			glEnd();
		}
	}

	glDisable(GL_BLEND);								// Disable Blending

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: RenderText
//
/*void GLGuiRender::RenderText( char *strText, Rect rc, int iCursorPos, int iRenderDistFromTop,
							  bool bMultiLine, int& chars_printed, int& rows_printed)
{
	g_bLettersPrinted = 0;

	if(m_pkFont == NULL)
		return; 

	bool bDrawMasked = true;

	int fontTexture = m_pkTextureManger->Load(m_pkFont->m_szFileName.c_str(),0);

	m_iCursorPos = iCursorPos;

	m_rcTextBox = rc;

	Rect t = rc;

	m_rcTextBox = Rect(t.Left,m_iScreenHeight-t.Bottom,t.Right,m_iScreenHeight-t.Top);

	if(bDrawMasked)
	{
		glEnable(GL_BLEND);					// Enable Blending
		glDisable(GL_DEPTH_TEST);			// Disable Depth Testing
		glBlendFunc(GL_DST_COLOR,GL_ZERO);	// Blend Screen Color With Zero (Black)
		m_pkTextureManger->BindTexture( fontTexture );				
		glDisable(GL_TEXTURE_2D);

		glColor3f(1,1,1);
		
		if(bMultiLine)
			PrintRows(strText, rc, iCursorPos, iRenderDistFromTop, 
				chars_printed, rows_printed);
		else
			PrintRow(strText, rc, iCursorPos, iRenderDistFromTop, chars_printed);
	}

	int texture = fontTexture;
		 		
	if(texture >=0 )
	{
		m_pkTextureManger->BindTexture( texture );		
		glEnable(GL_TEXTURE_2D);

		if(bDrawMasked)
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);				
	}
	else
	{
		return;
	}

	glColor3f(1,1,1);

	if(bMultiLine)
		PrintRows(strText, rc, iCursorPos, iRenderDistFromTop, 
			chars_printed, rows_printed);
	else
		PrintRow(strText, rc, iCursorPos, iRenderDistFromTop, chars_printed);

	if(bDrawMasked)
		glDisable(GL_BLEND);
}*/
void GLGuiRender::RenderText( char *strText, Rect rc, int iCursorPos, int iRenderDistFromTop,
							  bool bMultiLine, int& chars_printed, int& rows_printed)
{
	g_bLettersPrinted = 0;

	if(m_pkFont == NULL)
		return; 

	bool bDrawMasked = true;

	int fontTexture = m_pkTextureManger->Load(m_pkFont->m_szFileName.c_str(),0);

	m_iCursorPos = iCursorPos;

	m_rcTextBox = rc;

	Rect t = rc;

	m_rcTextBox = Rect(t.Left,m_iScreenHeight-t.Bottom,t.Right,m_iScreenHeight-t.Top);

	glColor3f(1,1,1);

	if(bDrawMasked)
	{
		glEnable(GL_BLEND);					// Enable Blending
		glDisable(GL_DEPTH_TEST);			// Disable Depth Testing
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);	
		m_pkTextureManger->BindTexture( fontTexture );				
		glDisable(GL_TEXTURE_2D);
		
		if(bMultiLine)
			PrintRows(strText, rc, iCursorPos, iRenderDistFromTop, 
				chars_printed, rows_printed);
		else
			PrintRow(strText, rc, iCursorPos, iRenderDistFromTop, chars_printed);
	}

	int texture = fontTexture;
		 		
	if(texture >=0 )
	{
		m_pkTextureManger->BindTexture( texture );		
		glEnable(GL_TEXTURE_2D);

		if(bDrawMasked)
			glBlendFunc(GL_DST_COLOR, GL_ZERO);	// Blend Screen Color With Zero (Black)
			
	}
	else
	{
		return;
	}

	if(bMultiLine)
		PrintRows(strText, rc, iCursorPos, iRenderDistFromTop, 
			chars_printed, rows_printed);
	else
		PrintRow(strText, rc, iCursorPos, iRenderDistFromTop, chars_printed);

	if(bDrawMasked)
		glDisable(GL_BLEND);
}

bool GLGuiRender::PrintRows(char* text, Rect rc, int iCursorPos, int iRenderDistFromTop,
							int& chars_printed, int& rows_printed) 
{
	int characters_totalt = strlen(text);
	int width = rc.Width();
	int xpos=0, ypos=0, row_height = m_pkFont->m_cCharCellSize;
	int rows = 0, offset = 0, max_width = rc.Width();

	pair<int,int> kLength; // first = character, second = pixels.

	glBegin(GL_QUADS);	 

		while(offset < characters_totalt) // antal ord
		{
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

			int x,y;
			x = rc.Left + xpos;
			y = m_iScreenHeight-rc.Top-m_pkFont->m_cCharCellSize -
				ypos - iRenderDistFromTop;

			bool bInside = true;
			if(ypos+iRenderDistFromTop+m_pkFont->m_cCharCellSize>rc.Height())
				bInside = false;
			if(rc.Top + ypos + iRenderDistFromTop < rc.Top)
				bInside = false;

			if(bInside)
				PrintWord(x, y, text, offset, kLength.first);

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
		}

		// Print cursor outside the loop if last character.
		if(iCursorPos == characters_totalt && characters_totalt != 0)
		{
			int x,y;
			x = rc.Left + xpos - 2;
			y = m_iScreenHeight-rc.Top-m_pkFont->m_cCharCellSize -
				ypos - iRenderDistFromTop;
			PrintWord(x, y, "|", 0, 1);
		}
		
	glEnd();

	chars_printed = offset;
	rows_printed = rows;

	return true;
}

bool GLGuiRender::PrintRow(char *text, Rect rc, int iCursorPos, 
						   int iRenderDistFromLeft, int &chars_printed)
{
	int characters_totalt = strlen(text);
	int xpos = 0, offset = 0, max_width = rc.Width();
	int y = m_iScreenHeight-rc.Top-m_pkFont->m_cCharCellSize;

	// Center text vertically
	if(rc.Height() > m_pkFont->m_cCharCellSize)
	{
		y = m_iScreenHeight - rc.Top - (rc.Bottom-rc.Top)/2 - 
			m_pkFont->m_cCharCellSize/2;
	}

	pair<int,int> kLength; // first = character, second = pixels.

	// Must calc width if text should be centered.
	if(iRenderDistFromLeft == ZG_CENTER_TEXT_HORZ)
	{
		while(offset < characters_totalt) // antal ord
		{
			kLength = GetWordLength(text, offset, max_width);

			offset += kLength.first;
			xpos += kLength.second;

			if(xpos >= max_width)
				break;
		}

		int row_length = xpos;

		// reset values
		xpos = 0;
		offset = 0;

		xpos = rc.Width()/2 - row_length/2;
		iRenderDistFromLeft = 0;
	}

	glBegin(GL_QUADS);

		while(offset < characters_totalt) // antal ord
		{
			kLength = GetWordLength(text, offset, 999999);//max_width);

			int x = rc.Left + xpos - iRenderDistFromLeft;
			PrintWord(x, y, text, offset, kLength.first);

			offset += kLength.first;
			xpos += kLength.second;

		/*	if(xpos >= max_width)
				break;*/
		}

		// Print cursor outside the loop if last character.
		if(iCursorPos == characters_totalt)
		{
			int x = rc.Left + xpos - iRenderDistFromLeft;

			if(x < rc.Left + 2)
				x = rc.Left + 2;
			
			PrintWord(x, y, "|", 0, 1);
		}

	glEnd();

	chars_printed = offset;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Name: SetSkin
//
bool GLGuiRender::SetSkin(ZGuiSkin* pkSkin)
{
	m_pkSkin = pkSkin;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetFont
//
bool GLGuiRender::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
	return true;
}

void GLGuiRender::GetScreenSize(int& cx, int& cy)
{
	cx = m_iScreenWidth;
	cy = m_iScreenHeight;
}

pair<int,int> GLGuiRender::GetWordLength(char *text, int offset, int max_width)
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

void GLGuiRender::PrintWord(int x, int y, char *szWord, 
							int offset, int length)
{
	int i;
	for(i=offset; i<offset+length; i++)
	{
		// Print cursor
		if(i == m_iCursorPos && !(m_iCursorPos == 0 && strlen(szWord) <= 1))
		{
			int iCursorX = x;
			int iCursorY = y;

			int index = '|'-32;
			int fx = m_pkFont->m_aChars[index].iPosX;
			int fy = m_pkFont->m_aChars[index].iPosY;
			int fw = m_pkFont->m_aChars[index].iSizeX;
			int fh = m_pkFont->m_aChars[index].iSizeY;

			float tx = (float) fx / m_pkFont->m_iBMPWidth;
			float ty = (float) fy / m_pkFont->m_iBMPWidth;
			float tw = (float) fw / m_pkFont->m_iBMPWidth;
			float th = (float) fh / m_pkFont->m_iBMPWidth;

			iCursorX -= 2;	// minska markörens xpos ytterligare 2 pixlar.
							// som en kompensation för tecknets egen storlek.

			glTexCoord2f(tx,ty);		glVertex2i(iCursorX,iCursorY+fh);		 
			glTexCoord2f(tx+tw,ty);		glVertex2i(iCursorX+fw,iCursorY+fh);    
			glTexCoord2f(tx+tw,ty+th);	glVertex2i(iCursorX+fw,iCursorY);    
			glTexCoord2f(tx,ty+th);		glVertex2i(iCursorX,iCursorY);
		}

		int iCurrLegth = 0;

		int pos = szWord[i]-32;
		if(pos < 0 || pos > 255)
		{
			continue;
		}

		if(szWord[i] == '\n')
			return;

		int fx = m_pkFont->m_aChars[pos].iPosX;
		int fy = m_pkFont->m_aChars[pos].iPosY;
		int fw = m_pkFont->m_aChars[pos].iSizeX;
		int fh = m_pkFont->m_aChars[pos].iSizeY;

		iCurrLegth = fw;

		if(m_rcTextBox.Inside(x, y))
		{
			if( !m_bClipperEnabled || (m_bClipperEnabled && 
				 x > m_rcClipperArea.Left && x+fw < m_rcClipperArea.Right &&
				 y < m_iScreenHeight-m_rcClipperArea.Top && 
				 y+fh > m_iScreenHeight-m_rcClipperArea.Bottom))
			{
				float tx = (float) fx / m_pkFont->m_iBMPWidth;
				float ty = (float) fy / m_pkFont->m_iBMPWidth;
				float tw = (float) fw / m_pkFont->m_iBMPWidth;
				float th = (float) fh / m_pkFont->m_iBMPWidth;

				glTexCoord2f(tx,ty);		glVertex2i(x,y+fh);		 
				glTexCoord2f(tx+tw,ty);		glVertex2i(x+fw,y+fh);    
				glTexCoord2f(tx+tw,ty+th);	glVertex2i(x+fw,y);    
				glTexCoord2f(tx,ty+th);		glVertex2i(x,y);
				
				g_bLettersPrinted++;
			}
		}

		x+=iCurrLegth;
	}
}

bool GLGuiRender::RenderLines(vector<Point>& akLines, 
							  unsigned char r,
							  unsigned char g,
							  unsigned char b,
							  float width)
{
	glColor3ub(r,g,b);
	glDisable(GL_TEXTURE_2D);

	float fPrevLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &fPrevLineWidth);

	glLineWidth(width);

	glBegin(GL_LINES);

	int iSize = akLines.size();
	for(int i=0; i<iSize; i+=2)
	{
		glVertex2i(akLines[i].x,m_iScreenHeight-akLines[i].y);
		glVertex2i(akLines[i+1].x,m_iScreenHeight-akLines[i+1].y);		
	}

	glEnd();

	glLineWidth(fPrevLineWidth);

	return true;
}

bool GLGuiRender::RenderPoints(vector<tRGBPoint>& akLines)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POINTS);

	int iSize = akLines.size();
	for(int i=0; i<iSize; i++)
	{
		glColor3ub(akLines[i].second.r,akLines[i].second.g,akLines[i].second.b);
		glVertex2i(akLines[i].first.x,m_iScreenHeight-akLines[i].first.y);		
	}

	glEnd();

	return true;
}

bool GLGuiRender::RenderRects(vector<tRGBRect>& akRects)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);

	int iSize = akRects.size();
	for(int i=0; i<iSize; i++)
	{
		Rect rc = akRects[i].first;
		glColor3ub(akRects[i].second.r,akRects[i].second.g,akRects[i].second.b);
		glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
		glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
		glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
		glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);   		
	}

	glEnd();

	return true;
}

void GLGuiRender::SetClipperArea(Rect rc)
{
	m_rcClipperArea = rc;
}

void GLGuiRender::EnableClipper(bool bEnable)
{
	m_bClipperEnabled = bEnable;
}


