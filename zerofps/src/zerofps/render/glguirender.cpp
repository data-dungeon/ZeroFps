// GLRender.cpp: implementation of the GLRender class.
//
///////////////////////////////////////////////////////////////////////////////
#include "../ogl/zfpsgl.h"
#include "glguirender.h"
#include "../basic/zguiskin.h"
#include "texturemanager.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////

GLGuiRender::GLGuiRender()
{
	m_pkFont = NULL;
	m_pkSkin = NULL;
	m_iScreenWidth = 800;
	m_iScreenHeight = 600;
	m_pkTextureManger=static_cast<TextureManager*>
		(g_ZFObjSys.GetObjectPtr("TextureManager"));	
}

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
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
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

	// Don't render if skin is transparent.
	if(m_pkSkin->m_bTransparent)
		return true;

	bool bMask = m_pkSkin->m_iBkTexAlphaID != -1;

	glLoadIdentity();

	bool bDrawMasked = (bMask == true && m_pkSkin->m_iBkTexAlphaID > 0) ? 
		true : false;

	float wx = 1.0f, wy = 1.0f;
	int texture = m_pkSkin->m_iBkTexID;

	if(m_pkSkin->m_bTileBkSkin == true && texture > 0)
	{
		glEnable(GL_TEXTURE_2D);
		m_pkTextureManger->BindTexture( texture );

		int iWidth=32, iHeight=32;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iHeight);

		wx = (float)(rc.Right-rc.Left) / iWidth;
		wy = (float)(rc.Bottom-rc.Top) / iHeight;
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
			glTexCoord2f(0,0);	  glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
			glTexCoord2f(0,-wy);  glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
			glTexCoord2f(wx,-wy); glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
			glTexCoord2f(wx,0);	  glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);    
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

		if(bDrawMasked)
			glColor3f(1,1,1);
		else
			glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],
				m_pkSkin->m_afBkColor[2]);

		glTexCoord2f(0,0);		glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
		glTexCoord2f(0,-wy);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
		glTexCoord2f(wx,-wy);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
		glTexCoord2f(wx,0);		glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);    
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
bool GLGuiRender::RenderText( char *strText, Rect rc, int iCursorPos, 
							  bool bCenterTextVertically)
{
	if(m_pkFont == NULL)
		return false;

	bool bDrawMasked = true;

	if(bCenterTextVertically)
	{
		int h = rc.Height();
		rc = rc.Move(0, h/4-m_pkFont->m_cCharCellSize/4);
	}

	int fontTexture = m_pkTextureManger->Load(m_pkFont->m_szFileName.c_str(),0);

	if(!fontTexture)
		return false;

	if(bDrawMasked)
	{
		glEnable(GL_BLEND);					// Enable Blending
		glDisable(GL_DEPTH_TEST);			// Disable Depth Testing
		glBlendFunc(GL_DST_COLOR,GL_ZERO);	// Blend Screen Color With Zero (Black)

		m_pkTextureManger->BindTexture( fontTexture );
		glDisable(GL_TEXTURE_2D);

		glColor3f(1,1,1);							
		PrintRows(strText, rc, iCursorPos);
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
		return true;
	}

	glColor3f(1,1,1);
	bool bFit = PrintRows(strText, rc, iCursorPos);

	if(bDrawMasked)
		glDisable(GL_BLEND);

	return bFit;
}

///////////////////////////////////////////////////////////////////////////////
// Name: PrintRows
//
bool GLGuiRender::PrintRows(char* text, Rect rc, int iCursorPos) 
{
	bool bFit = true;

	unsigned int x = rc.Left;
	unsigned int y = m_iScreenHeight-rc.Top-m_pkFont->m_cCharCellSize;

	int row=0;
	unsigned int num_chars = strlen(text);
	int max_rows = rc.Height() / m_pkFont->m_cCharCellSize;

	int iCursorX = x;
	int iCursorY = y;

	vector<int> row_start_pos;

	glBegin(GL_QUADS);	 

		for(unsigned int i=0; i<num_chars; i++)
		{
			int pos = text[i]-32;
			if(pos < 0 || pos > 255)
				continue;

			int fx = m_pkFont->m_aChars[pos].iPosX;
			int fy = m_pkFont->m_aChars[pos].iPosY;
			int fw = m_pkFont->m_aChars[pos].iSizeX;
			int fh = m_pkFont->m_aChars[pos].iSizeY;

			int iCurrLegth = fw;

			if(text[i] != ' ') // lägg inte till extra utrymmer efter ett space
				iCurrLegth = (fw + m_pkFont->m_cPixelGapBetweenChars);

			if(x + iCurrLegth > rc.Right)
			{
				x = rc.Left;
				y -= m_pkFont->m_cCharCellSize;
				row++;

				if(i < iCursorPos)
				{
					iCursorX = x;
					iCursorY = y;
				}

				row_start_pos.push_back(i);
			}
			
			if(i < iCursorPos)
				iCursorX += iCurrLegth;

			if(row >= max_rows)
			{
				bFit = false;
				break;
			}

			float tx = (float) fx / m_pkFont->m_iBMPWidth;
			float ty = (float) fy / m_pkFont->m_iBMPWidth;
			float tw = (float) fw / m_pkFont->m_iBMPWidth;
			float th = (float) fh / m_pkFont->m_iBMPWidth;

			glTexCoord2f(tx,ty);		glVertex2i(x,y+fh);		 
			glTexCoord2f(tx+tw,ty);		glVertex2i(x+fw,y+fh);    
			glTexCoord2f(tx+tw,ty+th);	glVertex2i(x+fw,y);    
			glTexCoord2f(tx,ty+th);		glVertex2i(x,y);

			x += iCurrLegth;
		}

		// Print cursor
		if(iCursorPos != -1 && bFit)
		{
			int index = '|'-32;
			int fx = m_pkFont->m_aChars[index].iPosX;
			int fy = m_pkFont->m_aChars[index].iPosY;
			int fw = m_pkFont->m_aChars[index].iSizeX;
			int fh = m_pkFont->m_aChars[index].iSizeY;

			bool bEndOfRow = false;

			for(int i=0; i<row_start_pos.size(); i++)
				if(row_start_pos[i] == iCursorPos)
					bEndOfRow = true;

			// Flytta ner markören till början på nästa rad 
			// om den befinner sig sist på en rad.
			if(bEndOfRow)
			{
				iCursorX = rc.Left;
				iCursorY -= m_pkFont->m_cCharCellSize;
			}

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
		
	glEnd();

	return bFit;
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






