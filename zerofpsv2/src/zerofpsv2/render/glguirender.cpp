// GLRender.cpp: implementation of the GLRender class.
//
///////////////////////////////////////////////////////////////////////////////
#include "../ogl/zfpsgl.h"
#include "glguirender.h" 
#include "../basic/zguiskin.h"
#include "../basic/vector4.h"
#include "texturemanager.h"
#include "../basic/zfresourcedb.h"
#include "../basic/zifanimation.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
  
GLGuiRender::GLGuiRender()
{
	m_bDrawList = false;
	m_pkFont = NULL;
	m_pkSkin = NULL;
	m_iScreenWidth = 800;
	m_iScreenHeight = 600;
	m_bClipperEnabled = false;
	m_afTextColor[0] = m_afTextColor[1] = m_afTextColor[2] = 0.0f;
	m_bSearchForSytax = false;
	m_eGUIScaleMode = GUIScaleProjMatBeforeRendering;
}

bool GLGuiRender::StartUp()	
{ 
	m_pkTextureManger=static_cast<TextureManager*>
		(g_ZFObjSys.GetObjectPtr("TextureManager"));	
		
	m_pkShader	= static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
/*		
	m_pkTextureManger=static_cast<TextureManager*>
		(g_ZFObjSys.GetObjectPtr("TextureManager"));			
*/		
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

void GLGuiRender::SetScaleMode(GUIScaleMode eGUIScaleMode)
{
	m_eGUIScaleMode = eGUIScaleMode;
}

///////////////////////////////////////////////////////////////////////////////
// Name: StartRender
//
bool GLGuiRender::StartRender(bool bClear)
{
	m_pkShader->Push("gui");

	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_FOG_BIT | 
		GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT); 
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);



	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//gluOrtho2D(0, m_iScreenWidth + 1, -1, m_iScreenHeight);
	gluOrtho2D(0, m_iScreenWidth , 0, m_iScreenHeight);

	glScissor  ( 0, 0,	m_iScreenWidth, m_iScreenHeight ); // lade till 22 okt 2004 för att kunna rendera camera i fönster	
	glViewport ( 0, 0,	m_iScreenWidth, m_iScreenHeight  );	// lade till 22 okt 2004 för att kunna rendera camera i fönster	

	if(bClear)
		glClear(GL_COLOR_BUFFER_BIT);	

		glDisable(GL_COLOR_MATERIAL);	 // lade till 19 okt 2004


	//
	// Skala om projektions matrisen.
	//
	if(m_eGUIScaleMode == GUIScaleProjMatBeforeRendering)
	{
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
	}

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Fuckar upp grafiken!
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Fuckar upp grafiken!
		
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
	
	m_pkShader->Pop();
	
	return true;
}

