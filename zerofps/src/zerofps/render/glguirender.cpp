// GLRender.cpp: implementation of the GLRender class.
//
//////////////////////////////////////////////////////////////////////
#include "../ogl/zfpsgl.h"
#include "glguirender.h"
#include "../basic/zguiskin.h"
#include "texturemanager.h"

#define FONTWIDTH 32
#define TEXT_MAX_LENGHT 256

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLGuiRender::GLGuiRender(/*int w, int h, TextureManager* pkTextureManger*/)
{
	m_pkFont = NULL;
	m_pkSkin = NULL;
	m_iScreenWidth = 0;
	m_iScreenHeight = 0;
	m_iMaskTexture = -1;
	//m_pkTextureManger = pkTextureManger;
	m_pkTextureManger=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
//	m_iFontDisplaylistID = 0;
//	BuildFont();

}

GLGuiRender::~GLGuiRender()
{

}

bool GLGuiRender::StartRender()
{
	//glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_FOG_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);

/*	glViewport(0,0,m_iScreenWidth,m_iScreenHeight);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
*/
	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, m_iScreenWidth + 1, -1, m_iScreenHeight);
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();


/*	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Test To Do
		
		glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
		glEnable(GL_TEXTURE_2D);							// Enable 2D Texture Mapping*/
		
	return true;
}

bool GLGuiRender::SetDisplay(int w, int h)
{
	m_iScreenWidth = w, 
	m_iScreenHeight = h;
	return true;
}

bool GLGuiRender::EndRender()
{
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	return true;
}

bool GLGuiRender::RenderQuad(Rect kScreenRect, bool bMask)
{
	glLoadIdentity();

	//glDisable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);

	bool bDrawMasked = (bMask == true && m_iMaskTexture > 0) ? true : false;

	if(m_pkSkin == NULL)
		return false;

/*	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       // Linear Filtered
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       // Linear Filtered*/

//	glLoadIdentity();

	float wx = 1.0f, wy = 1.0f;
	int texture = m_pkSkin->m_iBkTexID;

	if(m_pkSkin->m_bTileBkSkin == true && texture > 0)
	{
		glEnable(GL_TEXTURE_2D);
		m_pkTextureManger->BindTexture( texture );
		//glBindTexture(GL_TEXTURE_2D, texture);

		int iWidth=32, iHeight=32;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iHeight);

		wx = (float)(kScreenRect.Right-kScreenRect.Left) / iWidth;
		wy = (float)(kScreenRect.Bottom-kScreenRect.Top) / iHeight;
	}

	if(bDrawMasked)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);								
		glDisable(GL_DEPTH_TEST);								
		glBlendFunc(GL_DST_COLOR,GL_ZERO);		

		//glBindTexture(GL_TEXTURE_2D, m_iMaskTexture);
		m_pkTextureManger->BindTexture( m_iMaskTexture );
		glBegin(GL_QUADS);		
			//glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],m_pkSkin->m_afBkColor[2]);
			glColor3f(1,1,1);
			glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom);		 
			glTexCoord2f(0,wy);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);		
			glTexCoord2f(wx,wy);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);    
			glTexCoord2f(wx,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);    
		glEnd();							
	}
	 		
	if(texture > 0)
	{
		glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, texture);
		m_pkTextureManger->BindTexture( texture );

		if(bDrawMasked)
			glBlendFunc(GL_ONE, GL_ONE);					// Copy Image 1 Color To The Screen
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	glBegin(GL_QUADS);	 

		if(bDrawMasked)
			glColor3f(1,1,1);
		else
			glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],m_pkSkin->m_afBkColor[2]);

		glTexCoord2f(0,0);		glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom);		 
		glTexCoord2f(0,wy);		glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);		
		glTexCoord2f(wx,wy);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);    
		glTexCoord2f(wx,0);		glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);    
	glEnd();

	if(bDrawMasked)
	{
		//glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
		glDisable(GL_BLEND);								// Disable Blending
	}

	return true;
}

