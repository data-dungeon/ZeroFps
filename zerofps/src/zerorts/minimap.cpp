// minimap.cpp: implementation of the MiniMap class.
//
//////////////////////////////////////////////////////////////////////

#include "../zerofps/render/texturemanager.h"
#include "../zerofps/engine/levelmanager.h"
#include "../zerofps/gui/zgui.h"
#include "guibuilder.h"
#include "minimap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MiniMap::MiniMap()
{
	m_fCameraPosX = 0.5f;
	m_fCameraPosY = 0.5f;
}

MiniMap::~MiniMap()
{

}

void MiniMap::Draw(Camera *pkCamera, ZGui* pkGui)
{
	// minimap dimension
	int mmSize = m_iSize; 
	int mmLeft = m_kScreenPos.x;
	int mmTop  = m_kScreenPos.y;

	MoveFov(pkCamera->GetPos().x, pkCamera->GetPos().z);

	// camera rect dimension
	int fvWidth  = (float)mmSize/4;
	int fvHeight = (float)fvWidth * 0.75f;
	int fvLeft = mmLeft + m_fCameraPosX*mmSize - fvWidth/2;
	int fvTop  = mmTop  + m_fCameraPosY*mmSize - fvHeight/2;

	if(fvLeft < mmLeft)
		fvLeft = mmLeft;
	if(fvLeft+fvWidth > mmLeft+mmSize)
		fvLeft = mmLeft+mmSize-fvWidth;

	if(fvTop < mmTop)
		fvTop = mmTop;
	if(fvTop+fvHeight > mmTop+mmSize)
		fvTop = mmTop+mmSize-fvHeight;
	
	pkGui->DrawLine(Point(fvLeft, fvTop), Point(fvLeft, fvTop+fvHeight)); // left
	pkGui->DrawLine(Point(fvLeft, fvTop), Point(fvLeft+fvWidth, fvTop));    // top
	pkGui->DrawLine(Point(fvLeft+fvWidth, fvTop), Point(fvLeft+fvWidth, fvTop+fvHeight)); // right
	pkGui->DrawLine(Point(fvLeft, fvTop+fvHeight), Point(fvLeft+fvWidth, fvTop+fvHeight)); 
}

void MiniMap::Create(TextureManager *pkTexMan, LevelManager *pkLevelMan,
					 GuiBuilder* pkGuiBuilder)
{
	pkTexMan->BindTexture("../data/textures/minimap.bmp", T_NOMIPMAPPING);
	
	HeightMap* hm = pkLevelMan->GetHeightMap();

	int size = hm->m_iHmSize, x,y,r,g,b;

	float scale = size / 128.0f, fx, fy, min=100000000, max=-100000000;

	for( y=0; y<128; y++)
	{
		fy = scale*(float)y;
		for( x=0; x<128; x++)
		{
			fx = scale*(float)x;
			HM_vert* pkVert = hm->GetVert(fx,fy);
			if(pkVert->height < min)
				min = pkVert->height;
			if(pkVert->height > max)
				max = pkVert->height;
		}
	}

	float length = (max - min);

	for( y=0; y<128; y++)
	{
		fy = scale*(float)y;
		for( x=0; x<128; x++)
		{
			fx = scale*(float)x;
			float height = hm->GetVert(fx,fy)->height;
			float procent = (1.0f / (length)) + ((height-min) / length);
			
			int form_height = (int) (procent*255);
			
			if(form_height >= 0 && form_height <= 255)
				r = g = b = form_height;
			else
			{
				r = 255 ; g = 0; b = 255;
			}

			if(height < epsilon)
			{
				b = 255;
			}
			else
			{
				g = 255;
			}

			pkTexMan->PsetRGB(x,y,r,g,b);
		}
	}

	pkTexMan->SwapTexture();

	pkGuiBuilder->GetSkin("minimap")->m_iBkTexID = 
		pkTexMan->Load("../data/textures/minimap.bmp", T_NOMIPMAPPING);

	Rect rc = pkGuiBuilder->Get("MinMapWnd")->GetScreenRect();
	m_kScreenPos.x = rc.Left;
	m_kScreenPos.y = rc.Top;
	m_iSize = rc.Width();
}

void MiniMap::MoveFov(float fCameraPosX, float fCameraPosY)
{
	static int counter = 0;

	float length = 362, hojd = 417;
	float procent_av_bredd = (float) (fCameraPosX + 181) / length;
	float procent_av_hojd = (float) (fCameraPosY + 182) / hojd;

	if(counter > 23)
	{
		printf("fCameraPosX = %f, fCameraPosY = %f\nprocent_av_bredd = %f\n" \
			"procent_av_hojd = %f\n", fCameraPosX, fCameraPosY, procent_av_bredd,
			 procent_av_hojd);

		counter = 0;
	}

	//counter++;

	m_fCameraPosX = procent_av_bredd;
	m_fCameraPosY = procent_av_hojd;

	// -181 till 181 (x) = 362
	// -182 till 236 (y) = 417
}