void GLGuiRender::UpdateAnimation()
{	
	bool bRebuildTexture;
	if(m_pkSkin->m_pkZIFAnimation->Update(bRebuildTexture))
	{
		m_pkSkin->m_iBkTexID = m_pkTextureManger->CreateTextureFromRGB(
			m_pkSkin->m_pkZIFAnimation->GetTexIDName(), 
			(color_rgb*)m_pkSkin->m_pkZIFAnimation->GetFramePixels(), 
			m_pkSkin->m_pkZIFAnimation->m_iWidth, m_pkSkin->m_pkZIFAnimation->m_iHeight, 
			TextureManager::BGR, bRebuildTexture);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Name: RenderQuad
//
bool GLGuiRender::RenderQuad(Rect rc)
{
	if(!m_pkSkin)
		return false;

	if(m_pkSkin->m_pkZIFAnimation != NULL)
		UpdateAnimation();

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

	//glLoadIdentity();
	
	bool bDrawMasked = (bMask == true && m_pkSkin->m_iBkTexAlphaID > 0) ? 
		true : false;


	bool bIsTGA;// = m_pkTextureManger->TextureHaveAlpha(m_pkSkin->m_iBkTexID);
	bIsTGA = m_pkTextureManger->TextureIsTGA(m_pkSkin->m_iBkTexID);

	if(bIsTGA)
		bDrawMasked = false;

	int texture = m_pkSkin->m_iBkTexID;

	if(m_pkSkin->m_rcBkTile != Rect(0,0,0,0) && texture > 0)
	{
		glEnable(GL_TEXTURE_2D);
		m_pkTextureManger->BindTexture( texture );

		int iTextureWidth, iTextureHeight;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iTextureWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iTextureHeight);

		tw = (float) m_pkSkin->m_rcBkTile.Right / (float) iTextureWidth;
		tx = (float) m_pkSkin->m_rcBkTile.Left / (float) iTextureWidth;
		th = (float) m_pkSkin->m_rcBkTile.Top / (float) iTextureHeight;
		ty = ((float) m_pkSkin->m_rcBkTile.Bottom / (float) iTextureHeight) + th;
	}
	else
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

	float txs[] = {tx,tx,tx+tw,tx+tw};
	float tys[] = {ty,th,th,ty};

	if(m_pkSkin->m_ucRots90Degree != 0) 
		RotateVertexCoords90deg(txs, tys, m_pkSkin->m_ucRots90Degree);

	if(bDrawMasked)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);								
		//glDisable(GL_DEPTH_TEST);								
		glBlendFunc(GL_DST_COLOR,GL_ZERO);		

		m_pkTextureManger->BindTexture( m_pkSkin->m_iBkTexAlphaID );

		glBegin(GL_QUADS);		
			glColor3f(1,1,1);

//			glTexCoord2f(tx,ty);				glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
//			glTexCoord2f(tx,th);				glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
//			glTexCoord2f(tx+tw,th);			glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
//			glTexCoord2f(tx+tw,ty);			glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);  
			
			glTexCoord2f(txs[0],tys[0]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
			glTexCoord2f(txs[1],tys[1]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
			glTexCoord2f(txs[2],tys[2]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);     
			glTexCoord2f(txs[3],tys[3]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);   
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

	if(bIsTGA)
	{
		glColor4f(1,1,1,1);		
		//glDisable(GL_LIGHTING);
		glAlphaFunc(GL_GREATER,0.1);
		glEnable(GL_ALPHA_TEST);
	}

	glBegin(GL_QUADS);	 

		//if(bDrawMasked)
		//	glColor3f(1,1,1);
		//else
			glColor3f(m_pkSkin->m_afBkColor[0],m_pkSkin->m_afBkColor[1],
				m_pkSkin->m_afBkColor[2]);

		if(bIsTGA)
		{
			glTexCoord2f(txs[0],tys[2]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
			glTexCoord2f(txs[1],tys[3]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
			glTexCoord2f(txs[2],tys[0]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
			glTexCoord2f(txs[3],tys[1]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);  
		}
		else
		{
			if(m_pkSkin->m_pkZIFAnimation == NULL)
			{
				glTexCoord2f(txs[0],tys[0]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
				glTexCoord2f(txs[1],tys[1]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
				glTexCoord2f(txs[2],tys[2]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
				glTexCoord2f(txs[3],tys[3]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);  
			}
			else
			{
				glTexCoord2f(txs[0],tys[2]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Bottom);		 
				glTexCoord2f(txs[1],tys[3]);	glVertex2i(rc.Left,m_iScreenHeight-rc.Top);		
				glTexCoord2f(txs[2],tys[0]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Top);    
				glTexCoord2f(txs[3],tys[1]);	glVertex2i(rc.Right,m_iScreenHeight-rc.Bottom);  
			}

		}


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

	int texture = m_pkSkin->m_iHorzBorderTexAlphaID;
	bool bDrawAlpha = false;

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
				//glDisable(GL_DEPTH_TEST);								
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
				glTexCoord2f(-1,wy); glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Top);    
				glTexCoord2f(-1,0);	glVertex2i(rc.Right+sz,m_iScreenHeight-rc.Bottom);      
			glEnd();
		}
	}

	glDisable(GL_BLEND);								// Disable Blending

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

void GLGuiRender::DoTextTag()
{
	m_strSyntax.push_back('\0');

	// Bli av med alla skräptecken
	for(int bb=0; bb<m_strSyntax.length(); bb++)
		if(m_strSyntax[bb] == '\n' || m_strSyntax[bb] == '\t' || 
			m_strSyntax[bb] == ' '){
			m_strSyntax.erase(bb,1);
			bb--;
		}

	//printf(m_strSyntax.c_str());

	int ch;
	char dst[50];
	int length, pos;

	// Byt textfärg
	if((ch=m_strSyntax.find("col:", 0)) != string::npos)
	{
		if((ch = m_strSyntax.find(":", ch)) != string::npos)
		{
			length = m_strSyntax.find(",", ch)-ch-1;
			pos = ch+1;
			strncpy(dst, m_strSyntax.c_str()+pos, length);
			dst[length] = '\0';
			m_afTextColor[0] = (float) atoi(dst) / 255.0f;
			pos += length+1;

			length = m_strSyntax.find(",", pos)-pos;
			strncpy(dst, m_strSyntax.c_str()+pos, length);
			dst[length] = '\0';
			m_afTextColor[1] = (float) atoi(dst) / 255.0f;
			pos += length+1;

			length = m_strSyntax.length()-pos-1;
			strncpy(dst, m_strSyntax.c_str()+pos, length);
			dst[length] = '\0';
			m_afTextColor[2] = (float) atoi(dst) / 255.0f;
		}
	}

	m_strSyntax.clear(); 
	m_bSearchForSytax = false;
}

void GLGuiRender::RotateVertexCoords90deg(float *pfTUs, float *pfTVs, 
														unsigned char ucRotLaps)
{
	unsigned char A=0, B=1, C=2, D=3;

	for(unsigned char i=0; i<ucRotLaps; i++)
	{
		float pfTempTUs[4] = {pfTUs[0], pfTUs[1], pfTUs[2], pfTUs[3] };
		float pfTempTVs[4] = {pfTVs[0], pfTVs[1], pfTVs[2], pfTVs[3] };

		// 90 degree
		pfTUs[A] = pfTempTUs[D];
		pfTUs[B] = pfTempTUs[A];
		pfTUs[C] = pfTempTUs[B];
		pfTUs[D] = pfTempTUs[C];

		pfTVs[A] = pfTempTVs[D];
		pfTVs[B] = pfTempTVs[A];
		pfTVs[C] = pfTempTVs[B];
		pfTVs[D] = pfTempTVs[C];
	}
}


///////////////////////////////////////////////////////////////////////////////
// Font rendering
///////////////////////////////////////////////////////////////////////////////

//void GLGuiRender::RenderText( char *strText, Rect rc, int iCursorPos, 
//									   int iRenderDistFromTop, bool bMultiLine, 
//										int& chars_printed, int& rows_printed, 
//										float afTextcolor[3])
//{
//	if(m_pkFont == NULL || m_pkFont->m_iTextureID < 0)
//		return; 
//
//	m_iCursorPos = iCursorPos;
//	m_rcTextBox = Rect(rc.Left,m_iScreenHeight-rc.Bottom,rc.Right,
//		m_iScreenHeight-rc.Top);
//
//	m_afTextColor[0] = afTextcolor[0];
//	m_afTextColor[1] = afTextcolor[1];
//	m_afTextColor[2] = afTextcolor[2];
//		 		
//	m_pkTextureManger->BindTexture( m_pkFont->m_iTextureID );		
//	glEnable(GL_TEXTURE_2D);
//
//	glColor4f(0.0f,0.0f,0.0f,1);		
//	//glDisable(GL_LIGHTING);
//	glAlphaFunc(GL_GREATER,0.1f);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_BLEND);			
//	glEnable(GL_ALPHA_TEST);
//
//	if(bMultiLine)
//		PrintRows(strText, rc, iCursorPos, iRenderDistFromTop, 
//			chars_printed, rows_printed);
//	else
//		PrintRow(strText, rc, iCursorPos, iRenderDistFromTop, chars_printed);
//}

void GLGuiRender::RenderText( char *strText, Rect rc, int iCursorPos, 
							  float afTextcolor[3], int iRenderDistFromLeft)
{

	if(m_pkFont == NULL || m_pkFont->m_iTextureID < 0)
		return; 

	m_iCursorPos = iCursorPos;
	m_rcTextBox = Rect(rc.Left,m_iScreenHeight-rc.Bottom,rc.Right,
		m_iScreenHeight-rc.Top);

	m_afTextColor[0] = afTextcolor[0];
	m_afTextColor[1] = afTextcolor[1];
	m_afTextColor[2] = afTextcolor[2];
		 		
	m_pkTextureManger->BindTexture( m_pkFont->m_iTextureID );		
	glEnable(GL_TEXTURE_2D);

	glColor4f(0.0f,0.0f,0.0f,1);		
	//glDisable(GL_LIGHTING);
	glAlphaFunc(GL_GREATER,0.1f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);			
	glEnable(GL_ALPHA_TEST);

	int chars_printed;
	PrintRow(strText, rc, iCursorPos, iRenderDistFromLeft, chars_printed);

}

bool GLGuiRender::PrintRows(char* text, Rect rc, int iCursorPos, 
									 int iRenderDistFromTop, int& chars_printed, 
									 int& rows_printed) 
{
	int characters_totalt = strlen(text);
	int width = rc.Width();
	int xpos=0, ypos=0, row_height = m_pkFont->m_iRowHeight;
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
		/*		if(text[offset+kLength.first-1] == ' ')
				{
					int index = ' ' - 32;
					kLength.second -= m_pkFont->m_aChars[index].iSizeX;
				}*/

				//if(xpos+kLength.second >= width)
				//{
				//	//if(text[offset+kLength.first-1] == ' ')
				//	//{
				//	//	int index = ' ' - 32;
				//	//	kLength.second += m_pkFont->m_aChars[index].iSizeX;
				//	//}

				//	rows++;
				//	ypos += row_height;
				//	xpos = 0;
				//}

					rows++;
					(*m_ppRowOffsets)[rows] = offset;
					ypos += row_height;
					xpos = 0;
			}

			int x,y;
			x = rc.Left + xpos;
			y = m_iScreenHeight-rc.Top-m_pkFont->m_iRowHeight -
				ypos - iRenderDistFromTop;

			bool bInside = true;
			if(ypos+iRenderDistFromTop+m_pkFont->m_iRowHeight>rc.Height())
				bInside = false;
			if(rc.Top + ypos + iRenderDistFromTop < rc.Top)
				bInside = false;

			if(bInside)
				PrintWord(x, y, text, offset, kLength.first);

			bool bRowBreak = false;
			
			if(text[offset+kLength.first-1] == '\n')
			{	
				rows++;
				(*m_ppRowOffsets)[rows] = offset+kLength.first;
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
			y = m_iScreenHeight-rc.Top-m_pkFont->m_iRowHeight -
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
	int y = m_iScreenHeight-rc.Top-m_pkFont->m_iRowHeight;

	// Center text vertically
	if(rc.Height() > m_pkFont->m_iRowHeight)
	{
		y = m_iScreenHeight - rc.Top - (rc.Bottom-rc.Top)/2 - 
			m_pkFont->m_iRowHeight/2;
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

			//if(xpos >= max_width)
			//	break;
		}

		// Print cursor outside the loop if last character.
		if(iCursorPos == characters_totalt)
		{
			int x = rc.Left + xpos - iRenderDistFromLeft;

			if(x < rc.Left + 2)
				x = rc.Left + 2;
			
			//float prevcolor[3] = {
			//	m_afTextColor[0], m_afTextColor[1], m_afTextColor[2]
			//};

			//m_afTextColor[0]=m_afTextColor[1]=m_afTextColor[2]=0;

			PrintWord(x, y, "|", 0, 1);

			//m_afTextColor[0]=prevcolor[0];
			//m_afTextColor[1]=prevcolor[1];
			//m_afTextColor[2]=prevcolor[2];
		}

	glEnd();

	chars_printed = offset;

	return true;
}

void GLGuiRender::PrintWord(int x, int y, char *szWord, 
							int offset, int length)
{
	int i, fx, fy, fw, fh, pos, iCurrLegth;
	float tx, ty, tw, th;

	for(i=offset; i<offset+length; i++)
	{
		// Print cursor
		if(i == m_iCursorPos && !(m_iCursorPos == 0 && strlen(szWord) <= 1))
		{
			int iCursorX = x;
			int iCursorY = y;

			int index = '|';
			fx = m_pkFont->m_aChars[index].iPosX;
			fy = m_pkFont->m_aChars[index].iPosY;
			fw = m_pkFont->m_aChars[index].iSizeX;
			fh = m_pkFont->m_aChars[index].iSizeY;

			tx = (float) fx / m_pkFont->m_iTextureWidth;
			ty = (float) fy / m_pkFont->m_iTextureHeight;
			tw = (float) fw / m_pkFont->m_iTextureWidth;
			th = (float) fh / m_pkFont->m_iTextureHeight;

			iCursorX -= 2;	// minska markörens xpos ytterligare 2 pixlar.
								// som en kompensation för tecknets egen storlek.

//			glColor3f(0,0,0);

			glTexCoord2f(tx,1-ty);			glVertex2i(iCursorX,iCursorY+fh);		 
			glTexCoord2f(tx+tw,1-ty);		glVertex2i(iCursorX+fw,iCursorY+fh);    
			glTexCoord2f(tx+tw,1-ty-th);	glVertex2i(iCursorX+fw,iCursorY);    
			glTexCoord2f(tx,1-ty-th);		glVertex2i(iCursorX,iCursorY);
		}

		pos = szWord[i]; // m_pkFont->m_mapChars[ szWord[i] ];
		if(pos < 0 || pos > 255)
			continue;
		if(szWord[i] == '\n')
			return;

		// Do tags
		if(szWord[i] == '<'){
			m_bSearchForSytax = true;
			continue;
		} else if(szWord[i] == '>'){
			DoTextTag();
			continue;
		} else if(m_bSearchForSytax){
			m_strSyntax.push_back(szWord[i]);
			continue;
		}

		fx = m_pkFont->m_aChars[pos].iPosX;
		fy = m_pkFont->m_aChars[pos].iPosY;
		fw = m_pkFont->m_aChars[pos].iSizeX;
		fh = m_pkFont->m_aChars[pos].iSizeY;

		iCurrLegth = fw;

		if(m_rcTextBox.Inside(x, y) && szWord[i] != ' ')
		{
			if( !m_bClipperEnabled || (m_bClipperEnabled && 
				 x > m_rcClipperArea.Left && x+fw < m_rcClipperArea.Right &&
				 y < m_iScreenHeight-m_rcClipperArea.Top && 
				 y+fh > m_iScreenHeight-m_rcClipperArea.Bottom))
			{
				tx = (float) fx / m_pkFont->m_iTextureWidth;
				ty = (float) fy / m_pkFont->m_iTextureHeight;
				tw = (float) fw / m_pkFont->m_iTextureWidth;
				th = (float) fh / m_pkFont->m_iTextureHeight;

				glColor4f(m_afTextColor[0], m_afTextColor[1], m_afTextColor[2], 1);
				
				

				glTexCoord2f(tx,1-ty);			glVertex2i(x,y+fh);		 
				glTexCoord2f(tx+tw,1-ty);		glVertex2i(x+fw,y+fh);    
				glTexCoord2f(tx+tw,1-ty-th);	glVertex2i(x+fw,y);    
				glTexCoord2f(tx,1-ty-th);		glVertex2i(x,y);
			}
		}

		x+=iCurrLegth;
	}
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

		int index = text[i];
		if(index < 0 || index > 255)
		{
			char_counter++;
			continue;
		}

		// Do tags
		if(text[i] == '<'){
			m_bSearchForSytax = true;
		} else if(text[i] == '>'){
			m_bSearchForSytax = false;
		} 
		else
		if(m_bSearchForSytax == false)
		{
			if(text[i] != '\n')
				length_counter += m_pkFont->m_aChars[index].iSizeX;
		}

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


int GLGuiRender::StartDrawText(bool bCreateDisplayList)
{
	int iListID = 0;

	if(bCreateDisplayList)
	{
		iListID = glGenLists(1);
		glNewList(iListID,GL_COMPILE );
		m_bDrawList = true;
	}
	else
	{
		m_bDrawList = false;
	}

	glColor4f(0.0f,0.0f,0.0f,1);		
	glAlphaFunc(GL_GREATER,0.1f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);			
	glEnable(GL_ALPHA_TEST);

	return iListID;
}

void GLGuiRender::EndDrawText()
{
	if(m_bDrawList)
		glEndList();
}

void GLGuiRender::DrawStringDisplayList(int iDisplayListID)
{
	int apa[] = { iDisplayListID };
	glCallLists(1,GL_INT,apa);
}

void GLGuiRender::DeleteStringDisplayList(int iDisplayListID)
{
	glDeleteLists(iDisplayListID, 1);
}

//void GLGuiRender::DrawString(const char* text, const int length, int x, int y, 
//									  const float color[3], const ZGuiFont* pkFont)
//{		 		
//	m_pkTextureManger->BindTexture( pkFont->m_iTextureID );		
//	glEnable(GL_TEXTURE_2D);
//
//	int index;
//	GLfloat tx, ty, tw, th;
//
//	//const int CHARACTERS_TOTALT = strlen(text);
//	y = m_iScreenHeight-y-pkFont->m_iRowHeight;
//
//	glBegin(GL_QUADS);
//
//	glColor4f(color[0], color[1], color[2], 1);
//
//	for(int i=0; i<length; i++)
//	{		
//		index = text[i];
//		if(index < 0 || index > 255)
//			continue;
//	
//		if(text[i] != ' ' && text[i] != '\t')
//		{
//			tx = (float) pkFont->m_aChars[index].iPosX / pkFont->m_iTextureWidth;
//			ty = (float) pkFont->m_aChars[index].iPosY / pkFont->m_iTextureHeight;
//			tw = (float) pkFont->m_aChars[index].iSizeX / pkFont->m_iTextureWidth;
//			th = (float) pkFont->m_aChars[index].iSizeY / pkFont->m_iTextureHeight;
//
//			glTexCoord2f(tx,1.0f-ty);			glVertex2i(x, y + pkFont->m_aChars[index].iSizeY);		 
//			glTexCoord2f(tx+tw,1.0f-ty);		glVertex2i(x + pkFont->m_aChars[index].iSizeX, y + pkFont->m_aChars[index].iSizeY);    
//			glTexCoord2f(tx+tw,1.0f-ty-th);	glVertex2i(x + pkFont->m_aChars[index].iSizeX, y);    
//			glTexCoord2f(tx,1.0f-ty-th);		glVertex2i(x, y);
//		}
//
//		x += pkFont->m_aChars[index].iSizeX;
//	}
//
//	glEnd();
//}


void GLGuiRender::DrawString(const char* text, const int length, int x, int y, 
									  const float color[3], const ZGuiFont* pkFont)
{		 		
	m_pkTextureManger->BindTexture( pkFont->m_iTextureID );		
	glEnable(GL_TEXTURE_2D);

	int index;
	GLfloat tx, ty, tw, th;

	y = m_iScreenHeight-y-pkFont->m_iRowHeight;

	float temp = y;

	glBegin(GL_QUADS);

	glColor4f(color[0], color[1], color[2], 1);
	
	int clipp_side=0; // 0 = top, 1 = bottom
	Rect rcClipper = m_rcClipperArea;
	if(m_bClipperEnabled)
	{
		rcClipper.Top = -rcClipper.Top + m_iScreenHeight-pkFont->m_iRowHeight;
		rcClipper.Bottom = -rcClipper.Bottom + m_iScreenHeight;
		clipp_side = 0;

		if(m_rcClipperArea.Top == -1)
			clipp_side = 1;
	}

	for(int i=0; i<length; i++)
	{		
		index = text[i];
		if(index < 0 || index > 255)
			continue;
	
		if(text[i] != ' ' && text[i] != '\t')
		{
			if(m_bClipperEnabled)
			{	
				float fTexOffset=0, fQuadHeight=0;

				if(clipp_side == 0)
				{
					fQuadHeight = rcClipper.Top + pkFont->m_aChars[index].iSizeY - temp;				
					fTexOffset = (temp -(float) rcClipper.Top) / 
						(float) pkFont->m_aChars[index].iSizeY * ( 
						(float) pkFont->m_aChars[index].iSizeY / pkFont->m_iTextureHeight);
					y = int(temp);

					tx = (float) pkFont->m_aChars[index].iPosX / pkFont->m_iTextureWidth;
					ty = (float) pkFont->m_aChars[index].iPosY / pkFont->m_iTextureHeight + fTexOffset;
					tw = (float) pkFont->m_aChars[index].iSizeX / pkFont->m_iTextureWidth;
					th = (float) pkFont->m_aChars[index].iSizeY / pkFont->m_iTextureHeight - fTexOffset;

					if( fQuadHeight > 0)
					{
						glTexCoord2f(tx,1.0f-ty);			glVertex2i(x, y + fQuadHeight);		 
						glTexCoord2f(tx+tw,1.0f-ty);		glVertex2i(x + pkFont->m_aChars[index].iSizeX, y + fQuadHeight);    
						glTexCoord2f(tx+tw,1.0f-ty-th);	glVertex2i(x + pkFont->m_aChars[index].iSizeX, y);    
						glTexCoord2f(tx,1.0f-ty-th);		glVertex2i(x, y);
					}
				}
				else
				{
					tx = (float) pkFont->m_aChars[index].iPosX / pkFont->m_iTextureWidth;
					ty = (float) pkFont->m_aChars[index].iPosY / pkFont->m_iTextureHeight;
					tw = (float) pkFont->m_aChars[index].iSizeX / pkFont->m_iTextureWidth;
					th = (float) pkFont->m_aChars[index].iSizeY / pkFont->m_iTextureHeight;

					fQuadHeight = pkFont->m_aChars[index].iSizeY ;
					y = temp;

					if( y+fQuadHeight >= rcClipper.Bottom)
					{
						int height = float(y+fQuadHeight-rcClipper.Bottom);
						float procent = (float) height / (float) pkFont->m_iRowHeight;

						th = th*procent;
						fQuadHeight = fQuadHeight*procent;
						y += float(pkFont->m_iRowHeight-fQuadHeight);
					}

					glTexCoord2f(tx,1.0f-ty);			glVertex2i(x, y + fQuadHeight);		 
					glTexCoord2f(tx+tw,1.0f-ty);		glVertex2i(x + pkFont->m_aChars[index].iSizeX, y + fQuadHeight);    
					glTexCoord2f(tx+tw,1.0f-ty-th);	glVertex2i(x + pkFont->m_aChars[index].iSizeX, y);    
					glTexCoord2f(tx,1.0f-ty-th);		glVertex2i(x, y);	
				}
				
			}
			else
			{
				tx = (float) pkFont->m_aChars[index].iPosX / pkFont->m_iTextureWidth;
				ty = (float) pkFont->m_aChars[index].iPosY / pkFont->m_iTextureHeight;
				tw = (float) pkFont->m_aChars[index].iSizeX / pkFont->m_iTextureWidth;
				th = (float) pkFont->m_aChars[index].iSizeY / pkFont->m_iTextureHeight;

				glTexCoord2f(tx,1.0f-ty);			glVertex2i(x, y + pkFont->m_aChars[index].iSizeY);		 
				glTexCoord2f(tx+tw,1.0f-ty);		glVertex2i(x + pkFont->m_aChars[index].iSizeX, y + pkFont->m_aChars[index].iSizeY);    
				glTexCoord2f(tx+tw,1.0f-ty-th);	glVertex2i(x + pkFont->m_aChars[index].iSizeX, y);    
				glTexCoord2f(tx,1.0f-ty-th);		glVertex2i(x, y);
			}
		}

		x += pkFont->m_aChars[index].iSizeX;
	}

	glEnd();
}

//bool GLGuiRender::TextureIsTGA(int iID)
//{
//	if(iID == -1)
//		return false;
//
//	char* find;
//	const char* filename = m_pkTextureManger->GetFileName(iID);
//
//	find = strstr(filename, ".tga");
//
//	if(find == NULL)
//		return false;
//	else
//		return true;
//}







