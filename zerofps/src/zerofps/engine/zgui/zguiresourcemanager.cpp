// zguiresourcemanager.cpp: implementation of the ZGuiResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiresourcemanager.h"
#include "../../basic/zguiskin.h"
#include "../../basic/zguifont.h"
#include "../../basic/zfassert.h"
#include "zguiwindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiResourceManager::ZGuiResourceManager() : ZFObject("ZGuiResourceManager")
{

}

ZGuiResourceManager::~ZGuiResourceManager()
{

}

ZGuiWnd* ZGuiResourceManager::Wnd(string szName)
{
	map<string, ZGuiWnd*>::iterator it;
	it = m_kWindows.find(szName);
	if(it != m_kWindows.end())
		return it->second;
	return NULL;
}

ZGuiSkin* ZGuiResourceManager::Skin(string szName)
{
	map<string, ZGuiSkin*>::iterator it;
	it = m_kSkins.find(szName);
	if(it != m_kSkins.end())
		return it->second;
	return NULL;
}

ZGuiFont* ZGuiResourceManager::Font(string szName)
{
	map<string, ZGuiFont*>::iterator it;
	it = m_kFonts.find(szName);
	if(it != m_kFonts.end())
		return it->second;
	return NULL;
}

ZGuiWnd* ZGuiResourceManager::Add(string szName, ZGuiWnd* pkNewWnd)
{
	ZGuiWnd* pkExisting = Wnd(szName);
	if( pkExisting == NULL) 
	{
		m_kWindows.insert( map<string, ZGuiWnd*>::value_type(szName, pkNewWnd));
		return Wnd(szName);
	}

	return pkExisting;
}

ZGuiSkin* ZGuiResourceManager::Add(string szName, ZGuiSkin* pkNewSkin)
{
	ZGuiSkin* pkExisting = Skin(szName);
	if( pkExisting == NULL) 
	{
		m_kSkins.insert( map<string, ZGuiSkin*>::value_type(szName, pkNewSkin));
		return Skin(szName);
	}

	return pkExisting;
}

ZGuiFont* ZGuiResourceManager::Add(string szName, ZGuiFont* pkNewFont)
{
	ZGuiFont* pkExisting = Font(szName);
	if( pkExisting == NULL) 
	{
		m_kFonts.insert( map<string, ZGuiFont*>::value_type(szName, pkNewFont));
		return Font(szName);
	}

	return pkExisting;
}














