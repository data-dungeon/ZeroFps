#ifndef _PROPERTYBOX_H
#define _PROPERTYBOX_H

#include "dialogbox.h"
#include <vector>
using namespace std;

class PropertyBox : public DlgBox  
{
public:
	bool IsOpen();
	void Update(ZGuiWnd* pkWnd);
	PropertyBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc);
	virtual ~PropertyBox();

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
				  int iNumberOfParams,void *pkParams );

	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);
	
private:
	bool IsResNameLegal(char* szResName);
	void UpdateUniquePropertyText(ZGuiWnd* pkControl, CtrlType eWndType);
	void CreateUniqueProperites();

	vector<pair<CtrlType, ZGuiWnd*> > m_kSpecCtrls;
};

#endif // #ifndef _PROPERTYBOX_H
