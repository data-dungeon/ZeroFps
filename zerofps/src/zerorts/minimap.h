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
	void Create(TextureManager* pkTexMan, LevelManager* pkLevelMan, GuiBuilder* pkGuiBuilder);
	void Draw(Camera* pkCamera, ZGui* pkGui);
	MiniMap();
	virtual ~MiniMap();

private:
	Point m_kScreenPos;
	int m_iSize;

	void MoveFov(float fCameraPosX, float fCameraPosY);
	float m_fCameraPosX;
	float m_fCameraPosY;
};

#endif // !defined(AFX_MINIMAP_H__07AFEE97_DF3C_4279_A1BE_E29D50380DB4__INCLUDED_)
