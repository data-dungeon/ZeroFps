// zguilistitem.h: interface for the ZGuiListitem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUILISTITEM_H__375F63D5_876F_46C7_AFDA_9EED11404346__INCLUDED_)
#define AFX_ZGUILISTITEM_H__375F63D5_876F_46C7_AFDA_9EED11404346__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\engine_x.h"

class ZGuiButton;
class ZGuiSkin;
class ZGuiWnd;

class ENGINE_API ZGuiListitem  
{
public:
	void SetText(char* strText);
	ZGuiListitem(ZGuiWnd* pkParent,char* strText=0, unsigned int iID=0, 
		ZGuiSkin* pkSkin=0, ZGuiSkin* pkSelected=0, 
		ZGuiSkin* pkHighLigtSkin=0, ZGuiSkin* pkTextSkin=0,
		int iFontMaskTexture=-1);
	virtual ~ZGuiListitem();

	char* GetText();
	void Move(int dx, int dy);
	void Select();
	void Deselect();
	void SetPos(int x, int y);
	void Resize(int w, int h);
	unsigned int GetID();
	
private:
	unsigned int m_iID;
	ZGuiButton* m_pkButton;	
};

#endif // !defined(AFX_ZGUILISTITEM_H__375F63D5_876F_46C7_AFDA_9EED11404346__INCLUDED_)