bool GLGuiRender::RenderBorder(Rect kScreenRect, bool bMask)
{
	bool bDrawMasked = (bMask == true && m_iMaskTexture > 0) ? true : false;

	if(m_pkSkin == NULL)
		return false;

	unsigned short sz = m_pkSkin->m_unBorderSize;

	if(sz == 0)
		return false;

/*	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       // Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       // Linear Filtered*/

	int texture = m_pkSkin->m_iHorzBorderTexID;
		 		
	if(texture > 0)
	{
		//glBindTexture(GL_TEXTURE_2D, texture);
		m_pkTextureManger->BindTexture( texture );
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	float wx = (float)(kScreenRect.Right-kScreenRect.Left) / sz;
	float wy = (float)(kScreenRect.Bottom-kScreenRect.Top) / sz;

	glBegin(GL_QUADS);	 
		glColor3f(m_pkSkin->m_afBorderColor[0],m_pkSkin->m_afBorderColor[1],m_pkSkin->m_afBorderColor[2]);
		// Översta raden
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);		 
		glTexCoord2f(0,1);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top+sz);		
		glTexCoord2f(wx,1);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top+sz);    
		glTexCoord2f(wx,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);   
		// Understa raden;	 
		glTexCoord2f(0,1);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom-sz);		 
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom);		
		glTexCoord2f(wx,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);    
		glTexCoord2f(wx,1);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom-sz);   
	glEnd();

	texture = m_pkSkin->m_iVertBorderTexID;
		 		
	if(texture > 0)
	{
		//glBindTexture(GL_TEXTURE_2D, texture);
		m_pkTextureManger->BindTexture( texture );
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	glBegin(GL_QUADS);
		// Vänstra kolonnen
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Bottom);		 
		glTexCoord2f(0,wy);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Top);		
		glTexCoord2f(1,wy);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);    
		glTexCoord2f(1,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom); 
		// Högra kolonnen
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);		 
		glTexCoord2f(0,wy);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);		
		glTexCoord2f(-1,wy);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Top);    
		glTexCoord2f(-1,0);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Bottom);      
	glEnd();

	texture = m_pkSkin->m_iBorderCornerTexID;
		 		
	if(texture > 0)
	{
		// Slå på alpha
/*		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glAlphaFunc(GL_GREATER,0.1f);
		glEnable(GL_ALPHA_TEST);*/

		//glBindTexture(GL_TEXTURE_2D, texture);
		m_pkTextureManger->BindTexture( texture );
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	if(bDrawMasked)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);								
		glDisable(GL_DEPTH_TEST);								
		glBlendFunc(GL_DST_COLOR,GL_ZERO);	

		m_pkTextureManger->BindTexture( m_iMaskTexture );

		glBegin(GL_QUADS);
			// Översta, vänstra hörnet
			glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Top);		 
			glTexCoord2f(0,1);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Top+sz);		
			glTexCoord2f(1,1);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top+sz);    
			glTexCoord2f(1,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);    
			// Översta, högra hörnet
			glTexCoord2f(1,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);		 
			glTexCoord2f(1,1);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top+sz);		
			glTexCoord2f(0,1);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Top+sz);    
			glTexCoord2f(0,0);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Top);    
			// Understa, vänstra hörnet
			glTexCoord2f(0,1);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Bottom-sz);		 
			glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Bottom);		
			glTexCoord2f(1,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom);    
			glTexCoord2f(1,1);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom-sz);    
			// Understa, högra hörnet
			glTexCoord2f(1,1);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom-sz);		 
			glTexCoord2f(1,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);		
			glTexCoord2f(0,0);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Bottom);    
			glTexCoord2f(0,1);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Bottom-sz);    
		glEnd();

	}

	if(bDrawMasked)
		glBlendFunc(GL_ONE, GL_ONE);					// Copy Image 1 Color To The Screen

	m_pkTextureManger->BindTexture( m_pkSkin->m_iBorderCornerTexID );

	glBegin(GL_QUADS);
		// Översta, vänstra hörnet
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Top);		 
		glTexCoord2f(0,1);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Top+sz);		
		glTexCoord2f(1,1);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top+sz);    
		glTexCoord2f(1,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);    
		// Översta, högra hörnet
		glTexCoord2f(1,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);		 
		glTexCoord2f(1,1);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top+sz);		
		glTexCoord2f(0,1);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Top+sz);    
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Top);    
		// Understa, vänstra hörnet
		glTexCoord2f(0,1);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Bottom-sz);		 
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Left-sz,m_iScreenHeight-kScreenRect.Bottom);		
		glTexCoord2f(1,0);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom);    
		glTexCoord2f(1,1);	glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom-sz);    
		// Understa, högra hörnet
		glTexCoord2f(1,1);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom-sz);		 
		glTexCoord2f(1,0);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);		
		glTexCoord2f(0,0);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Bottom);    
		glTexCoord2f(0,1);	glVertex2i(kScreenRect.Right+sz,m_iScreenHeight-kScreenRect.Bottom-sz);    
	glEnd();


	if(bDrawMasked)
	{
		//glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
		glDisable(GL_BLEND);								// Disable Blending
	}
