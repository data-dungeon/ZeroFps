// logbox.cpp: implementation of the LogBox class.
//
//////////////////////////////////////////////////////////////////////

#include "logbox.h"
#include "../../data/gui_resource_files/log_id.h"
#include "../common/logproperty.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogBox::LogBox(ZGui* pkGui, Input* pkInput, TextureManager* pkTexMan,
			   ZGuiWndProc oMainWndProc) : DlgBox(pkGui, pkInput, oMainWndProc)
{
	m_pkLogProperty = NULL;	
	m_pkTexMan = pkTexMan;
}

LogBox::~LogBox()
{

}

bool LogBox::OnOpen(int x, int y)
{
	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x,y,true,true);

	ZGuiWnd* pkTextbox = m_pkGuiMan->Wnd( string("LogBoxText") );
	if(pkTextbox == NULL)
		return false;

	((ZGuiTextbox*)pkTextbox)->SetCursorRow(0, true);

	return true;
}

bool LogBox::OnClose(bool bSave)
{
	return true;
}

bool LogBox::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	if(m_pkGui->LoadDialog(szResourceFile, szDlgName, m_oMainWndProc) == false)
		return false;

	m_pkDlgBox = m_pkGuiMan->Wnd(szDlgName);

	if(m_pkDlgBox == NULL)
		return false;

	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x,y,true,true);

	m_pkGui->ShowMainWindow(m_pkDlgBox, false);

	ZGuiWnd* pkTextbox = m_pkGuiMan->Wnd( string("LogBoxText") );
	if(pkTextbox == NULL)
		return false;

	ZGuiFont* pkFont = new ZGuiFont(16,16,0,20);
	pkFont->CreateFromFile("../data/textures/text/comic_sans_8.bmp");
	pkTextbox->SetFont(pkFont);

	((ZGuiTextbox*)pkTextbox)->SetReadOnly(true);

	return true;
}

bool LogBox::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage, 
					  int iNumberOfParams,void *pkParams )
{
	switch(uiMessage)
	{
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case LogBoxClose:
			Close(false);
			break;
		}
		break;
	}
	return true;
}

bool LogBox::SetLogProperty(LogProperty *lp)
{	
	if(lp == NULL)
		return false;

	m_pkLogProperty = lp;

	ZGuiWnd* pkTextbox = m_pkGuiMan->Wnd( string("LogBoxText") );
	if(pkTextbox == NULL)
		return false;

	pkTextbox->SetText((char*)m_pkLogProperty->m_sLog.c_str());

	if(lp->m_kLogIcon.length() > 0)
	{
		ZGuiWnd* pkIcon = m_pkGuiMan->Wnd( string("LogBoxPicture") );
		if(pkIcon == NULL)
			return false;

		ZGuiSkin* pkSkin = pkIcon->GetSkin();
		if(pkSkin == NULL)
			return false;

		char szTexpath[512];
		sprintf(szTexpath, "file:../data/textures/%s", lp->m_kLogIcon.c_str());
		pkSkin->m_iBkTexID = m_pkTexMan->Load(szTexpath,0);

		if(pkSkin->m_iBkTexID < 0)
		{
			printf("Failed to load logicon %s!\n", lp->m_kLogIcon.c_str());
		}
		else
		{
			pkIcon->SetSkin(pkSkin);
		}
	}

	return true;
}
