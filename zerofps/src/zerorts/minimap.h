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
	void Create(/*TextureManager* pkTexMan,*/ LevelManager* pkLevelMan);
	void Draw(Camera* pkCamera, ZGui* pkGui, P_FogRender* pkFogRender, Render* pkRender);
	MiniMap(GuiBuilder* pkGuiBuilder, TextureManager *pkTexMan);
	virtual ~MiniMap();

private:
	
	const int m_iDisclosuredCellRow;
	Point m_kScreenPos;
	int m_iSize;

	void MoveFov(float fCameraPosX, float fCameraPosY);
	float m_fCameraPosX;
	float m_fCameraPosY;
	bool m_bVisible;

	GuiBuilder* m_pkGuiBuilder;
	TextureManager *m_pkTexMan;
};

#endif // !defined(AFX_MINIMAP_H__07AFEE97_DF3C_4279_A1BE_E29D50380DB4__INCLUDED_)
