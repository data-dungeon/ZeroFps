// zguislider.h: interface for the ZGuiProgressbar class.

#ifndef _GUI_ZGUIPROGRESSBAR_H
#define _GUI_ZGUIPROGRESSBAR_H

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiLabel;
class ZGuiSkin;

enum ProgressbarDir
{
	PBDIR_LEFT_TO_RIGHT	= 0,
	PBDIR_RIGHT_TO_LEFT	= 1,
	PBDIR_TOP_TO_BOTTOM	= 2,
	PBDIR_BOTTOM_TO_TOP	= 3
};

enum ProgressbarTextOrientation
{
	PBTEXTORIENT_CENTER	= 0,
	PBTEXTORIENT_LEFT		= 1,
	PBTEXTORIENT_OVER		= 2,
	PBTEXTORIENT_RIGHT	= 3,
	PBTEXTORIENT_UNDER	= 4
};

/** \brief	A Progressbar control (monitors an operation's progress toward completion).
	 \ingroup Gui
*/
class GUI_API ZGuiProgressbar : public ZGuiWnd
{
public:
	ZGuiProgressbar(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID, int iMin=0, int iMax=100, 
		ProgressbarDir eDir=PBDIR_LEFT_TO_RIGHT, ProgressbarTextOrientation eTextOrientation=PBTEXTORIENT_CENTER);
	virtual ~ZGuiProgressbar();
	
	int GetPos();
	void SetPos(int iPos);
	void GetRange(int& riMin, int& riMax);
	void SetRange(int iMin, int iMax);

	void SetDir(ProgressbarDir eDir);
	ProgressbarDir GetDir();

	void SetTextOrientation(ProgressbarTextOrientation eOrient);
	ProgressbarTextOrientation GetTextOrientation();
	
	void SetSkin(ZGuiSkin* pkFront, ZGuiSkin* pkBack);
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	bool Render( ZGuiRender* pkRenderer ); // overloaded
	void Resize(int iWidth, int iHeight, bool bChangeMoveArea); // overloaded

	bool m_bShowText;
	
private:
	void CreateInternalControls();

	ZGuiLabel* m_pkLabelFront;
	ZGuiLabel* m_pkLabelBkgrd;

	int m_iMax, m_iMin, m_iPos;

	ProgressbarDir m_eDir;
	ProgressbarTextOrientation m_eTextOrient;
};

#endif // #ifndef _GUI_ZGUIPROGRESSBAR_H
