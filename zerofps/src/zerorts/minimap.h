// minimap.h: interface for the MiniMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINIMAP_H__07AFEE97_DF3C_4279_A1BE_E29D50380DB4__INCLUDED_)
#define AFX_MINIMAP_H__07AFEE97_DF3C_4279_A1BE_E29D50380DB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MiniMap  
{
public:
	
	void Show(bool bVisible);
	void Create(LevelManager* pkLevelMan, ObjectManager* pkObjMan);
	void Draw(Camera* pkCamera, ZGui* pkGui, P_FogRender* pkFogRender, float fCurrTime);
	MiniMap(GuiBuilder* pkGuiBuilder, TextureManager *pkTexMan);
	virtual ~MiniMap();

private:

	void DrawUnits(float fTime, ZGui* pkGui, float wnd_x, float wnd_y, float wnd_sz);
	
	const int m_iDisclosuredCellRow;
	Point m_kScreenPos;
	int m_iSize;

	void MoveFov(float fCameraPosX, float fCameraPosY);
	float m_fCameraPosX;
	float m_fCameraPosY;
	bool m_bVisible;

	GuiBuilder* m_pkGuiBuilder;
	TextureManager *m_pkTexMan;
	ObjectManager  *m_pkObjectMan;
};

#endif // !defined(AFX_MINIMAP_H__07AFEE97_DF3C_4279_A1BE_E29D50380DB4__INCLUDED_)
