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

MiniMap::MiniMap() : m_iDisclosuredCellRow(43)
{
	m_pbDisclosuredCells = new bool[m_iDisclosuredCellRow*m_iDisclosuredCellRow];
	memset(m_pbDisclosuredCells, 0, 
		sizeof(bool)*(m_iDisclosuredCellRow*m_iDisclosuredCellRow));
	m_fCameraPosX = 0.5f;
	m_fCameraPosY = 0.5f;
	bDraw = true;
}

MiniMap::~MiniMap()
{
	delete[] m_pbDisclosuredCells;
}	

void MiniMap::Draw(Camera *pkCamera, ZGui* pkGui)
{
	if(!bDraw)
		return;

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

	Rect rcCamera(fvLeft, fvTop, fvLeft+fvWidth, fvTop+fvHeight);
	DiscloseCells(rcCamera);

	static bool init = false;
	static Point array[800];

	if(!init)
	{
		Point p;
		
		int k=0;

		int s[] = { 25,25,  mmSize-20-25,15,  mmSize-30,mmSize-30,  11,mmSize-40 };

		for(int i=0; i<4; i++)
		{
			p = Point(mmLeft+s[i*2], mmTop+s[i*2+1]);

			for(int j=0; j<200; j++)
				array[k++] = Point(p.x+rand()%30, p.y+rand()%20);
		}

		init = true;
	}

	for(int i=0; i<200; i++)
		pkGui->DrawPoint(array[i], 255, 128, 0);

	for( i=200; i<400; i++)
		pkGui->DrawPoint(array[i], 0, 0, 128);

	for( i=400; i<600; i++)
		pkGui->DrawPoint(array[i], 0, 44, 128);

	for( i=600; i<800; i++)
		pkGui->DrawPoint(array[i], 243, 213, 2);

	int x, y;

	float center_x = mmLeft + mmSize/2;
	float center_y = mmTop + mmSize/2;

	const float TILE_SIZE = (float) mmSize / m_iDisclosuredCellRow;
	
	float dx=0,dy=0;
	for( y = 0; y < m_iDisclosuredCellRow; y++)
	{
		for( x = 0; x < m_iDisclosuredCellRow; x++)
		{
			if(m_pbDisclosuredCells[y*m_iDisclosuredCellRow+x] == false)
				pkGui->DrawRect(mmLeft+dx, mmTop+dy, TILE_SIZE, TILE_SIZE, 0, 0, 0);

			dx += TILE_SIZE;
		}
		dy += TILE_SIZE;
		dx = 0;
	}

}

void MiniMap::Create(TextureManager *pkTexMan, LevelManager *pkLevelMan,
					 GuiBuilder* pkGuiBuilder)
{
	const int TEXTURE_SIZE = 128; // not same as minimap window size...

	pkTexMan->BindTexture("../data/textures/minimap.bmp", T_NOMIPMAPPING);
	
	HeightMap* hm = pkLevelMan->GetHeightMap();

	int size = hm->m_iHmSize, x,y,r,g,b;

	float scale = (float) size / TEXTURE_SIZE, fx, fy, min=100000000, max=-100000000;

	for( y=0; y<TEXTURE_SIZE; y++)
	{
		fy = scale*(float)y;
		for( x=0; x<TEXTURE_SIZE; x++)
		{
			fx = scale*(float)x;
			HM_vert* pkVert = hm->GetVert(fx,fy);
			if(pkVert->height < min)
				min = pkVert->height;
			if(pkVert->height > max)
				max = pkVert->height;
		}
	}

	float max_height = (max - min);

	for( y=0; y<TEXTURE_SIZE; y++)
	{
		fy = scale*(float)y;
		for( x=0; x<TEXTURE_SIZE; x++)
		{
			fx = scale*(float)x;
			float height = hm->GetVert(fx,fy)->height;
			float procent = (1.0f / (max_height)) + ((height-min) / max_height);
			
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

	m_fCameraPosX = procent_av_bredd;
	m_fCameraPosY = procent_av_hojd;

	// -181 till 181 (x) = 362
	// -182 till 236 (y) = 417
}

void MiniMap::DiscloseCells(Rect rcCamera)
{
	int mmSize = m_iSize; 
	int mmLeft = m_kScreenPos.x;
	int mmTop  = m_kScreenPos.y;

	const float TILE_SIZE = (float) mmSize / m_iDisclosuredCellRow;

	int x,y;
	float dx=0,dy=0;
	for( y = 0; y < m_iDisclosuredCellRow; y++)
	{
		for( x = 0; x < m_iDisclosuredCellRow; x++)
		{
			Rect rcCell(mmLeft+dx, mmTop+dy, 
				mmLeft+dx+TILE_SIZE, mmTop+dy+TILE_SIZE);

			if(rcCamera.Inside(rcCell))
				m_pbDisclosuredCells[y*m_iDisclosuredCellRow+x] = true;

			dx += TILE_SIZE;
		}
		dy += TILE_SIZE;
		dx = 0;
	}	
}
