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

GLGuiRender::GLGuiRender(int w, int h, TextureManager* pkTextureManger, int iFontID)
{
	m_pkSkin = NULL;
	m_iScreenWidth = w;
	m_iScreenHeight = h;
	m_iMaskTexture = -1;
	m_pkTextureManger = pkTextureManger;
	m_iFontDisplaylistID = 0;
	BuildFont(iFontID);
}

GLGuiRender::~GLGuiRender()
{

}

bool GLGuiRender::StartRender()
{
	

	//glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

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
			glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],m_pkSkin->m_afBkColor[2]);
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

/*	glDisable(GL_ALPHA_TEST);*/
/*	glPopAttrib();*/


	return true;
}

bool GLGuiRender::SetSkin(ZGuiSkin* pkSkin)
{
	m_pkSkin = pkSkin;
	return true;
}

GLvoid GLGuiRender::glPrint(GLint x, GLint y, char *string, int set, int bind_texture_id)	// Where The Printing Happens
{
	if (set>1)
	{
		set=1;
	}
	//glBindTexture(GL_TEXTURE_2D, bind_texture_id);	// Select Our Font Texture
	glLoadIdentity();									// Reset The Modelview Matrix
	glTranslated(x,y,0);								// Position The Text (0,0 - Bottom Left)
	glListBase(m_iFontDisplaylistID-32+(128*set));		// Choose The Font Set (0 or 1)
	glCallLists(strlen(string),GL_BYTE,string);			// Write The Text To The Screen
}

bool GLGuiRender::RenderText( char *strText, Rect kScreenRect, int iFontSize, int iCursorPos, bool bMask)
{
	bool bDrawMasked = (bMask == true && m_iMaskTexture > 0) ? true : false;

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       // Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       // Linear Filtered

	glPushMatrix();

	glLoadIdentity();

	int iTextWidth = strlen(strText) * iFontSize;
	int iYPos = m_iScreenHeight-kScreenRect.Top - kScreenRect.Height()/2 - iFontSize/2; 
	int iXPos = kScreenRect.Left/* + kScreenRect.Width()/2 - iTextWidth/2*/; 

	if(bDrawMasked)
	{
		glEnable(GL_BLEND);								// Enable Blending
		glDisable(GL_DEPTH_TEST);						// Disable Depth Testing
		glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)

		m_pkTextureManger->BindTexture( m_iMaskTexture );
		glDisable(GL_TEXTURE_2D);

		glColor3f(1,1,1);
		glPrint(iXPos, iYPos, strText, 1, m_iMaskTexture);							
	}

	int texture = m_pkSkin->m_iBkTexID;
		 		
	if(texture > 0)
	{
		m_pkTextureManger->BindTexture( texture );
		glEnable(GL_TEXTURE_2D);

		if(bDrawMasked)
			//glBlendFunc(GL_ONE, GL_ONE);					// Copy Image 1 Color To The Screen
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);					// Copy Image 1 Color To The Screen
	}
	else
	{
		return true;
	}

	glColor3f(1,1,1);
	glPrint(iXPos, iYPos, strText, 0, m_pkSkin->m_iBkTexID);

	if(bDrawMasked)
	{
		glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
		glDisable(GL_BLEND);								// Disable Blending
	}

	glPopMatrix();

	return true;
}

void GLGuiRender::PrintChar(char cChar, int xoffset, int yoffset, int iFontSize) 
{
	int texwidth=iFontSize*16;	
	int pos=int(cChar)*iFontSize;		
	float glu=1.0f/texwidth;				//opengl texture cordinats is 0-1
	float width=glu*iFontSize;
	
	float y=1.0f-(float(int(pos/texwidth)*iFontSize)*glu+width);
	float x=float(pos%texwidth)*glu;//+width/2;
	
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glAlphaFunc(GL_GREATER,0.1f);
	glEnable(GL_ALPHA_TEST);                                                // Enable Texture Mapping

 	int texture = m_pkSkin->m_iBkTexID; 

	if(texture > 0)
	{
		m_pkTextureManger->BindTexture(texture);
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		return;
	}

	Rect kScreenRect(xoffset,yoffset,xoffset+iFontSize,yoffset+iFontSize);
		
		glBegin(GL_QUADS);	 
			glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],m_pkSkin->m_afBkColor[2]);
			glTexCoord2f(x,y);				glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Bottom);		 
			glTexCoord2f(x+width,y);		glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Bottom);    
			glTexCoord2f(x+width,y-width);	glVertex2i(kScreenRect.Right,m_iScreenHeight-kScreenRect.Top);    
			glTexCoord2f(x,y-width);		glVertex2i(kScreenRect.Left,m_iScreenHeight-kScreenRect.Top);		
		glEnd();			
	
	glDisable(GL_ALPHA_TEST);
}


void GLGuiRender::BuildFont(int iFontID)
{
	float	cx;											// Holds Our X Character Coord
	float	cy;											// Holds Our Y Character Coord

	m_iFontDisplaylistID=glGenLists(256);				// Creating 256 Display Lists
	m_pkTextureManger->BindTexture(iFontID);		// Select Our Font Texture
	for (GLuint loop=0; loop<256; loop++)						// Loop Through All 256 Lists
	{
		cx=float(loop%16)/16.0f;						// X Position Of Current Character
		cy=float(loop/16)/16.0f;						// Y Position Of Current Character

		glNewList(m_iFontDisplaylistID+loop,GL_COMPILE);				// Start Building A List
		//glColor3f(1.0f,1.0f,1.0f);
			glBegin(GL_QUADS);							// Use A Quad For Each Character
				glTexCoord2f(cx,1-cy-0.0625f);			// Texture Coord (Bottom Left)
				glVertex2i(0,0);						// Vertex Coord (Bottom Left)
				glTexCoord2f(cx+0.0625f,1-cy-0.0625f);	// Texture Coord (Bottom Right)
				glVertex2i(16,0);						// Vertex Coord (Bottom Right)
				glTexCoord2f(cx+0.0625f,1-cy);			// Texture Coord (Top Right)
				glVertex2i(16,16);						// Vertex Coord (Top Right)
				glTexCoord2f(cx,1-cy);					// Texture Coord (Top Left)
				glVertex2i(0,16);						// Vertex Coord (Top Left)
			glEnd();									// Done Building Our Quad (Character)
			glTranslated(12,0,0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}													// Loop Until All 256 Are Built
}
