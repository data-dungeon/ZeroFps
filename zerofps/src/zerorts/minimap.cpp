// minimap.cpp: implementation of the MiniMap class.
//
//////////////////////////////////////////////////////////////////////

#include "../common/p_fogrender.h"
#include "../zerofps/render/texturemanager.h"
#include "../zerofps/engine/levelmanager.h"
#include "../zerofps/gui/zgui.h"
//#include "../common/p_clientunit.h"
#include "guibuilder.h"
#include "minimap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MiniMap::MiniMap(GuiBuilder* pkGuiBuilder, TextureManager *pkTexMan) : m_iDisclosuredCellRow(43)
{
	m_fCameraPosX = 0.5f;
	m_fCameraPosY = 0.5f;
	m_bVisible = true;
	m_pkGuiBuilder = pkGuiBuilder;
	m_pkTexMan = pkTexMan;
}

MiniMap::~MiniMap()
{

}	

void MiniMap::Draw(Camera *pkCamera, ZGui* pkGui, P_FogRender* pkFogRender, 
				   float fCurrTime)
{
	// minimap dimension
	int mmSize = m_iSize; 
	int mmLeft = m_kScreenPos.x;
	int mmTop  = m_kScreenPos.y;

	if(!m_bVisible)
		return;

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

	DrawUnits(fCurrTime, pkGui, mmLeft, mmTop, mmSize);

	int x, y;

	float center_x = mmLeft + mmSize/2;
	float center_y = mmTop + mmSize/2;

	const float TILE_SIZE = (float) mmSize / m_iDisclosuredCellRow;
	
	float dx=0,dy=0;
	for( y = 0; y < m_iDisclosuredCellRow; y++)
	{
		for( x = 0; x < m_iDisclosuredCellRow; x++)
		{
			float sx = 255.0f / m_iDisclosuredCellRow * x;
			float sy = 255.0f / m_iDisclosuredCellRow * y;

			if(pkFogRender->m_bExploredSqrs[(int)sy][(int)sx] == false)
				pkGui->DrawRect(mmLeft+dx, mmTop+dy, TILE_SIZE, TILE_SIZE, 0, 0, 0);

			dx += TILE_SIZE;
		}
		dy += TILE_SIZE;
		dx = 0;
	}

/*	//if(pkFogRender->m_bHaveChanged)
	{
		ZGuiSkin* pkSkin = m_pkGuiBuilder->Get("MiniMapWnd")->GetSkin();
		pkSkin->m_iBkTexID = m_pkTexMan->Load("../data/textures/fog.tga",T_NOMIPMAPPING);
		//pkSkin->m_iBkTexAlphaID = m_pkTexMan->Load("../data/textures/fog_a.bmp",0);
		m_pkGuiBuilder->Get("MiniMapWnd")->SetSkin(pkSkin);
	}*/
}

void MiniMap::Create(LevelManager *pkLevelMan, ObjectManager* pkObjMan)
{
	m_pkObjectMan = pkObjMan;

	const int TEXTURE_SIZE = 128; // not same as minimap window size...

	m_pkTexMan->BindTexture("../data/textures/minimap.bmp", T_NOMIPMAPPING);
	
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
				b = 255;
			else
				g = 255;

			m_pkTexMan->PsetRGB(x,y,r,g,b);
		}
	}

	m_pkTexMan->SwapTexture();

	m_pkGuiBuilder->GetSkin("minimap")->m_iBkTexID = 
		m_pkTexMan->Load("../data/textures/minimap.bmp", T_NOMIPMAPPING);

	Rect rc = m_pkGuiBuilder->Get("MiniMapWnd")->GetScreenRect();
	m_kScreenPos.x = rc.Left;
	m_kScreenPos.y = rc.Top;
	m_iSize = rc.Width();
}

void MiniMap::MoveFov(float fCameraPosX, float fCameraPosY)
{
	static int counter = 0;

	float length = /*362*/512, hojd = /*417*/512;
	float procent_av_bredd = (float) (fCameraPosX + /*181*/256) / length;
	float procent_av_hojd = (float) (fCameraPosY + /*182*/256) / hojd;

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



void MiniMap::Show(bool bVisible)
{
	m_bVisible = bVisible;

	if(m_bVisible)
		m_pkGuiBuilder->Get("MiniMapWnd")->Show();
	else
		m_pkGuiBuilder->Get("MiniMapWnd")->Hide();
}

void MiniMap::DrawUnits(float fTime, ZGui* pkGui, float wnd_x, float wnd_y, float wnd_sz)
{
	static float s_fPrevTime = -1;

/*	// Är det dags att uppdatera grafiken?
	if(fTime - s_fPrevTime < 1.00f)
		return;*/

	vector<Object*> akAllObjs;
	m_pkObjectMan->GetAllObjects(&akAllObjs);

	float map_size  = 256;
	float map_size_d2 = map_size*2;

	const int antal = akAllObjs.size();
	for(int i=0; i<antal; i++)
	{
		if(akAllObjs[i]->GetProperty("P_ClientUnit"))
		{
			Vector3 kObjPos = akAllObjs[i]->GetPos();

			Point p;
			float procent_av_bredd = (float) (kObjPos.x + map_size) / map_size_d2;
			float procent_av_hojd = (float) (kObjPos.z + map_size) / map_size_d2;

			p.x = wnd_x  + procent_av_bredd*wnd_sz;
			p.y = wnd_y  + procent_av_hojd*wnd_sz;

			pkGui->DrawPoint(p, 255, 128, 0); p.x += 1;
			pkGui->DrawPoint(p, 255, 128, 0); p.y += 1;
			pkGui->DrawPoint(p, 255, 128, 0); p.x -= 1;
			pkGui->DrawPoint(p, 255, 128, 0); 
		}
	}


	s_fPrevTime = fTime;

/*	static bool init = false;
	static Point array[800];

	if(!init)
	{
		Point p;
		
		int k=0;

		int s[] = { 25,25,  wnd_sz-20-25,15,  wnd_sz-30,wnd_sz-30,  11,wnd_sz-40 };

		for(int i=0; i<4; i++)
		{
			p = Point(wnd_x+s[i*2], wnd_y+s[i*2+1]);

			for(int j=0; j<200; j++)
				array[k++] = Point(p.x+rand()%30, p.y+rand()%30);
		}

		init = true;
	}

	int i;
	for(i=0; i<10; i++)
	{
		Point p = array[i];
		pkGui->DrawPoint(p, 255, 128, 0); p.x += 1;
		pkGui->DrawPoint(p, 255, 128, 0); p.y += 1;
		pkGui->DrawPoint(p, 255, 128, 0); p.x -= 1;
		pkGui->DrawPoint(p, 255, 128, 0); 
	}

	for( i=200; i<400; i++)
	{
		Point p = array[i];
		pkGui->DrawPoint(p, 0, 0, 128); p.x += 1;
		pkGui->DrawPoint(p, 0, 0, 128); p.y += 1;
		pkGui->DrawPoint(p, 0, 0, 128); p.x -= 1;
		pkGui->DrawPoint(p, 0, 0, 128); 
	}

	for( i=400; i<600; i++)
		pkGui->DrawPoint(array[i], 0, 44, 128);

	for( i=600; i<800; i++)
		pkGui->DrawPoint(array[i], 243, 213, 2);*/
}
