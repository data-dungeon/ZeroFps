// zguilistitem.h: interface for the ZGuiListitem class.

#ifndef _GUI_ZGUILISTITEM_H_
#define _GUI_ZGUILISTITEM_H_

#include "gui_x.h"

class ZGuiButton;
class ZGuiSkin;
class ZGuiWnd;

/** \brief	???.
	 \ingroup Gui
*/
class GUI_API ZGuiListitem  
{
public:
	void SetText(char* strText);
	ZGuiListitem(ZGuiWnd* pkParent,char* strText=0, unsigned int iIndex=0, 
		ZGuiSkin* pkSkin=0, ZGuiSkin* pkSelected=0, ZGuiSkin* pkHighLigtSkin=0);
	virtual ~ZGuiListitem();

	char* GetText();
	void Move(int dx, int dy);
	void Select();
	void Deselect();
	void SetPos(int x, int y);
	void Resize(int w, int h);
	ZGuiButton* GetButton() { return m_pkButton; }
	unsigned int GetIndex();
	void SetIndex(unsigned int iIndex);

	bool operator < (const ZGuiListitem &I)
	{         
	   if(m_iIndex > I.m_iIndex)
		   return false;
	   else
		   return true;
	}

private:
	unsigned int m_iIndex;
	ZGuiButton* m_pkButton;	
};

#endif 