/*	glPopAttrib();*/


	return true;
}

bool GLGuiRender::SetSkin(ZGuiSkin* pkSkin)
{
	m_pkSkin = pkSkin;
	return true;
}

bool GLGuiRender::RenderText( char *strText, Rect kScreenRect, int iCursorPos, bool bCenterTextVertically)
{
	bool bDrawMasked = true;

	if(m_pkFont == NULL)
		return false;

	if(bCenterTextVertically)
	{
		int h = kScreenRect.Height();
		kScreenRect = kScreenRect.Move(0, h/4-m_pkFont->m_cCharCellSize/4);
	}

	int fontTexture = m_pkTextureManger->Load(m_pkFont->m_szFileName.c_str(), 0);

	if(fontTexture)
		m_iMaskTexture = fontTexture;
	else
		return false;

	if(bDrawMasked)
	{
		glEnable(GL_BLEND);								// Enable Blending
		glDisable(GL_DEPTH_TEST);						// Disable Depth Testing
		glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)

		m_pkTextureManger->BindTexture( m_iMaskTexture );
		glDisable(GL_TEXTURE_2D);

		glColor3f(1,1,1);							
		PrintRows(strText, kScreenRect, iCursorPos);
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
	bool bFit = PrintRows(strText, kScreenRect, iCursorPos);

	if(bDrawMasked)
	{
		glDisable(GL_BLEND);								
	}

	return bFit;
}

bool GLGuiRender::PrintRows(char* text, Rect rc, int iCursorPos) 
{
	bool bFit = true;

	unsigned int x = rc.Left;
	unsigned int y = m_iScreenHeight-rc.Top-m_pkFont->m_cCharCellSize;

	int row=0;
	unsigned int num_chars = strlen(text);
	int max_rows = rc.Height() / m_pkFont->m_cCharCellSize;

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

			float tx = (float) fx / m_pkFont->m_iBMPWidth;
			float ty = (float) fy / m_pkFont->m_iBMPWidth;
			float tw = (float) fw / m_pkFont->m_iBMPWidth;
			float th = (float) fh / m_pkFont->m_iBMPWidth;

			glTexCoord2f(tx,ty);		glVertex2i(x,y+fh);		 
			glTexCoord2f(tx+tw,ty);		glVertex2i(x+fw,y+fh);    
			glTexCoord2f(tx+tw,ty+th);	glVertex2i(x+fw,y);    
			glTexCoord2f(tx,ty+th);		glVertex2i(x,y);

			if(text[i] != ' ')
				x += (fw + m_pkFont->m_cPixelGapBetweenChars);
			else
				x += fw;

			if(x > rc.Right-m_pkFont->m_cCharCellSize)
			{
				x = rc.Left;
				y -= m_pkFont->m_cCharCellSize;
				row++;
			}

			if(row >= max_rows)
			{
				bFit = false;
				break;
			}
		}
		
	glEnd();

	return bFit;
}


bool GLGuiRender::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
	return true;
}
